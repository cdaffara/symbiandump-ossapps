/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Telephony Domain Publish and Subscribe keys.
*
*/


#ifndef VIDEOTELEPHONYDOMAINPSKEYS_H
#define VIDEOTELEPHONYDOMAINPSKEYS_H

#include <e32std.h>


// CONSTANTS

/**
* Media Configuration API for video telephony.
*/
const TUid KPSUidVTMediaConfiguration = {0x1020736A};

/**
* Preferred camera that the application should use as source.
* @type RProperty::EInt
*/
const TUint32 KPSPreferredCamera = 0x00000000;
enum EPSPreferredCamera
    {
    EPSPreferredCameraUnknown,
    EPSPreferredCameraInwards,
    EPSPreferredCameraOutwards
    };


#endif      // VIDEOTELEPHONYDOMAINPSKEYS_H

// End of file