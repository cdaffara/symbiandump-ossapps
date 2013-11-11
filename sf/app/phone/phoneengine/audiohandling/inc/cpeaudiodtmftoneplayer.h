/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class CPEAudioDtmfTonePlayer, 
*                that plays DTMF tones using media server.
*
*/


#ifndef CPEAUDIODTMFTONEPLAYER_H
#define CPEAUDIODTMFTONEPLAYER_H

// INCLUDES
#include    <e32base.h>
#include    <sounddevice.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CMMFDevSound;

// CLASS DECLARATION

/**
*  plays DTMF tones using media server
*
*  @lib audiohandling.lib
*  @since Series60_2.7
*/
class CPEAudioDtmfTonePlayer 
    : public CBase, 
      public MDevSoundObserver
    {
    public:  // Constructors and destructor      
        
        /**
        * Two-phased constructor.
        */
        static CPEAudioDtmfTonePlayer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPEAudioDtmfTonePlayer();

    public:

        /**
        * Sets volume.
        * @since Series60_2.7
        * @param aVolume volume value or KDtmfSilent.
        * @return None.
        */
        void SetVolume( TInt aVolume );

        /**
        * Starts playing given dtmf tone infinitely.
        * @since Series60_2.7
        * @param aTone Tone to be played.
        * @return None.
        */
        void PlayDtmfTone( TChar aTone );

        /**
        * Cancels playing.
        * @since Series60_2.7
        * @param None.
        * @return None.
        */
        void Cancel();
        
    public: //From MDevSoundObserver
          /**
          * See MDevSoundObserver
          */
           void InitializeComplete( TInt aError );

          /**
          * See MDevSoundObserver
          */
           void ToneFinished( TInt aError );

          /**
          * See MDevSoundObserver
          */
           void BufferToBeFilled( CMMFBuffer* aBuffer );

          /**
          * See MDevSoundObserver
          */
           void PlayError( TInt aError );

          /**
          * See MDevSoundObserver
          */
           void BufferToBeEmptied( CMMFBuffer* aBuffer );

          /**
          * See MDevSoundObserver
          */
           void RecordError( TInt aError );

          /**
          * See MDevSoundObserver
          */
           void ConvertError( TInt aError );
          /**
          * See MDevSoundObserver
          */       
          void DeviceMessage( TUid aMessageType, const TDesC8& aMsg );   
          
    protected:

        /**
        * C++ default constructor.
        */
        CPEAudioDtmfTonePlayer();

        void ConstructL();


        /**
        * Normalize tone.
        * @since Series60_2.7
        * @param aTone dtmf tone.
        * @return None.
        */
        void Normalize( TChar& aTone );

  
        
    private: //new functions
        
        /**
         * Converts the given value to the one that matches tha range used by 
         * CMMFDevSound. The CMMFDevSound uses valuess that can from 0 to the
         * value returned by a call to CMMFDevSound::MaxVolume().
         */
        TInt ConvertVolume( TInt aVolume );
    
    protected:    // Data

        // Current volume setting.
        TInt iVolume;
        
        // DTMF player instance
        CMMFDevSound*        iDTMFPlayer;

    };

#endif      // CPEAUDIODTMFTONEPLAYER_H
            
// End of File
