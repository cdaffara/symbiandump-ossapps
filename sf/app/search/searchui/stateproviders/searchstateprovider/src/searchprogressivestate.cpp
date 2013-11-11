/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Implementation of the progressive search state.
 *
 */
#include "searchprogressivestate.h"
#include "indevicehandler.h"
#include "searchuiloader.h"
#include <cpixdocument.h>
#include <cpixdocumentfield.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbmenu.h>
#include <hbinstance.h>
#include <hbsearchpanel.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <qsortfilterproxymodel.h>
#include <AknsUtils.h>
#include <bitdev.h> 
#include <bitstd.h>
#include <qbitmap.h>
#include <fbs.h>
#include <AknInternalIconUtils.h>
#include <AknIconUtils.h> 
#include <apaidpartner.h>
#include <qpluginloader.h>
#include <eventviewerplugininterface.h>
#include <noteseditorinterface.h>
#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <AknTaskList.h>
#include <apacmdln.h>
#include <xqconversions.h>
#include <apparc.h>
#include <qdesktopservices.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>
#include <hbactivitymanager.h>
#include <xqaiwdecl.h>
#include <xqaiwdeclplat.h>
#include <qservicemanager.h>
#include <qurl.h>
#include <email_services_api.h>
#include <xqrequestinfo.h>
QTM_USE_NAMESPACE
#define hbApp qobject_cast<HbApplication*>(qApp)

const int intial_iteration = 3;
const int batch_iteration = 20;


// ---------------------------------------------------------------------------
// SearchProgressiveState::SearchProgressiveState
// ---------------------------------------------------------------------------
SearchProgressiveState::SearchProgressiveState(QState *parent) :
    QState(parent), mMainWindow(NULL), mView(NULL), mListView(NULL),
            mSearchHandler(NULL), mNotesEditor(0), mAiwMgr(0), mRequest(0)
    {
    mUiLoader = SearchUiLoader::instance();
    connect(mUiLoader, SIGNAL(dbChanged), this, SLOT(slotdbChanged()));

    mMainWindow = hbInstance->allMainWindows().at(0);

    mAiwMgr = new XQApplicationManager;

    mView = mUiLoader->View();
    mListView = mUiLoader->ListWidget();
    mSearchPanel = mUiLoader->SearchPanel();

    HbStyle style;
    qreal x;
    style.parameter("hb-param-graphic-size-primary-large", x);
    QSizeF size(x, x);
    mListViewIconSize = size.toSize();

    if (mSearchPanel)
        {
        mSearchPanel->setPlaceholderText(hbTrId(
                "txt_search_dialog_search_device"));
        }

    constructHandlers();
    if (mView && mMainWindow)
        {
        mMainWindow->addView(mView);
        mMainWindow->setCurrentView(mView);
        }
    mDatabasecount = 0;
    mLinkindex = 0;
    mResultcount = 0;
    mResultparser = 0;
    loadSettings = true;

    if (hbApp)
        {
        connect(hbApp->activityManager(), SIGNAL(activityRequested(QString)),
                this, SLOT(slotactivityRequested(QString)));
        }

#ifdef OST_TRACE_COMPILER_IN_USE 
    //start() the timers to avoid worrying abt having to start()/restart() later
    m_categorySearchApiTime.start();
    m_categorySearchUiTime.start();
    m_totalSearchUiTime.start();
    m_categoryGetDocumentApiTime.start();
    m_getDocumentCatergoryTimeAccumulator = 0;
#endif

    //Notes Editor Interface loading 
    QDir dir(NOTES_EDITOR_PLUGIN_PATH);
    QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

    // Create plugin loader.
    mNotespluginLoader = new QPluginLoader(pluginName);

    if (mNotespluginLoader)
        {
        // Load the plugin.
        mNotespluginLoader->load();

        QObject *plugin = qobject_cast<QObject*> (
                mNotespluginLoader->instance());
        mNotesEditor = qobject_cast<NotesEditorInterface*> (plugin);
        }

    mOnlineQueryAvailable = false;
   
    // creating the handler if it is not prepared, useful when user try to create before 
    mValidateHandlerCreation = false;

    // to retrive the search documents form where it ends when search server crashes
    mContinuationSearch = false;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::~SearchProgressiveState
// ---------------------------------------------------------------------------
SearchProgressiveState::~SearchProgressiveState()
    {
    if(mRequest)
        {
        delete mRequest;
        }
    if (mAiwMgr)
        {
        delete mAiwMgr;
        }    
    for (int i = 0; i < mSearchHandlerList.count(); i++)
        {
        delete mSearchHandlerList.at(i);
        }
    if (mNotespluginLoader)
        {
        mNotespluginLoader->unload();
        delete mNotespluginLoader;
        }
    SearchUiLoader::deleteinstance();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::constructHandlers
// ---------------------------------------------------------------------------
void SearchProgressiveState::constructHandlers()
    {
    for (int i = 0; i < mSearchHandlerList.count(); i++)
        {
        disconnect(mSearchHandlerList.at(i),
                SIGNAL(handleAsyncSearchResult(int,int)), this,
                SLOT(slotonAsyncSearchComplete(int,int)));
        disconnect(mSearchHandlerList.at(i),
                SIGNAL(handleDocument(int,CpixDocument*)), this,
                SLOT(slotonGetDocumentComplete(int,CpixDocument*)));
        disconnect(mSearchHandlerList.at(i),
                SIGNAL(handleBatchDocument(int,int,CpixDocument**)), this,
                SLOT(slotonGetBatchDocumentComplete(int,int,CpixDocument**)));
        delete mSearchHandlerList.at(i);
        }
    mSearchHandlerList.clear();

    for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
        {
        InDeviceHandler* handler = NULL;
        handler = new InDeviceHandler();
        handler->setCategory(mUiLoader->ContentInfoList().at(i)->getBaseApp());
        mSearchHandlerList.append(handler);
        if(mUiLoader->ContentInfoList().at(i)->getTranslationPath().length())
            {
            loadTranslator(mUiLoader->ContentInfoList().at(i)->getTranslationPath());
            }
        }
    for (int i = 0; i < mSearchHandlerList.count(); i++)
        {
        connect(mSearchHandlerList.at(i),
                SIGNAL(handleAsyncSearchResult(int,int)), this,
                SLOT(slotonAsyncSearchComplete(int,int)));
        connect(mSearchHandlerList.at(i),
                SIGNAL(handleDocument(int,CpixDocument*)), this,
                SLOT(slotonGetDocumentComplete(int,CpixDocument*)));
        connect(mSearchHandlerList.at(i),
                SIGNAL(handleBatchDocument(int,int,CpixDocument**)), this,
                SLOT(slotonGetBatchDocumentComplete(int,int,CpixDocument**)));
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::onEntry
// ---------------------------------------------------------------------------
void SearchProgressiveState::onEntry(QEvent *event)
    {
    QState::onEntry(event);
    mStateStatus = true;// used for conditional execution for the slots that are connected as transitions
    if (mSearchPanel)
        {
        mSearchPanel->setPlaceholderText(hbTrId(
                "txt_search_dialog_search_device"));
        mSearchPanel->setProgressive(true);
        }
    if (mListView)
        {
        mListView->setVisible(true);
        }
    activateSignals();
    // If this is not the current view, we're getting back from plugin view  
    if (mMainWindow)
        {
        if (mMainWindow->currentView() != mView)
            {
            mMainWindow->setCurrentView(mView, true);
            }
        mMainWindow->show();
        }
    if (loadSettings)
        {
        // to get the intial settings form delimeter  
        emit switchProToSettingsState();
        loadSettings = false;
        }
    if (mOnlineQueryAvailable)
        {
        mSearchPanel->setCriteria(mOriginalString);
        mOnlineQueryAvailable = false;
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::onExit
// ---------------------------------------------------------------------------
void SearchProgressiveState::onExit(QEvent *event)
    {
    QState::onExit(event);
    mStateStatus = false;
    deActivateSignals();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::activateSignals
// ---------------------------------------------------------------------------
void SearchProgressiveState::activateSignals()
    {
    if (mListView)
        {
        connect(mListView, SIGNAL(activated(HbListWidgetItem *)), this,
                SLOT(slotopenResultitem(HbListWidgetItem *)));
        }
    if (mSearchPanel)
        {
        connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this,
                SLOT(slotstartNewSearch(QString)));
        connect(mSearchPanel, SIGNAL(searchOptionsClicked()), this,
                SLOT(slotsetSettings()));
        }
    connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(slotviewReady()));
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::deActivateSignals
// ---------------------------------------------------------------------------
void SearchProgressiveState::deActivateSignals()
    {
    if (mListView)
        {
        disconnect(mListView, SIGNAL(activated(HbListWidgetItem *)), this,
                SLOT(slotopenResultitem(HbListWidgetItem *)));
        }
    if (mSearchPanel)
        {
        disconnect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this,
                SLOT(slotstartNewSearch(QString)));
        disconnect(mSearchPanel, SIGNAL(searchOptionsClicked()), this,
                SLOT(slotsetSettings()));
        }
    disconnect(mMainWindow, SIGNAL(viewReady()), this, SLOT(slotviewReady()));
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotonAsyncSearchComplete
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotonAsyncSearchComplete(int aError,
        int aResultCount)
    {
    PERF_CAT_API_ENDLOG
    PERF_CAT_HITS_ENDLOG
    if (aError != 0)
        {
        if (aError == KErrServerTerminated) // server terminated refresh the handlers
            {
            constructHandlers();
            mDatabasecount = 0;
            mLinkindex = 0;
            }
        searchOnCategory(mSearchString);
        return;
        }
    if (aResultCount == 0)
        {
        searchOnCategory(mSearchString);
        }
    else if (aResultCount > 0)
        {
        mResultcount = aResultCount;
        if (mContinuationSearch)
            {
            mContinuationSearch = false;
            }
        else
            {
            mResultparser = 0;
            }
        PERF_CAT_GETDOC_TIME_RESTART
        PERF_CAT_GETDOC_ACCUMULATOR_RESET
        if (mListView->count() >= intial_iteration)
            {
            mSearchHandler->getBatchDocumentAsyncAtIndex(mResultparser,
                    batch_iteration);
            }
        else
            {
            mSearchHandler->getDocumentAsyncAtIndex(mResultparser);
            }
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotonGetDocumentComplete
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotonGetDocumentComplete(int aError,
        CpixDocument* aDoc)
    {
    if (aError == KErrServerTerminated) // server terminated refresh the handlers
        {
        constructHandlers();
        mDatabasecount = 0;
        mLinkindex = 0;
        searchOnCategory(mSearchString);
        mContinuationSearch = true;
        return;
        }
    if (aError)
        return;
    parseDocument(aDoc);
    mResultparser++;
    if (mResultparser < mResultcount)
        {
        PERF_CAT_GETDOC_TIME_RESTART
        if (mListView->count() < intial_iteration)
            {
            mSearchHandler->getDocumentAsyncAtIndex(mResultparser);
            }
        else
            {
            mSearchHandler->getBatchDocumentAsyncAtIndex(mResultparser,
                    batch_iteration);
            }
        }
    else
        {
        PERF_CAT_GETDOC_ACCUMULATOR_ENDLOG
        searchOnCategory(mSearchString);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotonGetDocumentComplete
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotonGetBatchDocumentComplete(int aError,
        int aCount, CpixDocument** aDoc)
    {
    if (aError == KErrServerTerminated) // server terminated refresh the handlers
        {
        constructHandlers();
        mDatabasecount = 0;
        mLinkindex = 0;
        searchOnCategory(mSearchString);
        mContinuationSearch = true;
        return;
        }
    if (aError)
        return;
    for (int i = 0; i < aCount; i++)
        {
        parseDocument(aDoc[i]);
        }
    delete aDoc;
    mResultparser += aCount;
    if (mResultparser < mResultcount)
        {
        PERF_CAT_GETDOC_TIME_RESTART
        mSearchHandler->getBatchDocumentAsyncAtIndex(mResultparser,
                batch_iteration);
        }
    else
        {
        PERF_CAT_GETDOC_ACCUMULATOR_ENDLOG
        searchOnCategory(mSearchString);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotopenResultitem
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotopenResultitem(HbListWidgetItem * item)
    {
    if (item == NULL)
        return;
    PERF_RESULT_ITEM_LAUNCH_TIME_RESTART
    QList<QVariant> args;
    bool t;
    if(mRequest)
       delete mRequest;    
    mRequest = NULL;
    if (item->data(Qt::UserRole + 1).toString().contains(CONTACTCATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(CONTACTCATEGORY)
        mRequest = mAiwMgr->create(XQI_CONTACTS_VIEW,XQOP_CONTACTS_VIEW_CONTACT_CARD, true);

        int uid = (item->data(Qt::UserRole)).toInt(&t);
        args << uid;
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(BOOKMARKCATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(BOOKMARKCATEGORY)
        QDesktopServices::openUrl(item->data(Qt::UserRole + 2).toString());
        PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG ("")

        }
    else if (item->data(Qt::UserRole + 1).toString().contains(CALENDARCATEGORY))
        {
        QDir pluginDir = QDir(CALENDAR_EVENTVIEWER_PLUGIN_PATH);
        QPluginLoader *calAgandaViewerPluginLoader = new QPluginLoader(
                pluginDir.absoluteFilePath(CALENDAR_EVENTVIEWER_PLUGIN_NAME));

        calAgandaViewerPluginInstance = qobject_cast<
                EventViewerPluginInterface *> (
                calAgandaViewerPluginLoader->instance());

        connect(calAgandaViewerPluginInstance, SIGNAL(viewingCompleted()),
                this, SLOT(slotviewingCompleted()));

        calAgandaViewerPluginInstance->viewEvent(
                item->data(Qt::UserRole).toInt(),
                EventViewerPluginInterface::ActionEditDelete, NULL);
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(APPLICATIONCATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(APPLICATIONCATEGORY)
        TRAP_IGNORE(LaunchApplicationL((item->data(Qt::UserRole)).toString()));
        PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG("")
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(FILECATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(FILECATEGORY)
        QString uid = item->data(Qt::UserRole).toString();
        QFile file(uid);
        mRequest = mAiwMgr->create(file, true);
        args << file.fileName();
        }
    else if ((item->data(Qt::UserRole + 1).toString().contains(VIDEOCATEGORY))
            || (item->data(Qt::UserRole + 1).toString().contains(AUDIOCATEGORY)))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(AUDIOCATEGORY)
        QString uid = getDrivefromMediaId(
                item->data(Qt::UserRole + 2).toString());
        uid.append(':');
        uid.append(item->data(Qt::UserRole).toString());
        QFile file(uid);
        mRequest = mAiwMgr->create(file, false);
        args << file.fileName();
        if(mRequest)
            {
            QVariant title(hbTrId("txt_search_title_search"));
            XQRequestInfo info;
            info.setInfo(XQINFO_KEY_WINDOW_TITLE, title);
            mRequest->setInfo(info);
            mRequest->setBackground(false);
            mRequest->setSynchronous(false);        
            }
        }
    else if ((item->data(Qt::UserRole + 1).toString().contains(IMAGECATEGORY)))
            {
            PERF_RESULT_ITEM_FOR_LAUNCHING(IMAGECATEGORY)
            QString uid = getDrivefromMediaId(
                    item->data(Qt::UserRole + 2).toString());
            uid.append(':');
            uid.append(item->data(Qt::UserRole).toString());
            QFile file(uid);
            mRequest = mAiwMgr->create(file, true);
            args << file.fileName();
            }    
    else if (item->data(Qt::UserRole + 1).toString().contains(NOTESCATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(NOTESCATEGORY)
        if (mNotesEditor)
            {
            mNotesEditor->edit(item->data(Qt::UserRole).toInt());
            }
        PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG("")
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(EMAILCATEGORY))
        {
        QVariantList  listarg;        
        listarg.append(item->data(Qt::UserRole + 2).toULongLong(&t));
        listarg.append(item->data(Qt::UserRole + 3).toULongLong(&t));
        listarg.append(item->data(Qt::UserRole).toULongLong(&t));
        
        QVariant idListAsVariant =  QVariant::fromValue(listarg);
        
        mRequest = mAiwMgr->create(XQI_EMAIL_MESSAGE_VIEW,XQOP_EMAIL_MESSAGE_VIEW, true);       
        args.append(idListAsVariant);
        args.append(EmailNoFlags);
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(MESSAGECATEGORY))
        {
        PERF_RESULT_ITEM_FOR_LAUNCHING(MESSAGECATEGORY)
        mRequest = mAiwMgr->create(XQI_MESSAGE_VIEW, XQOP_MESSAGE_VIEW, true);

        int uid = (item->data(Qt::UserRole)).toInt(&t);
        args << uid;
        }
    else if (item->data(Qt::UserRole + 1).toString().contains(SUGGESTIONLINKS))
        {
        emit launchLink((item->data(Qt::UserRole)).toInt(&t), mOriginalString);
        }
    if (mRequest)
        {
        connect(mRequest, SIGNAL(requestOk(const QVariant&)), this,
                SLOT(slothandleOk(const QVariant&)));
        connect(mRequest, SIGNAL(requestError(int,const QString&)), this,
                SLOT(slothandleError(int,const QString&)));// Connect error handling signal or apply lastError function instead.
        mRequest->setArguments(args);
        if (!mRequest->send())// Make the request

            {
            qDebug() << "AIW-ERROR: Send failed" << mRequest->lastError();
            }
        disconnect(mRequest, SIGNAL(requestOk(const QVariant&)), this,
                SLOT(slothandleOk(const QVariant&)));
        disconnect(mRequest, SIGNAL(requestError(int,const QString&)), this,
                SLOT(slothandleError(int,const QString&)));// Connect error handling signal or apply lastError function instead.
        }
    else
        {
        qDebug() << "AIW-ERROR: NULL request";
        return;
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slothandleOk
// ---------------------------------------------------------------------------
void SearchProgressiveState::slothandleOk(const QVariant& var)
    {
    Q_UNUSED(var);
    PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG ("")
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slothandleError
// ---------------------------------------------------------------------------
void SearchProgressiveState::slothandleError(int ret, const QString& var)
    {
    Q_UNUSED(ret);
    Q_UNUSED(var);
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::createSuggestionLink
// ---------------------------------------------------------------------------
void SearchProgressiveState::createSuggestionLink()
    {
    QString linkString = QString(hbTrId("txt_search_list_search_for_1").arg(
            mOriginalString));
    QMapIterator<int, HbIcon> i(mISprovidersIcon);
    while (i.hasNext())
        {
        i.next();
        HbListWidgetItem* listitem = new HbListWidgetItem();
        listitem->setText(linkString);
        listitem->setData(i.key(), Qt::UserRole);
        listitem->setData(i.value(), Qt::DecorationRole);
        listitem->setData(SUGGESTIONLINKS, Qt::UserRole + 1);
        mListView->addItem(listitem);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::noResultsFound
// ---------------------------------------------------------------------------
void SearchProgressiveState::noResultsFound(QString aKeyword)
    {
    if (aKeyword.length())
        {
        HbListWidgetItem* listitem = new HbListWidgetItem();
        QString noResultMsg = QString("<align=\"center\">" + hbTrId(
                "txt_search_list_no_match_found"));
        listitem->setText(noResultMsg);
        mListView->addItem(listitem);

        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::clear
// ---------------------------------------------------------------------------
void SearchProgressiveState::clear()
    {
    mListView->clear();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::searchOnCategory
// ---------------------------------------------------------------------------
void SearchProgressiveState::searchOnCategory(const QString aKeyword)
    {
#ifdef OST_TRACE_COMPILER_IN_USE
    if (mDatabasecount != 0)
        {//Search just started.
        PERF_CAT_UI_ENDLOG
        }
    PERF_CAT_UI_TIME_RESTART
#endif
    mResultparser = 0;
    mResultcount = 0;
    mSearchHandler = NULL;
    if (mDatabasecount < mUiLoader->ContentInfoList().count())
        {
        for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
            {
            //skip all categories if "selectall" option get selected, skip other iterations             
            if (i == 0 && mUiLoader->ContentInfoList().at(i)->getSelected())
                {
                mSearchHandler = mSearchHandlerList.at(i);
                mDatabasecount = mUiLoader->ContentInfoList().count();
                break;
                }
            if (mUiLoader->ContentInfoList().at(i)->getSelected()
                    && (!mUiLoader->ContentInfoList().at(i)->getSearchProgress()))
                {             
                mSearchHandler = mSearchHandlerList.at(i);
                mUiLoader->ContentInfoList().at(i)->setSearchProgress(true);
                break;
                }            
            }
        mDatabasecount++;
        if (mSearchHandler != NULL && mSearchHandler->isPrepared())
            {
            PERF_CAT_API_TIME_RESTART
            mSearchHandler->searchAsync(aKeyword, DEFAULT_SEARCH_FIELD);

            }
        else
            {
            searchOnCategory(mSearchString);
            }
        }
    else
        {
        PERF_TOTAL_UI_ENDLOG
        if (mListView->count() == 0 && aKeyword.length() != 0)
            {
            noResultsFound(mOriginalString);
            createSuggestionLink();
            }
        return;
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotstartNewSearch
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotstartNewSearch(const QString &aKeyword)
    {
    PERF_CAT_TOTAL_TIME_RESTART
    mOriginalString = aKeyword.trimmed();

    for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
        {
        mUiLoader->ContentInfoList().at(j)->setSearchProgress(false);
        }
    for (int i = 0; i < mSearchHandlerList.count(); i++)
        {
        mSearchHandlerList.at(i)->cancelLastSearch();
        }
    if (mListView->count() != 0)
        {
        clear();
        }
    if (mOriginalString.length())
        {
        mDatabasecount = 0;
        mLinkindex = 0;
        mSearchString = "$prefix(\"";
        mSearchString += mOriginalString;
        mSearchString += "\")";
        searchOnCategory(mSearchString);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotsetSettings
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotsetSettings()
    {
    for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
        {
        mSelectedCategory.insert(j,
                mUiLoader->ContentInfoList().at(j)->getSelected());
        }
    if (mOriginalString.length())
        emit inDeviceSearchQuery(mOriginalString);
    emit switchProToSettingsState();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotsettingsaction
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotsettingsaction(bool avalue)
    {
    if (avalue && mStateStatus)
        {
        QMap<int, bool> mTempSelectedCategory;
        for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
            {
            mTempSelectedCategory.insert(i,
                    mUiLoader->ContentInfoList().at(i)->getSelected());
            }
        QMapIterator<int, bool> j(mTempSelectedCategory);
        QMapIterator<int, bool> k(mSelectedCategory);
        while (j.hasNext())
            {
            j.next();
            k.next();
            if (j.value() != k.value())
                {
                slotstartNewSearch(mOriginalString);
                break;
                }
            }
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::getAppIconFromAppId
// ---------------------------------------------------------------------------
HbIcon SearchProgressiveState::getAppIconFromAppIdL(TUid auid)
    {
    HbIcon icon;
    RApaLsSession apaLsSession;
    CleanupClosePushL(apaLsSession);
    User::LeaveIfError(apaLsSession.Connect());
    CApaAppServiceInfoArray* skinArray(NULL);
    TRAPD( err, skinArray = apaLsSession.GetAppServiceOpaqueDataLC(auid, TUid::Uid(0x2002DCF3));
            if (err == KErrNone && skinArray )
                {
                TArray<TApaAppServiceInfo> tmpArray( skinArray->Array() );
                if ( tmpArray.Count() )
                    {
                    TPtrC8 opaque(tmpArray[0].OpaqueData());
                    const TPtrC16 iconName((TText16*) opaque.Ptr(),(opaque.Length()+1)>>1);
                    icon = HbIcon( XQConversions:: s60DescToQString( iconName ) );
                    }
                }
            CleanupStack::PopAndDestroy(skinArray);
    );
    if (icon.isNull() || !(icon.size().isValid()))
        {
        TSize iconSize(mListViewIconSize.width(), mListViewIconSize.height());
        CApaMaskedBitmap* apaMaskedBitmap = CApaMaskedBitmap::NewLC();
        TInt err = apaLsSession.GetAppIcon(auid, iconSize, *apaMaskedBitmap);
        TInt iconsCount(0);
        apaLsSession.NumberOfOwnDefinedIcons(auid, iconsCount);
        QPixmap pixmap;
        if ((err == KErrNone) && (iconsCount > 0))
            {
            fromBitmapAndMaskToPixmapL(apaMaskedBitmap,
                    apaMaskedBitmap->Mask(), pixmap);
            pixmap = pixmap.scaled(mListViewIconSize,
                    Qt::KeepAspectRatioByExpanding);
            icon = HbIcon(QIcon(pixmap));
            }
        else
            {
            HBufC* fileNameFromApparc;
            TInt err2 = apaLsSession.GetAppIcon(auid, fileNameFromApparc);
            CleanupStack::PushL(fileNameFromApparc);
            if (err2 == KErrNone)
                {
                QString fileName = XQConversions::s60DescToQString(
                        fileNameFromApparc->Des());
                if (fileName.contains(QString(".mif")))
                    {
                    TPtr ptr(fileNameFromApparc->Des());
                    GetPixmapByFilenameL(ptr, mListViewIconSize, pixmap);
                    pixmap = pixmap.scaled(mListViewIconSize,
                            Qt::KeepAspectRatioByExpanding);
                    icon = HbIcon(QIcon(pixmap));
                    }
                }
            CleanupStack::Pop(fileNameFromApparc);
            }
        CleanupStack::PopAndDestroy(apaMaskedBitmap);
        }
    CleanupStack::PopAndDestroy(&apaLsSession);
    if (icon.isNull() || !(icon.size().isValid()))
        icon = HbIcon("qtg_large_application");
    return icon;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::GetPixmapByFilenameL
// ---------------------------------------------------------------------------
void SearchProgressiveState::GetPixmapByFilenameL(TDesC& fileName,
        const QSize &size, QPixmap& pixmap)
    {
    CFbsBitmap *bitamp(0);
    CFbsBitmap *mask(0);
    if (AknIconUtils::IsMifFile(fileName))
        {
        // SVG icon
        // SVG always has only one icon
        TInt bitmapIndex = 0;
        TInt maskIndex = 1;
        AknIconUtils::ValidateLogicalAppIconId(fileName, bitmapIndex,
                maskIndex);

        AknIconUtils::CreateIconLC(bitamp, mask, fileName, bitmapIndex,
                maskIndex);
        }
    AknIconUtils::DisableCompression(bitamp);
    AknIconUtils::SetSize(bitamp, TSize(size.width(), size.height()),
            EAspectRatioPreservedAndUnusedSpaceRemoved);
    AknIconUtils::DisableCompression(mask);
    AknIconUtils::SetSize(mask, TSize(size.width(), size.height()),
            EAspectRatioPreservedAndUnusedSpaceRemoved);
    fromBitmapAndMaskToPixmapL(bitamp, mask, pixmap);
    // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
    CleanupStack::Pop(2);
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::TDisplayMode2Format
// ---------------------------------------------------------------------------
QImage::Format SearchProgressiveState::TDisplayMode2Format(TDisplayMode mode)
    {
    QImage::Format format;
    switch (mode)
        {
        case EGray2:
            format = QImage::Format_MonoLSB;
            break;
        case EColor256:
        case EGray256:
            format = QImage::Format_Indexed8;
            break;
        case EColor4K:
            format = QImage::Format_RGB444;
            break;
        case EColor64K:
            format = QImage::Format_RGB16;
            break;
        case EColor16M:
            format = QImage::Format_RGB888;
            break;
        case EColor16MU:
            format = QImage::Format_RGB32;
            break;
        case EColor16MA:
            format = QImage::Format_ARGB32;
            break;
        case EColor16MAP:
            format = QImage::Format_ARGB32_Premultiplied;
            break;
        default:
            format = QImage::Format_Invalid;
            break;
        }
    return format;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::fromSymbianCFbsBitmap
// ---------------------------------------------------------------------------
QPixmap SearchProgressiveState::fromSymbianCFbsBitmap(CFbsBitmap *aBitmap)
    {
    aBitmap->BeginDataAccess();
    uchar *data = (uchar *) aBitmap->DataAddress();
    TSize size = aBitmap->SizeInPixels();
    TDisplayMode displayMode = aBitmap->DisplayMode();
    QImage image(data, size.iWidth, size.iHeight, TDisplayMode2Format(
            displayMode));// QImage format must match to bitmap format
    aBitmap->EndDataAccess();
    // No data copying happens because image format matches native OpenVG format.
    // So QPixmap actually points to CFbsBitmap data.
    return QPixmap::fromImage(image);
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::copyBitmapLC
// ---------------------------------------------------------------------------
CFbsBitmap *SearchProgressiveState::copyBitmapLC(CFbsBitmap *input)
    {
    CFbsBitmap *bmp = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);
    bmp->Create(input->SizeInPixels(), input->DisplayMode());
    CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(bmp);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc *bmpGc;
    bitmapDevice->CreateContext(bmpGc);
    bmpGc->BitBlt(TPoint(0, 0), input);
    delete bmpGc;
    CleanupStack::PopAndDestroy(bitmapDevice);
    return bmp;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::fromBitmapAndMaskToPixmapL
// ---------------------------------------------------------------------------
void SearchProgressiveState::fromBitmapAndMaskToPixmapL(
        CFbsBitmap* fbsBitmap, CFbsBitmap* fbsMask, QPixmap& pixmap)
    {
    if (fbsBitmap->Header().iCompression == ENoBitmapCompression)
        {
        pixmap = fromSymbianCFbsBitmap(fbsBitmap);
        QPixmap mask = fromSymbianCFbsBitmap(fbsMask);
        pixmap.setAlphaChannel(mask);
        }
    else
        {
        CFbsBitmap *temp(NULL);
        temp = copyBitmapLC(fbsBitmap);
        pixmap = fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        temp = copyBitmapLC(fbsMask);
        QPixmap mask = fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        pixmap.setAlphaChannel(mask);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::filterDoc
// ---------------------------------------------------------------------------
QString SearchProgressiveState::filterDoc(const CpixDocument* aDoc,
        const QString& filter)
    {
    for (int i = 0; i < aDoc->fieldCount(); i++)
        {
        if (aDoc->field(i).name().contains(filter))
            {
            return aDoc->field(i).value();
            }
        }
    return NULL;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::filterDoc
// ---------------------------------------------------------------------------
QStringList SearchProgressiveState::filterDoc(const CpixDocument* aDoc,
        const QString& filter1, const QString& filter2,
        const QString& filter3)
    {
    QStringList docList;
    docList.append(QString());
    docList.append(QString());
    docList.append(QString());
    bool bfilter1 = false;
    bool bfilter2 = false;
    bool bfilter3 = false;
    if (!filter3.length())
        {
        bfilter3 = true;
        }
    for (int i = 0; i < aDoc->fieldCount(); i++)
        {
        if (!bfilter1 && aDoc->field(i).name().contains(filter1))
            {
            docList.replace(0, aDoc->field(i).value());
            bfilter1 = true;
            }
        if (!bfilter2 && aDoc->field(i).name().contains(filter2))
            {
            docList.replace(1, aDoc->field(i).value());
            bfilter2 = true;
            }
        if (!bfilter3 && aDoc->field(i).name().contains(filter3))
            {
            docList.replace(2, aDoc->field(i).value());
            bfilter3 = true;
            }
        if (bfilter1 && bfilter2 && bfilter3)
            break;
        }
    return docList;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::getDrivefromMediaId
// ---------------------------------------------------------------------------
QString SearchProgressiveState::getDrivefromMediaId(QString aMediaId)
    {
    TBuf<15> mediaIdBuf(aMediaId.utf16());
    if (KErrNone == iFs.Connect())
        {
        TUint mediaNum;
        TVolumeInfo vmInfo;
        TChar driveLetter;
        TLex lex(mediaIdBuf);
        lex.Val(mediaNum);
        TDriveNumber drive = TDriveNumber(KErrNotSupported);

        for (TInt i = 0; i <= EDriveZ; i++)
            {
            TInt err = iFs.Volume(vmInfo, i);// Volume() returns KErrNotReady if no volume present.       
            if (err != KErrNotReady)// In this case, check next drive number
                {
                if (vmInfo.iUniqueID == mediaNum)
                    {
                    drive = TDriveNumber(i);//Is the drive
                    if (KErrNone == iFs.DriveToChar(drive, driveLetter))
                        {
                        mediaIdBuf.Zero();
                        mediaIdBuf.Append(driveLetter);
                        mediaIdBuf.LowerCase();
                        }
                    break;
                    }
                }
            }
        iFs.Close();
        }
    return QString::fromUtf16(mediaIdBuf.Ptr(), mediaIdBuf.Length());
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::LaunchApplicationL
// ---------------------------------------------------------------------------
void SearchProgressiveState::LaunchApplicationL(const QString aUid)
    {  
    if(aUid.contains(SEARCHAPPUID,Qt::CaseInsensitive))
        {       
        return;
        }
    QServiceManager serviceManager;
    QObject* mActivityManager = serviceManager.loadInterface(
            "com.nokia.qt.activities.ActivityManager");
    if (!mActivityManager)
        {
        return;
        }
    QUrl url;
    url.setScheme(XQURI_SCHEME_ACTIVITY);
    QString str("0x");
    str.append(aUid);
    url.setHost(str);
    QMetaObject::invokeMethod(mActivityManager, "launchActivity",
            Q_ARG(QUrl, url));
    delete mActivityManager;
    PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG("")
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotviewingCompleted
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotviewingCompleted()
    {
    if (calAgandaViewerPluginInstance)
        calAgandaViewerPluginInstance->deleteLater();
    PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG ("")
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotviewReady
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotviewReady()
    {
    if (hbApp)
        {
        if (hbApp->activateReason() == Hb::ActivationReasonActivity)
            {
            QVariantHash params = hbApp->activateParams();
            QString searchKey = params.value(SEARCHAIWQUERY).toString();
            params.remove(SEARCHAIWQUERY);
            params.remove(XQURI_KEY_ACTIVITY_NAME);
            QList<QVariant> list = params.values();
            for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
                {
                mUiLoader->ContentInfoList().at(i)->setSelected(false);
                }
            for (int i = 0; i < list.count(); i++)
                {
                QString str = list.at(i).toString();
                if (!str.isNull())
                    {
                    for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
                        {
                        if (mUiLoader->ContentInfoList().at(i)->getBaseApp().contains(
                                str))
                            mUiLoader->ContentInfoList().at(i)->setSelected(
                                    true);
                        }
                    }
                }
            if (searchKey.length() > 0)
                mSearchPanel->setCriteria(searchKey);
            }
        }
    PERF_APP_LAUNCH_END("SearchAppplication View is ready");
    emit applicationReady();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotOnlineQuery
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotOnlineQuery(QString str)
    {
    if (mOriginalString != str)
        {
        mOriginalString = str;
        mOnlineQueryAvailable = true;
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotISProvidersIcon
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotISProvidersIcon(int id, HbIcon icon)
    {
    mISprovidersIcon.insert(id, icon);
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotactivityRequested
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotactivityRequested(const QString &name)
    {
    if (name == SEARCHAIWDECLINDEVICE)
        {
        QVariantHash params = hbApp->activateParams();
        QString searchKey = params.value(SEARCHAIWQUERY).toString();
        int i = params.count();
        params.remove(SEARCHAIWQUERY);
        params.remove(XQURI_KEY_ACTIVITY_NAME);
        QList<QVariant> list = params.values();
        for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
            {
            mUiLoader->ContentInfoList().at(i)->setSelected(false);
            }
        for (int i = 0; i < list.count(); i++)
            {
            QString str = list.at(i).toString();
            if (!str.isNull())
                {
                for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
                    {
                    if (mUiLoader->ContentInfoList().at(i)->getBaseApp().contains(
                            str))
                        mUiLoader->ContentInfoList().at(i)->setSelected(true);
                    }
                }
            }
        if (searchKey.length() > 0)
            mSearchPanel->setCriteria(searchKey);
        }
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::parseDocument
// ---------------------------------------------------------------------------
void SearchProgressiveState::parseDocument(CpixDocument* aDoc)
    {
    PERF_CAT_GETDOC_TIME_ACCUMULATE
    if (aDoc == NULL)
        return;
    QString secondrow = aDoc->excerpt();
    QString firstrow;
    HbListWidgetItem* listitem = new HbListWidgetItem();
    bool addtoList = true;

    if (aDoc->baseAppClass().contains(FILEFOLDERCATEGORY))
        {
        bool ok;
        QStringList fileList = filterDoc(aDoc, NAMEFIELD, ISFOLDERFIELD,
                EXTENSIONFIELD);
        firstrow = fileList.at(0);
        if (fileList.at(1).toInt(&ok) == 1) // not to show folder results 
            {
            addtoList = false;
            }
        else
            {
            if (fileList.at(2).contains("sis", Qt::CaseInsensitive)
                    || fileList.at(1).contains("sisx", Qt::CaseInsensitive))
                {
                listitem->setData(HbIcon("qtg_large_sisx"), Qt::DecorationRole);
                }
            else if (fileList.at(2).contains("java", Qt::CaseInsensitive)
                    || fileList.at(2).contains("jar", Qt::CaseInsensitive)
                    || fileList.at(2).contains("jad", Qt::CaseInsensitive))
                {
                listitem->setData(HbIcon("qtg_large_java"), Qt::DecorationRole);
                }
            else if (fileList.at(2).contains("swf", Qt::CaseInsensitive))
                {
                listitem->setData(HbIcon("qtg_large_flash"), Qt::DecorationRole);
                }
            else
                {
                listitem->setData(HbIcon("qtg_large_query"), Qt::DecorationRole);
                }
            }
        }
    else if (aDoc->baseAppClass().contains(FILECATEGORY))
        {
        QStringList fileList = filterDoc(aDoc, NAMEFIELD, EXTENSIONFIELD);
        firstrow = fileList.at(0);
        if (firstrow.length() == 0)
            firstrow = aDoc->baseAppClass();
        if (fileList.at(1).contains("txt", Qt::CaseInsensitive))
            {
            listitem->setData(HbIcon("qtg_large_text"), Qt::DecorationRole);
            }
        else
            {
            listitem->setData(HbIcon("qtg_large_query"), Qt::DecorationRole);
            }
        }
    else
        {
        if (aDoc->baseAppClass().contains(MEDIACATEGORY))
            {
            QStringList medialist = filterDoc(aDoc, FIRSTLINEFIELD, MEDIAIDFIELD);
            firstrow = medialist.at(0);
            listitem->setData(medialist.at(1), Qt::UserRole + 2);
            }
        else if (aDoc->baseAppClass().contains(EMAILCATEGORY))
            {
            QStringList emaillist = filterDoc(aDoc, FIRSTLINEFIELD,
                    MAILBOXFIELD, FOLDERFIELD);
            firstrow = emaillist.at(0);
            listitem->setData(emaillist.at(1), Qt::UserRole + 2);
            listitem->setData(emaillist.at(2), Qt::UserRole + 3);
            }
        else
            {
            firstrow = filterDoc(aDoc, FIRSTLINEFIELD);
            }                
        for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
            {
            if (aDoc->baseAppClass()
                    == mUiLoader->ContentInfoList().at(i)->getBaseApp())
                {
                if (!firstrow.length())
                    {
                    firstrow = QString(hbTrId(mUiLoader->ContentInfoList().at(i)->getExceptionString().toAscii()));
                    }                
                if(mUiLoader->ContentInfoList().at(i)->getDisplayIcon().length())
                    {
                    listitem->setData(HbIcon(mUiLoader->ContentInfoList().at(i)->getDisplayIcon()), Qt::DecorationRole);
                    }
                else
                    {
                    bool ok;
                    TRAP_IGNORE(listitem->setData(getAppIconFromAppIdL(TUid::Uid(aDoc->docId().toUInt(&ok, 16))), Qt::DecorationRole));
                    }
                }
            }
        }
    listitem->setText(firstrow);
    listitem->setSecondaryText(secondrow);
    listitem->setData(aDoc->docId(), Qt::UserRole);
    listitem->setData(aDoc->baseAppClass(), Qt::UserRole + 1);
    if (addtoList)
        mListView->addItem(listitem);
    else
        delete listitem;

    delete aDoc;
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::slotdbChanged
// ---------------------------------------------------------------------------
void SearchProgressiveState::slotdbChanged()
    {
    constructHandlers();
    }
// ---------------------------------------------------------------------------
// SearchProgressiveState::loadTranslator
// ---------------------------------------------------------------------------
void SearchProgressiveState::loadTranslator(QString localizationpath)
    {    
    QTranslator* translator = new QTranslator();
    QString lang = QLocale::system().name();
    
    int pos = localizationpath.lastIndexOf("/");
    
    QString path = localizationpath.left(pos);
    QString filename = localizationpath.right(pos);

    translator->load(filename + lang, path);

    QCoreApplication::installTranslator(translator);
    }
