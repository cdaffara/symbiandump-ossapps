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
#include "logsrecentcallsview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsmodel.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logseffecthandler.h"
#include "logsmatchesmodel.h"
#include "logspageindicator.h"
#include "logsconfigurationparams.h"

//SYSTEM
#include <hbview.h>
#include <hblistview.h>
#include <hbabstractviewitem.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbgroupbox.h>
#include <hbmainwindow.h>
#include <hbswipegesture.h>
#include <hbmessagebox.h>
#include <hbactivitymanager.h>
#include <hbstyleloader.h>
#include <QTimer>

Q_DECLARE_METATYPE(LogsMatchesModel*)

const int logsMissedCallsMarkingDelayMs = 1000;

// -----------------------------------------------------------------------------
// LogsRecentCallsView::LogsRecentCallsView
// -----------------------------------------------------------------------------
//
LogsRecentCallsView::LogsRecentCallsView( 
    LogsComponentRepository& repository, LogsAbstractViewManager& viewManager )
    : LogsBaseView(LogsRecentViewId, repository, viewManager),
      mViewName(0),
      mListView(0),
      mFilter(0),
      mCurrentView(XQService::LogsViewAll),
      mAppearingView(XQService::LogsViewAll),
      mMoveLeftInList(false),
      mEffectHandler(0),
      mListViewX(0),
      mEmptyListLabelX(0),
      mMatchesModel(0),
      mMarkingMissedAsSeen(false),
      mPageIndicator(0),
      mListScrollBarPolicy(HbScrollArea::ScrollBarAutoHide),
      mEffectInDissappearedPhase(false)
{
    LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::LogsRecentCallsView()" );
    mModel = mRepository.model();
    	
    //TODO: taking away due to toolbar bug. If toolbar visibility changes on view
    //activation, there will be a crash due to previous view effect is playing
    //addViewSwitchingEffects();
    
    // Important to add in the same order as mCurrentView enums as correct
    // activity id is taken directly from the array based on the enum
    mActivities.append( logsActivityIdViewRecent );
    mActivities.append( logsActivityIdViewReceived );
    mActivities.append( logsActivityIdViewCalled );
    mActivities.append( logsActivityIdViewMissed );
}
    
// -----------------------------------------------------------------------------
// LogsRecentCallsView::~LogsRecentCallsView
// -----------------------------------------------------------------------------
//
LogsRecentCallsView::~LogsRecentCallsView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::~LogsRecentCallsView()" );
    
    delete mMatchesModel;
    mModel = 0;
    
    delete mEffectHandler;
    delete mFilter;
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::~LogsRecentCallsView()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::activated
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::activated()" );
    HbStyleLoader::registerFilePath(":/hbgroupboxheadingwidget.css");
    
    // base class handling first
    LogsBaseView::activated(showDialer, args);
    
    int internalViewId = args.toInt();
    if ( internalViewId < 0 || internalViewId > XQService::LogsViewMissed ){
        internalViewId = XQService::LogsViewAll;
    }
    XQService::LogsViewIndex view = static_cast<XQService::LogsViewIndex>( internalViewId );

    // View update is needed when we activate view for the first time (!mFilter)
    // or if view has to be changed
    if (  !mFilter || ( !args.isNull() && (mCurrentView != view) ) ) {
        updateView( view );
    } else {
        activateEmptyListIndicator(mFilter);
        handleMissedCallsCounter();
    }
    
    mPageIndicator->setActiveItemIndex(mConversionMap.value(mCurrentView));

    if (mEmptyListLabel) {
        mEmptyListLabelX = mEmptyListLabel->pos().x();
    }

    LogsBaseView::activationCompleted();
 
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::activated()" );  
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::deactivated
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::deactivated()
{
    HbStyleLoader::unregisterFilePath(":/hbgroupboxheadingwidget.css");

    //base class handling first
    LogsBaseView::deactivated();
    
    deactivateEmptyListIndicator(mFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::handleExit()
{
    markMissedCallsSeen();
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::isExitAllowed
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::isExitAllowed()
{
    return !mMarkingMissedAsSeen;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsRecentCallsView::saveActivity(
    QDataStream& serializedActivity, QVariantHash& metaData)
{
    Q_UNUSED( serializedActivity );
    Q_UNUSED( metaData );
    if ( mCurrentView >= 0 && mCurrentView < mActivities.count() ){
        return mActivities.at( mCurrentView );
    }
    return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsRecentCallsView::loadActivity(
    const QString& activityId, QDataStream& serializedActivity, QVariantHash& metaData)
{
    Q_UNUSED( serializedActivity );
    Q_UNUSED( metaData );
    return mActivities.indexOf(activityId);
}


// -----------------------------------------------------------------------------
// LogsRecentCallsView::initView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::initView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::initView()" );
    //base class handling first
    LogsBaseView::initView();
    
    mViewName = 
        qobject_cast<HbGroupBox*>(mRepository.findWidget(logsGroupboxViewNameId));
    
    addStringsToMap();
    initListWidget();
    
    mEffectHandler = new LogsEffectHandler;
    connect(mEffectHandler, SIGNAL(dissappearByMovingComplete()), 
            this, SLOT(dissappearByMovingComplete()));
    connect(mEffectHandler, SIGNAL(dissappearByFadingComplete()), 
            this, SLOT(dissappearByFadingComplete()));
    connect(mEffectHandler, SIGNAL(appearByMovingComplete()), 
            this, SLOT(appearByMovingComplete()));
    connect(mEffectHandler, SIGNAL(appearStarting()), this, SLOT(appearStarting()));
    mPageIndicator = qobject_cast<LogsPageIndicator*>
                        (mRepository.findWidget(logsPageIndicatorId));
    
    mPageIndicator->initialize(mTitleMap.count(), mConversionMap.value(mCurrentView));
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::initView()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::model
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsRecentCallsView::model() const
{
    return mFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAbstractModel* LogsRecentCallsView::logsModel() const
{
    return mModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbListView* LogsRecentCallsView::listView() const
{
    return mListView;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::markingCompleted
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::markingCompleted(int err)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::markingCompleted(), err", err );
    Q_UNUSED(err);
    mMarkingMissedAsSeen = false;
    emit exitAllowed();
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::markingCompleted()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::openDialpad
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::openDialpad()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::openDialpad()" );    
    LogsBaseView::openDialpad();
    
    // Create matches model already before any input to optimize 
    // first search
    if ( !mMatchesModel ){
        mMatchesModel = mModel->logsMatchesModel();
    }
    
    updateCallButton();

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::openDialpad()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::dialpadEditorTextChanged
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dialpadEditorTextChanged()" );
    if ( mDialpad->editor().text().length() > 0 && isContactSearchEnabled() ) {
        QVariant arg = qVariantFromValue( mMatchesModel );
        if ( mViewManager.activateView( LogsMatchesViewId, true, arg ) ){
            mMatchesModel = 0; // Ownership was given to matches view
        }
    } else {
        updateMenu();
        updateCallButton();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dialpadEditorTextChanged()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::clearList
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::clearList()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::clearList()->" );
    if ( mFilter ) {
        HbMessageBox::question(
                hbTrId("txt_dialer_ui_info_all_call_events_will_be_remo"),
                this,
                SLOT(clearListAnswer(int)),
                HbMessageBox::Ok | HbMessageBox::Cancel,
                new HbLabel(hbTrId("txt_dialer_ui_title_clear_list")));
    }
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::clearList()<-" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::clearListAnswer
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::clearListAnswer(int action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::clearListAnswer()" );
    if ((action == HbMessageBox::Ok) && mFilter) {
        mModel->clearList( mFilter->clearType() );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::clearListAnswer()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::toggleContactSearch
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::toggleContactSearch()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::toggleContactSearch()" );
    
    if ( isContactSearchPermanentlyDisabled() ){
        LOGS_QDEBUG( "logs [UI]     permanently disabled" );
    } else if ( isContactSearchEnabled() ){
        mModel->setPredictiveSearch( false );  
        updateMenu();
    } else {
        mModel->setPredictiveSearch( true );  
        updateMenu();
        if ( isDialpadInput() ){
           LogsRecentCallsView::dialpadEditorTextChanged();
        }
    }
 
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::toggleContactSearch()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateView(XQService::LogsViewIndex view)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::updateView(), view:", view );
    mCurrentView = view;
    LogsFilter::FilterType filter = getFilter( view );
    updateFilter(filter);
    updateViewName();
    updateContextMenuItems(mCurrentView);
    handleMissedCallsCounter();
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateView()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::changeFilter
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::changeFilter(HbAction* action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::changeFilter()" );
    XQService::LogsViewIndex view = mActionMap.key( action->objectName(),
            XQService::LogsViewAll );
    updateContextMenuItems(view);
    changeView(view);

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeFilter()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::handleBackSoftkey
// -----------------------------------------------------------------------------
//
void  LogsRecentCallsView::handleBackSoftkey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::handleBackSoftkey()" );
    mViewManager.exitApplication();
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::handleBackSoftkey()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::addStringsToMap
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::addStringsToMap()
{
    mTitleMap.insert(LogsBaseView::ViewAll, hbTrId("txt_dialer_subhead_recent_calls"));
    mTitleMap.insert(LogsBaseView::ViewCalled, hbTrId("txt_dialer_subhead_dialled_calls"));
    mTitleMap.insert(LogsBaseView::ViewReceived, hbTrId("txt_dial_subhead_received_calls"));
    mTitleMap.insert(LogsBaseView::ViewMissed, hbTrId("txt_dialer_subhead_missed_calls"));
    
    mConversionMap.insert(XQService::LogsViewAll, LogsBaseView::ViewAll);
    mConversionMap.insert(XQService::LogsViewCalled, LogsBaseView::ViewCalled);
    mConversionMap.insert(XQService::LogsViewReceived, LogsBaseView::ViewReceived);
    mConversionMap.insert(XQService::LogsViewMissed, LogsBaseView::ViewMissed);        
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::initListWidget
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::initListWidget()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::initListWidget()" );
    mListView = qobject_cast<HbListView*> 
                    ( mRepository.findWidget( logsListWidgetId ) );
    Q_ASSERT_X(mListView != 0, "logs [UI] ", "couldn't find list widget !!");
    
    
     // Optimize memory usage, list reserves only memory for visible items
    mListView->setItemRecycling(true);
    connect(mListView, SIGNAL(activated(const QModelIndex)),
            this, SLOT(initiateCallback(const QModelIndex)), 
            Qt::UniqueConnection);
    connect(mListView,
            SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this,
            SLOT(showListItemMenu(HbAbstractViewItem*, const QPointF&)), 
            Qt::UniqueConnection);
    
    mListView->setFrictionEnabled(true);
    
    mListViewX = mListView->pos().x();
    
    grabGesture(Qt::SwipeGesture);
    
    mListScrollBarPolicy = mListView->verticalScrollBarPolicy();

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::initListWidget() " );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateFilter
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateFilter(LogsFilter::FilterType type)
{
    if ( mListView ) {
        LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateFilter()" );
        
        deactivateEmptyListIndicator(mFilter);
        
        handleMissedCallsMarking();
        
        delete mFilter;
        mFilter = 0;
        mFilter = new LogsFilter( type );
        mFilter->setSourceModel( mModel );
        
        mListView->setModel( mFilter );//ownership not transferred
        
        scrollToTopItem(mListView);
        
        activateEmptyListIndicator(mFilter);
        
        LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateFilter() " );
    }  else {
        LOGS_QWARNING( "logs [UI] LogsRecentCallsView::updateFilter(), !no list widget!" );
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateViewName
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateViewName()
{
    if ( mViewName ) {
        mViewName->setHeading( mTitleMap.value(mConversionMap.value(mCurrentView))); 
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateContextMenuItems
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateContextMenuItems(XQService::LogsViewIndex view)
{
    LOGS_QDEBUG_2( 
        "logs [UI] -> LogsRecentCallsView::updateContextMenuItems(), view:", view );
    if ( mShowFilterMenu ) {
        QString activeActionName = mActionMap.value(view);
        foreach (QAction* action, mShowFilterMenu->actions() ) {
            action->setChecked( action->objectName() == activeActionName );
        }
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateContextMenuItems()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::getFilter
// -----------------------------------------------------------------------------
//
LogsFilter::FilterType LogsRecentCallsView::getFilter(XQService::LogsViewIndex view)
{
    LogsFilter::FilterType filter = LogsFilter::All;
    switch (view){
        case XQService::LogsViewAll:
            filter = LogsFilter::All;
            break;
        case XQService::LogsViewReceived:
            filter = LogsFilter::Received;
            break;
        case XQService::LogsViewCalled:
            filter = LogsFilter::Called;
            break;
        case XQService::LogsViewMissed:
            filter = LogsFilter::Missed;
            break;
        default:
            break;
    }
    return filter;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::gestureEvent
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::gestureEvent(QGestureEvent *event)
{
    QGesture* gesture = event->gesture(Qt::SwipeGesture);
    if (gesture) {
        LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::gestureEvent()" );
        HbSwipeGesture* swipe = static_cast<HbSwipeGesture *>(gesture);
        if (swipe) {
            QSwipeGesture::SwipeDirection direction = swipe->sceneHorizontalDirection(); 
            if (swipe->state() == Qt::GestureStarted) {
                //fix for ou1cimx1#481152, if policy not set then every swipe
                //within list item boundaries will also activate list item
                if (direction == QSwipeGesture::Left 
                   || direction == QSwipeGesture::Right) {
                    swipe->setGestureCancelPolicy(QGesture::CancelAllInContext);
                    LOGS_QDEBUG( "logs [UI] swipe->state() == Qt::GestureStarted" );
                }
            } else  if (swipe->state() == Qt::GestureFinished) {
                LOGS_QDEBUG_2( "logs [UI] sceneSwipeAngle: ", swipe->sceneSwipeAngle() );
                LOGS_QDEBUG_2( "logs [UI] swipeAngle: ", swipe->swipeAngle() );
                
                if ( decideListMoveDirection(direction) ) {
                    event->accept(Qt::SwipeGesture);
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::decideListMoveDirection
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::decideListMoveDirection(
    QSwipeGesture::SwipeDirection direction)
{
    bool moved( false );
    if ( direction == QSwipeGesture::Left || direction == QSwipeGesture::Right ) {
        bool moveForward( direction == QSwipeGesture::Left );
        if ( layoutDirection() == Qt::RightToLeft ){
            moveForward = !moveForward;
        }
        if ( moveForward ){
            moveForwardInLists();
        } else {
            moveBackwardInLists();
        }
        moved = true;
    }
    return moved;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::moveForwardInLists
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::moveForwardInLists()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::moveForwardInLists()" );
	if ( mConversionMap.value(mCurrentView) + 1 < mTitleMap.count() ){
    	LogsBaseView::LogsViewMap viewmap = 
            static_cast<LogsBaseView::LogsViewMap>(mConversionMap.value(mCurrentView) +1);
    	changeView( mConversionMap.key(viewmap) );
    } else {
        if (model() && model()->rowCount() > 0) {
            mEffectHandler->startMoveNotPossibleEffect(
                *mListView, moveToLeft(false), mListViewX);
        } else {
            mEffectHandler->startMoveNotPossibleEffect(
                *mEmptyListLabel, moveToLeft(false), mEmptyListLabelX);
        }
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::moveForwardInLists()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::moveBackwardInLists
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::moveBackwardInLists()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::moveBackwardInLists()" );
    if ( mConversionMap.value(mCurrentView) > 0 ){
    	LogsBaseView::LogsViewMap viewmap = 
            static_cast<LogsBaseView::LogsViewMap>(mConversionMap.value(mCurrentView) - 1);
    	changeView( mConversionMap.key(viewmap) );	
    } else {
        if (model() && model()->rowCount() > 0) {
            mEffectHandler->startMoveNotPossibleEffect(
                *mListView, moveToLeft(true), mListViewX);
        } else {
            mEffectHandler->startMoveNotPossibleEffect(
                *mEmptyListLabel, moveToLeft(true), mEmptyListLabelX);
        }
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::moveBackwardInLists()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::changeView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::changeView(XQService::LogsViewIndex view)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::changeView(), view:", view );
    
    if ( view == mCurrentView ){
        LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeView(), view already correct" );
        // Already correct view, running animation can continue but make sure that
        // appearing view is then correct.
        mAppearingView = view;
        return;
    }
    
    mMoveLeftInList = 
        moveToLeft( mConversionMap.value(view) < mConversionMap.value(mCurrentView) );
    mAppearingView = view;
    // Disable scrollbar while moving the list for more nice looks
    mListView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);  
    mEffectHandler->startDissappearAppearByFadingEffect(*mViewName);
    mEffectHandler->startDissappearAppearByMovingEffect(
            *mListView, *mEmptyListLabel, !mMoveLeftInList, 
             mListViewX, mEmptyListLabelX);

    mPageIndicator->setActiveItemIndex(mConversionMap.value(mAppearingView));
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dissappearByFadingComplete()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dissappearByFadingComplete()" )

    // Previous view name has dissappeared by fading, set new view name 
    // as it is brought visible by effect
    mViewName->setHeading( mTitleMap.value(mConversionMap.value(mAppearingView)) );
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dissappearByFadingComplete()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dissappearByMovingComplete()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dissappearByMovingComplete()" )
    
    mEffectInDissappearedPhase = true;
    mListView->setVisible(false); 
    updateView( mAppearingView ); 
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dissappearByMovingComplete()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::appearStarting()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::appearStarting()" )
        
    mEffectInDissappearedPhase = false;
    mListView->setVisible(true);
    updateEmptyListLabelVisibility();
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::appearStarting()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::appearByMovingComplete()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::appearByMovingComplete()" )
    
    // Restore scrollbar which was hidden when animation started   
    mListView->setVerticalScrollBarPolicy(mListScrollBarPolicy);

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::appearByMovingComplete()" )
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateMenu
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateMenu()" );
    HbAction* action = qobject_cast<HbAction*>( 
            mRepository.findObject( logsRecentViewClearListMenuActionId ) );
    if ( action && model() ) {
        bool visible( model()->rowCount() > 0 );
        action->setVisible( visible );
    }
    
    HbAction* addToContactsAction = qobject_cast<HbAction*>( 
        mRepository.findObject( logsCommonAddToContactsMenuActionId ) );
    toggleActionAvailability(addToContactsAction, isDialpadInput());
    
    updateDialpadCallAndMessagingActions();
    updateContactSearchAction();
    
    updateMenuVisibility();
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateMenu()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateEmptyListWidgetsVisibility
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateEmptyListWidgetsVisibility()
{
    updateMenu();
    updateEmptyListLabelVisibility();
    updateCallButton();
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateWidgetsSizeAndLayout
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateWidgetsSizeAndLayout()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateWidgetsSizeAndLayout()" );
    if ( mListView ) {
        updateMenu();
        updateListLayoutName(*mListView);
        updateListSize(*mListView);
        LogsConfigurationParams param;
        param.setListItemTextWidth( getListItemTextWidth() );
        mModel->updateConfiguration(param);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateWidgetsSizeAndLayout()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateEmptyListLabelVisibility()
{
    if ( mEmptyListLabel && model() ){
        LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::updateEmptyListLabelVisibility()" );   
        bool visible(model()->rowCount() == 0 && !mEffectInDissappearedPhase );
        mEmptyListLabel->setVisible(visible);
    }  
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::getListItemTextWidth
// -----------------------------------------------------------------------------
//
int LogsRecentCallsView::getListItemTextWidth()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::getListItemTextWidth()" );

    qreal width = 0.0;
    QRectF screenRect = mViewManager.mainWindow().layoutRect();
    LOGS_QDEBUG_2( "logs [UI]  screenRect:", screenRect );
    
    // Cannot use hb-param-screen-width in expressions currently due bug in layoutrect
    qreal modifier = 0.0;
    QString expr;
    if (mListView->layoutName() == QLatin1String(logsListDefaultLayout)) {
        expr = "expr(var(hb-param-graphic-size-primary-medium) + var(hb-param-margin-gene-left) ";
        expr += "+ var(hb-param-margin-gene-right) + var(hb-param-margin-gene-middle-horizontal))";
        width = screenRect.width();
    } else {
        expr = "expr(var(hb-param-graphic-size-primary-medium) + var(hb-param-margin-gene-left) ";
        expr += "+ var(hb-param-margin-gene-right))";
        width = screenRect.width() / 2;
    }
    
    if ( expr.isEmpty() || !style()->parameter(expr, modifier) ){
        LOGS_QDEBUG( "logs [UI] No expression or incorrect expression" );
    }
    width -= modifier;
    
    LOGS_QDEBUG_2( "logs [UI] <- LogsRecentCallsView::getListItemTextWidth(): ", width );   
    return qRound(width);
}


// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateCallButton
// If dialpad contains text or there is items in list, call button is enabled
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateCallButton()
{  
    bool isVisible = !mDialpad->editor().text().isEmpty();
    if ( !isVisible && mFilter ) {
        isVisible = ( mFilter->rowCount() > 0 );
    }
    mDialpad->setCallButtonEnabled( isVisible );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::handleMissedCallsMarking
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::handleMissedCallsMarking()
{
    if ( mFilter && !mMarkingMissedAsSeen && !mActivating && 
          ( mFilter->filterType() == LogsFilter::Missed || 
            mFilter->filterType() == LogsFilter::All ) ){
        // Don't care if timer would be already running, slot's implementation
        // takes care that marking is done only once
        LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::handleMissedCallsMarking()" );
        QTimer::singleShot( 
                logsMissedCallsMarkingDelayMs, this, SLOT(markMissedCallsSeen()) );
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::markMissedCallsSeen
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::markMissedCallsSeen()
{
    if ( !mMarkingMissedAsSeen ){
        connect( mModel, SIGNAL(markingCompleted(int)), 
                 this, SLOT(markingCompleted(int)),
                 Qt::UniqueConnection);
        mMarkingMissedAsSeen = 
            mModel->markEventsSeen(LogsModel::TypeLogsClearMissed);
    }
    return mMarkingMissedAsSeen;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::moveToLeft
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::moveToLeft(bool toLeft) const
{
    if ( layoutDirection() == Qt::RightToLeft ){
        return !toLeft;
    }
    return toLeft;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::handleMissedCallsCounter
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::handleMissedCallsCounter()
{
    if (mModel && mCurrentView == XQService::LogsViewMissed) {
        LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::clearMissedCallsCounter()" );
        mModel->clearMissedCallsCounter();
    }
}

