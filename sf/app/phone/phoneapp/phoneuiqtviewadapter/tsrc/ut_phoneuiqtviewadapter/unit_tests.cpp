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
* Description:  Unit tests for PhoneUIQtViewAdapter.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QList>
#include <QKeyEvent>
#include <hbapplication.h>
#include <QSignalSpy>
#include <hbmainwindow.h>
#include <phoneappcommands.hrh>
#include <hbmenu.h>
#include "phoneuiqtviewadapter.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamaudiooutput.h"
#include "phoneuiqtviewif.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamemergencycallheaderdata.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamringtone.h"
#include "tphonecmdparamkeycapture.h"
#include "pevirtualengine.h"
#include "bubblemanagerif.h"
#include "phoneresourceids.h"
#include "phonebubblewrapper.h"
#include "phoneuiqtbuttonscontroller.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamstring.h"
#include "phoneindicatorcontroller.h"
#include "phoneuitestmain.h"

extern int m_phoneButtonFlags;
extern bool m_EPhoneViewMuteRingToneOnAnswer_called;
extern bool m_EPhoneViewStopRingTone_called;
extern bool m_EPhoneViewMuteRingTone_called;
extern bool m_EPhoneViewPlayRingTone_called;
extern bool m_showGlobalNoteCalled;
extern bool m_removeCallHeaderCalled;
extern bool m_addToConferenceBubbleCalled;
extern bool m_showQueryCalled;
extern bool m_removeDtmfNoteCalled;
extern bool m_removeNoteCalled;
extern bool m_removeQueryCalled;
extern bool m_showNoteCalled;
extern bool m_removeGlobalWaitNoteCalled;
extern bool m_ordinalPositionCalled;
extern bool m_sendToBackgroundCalled;
extern bool m_phoneVisibleReturnValue;
extern bool m_createCallHeaderCalled;
extern int m_createCallHeaderCallId;
extern bool m_createEmergencyCallHeaderCalled;
extern int m_updateCallHeaderCallId;
extern int m_updateCallHeaderRemoteInfoCallId;
extern int m_updateCallHeaderRemoteInfoAndLabelCallId;
extern int m_handleCipheringInfoChangeCallId;
extern int m_createConferenceBubbleCallId;
extern bool m_removeAllCallHeadersCalled;
extern bool m_removeConferenceBubbleCalled;
extern int m_removeCallFromConferenceCallId;
extern int m_setPrivateFromConferenceCallId;
extern int m_removeCallHeaderCallId;
extern bool m_isVoiceCall;

// Own assert initialization  
/*void qt_assert(const char *assertion, const char *file, int line)
{
    qDebug () << "View test assert FAIL";
}*/

class TestPhoneUIQtViewAdapter : public QObject, public PhoneUIQtViewIF, public BubbleManagerIF
{
    Q_OBJECT
public:
    TestPhoneUIQtViewAdapter();
    virtual ~TestPhoneUIQtViewAdapter();

    // From PhoneUIQtViewIF
    BubbleManagerIF& bubbleManager ();
    void addBubbleCommand (int bubbleId, const PhoneAction& action);
    void clearBubbleCommands (int bubbleId);
    void addParticipantListAction(
            int commandId,  
            const QString &text, 
            const HbIcon &icon);
    void clearParticipantListActions();
    void hideToolbar () { };
    void showToolbar () { m_showToolbarCalled = true; };
    void setToolbarActions (const QList<PhoneAction*>& actions) {m_toolbarActionCount = actions.count(); };
    int volumeSliderValue () { m_volumeSliderValueCalled = true; return 5; };
    void removeVolumeSlider () { m_removeVolumeSliderCalled = true; };
    void setVolumeSliderValue (
            int value, 
            int , 
            int , 
            int  ) { m_setVolumeSliderValueCalled = true; m_volumeSliderValue = value; };

    void setExpandAction(int bubbleId, int commandId) {m_expandAction[bubbleId]=commandId;};
    void removeExpandAction(int ) {};
    void showDialpad() {m_showDialpadCalled = true;};  
    void hideDialpad() {m_hideDialpadCalled = true;};  
    bool isDialpadVisible() 
        {m_dialpadVisibilityCalled = true;
    return m_isDialpadVisible; }
    QString dialpadText() {return m_dialpadText;};
    void clearDialpad() { m_clearDialpadCalled = true; };
    void clearAndHideDialpad() { m_clearAndHideDialpadCalled = true;};
    void bringToForeground() {};
    void setMenuActions(const QList<PhoneAction*>& ) { m_setMenuActionsCalled = true;};
    void shutdownPhoneApp() {};
    void setBackButtonVisible(bool ) {};
    HbMenu &menuReference(){return m_menu;};
    void captureKey(Qt::Key key, bool capture) {
        m_capturedKey = key;
        m_captured = capture;};
    
    // From BubbleManagerIF
    void startChanges () { m_startChangesCalled=true; };
    void endChanges () { m_endChangesCalled=true; };
    int createCallHeader () { return 0; };
    void removeCallHeader (int ) {};
    void setState (int , PhoneCallState ) {};
    void setLabel ( 
        int , 
        const QString& , 
        Qt::TextElideMode ) {};
    void setCli (int , 
                 const QString& , 
                 Qt::TextElideMode ) {};
    void updateCLI ( 
        int , 
        const QString& , 
        Qt::TextElideMode ) {};
    void setSecondaryCli ( 
        int , 
        const QString& , 
        Qt::TextElideMode ) {};
    void setCallTime (int , const QString& ) {};
    void updateCallTime (int bubbleId, const QString& callTime) { m_updateCallTimeCalled=true; m_int=bubbleId; m_string=callTime; };
    void setCallObjectImage (int , const QString &) {};
    void setCallObjectFromTheme (int ) {};
    void setCallObjectText (int , const QString &) {};
    void setCallFlags (int , int ) {};
    void setCallFlag (int , PhoneCallFlags , bool ) {};
    void setNumberType (int , PhoneNumberType ) {};
    void updateCallHeaderText( 
        int , 
        const QString &, 
        const QString &, 
        Qt::TextElideMode ) {};
    void setPhoneMuted (bool muted) { m_setPhoneMutedCalled = muted; };
    int createConference (int , int ) { return 1; };
    void removeConference () {};
    void addRowToConference (int ) {};
    void removeRowFromConference (int ) {};
    int conferenceRowCount () const { return 2; };
    void setSelectionInConference (int ) {};
    void setSelectionIdInConference (int ) {};
    int selectionInConference () const { return 0; };
    int selectionIdInConference () const { return 0; };
    void moveHighlightOneUpInConference () {};
    void moveHighlightOneDownInConference () {};
    void setExpandedConferenceCallHeader (bool ) { m_ExpandConferenceCalled = true; };
    bool isConferenceExpanded () const { return false; };
    int shownHeaderCount () const { return 1; };
    void setParticipantListCli (int , ParticipantListCli ) {};
    void addAction (int , HbAction *) {};
    void clearActions (int ) {};
    QGraphicsWidget* graphicsWidgetForAction (HbAction *) const { return 0;};
    void addParticipantListAction (HbAction *) {};
    int expandedBubble() const { return 1; };
    void setExpandAction(int , HbAction* ) {};
    void setBubbleSelectionDisabled(bool ) {};
    void setRestrictedMode(bool ) {};
    
signals:
    void dialpadAboutToClose();
    void keyPressReleased(QKeyEvent *event);
    void windowActivated();
    void windowDeactivated();
    void focusLost();
    void focusGained();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup ();
    
private slots:
    void testEPhoneViewUpdateCallHeaderCallDurationCommand ();
    void testEPhoneViewCreateCallHeaderCommand ();
    void testEPhoneViewCreateCallHeaderCommand2 ();
    void testEPhoneViewCreateEmergencyCallHeader ();
    void testEPhoneViewUpdateBubble ();
    void testCallIdByState ();
    void testSetToolbarButtons ();
    void testEPhoneViewHideNaviPaneAudioVolume ();
    void testEPhoneViewSetNaviPaneAudioVolume ();
    void testEPhoneViewGetAudioVolumeLevel ();
    void testEPhoneViewActivateMuteUIChanges ();
    void testEPhoneViewGetCountOfActiveCallss ();
    void testExecuteCommandLwithCmdId ();
    void testExecuteCommandLwithCmdIdAndCallId ();
    void testExecuteCommandLwithCmdIdAndCmdParam ();
    void testSetTouchButtons ();
    void testCreateConferenceBubble ();
    void testHandleCommandL ();
    void testGetSelectedConferenceMember ();
    void testSetAudioPath ();
    void testMuteRingToneOnAnswer ();
    void testStopRingTone ();
    void testMuteRingTone ();
    void testPlayRingTone ();
    void testShowGlobalNote ();
    void testSetExpandActions();
    void testShowDialpad ();
    void testDialpadVisibility ();
    void removeAllCallHeaders ();
    void testRemoveDialpad();
    void testGetDialpadStringLength();
    void testGetDialpadString();
    void testAddToConference();
    void testSetConferenceExpanded();
    void testSetMenuActions();
    void testNoteController();
    void testHsToForegroundAfterCall();
    void testCipheringInfoChange();
    void testSetHidden();
    void testBeginEndUiUpdate();
    void testIndicatorController();
    void testOpenLogs();
    void testSetFlags();
    void testCaptureEndKey();

private:
    PhoneUIQtViewAdapter *m_adapter; // class under test
    bool m_showToolbarCalled;
    bool m_updateCallTimeCalled;
    int m_int;
    QString m_string;
    HbAction *m_action;
    int  m_toolbarActionCount;
    bool m_startChangesCalled;
    bool m_endChangesCalled;
    bool m_volumeSliderValueCalled;
    bool m_removeVolumeSliderCalled;
    bool m_setVolumeSliderValueCalled;
    int m_volumeSliderValue;
    bool m_setPhoneMutedCalled;
    QMap<int, QList<int> *> m_actionMap;
    QMap<int,int> m_expandAction;
    bool m_showDialpadCalled;
    bool m_hideDialpadCalled;
    bool m_dialpadVisibilityCalled;
    bool m_clearDialpadCalled;
    bool m_clearAndHideDialpadCalled;
    bool m_ExpandConferenceCalled;
    bool m_setMenuActionsCalled;
    bool m_isDialpadVisible;
    QString m_dialpadText;
    HbMenu m_menu;
    bool m_captured;
    Qt::Key m_capturedKey;
    QList<int> mParticipantListActions;
};

TestPhoneUIQtViewAdapter::TestPhoneUIQtViewAdapter () :
    m_isDialpadVisible(false),
    m_dialpadText("1234567")
{
}

TestPhoneUIQtViewAdapter::~TestPhoneUIQtViewAdapter ()
{
}

void TestPhoneUIQtViewAdapter::initTestCase ()
{
    m_adapter = new PhoneUIQtViewAdapter (*this);
}

void TestPhoneUIQtViewAdapter::cleanupTestCase ()
{
    delete m_adapter;
}

void TestPhoneUIQtViewAdapter::init ()
{
    m_startChangesCalled = false;
    m_endChangesCalled = false;
    m_setCipheringCalled = false;
    m_isVoiceCall = true;
}

void TestPhoneUIQtViewAdapter::cleanup ()
{
}

BubbleManagerIF& TestPhoneUIQtViewAdapter::bubbleManager ()
{
    return *this;
}

void TestPhoneUIQtViewAdapter::addBubbleCommand (
        int bubbleId, const PhoneAction& action)
{
    if (m_actionMap.keys().contains(bubbleId)) {
        m_actionMap.value(bubbleId)->append(action.command());
    } else {
        QList<int> *list = new QList<int>();
        list->append(action.command());
        m_actionMap.insert( bubbleId, list);
    }
}

void TestPhoneUIQtViewAdapter::clearBubbleCommands (int bubbleId)
{
    if (m_actionMap.keys().contains(bubbleId)) {
        QList<int> *list = m_actionMap.value(bubbleId);
        list->clear();
        delete list;
        m_actionMap.remove(bubbleId);
    }
}

void TestPhoneUIQtViewAdapter::addParticipantListAction(
    int commandId,
    const QString& , 
    const HbIcon& )
{
    mParticipantListActions.append(commandId);
}

void TestPhoneUIQtViewAdapter::clearParticipantListActions()
{
    mParticipantListActions.clear();
}

void TestPhoneUIQtViewAdapter::testEPhoneViewUpdateCallHeaderCallDurationCommand ()
{
    TPhoneCmdParamInteger time;
    time.SetInteger(73);
    m_adapter->ExecuteCommandL (EPhoneViewUpdateCallHeaderCallDuration, 1, &time);	
    QCOMPARE (m_updateCallTimeCalled, true);
    QCOMPARE (m_int, 2);
    QVERIFY (m_string == "0:01:13");
}

void TestPhoneUIQtViewAdapter::testEPhoneViewCreateCallHeaderCommand ()
{
    int callId = 2;
    m_createCallHeaderCalled = false;
    m_adapter->ExecuteCommandL(EPhoneViewCreateCallHeader, callId);

    QVERIFY (m_createCallHeaderCalled == true);
    QCOMPARE (m_createCallHeaderCallId, callId);

}

void TestPhoneUIQtViewAdapter::testEPhoneViewCreateCallHeaderCommand2 ()
{
    m_isVoiceCall = false;
    m_setActiveCallCalled = false;
    m_adapter->ExecuteCommandL (EPhoneViewCreateCallHeader, 2 );

    QVERIFY (m_setActiveCallCalled == false);
    
    m_isVoiceCall = true;
    m_adapter->ExecuteCommandL (EPhoneViewCreateCallHeader, 2 );
    QVERIFY (m_setActiveCallCalled == true);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewCreateEmergencyCallHeader ()
{
    int callId = 9;
    m_createEmergencyCallHeaderCalled = false;
    m_adapter->ExecuteCommandL (EPhoneViewCreateEmergencyCallHeader, callId);

    QVERIFY(m_createEmergencyCallHeaderCalled == true);
    QCOMPARE(callId, m_createCallHeaderCallId);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewUpdateBubble()
{
    int callId = 4;
    m_adapter->ExecuteCommandL(EPhoneViewUpdateBubble, callId);

    QCOMPARE (callId, m_updateCallHeaderCallId);

}

void TestPhoneUIQtViewAdapter::testCallIdByState ()
{
    TPhoneCmdParamCallStateData data;
    data.SetCallState (EPEStateConnected);
    m_adapter->HandleCommandL (EPhoneViewGetCallIdByState, &data);
    QCOMPARE (data.CallId(), 0);
}

void TestPhoneUIQtViewAdapter::testSetToolbarButtons ()
{
    m_toolbarActionCount = 0;
    TPhoneCmdParamInteger intParam;
    intParam.SetInteger (R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA);
    m_adapter->ExecuteCommand (EPhoneViewUpdateCba, &intParam);
    QCOMPARE(m_toolbarActionCount,2);

    m_toolbarActionCount = 0;
    intParam.SetInteger (R_PHONEUI_DIALER_CBA);
    m_adapter->ExecuteCommand (EPhoneViewUpdateCba, &intParam);
    QCOMPARE(m_toolbarActionCount,4);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewHideNaviPaneAudioVolume ()
{
    m_adapter->ExecuteCommandL (EPhoneViewHideNaviPaneAudioVolume);
    QCOMPARE (m_removeVolumeSliderCalled, true);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewSetNaviPaneAudioVolume ()
{
    TPhoneCmdParamInteger intParam;
    intParam.SetInteger (3);
    m_adapter->ExecuteCommandL (EPhoneViewSetNaviPaneAudioVolume, &intParam);
    QCOMPARE (m_setVolumeSliderValueCalled, true);
    QCOMPARE (m_volumeSliderValue, 3);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewGetAudioVolumeLevel ()
{
    TPhoneCmdParamInteger intParam;
    m_adapter->ExecuteCommand (EPhoneViewGetAudioVolumeLevel, &intParam);
    QCOMPARE (m_volumeSliderValueCalled, true);
    QCOMPARE (intParam.Integer (), 5);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewActivateMuteUIChanges ()
{
    TPhoneCmdParamBoolean boolParam;
    boolParam.SetBoolean (true);
    m_adapter->ExecuteCommandL (EPhoneViewActivateMuteUIChanges, &boolParam);
    QCOMPARE (m_setPhoneMutedCalled, true);
}

void TestPhoneUIQtViewAdapter::testEPhoneViewGetCountOfActiveCallss ()
{
    TPhoneCmdParamInteger intParam;
    m_adapter->ExecuteCommandL (EPhoneViewGetCountOfActiveCalls, &intParam);
    QVERIFY (m_activeCallCountCalled == true);
}

void TestPhoneUIQtViewAdapter::testExecuteCommandLwithCmdId ()
{
    // Default
    m_adapter->ExecuteCommandL (0);    
}

void TestPhoneUIQtViewAdapter::testExecuteCommandLwithCmdIdAndCallId ()
{
    // Default
    m_adapter->ExecuteCommandL (0, 0);

    int callId = 5;
    m_adapter->ExecuteCommandL (EPhoneViewRemoveCallHeader, callId);
    QCOMPARE(m_removeCallHeaderCallId, callId);
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveFromConference, callId);   
    QCOMPARE(m_removeCallFromConferenceCallId, callId);
    
    m_adapter->ExecuteCommandL (EPhoneViewPrivateFromConference, callId);   
    QCOMPARE(m_setPrivateFromConferenceCallId, callId);
    
}

void TestPhoneUIQtViewAdapter::testExecuteCommandLwithCmdIdAndCmdParam ()
{
    TPhoneCmdParamBoolean boolParam;
    // Default
    m_adapter->ExecuteCommandL (0, &boolParam);
    
    
    // Is conference
    m_adapter->ExecuteCommandL (EPhoneViewGetIsConference, &boolParam);
    QCOMPARE ((bool)boolParam.Boolean (), false);
    
    
    // Get expand bubble call id
    TPhoneCmdParamInteger intParam;
    m_adapter->ExecuteCommand (EPhoneViewGetExpandedBubbleCallId, &intParam);
    QCOMPARE ((int)intParam.Integer(), -1);
    
    m_callStateMap.insert(0, EPEStateConnected);
    m_bubblesMap.insert(0, 0); 
    m_callStateMap.insert(1, EPEStateHeld);
    m_bubblesMap.insert(1, 1);  
    
    m_adapter->ExecuteCommand (EPhoneViewGetExpandedBubbleCallId, &intParam);
    QCOMPARE ((int)intParam.Integer(), 1);
    
    m_callStateMap.clear();
    m_bubblesMap.clear();
}

void TestPhoneUIQtViewAdapter::testSetTouchButtons ()
{
    TPhoneCmdParamBoolean boolParam;
    boolParam.SetBoolean( false );
    
    
    
    //Connected call
    m_callStateMap.insert(0, EPEStateConnected);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);    
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    
    //Emergency
    boolParam.SetBoolean(true);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(false == m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    
    //Held and active
    boolParam.SetBoolean(false);
    m_callStateMap.insert(1, EPEStateHeld);
    m_bubblesMap.insert(1, 1);    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneInCallCmdEndThisActiveCall));
    clearBubbleCommands(0);
    clearBubbleCommands(1);

    //Connected call and waiting
    m_callStateMap.insert(0, EPEStateConnected);
    m_bubblesMap.insert(0, 0);
   
    boolParam.SetBoolean(false);
    m_callStateMap.insert(1, EPEStateRinging);
    m_bubblesMap.insert(1, 1);    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneCallComingCmdReject));
    clearBubbleCommands(0);
    clearBubbleCommands(1);

    //Held call and waiting
    m_callStateMap.insert(0, EPEStateHeld);
    m_bubblesMap.insert(0, 0);
   
    boolParam.SetBoolean(false);
    m_callStateMap.insert(1, EPEStateRinging);
    m_bubblesMap.insert(1, 1);    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneCallComingCmdReject));
    clearBubbleCommands(0);
    clearBubbleCommands(1);    
    
    //Idle
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateIdle);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(0==m_actionMap.keys().size());
    clearBubbleCommands(0);
    
    //Conference Idle
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateConferenceIdle);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(0==m_actionMap.keys().size());
    clearBubbleCommands(0);
    
    //Disconnecting
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateDisconnecting);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(0==m_actionMap.keys().size());
    clearBubbleCommands(0);
    
    //Dialling
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateDialing);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(2==m_actionMap.value(0)->size());
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisOutgoingCall));
    clearBubbleCommands(0);
    
    //Ringing
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateRinging);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(2==m_actionMap.value(0)->size());
    QVERIFY(m_actionMap.value(0)->contains(EPhoneCallComingCmdAnswer));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneCallComingCmdReject));
    clearBubbleCommands(0);
    
    //Connecting
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateConnecting);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(2==m_actionMap.value(0)->size());
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisOutgoingCall));
    clearBubbleCommands(0);

    //Connected Conference
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateConnectedConference);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(2==m_actionMap.value(0)->size());
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));

    // Conference and held
    m_callStateMap.insert(1, EPEStateHeld);
    m_bubblesMap.insert(1, 1);    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneInCallCmdMute));
    QVERIFY(m_actionMap.value(1)->contains(EPhoneInCallCmdEndThisActiveCall));
    clearBubbleCommands(0);
    clearBubbleCommands(1);
    
    //Held conference
    m_conferenceCallList.append(1); //for coverage
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateHeldConference);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(2==m_actionMap.value(0)->size());
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdEndThisActiveCall));
    QVERIFY(m_actionMap.value(0)->contains(EPhoneInCallCmdMute));
    clearBubbleCommands(0);
    m_conferenceCallList.clear();
    
    //Unknown state
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateUnknown);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(0==m_actionMap.keys().size());
    clearBubbleCommands(0);
    
    //Default case
    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, 100);
    m_bubblesMap.insert(0, 0);
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(0==m_actionMap.keys().size());
    clearBubbleCommands(0);
}

void TestPhoneUIQtViewAdapter::testCreateConferenceBubble ()
{
    int callId = 8;
    
    m_adapter->ExecuteCommandL(EPhoneViewCreateConference, callId);    
    QCOMPARE(m_createConferenceBubbleCallId, callId);
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveConferenceBubble);
    QVERIFY(m_removeConferenceBubbleCalled);

}

void TestPhoneUIQtViewAdapter::testHandleCommandL ()
{
    QVERIFY( EPhoneViewResponseFailed == m_adapter->HandleCommandL (
            EPhoneIsDTMFDialerVisible) );    
    QVERIFY( EPhoneViewResponseFailed == m_adapter->HandleCommandL (
            EPhoneViewGetNumberEntryIsVisibleStatus) ); 
    /*QVERIFY( EPhoneViewResponseFailed == m_adapter->HandleCommandL (
            EPhoneViewGetNumberEntryIsUsedStatus) );*/ 
    QVERIFY( EPhoneViewResponseFailed == m_adapter->HandleCommandL (
            EPhoneIsCustomizedDialerVisible) );
    
    //Default handling for coverage
    QVERIFY( EPhoneViewResponseSuccess == m_adapter->HandleCommandL (0) ); 
    TPhoneCmdParamBoolean boolean;
    QVERIFY( EPhoneViewResponseSuccess == m_adapter->HandleCommandL (0, &boolean) );
    
    m_adapter->ExecuteCommand(0);
    m_adapter->ExecuteCommand(0, &boolean);
    m_adapter->ExecuteCommandL(0, 0, &boolean);
    _LIT (KText, "test");
    TBufC<8> textBuf (KText);
    m_adapter->ExecuteCommandL(0, 0, textBuf);
    m_adapter->FetchContent();
    
}

void TestPhoneUIQtViewAdapter::testGetSelectedConferenceMember ()
{
    m_callStateMap.clear();
    m_bubblesMap.clear();

    TPhoneCmdParamInteger  integerParam;
    QVERIFY( EPhoneViewResponseFailed == m_adapter->HandleCommandL(
            EPhoneViewSelectedConfMember, &integerParam) );    
    
    m_callStateMap.insert(1, EPEStateConnected);
    m_bubblesMap.insert(1, 0);
    
    QVERIFY( EPhoneViewResponseSuccess == m_adapter->HandleCommandL(
            EPhoneViewSelectedConfMember, &integerParam) ); 
    
    QVERIFY( integerParam.Integer() == 1 );    
}

void TestPhoneUIQtViewAdapter::testSetAudioPath ()
{
    TPhoneCmdParamAudioOutput audioOutput;
    audioOutput.SetAudioOutput( EPELoudspeaker );
    m_adapter->ExecuteCommandL(EPhoneViewActivateAudioPathUIChanges, &audioOutput);
    QVERIFY( m_phoneButtonFlags == PhoneUIQtButtonsController::Ihf );
    
    audioOutput.SetAudioOutput( EPEWiredAudioAccessory );
    m_adapter->ExecuteCommandL(EPhoneViewActivateAudioPathUIChanges, &audioOutput);
    QVERIFY( m_phoneButtonFlags == PhoneUIQtButtonsController::Wired );
    
    audioOutput.SetAudioOutput( EPEBTAudioAccessory );
    m_adapter->ExecuteCommandL(EPhoneViewActivateAudioPathUIChanges, &audioOutput);
    QVERIFY( m_phoneButtonFlags == PhoneUIQtButtonsController::Btaa );

}

void TestPhoneUIQtViewAdapter::testMuteRingToneOnAnswer ()
{
    m_adapter->ExecuteCommandL(EPhoneViewMuteRingToneOnAnswer);
    QVERIFY( m_EPhoneViewMuteRingToneOnAnswer_called == true );
}

void TestPhoneUIQtViewAdapter::testStopRingTone ()
{
    m_adapter->ExecuteCommandL(EPhoneViewStopRingTone);
    QVERIFY( m_EPhoneViewStopRingTone_called == true );
}

void TestPhoneUIQtViewAdapter::testMuteRingTone ()
{
    m_adapter->ExecuteCommandL(EPhoneViewMuteRingTone);
    QVERIFY( m_EPhoneViewMuteRingTone_called == true );
}

void TestPhoneUIQtViewAdapter::testPlayRingTone ()
{
    TPhoneCmdParamRingTone ringToneParam;
    m_adapter->ExecuteCommandL(EPhoneViewPlayRingTone, &ringToneParam);
    QVERIFY( m_EPhoneViewPlayRingTone_called == true );
}

void TestPhoneUIQtViewAdapter::testShowGlobalNote ()
{
    TPhoneCmdParamGlobalNote param;
    m_adapter->ExecuteCommandL(EPhoneViewShowGlobalNote, &param);
    QVERIFY( m_showGlobalNoteCalled == true );
}

void TestPhoneUIQtViewAdapter::testSetExpandActions()
{
    TPhoneCmdParamBoolean boolParam;
    boolParam.SetBoolean( false );

    m_callStateMap.clear();
    m_bubblesMap.clear();
    m_callStateMap.insert(0, EPEStateConnected);
    m_callStateMap.insert(1, EPEStateHeld);
    m_bubblesMap.insert(0, 0);
    m_bubblesMap.insert(1, 1);
    m_expandAction.clear();
    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_expandAction.contains(0));        
    QVERIFY(m_expandAction[0]==EPhoneCmdUpdateUiControls);
    QVERIFY(m_expandAction.contains(1));        
    QVERIFY(m_expandAction[1]==EPhoneInCallCmdSwap);
} 

void TestPhoneUIQtViewAdapter::testShowDialpad ()
{
    m_adapter->ExecuteCommandL(EPhoneViewOpenDialer);
    QVERIFY( m_showDialpadCalled == true );
}

void TestPhoneUIQtViewAdapter::testDialpadVisibility ()
{
    m_showDialpadCalled = false;
    m_hideDialpadCalled = false;
    
    TPhoneCmdParamBoolean param;
    param.SetBoolean(ETrue);
    m_adapter->ExecuteCommandL(EPhoneViewSetNumberEntryVisible, &param);
    QVERIFY( m_showDialpadCalled == true );
    QVERIFY( m_hideDialpadCalled == false );
    
    m_showDialpadCalled = false;
    m_hideDialpadCalled = false;
    
    param.SetBoolean(EFalse);
    m_adapter->ExecuteCommandL(EPhoneViewSetNumberEntryVisible, &param);
    QVERIFY( m_showDialpadCalled == false );
    QVERIFY( m_hideDialpadCalled == true );
}

void TestPhoneUIQtViewAdapter::removeAllCallHeaders()
{   
    m_removeAllCallHeadersCalled = false;
    m_adapter->ExecuteCommandL(EPhoneViewRemoveAllCallHeaders);   
    QVERIFY(m_removeAllCallHeadersCalled);
}

void TestPhoneUIQtViewAdapter::testRemoveDialpad ()
{
    m_clearAndHideDialpadCalled = false;
    m_adapter->ExecuteCommandL(EPhoneViewRemoveNumberEntry);
    QVERIFY( m_clearAndHideDialpadCalled == true );   
    m_clearAndHideDialpadCalled = false;

    m_clearDialpadCalled = false;
    m_phoneVisibleReturnValue = true;
    m_adapter->ExecuteCommandL(EPhoneViewClearNumberEntryContent);
    QVERIFY( m_clearDialpadCalled == true );
    m_phoneVisibleReturnValue = false;
    m_clearDialpadCalled = false;
}

void TestPhoneUIQtViewAdapter::testGetDialpadStringLength ()
{
    TPhoneCmdParamInteger integerParam;

    m_adapter->ExecuteCommandL(EPhoneViewGetNumberEntryCount, &integerParam );
    QVERIFY( integerParam.Integer() == 7 );
    
}

void TestPhoneUIQtViewAdapter::testGetDialpadString ()
{
    HBufC *numberEntryContent = 0;
    
    TRAP_IGNORE( numberEntryContent = HBufC::NewL( KPhoneNumberEntryBufferSize ) );
    if (numberEntryContent) {    
        TPtr ptr( numberEntryContent->Des() );
        ptr.Zero();   
                       
        TPhoneCmdParamString stringParam;
        stringParam.SetString( &ptr );
        m_adapter->ExecuteCommand(
                EPhoneViewGetNumberFromEntry,
                &stringParam );  
    
        QVERIFY( 0 == numberEntryContent->Des().Compare(_L("1234567")) );
        
        TPtr ptr2( numberEntryContent->Des() );
        ptr2.Zero();
        TPhoneCmdParamString stringParam2;
        stringParam2.SetString( &ptr2 );
        m_adapter->ExecuteCommandL(
                EPhoneViewGetNumberFromEntry,
                &stringParam2 );  
    
        QVERIFY( 0 == numberEntryContent->Des().Compare(_L("1234567")) );
    }
}

void TestPhoneUIQtViewAdapter::testAddToConference()
{
    m_adapter->ExecuteCommandL(EPhoneViewAddToConference);
    QVERIFY( m_addToConferenceBubbleCalled == true );
    
}

void TestPhoneUIQtViewAdapter::testSetConferenceExpanded()
{
    m_adapter->ExecuteCommand(EPhoneSetConferenceExpanded);
    QVERIFY( m_ExpandConferenceCalled == true );
    
}

void TestPhoneUIQtViewAdapter::testSetMenuActions()
{
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = false;
    TPhoneCmdParamBoolean boolParam;
    boolParam.SetBoolean( false );
    
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam); 
    
    QVERIFY(m_setMenuActionsCalled);
    
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = true;
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_setMenuActionsCalled);
    
    m_dialpadText = QString("");
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = true;
    m_adapter->ExecuteCommand(EPhoneViewSetTouchPaneButtons, &boolParam);
    QVERIFY(m_setMenuActionsCalled);
    
    connect(this, SIGNAL(dialpadAboutToClose()), m_adapter, SLOT(dialpadClosed()));
    connect(this, SIGNAL(keyPressReleased(QKeyEvent*)), 
            m_adapter, SLOT(keyReleased(QKeyEvent*)));
    
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = true;
    QKeyEvent keyEvent(QEvent::MouseButtonPress, 0, Qt::NoModifier);
    
    emit keyPressReleased(&keyEvent);
    QVERIFY(m_setMenuActionsCalled);
    
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = false;
    emit keyPressReleased(&keyEvent);
    QVERIFY(false == m_setMenuActionsCalled);
    
    m_setMenuActionsCalled = false;
    m_isDialpadVisible = true;
    emit dialpadAboutToClose();
    QVERIFY(m_setMenuActionsCalled);
    
    m_setMenuActionsCalled = false;
    emit keyPressReleased(&keyEvent);
    QVERIFY(false == m_setMenuActionsCalled);
}

void TestPhoneUIQtViewAdapter::testNoteController()
{
    m_showQueryCalled = false;
    m_removeDtmfNoteCalled = false;
    m_removeNoteCalled = false;
    m_removeQueryCalled = false;
    m_showNoteCalled = false;
    m_removeGlobalWaitNoteCalled = false;
    
    QVERIFY( (0 != m_adapter->noteController()) );
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveDtmfNote);
    QVERIFY( m_removeDtmfNoteCalled == true );
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveNote);
    QVERIFY( m_removeNoteCalled == true );
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveQuery);
    QVERIFY( m_removeQueryCalled == true );
    
    TPhoneCmdParamGlobalNote param;
    m_adapter->ExecuteCommandL(EPhoneViewShowQuery, &param);
    QVERIFY( m_showQueryCalled == true );
    
    m_adapter->ExecuteCommandL(EPhoneViewShowNote, &param);
    QVERIFY( m_showNoteCalled == true );
    
    m_adapter->ExecuteCommandL(EPhoneViewRemoveGlobalNote);
    QVERIFY( m_removeGlobalWaitNoteCalled == true );
    
    m_removeGlobalWaitNoteCalled = false;
    m_adapter->ExecuteCommandL(EPhoneViewRemoveGlobalWaitNote);
    QVERIFY( m_removeGlobalWaitNoteCalled == true );
    
    m_showQueryCalled = false;
    m_removeDtmfNoteCalled = false;
    m_removeNoteCalled = false;
    m_removeQueryCalled = false;
    m_showNoteCalled = false;
    
    m_adapter->ExecuteCommandL(EPhoneViewRemovePhoneDialogs);
    QVERIFY( m_removeQueryCalled == true );
    QVERIFY( m_removeNoteCalled == true );
    
}

void TestPhoneUIQtViewAdapter::testHsToForegroundAfterCall()
{
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( ETrue );
    m_adapter->ExecuteCommand( EPhoneViewHsToForegroundAfterCall,
        &booleanParam );
}

void TestPhoneUIQtViewAdapter::testCipheringInfoChange()
{
	int callId = 2;
    m_adapter->ExecuteCommandL(EPhoneViewCipheringInfoChange, callId);   
    QCOMPARE(m_handleCipheringInfoChangeCallId, callId);
}

void TestPhoneUIQtViewAdapter::testSetHidden()
{
    m_adapter->ExecuteCommandL(EPhoneViewUpdateFSW);
}

void TestPhoneUIQtViewAdapter::testBeginEndUiUpdate()
{
    m_adapter->ExecuteCommand(EPhoneViewBeginUpdate);
    QVERIFY (m_startChangesCalled == true);
    
    m_adapter->ExecuteCommand(EPhoneViewEndUpdate);
    QVERIFY (m_endChangesCalled == true);
}

void TestPhoneUIQtViewAdapter::testIndicatorController()
{
    connect( this, SIGNAL(focusLost()), m_adapter, SLOT(onFocusLost()));
    connect( this, SIGNAL(focusGained()), m_adapter, SLOT(onFocusGained()));
    emit focusLost();
    QVERIFY( m_enableCallIndCalled );
    emit focusGained();
    QVERIFY( m_disableCallIndCalled );
}

void TestPhoneUIQtViewAdapter::testOpenLogs()
{
    m_ordinalPositionCalled = false;
    m_sendToBackgroundCalled = false;
    TPhoneCmdParamString param;
    m_adapter->ExecuteCommandL(EPhoneViewLaunchLogs, &param);
    QVERIFY(m_ordinalPositionCalled);
    QVERIFY(m_sendToBackgroundCalled);
}

void TestPhoneUIQtViewAdapter::testSetFlags()
{
    TPhoneCmdParamBoolean param;
    param.SetBoolean( ETrue );
    m_phoneButtonFlags = 0;
    
    m_adapter->ExecuteCommand(EPhoneViewSetBlueToothFlag,&param);
    m_adapter->ExecuteCommand(EPhoneViewSetIhfFlag,&param);
    m_adapter->ExecuteCommand(EPhoneViewSetMuteFlag,&param);
    m_adapter->ExecuteCommand(EPhoneViewSetBluetoothAvailableFlag,&param);
    m_adapter->ExecuteCommand(EPhoneViewSetSoftRejectDimmed,&param);

    QVERIFY(m_phoneButtonFlags & PhoneUIQtButtonsController::Ihf);
    QVERIFY(m_phoneButtonFlags & PhoneUIQtButtonsController::Mute);
    QVERIFY(m_phoneButtonFlags & PhoneUIQtButtonsController::Btaa);
    QVERIFY(m_phoneButtonFlags & PhoneUIQtButtonsController::BluetoothAvailable);
    QVERIFY(m_phoneButtonFlags & PhoneUIQtButtonsController::DisableSoftReject);
}

void TestPhoneUIQtViewAdapter::testCaptureEndKey()
{
    TPhoneCmdParamKeyCapture captureParam;
    captureParam.SetKeyCode( EKeyNo );
    m_captured = false;
    m_capturedKey = Qt::Key_0;
    
    m_adapter->ExecuteCommand(EPhoneViewStartCapturingKey,&captureParam);
    QVERIFY(m_captured);
    QVERIFY(m_capturedKey == Qt::Key_No);
   
    m_adapter->ExecuteCommand(EPhoneViewStopCapturingKey,&captureParam);
    QVERIFY(false == m_captured);
    QVERIFY(m_capturedKey == Qt::Key_No);
    
    m_capturedKey = Qt::Key_0;
    captureParam.SetKeyCode( EKeyYes );
    
    m_adapter->ExecuteCommand(EPhoneViewStartCapturingKey,&captureParam);
    QVERIFY(false == m_captured);
    QVERIFY(m_capturedKey == Qt::Key_0);
    
    m_adapter->ExecuteCommand(EPhoneViewStopCapturingKey,&captureParam);
    QVERIFY(false == m_captured);
    QVERIFY(m_capturedKey == Qt::Key_0);
}

PHONE_UITEST_MAIN(TestPhoneUIQtViewAdapter)
#include "unit_tests.moc"
