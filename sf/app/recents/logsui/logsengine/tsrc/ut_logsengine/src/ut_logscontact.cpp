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
#include "ut_logscontact.h"
#include "logscontact.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsdbconnector.h"
#include "logsdbconnector_stub_helper.h"
#include "qtcontacts_stubs_helper.h"
#include "qthighway_stub_helper.h"

#include <xqaiwrequest.h>
#include <QtTest/QtTest>

const int logsContactsLocalIdTest1 = 1;
const char logsIContactsView[] = "com.nokia.symbian.IContactsView";
const char logsIContactsEdit[] = "com.nokia.symbian.IContactsEdit";

const int groupId = 99;


void UT_LogsContact::initTestCase()
{
    mDbConnector = new LogsDbConnector(mEvents);
    mLogsContact = 0;
}

void UT_LogsContact::cleanupTestCase()
{
    delete mDbConnector;
}

void UT_LogsContact::init()
{
    QtContactsStubsHelper::reset();
    mLogsEvent = new LogsEvent();
    LogsEventData* eventData = new LogsEventData;
    mLogsEvent->setLogsEventData(eventData);
    mLogsEvent->logsEventData()->setContactLocalId(logsContactsLocalIdTest1);
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
}

void UT_LogsContact::cleanup()
{
    delete mLogsContact;
    mLogsContact = 0;
    delete mLogsEvent;
    mLogsEvent = 0;
}

void UT_LogsContact::testConstructor()
{
    QVERIFY( mLogsContact );
    QVERIFY( !mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );
    
    LogsContact contactWithoutEvent("2345", *mDbConnector, 2);
    QVERIFY( contactWithoutEvent.mContactId == 2 );
    QVERIFY( contactWithoutEvent.mNumber == "2345" );
}

void UT_LogsContact::testAllowedRequestType()
{
    //Empty event with no matching contact
    QVERIFY( mLogsContact );
    QVERIFY( mLogsContact->allowedRequestType() == LogsContact::TypeLogsContactSave );
    
    //contact is in phonebook, but matching of contact is done only during
    //construction of mLogsContact, later changes not reflected
    mLogsEvent->logsEventData()->setContactLocalId(1);
    QtContactsStubsHelper::setContactId(1);
    QVERIFY( mLogsContact->allowedRequestType() == LogsContact::TypeLogsContactSave );
    
    //contact is in phonebook => open is allowed
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsEvent->logsEventData()->contactLocalId() );
    QVERIFY( mLogsContact->allowedRequestType() == LogsContact::TypeLogsContactOpen );
    
    //group is in phonebook => open is allowed
    mLogsEvent->logsEventData()->setContactLocalId(groupId);
    QtContactsStubsHelper::setContactId(groupId);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsEvent->logsEventData()->contactLocalId() );
    QVERIFY( mLogsContact->allowedRequestType() == LogsContact::TypeLogsContactOpenGroup );
    
    
    //contact not in phonebook, but caller ID present => save allowed
    QtContactsStubsHelper::reset();
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsEvent->logsEventData()->contactLocalId() );
    mLogsEvent->setNumber( "123" );
    QVERIFY( mLogsContact->allowedRequestType() == LogsContact::TypeLogsContactSave );
}

void UT_LogsContact::testOpen()
{
    //contact not in phonebook, can't open
    QVERIFY( !mLogsContact->mAiwRequest );
    QVERIFY( !mLogsContact->open() );
    QVERIFY( !mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );

    //contact is in phonebook, open is ok
    QtHighwayStubHelper::reset();
    mLogsEvent->logsEventData()->setContactLocalId(2);
    QtContactsStubsHelper::setContactId(2);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->open() );
    QVERIFY( mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactOpen );
    QVERIFY( mLogsContact->mAiwRequest->descriptor().interfaceName()
             == logsIContactsView );
    QVERIFY( mLogsContact->mAiwRequest->operation() == "openContactCard(int)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );    
    
    // Same but without using logsevent at construction
    QtHighwayStubHelper::reset();
    LogsContact contactWithoutEvent("2345", *mDbConnector, 2);
    QVERIFY( contactWithoutEvent.open() );
    QVERIFY( contactWithoutEvent.mAiwRequest );
    QVERIFY( contactWithoutEvent.mCurrentRequest == LogsContact::TypeLogsContactOpen );
    QVERIFY( contactWithoutEvent.mAiwRequest->descriptor().interfaceName()
             == logsIContactsView );
    QVERIFY( contactWithoutEvent.mAiwRequest->operation() == "openContactCard(int)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );
    
    //group is in phonebook, open is ok
    QtHighwayStubHelper::reset();
    mLogsEvent->logsEventData()->setContactLocalId(groupId);
    QtContactsStubsHelper::setContactId(groupId);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->open() );
    QVERIFY( mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactOpenGroup );
    QVERIFY( mLogsContact->mAiwRequest->descriptor().interfaceName()
             == logsIContactsView );
    QVERIFY( mLogsContact->mAiwRequest->operation() == "openGroup(int)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );

    // Request sending failed
    QtHighwayStubHelper::reset();
    QtHighwayStubHelper::setFailCreateAiwRequest(true);
    QVERIFY( !contactWithoutEvent.open() );
}

void UT_LogsContact::testAddNew()
{
    //no caller ID, contact won't be saved
    mLogsEvent->setEventType(LogsEvent::TypeVoiceCall);
    QVERIFY( mLogsEvent->getNumberForCalling().isEmpty() );
    QVERIFY( !mLogsContact->addNew() );
    QVERIFY( !mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );

    //called ID present, contact not in phonebook => save is ok
    QtHighwayStubHelper::reset();
    mLogsEvent->setNumber(QString::number(12345));
    mLogsEvent->setEventType(LogsEvent::TypeVoiceCall);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( !mLogsEvent->getNumberForCalling().isEmpty() );
    QVERIFY( !mLogsContact->isContactInPhonebook() );
    QVERIFY( mLogsContact->addNew() );
    QVERIFY( mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );
    QVERIFY( mLogsContact->mAiwRequest->descriptor().interfaceName()
             == logsIContactsEdit );
    QVERIFY( mLogsContact->mAiwRequest->operation() == "editCreateNew(QString,QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );
    
    //caller ID present, contact is in phonebook => save is ok
    QtHighwayStubHelper::reset();
    mLogsEvent->setEventType(LogsEvent::TypeVoIPCall);
    mLogsEvent->setNumber("");
    mLogsEvent->logsEventData()->mRemoteUrl = "someurl@blah";
    mLogsEvent->logsEventData()->setContactLocalId(2);
    QtContactsStubsHelper::setContactId(2);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactInPhonebook() );
    QVERIFY( mLogsContact->addNew() );
    QVERIFY( mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );
    QVERIFY( mLogsContact->mAiwRequest->descriptor().interfaceName()
             == logsIContactsEdit );
    QVERIFY( mLogsContact->mAiwRequest->operation() == "editCreateNew(QString,QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );
}

void UT_LogsContact::testUpdateExisting()
{
    //caller ID present, contact is in phonebook => update is ok
    QtHighwayStubHelper::reset();
    mLogsEvent->setNumber(QString::number(12345));
    mLogsEvent->setEventType(LogsEvent::TypeVoiceCall);
    mLogsEvent->logsEventData()->setContactLocalId(2);
    QtContactsStubsHelper::setContactId(2);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactInPhonebook() );
    QVERIFY( mLogsContact->updateExisting() );
    QVERIFY( mLogsContact->mAiwRequest );
    QVERIFY( mLogsContact->mCurrentRequest == LogsContact::TypeLogsContactSave );
    QVERIFY( mLogsContact->mAiwRequest->descriptor().interfaceName()
             == logsIContactsEdit );
    QVERIFY( mLogsContact->mAiwRequest->operation() == "editUpdateExisting(QString,QString)" );
    QVERIFY( QtHighwayStubHelper::isRequestEmbedded() );
    QVERIFY( !QtHighwayStubHelper::isRequestSynchronous() );
}

void UT_LogsContact::testIsContactInPhonebook()
{
    QVERIFY( !mLogsContact->isContactInPhonebook() );
    
    //any changes in phonebook after mLogsContact creation are not reflected
    mLogsEvent->logsEventData()->setContactLocalId(2);
    QtContactsStubsHelper::setContactId(2);
    QVERIFY( !mLogsContact->isContactInPhonebook() );
    
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactInPhonebook() );
    
    // 0 is not valid contact ID
    mLogsEvent->logsEventData()->setContactLocalId(0);
    QtContactsStubsHelper::setContactId(0);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( !mLogsContact->isContactInPhonebook() );
}

void UT_LogsContact::testIsContactRequestAllowed()
{
    // contact not in phonebook, caller ID not defined
    QVERIFY( !mLogsContact->isContactInPhonebook() );
    QVERIFY( mLogsEvent->number().isEmpty() );
    QVERIFY( mLogsEvent->logsEventData()->remoteUrl().isEmpty() );
    QVERIFY( !mLogsContact->isContactRequestAllowed() );
    
    // caller ID is defined, number
    mLogsEvent->setNumber( "1234" );
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactRequestAllowed() );
    
    // caller ID is defined, voip url
    mLogsEvent->setNumber( "" );
    mLogsEvent->logsEventData()->mRemoteUrl = "blah";
    mLogsEvent->setEventType( LogsEvent::TypeVoIPCall );
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactRequestAllowed() );

    // No caller ID, but contact is in phonebook
    mLogsEvent->logsEventData()->mRemoteUrl = "";
    mLogsEvent->logsEventData()->setContactLocalId(1);
    QtContactsStubsHelper::setContactId(1);
    delete mLogsContact;
    mLogsContact = 0;
    mLogsContact = new LogsContact(*mLogsEvent, *mDbConnector);
    QVERIFY( mLogsContact->isContactRequestAllowed() );
}

void UT_LogsContact::testHandleRequestCompeted()
{
    QSignalSpy spyOpened( mLogsContact, SIGNAL(openCompleted(bool)) );
    QSignalSpy spySaved( mLogsContact, SIGNAL(saveCompleted(bool)) );

    //open operation finished
    LogsDbConnectorStubHelper::reset();
    mLogsContact->mCurrentRequest = LogsContact::TypeLogsContactOpen;
    mLogsContact->handleRequestCompleted( QVariant(1) );
    QVERIFY( spyOpened.count() == 1 );
    QVERIFY( spySaved.count() == 0 );
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction() == "updateDetails" );
        
    //save operation finished successfully
    LogsDbConnectorStubHelper::reset();
    spyOpened.clear();
    spySaved.clear();
    mLogsContact->mCurrentRequest = LogsContact::TypeLogsContactSave;
    mLogsContact->handleRequestCompleted( QVariant(1) );
    QVERIFY( spyOpened.count() == 0 );
    QVERIFY( spySaved.count() == 1 );
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction() == "updateDetails" );
    
    //save operation finished unsuccessfully
    LogsDbConnectorStubHelper::reset();
    spyOpened.clear();
    spySaved.clear();
    mLogsContact->mCurrentRequest = LogsContact::TypeLogsContactSave;
    mLogsContact->handleRequestCompleted( QVariant(0) );
    QVERIFY( spyOpened.count() == 0 );
    QVERIFY( spySaved.count() == 1 );
    QVERIFY( LogsDbConnectorStubHelper::lastCalledFunction().isEmpty() );    
}

void UT_LogsContact::testCancelServiceRequest()
{
    XQAiwInterfaceDescriptor descr;
    QVERIFY( !mLogsContact->mAiwRequest );
    mLogsContact->mAiwRequest = new XQAiwRequest(descr, "message");
    mLogsContact->cancelServiceRequest();
    QVERIFY( !mLogsContact->mAiwRequest );
}
