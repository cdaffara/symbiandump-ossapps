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
* Description:  Defines the ECom plugin for HTI audio playback control service.
*
*/



#ifndef HTIAUDIOSERVICEPLUGIN_H
#define HTIAUDIOSERVICEPLUGIN_H

//  INCLUDES
#include <badesca.h>
#include <HtiServicePluginInterface.h>
#include <mdaaudiosampleplayer.h>
#include <mdaaudiotoneplayer.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The ECom plugin for HTI audio playback control service.
*
*/
class CHtiAudioServicePlugin : public CHTIServicePluginInterface,
                               public MMdaAudioPlayerCallback,
                               public MMdaAudioToneObserver
    {

    protected:
        // commands
        enum TAudioCommands
            {
            ECmdListAudioFiles = 0x01,
            ECmdPlayFile       = 0x02,
            ECmdPlayTone       = 0x03,
            ECmdPlayDtmf       = 0x04,
            ECmdStop           = 0x05,
            ECmdGetDuration    = 0x06,
            ECmdGetMaxVol      = 0x07,
            ECmdSetVol         = 0x08,
            ECmdPlayFile2      = 0x10,
            ECmdPlayTone2      = 0x11,
            ECmdPlayDtmf2      = 0x12,
            ECmdGetMaxVol2     = 0x13,
            ECmdSetVol2        = 0x14
            };

        // audio setting options
        enum TAudioSetting
            {
            EDefault,
            EGeneralMusic,
            ERingTonePreview,
            //EIncomingCall,
            EDtmfString
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHtiAudioServicePlugin* NewL();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CHTIServicePluginInterface
        * Called by the HTI Framework when sending message to this service.
        * @param aMessage message body destinated to a servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        /**
        * From CHTIServicePluginInterface
        * Called by HTI Framework to tell the service how much memory is
        * available for messages in the message queue.
        * @param aAvailableMemory amount of currently available memory
        *        in the message queue
        */
        void NotifyMemoryChange( TInt aAvailableMemory );

        /**
        * From CHTIServicePluginInterface
        * Indicates to HTI Framework whether the plugin is ready to process
        * a new message or if it's busy processing previous message.
        */
        TBool IsBusy();

        /**
        * From MMdaAudioToneObserver
        * Handle the event when a tone utility initialisation
        * operation has completed.
        * @param aError indicates whether an error occurred.
        */
        void MatoPrepareComplete( TInt aError );

        /**
        * From MMdaAudioToneObserver
        * Handle the event when a tone playing operation has completed.
        * @param aError indicates whether an error occurred.
        */
        void MatoPlayComplete( TInt aError );

        /**
        * From MMdaAudioPlayerCallback
        * Handle the event when initialisation of
        * the audio player utility is complete.
        * @param aError The status of the audio sample after initialisation
        * @param aDuration The duration of the sample
        */
        void MapcInitComplete( TInt aError,
                               const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback
        * Handle the event when when the audio player utility
        * completes asynchronous playing.
        * @param aError The status of playback
        */
        void MapcPlayComplete( TInt aError );


    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CHtiAudioServicePlugin();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CHtiAudioServicePlugin();


    protected:  // Functions from base classes

    private:

        void HandleListAudioFilesCmdL( const TDesC8& aMessage );

        void HandlePlayFileCmdL( const TDesC8&aMessage );

        void HandlePlayToneCmdL( const TDesC8& aMessage );

        void HandlePlayDtmfCmdL( const TDesC8& aMessage );

        void HandleStopCmdL( const TDesC8& aMessage );

        void HandleGetDurationCmdL( const TDesC8& aMessage );

        void HandleGetMaxVolCmdL( const TDesC8& aMessage );

        void HandleSetVolCmdL( const TDesC8& aMessage );

        TInt ParseString( const TDesC8& aRequest,
                               TInt aOffset, TDes& aResult );

        TInt SendResponseMsg( const TDesC8& aMsg,
                             const TUint8 aCommandId = 0  );

        TInt SendErrorResponseMsg( TInt aErrorCode,
                                   const TDesC8& aErrorDescription,
                                   const TUint8 aCommandId = 0 );

        void AddSubdirsRecursivelyL( const TDesC& aPath,
                                    CDesCArraySeg& aArray, RFs& aFs );

        TBool MatchMimeTypeL( const TDesC& aFilePath,
                              const TDesC& aMimeTypeMatchPattern );

        void SetAudioSettings( TAudioSetting aSetting );

        inline TUint ParseUint16( const TUint8* aPtr )
            {
            return aPtr[0] + ( aPtr[1] << 8 );
            }

        inline TUint ParseUint32( const TUint8* aPtr )
            {
            return aPtr[0] + ( aPtr[1] << 8 )
                           + ( aPtr[2] << 16 ) + ( aPtr[3] << 24 );
            }


    public:     // Data

    protected:  // Data

    private:    // Data
        // Flag telling if the service is busy processing a message
        TBool iIsBusy;

        // Flag telling if audio or tone is currently playing
        TBool iIsPlaying;

        // Id of the latest command
        TUint8 iCommandId;

        // Id of the latest play command
        TUint8 iPlayCommandId;

        // Container for the response message to be dispatched out
        HBufC8* iMessage;

        // Error code in the outgoing error message
        TInt iErrorCode;

        // Audio player utility object
        CMdaAudioPlayerUtility* iAudioPlayer;

        // Tone player utility object
        CMdaAudioToneUtility* iTonePlayer;

        // Current volume value
        TInt iVolume;

        // Current repeat value
        TInt iRepeats;

        // Current trailing silence value
        TInt iTrailingSilence;

        // DTMF tone length
        TInt iDtmfLength;

        // Silence between DTMF tones
        TInt iDtmfGapLength;

        // Playback start position
        TInt iStartPos;

        // Playback end position
        TInt iEndPos;

        // Audio priority setting
        TInt iAudioPriority;

        // Audio priority preference setting
        TMdaPriorityPreference iAudioPriorityPreference;


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // HTIAUDIOSERVICEPLUGIN_H

// End of File
