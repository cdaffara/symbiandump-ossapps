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
* Description:  Unit tests for Phone Services.
*
*/

#include <QtTest/QtTest>
#include "phoneservices.h"
#include "dialservice.h"
#include "dtmfservice.h"
#include "dialservicedepricated.h"
#include "dtmfservicedepricated.h"
#include "urischemehandlerservice.h"

bool g_dialServiceConstructed;
MPECallControlIF* g_dialServiceCallPointer;
MPECallSettersIF* g_dialServiceParameterPointer;

bool g_dtmfServiceConstructed;
MPECallControlIF* g_dtmfServiceCallPointer;
MPECallSettersIF* g_dtmfServiceParameterPointer;

bool g_uriSchemeHandlerServiceConstructed;
MPECallControlIF* g_uriSchemeServiceCallPointer;
MPECallSettersIF* g_uriSchemeServiceParameterPointer;

class TestPhoneServices 
    : 
    public QObject,
    public MPECallControlIF,
    public MPECallSettersIF
{
    Q_OBJECT
public:
    TestPhoneServices();
    virtual ~TestPhoneServices();    

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
    void testPhoneServiceStartup();

private:
    PhoneServices *m_phoneServices; // class under test
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
    ushort keyValue;
};

DialService::DialService(MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent) : 
    XQServiceProvider(QLatin1String("com.nokia.symbian.ICallDial"), parent), m_call (call), m_parameters (parameters)
{
    g_dialServiceConstructed = true;
    g_dialServiceCallPointer = &call;
    g_dialServiceParameterPointer = &parameters;
}

DialService::~DialService()
{
}

int DialService::dial(const QString& number)
{
    Q_UNUSED(number)
    return 0;
}

int DialService::dial(const QString& number, int contactId)
{
    Q_UNUSED(number)
    Q_UNUSED (contactId)
    return 0;
}

int DialService::dialVideo(const QString& number)
{
    Q_UNUSED(number)
}

int DialService::dialVideo(const QString& number, int contactId)
{
    Q_UNUSED(number)
    Q_UNUSED(contactId)
}

int DialService::dialVoip(const QString& address)
{
    Q_UNUSED(address)
}

int DialService::dialVoip(const QString& address, int contactId)
{
    Q_UNUSED(address)
    Q_UNUSED(contactId)
}

int DialService::dialVoipService(const QString& address, int serviceId)
{
    Q_UNUSED(address)
    Q_UNUSED(serviceId)
}

int DialService::dialVoipService(
        const QString& address, int serviceId, int contactId)
{
    Q_UNUSED(address)
    Q_UNUSED(serviceId)
    Q_UNUSED(contactId)
}

DialServiceDepricated::DialServiceDepricated(MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent) : 
    XQServiceProvider(QLatin1String("com.nokia.symbian.ICallDial"), parent), m_call (call), m_parameters (parameters)
{
}

DialServiceDepricated::~DialServiceDepricated()
{
}

int DialServiceDepricated::dial(const QString& number)
{
    Q_UNUSED(number)
}

int DialServiceDepricated::dial(const QString& number, int contactId)
{
    Q_UNUSED(number)
    Q_UNUSED(contactId)
}

void DialServiceDepricated::dialVideo(const QString& number)
{
    Q_UNUSED(number)
}

void DialServiceDepricated::dialVideo(const QString& number, int contactId)
{
    Q_UNUSED(number)
    Q_UNUSED(contactId)
}

void DialServiceDepricated::dialVoip(const QString& address)
{
    Q_UNUSED(address)
}

void DialServiceDepricated::dialVoip(const QString& address, int contactId)
{
    Q_UNUSED(address)
    Q_UNUSED(contactId)
}

void DialServiceDepricated::dialVoipService(const QString& address, int serviceId)
{
    Q_UNUSED(address)
    Q_UNUSED(serviceId)
}

void DialServiceDepricated::dialVoipService(
        const QString& address, int serviceId, int contactId)
{
    Q_UNUSED(address)
    Q_UNUSED(serviceId)
    Q_UNUSED(contactId)
}

QString DialServiceDepricated::modifyPhoneNumber(const QString &number)
{
    return number;
}

QString DialServiceDepricated::simplified(const QString &number)
{
    return number;
}

QString DialServiceDepricated::japanPrefixModifications(const QString &number)
{    
    return number;
}


DTMFService::DTMFService(MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent) : 
    XQServiceProvider(QLatin1String("com.nokia.symbian.IDtmfPlay"), parent), m_call (call), m_parameters (parameters)
{
    g_dtmfServiceConstructed = true;
    g_dtmfServiceCallPointer = &call;
    g_dtmfServiceParameterPointer = &parameters;
}

DTMFService::~DTMFService()
{
}

int DTMFService::playDTMFTone(const QChar& keyToPlay)
{
    Q_UNUSED(keyToPlay)
}

int DTMFService::stopDTMFPlay()
{
}

DTMFServiceDepricated::DTMFServiceDepricated(
    MPECallControlIF &call, 
    MPECallSettersIF &parameters, 
    QObject* parent) 
    : 
    XQServiceProvider(
        QLatin1String("com.nokia.symbian.IDtmfPlay"), parent), 
    m_call(call), 
    m_parameters(parameters)
{
    
}

DTMFServiceDepricated::~DTMFServiceDepricated()
{
}

void DTMFServiceDepricated::playDTMFTone(const QChar& /*keyToPlay*/)
{
}

void DTMFServiceDepricated::stopDTMFPlay()
{

}


UriSchemeHandlerService::UriSchemeHandlerService(
    MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent)
    : 
    XQServiceProvider(
        QLatin1String("phoneui.com.nokia.symbian.IUriView"), parent),
    m_callControlIf(call),
    m_callParameters(parameters)
{
    g_uriSchemeHandlerServiceConstructed = true;
    g_uriSchemeServiceCallPointer = &call;
    g_uriSchemeServiceParameterPointer = &parameters;
}

UriSchemeHandlerService::~UriSchemeHandlerService()
{
    
}

bool UriSchemeHandlerService::view(const QString &uri)
{
    Q_UNUSED(uri)
    return true;
}

TestPhoneServices::TestPhoneServices ()
{
}

TestPhoneServices::~TestPhoneServices ()
{
}

void TestPhoneServices::initTestCase ()
{
}

void TestPhoneServices::cleanupTestCase ()
{
}

void TestPhoneServices::init ()
{
    m_setPhoneNumberCalled = false;
    m_setCallTypeCommandCalled = false;
    m_handleDialCallCalled = false;
    m_clientCall = false;
    m_setContactIdCalled = false;
    m_contactId = -1;
    m_handleEndDTMFCalled = false;
    m_handlePlayDTMFLCalled = false;
    m_setKeyCodeCalled = false;
    keyValue = -1;
    g_dialServiceConstructed = false;
    g_dtmfServiceConstructed = false;
    
    g_uriSchemeHandlerServiceConstructed = false;
    g_uriSchemeServiceCallPointer = NULL;
    g_uriSchemeServiceParameterPointer = NULL;
    
    m_phoneServices = new PhoneServices(*this, *this, this);

    QVERIFY(g_dialServiceConstructed == true);
    QVERIFY(g_dialServiceCallPointer == this);
    QVERIFY(g_dialServiceParameterPointer == this);
    
    QVERIFY(g_dtmfServiceConstructed == true);
    QVERIFY(g_dtmfServiceCallPointer == this);
    QVERIFY(g_dtmfServiceParameterPointer == this);
    
    QVERIFY(g_uriSchemeHandlerServiceConstructed == true);
    QVERIFY(g_uriSchemeServiceCallPointer == this);
    QVERIFY(g_uriSchemeServiceParameterPointer == this);
}

void TestPhoneServices::cleanup ()
{
    delete m_phoneServices;
}

void TestPhoneServices::SetKeyCode( const TChar& aKeyCode )
{
    m_setKeyCodeCalled = true;
    keyValue = aKeyCode;
}

void TestPhoneServices::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
{
    m_setPhoneNumberCalled = true;
    m_phoneNumber = aPhoneNumber;	
}

void TestPhoneServices::SetCallTypeCommand( const TPECallType& aCallType )
{
    m_setCallTypeCommandCalled = true;
    m_callType = aCallType;	
}

void TestPhoneServices::HandlePlayDTMFL()
{
    m_handlePlayDTMFLCalled = true;
}

TInt TestPhoneServices::HandleEndDTMF()
{
    m_handleEndDTMFCalled = true;
    return 0;
}

void TestPhoneServices::SetContactId2( const TInt aContactId )
{
    m_setContactIdCalled = true;
    m_contactId = aContactId;	
}

void TestPhoneServices::SetServiceIdCommand( TUint32 aServiceId )
{
    Q_UNUSED(aServiceId)
}

TInt TestPhoneServices::HandleDialServiceCall( const TBool aClientCall )
{
    m_handleDialCallCalled = true;
    m_clientCall = aClientCall;
    return KErrNone;
}

void TestPhoneServices::testPhoneServiceStartup()
{

}

QTEST_MAIN(TestPhoneServices)
#include "unit_tests.moc"
