/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Visual Radio central repository keys.
*
*/


#ifndef IRINTERNALCRKEYS_H
#define IRINTERNALCRKEYS_H

#include <e32cmn.h>
#include "iruid3.hrh"

/** The UID of the category we use. Must be the same as Internet Radio's UID3 (SID). */
const TUid KCRUidInternetRadio = { UID3_INTERNET_RADIO_10_1_EXE};

// Keys Definition
const TUint32 KIRFavoritesMaxNum                        = { 0x00000001 };
const TUint32 KIRPlayerVolume                           = { 0x00000002 };
const TUint32 KIRUpdateAvailablity                      = { 0x00000003 };
const TUint32 KIRHttpTimeout                            = { 0x00000004 };
const TUint32 KIRNewVersionNotificationInterval         = { 0x00000005 };
const TUint32 KIRNewVersionNotificationStartingTime     = { 0x00000006 };
const TUint32 KIRIsdsUrl                                = { 0x00000007 };
const TUint32 KIRIrId                                   = { 0x00000008 };
const TUint32 KIRIrAppVersion                           = { 0x00000009 };
const TUint32 KIRTimeOffset                             = { 0x0000000A };
const TUint32 KIRSessionLogIndex                        = { 0x0000000B };
const TUint32 KIRStartingViewId                         = { 0x0000000C };
const TUint32 KIRPreferredQuality                       = { 0x0000000D };
const TUint32 KIRSongHistoryFTU                         = { 0x0000000E };
const TUint32 KIRGlobalAdvEnabled                       = { 0x0000000F };
const TUint32 KIRManuallyInputStationUrl                = { 0x00000010 };
const TUint32 KIRManuallyInputStationName               = { 0x00000011 };
const TUint32 KIRMinDiskSpaceRequired                   = { 0x00000012 };
const TUint32 KIRSongRecognitionAppUid                  = { 0x00000013 };
const TUint32 KIRIdentifySongEnabled                    = { 0x00000014 };
const TUint32 KIRSearchText                             = { 0x00000015 };

const TUint32 KIRTermsAndConditionsFlag                 = { 0x00000016 };

#endif 
