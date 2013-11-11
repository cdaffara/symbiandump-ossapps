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
#include "ut_logsmodel.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logscall.h"
#include "logscontact.h"
#include "logsdetailsmodel.h"
#include "logsmessage.h"
#include "logseventdata.h"
#include "logsdbconnector.h"
#include "logsdbconnector_stub_helper.h"
#include "logscommondata.h"
#include "logsconfigurationparams.h"
#include "hbstubs_helper.h"

#include <hbicon.h>
#include <QtTest/QtTest>
#include <hbfontspec.h>
#include <xqsettingsmanager.h>

Q_DECLARE_METATYPE(LogsEvent *)
Q_DECLARE_METATYPE(LogsCall *)
Q_DECLARE_METATYPE(LogsDetailsModel *)
Q_DECLARE_METATYPE(LogsMessage *)
Q_DECLARE_METATYPE(LogsContact *)

#define LOGS_TEST_CMP_ICONS( var, icon ){ \
     const HbIcon& tempIcon = qVariantValue<HbIcon>( var );\
     QVERIFY( tempIcon == *icon ); }

#define LOGS_TEST_CREATE_EVENT(eventName, index, eventState ) \
LogsEvent* eventName = new LogsEvent; \
eventName->setIndex(index); \
eventName->setLogId(index);\
eventName->setIsInView(true); \
eventName->mEventState = eventState; \
mModel->mEvents.insert(index, eventName)

void UT_LogsModel::initTestCase()
{
}

void UT_LogsModel::cleanupTestCase()
{
}


void UT_LogsModel::init()
{
    HbStubHelper::reset();
    mModel = new LogsModel();
    LogsCommonData::getInstance().currentConfiguration().setListItemTextWidth(360); 
}

void UT_LogsModel::cleanup()
{
    delete mModel;
    mModel = 0;
}

void UT_LogsModel::testConstructor()
{
    QVERIFY( mModel );
}

void UT_LogsModel::testRowCount()
{
    QVERIFY( mModel->rowCount(QModelIndex()) == 0 );
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    QVERIFY( mModel->rowCount(QModelIndex()) == 1 );
}

void UT_LogsModel::testData()
{
    QVERIFY( mModel->data(QModelIndex(), Qt::DisplayRole).isNull() );
    
    QString number("123");
    LogsEvent* event = new LogsEvent();    
    event->setNumber(number);
    
    mModel->mEvents.append(event);
    
    //display data, no duplicate items
    QVariant displayData = mModel->data(mModel->index(0), Qt::DisplayRole);
    QVERIFY ( displayData.type() == QVariant::StringList );
    QStringList list = displayData.toStringList();
    QVERIFY( list.count() == 2 );
    QVERIFY( list.at(0) == number );
    
    //display data with duplicates
    event->setDuplicates(1);
    displayData = mModel->data(mModel->index(0), Qt::DisplayRole);
    list = displayData.toStringList();
    QVERIFY( list.count() == 2 );
    QVERIFY( list.at(0) == QString("123(2)") );
    
    QVariant decorationData = mModel->data(mModel->index(0), Qt::DecorationRole);
    QVERIFY ( decorationData.canConvert< QList<QVariant> >() );
    
    // Full event
    QVariant fullEventdata = mModel->data(mModel->index(0), LogsModel::RoleFullEvent);
    const LogsEvent *fetchedEvent = qVariantValue<LogsEvent *>( fullEventdata );
    QVERIFY ( fetchedEvent );
    
    // Call not supported for this event
    mModel->mEvents.at(0)->setEventType(LogsEvent::TypeUndefined);
    QVariant callData = mModel->data(mModel->index(0), LogsModel::RoleCall);
    LogsCall *call = qVariantValue<LogsCall *>( callData );
    QVERIFY ( call );
    QVERIFY( call->defaultCallType() == LogsCall::TypeLogsCallNotAvailable );
    delete call;
    
    // Call supported
    mModel->mEvents.at(0)->setEventType(LogsEvent::TypeVoiceCall);
    QVariant callData2 = mModel->data(mModel->index(0), LogsModel::RoleCall);
    LogsCall *call2 = qVariantValue<LogsCall *>( callData2 );
    QVERIFY ( call2 );
    QVERIFY( call2->defaultCallType() != LogsCall::TypeLogsCallNotAvailable );
    delete call2;
    
    // Details model
    QVariant details = mModel->data(mModel->index(0), LogsModel::RoleDetailsModel);
    LogsDetailsModel *detailsModel = qVariantValue<LogsDetailsModel *>( details );
    QVERIFY( detailsModel );
    delete detailsModel;
    
    // Test message
    QVariant messageData = mModel->data(mModel->index(0), LogsModel::RoleMessage);
    LogsMessage* message = qVariantValue<LogsMessage *>( messageData );
    QVERIFY( message != 0 );
    delete message;
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "test@1.2.3.4";
    mModel->mEvents.at(0)->setLogsEventData( eventData );
    QVariant messageData2 = mModel->data(mModel->index(0), LogsModel::RoleMessage);
    LogsMessage* message2 = qVariantValue<LogsMessage *>( messageData2 );
    QVERIFY( message2 == 0 );
    
    // Contact supported
    QVariant contactData = mModel->data(mModel->index(0), LogsModel::RoleContact);
    LogsContact *contact = qVariantValue<LogsContact *>( contactData );
    QVERIFY( contact );
    delete contact;
    
    //contact not supported
    mModel->mEvents.at(0)->setNumber("");
    mModel->mEvents.at(0)->setLogsEventData( 0 );
    contactData = mModel->data(mModel->index(0), LogsModel::RoleContact);
    contact = qVariantValue<LogsContact *>( contactData );
    QVERIFY( !contact );
}

void UT_LogsModel::testDataAdded()
{
    QList<int> addedIndexes;
    QSignalSpy spy(mModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    
    // Empty list
    mModel->dataAdded(addedIndexes);
    QVERIFY( spy.count() == 0 );
        
    // One event added
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event); 
    addedIndexes.append( 0 );

    mModel->dataAdded(addedIndexes);
    QVERIFY( spy.count() == 1 );
    
    // Two more sequnetial events added
    LogsEvent* event2 = new LogsEvent();
    LogsEvent* event3 = new LogsEvent();
    mModel->mEvents.insert(0, event2);
    mModel->mEvents.insert(0, event3);
    addedIndexes.clear();
    addedIndexes.append( 0 );
    addedIndexes.append( 1 );
    mModel->dataAdded(addedIndexes);
    QVERIFY( spy.count() == 2 ); // +one addition sequence
    
    // Three events added, one is non-sequential with other two
    LogsEvent* event4 = new LogsEvent();
    LogsEvent* event5 = new LogsEvent();
    LogsEvent* event6 = new LogsEvent();
    mModel->mEvents.insert(0, event4);
    mModel->mEvents.insert(0, event5);
    mModel->mEvents.append(event6);
    addedIndexes.clear();
    addedIndexes.append( 0 );
    addedIndexes.append( 1 );
    addedIndexes.append( mModel->mEvents.count() - 1 ); // Added to end
    mModel->dataAdded(addedIndexes);
    QVERIFY( spy.count() == 4 ); // +two addition sequences
}

void UT_LogsModel::testDataUpdated()
{
    QList<int> updatedIndexes;
    QSignalSpy spy(mModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    
    // Empty list
    mModel->dataUpdated(updatedIndexes);
    QVERIFY( spy.count() == 0 );
    
    // Something updated
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);

    updatedIndexes.append( 0 );

    mModel->dataUpdated(updatedIndexes);
    QVERIFY( spy.count() == 1 );
}

void UT_LogsModel::testDataRemoved()
{
    QList<int> removedIndexes;
    QSignalSpy spy(mModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    mModel->dataRemoved(removedIndexes);
    QVERIFY( spy.count() == 0 );
    
    removedIndexes.append( 0 ); 
    mModel->dataRemoved(removedIndexes);
    QVERIFY( spy.count() == 1 );
}

void UT_LogsModel::testGetDecorationData()
{
    LogsEvent* event = new LogsEvent();
    mModel->mEvents.append(event);
    QList<QVariant> icons;
    
    //no direction/call type
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    QVERIFY( qVariantValue<QIcon>( icons.at(0) ).isNull() );
    
    icons.clear();
    event->setDirection(LogsEvent::DirIn);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    QVERIFY( qVariantValue<QIcon>( icons.at(0) ).isNull() );
    
    icons.clear();
    event->setDirection(LogsEvent::DirOut);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    QVERIFY( qVariantValue<QIcon>( icons.at(0) ).isNull() );
    
    icons.clear();
    event->setDirection(LogsEvent::DirMissed);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    QVERIFY( qVariantValue<QIcon>( icons.at(0) ).isNull() );
    
    //different directions/call types to get coverage
    icons.clear();
    event->setDirection(LogsEvent::DirIn);
    event->setEventType(LogsEvent::TypeVideoCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsReceivedVideoCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoIPCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsReceivedVoipCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoiceCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);    
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsReceivedVoiceCallIconId ));

    icons.clear();
    event->setDirection(LogsEvent::DirOut);
    event->setEventType(LogsEvent::TypeVideoCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsDialledVideoCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoIPCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsDialledVoipCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoiceCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);    
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsDialledVoiceCallIconId ));

    icons.clear();
    event->setDirection(LogsEvent::DirMissed);
    event->setEventType(LogsEvent::TypeVideoCall);
    event->setIsRead(true);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVideoCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoIPCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVoipCallIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoiceCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);    
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVoiceCallIconId ));
    
    icons.clear();
    event->setDirection(LogsEvent::DirMissed);
    event->setEventType(LogsEvent::TypeVideoCall);
    event->setIsRead(false);	
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVideoCallUnseenIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoIPCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVoipCallUnseenIconId ));
    
    icons.clear();
    event->setEventType(LogsEvent::TypeVoiceCall);
    mModel->getDecorationData(*event, icons);
    QVERIFY(icons.count() == 1);    
    LOGS_TEST_CMP_ICONS(icons.at(0), mModel->mIcons.value( logsMissedVoiceCallUnseenIconId ));
}

void UT_LogsModel::testIconName()
{
    LogsEvent event;
    QVERIFY( LogsModel::typeIconName(event) == "" );
    event.setEventType( LogsEvent::TypeVideoCall );
    QVERIFY( LogsModel::typeIconName(event) == logsVideoCallIconId );
    event.setEventType( LogsEvent::TypeVoIPCall );
    QVERIFY( LogsModel::typeIconName(event) == logsVoipCallIconId );
    event.setEventType( LogsEvent::TypeVoiceCall );
    QVERIFY( LogsModel::typeIconName(event) == logsVoiceCallIconId );
    
    QVERIFY( LogsModel::directionIconName(event) == "" );
    event.setDirection(LogsEvent::DirIn);
    event.setEventType( LogsEvent::TypeUndefined );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    event.setEventType( LogsEvent::TypeVoiceCall );
    QVERIFY( LogsModel::directionIconName(event) == logsReceivedVoiceCallIconId );
    event.setEventType( LogsEvent::TypeVoIPCall );
    QVERIFY( LogsModel::directionIconName(event) == logsReceivedVoipCallIconId );
    event.setEventType( LogsEvent::TypeVideoCall );
    QVERIFY( LogsModel::directionIconName(event) == logsReceivedVideoCallIconId );

    event.setDirection(LogsEvent::DirOut);
    event.setEventType( LogsEvent::TypeUndefined );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    event.setEventType( LogsEvent::TypeVoiceCall );
    QVERIFY( LogsModel::directionIconName(event) == logsDialledVoiceCallIconId );
    event.setEventType( LogsEvent::TypeVoIPCall );
    QVERIFY( LogsModel::directionIconName(event) == logsDialledVoipCallIconId );
    event.setEventType( LogsEvent::TypeVideoCall );
    QVERIFY( LogsModel::directionIconName(event) == logsDialledVideoCallIconId );

    event.setDirection(LogsEvent::DirMissed);
    event.setEventType( LogsEvent::TypeUndefined );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    event.setEventType( LogsEvent::TypeVoiceCall );
    event.setIsRead(true);
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVoiceCallIconId );
    event.setEventType( LogsEvent::TypeVoIPCall );
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVoipCallIconId );
    event.setEventType( LogsEvent::TypeVideoCall );
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVideoCallIconId );
    
    event.setDirection(LogsEvent::DirMissed);
    event.setEventType( LogsEvent::TypeUndefined );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    QVERIFY( LogsModel::directionIconName(event) == "" );
    event.setEventType( LogsEvent::TypeVoiceCall );
    event.setIsRead(false);
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVoiceCallUnseenIconId );
    event.setEventType( LogsEvent::TypeVoIPCall );
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVoipCallUnseenIconId );
    event.setEventType( LogsEvent::TypeVideoCall );
    QVERIFY( LogsModel::directionIconName(event) == logsMissedVideoCallUnseenIconId );
    
}

void UT_LogsModel::testGetCallerId()
{
    HbStubHelper::stringUtilDigitConversion(true);
    
    //add private and unknown
    // No name or number
    LogsEvent event;
    QVERIFY( mModel->getCallerId(event) == QString("") );

    // No name
    QString num("+12345555");
    event.setNumber(num);
    QCOMPARE( mModel->getCallerId(event), "conv" + num );
    
    // No number
    QString remote("Souuu");
    event.setRemoteParty(remote);
    event.setNumber("");
    QVERIFY( mModel->getCallerId(event) == remote );
    
    // Both, still use remote
    event.setNumber(num);
    QVERIFY( mModel->getCallerId(event) == remote );
    
    // Only remote url
    
    event.setNumber("");
    event.setRemoteParty("");
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "test@1.2.3.4";
    event.setLogsEventData( eventData );
    QVERIFY( mModel->getCallerId(event) == eventData->mRemoteUrl );
    
    // Duplicates
    event.setDuplicates(3);
    QVERIFY( mModel->getCallerId(event) == "test@1.2.3.4(conv4)" );
    
    // Duplicates for already read event
    event.setIsRead(true);
    QVERIFY( mModel->getCallerId(event) == "test@1.2.3.4" );
    HbStubHelper::reset();
}

void UT_LogsModel::testSqueezedString()
{
    QFontMetricsF fontMetrics(HbFontSpec(HbFontSpec::Primary).font());
    qreal maxwidth =  LogsCommonData::getInstance().currentConfiguration().listItemTextWidth();
    QString baseString = "4124";
    QString secondaryString = "(3)";
    qreal stringWidth = fontMetrics.width(baseString+secondaryString);
    
    //test with string that fits
    QVERIFY (mModel->SqueezedString(baseString,secondaryString,stringWidth + 10) == "4124(3)"); 
    
    //test with string that needs to be cutted
    QString squeezed = mModel->SqueezedString(baseString,secondaryString,stringWidth - 10);
    QVERIFY (squeezed.endsWith("...(3)")); 
    
    //test with empty basestring
    QVERIFY (mModel->SqueezedString("",secondaryString,stringWidth) == "(3)"); 
    
    //test with empty secondarystring
    QVERIFY (mModel->SqueezedString(baseString,"",stringWidth) == "4124"); 
}

void UT_LogsModel::testClearList()
{
    mModel->clearList(LogsModel::TypeLogsClearAll);
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction() == "clearList" );
}

void UT_LogsModel::testMarkEventsSeen()
{
    // No events
    mModel->markEventsSeen(LogsModel::TypeLogsClearMissed);
    
    // No missed events
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventAdded );
    event->setDirection(LogsEvent::DirIn);
    QVERIFY( !mModel->markEventsSeen(LogsModel::TypeLogsClearMissed) );
    QVERIFY( mModel->mDbConnector->mEventsSeen.count() == 0 );
    
    // Marking missed as seen (one is already seen)
    event->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event2, 1, LogsEvent::EventAdded );
    event2->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event3, 2, LogsEvent::EventAdded );
    event3->setDirection(LogsEvent::DirMissed);
    event3->setIsRead(true);
    QVERIFY( mModel->markEventsSeen(LogsModel::TypeLogsClearMissed) );
    QVERIFY( mModel->mDbConnector->mEventsSeen.count() == 2 );
    
    // Trying to clear missed again, id is appended to mark list
    event3->setIsRead(false);
    QVERIFY( mModel->markEventsSeen(LogsModel::TypeLogsClearMissed) );
    QVERIFY( mModel->mDbConnector->mEventsSeen.count() == 3 );
    
    // Clearing for the events already ongoing, don't try again
    event->markedAsSeenLocally(true);
    event2->markedAsSeenLocally(true);
    event3->markedAsSeenLocally(true);
    QVERIFY( !mModel->markEventsSeen(LogsModel::TypeLogsClearMissed) );  
    
    // Clearing received, does not find such
    mModel->mDbConnector->mEventsSeen.clear();
    QVERIFY( !mModel->markEventsSeen(LogsModel::TypeLogsClearReceived) );
    QVERIFY( mModel->mDbConnector->mEventsSeen.count() == 0 );
    
    // Clearing called, does not find such
    QVERIFY( !mModel->markEventsSeen(LogsModel::TypeLogsClearCalled) );
    QVERIFY( mModel->mDbConnector->mEventsSeen.count() == 0 );
}

void UT_LogsModel::testClearMissedCallsCounter()
{
    XQSettingsManager::mFailed = false;
    XQSettingsManager::mCurrentVal = 1;
    QVERIFY( mModel->clearMissedCallsCounter() == 0 );
}

void UT_LogsModel::testRefreshData()
{
    QVERIFY( mModel->refreshData() == 0 );
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction() == "refreshData" );
}

void UT_LogsModel::testCompressData()
{
    QVERIFY( mModel->compressData() == 0 );
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction() == "compressData" );
}

void UT_LogsModel::testPredictiveSearchStatus()
{
    LogsCommonData::getInstance().freeCommonData();
    XQSettingsManager::mCurrentVal = 1;
    QVERIFY( mModel->predictiveSearchStatus() == 1 );
}

void UT_LogsModel::testSetPredictiveSearch()
{
    XQSettingsManager::mCurrentVal = 2;
    QVERIFY( mModel->setPredictiveSearch(true) == 0 );
    QVERIFY( mModel->predictiveSearchStatus() == 1 );
}

void UT_LogsModel::testUpdateConfiguration()
{   
    QSignalSpy spy(mModel, SIGNAL(modelReset()));
    QSignalSpy spy2(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    
    // No previous config, reset not signaled
    LogsConfigurationParams params;
    params.setListItemTextWidth(200);
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 0 );
    
    // Previous config but no change, reset not signaled
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 0 );
    
    // Config changed but no unseen events, reset not signaled
    params.setListItemTextWidth(400);
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 0 );
    
    // Config changed and unseen events, reset signaled
    LOGS_TEST_CREATE_EVENT(event, 1, LogsEvent::EventAdded );
    event->setDirection(LogsEvent::DirMissed);
    event->setDuplicates(10);
    params.setListItemTextWidth(300);
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 1 );
    QVERIFY( spy2.count() == 0 );
        
    //Locale changed, model not empty
    spy.clear();
    params.setLocaleChanged(true);
    QVERIFY( mModel->rowCount(QModelIndex()) == 1 );
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 1 );
    
    // Locale changed, model is empty
    spy.clear();
    spy2.clear();
    qDeleteAll( mModel->mEvents );
    mModel->mEvents.clear();
    QVERIFY( mModel->rowCount(QModelIndex()) == 0 );
    mModel->updateConfiguration(params);
    QVERIFY( spy.count() == 0 );
    QVERIFY( spy2.count() == 0 );    
}
