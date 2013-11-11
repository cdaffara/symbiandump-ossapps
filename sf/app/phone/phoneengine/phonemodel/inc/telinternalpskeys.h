/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Telephony internal Publish and Subscribe keys.
*
*/


#ifndef TELINTERNALPSKEYS_H
#define TELINTERNALPSKEYS_H

#include <e32std.h>
#include <telcommsinfopskeys.h>

// CONSTANTS

/*****************************************************************************
* Telephony Internal Information API UID 
*****************************************************************************/

const TUid KPSUidTelInternalInformation = {0x102029A3}; 

/**
* Used between Telephony and Telephony service engine.
* The telephony does a contact match and provides it to TSE.
* It contain an information on remote party contact info.
* NOTE: Key to be removed in Taco release
* @see TTelRemotePartyContactInfo in TelephonyInternalPSTypes.h
* @type RProperty::EByteArray
*/
const TUint32 KTelRemotePartyContactInfo = 0x00000001;

/**
* Identifier of Video Telephone application.
* This key has two special values, see TPSTelVTAppState.
* 
* @type RProperty::EInt
*/
const TUint32 KTelVideoCallUid = 0x00000002;
enum TPSTelVTAppState
    {
    // Video Telephone was on background when call ended, no need
    // to set Phone/Idle on foreground.
    EPSTelVTAppStateBackground = -1,
    // Video Telephone
    EPSTelVTAppStateNone = 0
    };

/*****************************************************************************
* Telephony Audio Preference API UID 
*****************************************************************************/
const TUid KPSUidTelAudioPreference = {0x102029A4};
    
/**
* Indicates the in call audio routing
* preference to the phone (public/private)
* This key has two special values, see EPSTelAudioOutput.
* 
* @type RProperty::EInt
*/          
const TUint32 KTelAudioOutput = 0x00000001;
enum EPSTelAudioOutput
    {
    EPSAudioPrivate = 0,
    EPSAudioPublic    
    };    

/******************************************************************************
* Number entry Information API UID 
******************************************************************************/
const TUid KPSUidNEInformation = {0x102029A5};

/**
* Indication to Idle that whether number entry
* is open in phone or not
* NOTE:  deprecated in taco
*/
const TUint32 KTelNumberEntryInfo = 0x00000001; 
enum TPSTelNumberEntryInfo 
    {
    EPSTelNumberEntryClosed = 0,
    EPSTelNumberEntryOpen    
    };

/*****************************************************************************
* Telephony Generic Command API
* This can be used for requesting simple services from
* other process/application when it is inconvenient to implement
* client/server or similar complex pattern.
*****************************************************************************/
const TUid KPSUidTelGenericCommand = {0x102029A7};

/*
* Command operation values that should be used with keys in 
* Telephony Generic Command API. Client entity sets value
* to EPSTelGenCmdPerform. Service provider sets value 
* back to EPSTelGenCmdReset when command was succesfully 
* completed. In case of failure it may set it to EPSTelGenCmdFail
* to indicate error to requester.
* 
*/
enum {
    // Values to be set by service provider.
    EPSTelGenCmdFail = -1,
    EPSTelGenCmdReset,    
    
    // Client uses this value to request a service.
    EPSTelGenCmdPerform
    };

/**
* Command key for launching Log application.
* @type RProperty::EInt
*/
const TUint32 KTelGenCmdLaunchLogApp = 0x00000001;

#endif      // TELINTERNALPSKEYS_H

// End of file