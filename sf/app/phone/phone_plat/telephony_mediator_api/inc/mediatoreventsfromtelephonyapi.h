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
* Description:  Mediator API for events sent from Telephony.
*
*/


#ifndef MEDIATOREVENTSFROMTELEPHONYAPI_H
#define MEDIATOREVENTSFROMTELEPHONYAPI_H

// INCLUDES
#include <e32base.h>
#include <cntdef.h>

/** Max Length of Remote Phone Number */
const TInt KMaxPhoneNumberLength = 100;

/**  The category UID for the messages in this header file. */
const TUid KCatEventsFromTelephony = { 0x100058BA };

/**  Version number of this API */
const TInt KTelephonyEventsVersionMajor( 1 );
const TInt KTelephonyEventsVersionMinor( 0 );
const TInt KTelephonyEventsVersionBuild( 0 );

namespace TelephonyMediator {
/**
* Call states
*/
enum TCallState
    {
    ECallStateUnknown = 0,
    ECallStateIdle,
    ECallStateDialling,
    ECallStateEmergencyDialling,
    ECallStateRinging,
    ECallStateConnecting,
    ECallStateConnected,
    ECallStateHangingUp,
    ECallStateHeld,
    ECallStateAnswering,
    ECallStateRejecting,
    ECallStateDisconnecting
    };

enum TCallType
    {
    // Uninitialized or unknown call type
    ECallTypeUninitialized = 0,

    // Circuit switched voice call
    ECallTypeCSVoice,

    // Fax call
    ECallTypeFax,

    // Circuit switched data call with or without high speed HSCSD attribute
    ECallTypeData,
    
    // Circuit switched multimedia call
    ECallTypeVideo,
    
    // Packet switched voice call
    ECallTypeVoIP
    };


// Call Data Parameter is sent with EPhoneEventCallData
class TTelephonyCallDataParam
    {
    public:
        TInt iCallId;
        TBuf<KCntMaxTextFieldLength> iCLIText;
        TCallState iCallState;
        TCallType iCallType;
        TBuf<KMaxPhoneNumberLength> iRemotePhoneNumber;
    };
    
// Mediator package
typedef TPckgBuf<TTelephonyCallDataParam> TTelephonyCallDataParamPackage;

/**  Generic Events from Phone Application. */
enum TEventsFromTelephony
    {
    /**
     * This event is sent when Phone Application creates a new call bubble.
     * TTelephonyCallDataParamPackage is sent in Package Buffer.
     */
    EPhoneEventCallData = 0
    };

} // namespace TelephonyMediator

#endif // MEDIATOREVENTSFROMTELEPHONYAPI_H
