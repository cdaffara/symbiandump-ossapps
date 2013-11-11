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
#include <hbaction.h>
#include <phoneappvoipcommands.hrh>
#include <xqphoneappcommands.h>
#include <mockservice.h>
#include "phonetestmain.h"
#include "phoneresourceadapter.h"
#include "phoneuiqtbuttonscontroller.h"
#include "phoneresourceids.h"
#include "phoneui.hrh"

class TestPhoneResourceAdapter : public QObject, MockService
{
    Q_OBJECT
public:
    TestPhoneResourceAdapter();
    virtual ~TestPhoneResourceAdapter();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testConvert ();
    void testConvertToString ();
    void testConvertToString2 ();
    void testDefaultToolbarResourceId();
    void testConvertCommandToString();
    void testConvertToHbActions();
    void testConvertToToolBarCommandList();
    void testBtToolBarCommandList();

private:
    void testIncallToolbar (int id); // helper function
    
private:
    PhoneResourceAdapter *m_resourceAdapter; // class under test
};

TestPhoneResourceAdapter::TestPhoneResourceAdapter ()
{
}

TestPhoneResourceAdapter::~TestPhoneResourceAdapter ()
{
}

void TestPhoneResourceAdapter::initTestCase ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
}

void TestPhoneResourceAdapter::cleanupTestCase ()
{
    delete m_resourceAdapter;
}

void TestPhoneResourceAdapter::init ()
{
    initialize();
}

void TestPhoneResourceAdapter::cleanup ()
{
    reset();
}

void TestPhoneResourceAdapter::testConvert ()
{
    // For coverage
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
    PhoneUIQtButtonsController* buttonsController = 
        m_resourceAdapter->buttonsController ();
    
    QMap<PhoneAction::ActionType, PhoneAction *> map;
    PhoneAction *action = map [PhoneAction::LeftButton];
    
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_incoming_call") );        
    map = m_resourceAdapter->convert(R_PHONEUI_MTCAL_INCOMING_CALL);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_disconnected") );
    map = m_resourceAdapter->convert (R_PHONEUI_INCALL_CALL_DISCONNECTED);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);
    
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_on_hold") );
    map = m_resourceAdapter->convert (R_PHONEUI_INCALL_CALL_HELD);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_waiting") );
    map = m_resourceAdapter->convert (R_PHONEUI_MULTC_WAITING);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("%:0%H%:1%T%:2%S%:3") );
    map = m_resourceAdapter->convert (R_PHONEUI_TIME_DURAT_LONG_WITH_ZERO);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);
    
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_calling") );
    map = m_resourceAdapter->convert (R_PHONEUI_OUT_GOING_CALL);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_private_number") );
    map = m_resourceAdapter->convert (R_PHONEUI_MTCAL_CLI_WITHHELD);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_unknown_number") );
    map = m_resourceAdapter->convert (R_PHONEUI_MTCAL_CLI_UNKNOWN);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_emergency_call") );
    map = m_resourceAdapter->convert (R_PHONEUI_EMERGENCY_CALL_HEADER);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);

    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_attempting") );
    map = m_resourceAdapter->convert (R_PHONEUI_ATTEMPTING_EMERGENCY_CALL_TEXT);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);
        
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_other_conference_call") );
    map = m_resourceAdapter->convert (R_PHONEUI_CONFERENCE_CALL);
    QVERIFY(verify());
    QCOMPARE (map.size (), 1);
    
    TBuf<10> buffer(_L("123"));
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_info_sending") );
    map = m_resourceAdapter->convert (R_PHONEUI_SENDING_DTMF_WAIT_NOTE_TEXT, &buffer);
    QVERIFY(verify());
    QCOMPARE (map.size(), 1);
    
    EXPECT( PhoneAction, setText).with ( QString("txt_phone_info_send_string") );
    map = m_resourceAdapter->convert(R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY_TEXT, &buffer);
    QVERIFY(verify());
    QCOMPARE(map.size(), 1);
    
    // TODO
    map = m_resourceAdapter->convert (R_PHONEUI_CALLHANDLING_INCALL_HANDSET_CBA);
    
    map = m_resourceAdapter->convert (0);
    // QCOMPARE (map.size (), 0);
}

void TestPhoneResourceAdapter::testConvertToString ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
        
    QString testString = m_resourceAdapter->convertToString(R_PHONE_ERROR_CALL_NOT_ALLOWED); 
    QVERIFY( false == testString.isEmpty() );
    
    QString testString2 = m_resourceAdapter->convertToString(R_ERROR_NOTE_NOT_ALLOWED); 
    QVERIFY( false == testString2.isEmpty() );
    
    QString testString3 = m_resourceAdapter->convertToString(R_NOTETEXT_INVALID_PHONENUMBER); 
    QVERIFY( false == testString3.isEmpty() );
    
    QString testString4 = m_resourceAdapter->convertToString(R_NOTETEXT_NO_ANSWER); 
    QVERIFY( false == testString4.isEmpty() );
    
    QString testString5 = m_resourceAdapter->convertToString(R_PHONE_ERROR_NUMBER_BUSY); 
    QVERIFY( false == testString5.isEmpty() );
    
    QString testString6 = m_resourceAdapter->convertToString(R_PHONE_ERROR_REQUEST_REJECTED); 
    QVERIFY( false == testString6.isEmpty() );
    
    QString testString7 = m_resourceAdapter->convertToString(R_PHONE_SS_NOTIFICATION_INCAL_INFO_HOLD_TEXT); 
    QVERIFY( false == testString7.isEmpty() );
    
    QString testString8 = m_resourceAdapter->convertToString(R_INCAL_INFO_HOLD_TEXT); 
    QVERIFY( false == testString8.isEmpty() );
    
    QString testString9 = m_resourceAdapter->convertToString(R_INCAL_REMOTE_ACTIVE_TEXT); 
    QVERIFY( false == testString9.isEmpty() );
    
    /*QString testString10 = m_resourceAdapter->convertToString(R_INCAL_INFO_CONF_HOLD_TEXT); 
    QVERIFY( false == testString10.isEmpty() );*/
    
    QString testString11 = m_resourceAdapter->convertToString(R_PHONE_INCALL_INFO_NO_NETWORK_SUPPORT); 
    QVERIFY( false == testString11.isEmpty() );
    
    QString testString12 = m_resourceAdapter->convertToString(R_PHONE_ERROR_EMERGENCY_CALLS_ONLY); 
    QVERIFY( false == testString12.isEmpty() );
    
    QString testString13 = m_resourceAdapter->convertToString(R_NOTETEXT_NUMBER_NOT_IN_USE); 
    QVERIFY( false == testString13.isEmpty() );
    
    QString testString14 = m_resourceAdapter->convertToString(R_NOTETEXT_NETWORK_BUSY); 
    QVERIFY( false == testString14.isEmpty() );
    
    QString testString15 = m_resourceAdapter->convertToString(R_NOTETEXT_ERROR_IN_CONNECTION); 
    QVERIFY( false == testString15.isEmpty() );
    
    QString testString16 = m_resourceAdapter->convertToString(R_PHONEUI_ERROR_EMERGENCY_ATTEMPT_FAILED); 
    QVERIFY( false == testString16.isEmpty() );   
    
    QString testString17 = m_resourceAdapter->convertToString(R_PHONE_SS_NOTIFICATION_INCAL_TRANSFERRED_TEXT); 
    QVERIFY( false == testString17.isEmpty() );    
 
    QString testString18 = m_resourceAdapter->convertToString(R_PHONE_ERROR_NO_SERVICE); 
    QVERIFY( false == testString18.isEmpty() ); 
    
    QString testString19 = m_resourceAdapter->convertToString(R_NOTETEXT_NUMBER_BARRED); 
    QVERIFY( false == testString19.isEmpty() ); 
    
    /*QString testString20 = m_resourceAdapter->convertToString(R_PHONEUI_NO_VIDEO_NETWORK); 
    QVERIFY( false == testString20.isEmpty() );*/ 
    
    QString testString21 = m_resourceAdapter->convertToString(R_PHONEUI_VIDEO_CALL_NOT_POSSIBLE); 
    QVERIFY( false == testString21.isEmpty() ); 
    
    QString testString22 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE127); 
    QVERIFY( false == testString22.isEmpty() ); 
    
    QString testString23 = m_resourceAdapter->convertToString(R_CALL_INFO_NOT_SUPPORTED); 
    QVERIFY( false == testString23.isEmpty() ); 
    
    QString testString24 = m_resourceAdapter->convertToString(R_CALL_INFO_SERVICE_NOT_AVAILABLE); 
    QVERIFY( false == testString24.isEmpty() ); 
    
    QString testString25 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE38); 
    QVERIFY( false == testString25.isEmpty() ); 
    
    QString testString26 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE55); 
    QVERIFY( false == testString26.isEmpty() ); 
    
    QString testString27 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE58); 
    QVERIFY( false == testString27.isEmpty() ); 
    
    QString testString28 = m_resourceAdapter->convertToString(R_NOTE_PHONE_OUT_OF_3G_COVERAGE); 
    QVERIFY( false == testString28.isEmpty() ); 
    
    QString testString29 = m_resourceAdapter->convertToString(R_NOTE_VIDEO_CALL_ONLY_POSSIBLE_UNDER_3G_COVERAGE); 
    QVERIFY( false == testString29.isEmpty() ); 
    
    QString testString30 = m_resourceAdapter->convertToString(R_NOTE_CALLED_NUMBER_HAS_BARRED_INCOMING_CALLS); 
    QVERIFY( false == testString30.isEmpty() ); 
    
    QString testString31 = m_resourceAdapter->convertToString(R_INCAL_REMOTE_CREATE_CONFERENCE_TEXT); 
    QVERIFY( false == testString31.isEmpty() ); 
    
    QString testString32 = m_resourceAdapter->convertToString(R_NOTETEXT_CALL_BARRINGS); 
    QVERIFY( false == testString32.isEmpty() ); 
    
    QString testString33 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE16); 
    QVERIFY( false == testString33.isEmpty() ); 
    
    QString testString34 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE18); 
    QVERIFY( false == testString34.isEmpty() ); 
    
    QString testString35 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE21); 
    QVERIFY( false == testString35.isEmpty() ); 
    
    QString testString36 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE22); 
    QVERIFY( false == testString36.isEmpty() ); 
    
    QString testString37 = m_resourceAdapter->convertToString(R_CALL_INFO_CAUSE_VALUE34); 
    QVERIFY( false == testString37.isEmpty() ); 
    
    QString testString38 = m_resourceAdapter->convertToString(R_PHONE_ERROR_CALL_NOT_ALLOWED_FDN); 
    QVERIFY( false == testString38.isEmpty() ); 
    
    QString testString39 = m_resourceAdapter->convertToString(R_NOTE_UNABLE_TO_MAKE_VIDEO_CALL_NOT_SUPPORTED_BY_OTHER_PHONE); 
    QVERIFY( false == testString39.isEmpty() ); 

    QString testString40 = m_resourceAdapter->convertToString(R_NOTETEXT_DIVERTING_INCOMING_CALL); 
    QVERIFY( false == testString40.isEmpty() ); 
        
    QString testString41 = m_resourceAdapter->convertToString(R_NOTETEXT_DIVERTING); 
    QVERIFY( false == testString41.isEmpty() ); 
    
    QString testString42 = m_resourceAdapter->convertToString(R_NOTETEXT_ALL_INCOMING_CALLS_DIVERTED); 
    QVERIFY( false == testString42.isEmpty() ); 
        
    QString testString43 = m_resourceAdapter->convertToString(R_PHONE_ERROR_CHECK_CLIR); 
    QVERIFY( false == testString43.isEmpty() ); 
    
    QString testString44 = m_resourceAdapter->convertToString(R_PHONE_SS_NOTIFICATION_CLIR_SUPPR_REJECT_TEXT); 
    QVERIFY( false == testString44.isEmpty() ); 
    
    QString testString45 = m_resourceAdapter->convertToString(R_NOTE_TEXT_CLIR_CHANGE); 
    QVERIFY( false == testString45.isEmpty() ); 
    
    QString testString46 = m_resourceAdapter->convertToString(R_PHONE_ERROR_CHECK_NETWORK_SERVICES); 
    QVERIFY( false == testString46.isEmpty() ); 
    
    QString testString47 = m_resourceAdapter->convertToString(R_PHONE_INCALL_INFO_VIDEO_CALL_NOT_ALLOWED_DURING_RESTORE); 
    QVERIFY( false == testString47.isEmpty() ); 
    
    QString testString99 = m_resourceAdapter->convertToString(0); 
    QCOMPARE( testString99, QString ("") );
}

void TestPhoneResourceAdapter::testConvertToString2 ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
        
    QString testString = m_resourceAdapter->convertToStringWithParam(
            R_PHONE_TEXT_COLP_CONNECTED, QString("123456"));
    
    QVERIFY( false == testString.isEmpty() );
       
    QString testString2 = m_resourceAdapter->convertToStringWithParam(0, QString()); 
    QCOMPARE( testString2, QString ("") );
}

void TestPhoneResourceAdapter::testIncallToolbar (int id)
{
    QMap<PhoneAction::ActionType, PhoneAction *> map;
    PhoneAction *action;
    map = m_resourceAdapter->convert (id);
    QCOMPARE (map.size (), 4);
    
    switch(id) {
    case R_PHONEUI_CALLHANDLING_INCALL_UNMUTE_CBA:
        action = map [PhoneAction::ToolbarButton1];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_dialer"));
        action = map [PhoneAction::ToolbarButton2];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_mic_unmute"));
        QCOMPARE (action->command (), (int)EPhoneInCallCmdUnmute);    
        action = map [PhoneAction::ToolbarButton3];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_speaker"));
        QCOMPARE (action->command (), (int)EPhoneInCallCmdActivateIhf);        
        break;
    default:    
        action = map [PhoneAction::ToolbarButton1];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_dialer"));
        action = map [PhoneAction::ToolbarButton2];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_mic_mute"));
        QCOMPARE (action->command (), (int)EPhoneInCallCmdMute);    
        action = map [PhoneAction::ToolbarButton3];
        QCOMPARE (action->icon (), HbIcon("qtg_mono_speaker"));
        QCOMPARE (action->command (), (int)EPhoneInCallCmdActivateIhf);
        break;
    }
}

void TestPhoneResourceAdapter::testDefaultToolbarResourceId()
{
    QVERIFY(m_resourceAdapter->defaultToolbarResourceId()==
            R_PHONEUI_INCALL_DIALER_CBA);
}

void TestPhoneResourceAdapter::testConvertCommandToString ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
    
    QString testString = m_resourceAdapter->convertCommandToString(
            EPhoneInCallCmdEndAllCalls);    
    QVERIFY( false == testString.isEmpty() );
    
    testString = m_resourceAdapter->convertCommandToString(
            EPhoneInCallCmdTransfer);    
    QVERIFY( false == testString.isEmpty() );
    
    testString = m_resourceAdapter->convertCommandToString(
            EPhoneCmdHandoverToWlan);    
    QVERIFY( false == testString.isEmpty() );
    
    testString = m_resourceAdapter->convertCommandToString(
            EPhoneCmdHandoverToGsm);    
    QVERIFY( false == testString.isEmpty() );
    
    testString = m_resourceAdapter->convertCommandToString(0); 
    QCOMPARE( testString, QString ("") );
}

void TestPhoneResourceAdapter::testConvertToHbActions()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance (this);
    
    QList<HbAction*> testList = m_resourceAdapter->convertToHbActions(R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY);    
    QCOMPARE( testList.count(), 2 );
    QCOMPARE( (int)EPhoneInCallCmdContinueSendingDtmfString, 
              testList.at(0)->data().toInt());
    QCOMPARE( (int)EPhoneInCallCmdCancelSendingDtmfString, 
              testList.at(1)->data().toInt());
    qDeleteAll(testList);
    testList.clear();
    
    testList = m_resourceAdapter->convertToHbActions(R_PHONEUI_SENDING_DTMF_WAIT_NOTE);    
    QCOMPARE( testList.count(), 1 );
    QCOMPARE( (int)EPhoneInCallCmdCancelSendingDtmfString, 
              testList.at(0)->data().toInt());
    qDeleteAll(testList);
    testList.clear();
    
    testList = m_resourceAdapter->convertToHbActions(0);    
    QCOMPARE( testList.count(), 0 );
}

void TestPhoneResourceAdapter::testConvertToToolBarCommandList ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance(this);
    PhoneUIQtButtonsController* buttonsController = 
        m_resourceAdapter->buttonsController();
    
    QList<PhoneAction::ToolBarItem> testList;
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneCallComingCmdSilent == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear();
    
    // Set soft reject dimmed
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,true);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneCallComingCmdSilent == testList.at(1).mCommandId);
    QVERIFY(false == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear();    
    
    // Set soft reject
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,false);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_REJECT_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdReject == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject dimmed
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,true);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_REJECT_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdReject == testList.at(1).mCommandId);
    QVERIFY(false == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,false);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_SOFT_REJECT_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneCallComingCmdSilent == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject dimmed
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,true);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCOMINGCALL_SOFT_REJECT_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneCallComingCmdSilent == testList.at(1).mCommandId);
    QVERIFY(false == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,false);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLWAITING_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdJoinToConference == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject dimmed
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,true);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLWAITING_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneCallComingCmdSoftReject == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdJoinToConference == testList.at(1).mCommandId);
    QVERIFY(false == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    testList.clear(); 
    
    // Set soft reject
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject,false);
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_EMERGENCY_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneInCallCmdActivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear(); 
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_DIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdActivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdHold == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdOpenContacts == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    QVERIFY(true == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdActivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdHold == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdOpenContacts == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    QVERIFY(true == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
    
    // Set flags hold and ihf
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_EMERGENCY_CBA);
    QCOMPARE(2,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(1).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    testList.clear(); 
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_INCALL_DIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdUnhold == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdOpenContacts == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    QVERIFY(true == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdUnhold == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdOpenContacts == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    QVERIFY(true == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
    
    // Set flag multi call
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::MultiCall,true);
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdSwap == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdCreateConference == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(true == testList.at(1).mEnabled);
    QVERIFY(true == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear();
    
    // Set flag outgoing and conference call
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::Outgoing,true);
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::Conference,true);
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCALL_UNMUTE_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdSwap == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdJoinToConference == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    QVERIFY(false == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA);
    QCOMPARE(4,testList.count());
    QVERIFY(PhoneInCallCmdDeactivateIhf == testList.at(0).mCommandId);
    QVERIFY(PhoneInCallCmdSwap == testList.at(1).mCommandId);
    QVERIFY(PhoneInCallCmdJoinToConference == testList.at(2).mCommandId);
    QVERIFY(PhoneInCallCmdOpenDialer == testList.at(3).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    QVERIFY(false == testList.at(1).mEnabled);
    QVERIFY(false == testList.at(2).mEnabled);
    QVERIFY(true == testList.at(3).mEnabled);
    testList.clear(); 
}

void TestPhoneResourceAdapter::testBtToolBarCommandList ()
{
    m_resourceAdapter = PhoneResourceAdapter::Instance(this);
    PhoneUIQtButtonsController* buttonsController = 
        m_resourceAdapter->buttonsController();
    
    QList<PhoneAction::ToolBarItem> testList;
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::BluetoothAvailable,true);
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::IhfAsPushButton,false);
    
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCALL_BTAA_CBA);
    QVERIFY(0 < testList.count());
    QVERIFY(PhoneInCallCmdBtHandsfree == testList.at(0).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    testList.clear();
    
    buttonsController->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    testList = m_resourceAdapter->convertToToolBarCommandList(R_PHONEUI_CALLHANDLING_INCALL_BTAA_CBA);
    QVERIFY(0 < testList.count());
    QVERIFY(PhoneInCallCmdHandset == testList.at(0).mCommandId);
    QVERIFY(true == testList.at(0).mEnabled);
    testList.clear(); 
}

PHONE_TEST_MAIN(TestPhoneResourceAdapter)
#include "unit_tests.moc"
