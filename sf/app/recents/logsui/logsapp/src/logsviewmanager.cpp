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
* Description:
*
*/
//USER
#include "logsviewmanager.h"
#include "logscomponentrepository.h"
#include "logsrecentcallsview.h"
#include "logsmatchesview.h"
#include "logsdetailsview.h"
#include "logsfilter.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logsservicehandler.h"
#include "logsservicehandlerold.h"
#include "logsmainwindow.h"
#include "logsappsettings.h"
#include "logsforegroundwatcher.h"

//SYSTEM
#include <hbmainwindow.h>
#include <hbview.h>
#include <QApplication>
#include <hblineedit.h>
#include <dialpad.h>
#include <hbactivitymanager.h>
#include <hbapplication.h>
#include <tstasksettings.h>

// -----------------------------------------------------------------------------
// LogsViewManager::LogsViewManager
// -----------------------------------------------------------------------------
//
LogsViewManager::LogsViewManager( 
        LogsMainWindow& mainWindow, LogsServiceHandler& service,
        LogsServiceHandlerOld& serviceOld,
        LogsAppSettings& settings) : 
    QObject( 0 ), mMainWindow( mainWindow ), 
    mService( service ), mServiceOld( serviceOld ), mSettings( settings ),
    mFirstActivation(true), mViewActivationShowDialpad(false), 
    mBackgroundStartupWatcher(0)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::LogsViewManager()" );

    //It is important that we always handle orientation change first, before
    //dialpad widget. If connection is moved to a view, then it's not guarantied.
    connect( &mainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(handleOrientationChanged()) );
    connect( &mainWindow, SIGNAL(appGainedForeground()), this, SLOT(appGainedForeground()) );
    connect( &mainWindow, SIGNAL(appLostForeground()), this, SLOT(appLostForeground()) );

    mComponentsRepository = new LogsComponentRepository(*this);
    
    connect( &mService, SIGNAL( activateView(XQService::LogsViewIndex, bool, QString) ), 
             this, SLOT( changeRecentViewViaService(XQService::LogsViewIndex, bool, QString) ) );

    connect( &mService, SIGNAL( activateView(QString) ), 
             this, SLOT( changeMatchesViewViaService(QString) ));

    connect( &mServiceOld, SIGNAL( activateView(XQService::LogsViewIndex, bool, QString) ), 
             this, SLOT( changeRecentViewViaService(XQService::LogsViewIndex, bool, QString) ) );

    connect( &mServiceOld, SIGNAL( activateView(QString) ), 
             this, SLOT( changeMatchesViewViaService(QString) ));
    
    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));

    handleFirstActivation();
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::LogsViewManager()" );
}


// -----------------------------------------------------------------------------
// LogsViewManager::~LogsViewManager
// -----------------------------------------------------------------------------
//
LogsViewManager::~LogsViewManager()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::~LogsViewManager()" );
     
    delete mComponentsRepository;
    delete mBackgroundStartupWatcher;
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::~LogsViewManager()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::changeRecentViewViaService
// -----------------------------------------------------------------------------
//
void LogsViewManager::changeRecentViewViaService(
    XQService::LogsViewIndex view, bool showDialpad, QString dialpadText)
{
    QVariant args(view);
    activateViewViaService(LogsRecentViewId, showDialpad, dialpadText, args);
}

// -----------------------------------------------------------------------------
// LogsViewManager::changeMatchesViewViaService
// -----------------------------------------------------------------------------
//
void LogsViewManager::changeMatchesViewViaService(QString dialpadText)
{
    activateViewViaService(LogsMatchesViewId, true, dialpadText);
}

// -----------------------------------------------------------------------------
// LogsViewManager::changeRecentView
// -----------------------------------------------------------------------------
//
void LogsViewManager::changeRecentView(
    XQService::LogsViewIndex view, bool showDialpad)
{
    QVariant args(view);
    doActivateView(LogsRecentViewId, showDialpad, args);
}

// -----------------------------------------------------------------------------
// LogsViewManager::proceedExit
// -----------------------------------------------------------------------------
//
void LogsViewManager::proceedExit()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::proceedExit()" );
    
    if ( mSettings.logsFeatureFakeExitEnabled() ){
        doFakeExit();
    } else {
        qApp->quit();
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::proceedExit()" );
}


// -----------------------------------------------------------------------------
// LogsViewManager::activateView
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activateView(LogsAppViewId viewId)
{
    return doActivateView(viewId, false, QVariant());
}

// -----------------------------------------------------------------------------
// LogsViewManager::activateView
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activateView(
        LogsAppViewId viewId, bool showDialpad, QVariant args)
{
    return doActivateView(viewId, showDialpad, args);
}

// -----------------------------------------------------------------------------
// LogsViewManager::mainWindow
// -----------------------------------------------------------------------------
//
HbMainWindow& LogsViewManager::mainWindow()
{
    return mMainWindow;
}

// -----------------------------------------------------------------------------
// LogsViewManager::exitApplication
// -----------------------------------------------------------------------------
//
void LogsViewManager::exitApplication()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::exitApplication()" );
    
    // Send app immetiately to background as there might be some async stuff
    // that needs to complete before the process can be terminated.
    mMainWindow.sendAppToBackground();

    doExitApplication();
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::exitApplication()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::activatePreviousView
// Previously activated view is at slot 1 of view stack
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activatePreviousView()
{
    LogsAppViewId viewId = ( mViewStack.count() > 1 ) ? 
        mViewStack.at(1)->viewId() : LogsRecentViewId;
    return doActivateView(viewId, false, QVariant());
}

// -----------------------------------------------------------------------------
// LogsViewManager::doActivateView
// Currently activated view is always at beginning of view stack. Deactivates
// previously activated view and activates new view and place it at top of
// view stack.
// -----------------------------------------------------------------------------
//
bool LogsViewManager::doActivateView(
        LogsAppViewId viewId, bool showDialpad, 
        QVariant args, const QString& dialpadText, bool reset)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::doActivateView()" );
    
    bool activated(false);
    LogsBaseView* newView = 0; 
    LogsBaseView* oldView = mViewStack.count() > 0 ? mViewStack.at(0) : 0;
    
    viewId = checkMatchesViewTransition(viewId, dialpadText);
    
    for ( int i = 0; i < mViewStack.count(); ++i ){
        if ( mViewStack.at(i)->viewId() == viewId ){
            newView = mViewStack.takeAt(i);
        }
    }
    
    if ( !newView ){
        newView = createView(viewId);
    }
    
    if ( oldView && newView && oldView != newView ){   
        oldView->deactivated();
        disconnect( &mMainWindow, SIGNAL(callKeyPressed()), oldView, SLOT(callKeyPressed()) );
        disconnect( &mMainWindow, SIGNAL(localeChanged()), oldView, SLOT(localeChanged()) );
    }
    
    if ( newView ){
        mViewStack.insert(0, newView);
        if ( reset ){
            newView->resetView();
        }
        mMainWindow.setCurrentView(newView);
        //Setting mainwindow interactive to enable user input during
        //view switching animation.
        mMainWindow.setInteractive(true);
        mViewActivationArgs = args;
        mViewActivationShowDialpad = showDialpad;
        completeViewActivation();
        activated = true;
    }

    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::doActivateView()" );
    
    return activated;
}

// -----------------------------------------------------------------------------
// LogsViewManager::completeViewActivation
// -----------------------------------------------------------------------------
//
void LogsViewManager::completeViewActivation()
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsViewManager::completeViewActivation(), first", 
                   mFirstActivation );
    if ( mFirstActivation ){
        mFirstActivation = false;
        connect( &mMainWindow, SIGNAL(viewReady()), 
                 this, SLOT(completeViewActivation()), 
                 Qt::UniqueConnection );
    } else {
        disconnect( &mMainWindow, SIGNAL(viewReady()), this, SLOT(completeViewActivation()) );
        LogsBaseView* newView = mViewStack.at(0);
        newView->activated(mViewActivationShowDialpad, mViewActivationArgs);
        connect( &mMainWindow, SIGNAL(callKeyPressed()), 
                 newView, SLOT(callKeyPressed()),
                 Qt::UniqueConnection );
        connect( &mMainWindow, SIGNAL(localeChanged()), 
                 newView, SLOT(localeChanged()),
                 Qt::UniqueConnection );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::completeViewActivation()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::handleOrientationChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::handleOrientationChanged()" );
    QMetaObject::invokeMethod(mMainWindow.currentView(), "handleOrientationChanged");
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::handleOrientationChanged()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::saveActivity()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::saveActivity()" );
    
    if ( mViewStack.count() == 0 ){
        return;
    }
    
    clearActivities();
    
    HbActivityManager* activityManager = static_cast<HbApplication*>(qApp)->activityManager();
    QVariantHash metaData;
    LOGS_QDEBUG( "logs [UI] Start taking screenshot" );
    QImage* img = new QImage(mMainWindow.rect().size(), QImage::Format_ARGB32_Premultiplied);
    QPainter p( img );
    // Use render instead of QPixmap::grabWidget as otherwise screenshot
    // may become empty.
    mMainWindow.render( &p, mMainWindow.rect(), mMainWindow.rect() );
    metaData.insert("screenshot", QPixmap::fromImage(*img));
    delete img;
    LOGS_QDEBUG( "logs [UI] End taking screenshot" );
    
    QByteArray serializedActivity;
    QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
    
    metaData.insert( 
        logsActivityParamInternalViewId, mViewStack.at(0)->viewId() );
    metaData.insert( 
        logsActivityParamShowDialpad, mComponentsRepository->dialpad()->isOpen() );
    metaData.insert( 
        logsActivityParamDialpadText, mComponentsRepository->dialpad()->editor().text() );
    QString activityId = mViewStack.at(0)->saveActivity(stream, metaData);
    
    // add the activity to the activity manager
    bool ok = activityManager->addActivity(activityId, serializedActivity, metaData);
    if ( !ok ){
        LOGS_QDEBUG( "logs [UI] activity adding failed" );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::saveActivity()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsViewManager::loadActivity()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::loadActivity()" );
    HbApplication* hbApp = static_cast<HbApplication*>(qApp);
    QString activityId = hbApp->activateId();
    return doLoadActivity(activityId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAppViewId LogsViewManager::checkMatchesViewTransition(
    LogsAppViewId viewId, const QString& dialpadText)
{
    if ( !dialpadText.isEmpty() ){
        mComponentsRepository->dialpad()->editor().setText(dialpadText);
    }
    
    if ( viewId == LogsMatchesViewId ){
        LogsModel* model = mComponentsRepository->model();
        if ( model && model->predictiveSearchStatus() != logsContactSearchEnabled ){
            LOGS_QDEBUG( "logs [UI]     contact search disabled, go to recent view" );
            viewId = LogsRecentViewId;
        }
    }
    return viewId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::handleFirstActivation()
{      
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::handleFirstActivation()" );

    if ( mSettings.logsFeaturePreloadingEnabled() ) {
        delete mBackgroundStartupWatcher;
        mBackgroundStartupWatcher = 0;
        mBackgroundStartupWatcher = new LogsForegroundWatcher();
        connect( mBackgroundStartupWatcher, SIGNAL(gainingForeground()), 
                 this, SLOT(bgStartupForegroundGained()) );
        setTaskSwitcherVisibility(false);

    } else {
        Hb::ActivationReason reason = static_cast<HbApplication*>(qApp)->activateReason();
        bool isStartedByService( 
            mService.isStartedUsingService() || mServiceOld.isStartedUsingService() );
        
        // Start immediately using all possible resources
        mComponentsRepository->model()->refreshData();
        
        if ( reason == Hb::ActivationReasonActivity && loadActivity() ){
            LOGS_QDEBUG( "logs [UI] loaded saved activity" );    
            mMainWindow.bringAppToForeground();
        } else if ( mFirstActivation && !isStartedByService ) {
            activateDefaultView();
            mMainWindow.bringAppToForeground();
        }
        
        clearActivities();
    }

    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::handleFirstActivation()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsBaseView* LogsViewManager::createView(LogsAppViewId viewId)
{
    LogsBaseView* newView = 0;
    if ( viewId == LogsRecentViewId ){
        newView = mComponentsRepository->recentCallsView();
        mMainWindow.addView(newView);
    } else if ( viewId == LogsMatchesViewId ) {
        newView = mComponentsRepository->matchesView();
        mMainWindow.addView(newView);
    } else if ( viewId == LogsDetailsViewId ) {
        newView = mComponentsRepository->detailsView();
        mMainWindow.addView(newView);
    }
    return newView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::closeEmbeddedApplication()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::closeEmbeddedApplication()" );
    if (mViewStack.count()) {
        mViewStack.at(0)->cancelServiceRequest();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::closeEmbeddedApplication()" );
}

// -----------------------------------------------------------------------------
// If application comes back to foreground while exit is pending due unfinished
// operations, finising of those operations does not cause anymore real exit.
// -----------------------------------------------------------------------------
//
void LogsViewManager::appGainedForeground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::appGainedForeground()" );
    foreach ( LogsBaseView* view, mViewStack ){
        disconnect( view, SIGNAL(exitAllowed()), this, SLOT(proceedExit()) );
    }
    
    if ( mSettings.logsFeatureFakeExitEnabled() || mSettings.logsFeaturePreloadingEnabled() ){
        endFakeExit();
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::appGainedForeground()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::appLostForeground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::appLostForeground()" );
    
    if ( mSettings.logsFeatureFakeExitEnabled() ){
        doExitApplication(false); // Bypass view exit handling
    }
    // TODO: non-continuus app should also do exit at this point if 
    // didn't loose foreground because of embedded app. However, no sensible
    // way at the moment for handling embedded app foreground monitoring.
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::appLostForeground()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::activityRequested(const QString &activityId)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::activityRequested()" );
    if ( doLoadActivity(activityId) ){
        mMainWindow.bringAppToForeground();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::activityRequested()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::bgStartupForegroundGained()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::bgStartupForegroundGained()" );
    delete mBackgroundStartupWatcher;
    mBackgroundStartupWatcher = 0;
    endFakeExit();
    mMainWindow.bringAppToForeground(); 
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::bgStartupForegroundGained()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::doFakeExit()
{
    saveActivity();
    HbActivityManager* activityManager = 
        static_cast<HbApplication*>(qApp)->activityManager();
    connect( activityManager, SIGNAL(activityRequested(QString)), 
             this, SLOT(activityRequested(QString)), Qt::UniqueConnection );
    mComponentsRepository->model()->compressData();
    setTaskSwitcherVisibility(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::endFakeExit()
{
    setTaskSwitcherVisibility(true);
    mComponentsRepository->model()->refreshData();
    if ( !mMainWindow.currentView() ){
        activateDefaultView();
    }
    clearActivities();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsViewManager::doLoadActivity(const QString& activityId)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::doLoadActivity()" );
    bool loaded = false;
    HbApplication* hbApp = static_cast<HbApplication*>(qApp);
    LOGS_QDEBUG_2( "logs [UI] activity id:", activityId );
    bool ok = hbApp->activityManager()->waitActivity();
    if ( !ok ){
        LOGS_QDEBUG( "logs [UI] Activity reschedule failed" );
    }
    
    LogsBaseView* matchingView = 0;
    for ( int i = 0; i < mViewStack.count() && !matchingView; i++ ){
        if ( mViewStack.at(i)->matchWithActivityId(activityId) ){
            matchingView =  mViewStack.at(i);
        }
    }
    
    QList<QVariantHash> allParams = hbApp->activityManager()->activities();  
    QVariantHash params = allParams.isEmpty() ? QVariantHash() : allParams.at(0);
    LOGS_QDEBUG_2( "logs [UI] Activity params", params );
            
    if ( !matchingView ){
        // Try if creating such view is possible
        int viewId = params.value(logsActivityParamInternalViewId).toInt();
        matchingView = createView(static_cast<LogsAppViewId>(viewId));
    }
    
    if ( matchingView ){
        // Should have only one param hash in the list, use first always
        QList<QVariantHash> allParams = hbApp->activityManager()->activities();  
        QVariantHash params = allParams.isEmpty() ? QVariantHash() : allParams.at(0);
        LOGS_QDEBUG_2( "logs [UI] Activity params", params );
        bool showDialpad = params.value(logsActivityParamShowDialpad).toBool();
        QString dialpadText = params.value(logsActivityParamDialpadText).toString();
        
        QByteArray serializedActivity = hbApp->activateData().toByteArray();
        QDataStream stream(&serializedActivity, QIODevice::ReadOnly);
        
        QVariant args = matchingView->loadActivity(activityId, stream, params);
        loaded = doActivateView( 
            matchingView->viewId(), showDialpad, args, dialpadText );
    }
    
    LOGS_QDEBUG_2( "logs [UI] <- LogsViewManager::doLoadActivity() loaded:", loaded );
    return loaded;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::clearActivities()
{
    HbApplication* hbApp = static_cast<HbApplication*>(qApp);
    HbActivityManager* activityManager = hbApp->activityManager();
    foreach ( LogsBaseView* view, mViewStack ){
        view->clearActivity(*activityManager);
    }
    disconnect( activityManager, SIGNAL(activityRequested(QString)), 
                this, SLOT(activityRequested(QString)) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::activateViewViaService(
    LogsAppViewId viewId, bool showDialpad, 
    const QString& dialpadText, const QVariant& args)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsViewManager::activateViewViaService()", viewId );
    clearActivities();
    closeEmbeddedApplication();
    Dialpad* dpad = mComponentsRepository->dialpad();
    if ( !showDialpad ){
        dpad->closeDialpad();
    }
    dpad->editor().setText(dialpadText);
    if ( doActivateView(viewId, showDialpad, args, QString(), true) ){
        mMainWindow.bringAppToForeground();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::activateViewViaService()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::setTaskSwitcherVisibility(bool visible)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsViewManager::setTaskSwitcherVisibility()", visible );
    TsTaskSettings taskSettings;
    taskSettings.setVisibility(visible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::doExitApplication(bool viewExitHandling)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::doExitApplication()" );
    
    bool exitAllowed( true );
    foreach ( LogsBaseView* view, mViewStack ){
        if ( viewExitHandling ){
            view->handleExit();
        }
        if ( !view->isExitAllowed() ){
            exitAllowed = false;
            connect( view, SIGNAL(exitAllowed()),
                     this, SLOT(proceedExit()), 
                     Qt::UniqueConnection );
        }
    }
    if ( exitAllowed ){
        LOGS_QDEBUG( "logs [UI] Handle exit immediately" );
        proceedExit();
    } else {
        // Just wait for signal from view(s) that exiting can proceed
        LOGS_QDEBUG( "logs [UI] Delayed exit handling" );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::doExitApplication()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::activateDefaultView()
{
    changeRecentView( XQService::LogsViewAll, false );
}
