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
#include "logsbaseview.h"
#include "ut_logsbaseview.h"
#include "logscomponentrepository.h"
#include "qthighway_stub_helper.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "hbstubs_helper.h"
#include "logsmodel.h"
#include "logsdetailsmodel.h"
#include "logsmainwindow.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hblistviewitem.h>
#include <hblistview.h>
#include <hbmessagebox.h>
#include <QStringListModel>
#include <hbapplication.h>
#include <hbactivitymanager.h>

void UT_LogsBaseView::initTestCase()
{
    mMainWindow = new LogsMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsBaseView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsBaseView::init()
{
    mRepository = new LogsComponentRepository( *mViewManager );
    mBaseView = new LogsBaseView( LogsRecentViewId, *mRepository, *mViewManager );
    HbStubHelper::reset();
}

void UT_LogsBaseView::cleanup()
{
    delete mBaseView;
    mBaseView = 0;
    delete mRepository;
    mRepository = 0;
}

void UT_LogsBaseView::testConstructor()
{
    QVERIFY( mBaseView );
    QVERIFY( !mBaseView->mShowFilterMenu );
    QVERIFY( !mBaseView->mInitialized );
    QVERIFY( mBaseView->mActionMap.isEmpty() );
    QVERIFY( mBaseView->viewId() == LogsRecentViewId );
    QVERIFY( mBaseView->mDialpad );
    QVERIFY( !mBaseView->mCall );
    QVERIFY( !mBaseView->mMessage );
    QVERIFY( !mBaseView->mContact );
    QVERIFY( !mBaseView->mCallTypeMapper );
    QVERIFY( !mBaseView->mOptionsMenu );
}

void UT_LogsBaseView::testActivated()
{
    mBaseView->activated(false, QVariant());
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    QVERIFY( mBaseView->mInitialized );
    QVERIFY( !mBaseView->mShowFilterMenu );
    QVERIFY( mBaseView->mActionMap.count() == 4 );

    mBaseView->activated(true, QVariant());
    QVERIFY( mBaseView->mDialpad->isOpen() );
}

void UT_LogsBaseView::testDeactivated()
{
    mBaseView->deactivated(); // NOP
}

void UT_LogsBaseView::testResetView()
{
    mBaseView->resetView(); // NOP
}

void UT_LogsBaseView::testShowFilterMenu()
{
    HbStubHelper::reset();
    mBaseView->setLayoutDirection(Qt::LeftToRight);
    
    //no context menu
    QVERIFY( !mBaseView->mShowFilterMenu );
    mBaseView->showFilterMenu();
    QVERIFY( !HbStubHelper::menuShown() );
    
    //contect menu exists
    mBaseView->mShowFilterMenu = new HbMenu();
    QVERIFY( mBaseView->mShowFilterMenu );
    mBaseView->showFilterMenu();
    QVERIFY( HbStubHelper::menuShown() ); 
    QVERIFY( HbStubHelper::menuShownPlacement() == HbPopup::BottomRightCorner );
    
    // RTL layout dir
    HbStubHelper::reset();
    mBaseView->setLayoutDirection(Qt::RightToLeft);
    mBaseView->showFilterMenu();
    QVERIFY( HbStubHelper::menuShown() );
    QVERIFY( HbStubHelper::menuShownPlacement() == HbPopup::BottomLeftCorner );
    
    delete mBaseView->mShowFilterMenu;
    mBaseView->mShowFilterMenu = 0;
    
}

void  UT_LogsBaseView::testOpenDialpad()
{
    QVERIFY( !mBaseView->mDialpad->isOpen() );
    mBaseView->openDialpad();
    QVERIFY( mBaseView->mDialpad->isOpen() );    
}

void  UT_LogsBaseView::testSetDialpadPostion()
{
    QPointF pos = mBaseView->mDialpad->pos();
    HbMainWindow& window = mBaseView->mViewManager.mainWindow();
    
    window.setOrientation( Qt::Horizontal );
    mBaseView->setDialpadPosition();
    QVERIFY( pos != mBaseView->mDialpad->pos() );
    pos = mBaseView->mDialpad->pos();
    
    window.setOrientation( Qt::Vertical );
    mBaseView->setDialpadPosition();
    QVERIFY( pos != mBaseView->mDialpad->pos() );
}

void  UT_LogsBaseView::testDialpadClosed()
{
    mBaseView->mDialpad->editor().setText( QString("hello") );
    mBaseView->dialpadClosed();
    QVERIFY( !mBaseView->mDialpad->editor().text().isEmpty() );
}

void  UT_LogsBaseView::testDialpadOpened()
{
    // Base view impl never goes to matches view as logsModel is null
    //
    
    mBaseView->mRepository.model()->setPredictiveSearch(false);
    mBaseView->mDialpad->editor().setText( QString("hello") );
    mBaseView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    
    mBaseView->mRepository.model()->setPredictiveSearch(true);
    mBaseView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );

    mViewManager->reset();
    mBaseView->mDialpad->editor().setText( "" );
    mBaseView->dialpadOpened();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
}

void  UT_LogsBaseView::testDialpadEditorTextChanged()
{      
    // View does not change if contact search is off
    mViewManager->reset();
    mBaseView->mDialpad->editor().setText( QString("2") );
    mBaseView->mRepository.model()->setPredictiveSearch(false);
    mBaseView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( mBaseView->mDialpad->mIsCallButtonEnabled );
}

void  UT_LogsBaseView::testChangeFilter()
{
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    HbAction*  action = new HbAction();
    action->setObjectName(logsShowFilterMissedMenuActionId);
    mBaseView->changeFilter(action);
    QVERIFY( mViewManager->mViewId == LogsRecentViewId );
}

void UT_LogsBaseView::testModel()
{
    QVERIFY( !mBaseView->model() );
}

void UT_LogsBaseView::testOpenContactsApp()
{
    QtHighwayStubHelper::reset();
    mBaseView->openContactsApp();
    QVERIFY( QtHighwayStubHelper::service() == "com.nokia.services.phonebookappservices.Launch" );
    QVERIFY( QtHighwayStubHelper::message() == "launch()" );
}


void UT_LogsBaseView::testInitiateCallback()
{
    LogsCall::resetTestData();
    //call can't be created
    QVERIFY( !mBaseView->mCall );
    mBaseView->initiateCallback(QModelIndex());
    QVERIFY( LogsCall::lastCalledFunction().isEmpty() );
    
    //call can be created should be tested in derived class    
}

void UT_LogsBaseView::testInitiateCall()
{
    LogsCall::resetTestData();
    //no call
    QVERIFY( !mBaseView->mCall );
    mBaseView->initiateCall(LogsCall::TypeLogsVoiceCall);
    QVERIFY( LogsCall::lastCalledFunction().isEmpty() );
    
    //call exists
    mBaseView->mCall = new LogsCall();
    mBaseView->initiateCall(LogsCall::TypeLogsVideoCall);
    QVERIFY( LogsCall::lastCalledFunction() == QString("call") );
    QVERIFY( mBaseView->mCall->mTestLastCallType == LogsCall::TypeLogsVideoCall );
}

void UT_LogsBaseView::testShowListItemMenu()
{
    HbStubHelper::reset();

    mBaseView->mDialpad->openDialpad();
    //menu doen't have any actions
    HbListViewItem* item = new HbListViewItem();
    mBaseView->showListItemMenu( item, QPointF() );
    QVERIFY( HbStubHelper::widgetActionsCount() == 0 );
    QVERIFY( !HbStubHelper::menuShown() );
    //check that dialpad has closed
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    
    //menu has actions and can be executed should be tested in derived class
 }

void UT_LogsBaseView::testPopulateListItemMenu()
{
    HbStubHelper::reset();
    //no call/message/contact, menu has not actions
    HbMenu menu;
    mBaseView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 0 );
    
    //call, message and contact exist, menu has actions
    mBaseView->mCall = new LogsCall();
    mBaseView->mMessage = new LogsMessage();
    mBaseView->mContact = new LogsContact();
    mBaseView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 5 );
}

void UT_LogsBaseView::testUpdateMenuVisibility()
{
    HbMenu* menu = new HbMenu;
    HbAction* action = menu->addAction("action1");
    mBaseView->setMenu(menu);
    
    // No visible actions in menu => menu is replaced by empty one
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(!mBaseView->menu()->isEmpty());
    action->setVisible(false);
    mBaseView->updateMenuVisibility();
    QVERIFY(mBaseView->menu()->isEmpty());
    QVERIFY(mBaseView->mOptionsMenu == menu);
    QVERIFY(mBaseView->menu() != menu);
  
    // Visible action exists => restoring the menu
    action->setVisible(true);
    QVERIFY(mBaseView->mOptionsMenu);
    mBaseView->updateMenuVisibility();
    QVERIFY(!mBaseView->menu()->isEmpty());
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(mBaseView->menu() == menu);
 
    // Menu has no actions => menu is replaced by empty one
    mBaseView->setMenu(0);
    menu = mBaseView->menu();
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(mBaseView->menu()->isEmpty());
    mBaseView->updateMenuVisibility();
    QVERIFY(mBaseView->menu()->isEmpty());
    QVERIFY(mBaseView->mOptionsMenu == menu);
    QVERIFY(mBaseView->menu() != menu);   
}

void UT_LogsBaseView::testSetMenuVisible()
{
    HbMenu* menu = new HbMenu;
    menu->addAction("action1");
    mBaseView->setMenu(menu);
    
    // Hiding menu, view menu is replaced by empty menu
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(!mBaseView->menu()->isEmpty());
    mBaseView->setMenuVisible(false);
    QVERIFY(mBaseView->menu()->isEmpty());
    QVERIFY(mBaseView->mOptionsMenu == menu);
    QVERIFY(mBaseView->menu() != menu);
        
    // Trying to hide again, nothing happens
    mBaseView->setMenuVisible(false);
    QVERIFY(mBaseView->menu()->isEmpty());
    QVERIFY(mBaseView->mOptionsMenu == menu);
    QVERIFY(mBaseView->menu() != menu);
    
    // Showing menu, original menu is restored
    mBaseView->setMenuVisible(true);
    QVERIFY(!mBaseView->menu()->isEmpty());
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(mBaseView->menu() == menu);
    
    // Showing again, nothing happen
    mBaseView->setMenuVisible(true);
    QVERIFY(!mBaseView->menu()->isEmpty());
    QVERIFY(!mBaseView->mOptionsMenu);
    QVERIFY(mBaseView->menu() == menu);    
}

void UT_LogsBaseView::testSaveContact()
{
    //no contact, no actions
    mBaseView->saveContact();
    
    //contact exists, popup would be shown
    mBaseView->mContact = new LogsContact();
    mBaseView->saveContact();    
}

void UT_LogsBaseView::testUpdateCall()
{
    mBaseView->mCall = new LogsCall();
    mBaseView->updateCall( QModelIndex() );
    QVERIFY( !mBaseView->mCall );
}

void UT_LogsBaseView::testUpdateMessage()
{    
    mBaseView->mMessage = new LogsMessage();
    mBaseView->updateMessage( QModelIndex() );
    QVERIFY( !mBaseView->mMessage );
}

void UT_LogsBaseView::testUpdateContact()
{
    mBaseView->mContact = new LogsContact();
    mBaseView->updateContact( QModelIndex() );
    QVERIFY( !mBaseView->mContact );
}

void UT_LogsBaseView::testActivateEmptyListIndicator()
{
    QStringListModel model;
    
    // Label not in repository
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( !mBaseView->mEmptyListLabel );
    
    // Empty model, label is set visible
    mRepository->recentCallsView();
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( mBaseView->mEmptyListLabel );
    
    // Label exists already, situation does not change
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( mBaseView->mEmptyListLabel );
    
    // Null list does nothing
    mBaseView->activateEmptyListIndicator(0);
    QVERIFY( mBaseView->mEmptyListLabel );
}

void UT_LogsBaseView::testDeactivateEmptyListIndicator()
{
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->deactivateEmptyListIndicator(0);
    HbLabel label;
    mBaseView->mEmptyListLabel = &label;
    mBaseView->deactivateEmptyListIndicator(0);
    QVERIFY( mBaseView->mEmptyListLabel ); // Label not removed at deactivation
}

void UT_LogsBaseView::testUpdateEmptyListLabelVisibility()
{
     // Base view does not have model, nothing happens
    mBaseView->updateEmptyListLabelVisibility();
}

void UT_LogsBaseView::testUpdateListLayoutName()
{
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    HbListView list;

    //default layout
    list.setLayoutName("dummy");
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListDefaultLayout );
    
    //same layout, not updated
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListDefaultLayout );
    
    //landscape without dialpad
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListLandscapeLayout );
    
    //landscape with dialpad
    mBaseView->mDialpad->openDialpad();
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListLandscapeDialpadLayout );
    
    //landscape with dialpad, ignoreDialpad is true
    mBaseView->updateListLayoutName(list, true);
    QVERIFY( list.layoutName() == logsListDefaultLayout );

    //landscape without dialpad, ignoreDialpad is true
    mBaseView->mDialpad->closeDialpad();
    mBaseView->updateListLayoutName(list, true);
    QVERIFY( list.layoutName() == logsListLandscapeLayout );
}

void UT_LogsBaseView::testUpdateListSize()
{
    mBaseView->mLayoutSectionName = "dummy";
    
    //default section is loaded
    HbListView view;
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mBaseView->mDialpad->closeDialpad();
    mBaseView->updateListSize(view);
    QVERIFY( mBaseView->mLayoutSectionName == logsViewDefaultSection );
    
    //same section again, not loaded
    mBaseView->updateListSize(view);
    QVERIFY( mBaseView->mLayoutSectionName == logsViewDefaultSection );

    //portrait with dialpad
    mBaseView->mDialpad->openDialpad();
    mBaseView->updateListSize(view);
    QVERIFY( mBaseView->mLayoutSectionName == logsViewPortraitDialpadSection );
    
    //landscape with dialpad
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mBaseView->updateListSize(view);
    QVERIFY( mBaseView->mLayoutSectionName == logsViewLandscapeDialpadSection );
}

void UT_LogsBaseView::testHandleOrientationChanged()
{
    //dialpad position recalculated
    QPointF pos;
    mBaseView->mDialpad->setPos(pos);    
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mBaseView->handleOrientationChanged();
    QVERIFY( pos != mBaseView->mDialpad->pos() );
}

void UT_LogsBaseView::testHandleExit()
{
    mViewManager->reset();
    mBaseView->handleExit();
    QVERIFY( !mViewManager->mExitCalled );
}

void UT_LogsBaseView::testIsExitAllowed()
{
    QVERIFY( mBaseView->isExitAllowed() );
}

void UT_LogsBaseView::testAddToContacts()
{
    // Has dialpad input, contact is saved with that num
    mBaseView->mDialpad->mIsOpen = true;
    mBaseView->mDialpad->mLineEdit->setText("3344");
    mBaseView->addToContacts();
    QVERIFY( mBaseView->mContact->mNumber == "3344" );
    
    // No dialpad input, contact saved using list item if such exists
    mBaseView->mDialpad->mIsOpen = false;
    mBaseView->mContact->mNumber = "1234";
    mBaseView->addToContacts();
    QVERIFY( mBaseView->mContact->mNumber == "1234" );
    
    // No contact at all
    delete mBaseView->mContact;
    mBaseView->mContact = 0;
    mBaseView->addToContacts();
    QVERIFY( !mBaseView->mContact );
}

void UT_LogsBaseView::testSendMessageToCurrentNum()
{
    // No input
    LogsMessage::resetTestData();
    mBaseView->mDialpad->mIsOpen = false;
    mBaseView->mDialpad->mLineEdit->setText("");
    mBaseView->sendMessageToCurrentNum();
    QVERIFY( !LogsMessage::isMessageSent() );
    
    // Input, check that conversion to western digits is done
    QString number("4546626262");
    HbStubHelper::stringUtilDigitConversion(true);
    mBaseView->mDialpad->mIsOpen = true;
    mBaseView->mDialpad->mLineEdit->setText(number);
    mBaseView->sendMessageToCurrentNum();
    QVERIFY( LogsMessage::isMessageSent() );
    QCOMPARE( LogsMessage::sentToNumber(), QString("conv") + number );
}

void UT_LogsBaseView::testSaveNumberInDialpadToContacts()
{
    // No input
    mBaseView->mDialpad->mIsOpen = false;
    mBaseView->mDialpad->mLineEdit->setText("");
    mBaseView->saveNumberInDialpadToContacts();
    QVERIFY( !mBaseView->mContact );
    
    // Input, check that conversion to western digits is done
    QString number("4546626262");
    HbStubHelper::stringUtilDigitConversion(true);
    mBaseView->mDialpad->mIsOpen = true;
    mBaseView->mDialpad->mLineEdit->setText(number);
    mBaseView->saveNumberInDialpadToContacts();
    QVERIFY( mBaseView->mContact );
    QCOMPARE( mBaseView->mContact->mNumber, QString("conv") + number );
}

void UT_LogsBaseView::testDeleteEvent()
{
    // No model, nothing happens
    QVERIFY( !HbStubHelper::dialogShown() );
    QVERIFY( !mBaseView->mDetailsModel );
    mBaseView->deleteEvent();
    QVERIFY( !HbStubHelper::dialogShown() );

    // Model exists, confirmation dialog is shown
    mViewManager->reset();
    LogsDetailsModel* model = new LogsDetailsModel();
    mBaseView->mDetailsModel = model;
    mBaseView->deleteEvent();
    QVERIFY( HbStubHelper::dialogShown() );
}

void UT_LogsBaseView::testDeleteEventAnswer()
{
    // No model, nothing happens
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
    QVERIFY( !mBaseView->mDetailsModel );
    mBaseView->deleteEventAnswer(HbMessageBox::Ok);
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );

    // Model exists, cancel button pressed
    LogsDetailsModel* model = new LogsDetailsModel();
    mBaseView->mDetailsModel = model;
    mBaseView->deleteEventAnswer(HbMessageBox::Cancel);
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
    
    // Model exists, call to delete event made and view is closed
    mViewManager->reset();
    mBaseView->deleteEventAnswer(HbMessageBox::Ok);
    QVERIFY( LogsDetailsModel::mLastCallName == QLatin1String("clearEvent") );
    QVERIFY( !mViewManager->mPreviousActivated );    
}

void UT_LogsBaseView::testMatchWithActivityId()
{
    QVERIFY( !mBaseView->matchWithActivityId(QString("")) );
    QVERIFY( !mBaseView->matchWithActivityId(QString("somedummy")) );
    mBaseView->mActivities.append( "testActivity1" );
    mBaseView->mActivities.append( "testActivity2" );
    QVERIFY( !mBaseView->matchWithActivityId(QString("")) );
    QVERIFY( !mBaseView->matchWithActivityId(QString("somedummy")) );
    QVERIFY( mBaseView->matchWithActivityId(QString("testActivity2")) );   
}

void UT_LogsBaseView::testSaveActivity()
{
    QDataStream serializedActivity; 
    QVariantHash metaData;
    QVERIFY( mBaseView->saveActivity(serializedActivity, metaData).isEmpty() );
    mBaseView->mActivities.append( "testActivity1" );
    QVERIFY( mBaseView->saveActivity(serializedActivity, metaData) == QString("testActivity1") );
}

void UT_LogsBaseView::testLoadActivity()
{
    QDataStream serializedActivity; 
    QVariantHash metaData;
    QVERIFY( mBaseView->loadActivity(QString("dummy"), serializedActivity, metaData).isNull() );
}

void UT_LogsBaseView::testClearActivity()
{
    HbStubHelper::reset();
    HbActivityManager* manager = static_cast<HbApplication*>(qApp)->activityManager();
    manager->addActivity("someact", QVariant(), QVariantHash());
    QCOMPARE( manager->activities().count(), 1 );
    mBaseView->mActivities.append( "testActivity1" );
    mBaseView->clearActivity(*manager);
    QCOMPARE( manager->activities().count(), 0 );
}

void UT_LogsBaseView::testEnsureListPositioning()
{
    HbStubHelper::reset();
    HbListView list;
    QStringList itemTexts;
    itemTexts.append( "foo1" );
    itemTexts.append( "foo2" );
    QStringListModel model;
    model.setStringList(itemTexts);
    
    itemTexts.append( "foo3" );
    itemTexts.append( "foo4" );
    QStringListModel model2;
    model2.setStringList(itemTexts);
    
    list.setModel(&model);
    
    // No content found, nop
    mBaseView->ensureListPositioning(list, false);
    QVERIFY( !HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollToCalled() );
    
    // Content found, no visible items, nop
    mRepository->recentCallsView();
    mBaseView->activated(false, QVariant());
    mBaseView->ensureListPositioning(list, false);
    QVERIFY( !HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollBarPolicySet() );
    
    // Content found and visible items which can fit the screen, height of item zero, nop
    HbWidget* content = 
            qobject_cast<HbWidget*>( mRepository->findWidget( logsContentId ) );
    content->setContentsMargins(0,0,0,0);
    content->setGeometry( QRectF(0,0,100,200) );
    
    HbListViewItem* item = new HbListViewItem;
    HbStubHelper::listItems().append(item);
    HbListViewItem* item2 = new HbListViewItem;
    HbStubHelper::listItems().append(item2);
    mBaseView->ensureListPositioning(list, false);
    QVERIFY( !HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollToCalled() );
    QVERIFY( !HbStubHelper::listScrollBarPolicySet() );
    
    // Screen is already filled with items, scrolled to first fully visible item
    list.setModel(&model2);
    HbListViewItem* item3 = new HbListViewItem;
    HbStubHelper::listItems().append(item3);
    item->setGeometry(QRectF(0,0,100,100));
    item2->setGeometry(QRectF(0,0,100,100));
    item3->setGeometry(QRectF(0,0,100,100));
    mBaseView->ensureListPositioning(list, false);
    QVERIFY( !HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollToCalled() ); // Should be true but stub does not work due virtual method definition
    // List size not decreased, therefore no scrollbar force
    QVERIFY( !HbStubHelper::listScrollBarPolicySet() );
       
    // Content found and visible items which can fit the screen, all items ensured visible
    HbStubHelper::resetListScrolling();
    list.setModel(&model);
    delete HbStubHelper::listItems().takeLast();
    mBaseView->ensureListPositioning(list, true);
    QVERIFY( HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollToCalled() );
    // List size decreased but items fit the screen, therefore no scrollbar force
    QVERIFY( !HbStubHelper::listScrollBarPolicySet() );
    
    // Content found and more visible items which can fit the screen and currently screen
    // is not full of items, ensure that area is filled by scrolling 
    HbStubHelper::resetListScrolling();
    list.setModel(&model2);
    list.setCurrentIndex(model2.index(0,0));
    delete HbStubHelper::listItems().takeLast();
    mBaseView->ensureListPositioning(list, true);
    QVERIFY( !HbStubHelper::listEnsureVisibleCalled() );
    QVERIFY( !HbStubHelper::listScrollToCalled() ); // Should be true but stub does not work due virtual method definition
    // List size decreased and all items do not fit the screen, therefore scrollbar force
    QVERIFY( HbStubHelper::listScrollBarPolicySet() );
}

void UT_LogsBaseView::testCancelServiceRequest()
{
    QVERIFY( !mBaseView->mContact );
    QVERIFY( !LogsContact::mServiceRequestCanceled );
    mBaseView->cancelServiceRequest();
    QVERIFY( !LogsContact::mServiceRequestCanceled );

    mBaseView->mContact = new LogsContact();
    mBaseView->cancelServiceRequest();
    QVERIFY( LogsContact::mServiceRequestCanceled );
}
