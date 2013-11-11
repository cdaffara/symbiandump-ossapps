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

#ifndef RADIOSERVICES_H
#define RADIOSERVICES_H

#include <QLatin1String>

static const QLatin1String RADIO_SERVICE( "fmradio" );
static const QLatin1String RADIO_CONTROL_SERVICE( "com.nokia.symbian.IRadioControl" );
static const QLatin1String RADIO_CONTROL_SERVICE_OPERATION( "command(int)" );
static const QLatin1String RADIO_MONITOR_SERVICE( "com.nokia.symbian.IRadioMonitor" );
static const QLatin1String RADIO_MONITOR_SERVICE_OPERATION( "requestNotifications()" );
static const QLatin1String RADIO_MONITOR_SERVICE_REFRESH_OPERATION( "requestAllData()" );


const long KRadioPSUid = 0x101FF976;
const unsigned long KRadioStartupKey = 0x00000014;

const quint32 NOKIA_VENDORID = VID_DEFAULT;

namespace RadioServiceCommand
{
    enum CommandId
    {
        PowerOn,
        PowerOff,
        Previous,
        Next,
        PreviousFavorite,
        NextFavorite,
        SeekUp,
        SeekDown,
        Foreground,
        Background,
        Mute,
        UnMute
    };
}

namespace RadioServiceNotification
{
    enum Type {
        FavoriteCount = 1,
        LocalCount,
        Region,
        CurrentIsFavorite,
        RadioStatus,
        Frequency,
        Name,
        Genre,
        RadioText,
        DynamicPS
    };
}

namespace RadioStatus
{
    enum Status {
        UnSpecified,
        Playing,
        Muted,
        Seeking,
        NoAntenna,
        PoweringOff
    };
}

#endif // RADIOSERVICES_H
