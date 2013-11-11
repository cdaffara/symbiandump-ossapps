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
* Description:  Video call setting data.
*
*/



#ifndef CVTENGSETTINGS_H
#define CVTENGSETTINGS_H

//  INCLUDES
#include <e32base.h>
#include <bautils.h>
#include "MVtEngCRSettingObserver.h"
#include "CVtEngCRObserverItem.h"
#include "CVtEngCRProxy.h"
#include "MVtEngAudio.h"
#include "MVtEngMedia.h"
#include "MVtEngSessionInfo.h"

// CONSTANTS
// Default routing.
const MVtEngAudio::TVtEngRoutingSetting
    KVtEngDefaultAudioRouting = MVtEngAudio::EActivateHandsfree;

// FORWARD DECLARATIONS
class MVtEngSettingObserver;

// CLASS DECLARATION

/**
*  Container for video call settings.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngSettings ) : public CBase
, public MVtEngCRSettingObserver
    {
    public:

        /**
        * Settings ids
        */
        enum TSettingId
            {
            EStillImage,
            EStillImagePath,
            ECallTimer,
            EHandsetVolume,
            EHandsfreeVolume,
            EBTAudioAccessory,
            EIhfMode,
            ESelectVolume, // select Handset / Handsfree volume
            EDataportInfo
            };

        /**
        * Audio configuration
        */
        struct TVtEngAudioConfig
            {
            // Current volume in use, EFalse: handset,
            // ETrue: loudspeaker.
            TBool iCurrentVolume;
            // Speaker volume
            MVtEngAudio::TVtEngOutputVolume   iVolume;
            // Audio routing
            MVtEngAudio::TVtEngRoutingSetting iRouting;
            };

        /**
        * Video configuration
        */
        struct TVtEngVideoConfig
            {
            // Still image file name
            TFileName iStillImageFile;
            // Determines if still image is valid.
            TBool iImageIsValid;
            // Determines if video is enabled.
            TBool iVideoEnabled;
            };

        /**
        * Video quality framerates
        */
        struct TVtEngVideoQualityFrameRateConfig
            {
            // Frame rate for videoquality settings detail
            TInt    iDetail;

            // Frame rate for videoquality settings normal
            TInt    iNormal;

            // Frame rate for videoquality settings motion
            TInt    iMotion;
            };

        /**
        * Media configuration
        */
        struct TVtEngVideoCallConfig
            {
            // @see TVtEngAudioConfig
            TVtEngAudioConfig iAudio;
            // @see TVtEngVideoConfig
            TVtEngVideoConfig iVideo;
            // defines if call timer is enabled
            TBool iCallTimerOn;
            // Engine has green light to make connect on Protocol.
            TBool iReadyForConnect;
			// Dataport loaning status
			TBool iIsDataportLoaned;
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return instance of CVtEngSettings
        */
        static CVtEngSettings* NewL();

        /**
        * Destructor.
        */
        virtual ~CVtEngSettings();

    public: // New functions

        /**
        * Creates stored data observers.
        * @param aObserver callback interface
        * @param aUid UID to monitor
        * @param aKey ID to monitor
        * @param aType which data type
        */
        void CreateDataObserverL(
            MVtEngCRSettingObserver* aObserver, TUid aUid, TUint32 aKey,
            CCenRepNotifyHandler::TCenRepKeyType aType );

        /**
        * Returns configuration.
        * @return settings data
        */
        const CVtEngSettings::TVtEngVideoCallConfig& Config() const;

        /**
        * Sets current volume.
        * @param aVolume selected volume,
        *        EFalse: handset, ETrue: loudspeaker.
        */
        void SetCurrentVolume( TBool aVolume );

        /**
        * Returns current volume.
        * @return EFalse: handset, ETrue: loudspeaker.
        */
        TBool CurrentVolume() const;

        /**
        * Sets audio output volume levels.
        * @param aHandsetVolume handset volume level.
        * @param aHandsfreeVolume handsfree volume level.
        * @param aInternal not stored to shared data but
        * internal variable to be used when correct state
        * is entered.
        * @return Symbian error code
        */
        TInt SetVolume(
            const TInt aHandsetVolume,
            const TInt aHandsfreeVolume,
            const TBool aInternal );

        /**
        * Gets audio output volume levels from Shared Data.
        * @param aVolume volume level.
        * @param aHandsfree if true handsfree volume is read,
        * otherwise handset.
        * @param aInternal not read from shared data but
        * from internal variable
        * @return error code
        */
        TInt GetVolume(
            TInt& aVolume,
            const TBool aHandsfree,
            const TBool aInternal ) const;

        /**
        * Gets maximum audio output volume level.
        * @return max volume for device
        */
        inline TInt MaxVolume( ) const;


        /**
        * Gets minimum audio output volume level.
        * @return max volume for device
        */
        inline TInt MinVolume( ) const;

        /**
        * Checks the validity of the volume level.
        * return either constant minimum, constant maximum
        * or the passed value level.
        * @param aVolume the volume level to check
        * @return suitable volume level
        */
        inline TInt ValidVolume( const TInt aVolume ) const;

        /**
        * Sets audio routing setting.
        * @param aState audio routing state.
        */
        void SetRouting( const MVtEngAudio::TVtEngRoutingSetting& aState );

        /**
        * Sets output video state setting.
        * @param aEnabled ETrue if sending is enabled
        */
        void SetVideoEnabled( const TBool aEnabled );

        /**
        * Sets connected state.
        */
        void SetConnectReady();

        /**
        * Reads local variations.
        */
        void ReadVariationsL();

        /**
        * Gets camera orientations for primary and secondary camera.
        * @see MVtEngMedia::TCameraOrientation
        * @param aPrimaryCameraOrientation primary camera orientation.
        * @param aSecondaryCameraOrientation secondary camera orientation.
        */
        void GetCameraOrientations(
            MVtEngMedia::TCameraOrientation& aPrimaryCameraOrientation,
            MVtEngMedia::TCameraOrientation& aSecondaryCameraOrientation ) const;

        /**
        * Checks if given bits is turned on in local variation.
        * @param aBits bits to check
        * @return ETrue if turned on
        */
        TBool CheckBits( const TInt aBits );

        /**
        * Returns video quality frame rate configuration.
        * @return Reference to video quality frame rate
        * configuration.
        */
        const TVtEngVideoQualityFrameRateConfig&
            VideoQualityFrameRateConfig() const;

        /**
        * Resets settings to default values.
        */
        void ResetAll();

        /**
        * Registers interest on setting change.
        * @param aId event to observe for
        * @param aObserver setting observer
        */
        void NotifyChangeL(
            TSettingId aId,
            MVtEngSettingObserver& aObserver );

        /**
        * Cancels observering setting changes.
        * @param aId event to cancel
        * @param aObserver setting observer
        */
        void CancelNotifyChange(
            TSettingId aId,
            const MVtEngSettingObserver& aObserver );

		/**
        * Informs setting is dataport loaned or not yet. 
        * @param aLoaned TBool value is loaned (ETrue) or not (EFalse)
        */
		void SetDataportLoaned( TBool aLoaned );
		
		/**
        * Checks if still image exists. 
        * @return ETrue if still image exists.
        */
		TBool IsDefaultStillImageDefined();
		
		
		
		/**
		* Sets CLI. 
		* @since S60 5.0
		* @params aCLI call id and address (MSISDN) availability
		*/
		void SetCLI( const MVtEngSessionInfo::TCLI& aCLI );
		
		/**
		* Returns CLI
		* @since S60 5.0
		* @param aCLI call id and address (MSISDN) availability
		* @return ETrue if CLI is valid (stored in the first place)
		*/
		TBool GetCLI( MVtEngSessionInfo::TCLI& aCLI ) const;

        /**
		* Sets dataport info
		* @since S60 5.0
		* @param aPortInfo port name and number as string
		*/
		void SetDataportInfoL( const TDesC& aPortInfo );
		
		/**
		* Returns dataport info or NULL if not available yet
		* @since S60 5.0
		* @return dataport info
		*/
		const TDesC* DataportInfo() const;
		
	public:  // From MVtEngCRSettingObserver.

        /**
        * Callback to notify changed setting.
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue
        */
        void HandleNotifyInt( const TUid aUid, const TUint32 aId,
            TInt aNewValue );

        /**
        * Callback to notify changed setting.
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue
        */
        void HandleNotifyReal( const TUid aUid, const TUint32 aId,
            TReal aNewValue );

        /**
        * Callback to notify changed setting.
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue
        */
        void HandleNotifyString( const TUid aUid, const TUint32 aId,
            const TDesC16& aNewValue );

    private: // New functions.
        struct TObserverItem
            {
            // Callback interface.
            MVtEngSettingObserver* iObserver;
            // UID of item.
            TUid                   iUid;
            // Key of observer.
            TPtrC                  iKey;
            // Integer key, used with CR.
            TInt                   iIntKey;
            // Settings internal id.
            TSettingId             iId;
            };

        /**
        * Finds observer item.
        * @param aUid
        * @param aKey
        * @return observer item or NULL
        */
        TObserverItem* FindItem( const TUid& aUid, const TDesC& aKey ) const;

        /**
        * Finds observer item.
        * @param aUid
        * @param aKey
        * @return observer item or NULL
        */
        TObserverItem* FindItem( const TUid& aUid, const TInt& aKey ) const;

        /**
        * Returns value as integer.
        * @param aValue value to convert
        * @return value as integer
        */
        static TInt SettingAsTInt( const TDesC& aValue );

        /**
        * Derived class for internal book keeping. Engine knows whether
        * CLI has been received from call handling
        */
        struct TVtEngCLI : public MVtEngSessionInfo::TCLI
            {
            // Set to True when CLI received from call handling.
            TBool iValid;
            };
            
    private:

        /**
        * C++ default constructor.
        */
        CVtEngSettings();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Callback function to inform the observer.
        * @param aAny pointer to instance of this class.
        * @return KErrNone always.
        */
        static TInt DoInformSelectVolumeObserver( TAny* aAny );


        /**
        * Parses VQ frame rates read from cenrep.
        */
        void ParseFrameRates( TVtEngVideoQualityFrameRateConfig& aVQFR,
            const TDesC& aBuffer );

    private:    // Data
        // Central Repository proxy.
        CVtEngCRProxy* iCRProxy;

        // Settings data.
        TVtEngVideoCallConfig iConfig;

        // Setting data observers.
        CArrayFixFlat<TObserverItem>* iObserverItems;

        // File system handle.
        RFs iFs;

        // Owned idle callback instance.
        CIdle* iSelectVolumeIdle;

        // Observer for ESelectVolume.
        MVtEngSettingObserver* iSelectVolumeObserver;
        
        // Observer for EDataportInfo
        MVtEngSettingObserver* iDataportObserver;

        // Locally variated bits are read to this.
        TInt iVariantReadOnlyValues;

        // If local variations are read
        TBool iVariationsRead;

        // Video quality frame rate configuration holder
        TVtEngVideoQualityFrameRateConfig iVQFRConfig;
        
        // If default still image is defined
        TBool iDefaultStillImageDefined;
        
        // CLI information of video call. 
        TVtEngCLI iCLI;
        
        HBufC* iDataportInfo;
    };

#include    "CVtEngSettings.inl"

#endif      // CVTENGSETTINGS_H

// End of File
