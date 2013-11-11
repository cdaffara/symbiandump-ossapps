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
* Description:  Unit tests for PhoneBubbleWrapper.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QSignalSpy>
#include "phonetestmain.h"
#include "phonebubblewrapper.h"
#include "bubblemanagerif.h"
#include "pevirtualengine.h"

class TestPhoneBubbleWrapper : public QObject, public BubbleManagerIF
{
    Q_OBJECT
public:
    TestPhoneBubbleWrapper();
    virtual ~TestPhoneBubbleWrapper();
    QString sayTestPhoneBubbleWrapper ();

    // From BubbleManagerIF
    void startChanges () { m_startChangesCalled=true; };
    void endChanges () { m_endChangesCalled=true; };
    int createCallHeader () { return 5; };
    void removeCallHeader (int bubbleId) { m_removeCallHeaderCalled=true; m_int=bubbleId; };
    void setState (int bubbleId, PhoneCallState state) { m_int=bubbleId; m_bubbleState=state; };
    void setLabel ( 
        int bubbleId, 
        const QString& text, 
        Qt::TextElideMode clipDirection) { m_int=bubbleId; m_string=text; m_elideMode=clipDirection; };
    void setCli (int bubbleId, 
                 const QString& cliText, 
                 Qt::TextElideMode clipDirection) { m_int=bubbleId; m_string=cliText; m_elideMode=clipDirection; };
    void updateCLI ( 
        int bubbleId, 
        const QString& cliText, 
        Qt::TextElideMode clipDirection) {};
    void setSecondaryCli ( 
        int bubbleId, 
        const QString& cliText, 
        Qt::TextElideMode clipDirection = Qt::ElideLeft) { m_int=bubbleId; m_string=cliText; m_elideMode=clipDirection; };
    void setCallTime (int bubbleId, const QString& callTime) {};
    void updateCallTime (int bubbleId, const QString& callTime) {};
    void setCallObjectImage (int bubbleId, const QString &fileName) {};
    void setCallObjectFromTheme (int bubbleId) {};
    void setCallObjectText (int bubbleId, const QString &text) {};
    void setCallFlags (int bubbleId, int flags) {};
    void setCallFlag (int bubbleId, PhoneCallFlags flag, bool set) { m_setCallFlagCalled=true; m_int=bubbleId; m_callFlags=flag; m_set=set; };
    void setNumberType (int bubbleId, PhoneNumberType type) {};
    void updateCallHeaderText( 
        int bubbleId, 
        const QString &longText, 
        const QString &shortText, 
        Qt::TextElideMode clipDirection) {};
    void setPhoneMuted (bool muted) {};
    int createConference (int bubble1, int bubble2) { return 1; };
    void removeConference () {};
    void addRowToConference (int bubbleId) {};
    void removeRowFromConference (int bubbleId) {};
    int conferenceRowCount () const { return 2; };
    void setSelectionInConference (int rowNumber) {};
    void setSelectionIdInConference (int bubbleId) {};
    int selectionInConference () const { return 0; };
    int selectionIdInConference () const { return 0; };
    void moveHighlightOneUpInConference () {};
    void moveHighlightOneDownInConference () {};
    void setExpandedConferenceCallHeader (bool expanded) {};
    bool isConferenceExpanded () const { return false; };
    int shownHeaderCount () const { return 1; };
    void setParticipantListCli (int aBubbleId, ParticipantListCli aParticipantCli) {};
    void addAction (int bubble, HbAction *action) {};
    void clearActions (int bubble) {};
    QGraphicsWidget* graphicsWidgetForAction (HbAction *action) const { return 0;};
    void addParticipantListAction (HbAction *action) {};
    void clearParticipantListActions () {};
    int expandedBubble() const { return 0; };
    void setExpandAction(int bubbleId, HbAction* action) {};
    void setBubbleSelectionDisabled(bool b) {};

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testBubbleManager ();
    void testSetLabel ();
    void testSetCli ();
    void testSetSecondaryCli ();
    void testSetCallType ();
    void testCreateCallHeader ();
    void testRemoveCallHeader ();
    void testSetState ();
    void testBubbleId ();
    void testSetDivert();
    void testSetCiphering();
    void testActiveCallCount();
    void testCallStates ();
    void testBubbles ();
    void testSetConferenceCallId ();
    void testRemoveConferenceCallId ();
    void testCreateConferenceBubble ();
    void testRemoveCallFromConference ();
    void testRemoveConferenceBubble ();
    void testCallIdByBubbleId ();
    void testAddToConferenceBubble();
    void testSetServiceId();

private:
    PhoneBubbleWrapper *m_wrapper; // class under test
    QString m_string;
    int m_int;
    Qt::TextElideMode m_elideMode;
    bool m_startChangesCalled;
    bool m_endChangesCalled;
    bool m_removeCallHeaderCalled;
    bool m_setCallFlagCalled;
    bool m_set;
    PhoneCallState m_bubbleState;
    PhoneCallFlags m_callFlags;
};

TestPhoneBubbleWrapper::TestPhoneBubbleWrapper ()
{
}

TestPhoneBubbleWrapper::~TestPhoneBubbleWrapper ()
{
}

void TestPhoneBubbleWrapper::initTestCase ()
{
                    
}

void TestPhoneBubbleWrapper::cleanupTestCase ()
{
//    delete m_wrapper;
    
}

void TestPhoneBubbleWrapper::init ()
{
    m_wrapper = new PhoneBubbleWrapper (*this);
    m_string = "";
    m_int = -1;
    m_elideMode = Qt::ElideNone;
    m_startChangesCalled = false;
    m_endChangesCalled = false;
    m_removeCallHeaderCalled = false;
    m_setCallFlagCalled = false;
    m_set = false;
    m_callFlags = BubbleManagerIF::Normal;
}

void TestPhoneBubbleWrapper::cleanup ()
{
    delete m_wrapper;
    m_wrapper = 0;
}

QString TestPhoneBubbleWrapper::sayTestPhoneBubbleWrapper ()
{
    QString say ("TestPhoneBubbleWrapper");
    return say;
}

void TestPhoneBubbleWrapper::testBubbleManager ()
{
    TestPhoneBubbleWrapper &fetched = reinterpret_cast<TestPhoneBubbleWrapper&>(m_wrapper-> bubbleManager ());
	
    // Comparing two references not possible so we circumvent it this way
    // to be sure we are dealing with same object.
    // fetched should be this object so calling sayTestPhoneBubbleWrapper should
    // return a QString - if it's not this object it should simply crash.
    QCOMPARE (fetched.sayTestPhoneBubbleWrapper (), sayTestPhoneBubbleWrapper () );
}

void TestPhoneBubbleWrapper::testSetLabel ()
{
    _LIT (KTestText, "Test Name");
    m_wrapper->setLabel (2, KTestText);
    QCOMPARE (m_int, 2);
    QCOMPARE (m_string, QString ("Test Name"));
    QCOMPARE (m_elideMode, Qt::ElideRight);
}

void TestPhoneBubbleWrapper::testSetCli ()
{
    _LIT (KTestText, "Test Name");
    m_wrapper->setCli (3, KTestText, Qt::ElideRight);
    QCOMPARE (m_int, 3);
    QCOMPARE (m_string, QString ("Test Name"));
    QCOMPARE (m_elideMode, Qt::ElideRight);	
}

void TestPhoneBubbleWrapper::testSetSecondaryCli ()
{
    _LIT (KTestText, "Test Name");
    m_wrapper->setSecondaryCli (4, KTestText, Qt::ElideLeft);
    QCOMPARE (m_int, 4);
    QCOMPARE (m_string, QString ("Test Name"));
    QCOMPARE (m_elideMode, Qt::ElideLeft);	
}

void TestPhoneBubbleWrapper::testSetCallType ()
{
    m_wrapper->setCallType(0, EPECallTypeVideo);
    m_wrapper->setCallType(0, EPECallTypeVoIP);
    m_wrapper->setCallType(0, 100);
    
    m_wrapper->setCallType(1, EPECallTypeCSVoice);
    
    QVERIFY(m_setCallFlagCalled);
    QCOMPARE(m_int, 1);
    QCOMPARE(m_callFlags, BubbleManagerIF::Normal);
}

void TestPhoneBubbleWrapper::testCreateCallHeader ()
{
    QCOMPARE (m_wrapper->createCallHeader (1), 5);
    QCOMPARE (m_wrapper->createCallHeader (1), 5);
    m_wrapper->removeCallHeader(1);
}

void TestPhoneBubbleWrapper::testRemoveCallHeader ()
{
    m_wrapper->removeCallHeader (3);
    QVERIFY (m_startChangesCalled == false);
    QVERIFY (m_removeCallHeaderCalled == false);
    QVERIFY (m_endChangesCalled == false);
    
    m_wrapper->createCallHeader (2);
    m_wrapper->removeCallHeader (2);

    QVERIFY (m_removeCallHeaderCalled == true);
    QCOMPARE (m_int, 5);
}

void TestPhoneBubbleWrapper::testSetState ()
{
    m_wrapper->setState (0, 1, EPEStateIdle);
    QCOMPARE(m_int, 1);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Disconnected);
    QCOMPARE(m_wrapper->callId (EPEStateIdle), 0);
    
    m_wrapper->setState (0, 2, EPEStateDialing);
    QCOMPARE(m_int, 2);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Outgoing);
    QCOMPARE(m_wrapper->callId (EPEStateDialing), 0);

    m_wrapper->setState (0, 3, EPEStateRinging);
    QCOMPARE(m_int, 3);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Incoming);
    QCOMPARE(m_wrapper->callId (EPEStateRinging), 0);

    m_wrapper->setState (0, 4, EPEStateConnecting);
    QCOMPARE(m_int, 4);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Alerting);
    QCOMPARE(m_wrapper->callId (EPEStateConnecting), 0);

    m_wrapper->setState (0, 5, EPEStateConnected);
    QCOMPARE(m_int, 5);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Active);
    QCOMPARE(m_wrapper->callId (EPEStateConnected), 0);

    m_wrapper->setState (0, 6, EPEStateHeld);
    QCOMPARE(m_int, 6);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::OnHold);
    QCOMPARE(m_wrapper->callId (EPEStateHeld), 0);

    m_wrapper->setState (0, 7, EPEStateDisconnecting);
    QCOMPARE(m_int, 7);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Disconnected);
    QCOMPARE(m_wrapper->callId (EPEStateDisconnecting), 0);

    m_wrapper->setState (0, 8, EPEStateConferenceIdle);
    QCOMPARE(m_int, 8);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Disconnected);
    QCOMPARE(m_wrapper->callId (EPEStateConferenceIdle), 0);

    m_wrapper->setState (0, 9, EPEStateConnectedConference);
    QCOMPARE(m_int, 9);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::Active);
    QCOMPARE(m_wrapper->callId (EPEStateConnectedConference), 0);

    m_wrapper->setState (0, 10, EPEStateHeldConference);
    QCOMPARE(m_int, 10);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::OnHold);
    QCOMPARE(m_wrapper->callId (EPEStateHeldConference), 0);
    
    m_wrapper->setState (0, 11, EPEStateUnknown);
    QCOMPARE(m_int, 11);	
    QCOMPARE(m_bubbleState, BubbleManagerIF::None);
    QCOMPARE(m_wrapper->callId (EPEStateUnknown), 0);

    m_wrapper->createCallHeader (0);
    int bubble = m_wrapper->createCallHeader (1);
    m_wrapper->setState (1, bubble, EPEStateRinging);
    QCOMPARE (m_int, bubble);	
    QCOMPARE (m_bubbleState, BubbleManagerIF::Waiting);    
    QCOMPARE (m_wrapper->callId (EPEStateRinging), 1);
    m_wrapper->removeCallHeader (1);
    m_wrapper->removeCallHeader (0);
    
    m_wrapper->setState (0, 11, 100);
}

void TestPhoneBubbleWrapper::testBubbleId ()
{
    QCOMPARE (m_wrapper->bubbleId (0), -1);
    int bubble = m_wrapper->createCallHeader (0);
    QCOMPARE (m_wrapper->bubbleId (0), bubble);
    m_wrapper->removeCallHeader (0);
    QCOMPARE (m_wrapper->bubbleId (0), -1);    	
}

void TestPhoneBubbleWrapper::testSetDivert ()
{
    m_wrapper->setDivert (2, false);
    QVERIFY (m_setCallFlagCalled == false);
    QCOMPARE (m_int, -1);
    QCOMPARE (m_callFlags, (int)BubbleManagerIF::None);
    QVERIFY (m_set == false);

    m_wrapper->setDivert (1, true);
    QVERIFY (m_setCallFlagCalled == true);
    QCOMPARE (m_int, 1);
    QCOMPARE (m_callFlags, BubbleManagerIF::Diverted);
    QVERIFY (m_set == true);    
}

void TestPhoneBubbleWrapper::testSetCiphering()
{
    m_wrapper->setCiphering(2, false, false);
    QVERIFY(m_setCallFlagCalled == true);
    QCOMPARE(m_callFlags, (int)BubbleManagerIF::NoCiphering);
    QVERIFY(m_int == 2);
    QVERIFY(m_set == false);        
    
    m_wrapper->setCiphering(2, true, true);
    QVERIFY(m_setCallFlagCalled == true);
    QCOMPARE(m_callFlags, (int)BubbleManagerIF::NoCiphering);
    QVERIFY(m_int == 2);
    QVERIFY(m_set == false);    

    m_wrapper->setCiphering(2, true, false);
    QVERIFY(m_setCallFlagCalled == true);
    QCOMPARE(m_callFlags,  (int)BubbleManagerIF::NoCiphering);
    QVERIFY(m_int == 2);
    QVERIFY(m_set == true);        
}

void TestPhoneBubbleWrapper::testActiveCallCount ()
{
    QCOMPARE (m_wrapper->activeCallCount(), 0);
    int bubble = m_wrapper->createCallHeader (0);
    m_wrapper->setState(0,bubble,EPEStateConnected);
    QCOMPARE (m_wrapper->activeCallCount(), 1);
    m_wrapper->removeCallHeader (0);
    QCOMPARE (m_wrapper->activeCallCount(), 0);     
}

void TestPhoneBubbleWrapper::testCallStates ()
{
    QCOMPARE( 0, m_wrapper->callStates().size() );
    
    m_wrapper->createCallHeader (0);
    m_wrapper->createCallHeader (1);
    
    
    m_wrapper->updateCallState(0, 2);
    m_wrapper->updateCallState(1, 3);
    m_wrapper->updateCallState(2, 4);
    
    QCOMPARE( 2, m_wrapper->callStates().size() );
    
    m_wrapper->createCallHeader (2);
    QCOMPARE( 3, m_wrapper->callStates().size() );
    
    m_wrapper->setConferenceCallId(0);
    m_wrapper->setConferenceCallId(1);
    m_wrapper->updateCallState(8, 5);
    
    QCOMPARE( 1, m_wrapper->callStates().size() );
 
}

void TestPhoneBubbleWrapper::testBubbles ()
{
    QCOMPARE( 0, m_wrapper->bubbles().size() );
    
    m_wrapper->createCallHeader (0);
    m_wrapper->createCallHeader (1);
    m_wrapper->createCallHeader (3);
    
    QCOMPARE( 3, m_wrapper->bubbles().size() );
    
    m_wrapper->setConferenceCallId(0);
    
    QCOMPARE( 2, m_wrapper->bubbles().size() );  
}

void TestPhoneBubbleWrapper::testSetConferenceCallId ()
{
    m_wrapper->setConferenceCallId(0);
    m_wrapper->setConferenceCallId(1);
    
    QCOMPARE( 2, m_wrapper->conferenceCallList().size() );
    
    //set same call id
    m_wrapper->setConferenceCallId(1);
    QCOMPARE( 2, m_wrapper->conferenceCallList().size() );
    
    QCOMPARE( true, m_wrapper->conferenceCallId(0) );
    QCOMPARE( false, m_wrapper->conferenceCallId(3) );
}

void TestPhoneBubbleWrapper::testRemoveConferenceCallId ()
{
    m_wrapper->setConferenceCallId(0);
    m_wrapper->setConferenceCallId(1);
    
    QCOMPARE( 2, m_wrapper->conferenceCallList().size() );
    m_wrapper->removeConferenceCallId(3);
    QCOMPARE( 2, m_wrapper->conferenceCallList().size() );
    m_wrapper->removeConferenceCallId(1);
    QCOMPARE( 1, m_wrapper->conferenceCallList().size() );
    QCOMPARE( true, m_wrapper->conferenceCallId(0) );
    QCOMPARE( false, m_wrapper->conferenceCallId(1) );
    m_wrapper->removeConferenceCallId(0);
    QCOMPARE( 0, m_wrapper->conferenceCallList().size() );
}

void TestPhoneBubbleWrapper::testCreateConferenceBubble ()
{
    m_wrapper->createConferenceBubble(8,EPEStateConnectedConference,_L("conference"),_L("conference"));
    QVERIFY( 0 == m_wrapper->conferenceCallList().size() );
    
    m_wrapper->createCallHeader(0);
    m_wrapper->createCallHeader(1);
    
    m_wrapper->createConferenceBubble(0,EPEStateConnectedConference,_L("conference"),_L("conference"));
    
    QVERIFY( 2 == m_wrapper->conferenceCallList().size() );
    
}

void TestPhoneBubbleWrapper::testRemoveCallFromConference ()
{
    m_wrapper->createCallHeader(0);
    m_wrapper->createCallHeader(1);
    
    m_wrapper->createConferenceBubble(0,EPEStateConnectedConference,_L("conference"),_L("conference"));

    m_wrapper->removeCallFromConference(0);

    QVERIFY( 1 == m_wrapper->conferenceCallList().size() );
    
    m_wrapper->removeConferenceCallId(1);
    
    QVERIFY( 0 == m_wrapper->conferenceCallList().size() );
}

void TestPhoneBubbleWrapper::testRemoveConferenceBubble ()
{
    m_wrapper->createCallHeader(0);
    m_wrapper->createCallHeader(1);
    
    m_wrapper->createConferenceBubble(0,EPEStateConnectedConference,_L("conference"),_L("conference"));
    m_wrapper->removeConferenceBubble();
    
    QVERIFY( 0 == m_wrapper->conferenceCallList().size() );
}

void TestPhoneBubbleWrapper::testAddToConferenceBubble ()
{
    m_wrapper->addToConferenceBubble();

    int bubbleId = m_wrapper->createCallHeader(0);
    m_wrapper->setState(0, bubbleId, EPEStateRinging);
    int bubbleId2 = m_wrapper->createCallHeader(1);
    m_wrapper->setState(1, bubbleId2, EPEStateConnected);
    int bubbleId3 = m_wrapper->createCallHeader(2);
    m_wrapper->setState(2, bubbleId3, EPEStateHeld);
    
    m_wrapper->createConferenceBubble(8,EPEStateConnectedConference,_L("conference"),_L("conference"));
    
    m_wrapper->addToConferenceBubble();
    
    QCOMPARE( 3, m_wrapper->conferenceCallList().size() );
    
    m_wrapper->removeCallFromConference(0);
    QCOMPARE( 2, m_wrapper->conferenceCallList().size() );
    
    int bubbleId4 = m_wrapper->createCallHeader(3);
    m_wrapper->setState(3, bubbleId4, EPEStateHeld);
    
    m_wrapper->addToConferenceBubble();
    QCOMPARE( 3, m_wrapper->conferenceCallList().size() );
    
    m_wrapper->removeCallFromConference(0);
    m_wrapper->removeCallHeader(0);
    QCOMPARE( 3, m_wrapper->conferenceCallList().size() );
    
    m_wrapper->removeCallFromConference(1);
    m_wrapper->removeCallHeader(1);
    m_wrapper->removeCallFromConference(2);
    m_wrapper->removeCallHeader(2);
    m_wrapper->removeCallFromConference(3);
    m_wrapper->removeCallHeader(3);
    
    QCOMPARE( 0, m_wrapper->conferenceCallList().size() );
}

void TestPhoneBubbleWrapper::testCallIdByBubbleId ()
{
		int callId = 1;
    int bubbleId = m_wrapper->createCallHeader(callId);
    
    QVERIFY( callId == m_wrapper->callIdByBubbleId(bubbleId) );
}

void TestPhoneBubbleWrapper::testSetServiceId()
{
    int serviceId(1);
    int callId(2);
    int callId2(3);
    
    m_wrapper->setServiceId(callId, serviceId);
    
    QVERIFY(serviceId == m_wrapper->serviceIdByCallId(callId));
    QVERIFY(-1 == m_wrapper->serviceIdByCallId(callId2));
    QVERIFY(1 == m_wrapper->serviceIds().keys().count());
    
}

PHONE_TEST_MAIN(TestPhoneBubbleWrapper)
#include "unit_tests.moc"
