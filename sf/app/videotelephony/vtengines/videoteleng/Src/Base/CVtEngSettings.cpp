/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video call setting data
*
*/



// INCLUDE FILES
#include    "CVtEngSettings.h"
#include    <cvtlogger.h>
#include    "MVtEngSettingObserver.h"
#include    "VtEngUtils.h"
#include    "VtEngPanic.h"
#include    <telincallvolcntrlcrkeys.h>
#include    <rphcltserver.h>
#include    <cphcltimagehandler.h>
#include    <videotelephonyinternalcrkeys.h>
#include    <LogsDomainCRKeys.h>
#include    <settingsinternalcrkeys.h>
#include    <videotelephonyvariant.hrh>

// CONSTANTS
// Granularity for observer array.
const TInt KVtEngObserverArrayGranularity = 2;

// Volume default level.
const TInt KVtEngVolumeDefaultLevel = 4;

// Video quality frame rate configuration buffer size
const TInt KVtEngVQFRBufferSize = 32;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngSettings::CVtEngSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngSettings::CVtEngSettings() :
    iVariantReadOnlyValues( KErrNotReady )
    {
    iCLI.iValid = EFalse;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngSettings::ConstructL()
    {
    __VTPRINTENTER( "Settings.ConstructL" )
    iSelectVolumeIdle = CIdle::NewL( CActive::EPriorityHigh );
    iObserverItems = new ( ELeave ) 
        CArrayFixFlat<TObserverItem>( KVtEngObserverArrayGranularity );

    // Ear volume CR listener.
    iCRProxy = &CVtEngUtility::CRProxy();
#if 0
    CreateDataObserverL( this, KCRUidInCallVolume, KTelIncallEarVolume, 
        CCenRepNotifyHandler::EIntKey );   
    
    // IHF volume listener.
    CreateDataObserverL( 
        this, KCRUidInCallVolume, KTelIncallLoudspeakerVolume,
        CCenRepNotifyHandler::EIntKey );
#endif    
    // Call duration listener.
    CreateDataObserverL( this, KCRUidLogs, KLogsShowCallDuration, 
        CCenRepNotifyHandler::EIntKey );

    // Still image listener.
    CreateDataObserverL( this, KCRUidTelephonySettings, KSettingsVTStillImage,
        CCenRepNotifyHandler::EIntKey );

    // Still image path listener.
    CreateDataObserverL( 
        this, KCRUidTelephonySettings, KSettingsVTStillImagePath,
        CCenRepNotifyHandler::EStringKey );

    ReadVariationsL();
    ResetAll();


    TInt validImage( 0 );
    TInt err = iCRProxy->Get( KCRUidTelephonySettings, 
        KSettingsVTStillImage, validImage );
    __VTPRINT3( DEBUG_DETAIL, "Settings.ConL: use image", err, validImage )
    if ( !err && validImage )
        {
        __VTPRINT( DEBUG_DETAIL, "Settings.ConL: VALID" )
        iConfig.iVideo.iImageIsValid = ETrue;
        }

    TInt value( 0 ); // not shown
    err = iCRProxy->Get( KCRUidLogs, KLogsShowCallDuration, value );
    if ( err == KErrNone )
        {
        iConfig.iCallTimerOn = ( value == 1 ) ? ETrue : EFalse;
        }
    
    // Initialize the volume levels.
    // If value retrieval fails, use defaults.
    if ( iCRProxy->Get( KCRUidInCallVolume, 
        KTelIncallEarVolume, iConfig.iAudio.iVolume.iHandsetVolume ) 
        != KErrNone )
        {
        iConfig.iAudio.iVolume.iHandsetVolume = KVtEngVolumeDefaultLevel;
        }

    if ( iCRProxy->Get( KCRUidInCallVolume, KTelIncallLoudspeakerVolume, 
        iConfig.iAudio.iVolume.iHandsfreeVolume ) != KErrNone )       
        {
        iConfig.iAudio.iVolume.iHandsfreeVolume = KVtEngVolumeDefaultLevel;
        }

    // Open connection to phone server
    RPhCltServer phoneClient;
    User::LeaveIfError( phoneClient.Connect() );
    CleanupClosePushL( phoneClient );
	
	CPhCltImageHandler* stillH;
	stillH = CPhCltImageHandler::NewL();
    CleanupStack::PushL( stillH );

	// Load images
	RFile file;
	TInt res = stillH->OpenDefaultVtImage( file );
	file.Close();
	iDefaultStillImageDefined = ( res == KErrNone ) ? ETrue : EFalse;
     // Cleanup
	CleanupStack::PopAndDestroy( 2 ); // phoneClient, stillH, imageParams

    __VTPRINTEXIT( "Settings.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngSettings* CVtEngSettings::NewL()
    {
    CVtEngSettings* self = new( ELeave ) CVtEngSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::~CVtEngSettings
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngSettings::~CVtEngSettings()
    {
    __VTPRINTENTER( "Settings.~" )
    delete iSelectVolumeIdle;
    delete iObserverItems;
    delete iDataportInfo;
    __VTPRINTEXIT( "Settings.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::Config
// Returns configuration.
// -----------------------------------------------------------------------------
//
const CVtEngSettings::TVtEngVideoCallConfig& CVtEngSettings::Config() const
    {
    return iConfig;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetCurrentVolume
// -----------------------------------------------------------------------------
//
void CVtEngSettings::SetCurrentVolume( TBool aVolume )
    {
    TBool old = iConfig.iAudio.iCurrentVolume;
    iConfig.iAudio.iCurrentVolume = aVolume;

    if ( iSelectVolumeObserver && ( !old && aVolume ) || ( old && !aVolume ) )
        {
        iSelectVolumeIdle->Cancel();
        iSelectVolumeIdle->Start(
            TCallBack( DoInformSelectVolumeObserver, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::CurrentVolume
// -----------------------------------------------------------------------------
//
TBool CVtEngSettings::CurrentVolume() const
    {
    return iConfig.iAudio.iCurrentVolume;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetVolume
// Sets output volue setting.
// -----------------------------------------------------------------------------
//
TInt CVtEngSettings::SetVolume( 
        const TInt aHandsetVolume, 
        const TInt aHandsfreeVolume,
        const TBool aInternal )
    {
    __VTPRINTENTER( "Settings.SetVolume" )
    __VTPRINT3( DEBUG_DETAIL, "Settings:SetVolume hs=%d, ihf=%d", 
        aHandsetVolume, aHandsfreeVolume  )
    __VTPRINT2( DEBUG_DETAIL, "Settings:Internal=%d", aInternal )
    
    TInt volume ( 0 );    

    TInt res( KErrNone );
    if ( aInternal )
        {
        if ( iConfig.iAudio.iVolume.iHandsfreeVolume != aHandsfreeVolume )
            {
            iConfig.iAudio.iVolume.iHandsfreeVolume = 
                ValidVolume( aHandsfreeVolume );
            }
        if ( iConfig.iAudio.iVolume.iHandsetVolume != aHandsetVolume )
            {
            iConfig.iAudio.iVolume.iHandsetVolume = 
                ValidVolume( aHandsetVolume );
            }
        }
    else
        {        
	    iCRProxy->Get( KCRUidInCallVolume, 
	        KTelIncallEarVolume, volume );
	    if ( volume != aHandsetVolume ) 
	        {
	        iCRProxy->Set( KCRUidInCallVolume, 
	            KTelIncallEarVolume, aHandsetVolume );
	        }
	        
	    iCRProxy->Get( KCRUidInCallVolume, 
	        KTelIncallLoudspeakerVolume, volume );
	    if ( volume != aHandsfreeVolume )
	        {
	        iCRProxy->Set( KCRUidInCallVolume, 
	            KTelIncallLoudspeakerVolume, aHandsfreeVolume );         
	        }
        }
    __VTPRINTEXITR( "Settings.SetVolume res=%d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::GetVolume
// Gets audio volume.
// -----------------------------------------------------------------------------
//
TInt CVtEngSettings::GetVolume(
    TInt& aVolume, 
    const TBool aHandsfree,
    const TBool aInternal ) const
    {
    __VTPRINTENTER( "Settings.GetVolume" )
    TInt res( KErrNone );
    if ( aInternal )
        {
        if ( aHandsfree )
            {
            aVolume = iConfig.iAudio.iVolume.iHandsfreeVolume;
            }
        else
            {
            aVolume = iConfig.iAudio.iVolume.iHandsetVolume;
            }
        }
    else
        {
        if ( aHandsfree )
            {          
            res = iCRProxy->Get( KCRUidInCallVolume,
                KTelIncallLoudspeakerVolume, aVolume );
            }
        else
            {
            res = iCRProxy->Get( KCRUidInCallVolume, 
                KTelIncallEarVolume, aVolume );              
            }
        }
    __VTPRINTEXITR( "Settings.GetVolume res=%d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetRouting
// Sets audio routing setting.
// -----------------------------------------------------------------------------
//
void CVtEngSettings::SetRouting( 
    const MVtEngAudio::TVtEngRoutingSetting& aState )
    {
    iConfig.iAudio.iRouting = aState;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetVideoEnabled
// Sets video enabled setting.
// -----------------------------------------------------------------------------
//
void CVtEngSettings::SetVideoEnabled( const TBool aEnabled )
    {
    __VTPRINT2( DEBUG_GEN , "Settings.SVE",aEnabled )
    iConfig.iVideo.iVideoEnabled = aEnabled;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetConnectReady
// -----------------------------------------------------------------------------
//
void CVtEngSettings::SetConnectReady()
    {
    iConfig.iReadyForConnect = ETrue;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetDataportLoaned
// Sets dataport loaning status
// 
// -----------------------------------------------------------------------------
//
void CVtEngSettings::SetDataportLoaned( TBool aLoaned )
	{
	__VTPRINTENTER( "Settings.SetDataportLoaned" )
	iConfig.iIsDataportLoaned = aLoaned;
	__VTPRINTEXIT( "Settings.SetDataportLoaned" )
	}

// -----------------------------------------------------------------------------
// CVtEngSettings::ResetAll
// Reset all settings.
// -----------------------------------------------------------------------------
//
void CVtEngSettings::ResetAll()
    {
    __VTPRINT( DEBUG_GEN , "Settings.RA" )
    iConfig.iAudio.iRouting = KVtEngDefaultAudioRouting;
    iConfig.iVideo.iVideoEnabled = ETrue;
    iConfig.iReadyForConnect = EFalse;
	iConfig.iIsDataportLoaned = EFalse;
    // call duration setting not cleared
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::NotifyChangeL
// -----------------------------------------------------------------------------
//
void CVtEngSettings::NotifyChangeL( 
            TSettingId aId,
            MVtEngSettingObserver& aObserver )
    {
    __VTPRINTENTER( "Settings.NotifyChangeL" )
    __VTPRINT2( DEBUG_GEN, "Settings.NC id %d", aId )
    switch ( aId )
        {
        case ESelectVolume:
            iSelectVolumeObserver = &aObserver;
            break;
        case EDataportInfo:
            iDataportObserver = &aObserver;
            break;
        // others are CenRep items
        default:
        {
        const TInt count( iObserverItems->Count() );
        for ( TInt index = 0; index < count; index++ )
            {
            TObserverItem& item = iObserverItems->At( index );
            if ( item.iId == aId && item.iObserver == &aObserver )
                {
                __VTPRINT( DEBUG_GEN, "Settings.NC.already" )
                __VTPRINTEXIT( "Settings.NotifyChangeL" )
                return;
                }
            }

        TObserverItem item;
        item.iId = aId;
        switch ( aId )
            {
            case EStillImage:
                item.iIntKey = KSettingsVTStillImage;
                item.iUid = KCRUidTelephonySettings;
                break;
            case EStillImagePath:
                item.iIntKey = KSettingsVTStillImagePath;
                item.iUid = KCRUidTelephonySettings;
                break;
            case ECallTimer:
                item.iIntKey = KLogsShowCallDuration;
                item.iUid = KCRUidLogs;
                break;
            case EHandsetVolume:           
                item.iIntKey = KTelIncallEarVolume;
                item.iUid = KCRUidInCallVolume;
                break;
            case EHandsfreeVolume:
                item.iIntKey = KTelIncallLoudspeakerVolume;
                item.iUid = KCRUidInCallVolume;              
                break;
            default:
                break;
            }
        item.iObserver = &aObserver;
        iObserverItems->AppendL( item );
        }
        }
    __VTPRINTEXIT( "Settings.NotifyChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::CancelNotifyChange
// Cancels receiving change events.
// -----------------------------------------------------------------------------
//
void CVtEngSettings::CancelNotifyChange( 
    TSettingId aId, const MVtEngSettingObserver& aObserver )
    {
    __VTPRINTENTER( "Settings.CancelNotifyChange" )
    if ( aId == ESelectVolume )
        {
        if ( &aObserver == iSelectVolumeObserver )
            {
            iSelectVolumeObserver = NULL;
            }
        }
    else if ( &aObserver == iDataportObserver )
        {
        iDataportObserver = NULL;
        // also free space used for dataport info
        delete iDataportInfo;
        iDataportInfo = NULL;
        }
    else
        {
        TInt count( iObserverItems->Count() );
        while ( count-- )
            {
            const TObserverItem& item = iObserverItems->At( count );
            if ( item.iId == aId &&
                item.iObserver == &aObserver )
                {
                iObserverItems->Delete( count );
                count = 0; // breaks loop
                }
            }
        }
    __VTPRINTEXIT( "Settings.CancelNotifyChange" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CVtEngSettings::HandleNotifyInt( const TUid aUid, 
    const TUint32 aId, TInt aNewValue )
    {
    TBool informObserver = ETrue;       
    if ( aUid == KCRUidInCallVolume )   
        {
        switch ( aId )
            {
            case KTelIncallLoudspeakerVolume:
                {
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: HF volume" )
                if ( iConfig.iAudio.iVolume.iHandsfreeVolume == aNewValue )
                    {
                    informObserver = EFalse;
                    }
                else
                    {                
                    iConfig.iAudio.iVolume.iHandsfreeVolume = aNewValue;
                    }
                }
                break;
            case KTelIncallEarVolume:           
                {
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: HandsetVolume" )
                if ( iConfig.iAudio.iVolume.iHandsetVolume == aNewValue )
                    {
                    informObserver = EFalse;
                    }
                else
                    {
                    iConfig.iAudio.iVolume.iHandsetVolume = aNewValue;
                    }
                }
                break;
            default:
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: Non-ordered" );
                break;
            }
        }
    else if ( aUid == KCRUidLogs )
        {
        switch ( aId )
            {
            case KLogsShowCallDuration:
                {
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: call duration" )
                iConfig.iCallTimerOn = aNewValue > 0 ? ETrue : EFalse;
                }
                break;
            default:
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: Non-ordered" );
                break;
            }
        }
    else if ( aUid == KCRUidTelephonySettings )
        {
        switch ( aId )
            {
            case KSettingsVTStillImage:
                {
                __VTPRINT( DEBUG_GEN, "HandleNotifyInt still" )
                if ( aNewValue == 0 )
                    {
                    __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt disabled")
                    // still image disabled in settings
                    iConfig.iVideo.iStillImageFile.Zero();
                    iConfig.iVideo.iImageIsValid = EFalse;
                    }
                else
                    {
                    __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt enabled")
                    iConfig.iVideo.iImageIsValid = ETrue;
                    }
                }
                break;
            default:
                __VTPRINT( DEBUG_GEN, "Settings.HandleNotifyInt: Non-ordered" );
                break;
            }
        }

    TObserverItem* item = FindItem( aUid, aId );
    if ( informObserver && item && item->iObserver ) 
        {
        // The observer does not need the new value
        // Plain notification is all.
        TRAP_IGNORE( item->iObserver->HandleSettingChangedL( item->iId, KNullDesC16 ) );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::HandleNotifyReal
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSettings::HandleNotifyReal( const TUid /*aUid*/, 
    const TUint32 /*aId*/ , TReal /* aNewValue*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::HandleNotifyString
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSettings::HandleNotifyString( const TUid aUid, const TUint32 aId, 
    const TDesC16& aNewValue )
    {
    __VTPRINTENTER( "Settings.HN" )
    if ( aNewValue.Compare( iConfig.iVideo.iStillImageFile ) != 0 )
        {
        if( aId == KSettingsVTStillImagePath )
            {
            __VTPRINT( DEBUG_GEN, "Settings.HN  KGSVTStillImagePath" )
            iConfig.iVideo.iStillImageFile.Copy( aNewValue );
            }
        }
    TObserverItem* item = FindItem( aUid, aId );
    if ( item && item->iObserver ) 
        {
        TRAP_IGNORE( item->iObserver->HandleSettingChangedL( item->iId, aNewValue ) );
        }
    __VTPRINTEXIT( "Settings.HN" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::FindItem
// Finds observer item.
// -----------------------------------------------------------------------------
//
CVtEngSettings::TObserverItem* CVtEngSettings::FindItem( 
    const TUid& aUid, 
    const TDesC& aKey ) const
    {
    TInt count( iObserverItems->Count() );
    TObserverItem* item = NULL;
    TBool found( EFalse );
    while ( count-- && !found )
        {
        item = &iObserverItems->At( count );
        if ( item->iUid == aUid &&
             item->iKey.Compare( aKey ) == 0 )
            {
            found = ETrue;
            }
        }

    // Just NULL the item if not found
    if( !found )
        {
        item = NULL;
        }
    return item;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::FindItem
// Finds observer item.
// -----------------------------------------------------------------------------
//
CVtEngSettings::TObserverItem* CVtEngSettings::FindItem( 
    const TUid& aUid,const TInt& aKey ) const
    {
    TInt count( iObserverItems->Count() );
    TObserverItem* item = NULL;
    TBool found( EFalse );
    while ( count-- && !found )
        {
        item = &iObserverItems->At( count );
        if ( item->iUid == aUid &&
            item->iIntKey == aKey )
            {
            found = ETrue;
            }
        }

    // Just NULL the item if not found
    if( !found )
        {
        item = NULL;
        }
    return item;
    }
    
// -----------------------------------------------------------------------------
// CVtEngSettings::SettingAsTInt
// Returns setting as integer value.
// -----------------------------------------------------------------------------
//
TInt CVtEngSettings::SettingAsTInt( const TDesC& aValue )
    {
    TInt value( 0 );
    const TInt err = TLex( aValue ).Val( value );
    if ( err != KErrNone ) 
        {
        value = err;
        }
    return value;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::DoInformSelectVolumeObserver
// -----------------------------------------------------------------------------
//
TInt CVtEngSettings::DoInformSelectVolumeObserver( TAny* aAny )
    {
    CVtEngSettings* self = static_cast< CVtEngSettings* >( aAny );

    if ( self->iSelectVolumeObserver )
        {
        TRAP_IGNORE( self->iSelectVolumeObserver->HandleSettingChangedL( 
            ESelectVolume, 
            KNullDesC ) );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::ReadVariationsL
// Reads local variations.
// -----------------------------------------------------------------------------
//      
void CVtEngSettings::ReadVariationsL() 
    {
    __VTPRINTENTER( "Settings.ReadVariations" )
    User::LeaveIfError( iCRProxy->Get( KCRUidVTVariation, 
        KVTLocalVariationFlags, iVariantReadOnlyValues ));
    TBuf< KVtEngVQFRBufferSize > buffer;                    
   User::LeaveIfError( iCRProxy->Get( KCRUidVTConfiguration, 
        KVTVideoFrameRates, buffer ));
    ParseFrameRates( iVQFRConfig, buffer );             
    __VTPRINTEXIT( "Settings.ReadVariations" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::GetCameraOrientations
// Reads camera orientations from CR
// -----------------------------------------------------------------------------
//     

void CVtEngSettings::GetCameraOrientations( 
    MVtEngMedia::TCameraOrientation& aPrimaryCameraOrientation,
    MVtEngMedia::TCameraOrientation& aSecondaryCameraOrientation ) const
    {
    /*
    * Defines layout (screen orientation) used by 
    * Video Telephone application per active camera.
    * Each camera has one of the following values:
    * 1) "LS" = Landscape
    * 2) "PR" = Portrait
    * 3) "OL" = Obey layout in specific device mode (normal operation)
    * Camera values are:
    * 1) "C1" = VT primary camera (inwards)
    * 2) "C2" = VT secondary camera (outwards)
    * Example value (portrait for cam1, landscape for cam2):
    * C1:PR C2:LS
    */
    
    __VTPRINTENTER( "Settings.GetCameraOrientations" )
    const TInt KMaxKVTCameraLayoutValue = 11;
    const TInt KAbbreviationLength = 2;
    const TInt KSkipChars = 3;
    
    _LIT( KCameraOrientationLS, "LS" ); // landscape
    _LIT( KCameraOrientationPR, "PR" ); // portrait
    
    TBuf< KMaxKVTCameraLayoutValue > cameraOrientations;  
    TInt err = iCRProxy->Get( KCRUidVTConfiguration, 
       KVTCameraLayout, cameraOrientations );

    // parse cenrep value
    if( cameraOrientations.Length() > 0 )
        {            
        TBuf<KAbbreviationLength> abbreviation; 
        TLex lex( cameraOrientations );
        lex.SkipAndMark(KSkipChars);
        abbreviation.Append( lex.Get() );
        abbreviation.Append( lex.Get() );
        
        // primary camera value
        if ( abbreviation.CompareF( KCameraOrientationLS ) == 0 )
           {
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.prim:LS" );
           aPrimaryCameraOrientation = MVtEngMedia::EOrientationLandscape;
           }
        else if ( abbreviation.CompareF( KCameraOrientationPR ) == 0 )
           {
           aPrimaryCameraOrientation = MVtEngMedia::EOrientationPortrait;
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.prim:PR" );
           }
        else
           {
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.prim:OL" );
           aPrimaryCameraOrientation = MVtEngMedia::EOrientationObeyLayoutSwitch;
           }  
           
        abbreviation.Zero();
        lex.SkipSpace();
        lex.SkipAndMark(KSkipChars);
        abbreviation.Append( lex.Get() );
        abbreviation.Append( lex.Get() );

         //secondary camera value
        if ( abbreviation.CompareF( KCameraOrientationLS ) == 0)
           {
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.sec:LS" );
           aSecondaryCameraOrientation = MVtEngMedia::EOrientationLandscape;
           }
        else if ( abbreviation.CompareF( KCameraOrientationPR ) == 0)
           {
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.sec:PR" );
           aSecondaryCameraOrientation = MVtEngMedia::EOrientationPortrait;
           }
        else
           {
           __VTPRINT( DEBUG_GEN, "Settings.GetCamOrient.sec:OL" );
           aSecondaryCameraOrientation = MVtEngMedia::EOrientationObeyLayoutSwitch;
           } 
        }
    else
        {
        // reading from cenrep failed
        // set default values for camera orientation
        aPrimaryCameraOrientation = MVtEngMedia::EOrientationObeyLayoutSwitch;
        aSecondaryCameraOrientation = MVtEngMedia::EOrientationObeyLayoutSwitch;
        }
    __VTPRINTEXIT( "Settings.ReadVariations" )
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::CheckBit
// Checks if a certain bit is turned on in locally variated features.
// -----------------------------------------------------------------------------
//      
TBool CVtEngSettings::CheckBits( const TInt aBits ) 
    {
    __VTPRINT2( DEBUG_GEN, "Settings.CheckBit: %d", aBits )
    __VTPRINT2( DEBUG_GEN, "Settings.CheckBit=%d", 
        iVariantReadOnlyValues & aBits ? 1 : 0 )
    return iVariantReadOnlyValues & aBits;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::VideoQualityFrameRateConfig
// Returns video quality frame rate configuration.
// -----------------------------------------------------------------------------
//      
const CVtEngSettings::TVtEngVideoQualityFrameRateConfig& 
    CVtEngSettings::VideoQualityFrameRateConfig() const
    {
    __VTPRINTENTER( "Settings.VideoQualityFrameRateConfig" ) 
    __VTPRINTEXIT( "Settings.VideoQualityFrameRateConfig" ) 
    return iVQFRConfig;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::CreateDataObserverL
// Creates a shared data observer. Ownership transferred.
// -----------------------------------------------------------------------------
// 
void  CVtEngSettings::CreateDataObserverL( MVtEngCRSettingObserver* aObserver,
    TUid aUid, TUint32 aKey, CCenRepNotifyHandler::TCenRepKeyType aType )
    {
    CVtEngCRObserverItem* item = CVtEngCRObserverItem::NewL(
        aObserver, aUid, aKey, aType  );
    CleanupStack::PushL( item );
    iCRProxy->RegisterInterestedL( item );
    CleanupStack::Pop( item );
    }
   

// -----------------------------------------------------------------------------
// CVtEngSettings::ParseFrameRates
// Parses VQ frame rates read from cenrep.
// -----------------------------------------------------------------------------
// 
void CVtEngSettings::ParseFrameRates( TVtEngVideoQualityFrameRateConfig& aVQFR, 
    const TDesC& aBuffer )
    {
    __VTPRINTENTER( "Settings.ParseFrameRates" ) 
    Mem::FillZ( &aVQFR, sizeof( aVQFR ) );
    if( aBuffer.Length() > 0 )
        {            
        TLex lex( aBuffer );
        lex.Val( aVQFR.iDetail );
        __VTPRINT2( DEBUG_GEN, "Settings.ParseFrameRates iDetail=%d", aVQFR.iDetail );
        lex.SkipSpace();
        lex.Val( aVQFR.iNormal );
        __VTPRINT2( DEBUG_GEN, "Settings.ParseFrameRates iNormal=%d", aVQFR.iNormal );
        lex.SkipSpace();
        lex.Val( aVQFR.iMotion );
        __VTPRINT2( DEBUG_GEN, "Settings.ParseFrameRates iMotion=%d", aVQFR.iMotion );
        }
    __VTPRINTEXIT( "Settings.ParseFrameRates" )     
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::IsDefaultStillImageDefined
// Checks if still image exists.
// -----------------------------------------------------------------------------
//   
TBool CVtEngSettings::IsDefaultStillImageDefined()
    {
    __VTPRINTENTER( "Settings.IsDefaultStillImageDefined" ) 
    __VTPRINTEXITR( "Settings.IsDefaultStillImageDefined%d",iDefaultStillImageDefined )
    return iDefaultStillImageDefined;
    
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetCLI
// 
// -----------------------------------------------------------------------------
//   
void CVtEngSettings::SetCLI( const MVtEngSessionInfo::TCLI& aCLI )
    {
    __VTPRINTENTER( "Settings.SetCLI" )
    iCLI.iCallId = aCLI.iCallId;
    iCLI.iName = aCLI.iName;
    iCLI.iVoiceCallPossible = aCLI.iVoiceCallPossible;
    iCLI.iValid = ETrue;
    iCLI.iCallNumber = aCLI.iCallNumber;
    __VTPRINT2( DEBUG_GEN, "iVoiceCallPossible=%d,", iCLI.iVoiceCallPossible )
    __VTPRINTEXIT( "Settings.SetCLI" ) 
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::GetCLI
// 
// -----------------------------------------------------------------------------
//   
TBool CVtEngSettings::GetCLI( MVtEngSessionInfo::TCLI& aCLI ) const
    {
    aCLI.iCallId = iCLI.iCallId;
    aCLI.iVoiceCallPossible = iCLI.iVoiceCallPossible;
    aCLI.iName = iCLI.iName;
    aCLI.iCallNumber = iCLI.iCallNumber;
    return iCLI.iValid;
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::SetDataportInfoL
// 
// -----------------------------------------------------------------------------
//   
void CVtEngSettings::SetDataportInfoL( const TDesC& aPortInfo )
    {
    delete iDataportInfo;
    iDataportInfo = NULL;
    iDataportInfo = HBufC::NewL( aPortInfo.Length() );
    *iDataportInfo = aPortInfo;
    SetConnectReady();
    if ( iDataportObserver )
        {
        iDataportObserver->HandleSettingChangedL( EDataportInfo,
            *iDataportInfo );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngSettings::DataportInfo
// 
// -----------------------------------------------------------------------------
//   
const TDesC* CVtEngSettings::DataportInfo() const
    {
    return iDataportInfo;
    }
    
//  End of File  
