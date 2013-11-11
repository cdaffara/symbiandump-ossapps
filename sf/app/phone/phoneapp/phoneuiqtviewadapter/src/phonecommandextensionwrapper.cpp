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
#include "qtphonelog.h"
#include <xqplugin.h>
#include <xqpluginloader.h>
#include <pevirtualengine.h>


PhoneCommandExtensionWrapper::PhoneCommandExtensionWrapper(int pluginUid) :
    m_plugin(0),m_pluginUid(pluginUid)
{
    PHONE_DEBUG("PhoneCommandExtensionWrapper::PhoneMenuExtensionWrapper");
    XQPluginLoader pluginLoader;
    pluginLoader.setUid(m_pluginUid);

    QObject *plugin = pluginLoader.instance();

    if (plugin) {
        m_plugin = qobject_cast<XQTelUiCommandExtension*>(plugin);
    }
}

PhoneCommandExtensionWrapper::~PhoneCommandExtensionWrapper()
{
    PHONE_DEBUG("PhoneCommandExtensionWrapper::~PhoneCommandExtensionWrapper");
    release();
}

int PhoneCommandExtensionWrapper::pluginUid()
{
    return m_pluginUid;
}

void PhoneCommandExtensionWrapper::modifyMenuCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<int> &menuCmdList)
{
    if (m_plugin) {
        m_plugin->modifyMenuCommandList(callInfo, menuCmdList);
    }
}

void PhoneCommandExtensionWrapper::modifyPushButtonCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<int> &buttonCmdList)
{
    if (m_plugin) {
        m_plugin->modifyPushButtonCommandList(callInfo, buttonCmdList);
    }
}

void PhoneCommandExtensionWrapper::modifyToolBarCommandList(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<XQTelUiCommandExtension::ToolBarCommand> &toolBarCmdList)
{
    if (m_plugin) {
        m_plugin->modifyToolBarCommandList(callInfo, toolBarCmdList);
    }
}

void PhoneCommandExtensionWrapper::addMenuActions(
        const QList<XQTelUiCommandExtension::CallInfo> &callInfo,
        QList<HbAction*> &menuActions)
{
    if (m_plugin) {
        m_plugin->addMenuActions(callInfo,menuActions);
    }
}

void PhoneCommandExtensionWrapper::releaseMenu()
{
    if (m_plugin) {
        m_plugin->releaseMenu();
    }
}

void PhoneCommandExtensionWrapper::release()
{
    if (m_plugin) {
        m_plugin->release();
        m_plugin = 0;
    }
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
    case EPEStateDisconnecting: {
        state = XQTelUiCommandExtension::Disconnecting;
    }
    break;

    case EPEStateRinging: {
        state = XQTelUiCommandExtension::Incoming;
    }
    break;

    case EPEStateDialing:
    case EPEStateConnecting: {
        state = XQTelUiCommandExtension::Outgoing;
    }
    break;

    case EPEStateConnected:
    case EPEStateConnectedConference: {
        state = XQTelUiCommandExtension::Active;
    }
    break;

    case EPEStateHeld:
    case EPEStateHeldConference: {
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


