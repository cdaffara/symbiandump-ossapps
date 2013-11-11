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

#ifndef RADIOINTERNALCRKEYS_H
#define RADIOINTERNALCRKEYS_H

#include <e32cmn.h>

/** The UID of the category we use. Must be the same as FM Radio's UID3 ( SID ). */
const TUid KRadioCRUid = { 0x2002FF52 };


// APPLICATION SETTINGS

/** The base value for application setting keys. */
const TUint32 KRadioCRApplicationSettingsBase                   = { 0x100 };

/** The key associated with the http cache limit setting. */
const TUint32 KRadioCRUiFlags                                   = { KRadioCRApplicationSettingsBase + 0x0 };

/* The key associated with the application launch count setting */
const TUint32 KRadioCRLaunchCount                               = { KRadioCRApplicationSettingsBase + 0x1 };


// RADIO SETTINGS

/** The base value for radio setting keys. */
const TUint32 KRadioCRRadioSettingsBase                         = { 0x200 };

/** The key associated with the headset volume setting. */
const TUint32 KRadioCRHeadsetVolume                             = { KRadioCRRadioSettingsBase + 0x0 };

/** The key associated with the speaker volume setting. */
const TUint32 KRadioCRSpeakerVolume                             = { KRadioCRRadioSettingsBase + 0x1 };

/** The key associated with the output mode setting. */
const TUint32 KRadioCROutputMode                                = { KRadioCRRadioSettingsBase + 0x2 };

/** The key associated with the audio route setting. */
const TUint32 KRadioCRAudioRoute                                = { KRadioCRRadioSettingsBase + 0x3 };

/** The key associated with the headset volume mute setting. */
const TUint32 KRadioCRHeadsetMuteState                          = { KRadioCRRadioSettingsBase + 0x4 };

/** The key associated with the speaker volume mute setting. */
const TUint32 KRadioCRSpeakerMuteState                          = { KRadioCRRadioSettingsBase + 0x5 };

/** The key associated with the radio power state. */
const TUint32 KRadioCRRadioPowerState                           = { KRadioCRRadioSettingsBase + 0x6 };

/** The key associated with the tuned frequency setting.  */
const TUint32 KRadioCRTunedFrequency                            = { KRadioCRRadioSettingsBase + 0x7 };

/** The key associated with the default minimum volume level setting.  */
const TUint32 KRadioCRDefaultMinVolumeLevel                     = { KRadioCRRadioSettingsBase + 0x8 };

/** The key associated with the current region setting. */
const TUint32 KRadioCRCurrentRegion                             = { KRadioCRRadioSettingsBase + 0x9 };

/*** Region Japan allowed. */
const TUint32 KRadioCRRegionAllowedJapan                        = { KRadioCRRadioSettingsBase + 0xA };

/*** Default region. */
const TUint32 KRadioCRDefaultRegion                             = { KRadioCRRadioSettingsBase + 0xB };

/** The key associated with the network id setting. */
const TUint32 KRadioCRNetworkId                                 = { KRadioCRRadioSettingsBase + 0xC };

/** The key associated with the country code setting. */
const TUint32 KRadioCRCountryCode                               = { KRadioCRRadioSettingsBase + 0xD };

#endif // RADIOINTERNALCRKEYS_H
