/*
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
* Description:
*
*/

#ifndef SCREENSAVERDOMAINCRKEYS_H
#define SCREENSAVERDOMAINCRKEYS_H

#include <e32cmn.h>

/**
* Screensaver Settings repository UID.
*
* @publishedPartner
* @released
*/
const TUid KCRUidScreensaverSettings = {0x2002FFAB};


/*
* Swipe to open  
*
* Boolean value for swipe-to-open widget on active mode. 
* 
* Possible integer values:
*
* 0 = Swipe_to_open  not used
* 1 = Swipe_to_open exists on active mode
*
* Default value: 1
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverSwipeToOpen                   = 0x00000001;

/**
* Timeout from active mode to power save mode
* 
* Screensaver timeout from active mode to power save mode 
* Power save mode is clock display in OLED-devices, blank display in other devices
*
* Possible integer values:
*
* 5 - 60 Seconds
*
* Default value: 30
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverTimeToPowerSave           = 0x00000002;

/**
* User setting whether screensaver is shown in power save mode
* 
* Boolean value for screensaver on/off via control panel.
* Setting is visible only in OLED-devices, i.e. if FF_AVC_DISPLAY_OLED_LOWPOWER_SUPPORT is enabled 
* Non-OLED devices have allways blank display in power save mode
*
* Possible integer values:
*
* 0 = Screensaver is off
* 1 = Screensaver is on
*
* Default value: 1
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverStatus           = 0x00000003;

/**
* Type of display (for legacy reasons, prefer use of  FF_AVC_DISPLAY_OLED_LOWPOWER_SUPPORT)
*  
* 0 = Regular display type
* 1 = RegularAndOled
* 2 = OLED
* 
*
* Default value: 2
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverDisplayType     = 0x00000004;

/**
* Initial state of the screensaver
*  
* 1 = ViewTypeActive
* 2 = ViewTypePowerSave
* 
* Initial view type for startup 
*
* Default value: 2
*
* @publishedPartner
* @released
*/
const unsigned long int KScreensaverStartupView     = 0x00000005;


#endif  // SCREENSAVERDOMAINCRKEYS_H
            
// End of File
