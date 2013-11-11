/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles phone ui commands.
*/

#include "phoneuicommandcontroller.h"
#include "phoneresourceadapter.h"
#include "phoneuiqtbuttonscontroller.h"
#include "phoneconstants.h"
#include "qtphonelog.h"
#include <hbaction.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <xqphoneappcommands.h>
#include <phoneappcommands.hrh>
#include <phoneappvoipcommands.hrh>
#include <pevirtualengine.h>
#include <xqteluicommandextension.h>

  
PhoneUiCommandController::PhoneUiCommandController(
        PhoneUIQtViewIF &view, QObject *parent) 
: QObject (parent), m_view(view), m_lastCommandExtension(0)
{
    PHONE_DEBUG("PhoneMenuController::PhoneMenuController");
}

PhoneUiCommandController::~PhoneUiCommandController()
{
    PHONE_DEBUG("PhoneMenuController::~PhoneMenuController");
    for (int i=0;i<m_commandExtensions.count();++i) {
        m_commandExtensions.at(i)->release();
    }
}

void PhoneUiCommandController::setCallMenuActions(
        QMap<int,int> callStates, QMap<int,int> serviceIds, 
        int serviceId, int expandedCallId )
{
    PHONE_TRACE
    releaseMenu();
    
    PhoneCommandExtensionWrapper *extension = commandExtension(serviceId);
    QList<int> menuCmdList = menuCommands(callStates, serviceIds);
    
    if (extension) {
        QList<XQTelUiCommandExtension::CallInfo> callInfo;
        extension->getCallInfoList(
                callInfo,callStates,serviceIds,expandedCallId);
        
        //Modify command list by extension
        extension->modifyMenuCommandList(callInfo,menuCmdList);
        addMenuItems(menuCmdList);
        
        extension->addMenuActions(callInfo,m_customMenuActions);        
        for (int i=0;i<m_customMenuActions.count();++i) {
            m_view.menuReference().addAction(m_customMenuActions.at(i));
        }
    } else {
        addMenuItems(menuCmdList);
    }
        
    
}

void PhoneUiCommandController::setDialpadMenuActions()
{
    PHONE_DEBUG("PhoneMenuController::setDialpadMenuActions");
    releaseMenu();
    QList<PhoneAction*> values;
    m_view.setMenuActions(values);
    qDeleteAll(values);
}

QMap<PhoneAction::ActionType, PhoneAction *> 
    PhoneUiCommandController::pushButtonActionsForCall( 
        int callState,
        bool emergencyCall,
        QMap<int,int> callStates,
        QMap<int,int> serviceIds,
        int serviceId,
        int callId)
{
    PHONE_TRACE
    QMap<PhoneAction::ActionType, PhoneAction *> translatedActions;
    QList<int> commands = buttonCommandList(
            callState, emergencyCall, callStates.values());

    PhoneCommandExtensionWrapper *extension = commandExtension(serviceId);
    
    if (extension) {
        QList<XQTelUiCommandExtension::CallInfo> callInfo;
        extension->getCallInfoList(
                callInfo,callStates,serviceIds,callId);
        
        //Modify command list by extension
        extension->modifyPushButtonCommandList(callInfo,commands);
    }
    
    if (1 == commands.count()) {
        PhoneAction *action = mapCommandToAction(commands.at(0));
        if (action) {
            translatedActions[PhoneAction::LeftButton] = action;
        }
    } else if (1 < commands.count()){
        PhoneAction *action = mapCommandToAction(commands.at(0));
        PhoneAction *action2 = mapCommandToAction(commands.at(1));
        if (action) {
            translatedActions[PhoneAction::LeftButton] = action;
        }
        if (action2) {
            translatedActions[PhoneAction::RightButton] = action2;
        }
    }
    
    return translatedActions;
}

QList<PhoneAction *> PhoneUiCommandController::toolBarActions( 
        int resourceId,
        QMap<int,int> callStates,
        QMap<int,int> serviceIds,
        int serviceId,
        int callId)
{
    PHONE_TRACE
    QList<PhoneAction *> actions;
    
    //Set tool bar button flags
    setJoinFlag(callStates, serviceIds);
    setConferenceFlag(callStates.values());
    setHoldFlag(callStates.value(callId));
    setMultiCallFlag(callStates.values());
    setOutgoingFlag(callStates.values());
    
    //Get tool bar item list by resource id.
    QList<PhoneAction::ToolBarItem> commands = PhoneResourceAdapter::Instance()->
            convertToToolBarCommandList(resourceId);

    if (serviceId != -1) {
        PhoneCommandExtensionWrapper *extension = commandExtension(serviceId);
        
        if (extension) {
            QList<XQTelUiCommandExtension::CallInfo> callInfo;
            extension->getCallInfoList(
                    callInfo,callStates,serviceIds,callId);
            
            QList<XQTelUiCommandExtension::ToolBarCommand> toolBarCmdList;
            
            mapToExtensionToolBarItems(commands,toolBarCmdList);
            //Modify tool bar command list by extension
            extension->modifyToolBarCommandList(callInfo,toolBarCmdList);
            // Map tool bar item list back to the phone action tool bar item list.
            mapToPhoneActionToolBarItems(toolBarCmdList,commands);
        }
    }
    
    for ( int i=0; i < commands.count(); ++i) {
        PhoneAction *action = mapCommandToAction(
                commands.at(i).mCommandId, !commands.at(i).mEnabled);
        if (action) {
            actions.append(action);
        }
    }
    
    return actions;
}

QList<int> PhoneUiCommandController::menuCommands(
        QMap<int,int> callStates, QMap<int,int> serviceIds ) const
{
    PHONE_TRACE
    QList<int> commands;
    bool sameServices = areServicesSame(callStates,serviceIds);
    bool isEmergencyCall = emergencyCall(callStates);
    
    // No menu items if an emergency call ongoing.
    if (false == isEmergencyCall) {
        switch(callStates.values().count()) {
        case 1: {            
        }
        break;
        case 2: {
            if (!callStates.values().contains(EPEStateRinging) &&
                !callStates.values().contains(EPEStateDisconnecting) &&
                !callStates.values().contains(EPEStateDialing)) {
                commands.append(PhoneInCallCmdEndAllCalls);
            }
            if ((callStates.values().contains(EPEStateConnected) &&
                 callStates.values().contains(EPEStateHeld)) ||
                (callStates.values().contains(EPEStateConnecting) &&
                 callStates.values().contains(EPEStateHeld))) {
                if (sameServices) {
                    commands.append(PhoneInCallCmdTransfer);
                }
            }
        }
        break;
        case 3: {
            commands.append(PhoneInCallCmdEndAllCalls);
            
            if (callStates.values().contains(EPEStateConnected) &&
                callStates.values().contains(EPEStateHeld) &&
                callStates.values().contains(EPEStateRinging)) {
                if (sameServices) {
                    commands.append(PhoneInCallCmdTransfer);
                }
            }
        }
        break;
        default:
            break;
        }
    }
    
    return commands;
}

void PhoneUiCommandController::addMenuItems(QList<int> menuCmdList)
{
    QList<PhoneAction*> values;
    
    for (int i=0;i<menuCmdList.count();++i) {
        int command = mapCommand(menuCmdList.at(i));
        if (-1 != command) {
            PhoneAction* phoneAction = new PhoneAction();
            phoneAction->setCommand(command);
            phoneAction->setText(
                    PhoneResourceAdapter::Instance()->
                        convertCommandToString(command));
            
            values.append(phoneAction);
        }
    }
    
    m_view.setMenuActions(values);
    qDeleteAll(values);
}

int PhoneUiCommandController::mapCommand(int command) const
{
    int ret(-1);
    
    switch(command) {
    case PhoneInCallCmdEndAllCalls:
        ret = EPhoneInCallCmdEndAllCalls;
        break;     
    case PhoneInCallCmdTransfer:
        ret = EPhoneInCallCmdTransfer;
        break;
    case PhoneInCallCmdHandoverToWlan:
        ret = EPhoneCmdHandoverToWlan;
        break;
    case PhoneInCallCmdHandoverToGsm:
        ret = EPhoneCmdHandoverToGsm;
        break;
    default:
        break;
    }
    
    return ret;
}

PhoneCommandExtensionWrapper *PhoneUiCommandController::commandExtension(
        int serviceId)
{
    PhoneCommandExtensionWrapper *extension = 0;
    TRAP_IGNORE( extension = CommandExtensionL(serviceId) );
    return extension;
}

TUid PhoneUiCommandController::ResolveImplementationUidL( 
        TUint32 aServiceId, TServicePropertyName aPropertyName ) const
    {
    PHONE_TRACE
    TUid implUid = { 0 };
    CSPEntry* entry = CSPEntry::NewLC();

    CSPSettings* spSettings = CSPSettings::NewLC(); 
    
    User::LeaveIfError(
        spSettings->FindEntryL( aServiceId, *entry ) ); 

    CleanupStack::PopAndDestroy( spSettings ); 

    const CSPProperty* property = NULL;
    User::LeaveIfError( entry->GetProperty( property, aPropertyName ) );
    
    if ( property ) 
        {
        TInt temp(0);
        User::LeaveIfError( property->GetValue( temp ) );
        implUid.iUid = temp;
        }

    CleanupStack::PopAndDestroy( entry );
    return implUid;   
    }

PhoneCommandExtensionWrapper 
    *PhoneUiCommandController::CommandExtensionL( TUint aServiceId )
{
    PHONE_TRACE
    TUid pluginUid = ResolveImplementationUidL( aServiceId, 
        EPropertyCallMenuHandlerPluginId );
    
    int pluginInd(-1);
    PhoneCommandExtensionWrapper* wrapper = NULL;
    
    for (int i=0; i<m_commandExtensions.count();++i) {
        if (pluginUid.iUid==m_commandExtensions.at(i)->pluginUid()) {
            pluginInd = i;
            break;
        }
    }
    
    if ( -1 == pluginInd ) {
        wrapper = new PhoneCommandExtensionWrapper( pluginUid.iUid );
        if (wrapper) {
            m_commandExtensions.append( wrapper );
        }
    } else {
        wrapper = m_commandExtensions[pluginInd];
    }
    
    m_lastCommandExtension = wrapper;
    return wrapper;
}

void PhoneUiCommandController::releaseMenu()
{
    PHONE_TRACE
    for (int i=0;i<m_customMenuActions.count();i++) {
        m_view.menuReference().removeAction(m_customMenuActions.at(i));
    }
    m_customMenuActions.clear();
    
    if (m_lastCommandExtension) {
        m_lastCommandExtension->releaseMenu();
        m_lastCommandExtension = 0;
    }
}

bool PhoneUiCommandController::areServicesSame(
        QMap<int,int> callStates, QMap<int,int> serviceIds) const
{
    bool ret(true);
    int serviceId(-1);
    for(int i=0;i<callStates.count()&&ret;++i) {
        if (-1==serviceId) {
            serviceId = serviceIds.value(callStates.keys().at(i));
        } else if (serviceId != serviceIds.value(callStates.keys().at(i))) {
            ret = false;
        }
    }
    
    return ret;
}

void PhoneUiCommandController::setJoinFlag(
        QMap<int,int> callStates, QMap<int,int> serviceIds) const
{
    bool disable(PhoneResourceAdapter::Instance()->buttonsController()->
                    getButtonFlags(PhoneUIQtButtonsController::FullConference));
    
    if (!disable) {
        disable = !areServicesSame(callStates, serviceIds);
    }

    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::DisableJoin, disable);
}

void PhoneUiCommandController::setHoldFlag(int callState) const
{
    bool hold = (callState == EPEStateHeldConference ||
                 callState == EPEStateHeld );

    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Hold,hold);
}

void PhoneUiCommandController::setOutgoingFlag(
        QList<int> callStates) const
{
    bool outgoing(callStates.contains(EPEStateDialing) ||
                  callStates.contains(EPEStateConnecting));
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Outgoing, outgoing);
}

void PhoneUiCommandController::setConferenceFlag(
        QList<int> callStates) const
{
    bool conference(callStates.contains(EPEStateConnectedConference) ||
                    callStates.contains(EPEStateHeldConference));
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::Conference, conference);
}

void PhoneUiCommandController::setMultiCallFlag(
        QList<int> callStates) const
{
    bool multicall(false);
    
    if (1<callStates.count()) {  
        if (callStates.count() == 2 && callStates.contains(EPEStateRinging)) {
            multicall = false;
        } else {
            multicall = true;   
        }        
    }
    
    PhoneResourceAdapter::Instance()->buttonsController()->
            setButtonFlags(PhoneUIQtButtonsController::MultiCall, multicall);
}

void PhoneUiCommandController::mapToExtensionToolBarItems(
        const QList<PhoneAction::ToolBarItem> &sourceList, 
        QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList) const
{
    toolBarCmdList.clear();
    
    for (int i=0;i<sourceList.count();++i) {
        XQTelUiCommandExtension::ToolBarCommand command;
        command.mCommandId = sourceList.at(i).mCommandId;
        command.mIsEnabled = sourceList.at(i).mEnabled;
        toolBarCmdList.append(command);
    }
}

void PhoneUiCommandController::mapToPhoneActionToolBarItems( 
        const QList<XQTelUiCommandExtension::ToolBarCommand> &sourceList,
        QList<PhoneAction::ToolBarItem> &commandList) const
{
    commandList.clear();
    
    for (int i=0;i<sourceList.count();++i) {
        PhoneAction::ToolBarItem command(
                sourceList.at(i).mCommandId,
                sourceList.at(i).mIsEnabled);
        commandList.append(command);
    }
}

QList<int> PhoneUiCommandController::buttonCommandList(
        int callState,
        bool emergencyCall,
        QList<int> callStates) const
{
    PHONE_TRACE
    QList<int> ret;
    
    switch( callState ) {
    case EPEStateIdle:
    case EPEStateDisconnecting:
    case EPEStateConferenceIdle:    
    break;
    
    case EPEStateDialing:
    case EPEStateConnecting: {
        if (!emergencyCall) {
            if (PhoneResourceAdapter::Instance()->buttonsController()->
                   getButtonFlags(
                           PhoneUIQtButtonsController::IhfAsPushButton)) {
                if (PhoneResourceAdapter::Instance()->buttonsController()->getButtonFlags(
                        PhoneUIQtButtonsController::BluetoothAvailable)) {
                    if (PhoneResourceAdapter::Instance()->buttonsController()->getButtonFlags(
                            PhoneUIQtButtonsController::Btaa)) {
                        ret.append(PhoneInCallCmdHandset);
                    } else {
                        ret.append(PhoneInCallCmdBtHandsfree);
                    } 
                } else {
                    if (!PhoneResourceAdapter::Instance()->buttonsController()->
                            getButtonFlags(PhoneUIQtButtonsController::Ihf)) {
                        ret.append(PhoneInCallCmdActivateIhf);
                    } else {
                        ret.append(PhoneInCallCmdDeactivateIhf);
                    }           
                }
            } else {
                if (PhoneResourceAdapter::Instance()->buttonsController()->
                        getButtonFlags(PhoneUIQtButtonsController::Mute)) {
                    ret.append(PhoneInCallCmdUnmute);
                } else {
                    ret.append(PhoneInCallCmdMute);
                }
            }
        }
        
        ret.append(PhoneInCallCmdEndOutgoingCall);  
    }
    break;
    case EPEStateRinging: {
        if (callStates.size()<3){
            ret.append(PhoneInCallCmdAnswer);
            ret.append(PhoneInCallCmdReject); 
        } else {
            ret.append(PhoneInCallCmdReplace); 
        }
    }
    break;
    
    case EPEStateConnected:
    case EPEStateConnectedConference:     
    case EPEStateHeld:
    case EPEStateHeldConference: {
        if (!emergencyCall) {
            if (PhoneResourceAdapter::Instance()->buttonsController()->
                   getButtonFlags(
                           PhoneUIQtButtonsController::IhfAsPushButton)) {
                if (PhoneResourceAdapter::Instance()->buttonsController()->getButtonFlags(
                        PhoneUIQtButtonsController::BluetoothAvailable)) {
                    if (PhoneResourceAdapter::Instance()->buttonsController()->getButtonFlags(
                            PhoneUIQtButtonsController::Btaa)) {
                        ret.append(PhoneInCallCmdHandset);
                    } else {
                        ret.append(PhoneInCallCmdBtHandsfree);
                    } 
                } else {
                    if (!PhoneResourceAdapter::Instance()->buttonsController()->
                            getButtonFlags(PhoneUIQtButtonsController::Ihf)) {
                        ret.append(PhoneInCallCmdActivateIhf);
                    } else {
                        ret.append(PhoneInCallCmdDeactivateIhf);
                    }           
                }
            } else {
                if (PhoneResourceAdapter::Instance()->buttonsController()->
                        getButtonFlags(PhoneUIQtButtonsController::Mute)) {
                    ret.append(PhoneInCallCmdUnmute);
                } else {
                    ret.append(PhoneInCallCmdMute);
                }
            }
        }

        ret.append(PhoneInCallCmdEndActive);
    }
    break;
    
    case EPEStateUnknown:
    break;
    
    default:
    break;    
    }
    
    return ret;
}

PhoneAction *PhoneUiCommandController::mapCommandToAction(
        int command, bool disabled) const
{
    PhoneAction *action=0;
    
    switch( command ) {
    case PhoneInCallCmdJoinToConference: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_join_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdJoin);
    }
    break;

    case PhoneInCallCmdCreateConference: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_join_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdCreateConference);
    }
    break;
        
    case PhoneInCallCmdAnswer: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_call"));
        action->setDisabled(disabled);
        action->setCommand (EPhoneCallComingCmdAnswer);        
        action->setActionRole(PhoneAction::Accept);
    }
    break;
    
    case PhoneInCallCmdReject: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_reject_call"));
        action->setDisabled(disabled);
        action->setCommand (EPhoneCallComingCmdReject);
        action->setActionRole(PhoneAction::Decline);
    }
    break;
    
    case PhoneInCallCmdHold: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_hold_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdHold);
    }
    break;
    
    case PhoneInCallCmdUnhold: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdUnhold);
    }
    break;
    
    case PhoneInCallCmdEndActive: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_end_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdEndThisActiveCall);
        action->setActionRole(PhoneAction::Decline);
    }
    break;
    
    case PhoneInCallCmdEndOutgoingCall: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_end_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdEndThisOutgoingCall);
        action->setActionRole(PhoneAction::Decline);
    }
    break;

    case PhoneInCallCmdReplace: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_replace_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdReplace);
        action->setActionRole(PhoneAction::Accept);
    }
    break;
    
    case PhoneInCallCmdSwap: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_swap_call"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdSwap); 
    }
    break;
    case PhoneCallComingCmdSoftReject: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_send"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneCallComingCmdSoftReject); 
    }
    break;
    case PhoneCallComingCmdSilent: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_speaker_off"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneCallComingCmdSilent); 
    }
    break;
    case PhoneInCallCmdOpenDialer: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_dialer"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdDialer); 
    }
    break;
    case PhoneInCallCmdMute: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_mic_mute"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdMute); 
    }
    break;
    case PhoneInCallCmdUnmute: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_mic_unmute"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdUnmute); 
    }
    break;
    case PhoneInCallCmdActivateIhf: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_speaker"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdActivateIhf); 
    }
    break;
    case PhoneInCallCmdDeactivateIhf: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_mobile"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdDeactivateIhf); 
    }
    break;
    case PhoneInCallCmdOpenContacts: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_contacts"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdContacts); 
    }
    break;
    case PhoneInCallCmdBtHandsfree: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_bluetooth_headset"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdBtHandsfree); 
    }
    break;
    case PhoneInCallCmdHandset: {
        action = new PhoneAction;
        action->setIcon(HbIcon("qtg_mono_mobile"));
        action->setDisabled(disabled);
        action->setCommand(EPhoneInCallCmdHandset); 
    }
    break;
    default:
    break;
    }
            
    return action;
}

bool PhoneUiCommandController::emergencyCall(QMap<int,int> callStates) const
{   
    return callStates.keys().contains(KEmergencyCallId);
}

