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
#include <hbapplication.h>
#include <phoneappcommands.hrh>
#include <xqphoneappcommands.h>
#include "phoneuiqtviewif.h"
#include "pevirtualengine.h"
#include "bubblemanagerif.h"
#include "phonebubblewrapper.h"
#include "phoneuicommandcontroller.h"
#include "phoneresourceids.h"
#include "phoneresourceadapter.h"
#include "phoneuiqtbuttonscontroller.h"
#include "phonetestmain.h"

extern bool m_setActions;
extern bool m_setInvalidCommand;
extern QList<HbAction*> m_menuActions;
extern bool m_setInvalidButtonCommands;
extern bool m_setInvalidToolBarCommands;
extern bool m_setCustomToolBarCommands;
extern int m_phoneButtonFlags;


class TestPhoneUiCommandController : public QObject, public PhoneUIQtViewIF, public BubbleManagerIF
{
    Q_OBJECT
public:
    TestPhoneUiCommandController();
    virtual ~TestPhoneUiCommandController();

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
    void clearAndHideDialpad() { m_clearAndHideDialpadCalled = true;};
    void clearDialpad() {};
    void bringToForeground() {};
    void setMenuActions(const QList<PhoneAction*>& ) { m_setMenuActionsCalled = true;};
    void shutdownPhoneApp() {};
    void setBackButtonVisible(bool ) {};
    HbMenu &menuReference(){ HbMenu* menu = NULL; return *menu;};
    void captureKey(Qt::Key , bool ) {};
    
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
        Qt::TextElideMode clipDirection = Qt::ElideLeft) {};
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
    void addRowToConference (int bubbleId) {};
    void removeRowFromConference (int bubbleId) {};
    int conferenceRowCount () const { return 2; };
    void setSelectionInConference (int rowNumber) {};
    void setSelectionIdInConference (int bubbleId) {};
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

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup ();
    
private slots:
    void testSetCallMenuActions();
    void testSetDialpadMenuActions();
    void testPushButtonActionsForCall();
    void testToolBarActionsForCall();


private:
    PhoneUiCommandController *m_commandController; // class under test

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
    bool m_clearAndHideDialpadCalled;
    bool m_ExpandConferenceCalled;
    bool m_setMenuActionsCalled;
    bool m_isDialpadVisible;
    QString m_dialpadText;
};

TestPhoneUiCommandController::TestPhoneUiCommandController ()
{
}

TestPhoneUiCommandController::~TestPhoneUiCommandController ()
{
}

void TestPhoneUiCommandController::initTestCase ()
{
    m_commandController = new PhoneUiCommandController (*this);
}

void TestPhoneUiCommandController::cleanupTestCase ()
{
    delete m_commandController;
}

void TestPhoneUiCommandController::init ()
{
    m_startChangesCalled = false;
    m_endChangesCalled = false;
}

void TestPhoneUiCommandController::cleanup ()
{
}

BubbleManagerIF& TestPhoneUiCommandController::bubbleManager ()
{
    return *this;
}

void TestPhoneUiCommandController::addBubbleCommand (
        int bubbleId, const PhoneAction& action)
{

}

void TestPhoneUiCommandController::clearBubbleCommands (int bubbleId)
{

}

void TestPhoneUiCommandController::addParticipantListAction(
    int commandId,
    const QString& text, 
    const HbIcon& icon)
{

}

void TestPhoneUiCommandController::clearParticipantListActions()
{

}

void TestPhoneUiCommandController::testSetCallMenuActions()
{
    QMap<int,int> callStates;
    QMap<int,int> serviceIds;
    int serviceId(2);
    int expandedCallId(1);
    callStates[expandedCallId] = EPEStateConnected;
    serviceIds[expandedCallId] = serviceId;
    
    
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    
    m_setActions = true;
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    m_setActions = false;
    
    m_setInvalidCommand = true;
    callStates[expandedCallId] = EPEStateHeld;
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    m_setInvalidCommand = false;
    
    callStates[0] = EPEStateConnected;
    serviceIds[0] = serviceId;
    
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    
    callStates[0] = EPEStateRinging;
    
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    
    serviceIds[0] = 3;
    callStates[0] = EPEStateConnected;
    
    m_commandController->setCallMenuActions(callStates,serviceIds,3,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    
    callStates[2] = EPEStateRinging;
    serviceIds[2] = 3;
    
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
    
    callStates[3] = EPEStateDisconnecting;
    serviceIds[3] = 3;
    
    m_commandController->setCallMenuActions(callStates,serviceIds,serviceId,expandedCallId);
    QVERIFY(m_setMenuActionsCalled);
    m_setMenuActionsCalled = false;
}

void TestPhoneUiCommandController::testSetDialpadMenuActions()
{
    m_setMenuActionsCalled = false;
    m_commandController->setDialpadMenuActions();
    QVERIFY(m_setMenuActionsCalled);
}

void TestPhoneUiCommandController::testPushButtonActionsForCall()
{
    int callState = EPEStateConnected;
    bool emergencyCall = false;
    QMap<int,int> callStates;
    QMap<int,int> serviceIds;
    int serviceId(2);
    int callId(1);
    callStates[callId] = EPEStateConnected;
    serviceIds[callId] = serviceId;
    
    QMap<PhoneAction::ActionType, PhoneAction *> actions = 
        m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    
    qDeleteAll(actions.values());
    actions.clear();
    
    m_setInvalidButtonCommands = true;
    actions =  m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0==actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();  
    m_setInvalidButtonCommands = false;
    
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            true,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Ihf, true);
    
    callStates[callId] = EPEStateHeld;
    callState = EPEStateHeld;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateRinging;
    callState = EPEStateRinging;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateIdle;
    callState = EPEStateIdle;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0==actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();

    callStates[callId] = EPEStateDisconnecting;
    callState = EPEStateDisconnecting;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0==actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateConferenceIdle;
    callState = EPEStateConferenceIdle;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0==actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateUnknown;
    callState = EPEStateUnknown;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0==actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateDialing;
    callState = EPEStateDialing;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateConnecting;
    callState = EPEStateConnecting;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateConnectedConference;
    callState = EPEStateConnectedConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateHeldConference;
    callState = EPEStateHeldConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
 
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    //////////////////////////////////////////////////////////////////////////
    //2 calls
    //////////////////////////////////////////////////////////////////////////
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::BluetoothAvailable, true);
    
    callStates[2] = EPEStateHeldConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[2] = EPEStateHeldConference;
    callState = EPEStateHeldConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    serviceId = 3;
    serviceIds[callId] = serviceId;
    callStates[2] = EPEStateHeld;
    serviceIds[2] = serviceId;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            true,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[2] = EPEStateHeldConference;
    callState = EPEStateHeldConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[2] = EPEStateConnectedConference;
    callState = EPEStateHeld;
    callStates[callId] = EPEStateHeld;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callState = EPEStateConnectedConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    callStates[callId] = EPEStateConnected;
    callStates[2] = EPEStateHeldConference;
    callState = EPEStateConnected;
    
    callState = EPEStateConnectedConference;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Btaa, true);
    
    callStates[callId] = EPEStateConnected;
    callStates[2] = EPEStateHeldConference;
    callState = EPEStateHeldConference;
    
    //////////////////////////////////////////////////////////////////////////
    //3 calls
    //////////////////////////////////////////////////////////////////////////
    callStates[3] = EPEStateRinging;
    serviceIds[3] = serviceId;
    actions = m_commandController->pushButtonActionsForCall(
            callState,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    actions = m_commandController->pushButtonActionsForCall(
            EPEStateRinging,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            3);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
    actions = m_commandController->pushButtonActionsForCall(
            EPEStateHeld,
            emergencyCall,
            callStates,
            serviceIds,
            serviceId,
            2);
    
    QVERIFY(0<actions.values().count());
    qDeleteAll(actions.values());
    actions.clear();
    
}

void TestPhoneUiCommandController::testToolBarActionsForCall()
{
    m_phoneButtonFlags = 0;
    int callState = EPEStateConnected;
    QMap<int,int> callStates;
    QMap<int,int> serviceIds;
    int serviceId(2);
    int callId(1);
    callStates[callId] = EPEStateConnected;
    serviceIds[callId] = serviceId;
    m_setInvalidToolBarCommands = false;
    m_setCustomToolBarCommands = false;
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Btaa, false);
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::BluetoothAvailable, false);
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::IhfAsPushButton, false);
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Ihf, false);
    
    
    QList<PhoneAction *> actions = 
        m_commandController->toolBarActions(
            R_PHONEUI_DIALER_CBA,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(4==actions.count());
    QVERIFY(EPhoneInCallCmdActivateIhf == actions.at(0)->command());
    QVERIFY(EPhoneInCallCmdHold == actions.at(1)->command());
    QVERIFY(EPhoneInCallCmdContacts == actions.at(2)->command());
    QVERIFY(EPhoneInCallCmdDialer == actions.at(3)->command());
    
    qDeleteAll(actions);
    actions.clear();
    
    // Invalid resource id
    actions = m_commandController->toolBarActions(
            -1,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(0==actions.count());
    
    qDeleteAll(actions);
    actions.clear();
    
    // Service not found
    actions = m_commandController->toolBarActions(
            R_PHONEUI_DIALER_CBA,
            callStates,
            serviceIds,
            -1,
            -1);
    
    QVERIFY(4==actions.count());
    
    qDeleteAll(actions);
    actions.clear();
    
    // Invalid toolbar command by extension
    m_setInvalidToolBarCommands = true;
    
    actions = m_commandController->toolBarActions(
            R_PHONEUI_DIALER_CBA,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(3==actions.count());
    
    qDeleteAll(actions);
    actions.clear();
    
    
    // Command list modified by extension
    m_setInvalidToolBarCommands = false;
    m_setCustomToolBarCommands = true;

    actions = m_commandController->toolBarActions(
            R_PHONEUI_DIALER_CBA,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(4==actions.count());
    QVERIFY(EPhoneInCallCmdJoin == actions.at(0)->command());
    QVERIFY(EPhoneInCallCmdUnhold == actions.at(1)->command());
    QVERIFY(EPhoneInCallCmdEndThisOutgoingCall == actions.at(2)->command());
    QVERIFY(EPhoneCallComingCmdSilent == actions.at(3)->command());
    
    qDeleteAll(actions);
    actions.clear();
    
    m_setCustomToolBarCommands =false;
    
    // Multi call
    callStates[2] = EPEStateHeld;
    serviceIds[2] = 2;
    
    actions = m_commandController->toolBarActions(
            R_PHONEUI_INCALL_DIALER_CBA,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(4==actions.count());
    
    // No extension
    serviceIds[1] = 1;
    serviceIds[2] = 1;
    serviceId = 1;
    
    actions = m_commandController->toolBarActions(
            R_PHONEUI_INCALL_DIALER_CBA,
            callStates,
            serviceIds,
            serviceId,
            callId);
    
    QVERIFY(4==actions.count());
}

PHONE_TEST_MAIN(TestPhoneUiCommandController)
#include "unit_tests.moc"
