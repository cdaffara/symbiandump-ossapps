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
* Description:  Unit tests for PhoneResourceAdapter.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <hbapplication.h>
#include <QSignalSpy>
#include <QVariant>
#include <mockservice.h>
#include <cpeengineinfo.h>
#include <pevirtualengine.h>
#include "phoneconstants.h"
#include "phonetestmain.h"
#include "phonecallheadermanager.h"
#include "phoneuiqtviewif_stub.h"
#include "bubblemanagerif_stub.h"
#include "phonebubblewrapper.h"
#include "phonecallheaderutil.h"

class TestPhoneCallHeaderManager : public QObject, MockService
{
    Q_OBJECT
public:
    TestPhoneCallHeaderManager();
    virtual ~TestPhoneCallHeaderManager();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup(); 
    
private slots:
    void testCreateCallHeader();
    void testCreateEmergencyCallHeader();
    void testRemoveCallHeader();
    void testUpdateCallHeader();
    void testHandleCipheringInfoChange();
    void testConferenceBubble();
    void testExpandedConferenceCallHeader();
    void testRemoveAllCallHeaders();
    void testIsVoiceCall();

private:
    
private:
    BubbleManagerIFStub *mBubbleManagerIFStub;
    PhoneUIQtViewIFStub *mPhoneUIQtViewIFStub;
    PhoneBubbleWrapper *mPhoneBubbleWrapper;
    CPEEngineInfo *iEngineInfo;
    PhoneCallHeaderManager *mCallHeaderManager; // class under test
};

TestPhoneCallHeaderManager::TestPhoneCallHeaderManager ()
{
}

TestPhoneCallHeaderManager::~TestPhoneCallHeaderManager ()
{
}

void TestPhoneCallHeaderManager::initTestCase ()
{
    mBubbleManagerIFStub =  new BubbleManagerIFStub();
    mPhoneUIQtViewIFStub = new PhoneUIQtViewIFStub(*mBubbleManagerIFStub);
    TRAP_IGNORE( iEngineInfo = CPEEngineInfo::NewL());
    mPhoneBubbleWrapper = new PhoneBubbleWrapper(*mBubbleManagerIFStub);
    mCallHeaderManager = new PhoneCallHeaderManager(
            *mPhoneBubbleWrapper, *mPhoneUIQtViewIFStub);
    
    mCallHeaderManager->setEngineInfo(iEngineInfo);
}

void TestPhoneCallHeaderManager::cleanupTestCase ()
{
    delete mCallHeaderManager;
    delete mPhoneUIQtViewIFStub;
    delete mBubbleManagerIFStub;
    delete mPhoneBubbleWrapper;
    delete iEngineInfo;
}

void TestPhoneCallHeaderManager::init ()
{
    initialize();
}

void TestPhoneCallHeaderManager::cleanup ()
{
    reset();
}

void TestPhoneCallHeaderManager::testCreateCallHeader ()
{
    int callId = 1;
    iEngineInfo->SetCallState(EPEStateRinging, callId );
    
    EXPECT( PhoneBubbleWrapper, createCallHeader);
    mCallHeaderManager->createCallHeader(callId);
    QVERIFY(verify());
    
    iEngineInfo->SetCallState(EPEStateConnecting, callId );
    mIsConferenceExpanded = true;
    mSetExpandedConferenceCalled = false;
    
    mCallHeaderManager->createCallHeader(callId);
    QVERIFY(verify());
    QVERIFY(mSetExpandedConferenceCalled);
    
    mSetExpandedConferenceCalled = false;
    mIsConferenceExpanded = false;
}

void TestPhoneCallHeaderManager::testCreateEmergencyCallHeader()
{
    int callId = 1;

    EXPECT( PhoneBubbleWrapper, createCallHeader);
    EXPECT( PhoneBubbleWrapper, setLabel);
    EXPECT( PhoneBubbleWrapper, setCli);
    EXPECT( PhoneBubbleWrapper, setCiphering);
    mCallHeaderManager->createEmergencyCallHeader(callId);
    QVERIFY(verify());
}

void TestPhoneCallHeaderManager::testRemoveCallHeader()
{
    int callId = 1;
    int bubbleId = 1;
    QMap<int,int> bubbleMap;
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);
    EXPECT( PhoneBubbleWrapper, removeCallHeader).times(0);
    mCallHeaderManager->removeCallHeader(callId);
    QVERIFY(verify());
    reset();
    
    bubbleMap.insert(callId, bubbleId);

    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);
    EXPECT( PhoneBubbleWrapper, removeCallHeader).with(callId);
    mCallHeaderManager->removeCallHeader(callId);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderManager::testUpdateCallHeader()
{
    int callId = 1;
    int bubbleId = -1;
    EXPECT( PhoneBubbleWrapper, bubbleId).returns(bubbleId);
    EXPECT( PhoneBubbleWrapper, setState).times(0);
    mCallHeaderManager->updateCallHeader(callId);
    QVERIFY(verify());
    reset();
    
    bubbleId = 1;

    EXPECT( PhoneBubbleWrapper, bubbleId).returns(bubbleId);
    EXPECT( PhoneBubbleWrapper, setState).times(1);
    mCallHeaderManager->updateCallHeader(callId);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderManager::testHandleCipheringInfoChange()
{
    iEngineInfo->SetCallSecureStatus( ETrue, 0 );
    iEngineInfo->SetCallSecureStatus( EFalse, 1 );
    int callId = KPEConferenceCallID;
    int bubbleId = -1;
    QList<int> conferenceList;
    conferenceList.append(0);
    conferenceList.append(1);
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceList);
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceList);
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceList);
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceList);
    EXPECT( PhoneBubbleWrapper, setCiphering);
    mCallHeaderManager->handleCipheringInfoChange(callId);
    QVERIFY(verify());
    reset();
    
    callId = 1;

    EXPECT( PhoneBubbleWrapper, bubbleId).returns(bubbleId);
    EXPECT( PhoneBubbleWrapper, setCiphering).times(0);
    mCallHeaderManager->handleCipheringInfoChange(callId);
    QVERIFY(verify());
    reset();
    
    bubbleId = 1;
    
    EXPECT( PhoneBubbleWrapper, bubbleId).returns(bubbleId);
    EXPECT( PhoneBubbleWrapper, setCiphering);
    mCallHeaderManager->handleCipheringInfoChange(callId);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderManager::testConferenceBubble()
{
    int callId = 7;
    EXPECT( PhoneBubbleWrapper, createConferenceBubble);
    mCallHeaderManager->createConferenceBubble(callId);
    QVERIFY(verify());
    reset();
    
    EXPECT( PhoneBubbleWrapper, removeConferenceBubble);
    mCallHeaderManager->removeConferenceBubble();
    QVERIFY(verify());
    reset();
    
    QMap<int,int> bubbleMap;
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);
    EXPECT( PhoneBubbleWrapper, removeCallHeader).with(callId);
    mCallHeaderManager->removeCallFromConference(callId);
    QVERIFY(verify());
    reset();
    
    EXPECT( PhoneBubbleWrapper, removeCallFromConference).with(callId);
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);
    mCallHeaderManager->setPrivateFromConference(callId);
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderManager::testExpandedConferenceCallHeader()
{

    QMap<int,int> bubbleMap;
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap).times(2);    
    mCallHeaderManager->setExpandedConferenceCallHeader();
    QVERIFY(verify());
    QVERIFY(false == mIsExpandedConference);
    reset();
    
    bubbleMap.insert(1, 2);
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);
    EXPECT( PhoneBubbleWrapper, callIdByBubbleId).returns(KConferenceCallId);
    mCallHeaderManager->setExpandedConferenceCallHeader();
    QVERIFY(verify());
    QVERIFY(true == mIsExpandedConference);
    reset();
}

void TestPhoneCallHeaderManager::testRemoveAllCallHeaders()
{

    QList<int> conferenceListMap;
    QMap<int,int> bubbleMap;
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceListMap);
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap);    
    mCallHeaderManager->removeAllCallHeaders();
    QVERIFY(verify());
    reset();
    
    conferenceListMap.append(1);
    bubbleMap.insert(1,1);
    bubbleMap.insert(KEmergencyCallId, KEmergencyCallId);
    
    EXPECT( PhoneBubbleWrapper, conferenceCallList).returns(conferenceListMap);
    EXPECT( PhoneBubbleWrapper, removeConferenceBubble);
    EXPECT( PhoneBubbleWrapper, bubbles).returns(bubbleMap).times(2);
    EXPECT( PhoneBubbleWrapper, removeCallHeader).with(1);
    mCallHeaderManager->removeAllCallHeaders();
    QVERIFY(verify());
    reset();
}

void TestPhoneCallHeaderManager::testIsVoiceCall()
{
    EXPECT( PhoneCallHeaderUtil, IsVoiceCall).returns(true);
    QVERIFY(mCallHeaderManager->isVoiceCall( 1 ));
    QVERIFY(verify());
    reset();
}

PHONE_TEST_MAIN(TestPhoneCallHeaderManager)
#include "unit_tests.moc"
