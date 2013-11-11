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
* Description:  Unit tests for PhoneUIQtButtonsController.
*
*/

#include <QtTest/QtTest>
#include <mockservice.h>
#include "phonetestmain.h"
#include "phonecallheaderutil.h"
#include "pevirtualengine.h"
#include "cpeengineinfo.h"
#include "tphonecmdparamcallheaderdata.h"
#include "cphonemainresourceresolver.h"
#include "cphonecenrepproxy.h"
#include "phonerssbase.h"
#include <StringLoader.h>
#include <coemain.h>

_LIT(KSimulatedStringLoaderString,"1234567890");

class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {    
    };

class CPhoneCenRepProxyMock : public CPhoneCenRepProxy
    { 
public:
    CPhoneCenRepProxyMock() {};
    };

void simulateStringLoad(TDes& buf, TInt /*id*/, CCoeEnv* /*coeenv*/)
{
    buf.Append(KSimulatedStringLoaderString());
}

class TestPhoneCallHeaderUtil : public QObject, MockService
{
    Q_OBJECT
public:
    TestPhoneCallHeaderUtil();
    virtual ~TestPhoneCallHeaderUtil();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testLabelText();
    void testGetCliTexts();
    void testGetRemoteInfoData();
    void testEngineInfo();
    void testIsVoiceCall();
    void testIsVideoCall();
    void testCallHeaderLabelTextForRingingCall();
    void testEmergencyHeaderText();
    void testAttemptingEmergencyText();
    
private:
    CPEEngineInfo* m_engineInfo;
    PhoneCallHeaderUtil* m_util;
    CPhoneMainResourceResolverMock* m_resourceResolverMock;
    CPhoneCenRepProxyMock* m_cenrepProxyMock;
};

TestPhoneCallHeaderUtil::TestPhoneCallHeaderUtil ()
{
    
}

TestPhoneCallHeaderUtil::~TestPhoneCallHeaderUtil ()
{
}

void TestPhoneCallHeaderUtil::initTestCase ()
{
    m_engineInfo = CPEEngineInfo::NewL();
    m_util = new PhoneCallHeaderUtil(*m_engineInfo);
    
    m_resourceResolverMock = new CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( m_resourceResolverMock );
    
    m_cenrepProxyMock = new CPhoneCenRepProxyMock();
    SmcDefaultValue<CPhoneCenRepProxy*>::SetL( m_cenrepProxyMock );
}

void TestPhoneCallHeaderUtil::cleanupTestCase ()
{
    delete m_util;
    delete m_engineInfo;
    delete m_resourceResolverMock;
}

void TestPhoneCallHeaderUtil::init ()
{
    initialize();
}

void TestPhoneCallHeaderUtil::cleanup ()
{
    reset();
}

void TestPhoneCallHeaderUtil::testLabelText()
{
    const int callId = 1;
    QVERIFY( m_util->LabelText(1) == KNullDesC() );
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    
    m_engineInfo->SetCallState(EPEStateHeld, callId);       
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCallOnHold);
    TBuf<100> label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallState(EPEStateHeldConference, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCallOnHold);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallState(EPEStateDisconnecting, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneInCallDisconnected);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallState(EPEStateConnectedConference, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCLIConferenceCall);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallState(EPEStateDialing, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneOutgoingCallLabel);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallType(EPECallTypeVideo, callId);
    m_engineInfo->SetCallState(EPEStateDialing, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneOutgoingVideoCallLabel);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderUtil::testGetCliTexts()
{
    const int callId = 1;
    const int conferenceCallId = 8;
    const int emergencyCallId = 7;
    _LIT(KPhoneNumber,"123456");
    _LIT(KPhoneRemoteName,"test_name");
    
    TBuf<100> cliText;
    PhoneCallHeaderUtil::ClippingDirection cliClip;
    TBuf<100> secondaryCliText;
    PhoneCallHeaderUtil::ClippingDirection secondaryCliClip;
    
    
    m_engineInfo->SetRemotePhoneNumber(KPhoneNumber(), callId);

    m_util->GetCliTexts(callId,cliText, cliClip, secondaryCliText, secondaryCliClip);
    QVERIFY(cliText == KPhoneNumber());
    QVERIFY(cliClip == PhoneCallHeaderUtil::ELeft);
    QVERIFY(secondaryCliText == KNullDesC());
    
    m_engineInfo->SetRemoteName(KPhoneRemoteName(), callId);
    
    m_util->GetCliTexts(callId,cliText, cliClip, secondaryCliText, secondaryCliClip);
    QVERIFY(cliText == KPhoneRemoteName());
    QVERIFY(cliClip == PhoneCallHeaderUtil::ERight);
    QVERIFY(secondaryCliText == KPhoneNumber());
    QVERIFY(secondaryCliClip == PhoneCallHeaderUtil::ELeft);
    
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCLIConferenceCall);
    m_util->GetCliTexts(conferenceCallId,cliText, cliClip, secondaryCliText, secondaryCliClip);
    QVERIFY(verify());
    reset();
    
    secondaryCliText = KNullDesC();
    m_util->GetCliTexts(emergencyCallId,cliText, cliClip, secondaryCliText, secondaryCliClip);
    QVERIFY( secondaryCliText == KNullDesC() );
    QVERIFY( cliClip == PhoneCallHeaderUtil::ERight );
    
    m_util->GetSecondaryCli(emergencyCallId, secondaryCliText, secondaryCliClip);
    QVERIFY( secondaryCliText == KNullDesC() );
    
    m_util->GetCli(emergencyCallId, cliText, cliClip);
    QVERIFY( secondaryCliText == KNullDesC() );
}

void TestPhoneCallHeaderUtil::testGetRemoteInfoData()
{
    const int callId = 1;
    const int conferenceCallId = 8;
    const int emergencyCallId = 7;
    _LIT(KPhoneNumber,"123456");
    _LIT(KPhoneRemoteName,"test_remotename");
    _LIT(KPhoneRemotePartyName,"test_remotepartyname");
    _LIT(KPhoneRemoteCompanyName,"test_remotecompanyname");
    TBuf<100> cliText;
    PhoneCallHeaderUtil::ClippingDirection cliClip;
        
    m_engineInfo->SetRemotePhoneNumber(KPhoneNumber(), callId);
    m_engineInfo->SetRemoteName(KPhoneRemoteName(), callId);

    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(cliText == KPhoneRemoteName());
    QVERIFY(cliClip == PhoneCallHeaderUtil::ERight);
    
    m_engineInfo->SetRemoteName(KNullDesC(), callId);
    m_engineInfo->SetRemotePartyName(KPhoneRemotePartyName(), callId);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(cliText == KPhoneRemotePartyName());
    QVERIFY(cliClip == PhoneCallHeaderUtil::ERight);
    
    m_engineInfo->SetRemotePartyName(KNullDesC(), callId);
    m_engineInfo->SetRemoteCompanyName(KPhoneRemoteCompanyName(), callId);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(cliText == KPhoneRemoteCompanyName());
    QVERIFY(cliClip == PhoneCallHeaderUtil::ERight);
    
    m_engineInfo->SetRemoteCompanyName(KNullDesC(), callId);
    m_engineInfo->SetRemotePhoneNumber(KNullDesC(), callId);
    m_engineInfo->SetCallDirection(RMobileCall::EMobileTerminated , callId);
    m_engineInfo->SetRemotePhoneNumberType( EPEPrivateNumber, callId);
       
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCLIWithheld);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetRemotePhoneNumberType( EPEMobileNumber, callId);
    m_engineInfo->SetRemoteIdentity(
            RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone, callId);
    
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCLIPayphone);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetRemoteIdentity(
            RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone, callId);
    
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCLIPayphone);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetRemoteIdentity(
            RMobileCall::ERemoteIdentityUnknown, callId);
    
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCallCLIUnknown);
    m_util->GetCli(callId,cliText, cliClip);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderUtil::testEngineInfo()
{
    const int callId = 1;
    _LIT(KPhoneNumber,"123456");
    m_engineInfo->SetRemotePhoneNumber(KPhoneNumber(),callId);
    QVERIFY(m_util->RemotePhoneNumber(callId)==KPhoneNumber());
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    QVERIFY(m_util->CallType(callId)==EPECallTypeCSVoice);
    
    m_engineInfo->SetServiceId(callId, 1);
    QVERIFY(m_util->ServiceId(callId)==1);
    
    m_engineInfo->SetIncomingCallForwarded(true,callId);
    QVERIFY(m_util->IsCallForwarded(callId)==true);
    
    m_engineInfo->SetSecureSpecified(true);
    QVERIFY(m_util->SecureSpecified()==true);
    
    m_engineInfo->SetCallSecureStatus(true, callId);
    QVERIFY(m_util->Ciphering(callId)==true);
    
    QVERIFY(m_util->CallerImage(callId)==KNullDesC());
    
    m_engineInfo->SetCallState(EPEStateDialing,callId);
    QVERIFY(m_util->CallState(callId)==EPEStateDialing);    
}


void TestPhoneCallHeaderUtil::testIsVoiceCall()
{
    const int callId = 1;
    const int invalidCallId = -1;
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    QVERIFY(true == m_util->IsVoiceCall(callId));
    
    m_engineInfo->SetCallType(EPECallTypeVoIP, callId);
    QVERIFY(true == m_util->IsVoiceCall(callId));
    
    m_engineInfo->SetCallType(EPECallTypeVideo, callId);
    QVERIFY(false == m_util->IsVoiceCall(callId));
    
    m_engineInfo->SetCallTypeCommand(EPECallTypeCSVoice);
    QVERIFY(true == m_util->IsVoiceCall(invalidCallId));
    
    m_engineInfo->SetCallTypeCommand(EPECallTypeVoIP);
    QVERIFY(true == m_util->IsVoiceCall(invalidCallId));
    
    m_engineInfo->SetCallTypeCommand(EPECallTypeVideo);
    QVERIFY(false == m_util->IsVoiceCall(invalidCallId));
}

void TestPhoneCallHeaderUtil::testIsVideoCall()
{
    const int callId = 1;
    const int invalidCallId = -1;
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    QVERIFY(false == m_util->IsVideoCall(callId));
    
    m_engineInfo->SetCallType(EPECallTypeVideo, callId);
    QVERIFY(true == m_util->IsVideoCall(callId));
    
    m_engineInfo->SetCallTypeCommand(EPECallTypeCSVoice);
    QVERIFY(false == m_util->IsVideoCall(invalidCallId));
    
    m_engineInfo->SetCallTypeCommand(EPECallTypeVideo);
    QVERIFY(true == m_util->IsVideoCall(invalidCallId));
}

void TestPhoneCallHeaderUtil::testCallHeaderLabelTextForRingingCall()
{
    const int callId = 1;
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    
    m_engineInfo->SetCallState(EPEStateRinging, callId);       
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneIncomingCallLabel);
    TBuf<100> label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallType(EPECallTypeVideo, callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneVideoCallIncoming);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallType(EPECallTypeCSVoice, callId);
    m_engineInfo->SetCallState(EPEStateConnected, 2);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(EPhoneCallWaitingLabel);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
    
    m_engineInfo->SetCallState(EPEStateIdle, 2);
    m_engineInfo->SetRemotePhoneNumberType(EPEMobileNumber, callId);
    m_engineInfo->SetRemotePhoneNumber(KNullDesC(), callId);
    m_engineInfo->SetRemotePartyName(KNullDesC(), callId);
    EXPECT(CPhoneMainResourceResolver,ResolveResourceID).with(KPhoneRssCommonFirst);
    label = m_util->LabelText(1);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderUtil::testEmergencyHeaderText()
{
    _LIT( KResourceText, "emergency" );
    HBufC* text = KResourceText().AllocL();
    EXPECT(StringLoader,LoadL).returns<HBufC*>(0);
    QVERIFY(m_util->EmergencyHeaderText()==KNullDesC);
    verify();
    EXPECT(StringLoader,LoadL).returns(text);   
    QVERIFY(m_util->EmergencyHeaderText()==text->Des());
    verify();
}

void TestPhoneCallHeaderUtil::testAttemptingEmergencyText()
{
    _LIT( KResourceText, "attempting" );
    HBufC* text = KResourceText().AllocL();
    EXPECT(StringLoader,LoadL).returns<HBufC*>(0);
    QVERIFY(m_util->AttemptingEmergencyText()==KNullDesC);
    verify();
    EXPECT(StringLoader,LoadL).returns(text);   
    QVERIFY(m_util->AttemptingEmergencyText()==text->Des());
    verify();
}

PHONE_TEST_MAIN(TestPhoneCallHeaderUtil)
#include "unit_tests.moc"
