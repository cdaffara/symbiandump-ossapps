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

// System includes
#include <barsread.h>
#include <coemain.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <fmradioenginesettings.rsg>
#include <centralrepository.h>

// User includes
#include "radiointernalcrkeys.h"
#include "radioenginedef.h"
#include "cradioenginesettings.h"
#include "cradiosettingsimp.h"
#include "mradiosettingsobserver.h"
#include "cradioenginelogger.h"

// This has to be the last include.
#ifdef STUB_CONSTELLATION
#   include <RadioStubManager.h>
#endif //STUB_CONSTELLATION


 /**
  * On/Off type flags used by TBitFlags.
  */
 enum TFlagValues
     {
      EHeadsetVolMuted      = 1 << 0
     ,ESpeakerVolMuted      = 1 << 1
     ,EPowerOn              = 1 << 2
     ,EStereoOutput         = 1 << 3
     ,EHeadsetAudioRoute    = 1 << 4
     };

// ======== LOCAL FUNCTIONS ========

 // ---------------------------------------------------------------------------
 // Normalize return value from TBitFlags to 1 or 0 just in case
 // Two negations: 16 => 0 => 1
 // ---------------------------------------------------------------------------
 //
static TBool NormalizeBool( TBool aBool )
    {
    // Normalize return value from TBitFlags to 1 or 0 just in case
    // Two negations: 16 => 0 => 1
    return !( !aBool );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings* CRadioEngineSettings::NewL( CRadioSettingsImp& aRadioSettingsImp )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioEngineSettings* self = new ( ELeave ) CRadioEngineSettings( aRadioSettingsImp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRepository = CRepository::NewL( KRadioCRUid );

    InitializeRegionsL();

    InitializeDataHolders();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings::CRadioEngineSettings( CRadioSettingsImp& aRadioSettingsImp )
    : iRadioSettingsImp( aRadioSettingsImp )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings::~CRadioEngineSettings()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRegions.ResetAndDestroy();
    iRegions.Close();
    iResFile.Close();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::SetObserver( MRadioSettingsObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetHeadsetVolume( TInt aVolume )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt err = iRepository->Set( KRadioCRHeadsetVolume, aVolume );
    if ( !err )
        {
        iHeadsetVolume = aVolume;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::HeadsetVolume() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iHeadsetVolume;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetSpeakerVolume( TInt aVolume )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt err = iRepository->Set( KRadioCRSpeakerVolume, aVolume );
    if ( !err )
        {
        iSpeakerVolume = aVolume;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SpeakerVolume() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSpeakerVolume;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetVolume( TInt aVolume )
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return SetHeadsetVolume( aVolume );
        }
    else
        {
        return SetSpeakerVolume( aVolume );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::Volume() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return HeadsetVolume();
        }
    else
        {
        return SpeakerVolume();
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetOutputMode( TInt aOutputMode )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt err = iRepository->Set( KRadioCROutputMode, aOutputMode );
    if ( !err )
        {
        iFlags.Assign( EStereoOutput, aOutputMode == RadioEngine::ERadioStereo );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::OutputMode() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iFlags.IsSet( EStereoOutput ) ? RadioEngine::ERadioStereo : RadioEngine::ERadioMono;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// Stores the radio audio route ( headset/ihf ) to use.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetAudioRoute( TInt aAudioRoute )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt err = iRepository->Set( KRadioCRAudioRoute, aAudioRoute );
    if ( !err )
        {
        iFlags.Assign( EHeadsetAudioRoute, aAudioRoute == RadioEngine::ERadioHeadset );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// Retrieves the radio audio route in use.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::AudioRoute() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iFlags.IsSet( EHeadsetAudioRoute ) ? RadioEngine::ERadioHeadset : RadioEngine::ERadioSpeaker;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetHeadsetVolMuted( TBool aMuted )
    {
    LEVEL3( LOG_METHOD_AUTO );
    return SetFlagValue( KRadioCRHeadsetMuteState, EHeadsetVolMuted, aMuted );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsHeadsetVolMuted() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return NormalizeBool( iFlags.IsSet( EHeadsetVolMuted ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetSpeakerVolMuted( TBool aMuted )
    {
    LEVEL3( LOG_METHOD_AUTO );
    return SetFlagValue( KRadioCRSpeakerMuteState, ESpeakerVolMuted, aMuted );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsSpeakerVolMuted() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return NormalizeBool( iFlags.IsSet( ESpeakerVolMuted ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetVolMuted( TBool aMuted )
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return SetHeadsetVolMuted( aMuted );
        }
    else
        {
        return SetSpeakerVolMuted( aMuted );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsVolMuted() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return IsHeadsetVolMuted();
        }
    else
        {
        return IsSpeakerVolMuted();
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetPowerOn( TBool aPowerState )
    {
    LEVEL3( LOG_METHOD_AUTO );
    return SetFlagValue( KRadioCRRadioPowerState, EPowerOn, aPowerState );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsPowerOn() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return NormalizeBool( iFlags.IsSet( EPowerOn ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetTunedFrequency( TUint32 aFrequency )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt err = iRepository->Set( KRadioCRTunedFrequency, static_cast<TInt>( aFrequency ) );
    if ( !err )
        {
        iFrequency = aFrequency;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::TunedFrequency() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iFrequency;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::DefaultMinVolumeLevel() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iDefaultMinVolume;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::CountRegions() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRegions.Count();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
CRadioRegion& CRadioEngineSettings::Region( TInt aIndex ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( aIndex == KErrNotFound )
        {
        aIndex = RegionIndexForId( DefaultRegion() );
        }

    return *iRegions[aIndex];
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineSettings::RegionId() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRegionId;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineSettings::DefaultRegion() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iDefaultRegion;
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioEngineSettings::NetworkId() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return TPtrC( iNetworkId );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioEngineSettings::CountryCode() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return TPtrC( iCountryCode );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetRegionId( TInt aRegion )
    {
    LEVEL3( LOG_METHOD_AUTO );
    __ASSERT_ALWAYS( RegionIndexForId( aRegion ) != KErrNotFound,
                     User::Panic( _L( "CRadioEngineSettings" ), KErrArgument ) );

    TInt err = iRepository->Set( KRadioCRCurrentRegion, aRegion );
    if ( !err )
        {
        iRegionId = static_cast<TRadioRegion>( aRegion );
        UpdateCurrentRegionIdx( aRegion );
        err = SetTunedFrequency( Region( iCurrentRegionIdx ).MinFrequency() );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetNetworkId( const TDesC& aNetworkId )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iNetworkId.Copy( aNetworkId.Left( iNetworkId.MaxLength() ) );
    return iRepository->Set( KRadioCRNetworkId, iNetworkId );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetCountryCode( const TDesC& aCountryCode )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iCountryCode.Copy( aCountryCode.Left( iCountryCode.MaxLength() ) );
    return iRepository->Set( KRadioCRCountryCode, iCountryCode );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::FrequencyStepSize() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return Region( iCurrentRegionIdx ).StepSize();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::MaxFrequency() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return Region( iCurrentRegionIdx ).MaxFrequency();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::MinFrequency() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return Region( iCurrentRegionIdx ).MinFrequency();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::DecimalCount() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return Region( iCurrentRegionIdx ).DecimalCount();
    }

// ---------------------------------------------------------------------------
// Initializes the data holders
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::InitializeDataHolders()
    {
    TInt temp = 0;
    GetRepositoryValue( KRadioCRHeadsetMuteState, temp, EFalse );
    iFlags.Assign( EHeadsetVolMuted, temp );

    temp = RadioEngine::ERadioHeadsetDefaultVolume;
    GetRepositoryValue( KRadioCRHeadsetVolume, temp, temp );
    iHeadsetVolume = temp;

    temp = 0;
    GetRepositoryValue( KRadioCRSpeakerMuteState, temp, EFalse );
    iFlags.Assign( ESpeakerVolMuted, temp );

    temp = RadioEngine::ERadioSpeakerDefaultVolume;
    GetRepositoryValue( KRadioCRSpeakerVolume, temp, temp );
    iSpeakerVolume = temp;

    temp = 0;
    GetRepositoryValue( KRadioCRRadioPowerState, temp, EFalse );
    iFlags.Assign( EPowerOn, temp );

    temp = RadioEngine::ERadioStereo;
    if ( GetRepositoryValue( KRadioCROutputMode, temp, temp ) )
        {
        iFlags.Assign( EStereoOutput, temp == RadioEngine::ERadioStereo );
        }

    temp = RadioEngine::ERadioHeadset;
    if ( GetRepositoryValue( KRadioCRAudioRoute, temp, temp ) )
        {
        iFlags.Assign( EHeadsetAudioRoute, temp == RadioEngine::ERadioHeadset );
        }

    temp = RadioEngine::ERadioDefaultMinVolume;
    GetRepositoryValue( KRadioCRDefaultMinVolumeLevel, temp, temp );
    iDefaultMinVolume = temp;

    temp = ERadioRegionDefault;
    GetRepositoryValue( KRadioCRDefaultRegion, temp, temp );
    iDefaultRegion = static_cast<TRadioRegion>( temp );

    temp = iDefaultRegion;
    GetRepositoryValue( KRadioCRCurrentRegion, temp, temp );
    iRegionId = static_cast<TRadioRegion>( temp );
    UpdateCurrentRegionIdx( iRegionId );

    temp = MinFrequency();
    GetRepositoryValue( KRadioCRTunedFrequency, temp, temp );
    iFrequency = static_cast<TUint32>( temp );
    }

// ---------------------------------------------------------------------------
// Gets a repository value and sets the default value if the key is not found
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::GetRepositoryValue( int aKey, TInt& aValue, TInt aDefault )
    {
    TInt temp = 0;
    TInt err = iRepository->Get( aKey, temp );
    if ( !err )
        {
        aValue = temp;
        return ETrue;
        }
    else if ( err == KErrNotFound )
        {
        if ( iRepository->Set( aKey, aDefault ) == KErrNone )
            {
            aValue = aDefault;
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Sets a boolean value to cenrep and data holder
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetFlagValue( int aKey, int aFlagId, TInt aValue )
    {
    TInt err = iRepository->Set( aKey, aValue );
    if ( !err )
        {
        iFlags.Assign( aFlagId, aValue );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Initializes the regions based on resources.
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::InitializeRegionsL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRegions.ResetAndDestroy();

    TResourceReader reader;

    TFileName resourceFileName;
    resourceFileName.Append( KRadioSettingsResourceFile );

    //User::LeaveIfError( iRadioSettingsImp.FsSession().Connect() );

    //TODO: Remove the ResolveDriveL
    iRadioSettingsImp.ResolveDriveL( resourceFileName, KDC_RESOURCE_FILES_DIR );
    BaflUtils::NearestLanguageFile( iRadioSettingsImp.FsSession(), resourceFileName);

    iResFile.OpenL( iRadioSettingsImp.FsSession(), resourceFileName);

    iResFile.ConfirmSignatureL();
    HBufC8* readBuffer = iResFile.AllocReadLC(R_QRAD_REGIONS);
    reader.SetBuffer(readBuffer);
    TInt regionCount = reader.ReadInt16();

    for ( TInt i = 0; i < regionCount; ++i )
        {
        TInt resId = reader.ReadInt32(); // The next resource ID to read.
        HBufC8* regionBuffer = iResFile.AllocReadLC(resId);
        TResourceReader regionReader;
        regionReader.SetBuffer(regionBuffer);

        CRadioRegion* region = CRadioRegion::NewL( regionReader );
        CleanupStack::PushL( region );

        if ( IsRegionAllowed( region->Id() ))
            {
            User::LeaveIfError( iRegions.Append( region ) );
            CleanupStack::Pop( region );
            }
        else{
            CleanupStack::PopAndDestroy( region );
            }
        CleanupStack::PopAndDestroy(regionBuffer);
        }

    if ( CountRegions() <= 0 || !IsRegionAllowed( DefaultRegion() ) )
        {
        User::Leave( KErrCorrupt );
        }

    UpdateCurrentRegionIdx( RegionId() );
    CleanupStack::PopAndDestroy(readBuffer);
    }

// ---------------------------------------------------------------------------
// Converts Region ID to index
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::RegionIndexForId( TInt aRegionId ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt idx = KErrNotFound;
    for ( TInt i = 0 ; i < CountRegions(); ++i )
        {
        if ( Region( i ).Id() == static_cast<TRadioRegion>( aRegionId ) )
            {
            idx = i;
            break;
            }
        }
    return idx;
    }

// ---------------------------------------------------------------------------
// Updates the current region
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::UpdateCurrentRegionIdx( TInt aRegionId )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iCurrentRegionIdx = RegionIndexForId( aRegionId );
    }

// ---------------------------------------------------------------------------
// Determines if region is allowed .
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsRegionAllowed( TRadioRegion aRegionId ) const
    {
    LEVEL3( LOG_METHOD_AUTO );

    TInt regionAllowed( ETrue );
    if ( ERadioRegionJapan == aRegionId )
        {
        TInt err = iRepository->Get( KRadioCRRegionAllowedJapan, regionAllowed );
        if ( err != KErrNone )
            {
            // In case the key is not found or otherwise unsuccessfully read,
            // Japan region is allowed by default.
            regionAllowed = ETrue;
            }
        }

    return TBool( regionAllowed );
    }
