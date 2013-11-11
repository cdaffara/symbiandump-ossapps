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

#ifndef IRSERVICEDEF_H
#define IRSERVICEDEF_H

#include <QString>
#include "irquid3.hrh"


// For settings via QSetting
static const QString KIrSettingOrganization = "nokia.s60.mymex";
static const QString KIrSettingApplication  = "internetradio";
static const QString KIrSettingStationName  = "lastplayedinfo/station_name";
static const QString KIrDefaultStationName  = "no_station_played_yet";
static const QString KIrSettingStationLogo  = "lastplayedinfo/station_logo";
static const QString KIrSettingStationLogoAvailable  = "lastplayedinfo/station_logo_available";
 

// For P&S XQSettingsManager
const long int KInternetRadioPSUid = UID3_INTERNET_RADIO_10_1_EXE;   // Internet Radio's UID3
const unsigned long int KInternetRadioStartupKey = 0x00000035;  // a empty slot in internetradio2.0\include\irpubsubkeys.h


Q_ENUMS(IrServiceCmd::Type IrServiceNotification::Type IrAppState::Type IrServiceResult::Type)

namespace IrServiceCmd
{
    enum Type
    {
        Unknown                 = -1,
        LaunchNormally          = 0,
        LaunchNowPlayingView    = 1,
        Play                    = 2,
        Stop                    = 3,
        Cancel                  = 4,
        Foreground              = 5
    };
}

namespace IrServiceNotification
{
    enum Type
    {
        Unknown       = -1,
        IrState       = 0,
        StationLogo   = 1,
        StationName   = 2,
        MetaData      = 3
    };
}


namespace IrAppState
{
    enum Type
    {
        Unknown         = -1,
        NoRunInit       = 0,  // internet radio is not running, and no station has been played yet
        NoRunStopped    = 1,  // internet radio is not running, with last played station
        RunningInit     = 2,  // running, no station has been played yet
        RunningStopped  = 3,  // running, with last played station
        Playing         = 4,
        Loading         = 5
    };
}

namespace IrServiceResult
{
    enum Type
    {
        Success  = -1,
        Fail     = 0
    };
}

#endif // IRSERVICEDEF_H
