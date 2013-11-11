/*
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
 * Description:
 *
 */

#include "cpplugincommon.h"
#include "cppluginlogging.h"
#include <hbglobal.h>


bool Tools::voipSupported()
{
    DPRINT << "DUMMY WRAPPER";
    return true;
}

bool Tools::videoSupported()
{
    DPRINT << "DUMMY WRAPPER";
    return true;
}

/*!
  Tools::convertEtelMobileServiceCode.
*/
BasicServiceGroups Tools::convertEtelMobileServiceCode(int serviceCode)
{
    DPRINT << "DUMMY WRAPPER: DEFAULT code";
    return Unknown;
}

SettingsWrapper::SettingsWrapper(QObject *parent): 
    QObject(parent)
{
    DPRINT << "DUMMY WRAPPER";
}

SettingsWrapper::~SettingsWrapper()
{
    DPRINT << "DUMMY WRAPPER";
}

bool SettingsWrapper::showCallDuration()
{
    DPRINT << "DUMMY WRAPPER";
    return 0;
}

int SettingsWrapper::setShowCallDuration(bool value)
{
    DPRINT << "DUMMY WRAPPER: value:" << value;
    return 0;
}

void SettingsWrapper::readSoftRejectText(QString &text, bool &userDefined)
{
    DPRINT << "DUMMY WRAPPER: text:" << text << " ,userDefined:" << userDefined;
}

int SettingsWrapper::writeSoftRejectText(const QString &text, bool userDefined)
{
    DPRINT << "DUMMY WRAPPER: text:" << text << " ,userDefined:" << userDefined;
    return 0;
}

bool SettingsWrapper::numberGroupingSupported() const
{
    DPRINT << "DUMMY WRAPPER";
    return true;
}


bool SettingsWrapper::isFeatureCallWaitingDistiquishNotProvisionedEnabled()
{
    bool enabled = false;
    DPRINT << "DUMMY WRAPPER: enabled: " << enabled;
    return enabled;
}


bool SettingsWrapper::isPhoneOffline() const
{
    bool enabled = false;
    DPRINT << "DUMMY WRAPPER: enabled: " << enabled;
    return enabled;
}


bool SettingsWrapper::isOngoingCall() const
{
    bool ongoingCall = false;
    DPRINT << "DUMMY WRAPPER: ongoingCall: " << ongoingCall;
    return ongoingCall;
}
