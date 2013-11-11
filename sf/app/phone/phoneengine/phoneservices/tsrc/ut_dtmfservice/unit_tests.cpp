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
* Description:  Unit tests for DTMFService.
*
*/

#include <QtTest/QtTest>
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <mockservice.h>
#include "dtmfservice.h"
#include "phonetestmain.h"

typedef QSet<int> IntegerSet;

class TestDTMFService : public QObject, public MPECallControlIF, public MPECallSettersIF, public MockService
{
    Q_OBJECT
public:
    TestDTMFService();
    virtual ~TestDTMFService();    

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
    void testPlayDTMFTone ();
    void testStopDTMFPlay ();
    void testPlayDTMFTone_nocaps();

private:
    DTMFService *m_dtmfService; // class under test
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
    bool m_handlePlayDtmfLeave;
    ushort keyValue;
    XQRequestInfo m_XQRequestInfoMock;
};

TestDTMFService::TestDTMFService ()
{
}

TestDTMFService::~TestDTMFService ()
{
}

void TestDTMFService::initTestCase ()
{
}

void TestDTMFService::cleanupTestCase ()
{
}

void TestDTMFService::init ()
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
    m_handlePlayDtmfLeave = false;
    keyValue = -1;
    m_dtmfService = new DTMFService (*this, *this, this);
}

void TestDTMFService::cleanup ()
{
    SmcDefaultValue<IntegerSet>::Reset();
    SmcDefaultValue<XQRequestInfo>::Reset();
    reset();
    delete m_dtmfService;
}

void TestDTMFService::SetKeyCode( const TChar& aKeyCode )
{
    m_setKeyCodeCalled = true;
    keyValue = aKeyCode;
}

void TestDTMFService::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
{
    m_setPhoneNumberCalled = true;
    m_phoneNumber = aPhoneNumber;	
}

void TestDTMFService::SetCallTypeCommand( const TPECallType& aCallType )
{
    m_setCallTypeCommandCalled = true;
    m_callType = aCallType;	
}

void TestDTMFService::HandlePlayDTMFL()
{
    m_handlePlayDTMFLCalled = true;
    
    if (m_handlePlayDtmfLeave)
        User::Leave(KErrNotFound);
}

TInt TestDTMFService::HandleEndDTMF()
{
    m_handleEndDTMFCalled = true;
    return 0;
}

void TestDTMFService::SetContactId2( const TInt aContactId )
{
    m_setContactIdCalled = true;
    m_contactId = aContactId;	
}

void TestDTMFService::SetServiceIdCommand( TUint32 aServiceId )
{
    Q_UNUSED(aServiceId)
}

TInt TestDTMFService::HandleDialServiceCall( const TBool aClientCall )
{
    m_handleDialCallCalled = true;
    m_clientCall = aClientCall;
    return KErrNone;
}

// Test cases

void TestDTMFService::testPlayDTMFTone()
{
    QChar six('6');
    m_dtmfService->playDTMFTone(six);
    QVERIFY (m_setKeyCodeCalled == true);
    QCOMPARE (keyValue, six.unicode());
    QVERIFY (m_handlePlayDTMFLCalled == true);
    
    m_handlePlayDtmfLeave = true;
    
    TRAP_IGNORE(m_dtmfService->playDTMFTone(six));
    
    m_handlePlayDtmfLeave = false;
}

void TestDTMFService::testStopDTMFPlay()
{
    m_dtmfService->stopDTMFPlay();
    QVERIFY (m_handleEndDTMFCalled == true);
}

void TestDTMFService::testPlayDTMFTone_nocaps()
{
    QSet<int> caps;
    expect( "XQRequestInfo::clientCapabilities" ).
        returns(caps).
        times(2);
    QChar six('6');
    int ret = m_dtmfService->playDTMFTone(six);
    QVERIFY (ret == KErrPermissionDenied);
    ret = m_dtmfService->stopDTMFPlay();
    QVERIFY (ret == KErrPermissionDenied);
    
    QVERIFY( verify() );
}


PHONE_TEST_MAIN(TestDTMFService)
#include "unit_tests.moc"

