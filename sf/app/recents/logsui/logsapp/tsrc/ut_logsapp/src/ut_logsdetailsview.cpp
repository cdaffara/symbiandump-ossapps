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
#include "logsdetailsview.h"
#include "ut_logsdetailsview.h"
#include "logscomponentrepository.h"
#include "logsdetailsmodel.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logsdefs.h"
#include "logscontact.h"
#include "logsmainwindow.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hblistview.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hblineedit.h>
#include <hbgroupbox.h>
#include <dialpad.h>
#include <hbmessagebox.h>
#include <QGraphicsLinearLayout>

Q_DECLARE_METATYPE(LogsDetailsModel *)


void UT_LogsDetailsView::initTestCase()
{
    mMainWindow = new LogsMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsDetailsView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsDetailsView::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
    mDetailsView = new LogsDetailsView( *mRepository, *mViewManager );
}

void UT_LogsDetailsView::cleanup()
{
    delete mDetailsView;
    delete mRepository;
}

void UT_LogsDetailsView::testConstructor()
{
    QVERIFY(mDetailsView);
    QVERIFY(mDetailsView->viewId() == LogsDetailsViewId );
    QVERIFY(!mDetailsView->mDetailsModel);
    QCOMPARE( mDetailsView->mActivities.at(0), QString(logsActivityIdViewDetails) );
}

void UT_LogsDetailsView::testActivated()
{
    //list widget can't be found from repository
/*    //TODO: Q_ASSERT_X
    LogsDetailsModel* model = new LogsDetailsModel;
    QVariant arg = qVariantFromValue( model );
    mDetailsView->activated(false, arg);
    QVERIFY(!mDetailsView->mListView);
*/
    //list widget is in repository
    LogsDetailsModel* model2 = new LogsDetailsModel;
    delete model2->mEvent;
    model2->mEvent = 0;
    model2->mEvent = new LogsEvent();
    QVariant arg2 = qVariantFromValue( model2 );
    LogsDetailsView* view = mRepository->detailsView();
    QVERIFY( !view->mInitialized );
    view->activated(false, arg2);
    QVERIFY( view->mInitialized );
    QVERIFY(view->mListView);
    QVERIFY(view->mDetailsModel == model2);
    QVERIFY(view->mListView->model() == model2);
    QVERIFY(view->mActionMap.count() == 4);
    
    LogsDetailsModel* model3 = new LogsDetailsModel();
    delete model3->mEvent;
    model3->mEvent = 0;
    model3->mEvent = new LogsEvent();
    model3->mEvent->mIsPrivate = false;
    QVariant arg3 = qVariantFromValue( model3 );
    mDetailsView->activated(false, arg3);
    QVERIFY( mDetailsView->mDetailsModel == model3);
    QVERIFY(view->mListView->model() == model3);
    //QT_NO_DEBUG_OUTPUT
}

void UT_LogsDetailsView::testDeactivated()
{
    // Deactivation of not properly activated view
    LogsDetailsView* view = mRepository->detailsView();
    QVERIFY( !view->mListView );
    view->deactivated();
    QVERIFY( !view->mListView );
    QVERIFY( !view->mDetailsModel );
    
    // Deactivation of properly activated view
    LogsDetailsModel* model = new LogsDetailsModel;
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    QVariant arg = qVariantFromValue( model );
    view->activated(false, arg);
    QVERIFY( view->mListView );
    view->deactivated();
    QVERIFY( view->mListView );
    QVERIFY( !view->mDetailsModel );
}

void UT_LogsDetailsView::testHandleBackSoftkey()
{
    mViewManager->reset();
    mDetailsView->handleBackSoftkey();
    QVERIFY(  mViewManager->mPreviousActivated );
}

void UT_LogsDetailsView::testCallKeyPressed()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->callKeyPressed();
    LogsDetailsModel* model = new LogsDetailsModel();
    
    // Calling possible
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    model->mEvent->mEventType = LogsEvent::TypeVoiceCall;
    QVariant arg = qVariantFromValue( model );
    mRepository->detailsView();
    mDetailsView->activated(false, arg);
    QVERIFY( mDetailsView->mCall );
    mDetailsView->callKeyPressed();
    QVERIFY( mDetailsView->mCall->mTestLastCallType != -1 );
}

void UT_LogsDetailsView::testInitiateVoiceCall()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->initiateVoiceCall();
    
    // Call possible
    mDetailsView->mCall = new LogsCall;
    mDetailsView->initiateVoiceCall();
    QVERIFY( mDetailsView->mCall->mTestLastCallType == LogsCall::TypeLogsVoiceCall );
}

void UT_LogsDetailsView::testInitiateVideoCall()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->initiateVideoCall();
    
    // Call possible
    mDetailsView->mCall = new LogsCall;
    mDetailsView->initiateVideoCall();
    QVERIFY( mDetailsView->mCall->mTestLastCallType == LogsCall::TypeLogsVideoCall );
}

void UT_LogsDetailsView::testUpdateMenu()
{
    // No model, nothing happens
    mDetailsView->updateMenu();

    // No call, call actions disabled
    QVERIFY( !mDetailsView->mCall );
    mRepository->detailsView(); // Set correct object tree
    LogsDetailsModel* model = new LogsDetailsModel();
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    
    QVariant arg = qVariantFromValue( model );
    mDetailsView->activated(false, arg);
    mDetailsView->updateMenu();
    QObject* obj = mRepository->findObject( logsDetailsViewVoiceCallMenuActionId );
    HbAction* voiceCallAction = qobject_cast<HbAction*>( obj );
    QObject* obj2 = mRepository->findObject( logsCommonVideoCallMenuActionId );
    HbAction* videoCallAction = qobject_cast<HbAction*>( obj2 );
    QObject* obj3 = mRepository->findObject( logsCommonMessageMenuActionId );
    HbAction* messageAction = qobject_cast<HbAction*>( obj3 );    
    QVERIFY( !voiceCallAction->isVisible() );
    QVERIFY( !videoCallAction->isVisible() );
    QVERIFY( !messageAction->isVisible() );
    
    // Call exists, call actions enabled
    model->mEvent = new LogsEvent;
    model->mEvent->mEventType = LogsEvent::TypeVoiceCall;
    mDetailsView->updateMenu();
    QVERIFY( voiceCallAction->isVisible() );
    QVERIFY( videoCallAction->isVisible() );
    QVERIFY( messageAction->isVisible() );
    
    // Input in dialpad, add to contacts visible and voice call invisible
    LogsContact::setNextRequestType(LogsContact::TypeLogsContactOpen);
    mDetailsView->mDialpad->mIsOpen = true;
    mDetailsView->mDialpad->editor().setText( QString("3443535") );
    mDetailsView->updateMenu();
    HbAction* addToContactsAction = qobject_cast<HbAction*>( 
            mRepository->findObject( logsCommonAddToContactsMenuActionId ) );
    HbAction* openContactAction = qobject_cast<HbAction*>( 
            mRepository->findObject( logsDetailsOpenContactMenuActionId ) );
    QVERIFY( addToContactsAction->isVisible() );
    QVERIFY( !openContactAction->isVisible() );
    QVERIFY( !voiceCallAction->isVisible() );
    
    // No input in dialpad, open contact visible and voice call visible
    mDetailsView->mDialpad->mIsOpen = false;
    mDetailsView->updateMenu();
    QVERIFY( !addToContactsAction->isVisible() );
    QVERIFY( openContactAction->isVisible() );
    QVERIFY( voiceCallAction->isVisible() );
    LogsContact::reset();
}

void UT_LogsDetailsView::testDeleteEventAnswer()
{
    // No model, nothing happens
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
    QVERIFY( !mDetailsView->mDetailsModel );
    mDetailsView->deleteEventAnswer(HbMessageBox::Ok);
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );

    // Model exists, cancel button pressed
    LogsDetailsModel* model = new LogsDetailsModel();
    mDetailsView->mDetailsModel = model;
    mDetailsView->deleteEventAnswer(HbMessageBox::Cancel);
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );

    // Model exists, call to delete event made and view is closed
    mViewManager->reset();
    mDetailsView->deleteEventAnswer(HbMessageBox::Ok);
    QVERIFY( LogsDetailsModel::mLastCallName == QLatin1String("clearEvent") );
    QVERIFY( mViewManager->mPreviousActivated );
}

void UT_LogsDetailsView::testCopyNumberToClipboard()
{
    LogsDetailsModel* model = new LogsDetailsModel();
    mDetailsView->mDetailsModel = model;
    mDetailsView->copyNumberToClipboard();
    QVERIFY( LogsDetailsModel::mLastCallName == QLatin1String("getNumberToClipboard") );
    
    // Number taken from dialpad if that exists not from details model
    LogsDetailsModel::mLastCallName.clear();
    mDetailsView->mDialpad->mIsOpen = true;
    mDetailsView->mDialpad->mLineEdit->setText("22345");
    mDetailsView->copyNumberToClipboard();
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
}



void UT_LogsDetailsView::testChangeFilter()
{
    mViewManager->reset();
    HbAction action;
    action.setObjectName(logsShowFilterMissedMenuActionId);
    mDetailsView->changeFilter(&action);
    QVERIFY( mViewManager->mViewId == LogsRecentViewId );
    QVERIFY( mViewManager->mArgs.toInt() == (int)XQService::LogsViewAll );
}

void UT_LogsDetailsView::testContactActionCompleted()
{
    // No viewname for some reason, no effect
    HbGroupBox viewName; 
    LogsDetailsModel* model = new LogsDetailsModel();
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    mDetailsView->mDetailsModel = model;
    mDetailsView->contactActionCompleted(true);
    
    // Contact modified, viewname updated accordingly
    mDetailsView->mViewName = &viewName;
    model->mEvent = new LogsEvent;
    model->mEvent->setRemoteParty("someparty");
    mDetailsView->contactActionCompleted(true);
    QVERIFY( mDetailsView->mViewName->heading().length() > 0 );
    
    // No effect if no contact modify occurred
    mDetailsView->mViewName->setHeading("");
    mDetailsView->contactActionCompleted(false);
    QVERIFY( mDetailsView->mViewName->heading().length() == 0 );
}

void UT_LogsDetailsView::testModel()
{
    QVERIFY( !mDetailsView->model() );
    mDetailsView->mDetailsModel = new LogsDetailsModel();
    QVERIFY( mDetailsView->model() );
}

void UT_LogsDetailsView::testUpdateWidgetsSizeAndLayout()
{
    //no listView, nothing happens
    QVERIFY( !mDetailsView->mListView );
    mDetailsView->updateWidgetsSizeAndLayout();
    
    HbListView list;
    //listView exists, layout and size updated
    mDetailsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mDetailsView->mDialpad->openDialpad();
    mDetailsView->mListView = &list;
    mDetailsView->mListView->setLayoutName("dummy");
    mDetailsView->mLayoutSectionName = "dummy";
    mDetailsView->updateWidgetsSizeAndLayout();
    QVERIFY( mDetailsView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mDetailsView->mLayoutSectionName == logsViewLandscapeDialpadSection );
}

void UT_LogsDetailsView::testDialpadEditorTextChanged()
{   
    //text editor is one character long
    LogsDetailsModel* model = new LogsDetailsModel();
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    mDetailsView->mDetailsModel = model;
    mDetailsView->mDetailsModel->setPredictiveSearch(true);
    mDetailsView->mDialpad->editor().setText( QString("h") );
    mDetailsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsMatchesViewId );
    
    mViewManager->reset();
    mDetailsView->mDialpad->mIsCallButtonEnabled = true;
    mDetailsView->mDialpad->editor().setText( QString("") );
    mDetailsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( !mDetailsView->mDialpad->mIsCallButtonEnabled );
    
    // View does not change if contact search is off
    mViewManager->reset();
    mDetailsView->mDialpad->editor().setText( QString("2") );
    mDetailsView->mDetailsModel->setPredictiveSearch(false);
    mDetailsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( mDetailsView->mDialpad->mIsCallButtonEnabled );
}

void UT_LogsDetailsView::testSendMessage()
{
    // No input, message sent using orig message
    LogsMessage::resetTestData();
    LogsMessage* message = new LogsMessage;
    mDetailsView->mMessage = message;
    mDetailsView->sendMessage();
    QVERIFY( LogsMessage::isDefaultMessageSent() );
    QVERIFY( !LogsMessage::isMessageSent() );
    
    // Message sent using input field num
    LogsMessage::resetTestData();
    mDetailsView->mDialpad->mIsOpen = true;
    mDetailsView->mDialpad->editor().setText( QString("3443535") );
    mDetailsView->sendMessage();
    QVERIFY( !LogsMessage::isDefaultMessageSent() );
    QVERIFY( LogsMessage::isMessageSent() );
}

void UT_LogsDetailsView::testSaveActivity()
{
    QByteArray serializedActivity;
    int sizeBefore = serializedActivity.size();
    QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
    QVariantHash metaData;
    QVERIFY( mDetailsView->saveActivity(stream, metaData) == QString(logsActivityIdViewDetails) );
    QVERIFY(serializedActivity.size() == sizeBefore); // Event not serialized as no model
    
    mDetailsView->mDetailsModel = new LogsDetailsModel();
    mDetailsView->mDetailsModel->mEvent = new LogsEvent;
    QVERIFY( mDetailsView->saveActivity(stream, metaData) == QString(logsActivityIdViewDetails) );
    QVERIFY(serializedActivity.size() > sizeBefore); // Event serialized
    
}

void UT_LogsDetailsView::testLoadActivity()
{
    QByteArray serializedActivity;
    QDataStream stream(&serializedActivity, QIODevice::ReadWrite | QIODevice::Append);
    LogsEvent event;
    event.serialize(stream);
    QVariantHash metaData;
    QVariant args = mDetailsView->loadActivity(QString(logsActivityIdViewDetails), stream, metaData);
    QVERIFY( !args.isNull() );
    LogsDetailsModel* model = qVariantValue<LogsDetailsModel*>(args);
    QVERIFY( model != 0 );
    delete model;
}


void UT_LogsDetailsView::testUpdateChildItems()
{
    HbListView list;
    LogsDetailsModel* model = new LogsDetailsModel();
    list.setModel( model, new LogsDetailsViewItem());

    model->mDetailItemsCount = 5;
    QEvent* event = new QEvent(QEvent::GraphicsSceneResize);
    qApp->sendEvent(&list, event); //will create list items and call updateChildItems()
    
    // No separator item
    LogsDetailsViewItem* item = static_cast<LogsDetailsViewItem*>(list.viewItem(0));    
    QVERIFY(item);
    QVERIFY( !item->mLayout );
    
    // Separator item updated OK
    model->mSeparatorIndex = 0;
    item->updateChildItems();
    QVERIFY( item->mLayout );
    QCOMPARE( item->mLayout->count(), 1 );
    QGraphicsLayoutItem* layoutItem = item->mLayout->itemAt(0);
    QVERIFY( static_cast<HbGroupBox*>(layoutItem) );

    // Testing groupBoxClicked()
    QVERIFY( model->mSeparatorCollapsed );
    item->groupBoxClicked(false);
    QVERIFY( !model->mSeparatorCollapsed );
    
    
    list.setModel(0);
    delete model;
}
  
