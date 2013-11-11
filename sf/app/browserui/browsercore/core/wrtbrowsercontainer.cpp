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

#include "browserpagefactory.h"
#include "wrtbrowsercontainer_p.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"
#include "SchemeHandlerBr.h"
#include "webnetworkaccessmanager.h"
#include "webcookiejar.h"
#include "secureuicontroller.h"
#include "LoadController.h"
#include "WebDialogProvider.h"
#include "bedrockprovisioning.h"
#include <QPainter>
#include <QAuthenticator>
#include <QFile>
#include <QMessageBox>
#include <QWebHistory>
#include <QWebFrame>
#include <QGraphicsWebView>

QDataStream &operator<<(QDataStream &out, const WebPageData &myObj)
{
   out << myObj.m_thumbnail << myObj.m_zoomFactor << myObj.m_contentsPos;
   return out;
}

QDataStream &operator>>(QDataStream &in, WebPageData &myObj)
{
   in >> myObj.m_thumbnail >> myObj.m_zoomFactor >> myObj.m_contentsPos;
   return in;
}

namespace WRT
{
const int historyItemsCount = 10;

WrtBrowserContainerPrivate::WrtBrowserContainerPrivate(QObject* parent,
		WrtBrowserContainer* page/*never NULL*/) : m_schemeHandler(new SchemeHandler())
,   m_pageFactory(0)
,   m_widget(0)
,   m_fileChooser(0)
,   m_needUpdateThumbnail(false)
{
    m_page = page;

    m_secureController = new WRT::SecureUIController(parent);
    m_loadController = new  WRT::LoadController();
 
    // Turn off the scroll bars of main frame
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    
}

WrtBrowserContainerPrivate::~WrtBrowserContainerPrivate()
{
	m_page->setView(0);
	if (m_page)
		m_page->disconnect();
	delete m_schemeHandler;
  delete m_secureController;
  delete m_loadController;
  delete m_fileChooser;
}

/*!
 * Static function which creates Wrt page with parent Widget
 * @param parent   : parent widget for the new page
 * @return WrtBrowserContainer : Page handle to the newly created page
 */
WrtBrowserContainer* WrtBrowserContainer::createPageWithWidgetParent(
		QObject* parent, WrtBrowserContainer* page)
{
    if (page)
    {
        page->setParent(parent);
        return page;
    }
    else
        return new WrtBrowserContainer(parent);
}

/*!
 * WrtBrowserContainer Constructor
 * @param parent : Widget parent
 */
WrtBrowserContainer::WrtBrowserContainer(QObject* parent) :
	QWebPage(parent), d(new WrtBrowserContainerPrivate(this, this))
{
    
  settings()->setAttribute(QWebSettings::PluginsEnabled, true);
  settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("PopupBlocking").toInt());
	// Download related enable "forwardUnsupportedContent" to redirect unsupported content to download manager
	setForwardUnsupportedContent(true);
#ifdef BEDROCK_TILED_BACKING_STORE
    settings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);
    settings()->setAttribute(QWebSettings::ZoomTextOnly, false);
    settings()->setAttribute(QWebSettings::FrameFlatteningEnabled, true);

    //Configure tiling properties
    //This would set tile size to (256, 256) and add 25ms delay between constructing
    //individual tiles. The settings would try to cache an area 1.5x width and 1.5x height
    //of the current viewport (centered to the viewport) with tiles and would drop tiles
    //after they are outside an area 2x the width and 2.5x the height of the viewport.
    //Refer https://bugs.webkit.org/show_bug.cgi?id=39874

    setProperty("_q_TiledBackingStoreTileSize", QSize(256, 256));
    setProperty("_q_TiledBackingStoreTileCreationDelay", 25);
    setProperty("_q_TiledBackingStoreCoverAreaMultiplier", QSizeF(1.5, 1.5));
    setProperty("_q_TiledBackingStoreKeepAreaMultiplier", QSizeF(2., 2.5));
#endif

#ifndef NO_NETWORK_ACCESS_MANAGER	
	setNetworkAccessManager(new WebNetworkAccessManager(this,this));
#endif
	
	history()->setMaximumItemCount(historyItemsCount);

    /* Connect secure related signals and slots */
    connect(mainFrame(), SIGNAL(urlChanged(QUrl)), d->m_secureController, SLOT(setTopLevelScheme(const QUrl &)));
    connect(this, SIGNAL(loadFinished(bool)), d->m_secureController, SLOT( endSecureCheck(bool) ) );
    connect(d->m_secureController, SIGNAL(pageSecureState(int)), this, SLOT(pageSecureState(int)));
    connect(networkAccessManager(), SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), d->m_secureController, SLOT(onSslErrors(QNetworkReply *, const QList<QSslError> &)));

    connect(this, SIGNAL(loadStarted()), d->m_loadController, SLOT(loadStarted()));
    connect(this, SIGNAL(loadProgress(int)), d->m_loadController, SLOT(loadProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)), d->m_loadController, SLOT(loadFinished(bool)));
    connect(mainFrame(), SIGNAL(urlChanged(QUrl)), d->m_loadController, SLOT(urlChanged(QUrl)));
	connect(mainFrame(), SIGNAL(initialLayoutCompleted()), d->m_loadController, SLOT(initialLayoutCompleted()));
    
}

/*!
 * WrtBrowserContainer destructor
 */
WrtBrowserContainer::~WrtBrowserContainer()
{
	// gracefully but warn, disconnect anything connected to this page
	if (!disconnect())
		qWarning("WrtPage destructor blanket disconnect failed");

    //setNetworkAccessManager(NULL);
   delete d;
   
   // 
   QList<QWebHistoryItem> items = history()->items();

   for (int i = 0; i < history()->count(); i++) {
       QWebHistoryItem item = items.at(i);
       WebPageData* data = (WebPageData*)(item.userData().value<void*>());
       if(data){
           QVariant variant;
           item.setUserData(variant);
           delete data;
       }
   }
}

QGraphicsWidget* WrtBrowserContainer::webWidget() const
{
    return d->m_widget;
}

void WrtBrowserContainer::setWebWidget(QGraphicsWidget* view)
{
    d->m_widget = view;

    if (view)
    {
        QGraphicsWebView* webView = static_cast<QGraphicsWebView*>(view);
        if(webView)
            webView->setPage(this);

    }
}
 
/*!
 * Handles pageSecureState state 
 */
void WrtBrowserContainer::pageSecureState(int state) {

    emit secureStateChange(state);

}

/*!
 * Returns schemeHandler
 */
SchemeHandler* WrtBrowserContainer::schemeHandler() const
{
   return d->m_schemeHandler;
}

/*!
 * This function page thumbnail for this page as specified by X & Y co-ordinate scale factors
 * @param  scaleX :  X Co-ordinate scale factor for the page thumbnail
 * @param  scaleY :  y Co-ordinate scale factor for the page thumbnail
 */
QImage WrtBrowserContainer::pageThumbnail(qreal scaleX, qreal scaleY)
{
    qDebug() << "WrtBrowserContainer::pageThumbnail:" << webWidget()->size();
    QSize size = webWidget()->size().toSize();
    QImage image(size, QImage::Format_RGB32);

    QPainter painter(&image);
    QRect r(0, 0, size.width(), size.height());
    QRegion clip(r);
    painter.save();
    painter.setBrush(Qt::white);
    painter.drawRect(r);
    painter.restore();
    mainFrame()->render(&painter, clip);
    QImage thumbnail = image.scaled(scaleX * size.width(), scaleY * size.height());
    return thumbnail;
}

/*!
 Pulic Slots:
 void savePageDataToHistoryItem(QWebFrame*, QWebHistoryItem* item);
 void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *);
 void slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
 */

/*!
 * Public slot, saves the page data (like thumbail, current position, zoom factor etc ) to WebHistoryItem
 * @frame  main frame for which the data has to be stored
 * @item   handle to QWebHistoryItem into which the page data has to be saved.
 */

void WrtBrowserContainer::savePageDataToHistoryItem(QWebFrame* frame,
      QWebHistoryItem* item)
{
    if (frame != mainFrame() || !item->isValid() || !webWidget())
    {
        return;
    }
    if (restoreSession()) return;
    	
    WebPageData data(this);
    //   WebPageData data = item->userData().value<WebPageData>();
    data.m_zoomFactor = 1.0; // Need to find a way to get this.  Not used right now anyway
    data.m_thumbnail = pageThumbnail(0.5, 0.5);//data.m_zoomFactor, data.m_zoomFactor);

    QPoint pos(0, 0);
    //    pos = static_cast<WebCanvasWidget*>(webWidget())->canvas()->canvasToDocument(pos);
    pos = mainFrame()->scrollPosition();
    data.m_contentsPos = pos;
    QVariant variant;
    variant.setValue(data);
    item->setUserData(variant);
    //ii++;
}

/*!
 * Public slot  AuthenticationRequired
 * Launches dialog for user name and password if Authentication is required for page load.
 * @param reply     : network reply
 * @param  athenticator : athenticator
 */
void WrtBrowserContainer::slotAuthenticationRequired(QNetworkReply* reply,
      QAuthenticator* authenticator)
{
	QString username, password;
	if (WebDialogProvider::getUsernamePassword(0/*webWidget()*/, username, password))
	{
		authenticator->setUser(username);
		authenticator->setPassword(password);
    }
}

/*!
 * public slot for setting proxy when Authentication is Required
 * @param networkProxy : network Proxy
 * @param authenticator : authenticator
 */
void WrtBrowserContainer::slotProxyAuthenticationRequired(
      const QNetworkProxy& networkProxy, QAuthenticator* authenticator)
{
	QString username, password;
	if (WebDialogProvider::getUsernamePassword(0/*webWidget()*/, username, password))
	{
		authenticator->setUser(username);
		authenticator->setPassword(password);
    }
}

void WrtBrowserContainer::setPageFactory(BrowserPageFactory* f)
{
    d->m_pageFactory = f;
}

QString WrtBrowserContainer::pageTitle(){

    QString title = mainFrame()->title();
    
    // If mainFrame title is empty we may be restoring session
    // Check history title
    if (title.isEmpty()){ 
    	title = history()->currentItem().title();
    }
    
    /* If there is no title, provide the partial url */
    if (title.isEmpty()) {
        QUrl url  = mainFrame()->url(); 
        // If mainframe url is empty, we may be restoring session
        // check history url
        if (url.isEmpty()) {
        	url = history()->currentItem().url();
        }
        title = url.toString();
        QString scheme=url.scheme();
        title.remove(0, scheme.length() + 3); // remove "scheme://"
        if (title.startsWith("www.", Qt::CaseInsensitive)) {
            title.remove(0, 4);
        }

    }
    return title;
}

int WrtBrowserContainer::secureState() {

    return d->m_secureController->secureState();
}
bool WrtBrowserContainer::restoreSession() {
	if (d->m_pageFactory)
    return d->m_pageFactory->m_bRestoreSession;
  else
    return true;
}

bool WrtBrowserContainer::emptyWindow() {

    bool result= false;
    if (mainFrame()->title()  == "" && mainFrame()->url().toString() == "" )
        result = true;

    return result;
}

ZoomMetaData WrtBrowserContainer::pageZoomMetaData() {
    return d->m_zoomData ;

}
void WrtBrowserContainer::setPageZoomMetaData( ZoomMetaData  zoomData ){

    d->m_zoomData = zoomData;
}


WrtBrowserFileChooser::~WrtBrowserFileChooser()
{}

void WrtBrowserContainer::setFileChooser(WrtBrowserFileChooser * chooser)
{
    if (d->m_fileChooser) {
        delete d->m_fileChooser;
    }

    d->m_fileChooser = chooser;
}

QString WrtBrowserContainer::chooseFile(QWebFrame * parentFrame, const QString & suggestedFile)
{
    if (d->m_fileChooser != 0) {
        return d->m_fileChooser->chooseFile(parentFrame, suggestedFile);
    }

    return QWebPage::chooseFile(parentFrame, suggestedFile);
}

QString WrtBrowserContainer::userAgentForUrl(const QUrl& url) const
{
    QString uaString = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("UserAgentString");
   
    if (uaString.isEmpty())
   	{
   	    QUrl url;
        return QWebPage::userAgentForUrl(url); 
   	}
   	else
        return uaString;
}
	
WRT::WrtBrowserContainer* WrtBrowserContainer::createWindow(
    QWebPage::WebWindowType webWindowType)
{

    /* When WrtPage is created, QWebSettings::JavascriptCanOpenWindows is initialized
     * to popup setting value. Need not do any check here 
     */
    if (d->m_pageFactory)
    {
        // no need to signal in this case
        return d->m_pageFactory->openPage();
    }

    WrtBrowserContainer* wrtPage = new WrtBrowserContainer();
    emit createNewWindow(wrtPage);
    return wrtPage;
}


} // namespace WRT

