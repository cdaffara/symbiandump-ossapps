/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video session interface.
*
*/



#ifndef MVTENGSESSIONINFO_H
#define MVTENGSESSIONINFO_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Abstraction for video telephony session with remote end.
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngSessionInfo
    {
    public: // 

        /**
        * Session state
        * EIdle           Session not established.
        * EInitializing   Initializing MO session
        * ERemoteAlerting Remote end is alerting
        * EReceiving      Incoming session request from network
        * EConnected      Connection to peer is established.
        * ENegotiating    Handshaking with peer.
        * EOpen           Session active and media channels are open.
        * EClearing       Clearing connection.
        */
        enum TSessionState 
            {
            EUnknown = -1,
            EIdle,
            EInitializing,
            ERemoteAlerting,
            EReceiving,
            EConnected,
            ENegotiating,
            EOpen,
            EClearing
            };
        
        typedef TTimeIntervalSeconds TDuration;

        /**
        * Session direction
        * EDirectionNone In idle there's no direction .
        * EDirectionMO   Mobile originated sesison
        * EDirectionMT   Mobile terminated session
        */
        enum TDirection 
            {
            EDirectionNone,
            EDirectionMO,
            EDirectionMT
            };
        
        enum { KCallNameLength = 50 };
        enum { KCallNumberLength = 50 };
        
        typedef TBuf<KCallNameLength> TCallName;
        
        typedef TBuf<KCallNumberLength> TCallNumber;
        
        /**
        * CLI data related to a call
        */
        struct TCLI {
            // call id of the call
            TInt iCallId;
            
            // caller name
            TCallName iName;
            
            // indicates if voice call can be made to peer
            TBool iVoiceCallPossible;
            
            TCallNumber iCallNumber;
            };
        
        /**
        * Returns session state
        * @aForcedRefresh if ETrue current state is forced to update.
        * Should be EFalse when called in frequently because forced
        * refresh is more time consuming (requires context switch)
        * @return
        */
        virtual TSessionState State( TBool aForcedRefresh ) const = 0;

        /**
        *
        * @return direction
        */
        virtual TInt GetDirection( TDirection& aDirection ) const = 0;

        /**
        *
        * @param 
        * @param aEnabled
        */
        virtual TInt GetDuration( 
            TDuration& aDuration,
            TBool& aEnabled ) const = 0;
        
        
        virtual TInt GetCLI( TCLI& aCLI ) const = 0;
    };


#endif      // TVTENGSESSIONINFO_H
            
// End of File
