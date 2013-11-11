/*!
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
* Description:  Unit tests for DialService.
*
*/

#include <QtTest/QtTest>
#include <featmgr.h>
#include <settingsinternalcrkeys.h>
#include <xqsettingsmanager.h>
#include <e32capability.h>

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <mockservice.h>

#include "dialservice.h"
#include "phonetestmain.h"

bool m_featureManagerReturnValue;
int m_featureManagerRequestedFeatureSupport;

typedef QSet<int> IntegerSet;

TBool FeatureManager::FeatureSupported(TInt aFeature)
{
    m_featureManagerRequestedFeatureSupport = aFeature;
    return m_featureManagerReturnValue;
}

class TestDialService : public QObject, public MPECallControlIF, public MPECallSettersIF, public MockService
{
    Q_OBJECT
public:
    TestDialService();
    virtual ~TestDialService();    

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup ();
    
public:
    void SetKeyCode( const TChar& aKeyCode );
    void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber );
    void SetCallTypeCommand( const TPECallType& aCallType );
    TInt HandleDialServiceCall( const TBool aClientCall = EFalse );
    void HandlePlayDTMFL();
    TInt HandleEndDTMF();
    void SetContactId2( const TInt aContactId );
    void SetServiceIdCommand( TUint32 aServiceId );
    
private slots:
    void testDial ();
    void testDial2 ();
    void testDial3();
    void testDialWithJapanPrefixOn();
    void testDialVideo ();
    void testDialVideo2 ();
    void testDialVoip();
    void testDialVoip2();
    void testDialVoipService();
    void testDialVoipService2();
    void testDialNoCaps();
    
private:
    DialService *m_DialService; // class under test
    bool m_setPhoneNumberCalled;
    bool m_setCallTypeCommandCalled;
    bool m_handleDialCallCalled;
    TPEPhoneNumber m_phoneNumber;
    TPECallType m_callType;
    bool m_clientCall;
    int m_contactId;
    bool m_setContactIdCalled;
    bool m_handleEndDTMFCalled;
    bool m_handlePlayDTMFLCalled;
    bool m_setKeyCodeCalled;
    bool m_setServiceIdCalled;
    int m_serviceId;
    ushort keyValue;
    XQRequestInfo m_XQRequestInfoMock;
};

TestDialService::TestDialService ()
{
}

TestDialService::~TestDialService ()
{
}

void TestDialService::initTestCase ()
{
}

void TestDialService::cleanupTestCase ()
{
}

void TestDialService::init ()
{
    initialize();
    IntegerSet caps;
    caps.insert(ECapabilityNetworkServices);
    caps.insert(ECapabilityNetworkControl);
    SmcDefaultValue<IntegerSet>::SetL(caps);
    SmcDefaultValue<XQRequestInfo>::SetL(m_XQRequestInfoMock);
       
    m_setPhoneNumberCalled = false;
    m_setCallTypeCommandCalled = false;
    m_handleDialCallCalled = false;
    m_clientCall = false;
    m_setContactIdCalled = false;
    m_contactId = -1;
    m_handleEndDTMFCalled = false;
    m_handlePlayDTMFLCalled = false;
    m_setKeyCodeCalled = false;
    m_featureManagerReturnValue = false;
    m_featureManagerRequestedFeatureSupport = -1;
    keyValue = -1;
    m_DialService = new DialService (*this, *this, this);
}

void TestDialService::cleanup ()
{
    SmcDefaultValue<IntegerSet>::Reset();
    SmcDefaultValue<XQRequestInfo>::Reset();
    reset();
    delete m_DialService;
}

void TestDialService::SetKeyCode( const TChar& aKeyCode )
{
    m_setKeyCodeCalled = true;
    keyValue = aKeyCode;
}

void TestDialService::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
{
    m_setPhoneNumberCalled = true;
    m_phoneNumber = aPhoneNumber;	
}

void TestDialService::SetCallTypeCommand( const TPECallType& aCallType )
{
    m_setCallTypeCommandCalled = true;
    m_callType = aCallType;	
}

void TestDialService::HandlePlayDTMFL()
{
    m_handlePlayDTMFLCalled = true;
}

TInt TestDialService::HandleEndDTMF()
{
    m_handleEndDTMFCalled = true;
    return 0;
}

void TestDialService::SetContactId2( const TInt aContactId )
{
    m_setContactIdCalled = true;
    m_contactId = aContactId;	
}

void TestDialService::SetServiceIdCommand( TUint32 aServiceId )
{
    m_setServiceIdCalled = true;
    m_serviceId = aServiceId;   
}

TInt TestDialService::HandleDialServiceCall( const TBool aClientCall )
{
    m_handleDialCallCalled = true;
    m_clientCall = aClientCall;
    return KErrNone;
}

// Test cases

void TestDialService::testDial()
{
    m_DialService->dial(QString("0501234567"));
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("0501234567"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeCSVoice);
    QVERIFY (m_handleDialCallCalled == true);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDial2()
{
    m_DialService->dial(QString("0501234567"), 3127);
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("0501234567"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeCSVoice);
    QVERIFY (m_setCallTypeCommandCalled == true);
    QVERIFY (m_setContactIdCalled == true);
    QCOMPARE (m_contactId, 3127);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDial3()
{
    m_DialService->dial(QString("[+358] (0) 50-1234 5.6,7"), 3127);
    QString _number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(_number, QString("+3580501234567"));
}

void TestDialService::testDialWithJapanPrefixOn()
{
    XQSettingsKey dialPrefixMode(XQSettingsKey::TargetCentralRepository, KCRUidTelephonySettings.iUid, KSettingsDialPrefixChangeMode);
    XQSettingsKey dialPrefixString(XQSettingsKey::TargetCentralRepository, KCRUidTelephonySettings.iUid, KSettingsDialPrefixText);
    XQSettingsManager setManager;
    int err = setManager.writeItemValue(dialPrefixString, QString("00"));
    qDebug() << err;
    qDebug() << setManager.error();
    setManager.writeItemValue(dialPrefixMode, 1);
    m_featureManagerReturnValue = true;

    m_DialService->dial(QString("+358501234567"));    
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QCOMPARE(m_featureManagerRequestedFeatureSupport, KFeatureIdJapanPrefixChange);
    QVERIFY(m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("00358501234567"));
}

void TestDialService::testDialVideo()
{
    m_DialService->dialVideo(QString("0501234567"));
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("0501234567"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVideo);
    QVERIFY (m_handleDialCallCalled == true);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDialVideo2()
{
    m_DialService->dialVideo(QString("+358501234567"), 12345);
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("+358501234567"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVideo);
    QVERIFY (m_setCallTypeCommandCalled == true);
    QVERIFY (m_setContactIdCalled == true);
    QCOMPARE (m_contactId, 12345);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDialVoip()
{
    m_DialService->dialVoip(QString("address@domain"));
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("address@domain"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVoIP);
}

void TestDialService::testDialVoip2()
{
    m_DialService->dialVoip(QString("address@domain"), 123);
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("address@domain"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVoIP);
    QVERIFY (m_setContactIdCalled == true);
    QCOMPARE (m_contactId, 123);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDialVoipService()
{
    m_DialService->dialVoipService(QString("address@domain"), 123);
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("address@domain"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVoIP);
    QVERIFY (m_setServiceIdCalled == true);
    QCOMPARE (m_serviceId, 123);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDialVoipService2()
{
    m_DialService->dialVoipService(QString("address@domain"), 123, 12345);
    QString number((QChar*)m_phoneNumber.Ptr(), m_phoneNumber.Length());
    QVERIFY (m_setPhoneNumberCalled == true);
    QCOMPARE(number, QString("address@domain"));
    QVERIFY (m_setCallTypeCommandCalled == true);
    QCOMPARE((int)m_callType, (int)EPECallTypeVoIP);
    QVERIFY (m_setServiceIdCalled == true);
    QCOMPARE (m_serviceId, 123);
    QVERIFY (m_setContactIdCalled == true);
    QCOMPARE (m_contactId, 12345);
    QVERIFY (m_clientCall == false);
}

void TestDialService::testDialNoCaps()
    {
    QSet<int> caps;
    caps.insert(ECapabilityNetworkControl);
    expect( "XQRequestInfo::clientCapabilities" ).
        returns(caps).
        times(4);
    int ret = m_DialService->dial(QString("0501234567"));
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dial(QString("0501234567"), 3127);
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dialVideo(QString("0501234567"));
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dialVideo(QString("0501234567"), 3127);
    QVERIFY (ret == KErrPermissionDenied);

    reset();
    QSet<int> caps2;
    caps2.insert(ECapabilityNetworkServices);
    expect( "XQRequestInfo::clientCapabilities" ).
        returns(caps2).
        times(4);
    ret = m_DialService->dialVoip(QString("0501234567"));
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dialVoip(QString("0501234567"), 3127);
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dialVoipService(QString("0501234567"), 3);
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_DialService->dialVoipService(QString("0501234567"), 3, 3127);
    QVERIFY (ret == KErrPermissionDenied);
    
    QVERIFY( verify() );
    }

PHONE_TEST_MAIN(TestDialService)
#include "unit_tests.moc"
