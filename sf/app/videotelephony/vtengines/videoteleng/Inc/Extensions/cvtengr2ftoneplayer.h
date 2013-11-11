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
* Description:  Plays video start/stop sounds. 
*
*/


#ifndef CVTENGR2FTONEPLAYER_H
#define CVTENGR2FTONEPLAYER_H

// INCLUDES
#include <e32base.h>
#include <mdaaudiosampleplayer.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Plays video start/stop sounds.
* @since s60 v5.0
*/
NONSHARABLE_CLASS ( CVtEngR2FTonePlayer ) : public CBase,
                         public MMdaAudioPlayerCallback 
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVtEngR2FTonePlayer* NewL();

        /**
        * Destructor.
        */
        virtual ~CVtEngR2FTonePlayer();

    public: // New functions
        /**
        * Play a sound with given id.
        * @param aSound identifies which sound to play
        *        (see r_ccor_sound_list in resources).
        * @return Symbian error code. 
        */
        TInt PlaySound( TInt aSound );
        
        /**
        * Stop any ongoing sound.
        */
        void StopSound();

    private: // Functions from base classes
        /**
        * From MMdaAudioPlayerCallback.
        */
        void MapcInitComplete( TInt aError, 
            const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback.
        */
        void MapcPlayComplete( TInt aError );

    private: // New functions
        /**
        * C++ constructor.
        */
        CVtEngR2FTonePlayer();

        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();

    private: // Data

        TBool iOpenFileInProgress;
        TBool iSoundInProgress;
        CMdaAudioPlayerUtility* iAudioPlayer; // owned

        HBufC* iVideoStartSound;              // owned
        HBufC* iVideoStopSound;               // owned
    
    public:
       	
       	/**
    	*	Enum for tone player sound ID's
    	*/
  		enum TVtR2FSoundId
			{
			/** Video record start sound*/
     		EVtR2FVideoStartSoundId = 1,
     		/** Video record stopt sound*/
     		EVtR2FVideoStopSoundId  
     		};    
    };

#endif //CVTENGR2FTONEPLAYER_H

// End of File
