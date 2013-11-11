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
* Description:  Wraps phone command extensions.
*/

#include "phonecommandextensionwrapper.h"
#include <QtDebug>
#include <pevirtualengine.h>
#include <hbaction.h>
#include <xqphoneappcommands.h>

bool m_setActions;
bool m_setInvalidCommand;
bool m_setInvalidButtonCommands;
bool m_setInvalidToolBarCommands;
bool m_setCustomToolBarCommands;
QList<HbAction*> m_menuActions;

  
PhoneCommandExtensionWrapper::PhoneCommandExtensionWrapper(int pluginUid) :
    m_pluginUid(pluginUid)
{

}

PhoneCommandExtensionWrapper::~PhoneCommandExtensionWrapper()
{

}

int PhoneCommandExtensionWrapper::pluginUid()
{
    return m_pluginUid;
}

void PhoneCommandExtensionWrapper::modifyMenuCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo, 
        QList<int> &menuCmdList)
{    
    if (m_setInvalidCommand) {
        menuCmdList.append(-1);
    }
}

void PhoneCommandExtensionWrapper::modifyPushButtonCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo, 
        QList<int> &buttonCmdList)
{    
    if (m_setInvalidButtonCommands) {
        buttonCmdList.clear();
        buttonCmdList.append(-1);
        buttonCmdList.append(-2);
    }
}

void PhoneCommandExtensionWrapper::modifyToolBarCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList)
{
    if (m_setInvalidToolBarCommands) {
        XQTelUiCommandExtension::ToolBarCommand invalidCommand;
        invalidCommand.mCommandId = -1;
        invalidCommand.mIsEnabled = true;
        toolBarCmdList.replace(0,invalidCommand);
    } else if (m_setCustomToolBarCommands) {
        toolBarCmdList.clear();
        XQTelUiCommandExtension::ToolBarCommand command;
        command.mCommandId = PhoneInCallCmdJoinToConference;
        command.mIsEnabled = true;
        toolBarCmdList.append(command);
        command.mCommandId = PhoneInCallCmdUnhold;
        command.mIsEnabled = true;
        toolBarCmdList.append(command);
        command.mCommandId = PhoneInCallCmdEndOutgoingCall;
        command.mIsEnabled = true;
        toolBarCmdList.append(command);
        command.mCommandId = PhoneCallComingCmdSilent;
        command.mIsEnabled = true;
        toolBarCmdList.append(command);
    }
}

void PhoneCommandExtensionWrapper::addMenuActions(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<HbAction*> &menuActions)
{
    if (m_setActions) {
        HbAction *action = new HbAction;
        action->setText(QString("Test"));
        m_menuActions.append(action);
        menuActions.append(action);
    }
}

void PhoneCommandExtensionWrapper::releaseMenu()
{
    qDeleteAll(m_menuActions);
    m_menuActions.clear();
}

void PhoneCommandExtensionWrapper::release()
{

}

void PhoneCommandExtensionWrapper::getCallInfoList(
        QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QMap<int,int> callStates, 
        QMap<int,int> serviceIds,
        int expandedCall )
{
    for (int i=0;i<callStates.keys().count();++i) {
        XQTelUiCommandExtension::CallInfo info;
        info.mCallState = mapCallState(callStates.values().at(i));
        info.mServiceId = serviceIds.value(callStates.keys().at(i));
        info.mIsExpanded = (expandedCall == callStates.keys().at(i));
        callInfo.append(info);
    }
}

XQTelUiCommandExtension::CallState PhoneCommandExtensionWrapper::mapCallState(
        int callState )
{
    XQTelUiCommandExtension::CallState state(XQTelUiCommandExtension::None);
    
    switch( callState ) {
    case EPEStateDisconnecting:
        {
        state = XQTelUiCommandExtension::Disconnecting;
        }
        break;
    case EPEStateRinging:
        {
        state = XQTelUiCommandExtension::Incoming;
        }
        break;
    case EPEStateDialing:
    case EPEStateConnecting: 
        {
        state = XQTelUiCommandExtension::Outgoing;
        }
        break;
    case EPEStateConnected:
    case EPEStateConnectedConference:
        {
        state = XQTelUiCommandExtension::Active;
        }
        break;
    case EPEStateHeld:
    case EPEStateHeldConference:
        {
        state = XQTelUiCommandExtension::OnHold;
        }
        break;
    case EPEStateUnknown:
    case EPEStateIdle:
    case EPEStateConferenceIdle:   
        break;
    default:
        break;
    }
    
    return state;
}


