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

#include "ut_cntdefaultinfoprovider.h"
#include "cntdefaultinfoprovider.h"

void TestCntDefaultInfoProvider::initTestCase()
{
    mCntDefaultInfoProvider = NULL;
}

void TestCntDefaultInfoProvider::create()
{
    mCntDefaultInfoProvider = new CntDefaultInfoProvider();
}

void TestCntDefaultInfoProvider::testSupportedFields()
{
    QVERIFY(mCntDefaultInfoProvider->supportedFields() == ContactInfoIcon1Field | ContactInfoTextField);
}

void TestCntDefaultInfoProvider::testRequestInfo()
{
    QSignalSpy spy(mCntDefaultInfoProvider, SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)));
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    
    ContactInfoFields fields;
    fields = ContactInfoIcon2Field;
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 0);
    
    fields = ContactInfoIcon1Field | ContactInfoTextField;
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    
    QContactPhoneNumber number;
    number.setNumber("1234567");
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number);
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    
    c.setPreferredDetail("call", number);

    QContactPhoneNumber number2;
    number2.setNumber("7654321");
    number2.setContexts(QContactDetail::ContextWork);
    number2.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number2);
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    
    QContactAvatar avatar;
    c.saveDetail(&avatar);
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 2);
    spy.clear();
    
    avatar.setImageUrl(QUrl("dummyavatar"));
    c.saveDetail(&avatar);
    
    mCntDefaultInfoProvider->requestInfo(c, fields);
    QCOMPARE(spy.count(), 2);
}

void TestCntDefaultInfoProvider::cleanupTestCase()
{
    delete mCntDefaultInfoProvider;
    mCntDefaultInfoProvider = NULL;
}

