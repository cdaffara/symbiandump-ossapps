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
#include "ut_logsrecentcallsview.h"
#include "logscomponentrepository.h"
#include "logsmodel.h"
#include "logsfilter.h"
#include "logsdetailsmodel.h"
#include "logsdefs.h"
#include "logseffecthandler.h"
#include "hbstubs_helper.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logsmatchesmodel.h"
#include "logsmainwindow.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbswipegesture.h>
#include <hblistview.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <xqaiwdecl.h>

#define VERIFY_CHECKED_ACTION( v, actionName ) { \
QVERIFY( v->mShowFilterMenu );\
QVERIFY( v->mShowFilterMenu->actions().count() > 0 );\
QAction* action = 0;\
foreach ( action, v->mShowFilterMenu->actions() ){ \
    if ( action->objectName() == actionName ){ \
        QVERIFY( action->isChecked() ); \
    } else { \
        QVERIFY( !action->isChecked() ); \
    } \
}}


void UT_LogsRecentCallsView::initTestCase()
{
    mMainWindow = new LogsMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsRecentCallsView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsRecentCallsView::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
    mRecentCallsView = new LogsRecentCallsView( *mRepository, *mViewManager );
}

void UT_LogsRecentCallsView::cleanup()
{
    delete mRecentCallsView;
    mRecentCallsView = 0;
    delete mRepository;
    mRepository = 0;
}

void UT_LogsRecentCallsView::testConstructor()
{
    QVERIFY( mRecentCallsView );
    QVERIFY( !mRecentCallsView->mViewName );
    QVERIFY( !mRecentCallsView->mListView );
    QVERIFY( !mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mModel );
    QVERIFY( !mRecentCallsView->mShowFilterMenu );
    QVERIFY( mRecentCallsView->mCurrentView == XQService::LogsViewAll );
    QVERIFY( mRecentCallsView->viewId() == LogsRecentViewId );
    QVERIFY( mRecentCallsView->mLayoutSectionName == "" );
    QCOMPARE( mRecentCallsView->mActivities.at(XQService::LogsViewAll), QString(logsActivityIdViewRecent) );
    QCOMPARE( mRecentCallsView->mActivities.at(XQService::LogsViewReceived), QString(logsActivityIdViewReceived) );
    QCOMPARE( mRecentCallsView->mActivities.at(XQService::LogsViewCalled), QString(logsActivityIdViewCalled) );
    QCOMPARE( mRecentCallsView->mActivities.at(XQService::LogsViewMissed), QString(logsActivityIdViewMissed) );
    
}

void UT_LogsRecentCallsView::testInitView()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( view );
    QVERIFY( !view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( !view->mShowFilterMenu );
    QVERIFY( view->mTitleMap.isEmpty() );
    QVERIFY( view->mActionMap.isEmpty() );
    view->initView();
    QVERIFY( view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( view->mShowFilterMenu );
    QVERIFY( view->mTitleMap.count() == 4 );
    QVERIFY( view->mActionMap.count() == 4 );
    
    //TODO: Init twice not allowed
}

void UT_LogsRecentCallsView::testActivated()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( view );
    // Change for uninitialized causes initialization
    QVERIFY( !view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( !view->mEmptyListLabel );
    view->activated(false, QVariant(XQService::LogsViewAll));
    QVERIFY( view->mInitialized );
    QVERIFY( view->mFilter );
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( !view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterRecentMenuActionId )
    
    // Change views
    view->activated(false, QVariant(XQService::LogsViewReceived));
    QVERIFY( view->mFilter );
    QVERIFY( view->mFilter->filterType() == LogsFilter::Received );  
    QVERIFY( !view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterReceivedMenuActionId )
    
    view->activated(false, QVariant(XQService::LogsViewMissed));
    QVERIFY( view->mFilter->filterType() == LogsFilter::Missed );
    QVERIFY( view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterMissedMenuActionId )

    // Change to missed view again, no need to update view, only 
    // missed call counter updated
    view->mModel->mMissedCallsCounterCleared = false;
    view->updateFilter(LogsFilter::Received);    
    view->activated(false, QVariant(XQService::LogsViewMissed));
    QVERIFY( view->mFilter->filterType() == LogsFilter::Received );  
    QVERIFY( view->mModel->mMissedCallsCounterCleared );
    
    view->mModel->mMissedCallsCounterCleared = false;
    view->activated(false, QVariant(XQService::LogsViewCalled));
    QVERIFY( view->mFilter->filterType() == LogsFilter::Called );  
    QVERIFY( !view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterDialledMenuActionId )
    
    view->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    view->mDialpad->editor().setText( QString("hello") );
    view->mActivating = true;
    view->activated(false, QVariant(XQService::LogsViewAll));
    QVERIFY( view->mFilter->filterType() == LogsFilter::All );  
    QVERIFY( !view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterRecentMenuActionId )
    QVERIFY( !view->mDialpad->editor().text().isEmpty() );
    QVERIFY( view->mListView->layoutName() == logsListLandscapeLayout );
    QVERIFY( view->mLayoutSectionName == logsViewDefaultSection );
    QVERIFY( !view->mActivating );
}

void UT_LogsRecentCallsView::testDeactivated()
{
    mRecentCallsView->deactivated();
}

void UT_LogsRecentCallsView::testInitListWidget()
{
    //TODO:Q_ASSERT_X
/*    //list widget can't be found from repository
    QVERIFY( !mRecentCallsView->mListView );
    mRecentCallsView->initListWidget();
    QVERIFY( !mRecentCallsView->mListView );
  */
    //list widget is in repository
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( !view->mListView );
    view->initListWidget();
    QVERIFY( view->mListView );
    view = 0; 
}

void UT_LogsRecentCallsView::testUpdateFilter()
{
    //no list view
    QVERIFY( !mRecentCallsView->mListView );
    QVERIFY( !mRecentCallsView->mFilter );
    mRecentCallsView->updateFilter(LogsFilter::All);
    QVERIFY( !mRecentCallsView->mFilter );
    
    //first filter created with update
    HbStubHelper::reset();
    QVERIFY( !mRecentCallsView->mFilter );
    mRecentCallsView->mListView = new HbListView();
    mRecentCallsView->updateFilter(LogsFilter::All);
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::All );    
    QVERIFY( !HbStubHelper::singleShotTimerActive() );
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
            
    //filter is updated with a new one, missed calls marking as seen is started (by timer)
    mRecentCallsView->mActivating = false;
    QVERIFY( mRecentCallsView->mFilter );
    mRecentCallsView->mListView = new HbListView();
    mRecentCallsView->updateFilter(LogsFilter::Missed);
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::Missed );
    QVERIFY( HbStubHelper::singleShotTimerActive() );
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
    
    //filter is updated with a new one, missed calls marking as seen is not started
    //as view activation is ongoing
    HbStubHelper::reset();
    mRecentCallsView->mActivating = true;
    mRecentCallsView->mListView = new HbListView();
    mRecentCallsView->updateFilter(LogsFilter::Missed);
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::Missed );
    QVERIFY( !HbStubHelper::singleShotTimerActive() );
}

void UT_LogsRecentCallsView::testUpdateViewName()
{
    //no view name label
    QVERIFY( !mRecentCallsView->mViewName );
    mRecentCallsView->updateViewName();
    QVERIFY( !mRecentCallsView->mViewName );
    
    //view name label exists
    mRecentCallsView->mCurrentView = XQService::LogsViewMissed;
    mRecentCallsView->addStringsToMap();
    mRecentCallsView->mViewName = new HbGroupBox();
    mRecentCallsView->updateViewName();
    QVERIFY( mRecentCallsView->mViewName->heading() 
            == mRecentCallsView->mTitleMap.value(
                   mRecentCallsView->mConversionMap.value(XQService::LogsViewMissed)));
    delete mRecentCallsView->mViewName;
    mRecentCallsView->mViewName = 0;
}

void UT_LogsRecentCallsView::testChangeFilter()
{
    mRepository->recentCallsView();
    mRecentCallsView->activated( false, QVariant(XQService::LogsViewAll) );
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::All );
    
    HbAction*  action = new HbAction();
    action->setObjectName(logsShowFilterMissedMenuActionId);
    mRecentCallsView->changeFilter(action);
    // Scrollbar is disbaled while changing the list and is restored when appearByMoving slot is called
    QVERIFY( mRecentCallsView->mListView->verticalScrollBarPolicy() == HbScrollArea::ScrollBarAlwaysOff );
    
    // Because of effects, filter is not changed immediately, simulate effect completion
    QVERIFY( mRecentCallsView->mAppearingView == XQService::LogsViewMissed );
    mRecentCallsView->dissappearByMovingComplete();
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::Missed );   
    QVERIFY( mRecentCallsView->mEffectInDissappearedPhase );
    QVERIFY( !mRecentCallsView->mListView->isVisible() ); // Hidden while in dissappeared state
    QVERIFY( !mRecentCallsView->mEmptyListLabel->isVisible() );
    mRecentCallsView->appearStarting();
    QVERIFY( !mRecentCallsView->mEffectInDissappearedPhase );
    QVERIFY( mRecentCallsView->mListView->isVisible() ); // Not anymore hidden as about to appear
    QVERIFY( !mRecentCallsView->mEmptyListLabel->isVisible() ); // Still hidden as list has data
    mRecentCallsView->appearByMovingComplete();
    QVERIFY( mRecentCallsView->mListView->verticalScrollBarPolicy() != HbScrollArea::ScrollBarAlwaysOff );
    
    delete action;
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
    delete mRecentCallsView->mViewName;
    mRecentCallsView->mViewName = 0;
}

void UT_LogsRecentCallsView::testInitiateCallback()
{
    LogsCall::resetTestData();
    // Call cannot be constructed in model
    QModelIndex filterIndex;
    mRecentCallsView->initiateCallback(filterIndex);
    QVERIFY( LogsCall::lastCalledFunction().isEmpty() );

    // Call can be constructed in model
    filterIndex = makeValidFilterIndex(*mRecentCallsView);
    mRecentCallsView->initiateCallback(filterIndex);
    QVERIFY( LogsCall::lastCalledFunction() == QString("initiateCallback") );
}

void UT_LogsRecentCallsView::testCallKeyPressed()
{    
    LogsRecentCallsView* view = mRepository->recentCallsView();
    
    // Not ready for calling
    LogsCall::resetTestData();
    view->callKeyPressed();
    QVERIFY( LogsCall::lastCalledFunction() != "initiateCallback" );
    
    // No any item where to call
    LogsCall::resetTestData();
    view->activated( false, QVariant(XQService::LogsViewAll) );
    view->callKeyPressed();
    QVERIFY( LogsCall::lastCalledFunction() != "initiateCallback" );
    
    // Calling possible to list item
    LogsCall::resetTestData();
    mRepository->model()->mTextData.append("testdata");
    mRepository->model()->mTextData.append("testdata2");
    view->mListView->setCurrentIndex( 
        mRepository->model()->index( 1, 0 ), QItemSelectionModel::Select );
    view->callKeyPressed();
    QVERIFY( view->mListView->currentIndex().row() == 0 );
    QVERIFY( LogsCall::lastCalledFunction() == "initiateCallback" );
    
    // Dialpad open but no input, call to first item in list
    LogsCall::resetTestData();
    view->mDialpad->mIsOpen = true;
    view->callKeyPressed();
    QVERIFY( LogsCall::lastCalledFunction() == "initiateCallback" );
    
    // Dialpad open and text in input, call to number in input field
    LogsCall::resetTestData();
    QString dial("12345");
    view->mDialpad->editor().setText( dial );
    view->callKeyPressed();
    QVERIFY( LogsCall::isCallToNumberCalled() );
    QCOMPARE( LogsCall::lastCalledNumber(), dial );
    QVERIFY( LogsCall::lastCalledFunction() != "initiateCallback" );
    
    // Make sure that number is converted to western digits before passing
    // forward
    HbStubHelper::stringUtilDigitConversion(true);
    LogsCall::resetTestData();
    view->mDialpad->editor().setText( dial );
    view->callKeyPressed();
    QVERIFY( LogsCall::isCallToNumberCalled() );
    QCOMPARE( LogsCall::lastCalledNumber(), QString("conv") + dial );
    LogsCall::resetTestData();
    
    // Dialpad closed but contains text, call to first item in the list
    LogsCall::resetTestData();
    view->mDialpad->mIsOpen = false;
    view->callKeyPressed();
    QVERIFY( !LogsCall::isCallToNumberCalled() );
    QVERIFY( LogsCall::lastCalledFunction() == "initiateCallback" );
}

void UT_LogsRecentCallsView::testShowCallDetails()
{
    // No details model, nothing to show
    QVERIFY( mRecentCallsView->mDetailsModel == 0 );
    mRecentCallsView->showCallDetails();
    QVERIFY( mViewManager->mViewId != LogsDetailsViewId );
    
    // Details model shown
    mRecentCallsView->mDetailsModel = new LogsDetailsModel;
    mRecentCallsView->showCallDetails();
    QVERIFY( mViewManager->mViewId == LogsDetailsViewId );
}

void UT_LogsRecentCallsView::testOpenDialpad()
{
    QVERIFY( !mRecentCallsView->mFilter );
    QVERIFY( !mRecentCallsView->mDialpad->isOpen() );
    mRecentCallsView->openDialpad();
    QVERIFY( mRecentCallsView->mMatchesModel );
    QVERIFY( mRecentCallsView->mDialpad->isOpen() );
    
    
    mRecentCallsView->mFilter = new LogsFilter( LogsFilter::Missed );
    LogsMatchesModel* oldmatchesModel = mRecentCallsView->mMatchesModel;
    mRecentCallsView->openDialpad();
    QVERIFY( mRecentCallsView->mMatchesModel == oldmatchesModel );
    QVERIFY( mRecentCallsView->mDialpad->isOpen() );
}

void UT_LogsRecentCallsView::testDialpadEditorTextChanged()
{
    // Matches view is opened and matches model is passed as arg
    // text editor is one character long
    LogsDbConnector* dbConnector = 0;
    delete mRecentCallsView->mMatchesModel;
    mRecentCallsView->mMatchesModel = 0;
    mRecentCallsView->mMatchesModel = new LogsMatchesModel(*dbConnector);
    mRecentCallsView->mDialpad->editor().setText( QString("h") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsMatchesViewId );
    QVERIFY( mRecentCallsView->mMatchesModel == 0 );
    
    // No text in editor, no view change, no lines in model, call button
    // gets disabled
    mViewManager->reset();
    mRecentCallsView->mDialpad->mIsCallButtonEnabled = true;
    delete mRecentCallsView->mFilter;
    mRecentCallsView->mFilter = new LogsFilter();
    mRecentCallsView->mDialpad->editor().setText( QString("") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( !mRecentCallsView->mDialpad->mIsCallButtonEnabled );
    
    // No text in editor but items in list, call button is enabled
    mRecentCallsView->mDialpad->mIsCallButtonEnabled = false;
    mRecentCallsView->mFilter->setSourceModel(mRecentCallsView->mModel);
    mRecentCallsView->mDialpad->editor().setText( QString("") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( mRecentCallsView->mDialpad->mIsCallButtonEnabled );
}

void UT_LogsRecentCallsView::testGestureEvent()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->setLayoutDirection(Qt::LeftToRight);
    view->activated( false, QVariant(XQService::LogsViewCalled) );
    view->mCurrentView = XQService::LogsViewCalled;
    view->mAppearingView = XQService::LogsViewCalled;

    HbSwipeGesture* swipe = new HbSwipeGesture();
    QList<QGesture*> list;
    QGestureEvent event(list);
    event.ignore(Qt::SwipeGesture);

    //no swipe gesture in event
    QVERIFY(!event.isAccepted(Qt::SwipeGesture));    
    view->gestureEvent(&event);
    QVERIFY(!event.isAccepted(Qt::SwipeGesture));
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewCalled);
    
    //swipe gesture in event, but gesture isn't finished
    list.append(swipe);
    QGestureEvent event2(list);
    event2.ignore(Qt::SwipeGesture);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    QVERIFY(swipe->state() != Qt::GestureFinished);
    view->gestureEvent(&event2);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewCalled);
    
    //vertical swipe started, gesture cancel policy not modified
    HbStubHelper::setGestureState(Qt::GestureStarted);
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(70);
    QVERIFY(swipe->gestureCancelPolicy() == QGesture::CancelNone);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewCalled);
    QVERIFY(swipe->gestureCancelPolicy() == QGesture::CancelNone);
    
    //horizontal swipe started, gesture cancel policy modified to prevent tap
    const int swipeAngleRight = 10;
    HbStubHelper::setGestureState(Qt::GestureStarted);
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(swipeAngleRight);
    QVERIFY(swipe->gestureCancelPolicy() == QGesture::CancelNone);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewCalled);
    QVERIFY(swipe->gestureCancelPolicy() == QGesture::CancelAllInContext);
    
    //swipe right
    HbStubHelper::setGestureState(Qt::GestureFinished);
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(swipeAngleRight);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewAll );
    QVERIFY( event2.isAccepted(Qt::SwipeGesture) );
    
    //swipe left
    const int swipeAngleLeft = 170;
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(swipeAngleLeft);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewReceived);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));
    
    //swipe down, nothing happens
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(70);
    view->mAppearingView = view->mCurrentView;
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewCalled);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    
    // Swipe directions are mirrored in right-to-left layout direction
    view->setLayoutDirection(Qt::RightToLeft);
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(swipeAngleLeft);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewAll);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));
    
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSceneSwipeAngle(swipeAngleRight);
    view->gestureEvent(&event2);
    QCOMPARE(view->mCurrentView, XQService::LogsViewCalled);
    QCOMPARE(view->mAppearingView, XQService::LogsViewReceived);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));
    
    qDeleteAll(list);
}

void UT_LogsRecentCallsView::testViewChangeByFlicking()
{
    qApp->setLayoutDirection(Qt::LeftToRight);
    
    // At leftmost list, moving to left not possible
    // List not empty, starting list animation
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->activated( false, QVariant(XQService::LogsViewAll) );
    view->mCurrentView = XQService::LogsViewAll;
    QVERIFY(view->model() && view->model()->rowCount()>0);
    view->moveBackwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewAll);
    QVERIFY(view->mAppearingView == XQService::LogsViewAll);
    QVERIFY(view->mEffectHandler->mItemMoveNotPossibleAnimationStart->targetObject()
            == view->mListView);
    
    // Moving left not possible, list empty, starting empty label animation
    delete view->mFilter;
    view->mFilter = 0;
    QVERIFY(!view->model());
    view->moveBackwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewAll);
    QVERIFY(view->mAppearingView == XQService::LogsViewAll);
    QVERIFY(view->mEffectHandler->mItemMoveNotPossibleAnimationStart->targetObject()
            == view->mEmptyListLabel);
    
    // Moving to right possible
    view->moveForwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewAll);
    QVERIFY(view->mAppearingView == XQService::LogsViewCalled);
    QVERIFY(!view->mMoveLeftInList);
    
    // At rightmost list, moving further not possible
    // List empty, starting empty label animation
    view->mCurrentView = XQService::LogsViewMissed;
    QVERIFY(!view->model());
    view->moveForwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewMissed);
    QVERIFY(view->mAppearingView == XQService::LogsViewCalled);
    QVERIFY(view->mEffectHandler->mItemMoveNotPossibleAnimationStart->targetObject()
                == view->mEmptyListLabel);

    // Moving right not possible, list not empty, starting list animation
    LogsModel model;
    view->mFilter = new LogsFilter();
    view->mFilter->setSourceModel(&model);
    view->moveForwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewMissed);
    QVERIFY(view->mAppearingView == XQService::LogsViewCalled);
    QVERIFY(view->mEffectHandler->mItemMoveNotPossibleAnimationStart->targetObject()
                == view->mListView);
    
    // Now moving to left is possible
    view->moveBackwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewMissed);
    QVERIFY(view->mAppearingView == XQService::LogsViewReceived);
    QVERIFY(view->mMoveLeftInList);
    
    // Simulate effect completion which activates new view
    view->dissappearByFadingComplete();
    QVERIFY( view->mViewName->heading() 
             == view->mTitleMap.value(view->mConversionMap.value(XQService::LogsViewReceived)) );
    
    view->dissappearByMovingComplete();
    QVERIFY(view->mCurrentView == XQService::LogsViewReceived);
    QVERIFY(view->mAppearingView == XQService::LogsViewReceived);
    
    // If view would be already what expected, nothing happens
    view->dissappearByMovingComplete();
    QVERIFY(view->mCurrentView == XQService::LogsViewReceived);
    QVERIFY(view->mAppearingView == XQService::LogsViewReceived);
    
    // Check that moving direction is mirrored in RTL layout dir
    view->setLayoutDirection(Qt::RightToLeft);
    view->mCurrentView = XQService::LogsViewAll;
    view->moveForwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewAll);
    QVERIFY(view->mAppearingView == XQService::LogsViewCalled);
    QVERIFY(view->mMoveLeftInList);
    view->mCurrentView = XQService::LogsViewCalled;
    view->moveBackwardInLists();
    QVERIFY(view->mCurrentView == XQService::LogsViewCalled);
    QVERIFY(view->mAppearingView == XQService::LogsViewAll);
    QVERIFY(!view->mMoveLeftInList);
    
    
}


void UT_LogsRecentCallsView::testModel()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->activated(false, QVariant(XQService::LogsViewAll));
    QVERIFY( view->mFilter );
    QVERIFY( view->model() == view->mFilter );
}

void UT_LogsRecentCallsView::testShowListItemMenu()
{
    HbStubHelper::reset();
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    HbListViewItem* item = new HbListViewItem;
    item->setModelIndex(filterIndex);//mRecentCallsView->mFilter->index( 0, 0 ));

    mRecentCallsView->showListItemMenu(item, QPointF());
  
    //can't test, showed only if itemContextMenu.actions().count() > 0
    //if we stub actions(), then testActivated() will fail
//    QVERIFY( HbStubHelper::menuShown() );
    QVERIFY( HbStubHelper::widgetActionsCount() == 7 );
    delete item;
}

void UT_LogsRecentCallsView::testUpdateListItemData()
{
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mCall );
    QVERIFY( !mRecentCallsView->mContact );
    QVERIFY( !mRecentCallsView->mMessage );
    QVERIFY( !mRecentCallsView->mDetailsModel );
    
    mRecentCallsView->updateListItemData( filterIndex );

    QVERIFY( mRecentCallsView->mCall );
    QVERIFY( mRecentCallsView->mContact );
    QVERIFY( mRecentCallsView->mMessage );
    QVERIFY( mRecentCallsView->mDetailsModel );
}

void UT_LogsRecentCallsView::testPopulateListItemMenu()
{
    //call, message and contact exist, menu has actions
    HbStubHelper::reset();
    mRecentCallsView->mCall = new LogsCall();
    mRecentCallsView->mMessage = new LogsMessage();
    mRecentCallsView->mContact = new LogsContact();
    mRecentCallsView->mDetailsModel = new LogsDetailsModel();
    HbMenu menu;
    mRecentCallsView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 7 );
}

void UT_LogsRecentCallsView::testUpdateCall()
{
    //call can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mCall );
    mRecentCallsView->updateCall( filterIndex );
    QVERIFY( mRecentCallsView->mCall );

    //call can't be created
    mRecentCallsView->updateCall( QModelIndex() );
    QVERIFY( !mRecentCallsView->mCall );
}

void UT_LogsRecentCallsView::testUpdateMessage()
{        
    //message can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mMessage );
    mRecentCallsView->updateMessage( filterIndex );
    QVERIFY( mRecentCallsView->mMessage );

    //message can't be created
    mRecentCallsView->updateMessage( QModelIndex() );
    QVERIFY( !mRecentCallsView->mMessage );
}

void UT_LogsRecentCallsView::testUpdateContact()
{
    //contact can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mContact );
    mRecentCallsView->updateContact( filterIndex );
    QVERIFY( mRecentCallsView->mContact );
    
    //contact can't be created
    mRecentCallsView->updateContact( QModelIndex() );
    QVERIFY( !mRecentCallsView->mContact );
}

void UT_LogsRecentCallsView::testActivateEmptyListIndicator()
{
    // Empty model, label is set visible
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QObject* obj = mRepository->findObject( logsRecentViewClearListMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    delete view->mFilter;
    view->mFilter = 0;
    view->mFilter = new LogsFilter();
    QVERIFY( !view->mEmptyListLabel );
    view->activateEmptyListIndicator(view->mFilter);
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( view->mEmptyListLabel->isVisible() );
    QVERIFY(action && !action->isVisible());
    
    // Empty model but effect in dissapeared state, label is set invisible
    view->mEffectInDissappearedPhase = true;
    view->activateEmptyListIndicator(view->mFilter);
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( !view->mEmptyListLabel->isVisible() );
    
    // Model has items, label is set invisible
    view->mEffectInDissappearedPhase = false;
    LogsModel model;
    view->mFilter->setSourceModel(&model);
    view->activateEmptyListIndicator(view->mFilter);
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( !view->mEmptyListLabel->isVisible() );
    QVERIFY(action && action->isVisible());
}

void UT_LogsRecentCallsView::testUpdateMenu()
{
    //menu is not in repository
    mRecentCallsView->updateMenu();
    
    //menu is in repository, empty model => "Clear list" is not visible
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QObject* obj = mRepository->findObject( logsRecentViewClearListMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && action->isVisible());
    HbAction* addToContactsAction = qobject_cast<HbAction*>( 
            mRepository->findObject( logsCommonAddToContactsMenuActionId ) );
    HbAction* contactsSearchAction = qobject_cast<HbAction*>( 
                mRepository->findObject( logsRecentViewContactSearchMenuActionId ) );
    QVERIFY(addToContactsAction && addToContactsAction->isVisible());
    delete view->mFilter;
    view->mFilter = 0;
    view->mFilter = new LogsFilter();
    view->updateMenu();
    QVERIFY(action && !action->isVisible());
    QVERIFY(addToContactsAction && !addToContactsAction->isVisible());
    QVERIFY(contactsSearchAction && !contactsSearchAction->isVisible());
    
    //menu is in repository, non-empty model and dialpad open with content => 
    // "Clear list", "Add to contacts", "Contact search on/off" visible
    view->mDialpad->editor().setText("233141");
    view->mDialpad->mIsOpen = true;
    makeValidFilterIndex(*view);
    view->updateMenu();
    QVERIFY(action && action->isVisible());
    QVERIFY(addToContactsAction && addToContactsAction->isVisible());
    QVERIFY(contactsSearchAction && contactsSearchAction->isVisible());
}


QModelIndex UT_LogsRecentCallsView::makeValidFilterIndex(LogsRecentCallsView& view)
{
    if ( !view.mModel ) {
        return QModelIndex();
    }
    view.mModel->mTextData.append("testdata");
    delete view.mFilter;
    view.mFilter = 0;
    view.mFilter = new LogsFilter( LogsFilter::All );
    view.mFilter->setSourceModel( view.mModel );
    return  view.mFilter->index(0,0);
}

void UT_LogsRecentCallsView::testUpdateWidgetsSizeAndLayout()
{
    //no listView, nothing happens
    QVERIFY( !mRecentCallsView->mListView );
    mRecentCallsView->updateWidgetsSizeAndLayout();
    

    //listView exists, layout and size updated, also menu is updated
    HbListView list;
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    view->mDialpad->closeDialpad();
    view->mListView = &list;
    view->mListView->setLayoutName(QLatin1String("dummy"));
    view->mLayoutSectionName = QLatin1String("dummy");
    view->updateWidgetsSizeAndLayout();
    QVERIFY( view->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( view->mLayoutSectionName == logsViewDefaultSection );
    QVERIFY( view->mListView->layoutName() == logsListDefaultLayout );
    QObject* obj = mRepository->findObject( logsCommonMessageMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && !action->isVisible());
       
    // When dialpad is opened and has input, menu content is different
    view->mDialpad->openDialpad();
    QLatin1String hello("hello");
    view->mDialpad->editor().setText( hello );
    view->updateWidgetsSizeAndLayout();
    QVERIFY(action && action->isVisible());
}


void UT_LogsRecentCallsView::testGetListItemTextWidth()
{
    mRecentCallsView->mListView = new HbListView();
    
    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    
    // Default layout
    mRecentCallsView->mListView->setLayoutName(
                            QLatin1String(logsListDefaultLayout));
    QCOMPARE( mRecentCallsView->getListItemTextWidth(), 200 );
    
    // Landscape layout
    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mRecentCallsView->mListView->setLayoutName(
                            QLatin1String(logsListLandscapeLayout));
    QCOMPARE( mRecentCallsView->getListItemTextWidth(), 206 );
    
    // Landscape with dialpad layout
    mRecentCallsView->mListView->setLayoutName(
                            QLatin1String(logsListLandscapeDialpadLayout));
    QCOMPARE( mRecentCallsView->getListItemTextWidth(), 206 );
    
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
}

void UT_LogsRecentCallsView::testDialpadClosed()
{
    HbListView list;
    mRecentCallsView->mListView = &list;

    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mRecentCallsView->mDialpad->closeDialpad();
    mRecentCallsView->mLayoutSectionName = QString(logsViewLandscapeDialpadSection);
    QString hello("hello");
    mRecentCallsView->mDialpad->editor().setText( hello );
    mRecentCallsView->dialpadClosed();
    QVERIFY( !mRecentCallsView->mDialpad->editor().text().isEmpty()  );
    QVERIFY( mRecentCallsView->mLayoutSectionName == logsViewDefaultSection );
}

void UT_LogsRecentCallsView::testClearList()
{
    // No filter, nothing happens
    HbStubHelper::reset();
    QVERIFY( !HbStubHelper::dialogShown() );
    mRecentCallsView->clearList();
    QVERIFY( !HbStubHelper::dialogShown() );

    // Filter exists, confirmation dialog is shown
    mRecentCallsView->mFilter = new LogsFilter( LogsFilter::Missed );
    mRecentCallsView->clearList();
    QVERIFY( HbStubHelper::dialogShown() );

}

void UT_LogsRecentCallsView::testClearListAnswer()
{
    // No filter ,list is not cleared
    mRecentCallsView->mModel->mIsCleared = false;
    mRecentCallsView->clearListAnswer(HbMessageBox::Ok);
    QVERIFY( !mRecentCallsView->mModel->mIsCleared );

    // Filter exists, cancel button pressed
    mRecentCallsView->mFilter = new LogsFilter(LogsFilter::Missed);
    mRecentCallsView->clearListAnswer(HbMessageBox::Cancel);
    QVERIFY( !mRecentCallsView->mModel->mIsCleared );
    

    // Filter exists, list is cleared
    mRecentCallsView->clearListAnswer(HbMessageBox::Ok);
    QVERIFY( mRecentCallsView->mModel->mIsCleared );
}

void UT_LogsRecentCallsView::testIsExitAllowed()
{
    // Exit allowed, nothing to mark as seen
    mRecentCallsView->handleExit();
    QVERIFY( mRecentCallsView->isExitAllowed() );
    QVERIFY( !mRecentCallsView->mMarkingMissedAsSeen );
    
    // Exit not allowed, marking started
    mRecentCallsView->mModel->mTestIsMarkingNeeded = true;
    mRecentCallsView->handleExit();
    QVERIFY( !mRecentCallsView->isExitAllowed() );
    QVERIFY( mRecentCallsView->mMarkingMissedAsSeen );
    
    // Exit not allowed marking still ongoing
    QVERIFY( !mRecentCallsView->isExitAllowed() );
    QVERIFY( mRecentCallsView->mMarkingMissedAsSeen );
    
    // Complete marking, exit allowed
    QSignalSpy spy( mRecentCallsView, SIGNAL(exitAllowed()) );
    mRecentCallsView->mModel->mTestIsMarkingNeeded = false;
    mRecentCallsView->markingCompleted(0);
    QVERIFY( !mRecentCallsView->mMarkingMissedAsSeen );
    QVERIFY( spy.count() == 1 );
    QVERIFY( mRecentCallsView->isExitAllowed() );
}

void UT_LogsRecentCallsView::testContactSearch()
{
    int status = mRecentCallsView->mModel->predictiveSearchStatus();
    QVERIFY( status == 1 );
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->mDialpad->mIsOpen = true;
    view->mDialpad->editor().setText( "12344" );
    view->updateMenu();
    status = view->mModel->predictiveSearchStatus();
    QVERIFY( status == 1 );
    QObject* obj = mRepository->findObject( logsRecentViewContactSearchMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && action->isVisible());
    QVERIFY(action->text()== hbTrId("txt_dialer_ui_opt_contact_search_off"));     
    
    //set contact search off            
    view->toggleContactSearch();
    status = view->mModel->predictiveSearchStatus();
    QVERIFY( status == 2 );
    obj = mRepository->findObject( logsRecentViewContactSearchMenuActionId );
    action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && action->isVisible());
    QVERIFY(action->text()== hbTrId("txt_dialer_ui_opt_contact_search_on"));  
    
    //set contact search on
    view->toggleContactSearch();
    status = view->mModel->predictiveSearchStatus();
    QVERIFY( status == 1 );
    
    //set contact search permanently off
    view->mModel->mPredectiveSearchStatus = 0;
    view->toggleContactSearch();
    status = view->mModel->predictiveSearchStatus();
    QVERIFY( status == 0 );
    view->updateMenu();
    obj = mRepository->findObject( logsRecentViewContactSearchMenuActionId );
    action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && !action->isVisible());
    
    view->mModel->mPredectiveSearchStatus = 2;
    //set contact search on
    view->toggleContactSearch();
    status = view->mModel->predictiveSearchStatus();
    QVERIFY( status == 1 );
    
}

void UT_LogsRecentCallsView::testDialpadOpened()
{
    mRepository->recentCallsView();
    mRecentCallsView->activated( false, QVariant(XQService::LogsViewAll) );
        
    // If contact search is disabled, opening dialpad containing input
    // does not cause going to matches view
    mViewManager->reset();
    mRecentCallsView->mDialpad->mIsOpen = true;
    mRecentCallsView->mModel->setPredictiveSearch(false);
    mRecentCallsView->mDialpad->editor().setText( QString("hello") );
    mRecentCallsView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    
    // If contact search is enabled, opening dialpad containing input
    // causes going to matches view
    mRecentCallsView->mModel->setPredictiveSearch(true);
    mRecentCallsView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsMatchesViewId );
    
    // Nothing in input field, doe not cause transition to matches view
    // causes going to matches view
    mViewManager->reset();
    mRecentCallsView->mDialpad->editor().setText( "" );
    mRecentCallsView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
}

void UT_LogsRecentCallsView::testSaveActivity()
{
    QByteArray serializedActivity;
    QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
    QVariantHash metaData;
    mRecentCallsView->mCurrentView = XQService::LogsViewMissed;
    QVERIFY( mRecentCallsView->saveActivity(stream, metaData) == QString(logsActivityIdViewMissed) );
    mRecentCallsView->mCurrentView = XQService::LogsViewAll;
    QVERIFY( mRecentCallsView->saveActivity(stream, metaData) == QString(logsActivityIdViewRecent) );

}

void UT_LogsRecentCallsView::testLoadActivity()
{
    QByteArray serializedActivity;
    QDataStream stream(&serializedActivity, QIODevice::ReadOnly);
    QVariantHash metaData;
    QVariant args = mRecentCallsView->loadActivity(QString(logsActivityIdViewCalled), stream, metaData);
    QVERIFY( !args.isNull() );
    QVERIFY( args.toInt() == XQService::LogsViewCalled );
    QVariant args2 = mRecentCallsView->loadActivity(QString(logsActivityIdViewMissed), stream, metaData);
    QVERIFY( !args2.isNull() );
    QVERIFY( args2.toInt() == XQService::LogsViewMissed);
}

