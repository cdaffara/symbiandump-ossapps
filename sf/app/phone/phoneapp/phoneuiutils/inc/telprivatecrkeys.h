/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Telephony private Central Repository keys
*
*/


#ifndef TELEPHONYPRIVATECRKEYS_H
#define TELEPHONYPRIVATECRKEYS_H

#include <e32std.h>

/******************************************************************************
* Telephony Private Variation API
* Keys under this category are used in defining telephony private variation.
******************************************************************************/
const TUid KCRUidTelPrivateVariation = {0x102828B2}; 
 
/**
* Set of applications by UID that are closed when end key is pressed. Can 
* be list of UIDs or UID ranges (<uid1>-<uid2>) or combination of both or 
* all apps (i.e. '*'). UIDs are hexadecimal e.g. "10005951". Elements are 
* separated with space. Supports also closing any app when memory is low.
* This is marked with "OOM" token.
*/
const TUint32 KTelAppsCloseList                             = 0x00000001; 

/**
* Set of applications by UID  that should NOT be closed when end key is 
* pressed. Same format as in KTelAppsCloseList but OOM token is not 
* applicable.
*/
const TUint32 KTelAppsKeepList                              = 0x00000002;

/**
* Application UID for long '0' key press. By default value is 0x10008D39 
* which is web browser UID. Products can set this to some other to 
* launch different applications. Note if you change this remember to change
* KTelAppLaunchParam.
*/
const TUint32 KTelAppLaunch                                 = 0x00000003; 

/**
* Param which is passed to application which is launched using long '0' 
* key press. By default value is '5' which is special value for web 
* browser and max lenght is 10 characters.Note if you change KTelAppLaunch
* you will need to change this too.
*/
const TUint32 KTelAppLaunchParam                            = 0x00000004; 

/**
* The ascii code of a button that is specified to launch an application
* defined in KTelButton1AppUID. This key is of type Int.
*/
const TUint32 KTelButton1                                   = 0x00FF0101; 

/**
* The UID of application to be launched on a press of a button defined 
* in KTelButton1. This key is of type Int.
*/
const TUint32 KTelButton1AppUID                             = 0x00FF0102;

/**
* A optional parameter to be passed to the application defined in
* KTelButton1AppUID. This key is of type String.
*/
const TUint32 KTelButton1AppParam                           = 0x00FF0103;

/**
* The ascii code of a button that is specified to launch an application
* defined in KTelButton2AppUID. This key is of type Int.
*/
const TUint32 KTelButton2                                   = 0x00FF0201;

/**
* The UID of application to be launched on a press of a button defined 
* in KTelButton2. This key is of type Int.
*/
const TUint32 KTelButton2AppUID                             = 0x00FF0202;

/**
* A optional parameter to be passed to the application defined in
* KTelButton2AppUID. This key is of type String.
*/
const TUint32 KTelButton2AppParam                           = 0x00FF0203;

/**
* The ascii code of a button that is specified to launch an application
* defined in KTelButton3AppUID. This key is of type Int.
*/
const TUint32 KTelButton3                                   = 0x00FF0301;

/**
* The UID of application to be launched on a press of a button defined 
* in KTelButton3. This key is of type Int.
*/
const TUint32 KTelButton3AppUID                             = 0x00FF0302;

/**
* A optional parameter to be passed to the application defined in
* KTelButton3AppUID. This key is of type String.
*/
const TUint32 KTelButton3AppParam                           = 0x00FF0303;

#endif      // TELEPHONYPRIVATECRKEYS_H

// End of file