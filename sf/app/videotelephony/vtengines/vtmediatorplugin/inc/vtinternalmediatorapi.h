/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message Interface between VT mediator plugin and VT application
*
*/


#ifndef VTINTERNALMEDIATORAPI_H
#define VTINTERNALMEDIATORAPI_H

// INCLUDES
#include <e32base.h>

/** The category UID for the messages in this header file.
 * UID for category is not real UID, it just need to unique in Domain
 */
const TUid KCatVideotelInternalEvents = { 0x101F8690 };   
const TUid KCatVideotelInternalCommands = { 0x101F8691 };  

/**  Version number of this API */
const TInt KVideotelMdtrEventVersionMajor( 1 );
const TInt KVideotelMdtrEventVersionMinor( 0 );
const TInt KVideotelMdtrEventVersionBuild( 0 );

/**  Version number of this API */
const TInt KVideotelMdtrCommandsVersionMajor( 1 );
const TInt KVideotelMdtrCommandsVersionMinor( 0 );
const TInt KVideotelMdtrCommandsVersionBuild( 0 );

struct TVtVideoTelephonyCallInformation
{   
    /** Max length of display text */
    enum { TDisplayTextMaxLength = 100 }; 
    enum { TPhoneNumMaxLength = 100 };
                                     
    /** Display text type definition */
    typedef TBuf<TDisplayTextMaxLength> TDisplayText;

    /** Remote end CLI identifier (name/number/call id) */
    TDisplayText iDisplayText;
    
    /** Call id of the video call */
    TInt iCallId;
                                 
    /** Indicates voice call creation possibility to remote end (number is
     *  available)
     */
    TBool iVoiceCallPossible; 
    
    /** Dataport name or empty buffer if not specified in the event */
    TName iDataport;

    /**
    * Defines which parts of the event date is valid
    */
    enum TMessageDataValidity {
        EDisplayTextValid = 0x01,
        EDataportValid    = 0x02
    };
    
    /** Display text type definition */
    typedef TBuf<TPhoneNumMaxLength> TPhoneNumText;
      
    /** Remote end CLI identifier (name/number/call id) */
    TPhoneNumText iPhoneNumText;
                                               
    /** Event data validity */
    TUint iEventDataValidity;
        
};

typedef TPckgBuf<TVtVideoTelephonyCallInformation> TVtMediatorInfoPackage;

/**  Events bwtween Video Telephony Mediator plugin and VT application. */
enum TVideotelMdtrEvents
    {
    /**
     * Event with TVtVideoTelephonyCallInformation as content
     * Consuming event requires capability ECapabilityReadDeviceData
     */
    EVtMediatorEventVideoCallInformation = 0
    };

enum TVideotelMdtrCommands
    {
    EVtMediatorReleaseDataport = 0
    };

#endif // VTINTERNALMEDIATORAPI_H
