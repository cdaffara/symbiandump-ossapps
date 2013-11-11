/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Audio handler class.
*
*/



#ifndef CVTENGAUDIOHANDLER_H
#define CVTENGAUDIOHANDLER_H

//  INCLUDES
#include    <e32base.h>
#include    "MVtEngAudio.h"
#include    "CVtEngOperation.h"
#include    "MVtEngSettingPSObserver.h"
#include    "CVTEngPubSubsListener.h"
#include    "CVtEngAccessoryHandler.h"
#include    "MVtEngAccessoryObserver.h"
// new audio routing
#include    "TelephonyAudioRouting.h"
#include    "MTelephonyAudioRoutingObserver.h"
#include    <rphcltserver.h>
#include    <mmf/server/sounddevice.h>

// FORWARD DECLARATIONS
class CVtEngStateManager;
class CRoutingEnabler;

/**
*  Class for audio handling.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngAudioHandler )
    : public CBase,
      public MVtEngAudio,
      private MVtEngAccessoryObserver,
      private MTelephonyAudioRoutingObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVtEngAudioHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CVtEngAudioHandler();

    public: // New functions.

        /**
        * Performs on operation in the handler.
        * @param operation to be performed on the handler
        */
        void HandleL( CVtEngOperation& aOperation );

        /**
        * Creates/releases temporary DevSound instance.
        * @param aEnable
        *   ETrue creates devSound
        *   EFalse destructs devSound
        */
        void SetRoutingEnablerL( const TBool aEnable );

        /**
        * Increases audio volume
        */
        TBool IncreaseVolume();

        /**
        * Decreases audio volume
        */
        TBool DecreaseVolume();

    private: // Functions from base classes.

        // from MVtEngAccessoryObserver

        /** @see MVtEngAccessoryObserver */
        virtual void AccessoryModeChanged( const TAccMode& aMode,
            const TBool aActivated );

        // from MTelephonyAudioRoutingObserver

        /** @see MTelephonyAudioRoutingObserver::AvailableOutputsChanged */
		virtual void AvailableOutputsChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );

        /** @see MTelephonyAudioRoutingObserver::OutputChanged */
		virtual void OutputChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );

        /** @see MTelephonyAudioRoutingObserver::SetOutputComplete */
		virtual void SetOutputComplete( CTelephonyAudioRouting& aTelephonyAudioRouting,
		    TInt aError );

    public: // Functions from base classes.

        /** @see MVtEngAudio::GetRoutingState. */
        virtual TInt GetRoutingState( TAudioRoutingState& aAudioRoutingState );

        /** @see MVtEngAudio::GetRoutingAvailability. */
        virtual TInt GetRoutingAvailability(
            const TAudioRoutingState aAudioRoutingState,
            TBool& aAvailable );

        /** @see MVtEngAudio::GetHeadsetType. */
        virtual TInt GetHeadsetType( TWiredHeadsetType& aHeadsetType );

        /** @see MVtEngAudio::OutputVolume. */
        virtual TInt OutputVolume( const TBool aHandsetVolume ) const;

    private:

        /**
        * C++ default constructor.
        */
        CVtEngAudioHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Completes operation.
        * @param aResult error code
        */
        void CompleteOperation( TInt aResult );

        /**
        * Sets loudspeaker on / off.
        * @param aTurnOn ETrue if on, EFalse if off
        * @param aShowNote ETrue if note is allowed to be shown, EFalse if not
        */
        void SetLoudspeakerL( TBool aTurnOn, TBool aShowNote );

        /**
        * Sets Bluetooth on / off.
        * @param aTurnOn ETrue if on, EFalse if off
        */
        void SetBluetoothL( TBool aTurnOn );

        /**
        * Updates current volume that should be used.
        */
        void UpdateCurrentVolume();

        /**
        * Checks if given audio output route is available.
        * @return ETrue if given audio output route is available
        */
        TBool IsOutputAvailable( const CTelephonyAudioRouting::TAudioOutput aOutput );

        /**
        * Modifies the volume by aDelta.
        */
        TBool AdjustVolume( const TInt aDelta );

    private: // Data

        // Enumeration flags used in the implementation.
        enum
            {
            // Handset
            EFlagHandset = 1,
            // Loudspeaker
            EFlagLoudspeaker = 2,
            // Bluetooth handsfree
            EFlagBT = 4,
            // Wired headset
            EFlagWiredHeadset = 8
            };

        // Accessory mode listener class.
        CVtEngAccessoryHandler* iAccHandler;

        // Contains the current accessory and it's audio mode.
        TAccPolAccessoryMode iAccMode;

        // Pointer to telephony audio routing instance
        CTelephonyAudioRouting* iTelephonyAudioRouting;

        // Current audio routing state
        CTelephonyAudioRouting::TAudioOutput iCurrentAudioOutput;

        // Holds old routing state.
        TAudioRoutingState iOldAudioRoutingState;

        // Pointer to an operation.
        CVtEngOperation* iOperation;

        /**
        * For enabling audio routing before actual
        * DevSound for play is created.
        */
        CRoutingEnabler*    iRoutingEnabler;
    };

/**
* Class for handling dummy DevSound. It enables audio routing
* before video call is connected. Proper devSound instance
* must exist in order to control audio routing. Actual instance
* is created in protocol in later phase of the stack
* initialization and one in here is deleted.
*/
NONSHARABLE_CLASS( CRoutingEnabler ) :
    public CBase,
    private MDevSoundObserver
    {
    public: // constructor and destructor

        // Constructor
        static CRoutingEnabler* NewL();

        // Destructor
        ~CRoutingEnabler();

    private: // constructors

        // c++ constructor
        CRoutingEnabler();

        /**
        * Creates and starts devSound.
        */
        void StartL();

    public: // new functions

        public: // from MDevSoundObserver

        /**
        * @see MDevSoundObserver
        */
        virtual void InitializeComplete( TInt aError );

        /**
        * @see MDevSoundObserver
        */
        virtual void ToneFinished( TInt aError );

        /**
        * @see MDevSoundObserver
        */
        virtual void BufferToBeFilled( CMMFBuffer* aBuffer );

        /**
        * @see MDevSoundObserver
        */
        virtual void PlayError( TInt aError );

        /**
        * @see MDevSoundObserver
        */
        virtual void BufferToBeEmptied( CMMFBuffer* aBuffer );

        /**
        * @see MDevSoundObserver
        */
        virtual void RecordError( TInt aError );

        /**
        * @see MDevSoundObserver
        */
        virtual void ConvertError( TInt aError );

        /**
        * @see MDevSoundObserver
        */
        virtual void DeviceMessage(
            TUid aMessageType,
            const TDesC8& aMsg );

    private:
        // Temporary DevSound instance to enable audio routing
        // before actual instance in protocol is ready.
        // Routing can be changed already when MO call is alerting.
        CMMFDevSound* iDevSound;

        // Variable to detect failure in Devsound initialization or PlayInitL().
        // If failure happens, Devsound instance is deleted outside callback
        // in desctructor. ETrue; failure happened, EFalse; all OK
        TBool iDevsoundInitializeFailure;
    };


#endif      // CVTENGAUDIOHANDLER_H

// End of File
