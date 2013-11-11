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

#include "ut_cntpresenceinfoprovider.h"
#include "cntpresenceinfoprovider.h"

#include <prcpresencebuddyinfo_qt.h>
#include <prcpresencereader_qt.h>
#include <prcpresencewriter_qt.h>

void TestCntPresenceInfoProvider::initTestCase()
{
    mCntPresenceInfoProvider = NULL;
}

void TestCntPresenceInfoProvider::create()
{
    mCntPresenceInfoProvider = new CntPresenceInfoProvider();
}

void TestCntPresenceInfoProvider::testSupportedFields()
{
    QVERIFY(mCntPresenceInfoProvider->supportedFields() == ContactInfoIcon2Field);
}

void TestCntPresenceInfoProvider::testRequestInfo()
{
    PrcPresenceWriter *writer = PrcPresenceWriter::createWriter();
    
    PrcPresenceBuddyInfoQt *buddy = PrcPresenceBuddyInfoQt::createInstance();
    buddy->setIdentity("sip:test@test.com");
    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcNotAvailable, "meh");
    writer->writePresence(*buddy);
    
    QContactManager manager("symbian");
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QContactPhoneNumber number;
    number.setNumber("1234567");
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number);
    manager.saveContact(&c);
    
    ContactInfoFields fields;
    fields = ContactInfoTextField;
    
    QSignalSpy spy(mCntPresenceInfoProvider, SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)));
    
    mCntPresenceInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 0);
    QVERIFY(mCntPresenceInfoProvider->mBuddyMap.isEmpty());
    
    fields = ContactInfoIcon2Field;
    
    mCntPresenceInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 0);
    QVERIFY(mCntPresenceInfoProvider->mBuddyMap.isEmpty());
    
    QContactOnlineAccount account;
    account.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeSip);
    account.setServiceProvider("sip");
    account.setAccountUri("test@test.com");
    c.saveDetail(&account);
    QContactOnlineAccount account2;
    account2.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeSipVoip);
    account.setServiceProvider("sip");
    account2.setAccountUri("test@test.com");
    c.saveDetail(&account2);
    QContactOnlineAccount account3;
    account3.setSubTypes(QStringList() << QContactOnlineAccount::SubTypeSip);
    account3.setAccountUri("malformatted");
    c.saveDetail(&account3);
    manager.saveContact(&c);
    
    mCntPresenceInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mCntPresenceInfoProvider->mBuddyMap.count(), 1);
    
    delete mCntPresenceInfoProvider;
    mCntPresenceInfoProvider = NULL;
    
    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "meh");
    writer->writePresence(*buddy);
    
    mCntPresenceInfoProvider = new CntPresenceInfoProvider();
    
    QSignalSpy spy2(mCntPresenceInfoProvider, SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)));
    mCntPresenceInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(mCntPresenceInfoProvider->mBuddyMap.count(), 1);
    
    delete buddy;
    delete writer;
}

void TestCntPresenceInfoProvider::testHandlePresenceUpdate()
{
    QSignalSpy spy(mCntPresenceInfoProvider, SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)));
    
    PrcPresenceWriter *writer = PrcPresenceWriter::createWriter();
    PrcPresenceReader *reader = PrcPresenceReader::createReader();
    
    PrcPresenceBuddyInfoQt *dummyBuddy = PrcPresenceBuddyInfoQt::createInstance();
    dummyBuddy->setIdentity("sip:dummy@dummy.com");
    dummyBuddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "meh");
    writer->writePresence(*dummyBuddy);
    
    mCntPresenceInfoProvider->handlePresenceUpdate(true, dummyBuddy);
    QCOMPARE(spy.count(), 0);

    mCntPresenceInfoProvider->handlePresenceUpdate(false, dummyBuddy);
    QCOMPARE(spy.count(), 0);

    mCntPresenceInfoProvider->handlePresenceUpdate(true, NULL);
    QCOMPARE(spy.count(), 0);
    
    PrcPresenceBuddyInfoQt *buddy = reader->presenceInfo("sip:test@test.com");
    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcNotAvailable, "meh");
    writer->writePresence(*buddy);
    QTest::qWait(5000);
    QCOMPARE(spy.count(), 1);
    
    QContactManager manager("symbian");
    QContact c = manager.contact(mCntPresenceInfoProvider->mBuddyMap.value("sip:test@test.com"));
    
    QList<QContactOnlineAccount> accounts = c.details<QContactOnlineAccount>();
    foreach (QContactOnlineAccount account, accounts)
    {
        c.removeDetail(&account);
    }
    manager.saveContact(&c);
    
    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "meh");
    writer->writePresence(*buddy);
    QTest::qWait(5000);
    QCOMPARE(spy.count(), 2);
    
    delete writer;
    delete reader;
    delete dummyBuddy;
    delete buddy;
}

void TestCntPresenceInfoProvider::testParsePresence()
{
    PrcPresenceBuddyInfoQt *buddy = PrcPresenceBuddyInfoQt::createInstance();
    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcNotAvailable, "meh");

    QList<PrcPresenceBuddyInfoQt*> buddies;
    buddies.append(buddy);

    QVERIFY(mCntPresenceInfoProvider->parsePresence(buddies).isEmpty());

    buddy->setAvailability(PrcPresenceBuddyInfoQt::PrcAvailable, "meh");

    QVERIFY(mCntPresenceInfoProvider->parsePresence(buddies) == "qtg_small_online");
    
    delete buddy;
}

void TestCntPresenceInfoProvider::cleanupTestCase()
{
    delete mCntPresenceInfoProvider;
    mCntPresenceInfoProvider = NULL;
    
    QContactManager manager("symbian");
    QList<QContactLocalId> ids = manager.contactIds();
    QMap<int, QContactManager::Error> errorMap;
    manager.removeContacts(ids, &errorMap);
}

