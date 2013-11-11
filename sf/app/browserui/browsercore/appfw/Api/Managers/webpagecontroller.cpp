/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <assert.h>

#include "HistoryManager.h"
#include "actionjsobject.h"
#include "webpagedata.h"
#include "wrtbrowsercontainer.h"
#include "wrtbrowsercontainer_p.h"
#include "webpagecontroller_p.h"
#include "webpagecontroller.h"
#include "UiUtil.h"
#include "bedrockprovisioning.h"
#include "secureuicontroller.h"
#include "LoadController.h"
#include "ContentAgent.h"
#include "lowmemoryhandler.h"
#include <QWebFrame>
#include <QWebHistory>
#include <QGraphicsWebView>
#include <QFile>
#include <QDir>
#include <QtGui>
#include <QCoreApplication>
#include <QWebSecurityOrigin>
#include <QWebDatabase>
#include <QWebSettings>
#include <QtCore/QSettings>
using namespace WRT;

#ifdef ORBIT_UI
#include <shareui.h>
#include <qcontactemailaddress.h>
#include <xqaiwdeclplat.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#endif // ORBIT_UI


#if defined(Q_OS_SYMBIAN) // for shareToMail
#include <miutset.h>                
#include <sendui.h>      
#include <cmessagedata.h> 
#include <txtrich.h>   
#include <SendUiConsts.h>
const TUid KShareOnline = { 0x200009D5 }; 
const TUid KShareOnOvi = { 0x2001AA43 }; 
#endif 
#ifdef ORBIT_UI
static const QString EMAIL_SEND_TO_KEY = "to";
#endif // ORBIT_UI

#define MAX_NUM_WINDOWS_TO_RESTORE 5
#define MAX_NUM_WINDOWS 5

static const char KHISTORYEXTENSION[]       = ".history";
static const char KCOOKIESEXTENSION[]       = ".ini";


WebPageControllerPrivate::WebPageControllerPrivate(WebPageController* qq) :
    q(qq),
    m_widgetParent(0),
    m_currentPage(-1),
    m_secContext(0),
    m_actionsParent(0),
    donotsaveFlag(false)
{
    m_widgetParent = static_cast<QObject*>(qq); //new QWidget();

    // create the manager's actions
    m_actionReload = new QAction("Reload",m_widgetParent);
    m_actionReload->setObjectName("reload");
    m_actionStop = new QAction("Stop",m_widgetParent);
    m_actionStop->setObjectName("stop");
    m_actionBack = new QAction("Back",m_widgetParent);
    m_actionBack->setObjectName("back");
    m_actionForward = new QAction("Forward",m_widgetParent);
    m_actionForward->setObjectName("forward");

    m_actionReload->setEnabled(false);
    m_actionStop->setEnabled(false);
    m_actionBack->setEnabled(false);
    m_actionForward->setEnabled(false);
    
}

WebPageControllerPrivate::~WebPageControllerPrivate()
{
    // save history in the persistent storage, delete history files first
   
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");
    if (enabled)
    {
      q->deleteHistory();
      if(donotsaveFlag == false)
      {	
        int windowsSaved = 0;
        int activeWindowId = 0;
    	  q->saveHistory(&windowsSaved, &activeWindowId);
    	  q->saveNumberOfWindows(windowsSaved);    
        q->saveActiveWindowId(activeWindowId);
  	  }
  	  donotsaveFlag = true;
    }
    else
    {
    	q->deleteDataFiles();
    }

    // clean up all pages
    while ( !m_allPages.isEmpty() )
       q->closePage(m_allPages.at(0));

    Q_ASSERT ( m_allPages.isEmpty() );

//    delete(m_widgetParent);
}



/*!
 * \class WebPageController
 *
 * \brief Manages WrtPages
 *
 * This class is responsible for managing multiple WrtPages.
 * All page management operations go through this class, such as opening a page,
 * managing multiple pages, choosing what the current page among several is,
 * closing a page, etc.
 *
 * Multiple views may well have access to a WebPageManager
 *
 * For most cases, we expect there to be a single instance of WebPageController for a given
 * application, but it is up to the application to specify the instance used.
 *
 * This class is supposed to abstract page events, control into a simple interface
 * so that higher-level functions need not know or care about what the active page is, etc.
 */

WebPageController::WebPageController ( QObject* parent ) :
    QObject ( parent ),
    m_networkError (false),
    m_networkErrorMsg("No network error"),           
    m_networkErrorUrl("No Url"),
    m_bErrorUrlMatches(false),
    m_promptMsg("No message"),           
    m_promptReserved(""),
    m_memoryHandler(new LowMemoryHandler(this)),
    d(new WebPageControllerPrivate(this)),
    m_settingsLoaded(0)
{
    // Register a new MetaType WebPageData. It is needed to serialize history (starage)
    qRegisterMetaTypeStreamOperators<WebPageData> ("WebPageData");

    d->m_historyDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("DataBaseDirectory");

    // auto-connect actions
    connect( d->m_actionReload, SIGNAL( triggered() ), this, SLOT( currentReload() ) );
    connect( d->m_actionStop, SIGNAL( triggered() ), this, SLOT( currentStop() ) );
    connect( d->m_actionBack, SIGNAL( triggered() ), this, SLOT( currentBack() ) );
    connect( d->m_actionForward, SIGNAL( triggered() ), this, SLOT( currentForward() ) );

    connect( m_memoryHandler, SIGNAL( lowMemory() ), this, SIGNAL( lowMemory() ) );
    connect( m_memoryHandler, SIGNAL( lowMemory() ), this, SLOT( handleLowMemory() ) );
    connect( m_memoryHandler, SIGNAL( outOfMemory() ), this, SIGNAL( outOfMemory() ) );
    connect( m_memoryHandler, SIGNAL( outOfMemory() ), this, SLOT( handleOutOfMemory() ) );
    m_memoryHandler->start();

    updateJSActions();
    
    m_bRestoreSession = false;
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");
    if (enabled) m_bRestoreSession = true;

}
    
WebPageController::~WebPageController()
{
    m_memoryHandler->stop();
    delete d;
}



/*!
 * Open an empty WRT::WrtBrowserContainer
 * @param parent  Widget Parent
 * @return        WRT::WrtBrowserContainer Handle
 * @see WRT::WrtBrowserContainer                      
 */
WRT::WrtBrowserContainer* WebPageController::openPage(QObject* parent, WRT::WrtBrowserContainer* pg)
{
    WRT::WrtBrowserContainer* page =  currentPage();
    if (  d->m_allPages.count() < MAX_NUM_WINDOWS ) {


        // create without parent
        page = WRT::WrtBrowserContainer::createPageWithWidgetParent(parent, pg);

        // emit signal for creating network connection.

        Q_ASSERT( page );
        page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("DeveloperExtras"));
        connect( page, SIGNAL( loadFinished(bool) ), SLOT( onLoadFinishedForBackgroundWindow(bool) ) );

        // emit signal indicating that new page is being created
        emit creatingPage( page );

        /* Add the new page after the current page */
        WRT::WrtBrowserContainer * theCurrentPage = currentPage();
        int index = d->m_allPages.indexOf(theCurrentPage);
        d->m_allPages.insert (index+1, page );

  // set the max number of pages to hold in the memory page cache to pages
  // The Page Cache allows for a nicer user experience when navigating forth or back to pages in 
  // the forward/back history, by pausing and resuming up to pages per page group

		int maxPagesInCache = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("MaxPagesInCache");
        page->settings()->setMaximumPagesInCache(maxPagesInCache);

        // emit signal
        emit pageCreated( page );

        // AUTO-activate this page ONLY if there are no other pages
        if(d->m_allPages.size() == 1) {
            setCurrentPage(page);
        }
        int count = historyWindowCount();
        int activeWinId = activeWindowId();

        if (m_bRestoreSession)
        	startupRestoreHistory(NULL, index+1, page);
    	  
    }
    return page;
}


WRT::WrtBrowserContainer* WebPageController::openPage()
{
	WRT::WrtBrowserContainer* page = NULL;

	page = openPage(this, 0);
	page->setPageFactory(this);
	
	return page;
}

WRT::WrtBrowserContainer* WebPageController::openPageFromHistory(int index)
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    indexStr.setNum(index);
    
    
    QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
    QFile file(historyFile);    
    if(file.open(QIODevice::ReadOnly)) 
    {
    	  if(file.size() <= 12) // empty file
    	  {	
        	file.remove();
        	file.close();
        	return NULL;
        }
        else
        	return openPage();
    }    
    else // can not open file
    	return NULL;   
}

/*!
 * Given a WRT::WrtBrowserContainer instance, close it
 * @param page   WRT::WrtBrowserContainer instance to be closed 
 * @see  WRT::WrtBrowserContainer
 */
void WebPageController::closePage ( WRT::WrtBrowserContainer *page )
{
        WRT::WrtBrowserContainer * theCurrentPage = currentPage();
        bool updateCurrentPageIndex = false;

        // get index of page we want to remove
        int closeIndex = d->m_allPages.indexOf(page);
        if(closeIndex < 0)
            return;
    
        // was this page the "current page?"
        // if so, we need to select the page to become the new one
        // select the previous page unless at the beginning, then select next
        if(page == theCurrentPage) {
            int newCurrIndex = closeIndex - 1;
            if(closeIndex == 0 && pageCount() > 1) {
                newCurrIndex = closeIndex + 1;
            }
        
            // change the current page
            if(newCurrIndex >= 0) {
                theCurrentPage = d->m_allPages.at(newCurrIndex);
                setCurrentPage(theCurrentPage);
                updateCurrentPageIndex = true;
            }
            else {
                d->m_currentPage = -1;
            }
        }
        else {
             /* Adjust m_currentPage if the index of the page deleted is less than 
             * current page 
             */
           if (closeIndex < d->m_currentPage ) 
               updateCurrentPageIndex = true;
        }

        // actually delete the page from the list
        d->m_allPages.removeAt(closeIndex);

        // update the current page index if necessary
        // (this will just update the index now that we've removed the page from the list)
        if(updateCurrentPageIndex) {
            int index = d->m_allPages.indexOf(theCurrentPage);
            if(index >= 0)
                d->m_currentPage = index;
        }

        // and emit sig that it was done
        emit pageDeleted(page);

        delete page;
}

/*!
 *  Activate the given page as the current page
 *  @param page   page handle for setting current page
 *  @see  WRT::WrtBrowserContainer
 */
void WebPageController::setCurrentPage(WRT::WrtBrowserContainer* page)
{
    // verify page
    Q_ASSERT(page);
    int index = d->m_allPages.indexOf(page);
    if(index < 0)
        return;
    
    // fetch current page (if any)
    WRT::WrtBrowserContainer * oldPage = currentPage();

    // update the index of the current page
    // (even if the page hasn't changed, it's index might have)
    d->m_currentPage = index;

    // do not proceed any further if setting to the same page
    if(page == oldPage)
       return;

    // disconnect any existing aggregate signalling for pgMgr
    if(oldPage) {
        disconnect(oldPage, 0, this, 0);
        disconnect(oldPage->mainFrame(), 0, this, 0);
        disconnect(oldPage->networkAccessManager(), 0, this, 0);
        disconnect(oldPage->loadController(), 0, this, 0);
        connect(oldPage, SIGNAL(createNewWindow(WrtBrowserContainer*)), this, SLOT(createWindow(WrtBrowserContainer*)));
        connect(oldPage, SIGNAL(loadFinished(bool)), SLOT(onLoadFinishedForBackgroundWindow(bool)));
    }
    // aggregate "current page" signalling from this page to PageMgr clients
    connect(page, SIGNAL(secureStateChange(int)), this, SLOT(secureStateChange(int)));
    connect( page->mainFrame(), SIGNAL ( titleChanged ( const QString& ) ), SIGNAL ( titleChanged ( const QString& ) ) );
    connect( page, SIGNAL( loadStarted() ), SIGNAL( loadStarted() ) );
    connect( page->mainFrame(), SIGNAL(initialLayoutCompleted() ), SIGNAL( initialLayoutCompleted() ) );
    connect( page, SIGNAL( loadProgress(int) ), SIGNAL( loadProgress(int) ) );
    connect( page, SIGNAL( loadFinished(bool) ), SLOT( onLoadFinished(bool) ) );
    
    connect( page, SIGNAL( databaseQuotaExceeded (QWebFrame *,QString) ), SLOT( onDatabaseQuotaExceeded (QWebFrame *,QString)) );  
    
    connect( page->mainFrame(), SIGNAL( iconChanged() ), this, SIGNAL( pageIconChanged() ) );
    connect( page->loadController(), SIGNAL( pageLoadStarted() ), SIGNAL( pageLoadStarted() ) );
    connect( page->loadController(), SIGNAL( pageLoadFailed() ), SIGNAL( pageLoadFailed() ) );
    connect( page->loadController(), SIGNAL( pageLoadProgress(int) ), SIGNAL( pageLoadProgress(int) ) );
    connect( page->loadController(), SIGNAL( pageLoadFinished(bool) ), SIGNAL( pageLoadFinished(bool) ) );
    connect( page->loadController(), SIGNAL( pageUrlChanged(QString) ), SIGNAL( pageUrlChanged(QString) ) );
    connect( page, SIGNAL(createNewWindow(WrtBrowserContainer*)), this, SLOT(createWindow(WrtBrowserContainer*)));


    // Connect the unsuportedContent signal to start a new download
    connect( page, SIGNAL(unsupportedContent(QNetworkReply *)), this, SLOT(unsupportedContentArrived(QNetworkReply *)));

    connect( page, SIGNAL( loadStarted() ), SLOT( updateStatePageLoading() ) );
    

    connect( page->mainFrame(), SIGNAL( iconChanged() ), this, SIGNAL( currentPageIconChanged() ) );
    connect( page->mainFrame(), SIGNAL( urlChanged( const QUrl &)), this, SIGNAL( currentPageUrlChanged( const QUrl &) ));
    connect( page->mainFrame(), SIGNAL( urlChanged( const QUrl &)), this, SLOT( urlChanged( const QUrl &) ));

    connect( page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SIGNAL( sslErrors(QNetworkReply *, const QList<QSslError> &)));
    connect( page->networkAccessManager(), SIGNAL(showMessageBox(WRT::MessageBoxProxy*)), this, SIGNAL(showMessageBox(WRT::MessageBoxProxy*)));
	// Connect network reply errors 
    connect( page->networkAccessManager(), SIGNAL( networkErrorHappened(const QString &) ), this, SIGNAL( networkErrorHappened(const QString &) ) );
    connect( page->networkAccessManager(), SIGNAL( networkErrorHappened(const QString &) ), this, SLOT( processNetworkErrorHappened(const QString &) ) );
    connect( page->networkAccessManager(), SIGNAL( networkErrorUrl(const QUrl &) ), this, SLOT( processNetworkErrorUrl(const QUrl &) ) );
    
    connect(currentPage(), SIGNAL(pageScrollPositionZero()), this, SIGNAL(pageScrollPositionZero()) );
    connect(page, SIGNAL(scrollRequested(int, int, const QRect & )), this, SIGNAL(pageScrollRequested(int, int, const QRect & )) );

    Q_ASSERT ( d->m_currentPage >= 0 );
    
    updateActions(false);
    
    emit pageChanged( oldPage, page );
}



void WebPageController::onLoadFinished(bool ok)
{
    updateStatePageLoadComplete(ok);
    if (ok) {
        QTimer::singleShot(300, this, SLOT(updateHistory()));
    }
    else {
        emit(loadFinished(false));
    }
}


void WebPageController::onDatabaseQuotaExceeded (QWebFrame *frame, QString database)  
{
  QString  dbdir = QWebSettings::offlineStoragePath ();	
  QDir dir(dbdir);
  
  if(!dir.exists()||(dir.count() <= 1)) // empty DB
  	return;
	
		
	if(frame)
	{
		QWebSecurityOrigin qwso = frame->securityOrigin();
		qint64 quota = qwso.databaseQuota() ;
		qint64 usage = qwso.databaseUsage() ;		
	}
	//m_promptMsg = "Database Quota Error";
    m_promptMsg = qtTrId("txt_browser_error_database_quota");
	emit databaseQuotaExceeded (frame, database);
	
	return;
}

void WebPageController::onLoadFinishedForBackgroundWindow(bool ok)
{
    if (!ok)
        return;
    WRT::WrtBrowserContainer* page = qobject_cast<WRT::WrtBrowserContainer*> (sender());
    if (page){
        page->setUpdateThumbnail(true);
        // Current page is handled in onLoadFinished() so skip this case here
        if(page != currentPage()){
			HistoryManager::getSingleton()->addHistory(page->mainFrame()->url().toString(), page->pageTitle());	
			emit (loadFinishedForBackgroundWindow(true,page));
        }
	}
}

void WebPageController::updateHistory()
{
    HistoryManager::getSingleton()->addHistory(currentDocUrl(), currentDocTitle());
    emit(loadFinished(true));
}

/*!
 * Handling of network error happens signal 
 * @return    Returns current page handle
 * @see WRT::WrtBrowserContainer
 */ 
void WebPageController::processNetworkErrorHappened(const QString & msg )
    {
    m_networkError = true;
    m_networkErrorMsg = msg; 
    }

/*!
 * Handling of network error associated url
 * @return    Returns current page handle
 * @see WRT::WrtBrowserContainer
 */
void WebPageController::processNetworkErrorUrl(const QUrl & url )
    {
    QString errorUrl = url.toString();
    QString requestedUrl = currentRequestedUrl(); 
    m_networkErrorUrl = url.toString(); 
    
    // Check if requestedUrl matches the URL from the network error
    // This is to resolve the problems when error popups occur when they shouldn't
    // especially during download.
    int matches = m_networkErrorUrl.compare(requestedUrl);
    if (matches == 0) m_bErrorUrlMatches = true;
    }

//void QWEBKIT_EXPORT qt_drt_garbageCollector_collect();

/*! 
 * Try to release as much memory as possible from caches and collectors
 */
void WebPageController::releaseMemory()
    {
    // clears JavaScript Collector heap; private export in QWebFrame, so commented out for now
    // qt_drt_garbageCollector_collect(); 
    
    // webkit/webkit/qt/api
    QWebSettings::clearIconDatabase();    
    QWebSettings::clearMemoryCaches();
    
    QPixmapCache::clear();    
    }

/*! 
 * Handle low memory signals from LowMemoryHandler
 * @see LowMemoryHandler
 */
void WebPageController::handleLowMemory()
    {
    currentStop();
    releaseMemory();
    }

/*! 
 * Handle out of memory signals from LowMemoryHandler
 * @see LowMemoryHandler
 */
void WebPageController::handleOutOfMemory()
    {
    currentStop();
    releaseMemory();
    }

/*!
 * Retrieve a pointer to the current WRT::WrtBrowserContainer
 * @return    Returns current page handle
 * @see WRT::WrtBrowserContainer
 */
WRT::WrtBrowserContainer* WebPageController::currentPage() const
{
    if ( d->m_currentPage >= 0 && d->m_currentPage < d->m_allPages.count() )
        return d->m_allPages.at ( d->m_currentPage );
    else
        return NULL;
}

/*!
 * Retrieve a list of all of the pages managed by WebPageController
 * @return   List of all the pages opened by WebPageController
 * @see WRT::WrtBrowserContainer
 */
QList<WRT::WrtBrowserContainer*>* WebPageController::allPages()
{
    return &d->m_allPages;
}

/*! 
 * Retrieve the number of pages managed by WebPageController
 * @return  count of all the pages currently opend by WebPageController
 */
int WebPageController::pageCount()
{
    return d->m_allPages.count();
}

/*!
  Retrieve the icon of the current page's main frame or
  an NULL icon if there is no current page
*/
QIcon WebPageController::pageIcon()
{
    WRT::WrtBrowserContainer* page = currentPage();
    if ( page )
        return page->mainFrame()->icon();
    else
        return QIcon();
}
/*!
 * Return the widgetParent of this pageManager
 * @return : Parent Widget of PageManager
 */
/*QWidget * WebPageController::widgetParent()
{
    return d->m_widgetParent;
}*/

/*! 
  Return the list of public QActions most relevant to the managers's current context
  (most approptiate for contextual menus, etc.)
 * @return : returns public actions list (Reload, Stop, Forward, Back)
 */
QList<QAction*> WebPageController::getContext()
{
    // return all actions for now
    QList<QAction*> contextList;
    contextList << 
        d->m_actionBack <<
        d->m_actionForward <<
        d->m_actionReload <<
        d->m_actionStop <<
        d->m_actionForward;
    return contextList;
}

/*!
 * Action for reloading the current page
 * @return : Returns public Reload action handle
*/
QAction * WebPageController::getActionReload()
{
    return d->m_actionReload;
}

/*!
 * Action for stopping the load of the current page
 * @return : returns Public Stop action handle
*/
QAction * WebPageController::getActionStop()
{
    return d->m_actionStop;
}

/*!
 * Action for going to the previous site from history for the current page
 * @return : Returns Public Back action (Ownership with WebPageController)
*/
QAction * WebPageController::getActionBack()
{
    return d->m_actionBack;
}

/*!
 * Action for going to the next site from history for the current page
 * @return : Returns handle to Forward action(Ownership with WebPageController)
*/
QAction * WebPageController::getActionForward()
{
    return d->m_actionForward;
}

/*!
*  Reload for the current page
*/
void WebPageController::currentReload()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Reload);
}

/*!
  Stop loading for the current page
*/
void WebPageController::currentStop()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage){
        currentPage()->loadController()->setCanceled(true);
        activePage->triggerAction(QWebPage::Stop);
    }
}

/*!
  Go back for the current page
*/
void WebPageController::currentBack()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Back);
}

/*!
  Go forward for the current page
*/
void WebPageController::currentForward()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Forward);
}

/*!
  Load the given /a url into the current page
*/
void WebPageController::currentLoad(const QUrl & url)
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage) {
        activePage->mainFrame()->load(url);
    }
}

void WebPageController::currentLoad(const QString &url)
{
    currentLoad(QUrl(url));
}

/*
Share the url through mail client 
*/
#if defined(Q_OS_SYMBIAN) 
void WebPageController::share(const QString &url)
{
#ifdef ORBIT_UI
    ShareUi *shareUi = new ShareUi();
    QStringList stringList;
    stringList << QString(url);
    shareUi->send(stringList,true); 
    if(shareUi)
       delete shareUi;
#else
    QString body = url;    
    CSendUi* sendUi = CSendUi::NewLC();
    CMessageData* messageData = CMessageData::NewLC();

    TSendingCapabilities sendingCapabilities;
    sendingCapabilities.iFlags = TSendingCapabilities::ESupportsAttachments;
        	
    // Not interested in these following services
	CArrayFixFlat<TUid>* array = new (ELeave) CArrayFixFlat<TUid>(8); 
    CleanupStack::PushL(array);
   	array->AppendL(KSenduiTechnologyMmsUid);
    array->AppendL(KSenduiTechnologyIMUid);	    
    array->AppendL(KMmsDirectUpload);
    array->AppendL(KMmsIndirectUpload);
    array->AppendL(KMmsUploadService1Id);
    array->AppendL(KMmsUploadService2Id);
    array->AppendL(KMmsUploadService3Id);
    array->AppendL(KMmsUploadService4Id);
    array->AppendL(KShareOnline);
    array->AppendL(KShareOnOvi);
				    
    // Body
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* bodyRichText = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( bodyRichText );

    TPtrC bodyPtr(reinterpret_cast<const TUint16*>(body.utf16()));

    if( bodyPtr.Length() )
        bodyRichText->InsertL( 0, bodyPtr );
    else
        bodyRichText->InsertL( 0, KNullDesC );

    messageData->SetBodyTextL( bodyRichText );
    sendUi->ShowQueryAndSendL(messageData, sendingCapabilities, array); 
    CleanupStack::PopAndDestroy( 6 ); // bodyRichText, charFormat, paraFormat, messageData, sendUi
#endif // ORBIT_UI
 }
#else
void WebPageController::share(const QString &url)
{
    return; // Not supported 
}
#endif 

/*
Feedback from user
*/
#if defined(Q_OS_SYMBIAN) 
void WebPageController::feedbackMail(const QString &mailAddress, const QString &mailBody)
{
#ifdef ORBIT_UI
	
    XQAiwRequest *sendRequest;
    XQApplicationManager appManager;
    QVariant retValue;
	 
    sendRequest = appManager.create(XQI_EMAIL_MESSAGE_SEND, "send(QVariant)",true);
        
	if(sendRequest) {		 
        QMap<QString, QVariant> map;
        QStringList recipients;
	    QList<QVariant> data;	
	    recipients.append(mailAddress);
		map.insert(EMAIL_SEND_TO_KEY, recipients);
		data.append(map);
		data << mailBody;		
		sendRequest->setArguments(data);
		sendRequest->send(retValue);
	}

	if(sendRequest) {
        delete sendRequest;
	}	
#else
    QString to = mailAddress; 
    QString body = mailBody;
    CSendUi* sendUi = CSendUi::NewLC();
    CMessageData* messageData = CMessageData::NewLC();

    // Body
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* bodyRichText = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( bodyRichText );
    TPtrC ToPtr(reinterpret_cast<const TUint16*>(to.utf16()));
    TPtrC bodyPtr(reinterpret_cast<const TUint16*>(body.utf16()));
    if( bodyPtr.Length() )
        bodyRichText->InsertL( 0, bodyPtr );
    else
        bodyRichText->InsertL( 0, KNullDesC );

    messageData->AppendToAddressL( ToPtr );
    messageData->SetBodyTextL( bodyRichText );

    TRAP_IGNORE(sendUi->CreateAndSendMessageL( KUidMsgTypeSMTP, messageData ));
//    TRAP_IGNORE(sendUi->CreateAndSendMessageL( KSenduiMtmSmsUid, messageData ));
    CleanupStack::PopAndDestroy( 5 ); 
#endif //// ORBIT_UI  
}
#else
void WebPageController::feedbackMail(const QString &mailAddress, const QString &mailBody)
{
    return; // Not supported 
}
#endif 
/*!
  Load an item into the current page from a given /a historyIndex
*/
void WebPageController::currentSetFromHistory(int historyIndex)
{

    WRT::WrtBrowserContainer* activePage = currentPage();
    if(activePage) {
        QList<QWebHistoryItem> items = activePage->history()->items();
        QWebHistoryItem item = items.at(historyIndex);
        if (item.isValid()) {
            // userData will be set by canvas upon Commit
            currentPage()->history()->goToItem(item);
        }
    }
}

void WebPageController::gotoCurrentItem()
{
    WRT::WrtBrowserContainer* activePage = currentPage();
    if(activePage) {
        QList<QWebHistoryItem> items = activePage->history()->items();
        QWebHistoryItem item = activePage->history()->currentItem();
        if (item.isValid()) {
            // userData will be set by canvas upon Commit
            currentPage()->history()->goToItem(item);
        }
    }
}

/*! 
 * Private slot
 */
void WebPageController::updateStatePageLoading()
{
    updateActions(true);
    
    // Reset to false for new page
    m_bErrorUrlMatches = false;
    
    // This is needed for loading a restored window (otherwise url bar is empty)
    currentPage()->loadController()->setUrlText(currentPage()->history()->currentItem().url().toString());
}

/*!
 * Private slot
 */
void WebPageController::updateStatePageLoadComplete(bool)
{
    updateActions(false);
}

/*!
 * Private slot
 */
void WebPageController::updateActions(bool pageIsLoading)
{
    d->m_actionReload->setEnabled(!pageIsLoading);
    d->m_actionStop->setEnabled(pageIsLoading);
    
    WRT::WrtBrowserContainer* activePage = currentPage();    

    if(activePage) {
        int histLength = activePage->history()->count();
        int currItemIndex = activePage->history()->currentItemIndex();
        d->m_actionBack->setEnabled((histLength > 1) && (currItemIndex > 0));
        d->m_actionForward->setEnabled((histLength > 1) && (currItemIndex < (histLength-1)));
        	
    }
}

/*!
 * Private slot
 */
void WebPageController::unsupportedContentArrived(QNetworkReply *reply)
{
    emit unsupportedContent(reply);
}

/*!
 * Private slot
 */
void WebPageController::createWindow(WrtBrowserContainer* page)
{
   openPage(this,page) ;
}

void WebPageController::LoadInNewWindow(const QString & url)
{
    QWebPage* newPage = openPage();
    if (newPage) {
        newPage->mainFrame()->load(QUrl(url));
    }
}

/*!
 * Private slot
 * Emits signals based on secure state 
 */
void WebPageController::secureStateChange(int state)
{
    //qDebug() << __func__ << state;
    switch(state) {
        case SecureUIController::secureLoadFinished:
        case SecureUIController::untrustedLoadFinished:
        case SecureUIController::mixedLoadFinished:
        case SecureUIController::untrustedMixedLoadFinished:
            emit showSecureIcon();
            break;
        case SecureUIController::unsecureLoadFinished:
            emit hideSecureIcon();
            break;
        default:
            break; 
    }
}


WebPageController* WebPageController::getSingleton()
{
    static WebPageController* singleton = 0;
    if(!singleton) 
    {
    	singleton = new WebPageController;
        singleton->setObjectName("pageController"); 
    } // if(! singleton)

    assert(singleton);
    return singleton;
}

WRT::WrtBrowserContainer* BrowserPageFactory::openBrowserPage()
{
    return WebPageController::getSingleton()->openPage();
}


QString WebPageController::removeScheme(const QString & s) {

    return (WRT::UiUtil::removeScheme(s));
}

QString WebPageController::guessUrlFromString(const QString &s){
    QUrl u = WRT::UiUtil::guessUrlFromString(s);
    return u.toString();
}

QString WebPageController::searchUrl(const QString &s){
    QUrl u = WRT::UiUtil::searchUrl(s);
    return u.toString();
}

QObjectList WebPageController::fetchSuggestions(const QString &s){
    QObjectList suggestList = WRT::ContentAgent::getSingleton()->getUISuggestion(s);
	return suggestList;
}

void WebPageController::loadLocalFile()
	{
    QString chromeBaseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("LocalPagesBaseDirectory");
    QString startPage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage");
    QString startPagePath = chromeBaseDir + startPage;

    currentLoad(startPagePath);
	}

void WebPageController::loadInitialUrlFromOtherApp(QString url)
{
    m_bRestoreSession = false;
    
    // open in current window
    currentLoad(guessUrlFromString(url));
}

void WebPageController::loadFromHistory() 
{
   int count = historyWindowCount();
   
   if(!count)
   {
   	    m_bRestoreSession = FALSE;
    		loadLocalFile(); // load start page for first startup of the browser
   }
    else
    {	
    	 int count2 = restoreNumberOfWindows();
    	 
    	 // "sanity check" count shall be always equal to count2. If not, something wrong. Open only one window - start page in this case.
    	 	
    	 if((count > 0) && (count == count2)) // load all browser windows from the last session
    	 {
    	 	
    	 	for(int i = 0; i < count2-1; i++) //all windows except current
    	 	{
    	 		openPageFromHistory(i);
    	 	}		
    	  m_bRestoreSession = false;

    	 // Go to current window
    	 int pageIndex = activeWindowId();
    	 WRT::WrtBrowserContainer* page = d->m_allPages.at(pageIndex);
    	 setCurrentPage(page);
    	 gotoCurrentItem();
    	 	
    	 }
    	 else // browser did not close correctly; exception handler: delete all files and load start page as in first startup
    	 {
    	 	deleteHistory();
    	 	deleteDataFiles();
    	 	loadLocalFile();
    	 }			    	 
    }	 
    m_bRestoreSession = false;
    return ;

}


WRT::WrtBrowserContainer* WebPageController::startupRestoreHistory(QWidget* parent, int index, WRT::WrtBrowserContainer* page)
{
    Q_UNUSED(parent)
    // restore the history state from the disk for the current page
    QString indexStr;
    indexStr.setNum(index);
    QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
   
    QFile file(historyFile);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream restore(&file);
        restore >> *(page->history());
        file.close();
    }
    
    return page;
}

void WebPageController::setLastUrl(QString url)
{
	QString winFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    
  QFile file(winFile);
  if(file.open(QIODevice::WriteOnly)) {
        QTextStream textStream(&file);
        textStream << url;
        textStream.flush();
        file.close();
   }
	
}

QString WebPageController::getLastUrl()
{
	  QString winFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    QFile file(winFile);
    QString winStr = "";
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&file);
        textStream >> winStr;
        file.close();
     }
    
	  return winStr;
}

void WebPageController::saveHistory(int* windowsSaved, int* activeWindowId)
{
    QString indexStr;
    int index = 0;
    
    unsigned int pageCount =  d->m_allPages.count();
    
    // Get index of current page
    WRT::WrtBrowserContainer* theCurrentPage = currentPage();
    int currentIndex = d->m_allPages.indexOf(theCurrentPage);
         
    for (int tIndex = 0; tIndex < pageCount; tIndex++)
    {
       	// Don't save blank windows
       	WrtBrowserContainer* page = d->m_allPages.at(tIndex);
       	QString pageUrl = page->history()->currentItem().url().toString();
       	if (!pageUrl.isEmpty())
        {    	  

          // In case of blank windows, reset the active window id
          if (currentIndex == tIndex) *activeWindowId = index;

          // Serialize page to file
          indexStr.setNum(index);
          QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
          QFile file(historyFile);
          if(file.open(QIODevice::WriteOnly)) {
             QDataStream save(&file);
             save << *(d->m_allPages.at(tIndex)->history());
             file.flush();
             file.close();
          }
          index++;
        }
    }

    // Save last url
    WRT::WrtBrowserContainer* currentPage = d->m_allPages.at(*activeWindowId);
    QWebHistoryItem	item = theCurrentPage->history()->currentItem();
    QString lastUrl = item.url().toString();
  	setLastUrl(lastUrl);
    *windowsSaved = index;
}

void WebPageController::deleteDataFiles()
{
    
    QDir dir(d->m_historyDir);
    
    QString lasturlFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    QFile file1(lasturlFile);
    if(file1.open(QIODevice::ReadOnly)) 
    {
         file1.remove();
         file1.close();
    }
    
    QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
    QFile file2(winFile);
    if(file2.open(QIODevice::ReadOnly)) 
    {
         file2.remove();
         file2.close();
    }
    
    d->donotsaveFlag = true;
          
}

QString WebPageController::getTextEncoding()
{
 //QString encoding = QWebSettings::globalSettings()->defaultTextEncoding();
 QString encoding = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("BrowserEncoding").toString();
 	
 if(encoding == "Automatic")
 	QWebSettings::globalSettings()->setDefaultTextEncoding("");
 else		
  QWebSettings::globalSettings()->setDefaultTextEncoding(encoding);
 	
 return encoding;
} 	


void WebPageController::setTextEncoding(const QString & encoding )
{
 if(encoding == "Automatic")
 	QWebSettings::globalSettings()->setDefaultTextEncoding("");
 else		
 	QWebSettings::globalSettings()->setDefaultTextEncoding(encoding);
 	
 BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->setValue("BrowserEncoding", encoding);
 return ;
} 	


void WebPageController::setSettingsLoaded(int value)
{
 	m_settingsLoaded = value;
}	
 	
int WebPageController::getSettingsLoaded()
{
	return m_settingsLoaded;
}    

void WebPageController::savePopupSettings(int val)
{
    BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->setValue("PopupBlocking", val);
  
    //qDebug() << "WebPageController::savePopupSettings " << val  <<  d->m_allPages.count();
    for (int tIndex = 0; tIndex <  d->m_allPages.count(); tIndex++)
    {
        WrtBrowserContainer* page = d->m_allPages.at(tIndex);
        page->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, !val);

    }

}

bool WebPageController::getPopupSettings()
{
		bool val = (bool)BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("PopupBlocking").toInt();
		return val;
		
}


void WebPageController::saveSaverestoreSettings(int val)
{
	  BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->setValue("SaveSession", val);
	  BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->setValue("SaveHistory", val);
	  	
	  	if(val == 0)
	  		d->donotsaveFlag = true; // do not save .dat file
	  	else // val = 1
	  		d->donotsaveFlag = false;
	  		
}

bool WebPageController::getSaverestoreSettings()
{
		bool val = (bool)BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("SaveSession").toInt();
		return val;
		
}

void WebPageController::saveNumberOfWindows(int windowsSaved)
{
	QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
    
  QFile file(winFile);
  if(file.open(QIODevice::WriteOnly)) {
        QTextStream textStream(&file);
        textStream << windowsSaved;
        textStream.flush();
        file.close();
   }
	
}
void WebPageController::saveActiveWindowId(int activeWindowId)
{
	QString winFile = d->m_historyDir + QLatin1String("/activewindow") + QLatin1String(".dat");
  
  QFile file(winFile);
  if(file.open(QIODevice::WriteOnly)) {
        QTextStream textStream(&file);
        textStream << activeWindowId;
        textStream.flush();
        file.close();
   }
	
}

int WebPageController::restoreNumberOfWindows()
{
	QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
    QFile file(winFile);
    QString winStr = "0";
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&file);
        textStream >> winStr;
        file.close();
    }
    return winStr.toInt();
}

int WebPageController::activeWindowId()
{
	QString winFile = d->m_historyDir + QLatin1String("/activewindow") + QLatin1String(".dat");
    QFile file(winFile);
    QString winStr = "0";
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&file);
        textStream >> winStr;
        file.close();
    }
    return winStr.toInt();
}

int WebPageController::historyWindowCount()
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    int windowCount = 0;
    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {        	  
            windowCount++;            
        }
    }
    return windowCount;
}

QString WebPageController::currentDocTitle()
{
    assert(currentPage());
    return currentPage()->pageTitle();
}

QString WebPageController::currentDocUrl() const
{
    assert(currentPage());
    return currentPage()->mainFrame()->url().toString();
}

QString WebPageController::currentRequestedUrl() const
{
    assert(currentPage());
    //qDebug() << __func__ << "Current Page" << currentPage() << "Requested Url " << currentPage()->mainFrame()->requestedUrl().toString();
    return currentPage()->mainFrame()->requestedUrl().toString();
}

QString WebPageController::currentPartialUrl() 
{
    assert(currentPage());
    return (partialUrl(currentPage()->mainFrame()->url()));
}

QString WebPageController::currentPartialReqUrl() 
{
    assert(currentPage());
    return (partialUrl(currentPage()->mainFrame()->requestedUrl()));
}

int WebPageController::contentsYPos()
{
    assert(currentPage());
    return currentPage()->mainFrame()->scrollPosition().y();
}

int WebPageController::currentPageIndex(){

    return d->m_currentPage;
}

int WebPageController::secureState() {

    return (currentPage()->secureState());
}

// copy/paste from controllableviewjstobject. TODO: merge common code
void WebPageController::updateJSActions()
{   // slot
    if(d->m_actionsParent) {
        delete d->m_actionsParent;
        d->m_actionsParent = 0;
    }
    if(!getContext().isEmpty()) {
        d->m_actionsParent = new QObject(this);
        d->m_actionsParent->setObjectName("actions");
        foreach(QAction *action, getContext()) {
            ActionJSObject *jso = new ActionJSObject(d->m_actionsParent, action);
        }
    }
}


bool WebPageController::removeDirectory(QDir &aDir)
{
		bool has_err = false;
		if (aDir.exists())
		{
				QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot | 
				QDir::Dirs | QDir::Files);
				int count = entries.size();
				for (int idx = 0; ((idx < count) && (0 == has_err)); idx++)
				{
						QFileInfo entryInfo = entries[idx];
						QString path = entryInfo.absoluteFilePath();
						if (entryInfo.isDir())
						{
							QDir dir(path);
							has_err = removeDirectory(dir);
						}
						else
						{
							QFile file(path);
							if (!file.remove())
							has_err = true;
						}
				}
				if (!aDir.rmdir(aDir.absolutePath()))
					has_err = true;
		}
		return(has_err);
}

void WebPageController::clearHistoryInMemory()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
    {
        activePage->history()->clear();
    }
}

void WebPageController::deleteHistory()
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    int index = 0;    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {
            indexStr.setNum(index);
            QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
            QFile file(historyFile);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }
            index++;          
        }
    }
}

void WebPageController::deleteCookies()
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    int index = 0;    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        //if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {
	    if (filePath.endsWith(QString(KCOOKIESEXTENSION), Qt::CaseInsensitive)) {
            indexStr.setNum(index);
            QString cookiesFile = d->m_historyDir + QLatin1String("/cookies.ini");
            QFile file(cookiesFile);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }
            index++;          
        }
    }
}

void WebPageController::deleteCache()
{
	  
    QWebDatabase::removeAllDatabases();
    
    QString  str1 = QWebSettings::offlineStoragePath ();
    QString  str2 = QWebSettings::offlineWebApplicationCachePath ();
    QWebSettings * globalSettings = QWebSettings::globalSettings();
    QString  LocalStoragePath = globalSettings->localStoragePath();
    	
    /*QString deleteFlag = d->m_historyDir + QLatin1String("/deleteOfflineStorage");
    QFile deleteFileFlag(deleteFlag);
    if(deleteFileFlag.open(QIODevice::WriteOnly)) { // create indicator that marks offline storage for deletion while next browser startup
         deleteFileFlag.close();
    }*/
    
    QDir LocalStorage(LocalStoragePath);  	
    removeDirectory(LocalStorage); 
    
    QString applicationCacheFile = d->m_historyDir + QLatin1String("/ApplicationCache.db");  
    QFile file(applicationCacheFile);
    if(file.open(QIODevice::ReadOnly)) {
         file.remove(); // may fail if file in usage; will be deleted in next browser startup
         file.close();
    }
    
	  if ( !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheEnabled").toBool() ) 
			return;
		
		QString diskCacheDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheDirectoryPath").toString();
			
    if(diskCacheDir.isEmpty()) 
    	return;
    	
    //QDir dir1(d->m_historyDir +"/cwrtCache/http");	  
    QDir dir1(diskCacheDir + "/http");
    	
    QFileInfoList fileList1(dir1.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList1) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    //QDir dir2(d->m_historyDir +"/cwrtCache/https");
    QDir dir2(diskCacheDir +"/https");
    
    QFileInfoList fileList2(dir2.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList2) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    //QDir dir3(d->m_historyDir +"/brCache/prepared");
    QDir dir3(diskCacheDir +"/cwrtCache/prepared");
    
    QFileInfoList fileList3(dir3.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList3) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    
}

void WebPageController::urlChanged(const QUrl& url)
{
    //private slot
    QString urlString = partialUrl(url);
    emit partialUrlChanged(urlString);    
}

QString WebPageController::partialUrl(const QUrl& url)
{

    QString urlString = url.toString();
    QString scheme=url.scheme();
    urlString.remove(0, scheme.length() + 3); // remove "scheme://"
    if (urlString.startsWith("www.", Qt::CaseInsensitive)) {
        urlString.remove(0, 4);
    }
    return urlString;
}

QGraphicsWebView* WebPageController::webView()
{
    for (int i = 0; i < allPages()->count(); i++) {
        WRT::WrtBrowserContainer* page = allPages()->at(i);
        if (page->webWidget()) {
            return qobject_cast<QGraphicsWebView*> (page->webWidget());
        }
    }
    return NULL;
}

void WebPageController::checkAndUpdatePageThumbnails()
{
//    WebContentWidget* view = qobject_cast<WebContentWidget*> (webView());
    QGraphicsWebView *view = webView();
    WRT::WrtBrowserContainer* savedPage = qobject_cast<WRT::WrtBrowserContainer*> (view->page());
    if(!savedPage) return;
    QSize currSize = view->size().toSize();

    bool needRestore =  false;

    for (int i = 0; i < allPages()->count(); i++) {
        WRT::WrtBrowserContainer* page = allPages()->at(i);
        QWebHistoryItem item = page->history()->currentItem();
        WebPageData data = item.userData().value<WebPageData>();

        // If not still a blank window, check whether we need to update the img
        if (!page->emptyWindow() ){
            QImage img = data.m_thumbnail;
            bool isSameMode = ( (img.size().width() > img.size().height()) == (currSize.width() > currSize.height()) );
            if (img.isNull() || !isSameMode) {
                needRestore = true;
                view->setPage(page);
                page->setWebWidget(view);
                QCoreApplication::sendEvent(view, new WebPageControllerUpdateViewPortEvent());
                page->savePageDataToHistoryItem(page->mainFrame(), &item);
                page->setUpdateThumbnail(false);
            }
            if (page->needUpdateThumbnail()) {
                page->savePageDataToHistoryItem(page->mainFrame(), &item);
                page->setUpdateThumbnail(false);
            }
         }
    }

    // restore
    if (needRestore) {    
        view->setPage(savedPage);
        savedPage->setWebWidget(view);
    }
}

void WebPageController::updatePageThumbnails()
{
    // update current page's thumbnail forcely since the scrolling position may change
    WRT::WrtBrowserContainer *page = currentPage();
    if(page) {
       QWebHistoryItem item = page->history()->currentItem();
       page->savePageDataToHistoryItem(page->mainFrame(), &item);
       page->setUpdateThumbnail(false);
       checkAndUpdatePageThumbnails();
    }
}

void WebPageController::resizeAndUpdatePageThumbnails(QSize& s)
{
    webView()->resize(s); // resize the view

    checkAndUpdatePageThumbnails();
}

void WebPageController::urlTextChanged(QString str ) {

   //qDebug() << __func__ << "Current Page" << currentPage() << "Url " << str;
   currentPage()->loadController()->setUrlText(str);
}

int WebPageController::loadState() {
    //if (currentPage())

    return currentPage()->loadController()->mode();
}

void WebPageController::setEditMode(bool editing) {

    //qDebug() << __func__;
    currentPage()->loadController()->setEditMode(editing);
    //qDebug() << __func__ << currentPage()->loadController()->mode();
}

bool WebPageController::editMode() {

    return (currentPage()->loadController()->editMode());
}

int WebPageController::loadProgressValue() {

    //qDebug()  << __func__ << currentPage()->loadController()->progressValue();
    return currentPage()->loadController()->progressValue();
}

QString WebPageController::loadText() {

    //qDebug() << __func__ << "Current Page" << currentPage() << currentPage()->loadController()->urlText();
    return currentPage()->loadController()->urlText();
}

bool WebPageController::isPageLoading() {
    return currentPage()->loadController()->isPageLoading();
}

bool WebPageController::loadCanceled() {
    return currentPage()->loadController()->loadCanceled();
}

bool WebPageController::networkError() {
    return m_networkError; 
}

QString WebPageController::networkErrorMsg() {
    return m_networkErrorMsg; 
}

QString WebPageController::networkErrorUrl() {
    return m_networkErrorUrl; 
}

bool WebPageController::errorUrlMatches() {
  return m_bErrorUrlMatches;
}

QString WebPageController::promptMsg() {
    return m_promptMsg; 
}

QString WebPageController::promptReserved() {
    return m_promptReserved; 
} 

/*!
  \fn void WebPageController::pageCreated(WrtPage* newPage);
  emitted when a page is created
*/

/*!
  \fn void WebPageController::pageDeleted(WrtPage* newPage);
  emitted when a page is deleted (i.e. closed)
*/

/*!
  \fn void WebPageController::pageChanged(WrtPage* oldPage, WrtPage* NewPage);
  emitted when the current page is changed (note, either old or new could be NULL)
*/

/*!
  \fn void WebPageController::titleChanged(const QString &);
  emitted when the title of the current page changes
*/

/*!
  \fn void WebPageController::loadStarted();
  emitted when a load of the current page has begun
*/

/*!
  \fn void WebPageController::loadProgress( int progress );
  emitted to indicate load progress of the current page
*/

/*!
  \fn void WebPageController::loadFinished( bool ok );
  emitted to indicate load progress of the current page
  \a ok indicates whether load was successful
*/

/*!
  \fn void WebPageController::void loadFinishedForBackgroundWindow(const bool ok, WRT::WrtBrowserContainer *page);
  emitted to indicate load progress of the page in the background window
  \a ok indicates whether load was successful
  \a page is the background page that completed loading
*/

/*!
  \fn void WebPageController::currentPageIconChanged();
  emitted when the favicon of the current page has changed
*/

/*!
  \fn void WebPageController::currentPageUrlChanged( const QUrl & url );
  emitted when the url of the current page changed
*/

/*!
  \fn void WebPageController::partialUrlChanged(QString url);
  emitted when the url of the current page changed
*/

/*!
  \fn void WebPageController::unsupportedContent(QNetworkReply *);
  emitted when unsupported content has arrived
*/

/*!
  \fn void WebPageController::networkRequestStarted(QWebFrame*, QNetworkRequest*);
  emitted when a frame of the current page requests a web resource.
*/

/*!
  \fn void WebPageController::sslErrors(QNetworkReply *, const QList<QSslError> &);
  emitted when sslError occurs for the current page
*/

/*!
  \fn void pageScrollRequested(int, int);
  emitted when page gets scrolled
*/

