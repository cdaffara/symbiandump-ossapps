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

#ifndef C_RADIOENGINESETTINGS_H
#define C_RADIOENGINESETTINGS_H

// User includes
#include "mradioenginesettings.h"
#include "mradiosettingssetter.h"
#include "cradioregion.h"

// Forward declarations
class MRadioSettingsObserver;
class CRepository;
class CRadioSettingsImp;

// Constants

/**
 * Maximum size of the network country code. 
 * From CTelephony::TNetworkInfoV1 in etel3rdparty.h
 */
enum { KNetworkCountryCodeSize  = 4  };

/**
 * Maximum size of the network identity.
 * From CTelephony::TNetworkInfoV1 in etel3rdparty.h
 */
enum { KNetworkIdentitySize     = 8  };

/**
 * Concrete implementation of radio settings.
 */
NONSHARABLE_CLASS( CRadioEngineSettings ) : public CBase
                                          , public MRadioSettingsSetter
    {

public:

    static CRadioEngineSettings* NewL( CRadioSettingsImp& aRadioSettingsImp );

    ~CRadioEngineSettings();

    /**
     * Determines if region is allowed.
     */
    TBool IsRegionAllowed( TRadioRegion aRegionId ) const;

private:

    CRadioEngineSettings( CRadioSettingsImp& aRadioSettingsImp );

    void ConstructL();

// from base class MRadioEngineSettings

    TInt HeadsetVolume() const;
    TInt SpeakerVolume() const;
    TInt Volume() const;
    TInt OutputMode() const;
    TInt AudioRoute() const;
    TBool IsHeadsetVolMuted() const;
    TBool IsSpeakerVolMuted() const;
    TBool IsVolMuted() const;
    TBool IsPowerOn() const;
    TUint32 TunedFrequency() const;
    TInt DefaultMinVolumeLevel() const;
    TInt CountRegions() const;
    CRadioRegion& Region( TInt aIndex ) const;
    TRadioRegion RegionId() const;
    TUint32 FrequencyStepSize() const;
    TUint32 MaxFrequency() const;
    TUint32 MinFrequency() const;
    TInt DecimalCount() const;
    TRadioRegion DefaultRegion() const;
    TPtrC NetworkId() const;
    TPtrC CountryCode() const;

// from base class MRadioSettingsSetter

    void SetObserver( MRadioSettingsObserver* aObserver );
    TInt SetHeadsetVolume( TInt aVolume );
    TInt SetSpeakerVolume( TInt aVolume );
    TInt SetVolume( TInt aVolume );
    TInt SetOutputMode( TInt aOutputMode );
    TInt SetAudioRoute( TInt aAudioRoute );
    TInt SetHeadsetVolMuted( TBool aMuted );
    TInt SetSpeakerVolMuted( TBool aMuted );
    TInt SetVolMuted( TBool aMuted );
    TInt SetPowerOn( TBool aPowerState );
    TInt SetTunedFrequency( TUint32 aFrequency );
    TInt SetRegionId( TInt aRegion );
    TInt SetNetworkId( const TDesC& aNetworkId );
    TInt SetCountryCode( const TDesC& aCountryCode );

// New functions

    /**
     * Initializes the data holders
     */
    void InitializeDataHolders();

    /**
     * Gets a repository value and sets the default value if the key is not found
     */
    TBool GetRepositoryValue( int aKey, TInt& aValue, TInt aDefault );
    
    /**
     * Sets a boolean value to cenrep and data holder
     */
    TInt SetFlagValue( int aKey, int aFlagId, TInt aValue );
    
    /**
     * Initializes the regions as defined in the resource file.
     */
    void InitializeRegionsL();
    
    /**
     * Converts Region ID to index
     *
     * @param aRegionId The region ID that is wanted to convert
     * @return The region index, KErrNotFound if no match found
     */
    TInt RegionIndexForId( TInt aRegionId ) const;

    /**
     * Updates the current region index
     *
     * @param aRegionId The new region ID
     */
     void UpdateCurrentRegionIdx( TInt aRegionId );

private: // data

    /**
     * The observer for the setting changes.
     * Not own.
     */
    MRadioSettingsObserver*         iObserver;

    /**
     * The available regions.
     */
    RRadioRegionArray               iRegions;

    /**
     * The current region.
     */
    TInt                            iCurrentRegionIdx;
    
    CRadioSettingsImp&              iRadioSettingsImp;

    RResourceFile                   iResFile;

    CRepository*                    iRepository;

    // Data holders for quick access   
    TBitFlags                       iFlags;
    TBuf<KNetworkIdentitySize>      iNetworkId;
    TBuf<KNetworkCountryCodeSize>   iCountryCode;
    TInt                            iHeadsetVolume;
    TInt                            iSpeakerVolume;
    TUint32                         iFrequency;
    TInt                            iDefaultMinVolume;
    TRadioRegion                    iDefaultRegion;
    TRadioRegion                    iRegionId;

    };

#endif // C_RADIOENGINESETTINGS_H
