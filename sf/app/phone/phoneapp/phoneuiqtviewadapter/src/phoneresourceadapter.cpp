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
* Description:  Converts Symbian resources to PhoneAction objects.
*/

#include "phoneresourceadapter.h"
#include "phoneresourceids.h"
#include "phoneui.hrh"
#include "phoneuiqtbuttonscontroller.h"
#include <phoneappvoipcommands.hrh>
#include <xqphoneappcommands.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtDebug>
#include <QVariant>
#include <hbaction.h>

PhoneResourceAdapter *pinstance = 0;// initialize pointer

PhoneResourceAdapter* PhoneResourceAdapter::Instance (QObject *parent) 
{
    if (pinstance == 0)  // is it the first call?
    {  
        pinstance = new PhoneResourceAdapter (parent); // create sole instance
    }
    return pinstance; // address of sole instance
}
  
PhoneResourceAdapter::PhoneResourceAdapter (QObject *parent) : QObject (parent)
{
    m_buttonsCtrl = new PhoneUIQtButtonsController();
    
}

PhoneResourceAdapter::~PhoneResourceAdapter ()
{
    delete m_buttonsCtrl;
}

PhoneUIQtButtonsController* PhoneResourceAdapter::buttonsController ()
{
    PhoneUIQtButtonsController* retValue = 0;
    
    if (m_buttonsCtrl) 
        {
        retValue = m_buttonsCtrl;
        }
    
    return retValue;
}

QMap<PhoneAction::ActionType, PhoneAction *> PhoneResourceAdapter::convert (int symbianResourceId, ...)
{
    QMap<PhoneAction::ActionType, PhoneAction *> translatedActions;
    
    switch (symbianResourceId) {
	
    case R_PHONEUI_MTCAL_INCOMING_CALL:
        {
        PhoneAction *text = new PhoneAction;
        text->setText(hbTrId("txt_phone_other_incoming_call"));
        translatedActions[PhoneAction::Text] = text;
        }
        break;
    case R_PHONEUI_VIDEO_CALL_INCOMING:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_incoming_video_call"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
        
    case R_PHONEUI_INCALL_CALL_DISCONNECTED:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_disconnected"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;      
        
    case R_PHONEUI_INCALL_CALL_HELD:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_on_hold"));
        translatedActions [PhoneAction::Text] = text;    
        break;
        }

    case R_PHONEUI_MULTC_WAITING:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_waiting"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
       
    case R_PHONEUI_TIME_DURAT_LONG_WITH_ZERO:
        {
        PhoneAction *text = new PhoneAction;
        // Symbian modifier used here: %:0%H%:1%T%:2%S%:3
        // This is because this text is used by Symbian localization
        // for formatting later on.
        text->setText ("%:0%H%:1%T%:2%S%:3");
        translatedActions [PhoneAction::Text] = text;
        }
        break;
        
    case R_PHONEUI_OUT_GOING_CALL:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_calling"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
    case R_PHONEUI_OUT_GOING_VIDEO_CALL:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_calling_video_call"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
        
    case R_PHONEUI_MTCAL_CLI_WITHHELD:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_private_number"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
    case R_PHONEUI_MTCAL_CLI_UNKNOWN:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_unknown_number"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;
    case R_PHONE_MTCAL_CLI_PAYPHONE:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("Payphone number"));
        translatedActions [PhoneAction::Text] = text;
        }
        break;

    case R_PHONEUI_EMERGENCY_CALL_HEADER:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_emergency_call"));
        translatedActions [PhoneAction::Text] = text;            
        }
        break;  
     
    case R_PHONEUI_ATTEMPTING_EMERGENCY_CALL_TEXT:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_attempting"));
        translatedActions [PhoneAction::Text] = text;            
        }
        break;

    case R_PHONEUI_CONFERENCE_CALL:
        {
        PhoneAction *text = new PhoneAction;
        text->setText (hbTrId("txt_phone_other_conference_call"));
        translatedActions [PhoneAction::Text] = text; 
        }
        break;
    case R_PHONEUI_SENDING_DTMF_WAIT_NOTE_TEXT:
        {
        PhoneAction *text = new PhoneAction;
        VA_LIST list;
        VA_START (list, symbianResourceId);
        const TDesC *string = (TDesC*)VA_ARG(list, TDesC*);
        text->setText (hbTrId("txt_phone_info_sending").arg(QString::fromUtf16(string->Ptr(), 
                string->Length())));
        translatedActions [PhoneAction::Text] = text;
        VA_END (list);
        }
        break;
    case R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY_TEXT:
        {
        PhoneAction *text = new PhoneAction;
        VA_LIST list;
        VA_START (list, symbianResourceId);
        const TDesC *string = (TDesC*)VA_ARG(list, TDesC*);
        text->setText (hbTrId("txt_phone_info_send_string").arg(QString::fromUtf16(string->Ptr(), 
                string->Length())));
        translatedActions [PhoneAction::Text] = text;
        VA_END (list);
        }
        break;
    default:
        break;
    }
    
    return translatedActions;
}

QString PhoneResourceAdapter::convertToString(
        int symbianResourceId, const QString &causeCode) const
{
    QString ret;
    
    switch (symbianResourceId) {
        case R_PHONE_ERROR_CALL_NOT_ALLOWED:
            {
            ret = hbTrId("txt_phone_dpopinfo_not_allowed").arg(causeCode);      
            }
            break;
            
        case R_ERROR_NOTE_NOT_ALLOWED:
            {
            ret =  hbTrId("txt_phone_dpopinfo_not_allowed").arg(causeCode);  
            }
            break;
            
        case R_NOTETEXT_INVALID_PHONENUMBER:
            {
            ret = hbTrId("txt_phone_info_invalid_phone_number").arg(causeCode); 
            }
            break;
            
        case R_NOTETEXT_NO_ANSWER:
            {
            ret = hbTrId("txt_phone_info_no_answer").arg(causeCode); 
            }
            break;
        
        case R_PHONE_ERROR_NUMBER_BUSY:
            {
            ret = hbTrId("txt_phone_info_number_busy").arg(causeCode);
            }
            break;
            
        case R_PHONE_ERROR_REQUEST_REJECTED:
            {
            ret = hbTrId("txt_phone_dpopinfo_request_rejected_l1").arg(causeCode); 
            }
            break;
                
        case R_PHONE_SS_NOTIFICATION_INCAL_INFO_HOLD_TEXT:
            {
            ret = hbTrId("txt_phone_dpopinfo_on_hold");
            }
            break;
            
        case R_INCAL_INFO_HOLD_TEXT:
            {
            ret = hbTrId("txt_phone_dpopinfo_on_hold");
            }
            break;
            
        case R_INCAL_REMOTE_ACTIVE_TEXT:
            {
            ret = hbTrId("txt_phone_info_connected");
            }
            break;
            
        case R_PHONE_INCALL_INFO_NO_NETWORK_SUPPORT:
            {
            ret = hbTrId("txt_phone_info_videocall_only_possible_under_3g").arg(causeCode);
            }
            break;
        case R_PHONE_ERROR_EMERGENCY_CALLS_ONLY:
            {
            ret = hbTrId("txt_phone_info_emergency_calls_only").arg(causeCode);         
            }
            break;
        case R_NOTETEXT_NUMBER_NOT_IN_USE:
            {
            ret = hbTrId("txt_phone_info_number_not_in_use").arg(causeCode);         
            }
            break;
        case R_NOTETEXT_NETWORK_BUSY:
            {
            ret = hbTrId("txt_phone_info_network_busy").arg(causeCode);         
            }
            break;
        case R_NOTETEXT_ERROR_IN_CONNECTION:
            {
            ret = hbTrId("txt_phone_info_error_in_connection").arg(causeCode);         
            }
            break;
        case R_PHONEUI_ERROR_EMERGENCY_ATTEMPT_FAILED:
            {
            ret = hbTrId("txt_phone_info_emergency_call_failed").arg(causeCode);         
            }
            break;
        case R_PHONE_SS_NOTIFICATION_INCAL_TRANSFERRED_TEXT:
            {
            ret = hbTrId("txt_phone_info_transferred");         
            }
            break;
        case R_PHONE_ERROR_NO_SERVICE:
            {
            ret = hbTrId("txt_phone_dpopinfo_no_network_coverage").arg(causeCode);         
            }
            break;
        case R_NOTETEXT_NUMBER_BARRED:
            {
            ret = hbTrId("txt_phone_info_number_barred").arg(causeCode);
            }
            break;
        case R_PHONEUI_VIDEO_CALL_NOT_POSSIBLE:
            {
            ret = hbTrId("txt_phone_info_video_call_didnt_succeed_to_called").arg(causeCode);
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE127:
        case R_CALL_INFO_NOT_SUPPORTED:
        case R_CALL_INFO_SERVICE_NOT_AVAILABLE:
        case R_CALL_INFO_CAUSE_VALUE38:
        case R_CALL_INFO_CAUSE_VALUE55:
        case R_CALL_INFO_CAUSE_VALUE58:
        case R_NOTE_UNABLE_TO_MAKE_VIDEO_CALL_NOT_SUPPORTED_BY_OTHER_PHONE:
            {
            ret = hbTrId("txt_phone_info_unable_to_make_video_call_not_supp").arg(causeCode);
            }
            break;
        case R_NOTE_PHONE_OUT_OF_3G_COVERAGE:
            {
            ret = hbTrId("txt_phone_info_phone_switched_off_or_out_of_3g").arg(causeCode);
            }
            break;
        case R_NOTE_VIDEO_CALL_ONLY_POSSIBLE_UNDER_3G_COVERAGE:
            {
            ret = hbTrId("txt_phone_info_videocall_only_possible_under_3g").arg(causeCode);
            }
            break;
        case R_NOTE_CALLED_NUMBER_HAS_BARRED_INCOMING_CALLS:
            {
            ret = hbTrId("txt_phone_info_called_number_has_barred_incoming").arg(causeCode);
            }
            break;
        case R_INCAL_REMOTE_CREATE_CONFERENCE_TEXT:
            {
            ret = hbTrId("txt_phone_info_conference_call_active");
            }
            break;
        case R_NOTETEXT_CALL_BARRINGS:
            {
            ret = hbTrId("txt_phone_dpopinfo_note_you_have_active_barrings");
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE16:
            {
            ret = hbTrId("txt_phone_info_no_answer").arg(causeCode);
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE18:
            {
            ret = hbTrId("txt_phone_info_no_answer").arg(causeCode);
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE21:
            {
            ret = hbTrId("txt_phone_info_call_rejected").arg(causeCode);
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE22:
            {
            ret = hbTrId("txt_phone_info_number_not_in_use").arg(causeCode);
            }
            break;
        case R_CALL_INFO_CAUSE_VALUE34:
            {
            ret = hbTrId("txt_phone_info_network_busy").arg(causeCode);
            }
            break;
        case R_PHONE_ERROR_CALL_NOT_ALLOWED_FDN:
            {
            ret = hbTrId("txt_phone_info_call_not_allowed_fixed_dialling").arg(causeCode);
            }
            break;
        case R_NOTETEXT_DIVERTING_INCOMING_CALL:
        case R_NOTETEXT_DIVERTING:
            {
            ret = hbTrId("txt_phone_dpopinfo_diverting");
            }
            break;
        case R_NOTETEXT_ALL_INCOMING_CALLS_DIVERTED:
            {
            ret = hbTrId("txt_phone_dpopinfo_note_all_incoming_calls_diver");
            }
            break;
        case R_PHONE_ERROR_CHECK_CLIR:
            {
            ret = hbTrId("txt_phone_info_check_own_number_sending").arg(causeCode);
            }
            break;
        case R_PHONE_SS_NOTIFICATION_CLIR_SUPPR_REJECT_TEXT:
            {
            ret = hbTrId("txt_phone_info_could_not_send_own_number").arg(causeCode);
            }
            break;
        case R_NOTE_TEXT_CLIR_CHANGE:
            {
            ret = hbTrId("txt_phone_info_activate_own_number_sending").arg(causeCode);
            }
            break;
        case R_PHONE_ERROR_CHECK_NETWORK_SERVICES:
            {
            ret = hbTrId("txt_phone_info_check_network_services").arg(causeCode);
            }
            break;
        case R_PHONE_INCALL_INFO_VIDEO_CALL_NOT_ALLOWED_DURING_RESTORE:
            {
            ret = hbTrId("txt_phone_info_video_call_not_allowed_during_resto").arg(causeCode);
            }
            break;
        case R_NOTETEXT_NUMBER_NOT_IN_CUG:
            {
            ret = hbTrId("txt_phone_info_number_not_in_closed_group").arg(causeCode);
            }
            break;
        default:
            break;
    }

    return ret;
}

QString PhoneResourceAdapter::convertToStringWithParam(
        int symbianResourceId, const QString &text) const
{
    QString ret;
    
    switch (symbianResourceId) {

        case R_PHONE_TEXT_COLP_CONNECTED:
            {
            ret = hbTrId("txt_phone_info_connected_to_1").arg(text);
            }
            break;
        default:
            break;
    }

    return ret;
}

int PhoneResourceAdapter::defaultToolbarResourceId() const
{
    return R_PHONEUI_INCALL_DIALER_CBA; //R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA;
}

QString PhoneResourceAdapter::convertCommandToString(
        int symbianCommandId ) const
{
    QString ret;
    
    switch(symbianCommandId) {
    case EPhoneInCallCmdEndAllCalls:
        ret = hbTrId("txt_phone_opt_end_all_calls");
        break;     
    case EPhoneInCallCmdTransfer:
        ret = hbTrId("txt_phone_opt_transfer");
        break;
    case EPhoneCmdHandoverToWlan:
        ret = hbTrId("Handover to WLAN");
        break;  
    case EPhoneCmdHandoverToGsm:
        ret = hbTrId("Handover to GSM");
        break;  
        
    // TODO
    default:
        break;
    }

    return ret;
}

QList<HbAction*> PhoneResourceAdapter::convertToHbActions(
        int symbianResourceId ) const
{
    QList<HbAction*> ret;
    
    switch(symbianResourceId) {
        case R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY:
            {
            HbAction *leftButton = new HbAction(hbTrId("txt_common_button_send"));
            leftButton->setData(QVariant(EPhoneInCallCmdContinueSendingDtmfString));
            ret.append(leftButton);
    
            HbAction *rightButton = new HbAction(hbTrId("txt_common_button_cancel"));
            rightButton->setData(QVariant(EPhoneInCallCmdCancelSendingDtmfString));
            ret.append(rightButton);
            }
            break;
        case R_PHONEUI_SENDING_DTMF_WAIT_NOTE:
            {     
            HbAction *button = new HbAction(hbTrId("txt_common_button_cancel"));
            button->setData(QVariant(EPhoneInCallCmdCancelSendingDtmfString));
            ret.append(button); 
            }
            break;
        default:
            break;
    }

    return ret;
}

QList<PhoneAction::ToolBarItem> PhoneResourceAdapter::convertToToolBarCommandList(
        int symbianResourceId ) const
{
    QList<PhoneAction::ToolBarItem> ret;
    
    switch(symbianResourceId) {
        case R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA:
            {
            bool showSoftReject = (false == m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::DisableSoftReject));
            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneCallComingCmdSoftReject,showSoftReject)); 
            ret.append(PhoneAction::ToolBarItem(PhoneCallComingCmdSilent,true));      
            }
            break;

        case R_PHONEUI_CALLHANDLING_INCOMINGCALL_REJECT_CBA:
            {
            bool showSoftReject = (false == m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::DisableSoftReject));
            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneCallComingCmdSoftReject,showSoftReject)); 
            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdReject,true));  
            }
            break;
            
        case R_PHONEUI_CALLHANDLING_INCOMINGCALL_SOFT_REJECT_CBA:
            {
            bool showSoftReject = (false == m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::DisableSoftReject));
            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneCallComingCmdSoftReject,showSoftReject));            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneCallComingCmdSilent,false)); 
            }
            break;
            
        case R_PHONEUI_CALLHANDLING_CALLWAITING_CBA:        
            {
            bool showSoftReject = (false == m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::DisableSoftReject));
            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneCallComingCmdSoftReject,showSoftReject));            
            ret.append(PhoneAction::ToolBarItem(
                    PhoneInCallCmdJoinToConference,false));        
            }
            break;        
            
        case R_PHONEUI_DIALER_CBA:
        case R_PHONEUI_INCALL_DIALER_CBA:
        case R_PHONEUI_CALLHANDLING_INCALL_UNMUTE_CBA:
        case R_PHONEUI_CALLHANDLING_INCALL_HANDSET_CBA:
        case R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA:
        case R_PHONEUI_CALLHANDLING_INCALL_BTAA_CBA:
            {
            if (m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::IhfAsPushButton)) {
                if (m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::Mute)) {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdUnmute,true));
                } else {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdMute,true));
                }
            } else {
                if (m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::BluetoothAvailable)) {
                    if (m_buttonsCtrl->getButtonFlags(
                            PhoneUIQtButtonsController::Btaa)) {
                        ret.append(PhoneAction::ToolBarItem(
                                PhoneInCallCmdHandset,true));
                    } else {
                        ret.append(PhoneAction::ToolBarItem(
                                PhoneInCallCmdBtHandsfree,true));
                    }               
                } else {
                    if (!m_buttonsCtrl->getButtonFlags(
                            PhoneUIQtButtonsController::Ihf)) {
                        ret.append(PhoneAction::ToolBarItem(
                                PhoneInCallCmdActivateIhf,true));
                    } else {
                        ret.append(PhoneAction::ToolBarItem(
                                PhoneInCallCmdDeactivateIhf,true));
                    }
                }
            }
            
            if (!m_buttonsCtrl->getButtonFlags(
                    PhoneUIQtButtonsController::MultiCall)) {
                bool outgoing(m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::Outgoing));
                if (m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::Hold)) {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdUnhold,!outgoing));
                } else {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdHold,!outgoing));
                }
                         
                ret.append(PhoneAction::ToolBarItem(
                        PhoneInCallCmdOpenContacts,true));
            } else {
                bool outgoing(m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::Outgoing));
                bool disableJoin(outgoing ? true : 
                    m_buttonsCtrl->getButtonFlags(
                            PhoneUIQtButtonsController::DisableJoin));
                
                ret.append(PhoneAction::ToolBarItem(
                        PhoneInCallCmdSwap,!outgoing));
                
                if (m_buttonsCtrl->getButtonFlags(
                        PhoneUIQtButtonsController::Conference)) {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdJoinToConference,!disableJoin));
                } else {
                    ret.append(PhoneAction::ToolBarItem(
                            PhoneInCallCmdCreateConference,!disableJoin));
                }
            }
            
            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdOpenDialer,true)); 
            }
            break;
            
        case R_PHONEUI_CALLHANDLING_EMERGENCY_CBA:
            {
            
            if (!m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf))
                {
                ret.append(PhoneAction::ToolBarItem(
                        PhoneInCallCmdActivateIhf,true));
                }
            else
                {
                ret.append(PhoneAction::ToolBarItem(
                        PhoneInCallCmdDeactivateIhf,true));
                }                 
            }
			
            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdOpenDialer,true));
            break;
            
        /*case R_PHONEUI_CALLHANDLING_INCALL_HANDSET_CBA:
            {
            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdOpenDialer,true));

            if (m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold))
                {
                ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdUnhold,true));
                }
            else
                {
                ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdHold,true));
                }
            if (!m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold))
                {
                ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdMute,true));
                }
            else
                {
                ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdUnmute,true));
                }

            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdDeactivateIhf,true));
            ret.append(PhoneAction::ToolBarItem(PhoneInCallCmdOpenContacts,true));
            }
            break;*/
        default:
            break;
    }

    return ret;
}


