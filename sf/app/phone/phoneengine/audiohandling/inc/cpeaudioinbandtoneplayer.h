/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class CPEAudioInbandTonePlayer, that plays DTMF tones 
*                using media server
*
*/


#ifndef CPEAUDIOINBANDTONEPLAYER_H
#define CPEAUDIOINBANDTONEPLAYER_H

// INCLUDES
#include <pevirtualengine.h>
#include <e32base.h>
#include <mdaaudiotoneplayer.h>
#include <ccpdefs.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPEAudioToneUtility;
class CPEAudioData;
class CPEAudioFactory;

// CLASS DECLARATION

/**
*  This class is used to play inband tones using media server.
*
*  @lib audiohandling.lib
*  @since Series60_5.0
*/
class CPEAudioInbandTonePlayer 
    : public CBase, 
      private MMdaAudioToneObserver
    {
    public:  // Constructors and destructor      
        
        /**
        * Two-phased constructor.
        */
        static CPEAudioInbandTonePlayer* NewL( 
            CPEAudioData& aOwner, 
            RFs& aFsSession,
            CPEAudioFactory& aFactory );
        
        /**
        * Destructor.
        */
        ~CPEAudioInbandTonePlayer();

        /**
        * Plays given tone. Originates from CCP plugin.
        * @param aTone inband tone needed to play
        */
        void PlayInbandTone( TCCPTone aTone );

        /**
        * Cancels playing.
        */
        void Cancel();

        /**
         * Sets volume.
         * @since    S60 5.0
         * @param    aVolume     Volume in 0...10 scale.
         */
        void SetVolume( TInt aVolume );

    private:

        /**
        * C++ default constructor.
        */
        CPEAudioInbandTonePlayer( 
            CPEAudioData& aOwner, 
            RFs& aFsSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CPEAudioFactory& aFactory );

        /**
        * Playes current tone.
        * @since Series60_2.7
        * @param None.
        * @return None.
        */
        void PlayCurrentTone();

        //From MMdaAudioToneObserver
        /**
        * Handles completion of preparation.
        * @since Series60_2.7
        * @param aError Isn't used.
        * @return None.
        */
        void MatoPrepareComplete( TInt aError );

        //From MMdaAudioToneObserver
        /**
        * Handles completion of playing.
        * @since Series60_2.7
        * @param aError Isn't used.
        * @return None.
        */
        void MatoPlayComplete( TInt aError );
        
        /**
         * Calculates media server volume level.
         * @since   Series S60 5.1
         * @param   aVolume     Volume level in 0...10 scale.
         * @return  Volume level in media server scale.
         */
        TInt CalculateMediaServerVolume( TInt aVolume ) const;
        
        /**
         * Updates tone player's volume to the current audio volume level.
         * @since   Series S60 5.1
         */
        void UpdateTonePlayerVolume();
        
    private:    // Data

        // Currently playing, NULL or tone sequence.
        CPEAudioToneUtility* iCurrent;
        //Member variable for inband tone
        TCCPTone iToneName;
        // Owner of this object
        CPEAudioData& iOwner;
        // Current volume setting.
        //NetworkBusy sequence
        HBufC8* iResourceBusySeq;
        //NetworkBusy sequence
        HBufC8* iResourceReorderSeq;
        //NetworkBusy sequence
        HBufC8* iResourceCongestionSeq;
        //NetworkBusy sequence
        HBufC8* iResourceSpecialSeq;
        //NetworkBusy sequence
        HBufC8* iResourceRadioPathSeq;
        //NetworkBusy sequence
        HBufC8* iResourceRingGoingSeq;
        //CallWaiting sequence
        HBufC8* iResourceCallWaitingSeq;
        //Ringing type
        TProfileRingingType iRingingType;
        //Data call tone sequence
        HBufC8* iPlayDataSequence;
        //Data call silent sequence
        HBufC8* iPlayNoSoundSequence;
        //Data call beep sequence
        HBufC8* iPlayBeepSequence;
        //Handle to a file server session
        RFs& iFsSession;

    };

#endif      // CPEAUDIOINBANDTONEPLAYER_H
            
// End of File
