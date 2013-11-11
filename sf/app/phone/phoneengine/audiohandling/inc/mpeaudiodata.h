/*
* Copyright (c) 2002-2004,2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for Audiohandling
*
*/


#ifndef MPEAUDIODATA_H
#define MPEAUDIODATA_H

//  INCLUDES
#include <pevirtualengine.h>
#include <e32base.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
/**
*  Interface class for audiohandling
*  
*  @lib audiohandling.lib
*  @since S60_5.0
*/
class MPEAudioData 
    {
    public: // New functions
        
        /**
        * Retrieves Audio mute synchronously.
        * @param  aAudioMute, audio mute is returned in this parameter.
        */
        virtual void GetAudioMuteSync( TBool& aAudioMute ) const = 0;

        /**
        * Retrieves Audio volume synchronously.
        * @param  aAudioVolume, audio volume is returned in this parameter.
        */
        virtual void GetAudioVolumeSync( TInt& aAudioVolume ) const = 0;
      
        /**
        * Handles call audio route preference changed
        */
        virtual TInt CallAudioRoutePreferenceChanged() = 0;
 
        /**
        * Stops the playing of dtmf tone
        */
        virtual void StopDtmfTonePlay() = 0; 

        /**
        * Makes synchronous request to set Audio Mute.
        * @param  aAudioMute, mute to be set.
        */
        virtual void SetAudioMuteSync( const TBool aAudioMute ) = 0;

        /**
        * Makes synchronous request to set Audio Volume.
        * @param  aAudioVolume, volume to be set.
        */
        virtual void SetAudioVolumeSync( const TInt aAudioVolume ) = 0;
        
        /**
        * Makes request to Telephony Audio Routing to set audio output path
        * @param aOutput, audio output path
        * @param aShowNote, Show note status
        * @return KErrNone or possible errorcode.
        */
        virtual TInt SetAudioOutput( const TPEAudioOutput aAudioMute,
                                     TBool aShowNote ) = 0;
        
         /**
        * Gets volume levels at startup.
        */
        virtual void StartUp() = 0;
        
        /**
        * Initialises call audio route when call is alerting or answered
        */
        virtual TInt HandleCallStarting( TBool aVideoCall ) = 0;
        
        /**
        * Initialises energency call audio route
        */
        virtual TInt HandleEnergencyCallStarting() = 0;
        
        /**
        * Ends call audio route when call disconnected
        */
        virtual void HandleCallEnding() = 0;

    };

#endif    // MPEAUDIODATA_H     
            
// End of File
