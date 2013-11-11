/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CPHONEAUDIOPLAYER_H
#define CPHONEAUDIOPLAYER_H

// System includes
#include <e32base.h>
#include <mdaaudiosampleplayer.h>
#include <mdaaudiotoneplayer.h>

// Forward declarations
class MPhoneAudioPlayerObserver;
class C3DRingingToneInterface;
class CPhoneRingingTone;
class CAudioOutput;
 
/**
*  Audio player for playing audio sample or tone file. 
*
*  @since 1.0
*/
class CPhoneAudioPlayer :
    public CBase,
    private MMdaAudioPlayerCallback,
    private MMdaAudioToneObserver
    {
    public:

        enum TAudioPlayerStatus
            {
            EToneLoading,
            EToneReady,
            ETonePlaying
            };

        enum TRingingType
            {
            ETypeRinging,
            ETypeRingingOnce,
            ETypeAscending
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aRingingTone The ringing tone
        * @param aPriority The priority for the playing
        * @param aPreference The preference value
        * @param aObserver Audio player observer
        * @param aId The id of this audio player
        * @param aMdaServer The reference to media server
        * @param aExtSecNeeded ETrue if DRM extend security is needed
        * @return The instance of the audio player.
        */
        static CPhoneAudioPlayer* NewL( 
            const CPhoneRingingTone& aRingingTone, 
            TInt aPriority, 
            TUint aPreference,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aId = 0,
            CMdaServer* aMdaServer = NULL,
            TBool aExtSecNeeded = EFalse );

        /**
        * Two-phased constructor.
        * @param aRingingTone The ringing tone
        * @param aPriority The priority for the playing
        * @param aPreference The preference value
        * @param aObserver Audio player observer
        * @param aId The id of this audio player
        * @param aMdaServer The reference to media server
        * @param aExtSecNeeded ETrue if DRM extend security is needed
        * @return The instance of the audio player. NULL if the function fails.
        */
        static CPhoneAudioPlayer* New( 
            const CPhoneRingingTone& aRingingTone, 
            TInt aPriority, 
            TUint aPreference,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aId = 0,
            CMdaServer* aMdaServer = NULL,
            TBool aExtSecNeeded = EFalse );
            
        /**
        * Two-phased constructor.
        * @param aFileName The ringing tone file name.
        * @param aPriority The priority for the playing
        * @param aPreference The preference value
        * @param aObserver Audio player observer
        * @param aId The id of this audio player
        * @param aMdaServer The reference to media server
        * @param aExtSecNeeded ETrue if DRM extend security is needed
        * @return The instance of the audio player.
        */
        static CPhoneAudioPlayer* NewL( 
            const TDesC& aFileName, 
            TInt aPriority, 
            TUint aPreference,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aId = 0,
            CMdaServer* aMdaServer = NULL,
            TBool aExtSecNeeded = EFalse );            

        /**
        * Two-phased constructor, take audio from descriptor.
        *
        * @param aSequence sequence to be played.
        * @param aPriority priority of tone.
        * @param aPreference preference of tone.
        * @param aObserver observer.
        * @param aId identifier, used in observer interface 
        *            calls.
        * @param aMdaServer The reference to media server
        */
        static CPhoneAudioPlayer* NewSeqL(
            const TDesC8& aSequence,
            TInt aPriority,
            TUint aPreference,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aId = 0,
            CMdaServer* aMdaServer = NULL );
         
        /**
        * Two-phased constructor, takes Text-To-Speech text to be played.
        * @since Series 60 3.0
        * @param aText Text to be pronounced.
        * @param aPriority priority of tone.
        * @param aPreference preference of tone.
        * @param aObserver observer.
        * @param aId identifier, used in observer interface 
        *            calls.
        */        
        static CPhoneAudioPlayer* NewTtsL(
            const TDesC& aText,
            TInt aPriority,
            TUint aPreference,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aId = 0);
        
        /**
        * Destructor.
        */
        ~CPhoneAudioPlayer();
       
    public: // new function

        /**
        * Play the audio file or tone
        * @param aRingingType 
        * @param aVolume take value from 1 to 10
        * @param aTTsToBePlayed
        */
        void Play(
            TRingingType aRingType,
            TInt aVolume,
            TBool aTTsToBePlayed );

        /**
        * Stop current file playing
        */
        void StopPlaying();
        
          
        /**
        * Sets new volumes and ramptimes.
        * @param aVolume -volume to be set
        * @param aRamptime -ramptime to be set.
        */           
        void SetNewVolumeAndRamptime( TInt aVolume, TInt aRamptime );
        
        /**
        * Re starts playing.
        */
        void ReStartPlaying();
        /**
        * Enable or disable TTs playing.
        * @param aTTsToBePlayed
        */
        void SetTTsToBePlayed( TBool aTTsToBePlayed );
        
        /**
        * Sets volume level to zero.
        * @since Series 60 3.1
        */  
        void MutePlaying();
      

    private: //from base class

        /**
        * from MMdaAudioPlayerCallback
        */
        virtual void MapcInitComplete(
            TInt aError, 
            const TTimeIntervalMicroSeconds& aDuration );

        /**
        * from MMdaAudioPlayerCallback
        */
        virtual void MapcPlayComplete( TInt aError );

        /**
        * from MMdaAudioToneObserver
        */
        virtual void MatoPrepareComplete(TInt aError);

        /**
        * from MMdaAudioToneObserver
        */
        virtual void MatoPlayComplete(TInt aError);
        
    private:

        enum TAudioDataFormat
            {
            EFormatTone,
            EFormatSample,
            EFormatTts // Text-To-Speech
            };

        /**
        * C++ constructor.
        */
        CPhoneAudioPlayer( 
            TAudioDataFormat aDataFormat,
            MPhoneAudioPlayerObserver& aObserver, 
            TInt aPriority, 
            TUint aPreference,
            TInt aId );

        /**
        * Symbian OS constructor.
        */
        void ConstructL( 
            const TDesC& aFileName, CMdaServer* aMdaServer );

        /**
        * Symbian OS constructor.
        */
        void ConstructSeqL( 
            const TDesC8& aSequence,
            CMdaServer* aMdaServer ); 

        /**
        * Symbian OS constructor for the Text-To-Speech option
        * @since Series 60 3.0
        */
        void ConstructTtsL( 
            const TDesC& aTtsText, 
            TInt aPriority,
            TUint aPreference );

        /**
        * Do the playing stuff
        */
        void DoPlay();

        /**
        * Ser tinging type properties
        */
        void SetRingingTypeProperties();

        /**
        * Call back function for timer
        */
        static TInt TimerCallBack( TAny* aAny );

        /**
        * Handle timer events
        */
        void HandleTimerEvent();
        
        /*
        * Convert volume to the scale used by hardware
        * @param aVolume volume to be altered
        * @return TInt   new volume
        */
        TInt ConvertVolume( TInt aVolume );

    private:    // Data  

        // The format of audio data, audio sample file or tone file
        TAudioDataFormat iFormat;

        // Observer for error handling
        MPhoneAudioPlayerObserver& iObserver;

        // Priority. It's only used in playing tone file
        const TInt iPriority; 
        // Preference. Only used for tone file
        const TUint iPreference; 

        // Identifier number
        TInt iId;

        // Player for tone file.
        CMdaAudioToneUtility* iTonePlayer;

        // Player for audio file.
        CMdaAudioPlayerUtility* iSamplePlayer;

        // Player for the Text-To-Speech texts
        CMdaAudioPlayerUtility* iTtsPlayer;

        // The status of the player
        TAudioPlayerStatus iPlayerStatus;

        // How does the file played. It's only used if the player
        // is playing the file
        TRingingType iRingType;

        // Volume
        TInt iVolume;

        // For ascending and descending used for set current volume
        TInt iCurrentVolume;

        // Used by sample player
        TBool iToBePlaying;

        // Store for sequence.
        HBufC8* iSequence;
        
        // Store for the TTS source string
        HBufC8* iTtsText;

        // 3D Audio ringing tone plugin
        C3DRingingToneInterface* i3DPlugin;
        
        // Flag to  indicate whether we need to play TTs or not
        TBool iTTsToBePlayed;
        
        // Ringingtone output
        CAudioOutput* iAudioOutput; 
    };

#endif      // CPHONEAUDIOPLAYER_H   
            
// End of File
