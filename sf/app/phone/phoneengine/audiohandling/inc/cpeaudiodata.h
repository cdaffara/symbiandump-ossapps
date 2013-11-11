/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the class CPEAudioData
*                 which is the interface class for Audio Handling Module
*
*/


#ifndef CPEAUDIODATA_H
#define CPEAUDIODATA_H

//  INCLUDES
#include "mpeaudiodata.h"
#include <PSVariables.h>

#include <TelephonyAudioRouting.h>
#include <tmseffect.h>
#include <tmseffectobsrvr.h>


// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

using namespace TMS;

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;
class CPEAudioDtmfTonePlayer;
class CPEAudioRoutingMonitor;
class CPECallAudioRoutingHandler;
class CPEAudioFactory;
class CTelephonyAudioRouting;
class CPEAudioEffect;

// CLASS DECLARATION

/**
*  This file contains the header file of the class CPEAudioData,
*  which is an interface class to audio
*  properties of the system
*
*  @lib audiohandling.lib
*  @since S60_5.0
*/
class CPEAudioData : public CBase,
                     public MPEAudioData,
                     public TMSEffectObserver
    {

    public:

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPEAudioData();

    protected:

        /**
        * C++ default constructor.
        */
        CPEAudioData( MPEPhoneModelInternal& aPhoneModel );

    public: // from MPEAudioData

        /**
        * Retrieves Audio mute synchronously.
        * @param  aAudioMute, audio mute is returned in this parameter.
        */
        IMPORT_C void GetAudioMuteSync( TBool& aAudioMute ) const;

        /**
        * Retrieves Audio volume synchronously.
        * @param  aAudioVolume, audio volume is returned in this parameter.
        */
        IMPORT_C void GetAudioVolumeSync( TInt& aAudioVolume ) const;

        /**
        * Makes synchronous request to set Audio Mute.
        * @param  aAudioMute, mute to be set.
        */
        IMPORT_C void SetAudioMuteSync( const TBool aAudioMute );

        /**
        * Makes synchronous request to set Audio Volume.
        * @param  aAudioVolume, volume to be set.
        */
        IMPORT_C void SetAudioVolumeSync( const TInt aAudioVolume );

        /**
        * Makes request to Telephony Audio Routing to set audio output path
        * @param aOutput, audio output path
        * @param aShowNote, audio note status
        * @return KErrNone or possible errorcode.
        */
        IMPORT_C TInt SetAudioOutput(
            const TPEAudioOutput aOutput,
            TBool aShowNote );

        /**
        * Gets volume levels, audio mute, audio mode and audio volume
        * at startup and starts monitors.
        * @param  None.
        * @return None.
        */
        IMPORT_C void StartUp();

        /**
        * Handles call audio route preference changed
        */
        IMPORT_C TInt CallAudioRoutePreferenceChanged();

        /**
        * Initialises call audio route when call is alerting or answered
        */
        IMPORT_C TInt HandleCallStarting( TBool aVideoCall );

        /**
        * Initialises energency call audio route
        */
        IMPORT_C TInt HandleEnergencyCallStarting();

        /**
        * Ends call audio route when call disconnected
        */
        IMPORT_C void HandleCallEnding();

    public: // new

        /**
        * Sends AudioHandlingError message to PhoneEngine and saves
        * errorcode to member variable.
        * @param  aErrorCode, errorcode that is saved to member variable.
        * @return None.
        */
        void SendErrorMessage( const TInt aErrorCode );

        /**
        * Forwards message given as parameter to PhoneEngine.
        * @param  aMessage, message to be forwarded.
        * @return None.
        */
        void SendMessage(
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );

        /**
        * Forwards message given as parameter to Phone Engine
        * @param  aMessage, message to be forwarded.
        * @param  aOutput, audio output path that is saved in member variable.
        * @return None.
        */
        void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
                          const CTelephonyAudioRouting::TAudioOutput aOutput );

        /**
        * Forwards message given as parameter to Phone Engine
        * @param  aMessage, message to be forwarded.
        * @param  aOutput, audio output path that is saved in member variable.
        * @return None.
        */
        void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
                          CTelephonyAudioRouting& aTelephonyAudioRouting );

        /**
        * Sets value of audio routing preference
        */
        void SetRoutePreference( TBool aShowNote );

        /**
        * Return value of audio routing preference
        */
        CTelephonyAudioRouting::TAudioOutput RoutePreference();

        /**
        * Return value of previous output
        */
        CTelephonyAudioRouting::TAudioOutput PreviousOutput();

        /**
        * Return value of current output
        */
        CTelephonyAudioRouting::TAudioOutput Output();

        /**
        * Return status of wired accessory availability
        */
        TBool IsWiredAvailable();

        /**
        * Return status of BT accessory availability
        */
        TBool IsBTAvailable();

        /**
        * Return status of TTY accessory availability
        */
        TBool IsTTYAvailable();

        /**
        * Set new audio route to TAR
        */
        void SetTAROutput( CTelephonyAudioRouting::TAudioOutput aOutput,
                           TBool aShowNote );

        /**
         * From TMSEffectObserver
         */
    void EffectsEvent(const TMSEffect& tmseffect, TMSSignalEvent event);

    protected:

        /**
        * By default Symbian 2nd phase constructor is private, but we use
        * it at generalized class.
        */
        void ConstructL( CPEAudioFactory& aAudioFactory );

    protected:  // New functions

        /**
         * Updates volume on DataStore and if aSendMsg==ETrue sends
         * notification to engine monitor.
         * @since S60 v5.0
         * @param aVolume New volume value.
         * @param aSendMsg ETrue will send notification.
         */
        virtual void DoHandleVolumeChange( TInt aVolume, TBool aSendMsg=EFalse );

    private:

        /**
        * Gets all audio volume value and update member variable
        */
        void InitializeAudioVolume();


        /**
        * Makes request to Telephony Audio Routing to get available audio outputs
        * @return list of available outputs
        */
        TArray< TPEAudioOutput> GetAvailableOutputs(
                CTelephonyAudioRouting& aTelephonyAudioRouting );

        /**
        * Converts TAR route value to PE value
        */
        TPEAudioOutput ConvertToPE( CTelephonyAudioRouting::TAudioOutput aOutput );

        /**
        * Converts PE route value to TAR value
        */
        CTelephonyAudioRouting::TAudioOutput ConvertToTAR( TPEAudioOutput aOutput );

    protected:
        // CPhoneModel object which owns this object.
        MPEPhoneModelInternal& iPhoneModel;
        // Audio Routing Monitor
        CPEAudioRoutingMonitor* iAudioRoutingMonitor;
        // Audio Routing Handler
        CPECallAudioRoutingHandler* iAudioRoutingHandler;
        // Telephony Audio Routing interface
        CTelephonyAudioRouting* iAudioRouting;
        // Available Audio Outputs
        RArray<TPEAudioOutput> iPEavailableOutputs;
        //Flag to indicate whether is Call audio route initialized
        TBool iRouteInitialized;

        CPEAudioEffect* iAudioEffect;
     };

#endif      // CPEAUDIODATA_H

// End of File
