/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Local video configuration
*
*/



// INCLUDE FILES
#include    "CVtEngLocalVideo.h"
#include    "CVtEngMediaHandler.h"
#include    "VtEngPanic.h"
#include    "MVtEngFrameObserver.h"
#include    "CVtEngStateManager.h"
#include    "CVtEngEventManager.h"
#include    "MVtEngMedia.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "VtEngEvents.h"
#include    <videotelephonydomainpskeys.h>
#include    <videotelephonyvariant.hrh>
#include    <capivideosource.h>
#include    <cvtlogger.h>
#include    <mvtprotocolhandler.h>
#include    <fbs.h> 
#include    <w32std.h> 
#include    <graphics/suerror.h>
#include    <graphics/surfaceconfiguration.h> 

#include    "CVtEngCameraPreferences.h"

// LOCAL CONSTANTS AND MACROS

// Maximum count of providers stored (cameras, still, blank)
const TInt KVtEngProviderCountMax = 4;

// Max zoom step count.
const TInt KVtMaxZoomStep = 10;

// If prepare camera is called when provider is ready signal
// immediately back with positive value.
const TInt KVtEngAlreadySelected = 1;

// If source is already active signal with this.
const TInt KVtEngAlreadyActive = 2;

// Number of providers buffered in image sharer
const TInt KVtEngNumProvidersBuffered = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngLocalVideo
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngLocalVideo(
    CVtEngMediaHandler& aObserver )

   /** priority is high to ensure that this AO gets scheduled as soon as
    * possible after signaling. If local video is requested a service, e.g.
    * source switch while its signaled but not yet scheduled, local video's
    * internal state may be compromised (signal AO => start selecting new source
    * => RunL [this would break ongoing source selection]
    */
 : CActive( EPriorityHigh ),
   iRenderingMethod( EUnspecified ),
   iObserver( aObserver ),
   iCurrentCamera( MVtEngMedia::ESecondaryCamera ),
   iLayoutChangeHandler( *this )
    {
    ResetIndices();
    __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.c++")
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ConstructL()
    {
    __VTPRINTENTER( "LocVideo.ConstructL" )

    iAsyncCallBack = new ( ELeave ) CAsyncCallBack(
        TCallBack( CallBackFunc, this ), EPriorityStandard );

    iNGARender = CVtEngVFBitmapNGARender::NewL();

    TInt indexMobile( KErrNotFound );
    iShareImage = CVtEngShareImage::NewL( *this );
    iSourceController = CVSController::NewL( this );
    iVideoProviders =
        new ( ELeave ) CArrayFixFlat<TProviderItem>( KVtEngProviderCountMax );
    iSource = iSourceController->CreateDataSourceL( this );
    PauseVideoSending();
    TInt providers( iSourceController->ProvidersAvailable() );
    iNumSupportedCameras = providers - 1;
    ResetIndices();
    __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.providers=%d",providers)
    // Get the device specified default camera.
    TInt cameraId( iCurrentCamera );
    TInt err( SetCameraTrackingL( cameraId ) );
    if ( err == KErrNone && cameraId != EPSPreferredCameraUnknown )
        {
        // Only if valid camera.
        iCurrentCamera = MapCameraInternally( cameraId );
        err = KErrAlreadyExists;
        }

    while ( providers-- )
        {
        CVSDataProvider* provider = NULL;
        TRAP_IGNORE( provider =
            iSourceController->CreateDataProviderL( providers, this, iSource ) );
        if ( provider )
            {
            CleanupStack::PushL( provider );
            TProviderItem item;
            TVSDataProviderInfo info;
            provider->ProviderInfo( info );
            TBool destroy( ETrue );
            __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.Orient=%d",
                info.iOrientation )
            switch ( info.iOrientation )
                {
                case TCameraInfo::EOrientationInwards:
                    SetFlag( EFlagCamera1Available );
                    item.iType = KVtEngProviderCam1;
                    if ( err != KErrAlreadyExists )
                        {
                        // Set only if value not obtained from PubSub
                        iCurrentCamera = MVtEngMedia::EPrimaryCamera;
                        }
                    iProviderInfoIndexCam1 = providers;
                    __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.ConL ID C1=%d",
                        iProviderInfoIndexCam1 )
                    break;
                case TCameraInfo::EOrientationOutwards:
                    // Configures outwards camera, it is treated as camera 1
                    // if there is only one camera in the device.
                    ConfigureOutwardsCameraItem( item, providers );
                    break;
                case TCameraInfo::EOrientationMobile:
                    __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.Mobile")
                    item.iType = KVtEngProviderTempImage;
                    indexMobile = providers;
                    __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.Mobile ID=%d",
                        indexMobile )
                    break;
                case TCameraInfo::EOrientationUnknown:
                    {
                    item.iType = KVtEngProviderImage;
                    item.iProvider = provider;
                    item.iProviderState = EPermanent;
                    destroy = EFalse; // don't delete still provider
                    CreateBlankProviderL( providers );

                    if ( CVtEngUtility::Settings().
                    	 IsDefaultStillImageDefined() )
                        {
                        CreateDefaultStillImageProviderL( providers );
                        }

                    iProviderInfoIndexStill = providers;
                    __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT,
                    "LocVideo.ConL ID Still=%d",iProviderInfoIndexStill )
                    }
                    break;
                default:
                    break;
                }
            iVideoProviders->AppendL( item );
            CleanupStack::Pop();
            if ( destroy )
                {
                delete provider;
                provider = NULL;
                }
            }
        }
    iObserver.SetSelectedCameraId( iCurrentCamera );
    // Check if mobile is there, and either 1 or 2 is not.

    if ( indexMobile != KErrNotFound )
        {
        TProviderItem* item = NULL;
        const TInt err = GetProviderByType( KVtEngProviderTempImage, item );
           __ASSERT_ALWAYS( err != KErrNotFound,
            Panic( EVtEngPanicInvalidInitializationState ) );
        if ( iProviderInfoIndexCam1 == KErrNotFound )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.ConL Switch 1" )
            item->iType = KVtEngProviderCam1;
            iProviderInfoIndexCam1 = indexMobile;
            SetFlag( EFlagCamera1Available );
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "LocVideo.ConL Switch 2" )
	        DeleteProvider( KVtEngProviderCam2 );
            item->iType = KVtEngProviderCam2;
            iProviderInfoIndexCam2 = indexMobile;
            SetFlag( EFlagCamera2Available );
            }
        }
    __VTPRINTEXIT( "LocVideo.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ConfigureOutwardsCameraItem
// Creates providers item for outwards camera type. It is treated as
// a) primary camera in single camera configurations
// b) secondary camera in two camera configurations
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ConfigureOutwardsCameraItem(
    TProviderItem& aItem,
    const TInt aCurrentIndex )
    {
    __VTPRINTENTER( "LocVideo.ConfigureOutwardsCameraItem" )
    if ( iNumSupportedCameras == 1 )
        {
        SetFlag( EFlagCamera1Available );
        aItem.iType = KVtEngProviderCam1;
        iProviderInfoIndexCam1 = aCurrentIndex;
        iCurrentCamera = MVtEngMedia::EPrimaryCamera;
        __VTPRINT2( DEBUG_MEDIA,
            "LocVideo.ConL ID C1=%d", iProviderInfoIndexCam1 )
        }
    else
        {
        SetFlag( EFlagCamera2Available );
        aItem.iType = KVtEngProviderCam2;
        iProviderInfoIndexCam2 = aCurrentIndex;
        __VTPRINT2( DEBUG_MEDIA,
            "LocVideo.ConL ID C2=%d", iProviderInfoIndexCam2 )
        }
    __VTPRINTEXITR( "LocVideo.ConfigureOutwardsCameraItem iCurrentCamera=%d",
            iCurrentCamera )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CallBackFunc
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CompleteOperation( const TInt aResult )
    {
    __VTPRINTENTER( "LocVideo.CompleteOperation" )
    SetResult( aResult );
    iAsyncCallBack->CallBack();
    __VTPRINTEXITR( "LocVideo.CompleteOperation %d", aResult )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::VideoChannelOpenedL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::VideoChannelOpenedL()
    {
    __VTPRINTENTER( "LocVideo.VideoChannelOpenedL" )
    if ( iDelayedSelect.IsDelayedSelectPending() )
        {
        SelectSourceL( iDelayedSelect.DelayedSelectTarget() );
        }
    __VTPRINTEXIT( "LocVideo.VideoChannelOpenedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CallBackFunc
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CallBackFunc( TAny* aPtr )
    {
    __VTPRINTENTER( "LocVideo.CallBackFunc" )
    CVtEngLocalVideo* self = reinterpret_cast< CVtEngLocalVideo* >( aPtr );
    TInt result( KErrNone );
    TRAP( result,
        self->iObserver.HandleLocalVideoOperationCompleteL(
            self->Result() ) );
    __VTPRINTEXITR( "LocVideo.CallBackFunc %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::Result
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::Result() const
    {
    __VTPRINTENTER( "LocVideo.Result" )
    __VTPRINTEXITR( "LocVideo.Result %d", iResult )
    return iResult;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetResult
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetResult( const TInt aResult )
    {
    __VTPRINTENTER( "LocVideo.SetResult" )
    iResult = aResult;
    __VTPRINTEXITR( "LocVideo.SetResult %d", iResult )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo* CVtEngLocalVideo::NewL(
    CVtEngMediaHandler& aObserver )
    {
    __VTPRINTENTER( "LocVid.NewL" )
    CVtEngLocalVideo* self = new( ELeave )
        CVtEngLocalVideo( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    __VTPRINTEXIT( "LocVid.NewL" )
    return self;
    }


// -----------------------------------------------------------------------------
// CVtEngLocalVideo::~CVtEngLocalVideo
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::~CVtEngLocalVideo()
    {
    __VTPRINTENTER( "LocVid.~" )
    // Cancel the object
    if ( iActiveProvider )
        {
        iActiveProvider->DetachFromCameraPreferences();
        }
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        delete iAsyncCallBack;
        }
    Cancel();
    if ( iVideoProviders )
        {
        while ( iVideoProviders->Count() )
            {
            TProviderItem& item = (*iVideoProviders)[0];
            CVSDataProvider* provider = item.iProvider;
            delete provider;
            iVideoProviders->Delete( 0 );
            }
        delete iVideoProviders;
        }

    delete iCameraListener;
    iCameraListener = NULL;

    delete iSource;
    delete iSourceController;
    DeleteAudioSource();
    delete iShareImage;

    delete iNGARender;
    
    __VTPRINTEXIT( "LocVid.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::vsProviderError
// Handles error in provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::vsProviderError( TInt aError )
    {
    if( aError == KErrCancel )
        {
        vsProviderReady();
        return;
        }
    __VTPRINTENTER( "LocVid.vsProviderError" )
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.ProviderError=%d", aError )
    iInitializingProvider = NULL;
    if ( aError != KErrNone )
        {
        iShareImage->Error( aError );
        if ( !iLayoutChangeHandler.ProviderError( aError ) )
            {
            const TState previousState = iState;
            iState = ELocNone;
            __VTPRINT( DEBUG_MEDIA, " STATE TO NONE" )

            // If the error indicates that camera is in use
            if ( aError == KErrInUse && previousState == ELocInitializing )
            {
            // Store the error to be used later
            if( iSelectedProvider )
                {
                __VTPRINT( DEBUG_MEDIA,
                "LocVideo.ProviderError in use")
                iSelectedProvider->iErrorCode = aError;
                if ( iSelectedProvider->iType == KVtEngProviderCam1 )
                    {
                    ClearFlag( EFlagCamera1Available );
                    }
                else if ( iSelectedProvider->iType == KVtEngProviderCam2 )
                    {
                    ClearFlag( EFlagCamera2Available );
                    }
                }
            }
        // Camera has been lost
		if ( aError == KVsErrCameraNoLongerReserved )
			{
			if ( !IsActive() )
                {
                SetActive();
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, aError );
                __VTPRINT( DEBUG_MEDIA,
                	"LocVideo.ProviderError AO start" )
               	__VTPRINTEXITR(
                     "LocVid.vsProviderError err=%d", aError )

                __VTPRINT( DEBUG_MEDIA,
                	"LocVideo.ProviderError iSelectedProvider NULL" )
                if ( iSelectedProvider )
                	{
                	__VTPRINT( DEBUG_MEDIA,
                	"LocVideo.ProviderError iSelectedProvider NULL" )
                	iSelectedProvider->iInitialized = EFalse;
                	iSelectedProvider->iProviderState =
                    	(CVtEngLocalVideo::TProviderState) aError;
                	__VTPRINT( DEBUG_MEDIA,
                	"LocVideo.ProviderError iSelectedProvider NULL" )
                	}
                return;
            	}
			}
        if ( iSelectedProvider )
            {
            // Try to select the last known to function camera
            if ( iSelectedProvider->iType == KVtEngProviderCam1 ||
                 iSelectedProvider->iType == KVtEngProviderCam2 )
                {
                if ( !IsActive() )
                    {
                    SetActive();
                    TRequestStatus* status = &iStatus;
                    User::RequestComplete( status, aError );
                     __VTPRINT( DEBUG_MEDIA,
                     "LocVideo.ProviderError AO start" )
                     __VTPRINTEXITR(
                     "LocVid.vsProviderError err=%d", aError )
                     return;
                     }
                 }
                //Error code needed later
                iSelectedProvider->iInitialized = EFalse;
                iSelectedProvider->iProviderState =
                    (CVtEngLocalVideo::TProviderState) aError;
                iSelectedProvider = NULL;
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                    " selected provider nulled" )
                }
            else if ( iActiveProvider &&
                     !IsFlag( EFlagInitializeOnly ) &&
                     previousState == ELocSwichingProvider )
                {
                iActiveProvider = iFadingProvider;
                iFadingProvider = NULL;
                }
            if ( IsFlag( EFlagSignalSourceSelection ) )
                {
                ClearFlag( EFlagSignalSourceSelection );
                TRAP_IGNORE(
                    iObserver.HandleLocalVideoOperationCompleteL( aError ) );
                }
            }
        }
    __VTPRINTEXIT( "LocVid.vsProviderError" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::vsProviderReady
// Provider is ready to be taken into use.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::vsProviderReady()
    {
    __VTPRINTENTER( "LocVid.vsProviderReady" )

    __VTPRINT3( DEBUG_MEDIA, "LocVideo.ProviderReady state=%d, initOnly=%d",
        iState, IsFlag( EFlagInitializeOnly ) )
    const TState prevState( iState );

    if ( iInitializingProvider )
        {
        iInitializingProvider->iInitialized = ETrue;

        // Set camera flags in order to recover
        // error situation (error occured during init)
        if ( iInitializingProvider->iType == KVtEngProviderCam1 )
        	{
			__VTPRINT( DEBUG_MEDIA, " LocVid.vsProviderReady KVtEngProviderCam1" )
        	SetFlag( EFlagCamera1Available );
        	}
        else if ( iInitializingProvider->iType == KVtEngProviderCam2 )
        	{
        	__VTPRINT( DEBUG_MEDIA, " LocVid.vsProviderReady KVtEngProviderCam2" )
        	SetFlag( EFlagCamera2Available );
        	}

        iInitializingProvider = NULL;
        }

    iState = ELocReady;
    __VTPRINT( DEBUG_MEDIA, " STATE TO READY" )

    if ( iLayoutChangeHandler.ProviderReady() )
        {
        __VTPRINTEXIT( "LocVid.vsProviderReady" )
        return;
        }

    if ( prevState == ELocInitializing && !IsFlag( EFlagInitializeOnly ) )
        {
        TProviderItem* switchTo = iActiveProvider;
        if ( iSelectedProvider )
            {
            iSelectedProvider->iErrorCode = KErrNone;
            switchTo = iSelectedProvider;
            iSelectedProvider = NULL;
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " selected provider nulled" )
            }
        __VTPRINT( DEBUG_MEDIA, "LocVideo.ProviderReady switching")
        SwitchProvider( switchTo );
        __VTPRINTEXIT( "LocVid.vsProviderReady" )
        return;
        }
    else
        {
        iSelectedProvider = NULL;
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Selected provider nulled, UDS" )
        UpdateState();
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.vsProviderReady KVtEngSourceCapsChanged")
        CVtEngEventManager::NotifyEvent( KVtEngSourceCapsChanged );
        }

    if ( IsFlag( EFlagSignalSourceSelection ) )
        {
        ClearFlag( EFlagSignalSourceSelection );
        TRAP_IGNORE( iObserver.HandleLocalVideoOperationCompleteL( KErrNone ) );
        }
    __VTPRINTEXIT( "LocVid.vsProviderReady" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::vsProviderSwitchDone
// Provider switched.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::vsProviderSwitchDone( CVSDataProvider* aOldProvider )
    {
    __VTPRINTENTER( "LocVid.vsProviderSwitchDone" )
    iState = ELocReady;
    __VTPRINT( DEBUG_MEDIA, " STATE TO READY" )

    iFadingProvider = NULL;

    if ( iActiveProvider && iActiveProvider->iProvider )
        {
        iActiveProvider->iProvider->ProviderInfo( iProviderInfo );
        TInt error( iActiveProvider->RestoreZoomFactor() );
        if ( error )
            {
            __VTPRINT2( DEBUG_MEDIA,
                "LocVid.vsProviderSwitchDone restoring stored zoom FAILED %d",
                error )
            }
        }

    iShareImage->ProviderSwitchDone( aOldProvider );

    if ( iActiveProvider && iActiveProvider->iType == KVtEngProviderCam1 )
        {
        iCurrentCamera = MVtEngMedia::EPrimaryCamera;
        iObserver.SetSelectedCameraId( iCurrentCamera );
        }
    else if ( iActiveProvider && iActiveProvider->iType == KVtEngProviderCam2 )
        {
        iCurrentCamera = MVtEngMedia::ESecondaryCamera;
        iObserver.SetSelectedCameraId( iCurrentCamera );
        }

    // Let layout change handler handle providerswitch
    if ( iLayoutChangeHandler.ProviderSwitchDone( aOldProvider ) )
        {
        CVtEngEventManager::NotifyEvent( KVtEngLCHProviderSwitchDone );
        __VTPRINTEXIT( "LocVid.vsProviderSwitchDone" )
        return;
        }

    ClearFlag( EFlagFrozen );

    // If initilization of the waiting provider fails fall back to the last one.
    TVtEngProviderType backupProviderType( KVtEngProviderNone );

    // Delete non-permanent old provider
    const TInt indexI( ProviderIndexByInstance( aOldProvider ) );

    // delete provider that became unavailable
    if ( indexI != KErrNotFound )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
            "LocVideo.providerSwitchDone indexInstance=%d", indexI )
        TProviderItem& item( ( *iVideoProviders )[ indexI ] );
        if ( item.iProviderState != EPermanent &&
             item.iProviderState != ERefreshing )
            {
            DeleteProvider( item );
            }
        }

    // Uninitialize fading provider
    const TInt indexF = ProviderIndexByState( EFading );
    if ( indexF != KErrNotFound )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
            "LocVideo.providerSwitchDone fading indexFading=%d", indexF )
        TProviderItem& item = (*iVideoProviders)[indexF];

        // Mark up type of the last provider.
        backupProviderType = item.iType;

        DeleteProvider( item );
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
            "LocVideo.providerSwitchDone fading=%d", item.iType )
        TRAP_IGNORE( item.iProvider = CreateDataProviderL( item.iType ) );
        }

    // Initialize waiting provider
    TInt err( KErrNone );
    const TInt indexW( ProviderIndexByState( EWaiting ) );
    if ( indexW != KErrNotFound )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
            "LocVideo.providerSwitchDone waiting=%d", indexW )
        TProviderItem& item = (*iVideoProviders)[indexW];
        item.iProviderState = EUndefined;
        TRAP( err, InitializeProviderL( item ) );
        if ( err == KErrNone)
            {
            iSelectedProvider = &item;
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " selected provider set" )
            backupProviderType = KVtEngProviderNone;
            }
        else
            {
            iState = ELocNone;
            __VTPRINT( DEBUG_MEDIA, " STATE TO NONE" )
            __VTPRINT2( DEBUG_MEDIA, "failed initializing provider=%d", err )
            // If initialization failed indicate that provider is not available
            if ( item.iType == KVtEngProviderCam1 )
                {
                ClearFlag( EFlagCamera1Available );
                }
            else if ( item.iType == KVtEngProviderCam2 )
                {
                ClearFlag( EFlagCamera2Available );
                }
            else if( item.iType == KVtEngProviderShareImage )
                {
                iShareImage->Error( err );
                }
            CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
            }
        }

    // Reconstruct refreshing (new still image) provider.
    const TInt indexR( ProviderIndexByState( ERefreshing ) );
    if ( indexR != KErrNotFound )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
            "LocVideo.providerSwitchDone refreshing")
        TProviderItem& item = (*iVideoProviders)[indexR];
        DeleteProvider( item );

        // If still image disabled in settings.
        CVtEngSettings& settings = CVtEngUtility::Settings();
        const CVtEngSettings::TVtEngVideoCallConfig& config =
            settings.Config();
        const TBool stillImageValid( config.iVideo.iImageIsValid );
        TRAP( err,
            {
            item.iProvider = CreateDataProviderL( item.iType );
            if ( stillImageValid )
                {
                InitializeProviderL( item );
                }
            else
                {
                item.iProviderState = EPermanent; // change back to original
                }
            } );
        if ( err == KErrNone && stillImageValid )
            {
            iSelectedProvider = &item;
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " selected provider set" )
            }
        else
            {
            iState = ELocNone;
            __VTPRINT2( DEBUG_MEDIA, " STATE TO NONE initerr=%d", err )
            }
        }

    // If we have delayed select pending -> initiate new switch and return
    if ( iDelayedSelect.IsDelayedSelectPending() )
        {
        ClearFlag( EFlagCheckRevertToPrevious );
        TVtEngProviderType pending = iDelayedSelect.DelayedSelectTarget();
        __VTPRINT2( DEBUG_MEDIA, " delayed select pending type = %d", pending )
        if( iActiveProvider && iActiveProvider->iType == pending )
            {
            // clear the iDelayedSelect to KVtEngProviderUndefined
            // and no need to select again,continue complete this vsProviderSwitchDone
            iDelayedSelect.DelayedSelectTarget();
            }
        else
            {
            TRAP_IGNORE( SelectSourceL( pending ) );
            __VTPRINTEXIT( "LocVid.vsProviderSwitchDone" )
            return;
            }
        }

    if ( IsFlag( EFlagSignalSourceSelection ) &&
         iState != ELocInitializing && // Set in InitializeProviderL if called
         err == KErrNone &&            // but not if init (or create provider in
                                       // there) failed.
         !IsFlag( EFlagCheckRevertToPrevious ) )
        {
        // Signal only if we're in final state (i.e. not if blank provider
        // was switched and just started initializing a camera).
        ClearFlag( EFlagSignalSourceSelection );
        TRAP_IGNORE( iObserver.HandleLocalVideoOperationCompleteL( KErrNone ) );
        }
    else if ( IsFlag( EFlagCheckRevertToPrevious ) &&
        ( iState == ELocReady || iState == ELocNone ) )
        {
        ClearFlag( EFlagCheckRevertToPrevious );
        MVtEngMedia::TCameraId currentCam( MVtEngMedia::EPrimaryCamera );
        iObserver.GetCurrentCameraId( currentCam );
        backupProviderType =
            ( currentCam==MVtEngMedia::EPrimaryCamera ) ?
            KVtEngProviderCam1 : KVtEngProviderCam2;
        }

    __VTPRINT2( DEBUG_MEDIA, "        switch done iState=%d", iState )

    //Inform camera preferences that valid provider is set
	// Check that valid provider exist
	// and set it as provider to camera
	// preferences
    if ( iActiveProvider )
        {
        __VTPRINT2( DEBUG_MEDIA, "switch done prov.state=%d",
        	iActiveProvider->iProviderState )
        // Set provider to camera preferences
        // only if camera preferences exist
        if ( iCameraPref)
    		{
    		__VTPRINT2( DEBUG_MEDIA,
    		"LocVideo.providerSwitchDone iCameraPref->SetProvider type=%d",
        		iActiveProvider->iType )
    		iCameraPref->SetProvider( iActiveProvider->iProvider,
    		    *iActiveProvider );

            TInt error( iActiveProvider->RestorePreferences() );
            if ( error )
                {
                __VTPRINT2( DEBUG_MEDIA, " restoring preferences FAILED %d",
                    error )
                }
    		}
    	else
    		{
    		__VTPRINT( DEBUG_MEDIA,
    			"LocVideo.providerSwitchDone camera pref not created" )
    		}
        }

    if ( IsFlag( EFlagInitialized ) && iState != ELocInitializing )
        {
        __VTPRINT( DEBUG_MEDIA,
                "LocVideo.providerSwitchDone KVtEngSourceCapsChanged" )
        CVtEngEventManager::NotifyEvent( KVtEngSourceChanged );
        }

    TRAP( err, StartViewFinderIfWasStartedL() ); // err is ignored

    if ( backupProviderType != KVtEngProviderNone )
        {
        // Initialization of new provider failed,
        // try reverting back to last one.
        __VTPRINT2( DEBUG_MEDIA,
            "LocVideo.providerSwitchDone revert to %d", backupProviderType )
        TRAP( err, SelectSourceL( backupProviderType ) );
        if ( err != KErrNone )
            {
            CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
            }
        }

    __VTPRINTEXIT( "LocVid.vsProviderSwitchDone" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::vsViewFinderFrameReady
// View finder image can be set.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::vsViewFinderFrameReady( CFbsBitmap& aFrame )
    {
    __VTPRINTENTER( "LocVid.vsViewFinderFrameReady" )
    if ( iActiveProvider &&
         iActiveProvider->iType != KVtEngProviderNone )
        {
#if 0
        TRAP_IGNORE( iOptions.iObserver->vtHandleFrameL(
            MVtEngFrameObserver::ELocalVideoFrame, &aFrame ) );
#endif
        iNGARender->UpdateBitmapL( aFrame );
        }
    __VTPRINTEXIT( "LocVid.vsViewFinderFrameReady" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::vsProvidersChanged
// Provider state has changed.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::vsProvidersChanged( TBool aAttached )
    {
    __VTPRINTENTER( "LocVid.vsProvidersChanged" )

    __VTPRINT2( DEBUG_MEDIA, "   vsProvidersChanged attached=%d", aAttached )
    __VTPRINT2( DEBUG_MEDIA, "   vsProvidersChanged state=%d",iState )
    if ( iAudioSource == NULL )
        {
        // If this is called from ConstructL (indirectly) return immediately
        if ( aAttached || iCurrentCamera == MVtEngMedia::EPrimaryCamera )
            {
            SetFlag( EFlagCamera1Available );
            iObserver.SetSelectedCameraId( MVtEngMedia::EPrimaryCamera, ETrue );
            }
        else
            {
            ClearFlag( EFlagCamera1Available );
            iObserver.SetSelectedCameraId(
                MVtEngMedia::ESecondaryCamera, ETrue );
            }
        __VTPRINTEXIT( "LocVid.vsProvidersChanged" )
        return;
        }

    // If primary camera was not available and state is not "open"
    // UI should be requested for answer. Precondition is also that
    // primary camera is now available.
    TBool notifyAnswer( EFalse );
    TInt activeMedia( 0 );

    const MVtEngSessionInfo::TSessionState state(
        CVtEngUtility::StateManager()->SessionState() );

    __VTPRINT2( DEBUG_MEDIA, "         vsProvidersChanged, call state=%d", state )
    if ( state == MVtEngSessionInfo::EReceiving )
        {
        __VTPRINT2( DEBUG_MEDIA, "         vsProvidersChanged attached=%d", aAttached )
        if ( !IsFlag( EFlagCamera1Available ) && aAttached )
            {
            notifyAnswer = ETrue;
            }
        }

    __VTPRINT2( DEBUG_MEDIA,
        "         vsProvidersChanged answer=%d", notifyAnswer )
    CVtEngLocalVideo::TVtEngProviderType providerToSelect( KVtEngProviderNone );
    TRAPD( err, providerToSelect =  UpdateAvailableProvidersL() );
    if ( err == KErrNone )
        {
        // Now get the state to check if video has been disabled
        GetMediaState( activeMedia );
        __VTPRINT2( DEBUG_MEDIA,
            "         vsProvidersChanged mediastate=%d", activeMedia )
        __VTPRINT2( DEBUG_MEDIA,
            "         vsProvidersChanged iState=%d", iState )

        // values for checking if switch is allowed
        TBool activeIsStillOrBlank ( EFalse );
        TBool activeIsBlankAndNewCam1( EFalse );
        if ( iActiveProvider )
            {
            __VTPRINT2( DEBUG_MEDIA,
            "         vsProvidersChanged iActivePType=%d", iActiveProvider->iType )
            if ( iActiveProvider->iType == KVtEngProviderImage ||
                      iActiveProvider->iType == KVtEngProviderNone )
                {
                activeIsStillOrBlank = ETrue;
                if ( providerToSelect == KVtEngProviderCam1 &&
                     iActiveProvider->iType == KVtEngProviderNone )
                    {
                    activeIsBlankAndNewCam1 = ETrue;
                    }
                }
            }
        __VTPRINT2( DEBUG_MEDIA,
            "         vsProvidersChanged blank active new cam1=%d",
            activeIsBlankAndNewCam1 )

        __VTPRINT2( DEBUG_MEDIA,
            "         vsProvidersChanged provider=%d",  providerToSelect )

        if ( providerToSelect == KVtEngProviderCam1 )
            {
            iObserver.SetSelectedCameraId( MVtEngMedia::EPrimaryCamera );
            }
        else if ( providerToSelect == KVtEngProviderCam2 )
            {
            // delete provider that became unavailable
            iObserver.SetSelectedCameraId( MVtEngMedia::ESecondaryCamera );
            }

        // Check if SelectSource should be called
        if ( !notifyAnswer && // for incoming calls automatic switch not allowed
             ( iState == ELocNone || iState == ELocReady ) &&
             IsFlag( EFlagInitialized ) )
            {
            __VTPRINT( DEBUG_MEDIA, "         vsProvidersChanged ok to select" )
            // Allow switch if video disabled (still/blank active)
            // and cam1 available.
            ClearFlag( EFlagInitializeOnly );
            ClearFlag( EFlagCheckRevertToPrevious );
            if ( activeIsStillOrBlank &&
                 providerToSelect == KVtEngProviderCam2 )
                {
                SetFlag( EFlagInitializeOnly );
                }
            else if ( activeIsBlankAndNewCam1 )
                {
                iObserver.AutoEnableVideo();
                }
            else if ( providerToSelect == KVtEngProviderNone )
                {
                iObserver.AutoEnableVideo( EFalse );
                }
            SetFlag( EFlagAutomaticSwitchCallback );
            TRAP( err, SelectSourceL( providerToSelect ) );
            ClearFlag( EFlagAutomaticSwitchCallback );

            if ( err != KErrNone )
                {
                __VTPRINT2( DEBUG_MEDIA,
                    "         vsProvidersChanged,select failed=%d", err )
                if ( providerToSelect == KVtEngProviderCam1 &&
                     iNumSupportedCameras > 1 )
                    {
                    __VTPRINT( DEBUG_MEDIA, "LocVideo.vsProvidersChanged 2 2" )
                    SetFlag( EFlagCamera2Available );
                    ClearFlag( EFlagCamera1Available );
                    TRAP( err, SelectSourceL( KVtEngProviderCam2 ) );
                    }
                else if ( providerToSelect == KVtEngProviderCam2 )
                    {
                    __VTPRINT( DEBUG_MEDIA,
                        "LocVideo.vsProvidersChanged 2 still")
                    ClearFlag( EFlagCamera2Available );
                    TRAP( err, SelectSourceL( KVtEngProviderImage ) );
                    }
                }
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA, "         vsProvidersChanged Not Changed" )
            if ( iState == ELocInitializing &&
                 iSelectedProvider &&
                 iSelectedProvider->iType != providerToSelect )
                {
                SetFlag( EFlagCheckRevertToPrevious );
                }
            }

        CVtEngStateManager* states = CVtEngUtility::StateManager();
        states->Update();

        if ( IsFlag( EFlagCamera1Available ) ||
             IsFlag( EFlagCamera2Available ) )
            {
            if ( notifyAnswer )
                {
                __VTPRINT( DEBUG_MEDIA,
                    "LocVideo.vsProvidersChanged KVtEngSessionAnswerPossible")
                CVtEngEventManager::NotifyEvent( KVtEngSessionAnswerPossible );
                }
            else if ( !notifyAnswer &&
                // Don't notify before selection is done.
                      ( iState == ELocReady ||
                        iState == ELocNone ) )
                {
                __VTPRINT( DEBUG_MEDIA,
                    "LocVideo.vsProvidersChanged KVtEngSourceCapsChanged" )
                CVtEngEventManager::NotifyEvent( KVtEngSourceCapsChanged );
                }
            }
        else if ( !IsFlag( EFlagCamera1Available ) &&
            !IsFlag( EFlagCamera2Available ) )
            {
            __VTPRINT( DEBUG_MEDIA, "LocVideo.vsProvidersChanged NO CAMS" )
            CVtEngEventManager::NotifyEvent( KVtEngSourceCapsChanged );
            }
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.vsProvidersChanged KVtEngResourceMemAllocFailure" )
        CVtEngEventManager::NotifyEvent( KVtEngResourceMemAllocFailure  );
        }
    __VTPRINTEXITR( "LocVid.vsProvidersChanged err=%d", err )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetViewFinderParameters
// Configures parameters for rendering via window server.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetViewFinderParameters(
    const TVtEngRenderingOptions& aParams )
    {
    __VTPRINTENTER( "LocVid.SetViewFinderParameters" )

    // Before setting the params, stop VF
    if( ViewFinderStarted() )
        {
        StopViewFinder();
        }

    iOptions.iSize = aParams.iSize;
    __VTPRINT2( DEBUG_MEDIA,
        "         width = %d", iOptions.iSize.iWidth );
    __VTPRINT2( DEBUG_MEDIA,
        "         height = %d", iOptions.iSize.iHeight );

    iOptions.iObserver = aParams.iObserver;
    iRenderingMethod = EWindowServer;
    __VTPRINTEXIT( "LocVid.SetViewFinderParameters" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetViewFinderParameters
// Configures parameters for rendering with direct screen access.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetViewFinderParameters(
    const TVtEngRenderingOptionsDSA& aDSA )
    {
    __VTPRINTENTER( "LocVid.SetVFParamsDSA" )

    // Before setting the params, stop VF
    if( ViewFinderStarted() )
        {
        StopViewFinder();
        }

    iRenderingMethod = EDSA;
    iDsaOptions.iClipRect = aDSA.iClipRect;
    iDsaOptions.iRect = aDSA.iRect;
    iDsaOptions.iWindow = &aDSA.iWindow;
    iDsaOptions.iWs = &aDSA.iWs;
    iDsaOptions.iWsSD = &aDSA.iWsSD;
    __VTPRINTEXIT( "LocVid.SetVFParamsDSA" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetViewFinderParameters
// Configures parameters for rendering with display posting.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetViewFinderParameters(
    const TVtEngRenderingOptionsDP& aDP )
    {
    __VTPRINTENTER( "LocVid.SetVFParamsDP" )

    // Before setting the params, stop VF
    if( ViewFinderStarted() )
        {
        StopViewFinder();
        }

    iRenderingMethod = EDP;
    iDsaOptions.iClipRect = aDP.iClipRect;
    iDsaOptions.iRect = aDP.iRect;
    iDsaOptions.iWindow = &aDP.iWindow;
    iDsaOptions.iWs = &aDP.iWs;
    iDsaOptions.iWsSD = &aDP.iWsSD;
    __VTPRINTEXIT( "LocVid.SetVFParamsDP" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetViewFinderParameters
// Configures parameters for rendering with NGA.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetViewFinderParameters(
    const TVtEngRenderingOptionsNGA &aNGA )
    {
    __VTPRINTENTER( "LocVid.SetVFParamsNGA" )

    // Before setting the params, stop VF
    if( ViewFinderStarted() )
        {
        StopViewFinder();
        }

    iOptions.iSize.SetSize( KVtEngLocalVideoDefaultWidth, 
                            KVtEngLocalVideoDefaultHeight );
    
    iWindow = &aNGA.iWindow;
    iWs = &aNGA.iWs;
    
    iRenderingMethod = EWindowServer;

    iNGARender->AttachSurfaceL( iWs, iWindow );
    __VTPRINTEXIT( "LocVid.SetVFParamsNGA" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetDefaultStillImageL
// Sets default still image.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetDefaultStillImageL()
    {
    __VTPRINTENTER( "LocVid.SetDefaultStillImageL" )

   	TProviderItem* item = NULL;
    GetProviderByType( KVtEngProviderDefaultStillImage, item );
    if ( item )
        {
        CVSDataProvider* provider = item->iProvider;

        if ( provider && item->iType == KVtEngProviderDefaultStillImage )
            {
            if ( item->iInitialized )
                {
                StopViewFinder();

                TInt providerIndex =
                    ProviderIndex( KVtEngProviderDefaultStillImage );

                TProviderItem* defaultStillProviderItem =
                    &( *iVideoProviders )[providerIndex];
                SwitchProvider( defaultStillProviderItem );
                }
            else
                {
                InitializeProviderL( *item );
                iSelectedProvider = item;
                }
            }
        }
    __VTPRINTEXIT( "LocVid.SetDefaultStillImageL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetStillImageL
// Sets still image.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetStillImageL( TBool aSetAsActive )
    {
    __VTPRINTENTER( "LocVid.SetStillImageL" )

    CVtEngSettings& settings = CVtEngUtility::Settings();
    const CVtEngSettings::TVtEngVideoCallConfig& config =
        settings.Config();
    const TBool stillImageValid( config.iVideo.iImageIsValid );
    if ( iState == ELocInitializing && stillImageValid )
        {
        __VTPRINT( DEBUG_MEDIA, "  setStill failed" )
        User::Leave( KErrNotReady );
        }

    TInt count( iVideoProviders->Count() );
    TBool done( EFalse );
    while ( count-- )
        {
        TProviderItem& item = ( *iVideoProviders )[count];
        CVSDataProvider* provider = item.iProvider;

        if ( item.iType == KVtEngProviderImage )
            {
            if ( provider && item.iInitialized )
                {
                // If still image provider is active change to temp provider
                if ( iActiveProvider &&
                     ( iActiveProvider->iType == KVtEngProviderImage ) )
                    {
                    __VTPRINT( DEBUG_MEDIA, "Act.prov. must be deleted" )
                    StopViewFinder();
                    iActiveProvider->iProviderState = ERefreshing;
                    TInt providerIndex = ProviderIndex( KVtEngProviderNone );
                    TProviderItem* blankProviderItem =
                        &( *iVideoProviders )[providerIndex];
                    SwitchProvider( blankProviderItem );
                    __VTPRINTEXIT( "LocVideo.SetStillImageL" )
                    return;
                    }

                //Only way to change image is first delete the provider
                delete provider;
                item.iProvider = NULL;

                // Create it again.
                // If there is a provider info for still.
                if ( iProviderInfoIndexStill != KErrNotFound )
                    {
                    item.iProvider = iSourceController->CreateDataProviderL(
                        iProviderInfoIndexStill,
                        this, iSource );
                    item.iType = KVtEngProviderImage;
                    item.iInitialized = EFalse;
                    }
                }
            // Do initialization only if valid image (still not
            // cleared in settings).
            if ( stillImageValid )
                {
                InitializeProviderL( item );
                if ( aSetAsActive )
                    {
                    iSelectedProvider = &item;
                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                        " selected provider set" )
                    }
                }
            done = ETrue;
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                "LocVideo.SetStillImageL ok" )
            }
        }
    if ( !done )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.SetStillImageL fail")
        User::Leave( KErrGeneral );
        }
    __VTPRINTEXIT( "LocVid.SetStillImageL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeL
// Initializes default blank provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeL()
    {
    __VTPRINTENTER( "LocVid.InitializeL" )
    const TInt index( ProviderIndex( KVtEngProviderNone ) );
    TProviderItem* item = &(*iVideoProviders)[index];
    if ( iActiveProvider &&
         iActiveProvider->iType == KVtEngProviderImage )
        {
        // Blank provider is set to active only
        // if there is not valid still image.
        SetFlag( EFlagInitializeOnly );
        iSelectedProvider  = item;
        }
    else
        {
        iActiveProvider = item;
        }
    InitializeProviderL( *item );
    SetFlag( EFlagSignalSourceSelection );
    SetFlag( EFlagInitialized );
    __VTPRINTEXIT( "LocVid.InitializeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SelectSourceL
// Selects source.
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::SelectSourceL( TVtEngProviderType aSource )
    {
    __VTPRINTENTER( "LocVid.SelectSourceL" )

    __VTPRINT2( DEBUG_MEDIA, "  source=%d", aSource )

    if ( iDelayedSelect.IsDelayedSelectPending() &&
         ( aSource != KVtEngProviderShareImage ) &&
         ( aSource == KVtEngProviderCam1 ||
           aSource == KVtEngProviderCam2 ||
           aSource == KVtEngProviderImage ) )
        {
        TVtEngProviderType delayedSelect( iDelayedSelect.DelayedSelectTarget() );
        if ( delayedSelect != KVtEngProviderNone && 
             delayedSelect != KVtEngProviderImage )
            {
            aSource = delayedSelect;
            }
        }

    if( IsProviderWaiting( aSource ) )
        {
        // If waiting for provide switch and a provider initialization
        // don't allow selection.
        __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSourceL already waiting" )
        User::Leave( KErrNotReady );
        }

    // To be selected source is already active
    if ( IsActiveProvider( aSource ) )
        {
        Cancel();
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KVtEngAlreadyActive );
        SetFlag( EFlagSignalSourceSelection );
        __VTPRINT2( DEBUG_MEDIA, "LocVideo.SelectSourceL start AO=%d",
            KVtEngAlreadyActive )
        if ( iActiveProvider )
            {
            __VTPRINT2( DEBUG_MEDIA, "LocVideo. activeProv type=%d",
                iActiveProvider->iType )
            }
        // In case there were errors, NULL the selected provider
        // to be able to initialize it when needed
        if( iSelectedProvider )
            {
            if( iSelectedProvider->iErrorCode )
                {
                iSelectedProvider->iInitialized = EFalse;
                iSelectedProvider->iProviderState =
                    (CVtEngLocalVideo::TProviderState)
                    iSelectedProvider->iErrorCode;
                iSelectedProvider = NULL;
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " selprovider nulled" )
                }
            }
        // Not actually switching but virtually doing it because
        // async callback will be made.
        __VTPRINTEXITR( "LocVid.SelectSourceL %d", ETrue )
        return ETrue;
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.Selectsource iState=%d", iState )
    if ( iState == ELocInitializing &&
         aSource == KVtEngProviderImage &&
         iActiveProvider &&
         iActiveProvider->iType != KVtEngProviderNone )
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.SelectSource initialization pending" )
        User::Leave( KErrNotReady );
        }
    // We have switch pending
    else if ( iState == ELocSwichingProvider )
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.SelectSource switching -> set delayed select" )
        iDelayedSelect.NotifyDelayedSelect( aSource );
        __VTPRINTEXITR( "LocVid.SelectSourceL %d", ETrue )
        return ETrue;
        }

    TBool initialized( EFalse );
    TProviderItem item;
    TInt providerIndex( GetProviderByType( aSource, item ) );

    if ( providerIndex != KErrNotFound )
        {
        // requested provider found, set initialization status
        __VTPRINT2( DEBUG_MEDIA, "LocVideo.SelectSource index=%d", providerIndex )
        initialized = item.iInitialized;
        }
    else
        { // requested provider not found
        __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSource create provider" )
        // Try to create provider
        TInt index = KErrNotFound;
        switch ( aSource )
            {
            case KVtEngProviderCam2:
                index = iProviderInfoIndexCam2;
                break;
            case KVtEngProviderNone:
            case KVtEngProviderImage:
            case KVtEngProviderDefaultStillImage:
            // Image sharing
            case KVtEngProviderShareImage:
                index = iProviderInfoIndexStill;
                break;
            case KVtEngProviderCam1:
                index = iProviderInfoIndexCam1;
            default:
                break;
            }

        __VTPRINT2( DEBUG_MEDIA,
            "LocVideo.SelectSource create provider ID=%d", index )
        if ( index != KErrNotFound )
            {
            TProviderItem newItem;
            newItem.iProvider =
                iSourceController->CreateDataProviderL(
                index, this, iSource );
            newItem.iType = aSource;
            CleanupStack::PushL( newItem.iProvider );
            iVideoProviders->AppendL( newItem );
            CleanupStack::Pop();
            providerIndex = iVideoProviders->Count() - 1;
            __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
                "LocVideo.SelectSource create provider ok=%d",
                newItem.iProvider != NULL )
            }
        }

    TBool initOrSwitch = EFalse;

    // Initialize provider
    if ( providerIndex != KErrNotFound && !initialized )
        {
        if ( iActiveProvider->iProviderState != EPermanent )
            {
            // Active provider must be destroyed and new one initialized
            __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSource del Act.prov" )
            (*iVideoProviders)[providerIndex].iProviderState = EWaiting;
            iActiveProvider->iProviderState = EFading;
            StopViewFinder();
            providerIndex = ProviderIndex( KVtEngProviderNone );
            __VTPRINT2( DEBUG_MEDIA, "LocVideo.SelectSource index for none=%d",
                providerIndex )
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSource do init" )
            UninitializeProviderL();
            TProviderItem& initItem = ( *iVideoProviders )[providerIndex];
            InitializeProviderL( initItem );
            // This item is stored, make it initialized only when callback
            // received.
            iSelectedProvider = &initItem;
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " selected provider set" )
            initOrSwitch = InitializingOrSwitching();
            __VTPRINTEXITR( "LocVid.SelectSourceL %d", initOrSwitch )
            return initOrSwitch;
            }
        }

    // Switch provider
    if ( providerIndex != KErrNotFound )
        {
        if ( !IsFlag( EFlagInitializeOnly ) )
            {
            __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSource ready for switch")
            StopViewFinder();
            TProviderItem* switchItem = &(*iVideoProviders)[providerIndex];
            __VTPRINT2( DEBUG_MEDIA,
                "LocVideo.SelectSource switchItem %d", switchItem->iType )
            SwitchProvider( switchItem );
            }
        else if ( initialized && IsFlag( EFlagInitializeOnly ) )
            {
            __VTPRINT2( DEBUG_MEDIA,
                "LocVideo.SelectSource already initialized start AO %d",
                KVtEngAlreadySelected )
            TProviderItem* switchItem = &( *iVideoProviders )[providerIndex];
            iActiveProvider = switchItem;
            Cancel();
            TRequestStatus* status = &iStatus;
            SetActive();
            User::RequestComplete( status, KVtEngAlreadySelected );
            }
        else
            {
            __VTPRINT3( DEBUG_MEDIA, "   initialized=%d,flags=%d",
                initialized, iFlags )
            }
        }

    // Error
    if ( iActiveProvider == NULL )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.SelectSource leave" )
        User::Leave( KErrNotFound );
        }

    initOrSwitch = InitializingOrSwitching();
    __VTPRINTEXITR( "LocVid.SelectSourceL %d", initOrSwitch )
    return initOrSwitch;
    }

// ----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeProviderL
// Initializes a provider.
// ----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeProviderL(
    TProviderItem& aProviderItem )
    {
    __VTPRINTENTER( "LocVid.InitializeProviderL" )

    __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv type=%d", aProviderItem.iType )

    if ( iInitializingProvider )
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.InitProv ERROR, already initializing" )
        User::Leave( KErrNotReady );
        }

    iInitializingProvider = &aProviderItem;

    CVtEngSettings& settings = CVtEngUtility::Settings();
    const CVtEngSettings::TVtEngVideoCallConfig& config =
        settings.Config();
    TPckgBuf< TVSStillImageDataProviderParams > pckg;

    __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv type=%d", aProviderItem.iType )
    CVSDataProvider* provider = aProviderItem.iProvider;
    if ( provider == NULL )
        {
        aProviderItem.iInitialized = EFalse;
        aProviderItem.iProvider = CreateDataProviderL( aProviderItem.iType );
        provider = aProviderItem.iProvider;
        }
    else if ( aProviderItem.iInitialized )
        {
        __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv AO start %d", KErrNone )
        Cancel();
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        __VTPRINTEXIT( "LocVid.InitializeProviderL" )
        return;
        }
    User::LeaveIfNull( provider );
    switch ( aProviderItem.iType )
        {
        case KVtEngProviderCam1:
        case KVtEngProviderCam2:
            provider->InitializeL( KNullDesC8 );
            iState = ELocInitializing;
            __VTPRINT( DEBUG_MEDIA, " STATE TO INITIALIZING" )
            __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv type=%d",
                aProviderItem.iType )
            break;
        case KVtEngProviderNone:
            // Blank provider.
            pckg().iInitialize =
                TVSStillImageDataProviderParams::EInitializeAsBlankImage;
            provider->InitializeL( pckg );
            iState = ELocInitializing;
            __VTPRINT( DEBUG_MEDIA, " STATE TO INITIALIZING" )
            __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv type=%d",
                aProviderItem.iType )
            break;
        case KVtEngProviderImage:
            {
             if ( config.iVideo.iImageIsValid )
                {
                pckg().iInitialize =
                    TVSStillImageDataProviderParams::EInitializeFromGeneralSettings;
                provider->InitializeL( pckg );
                iState = ELocInitializing;
                __VTPRINT( DEBUG_MEDIA, " STATE TO INITIALIZING" )
                __VTPRINT( DEBUG_MEDIA, "LocVideo.InitProv Still image")
                }
             else
                {
                User::Leave( KErrPathNotFound );
                }
            }
            break;
        case KVtEngProviderDefaultStillImage:
                pckg().iInitialize =
                    TVSStillImageDataProviderParams::EInitializeAsDefaultStillImage;

            provider->InitializeL( pckg );
            iState = ELocInitializing;
            __VTPRINT( DEBUG_MEDIA, " STATE TO INITIALIZING" )
            __VTPRINT2( DEBUG_MEDIA, "LocVideo.InitProv type=%d",
                aProviderItem.iType )
            break;
        default:
            Panic( EVtEngPanicNoVideoProvider );
            break;
        }
    //aProviderItem.iInitialized = ETrue;
    __VTPRINTEXIT( "LocVid.InitializeProviderL" )
    }

// ----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeShareProviderL
// Initializes a provider that is used for sharing.
// ----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeShareProviderL(
    TProviderItem& aProviderItem, const TDesC8& aParams )
    {
    __VTPRINTENTER( "LocVid.InitializeShareProviderL" )
    aProviderItem.iProvider->InitializeL( aParams );
    aProviderItem.iInitialized = ETrue;
    iState = ELocInitializing;
    SetFlag( EFlagSignalSourceSelection | EFlagInitializeOnly );
    __VTPRINTEXIT( "LocVid.InitializeShareProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ReCreateProviderL
// -----------------------------------------------------------------------------
void CVtEngLocalVideo::ReCreateProviderL(
    TProviderItem& aProviderItem, const TVtEngProviderType aType )
    {
    __VTPRINTENTER( "LocVid.ReCreateProviderL" )

    TInt index( KErrNotFound );

    switch ( aType )
        {
        case KVtEngProviderShareImage:
            index = iProviderInfoIndexStill;
            break;

        default:
            User::Leave( KErrNotSupported );
        }

    // Delete old instance
    DeleteProvider( aProviderItem );

    // Create new instance
    aProviderItem.iProvider = iSourceController->CreateDataProviderL(
        index, this, iSource );
    aProviderItem.iType = aType;

    __VTPRINTEXIT( "LocVid.ReCreateProviderL" )
    }

// ----------------------------------------------------------------------------
// CVtEngLocalVideo::CameraIdToType
// Camera ID to type mapping.
// ----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType CVtEngLocalVideo::CameraIdToType(
    MVtEngMedia::TCameraId aId )
    {
    TVtEngProviderType type( KVtEngProviderCam2 );
    switch ( aId )
        {
        case MVtEngMedia::EPrimaryCamera:
            type = KVtEngProviderCam1;
            break;
        case MVtEngMedia::ESecondaryCamera:
            type = KVtEngProviderCam2;
            break;
        default:
            type = KVtEngProviderNone;
            break;
        }
    return type;
    }

// ----------------------------------------------------------------------------
// CVtEngLocalVideo::RunL
// CActive heir, acts according to status.
// ----------------------------------------------------------------------------
//
void CVtEngLocalVideo::RunL()
    {
    __VTPRINTENTER( "LocVid.RunL" )

    const TInt status( iStatus.Int() );
    TVtEngProviderType providerType = KVtEngProviderNone;
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.RunL %d", status )
    switch ( status )
        {
        case KVtEngAlreadySelected:
            SetFlag( EFlagSignalSourceSelection );
            vsProviderReady();
            break;
        case KVtEngAlreadyActive:
            TRAP_IGNORE(
                iObserver.HandleLocalVideoOperationCompleteL( KErrNone ) );
            break;
        case KErrNone:
            vsProviderSwitchDone( NULL );
            UpdateState();
            break;
        // Camera has been lost during VT
        case KVsErrCameraNoLongerReserved:
        	__VTPRINT2( DEBUG_MEDIA, "LocVideo.status =%d",
                status )
            //delete provider here and null pointer
            if ( iSelectedProvider )
            	{
            	DeleteProvider( *iSelectedProvider );
            	iSelectedProvider = NULL;
            	}

            // Check if still is in use
            providerType = KVtEngProviderImage;
            GetHighestSelectableProvider( providerType );

            __VTPRINT2( DEBUG_MEDIA, "LocVideo.providerType=%d",
                providerType )

        	iObserver.AutoEnableVideo( EFalse );
            SetFlag( EFlagAutomaticSwitchCallback );
        	TRAPD( err, SelectSourceL( providerType ) );
        	ClearFlag( EFlagAutomaticSwitchCallback );

            if ( err != KErrNone )
                {
                TRAP( err, iObserver.HandleLocalVideoOperationCompleteL( err) );
                }
            break;
         default:
            // error
            {
            switch( iSelectedProvider->iType )
                {
            case KVtEngProviderCam1:
                ClearFlag( EFlagCamera1Available );
                break;
            case KVtEngProviderCam2:
                ClearFlag( EFlagCamera2Available );
                break;
            default:
                break;
                }
            DeleteProvider( *iSelectedProvider );
            iSelectedProvider = NULL;
            MVtEngMedia::TCameraId currentCam( MVtEngMedia::EPrimaryCamera );
            iObserver.GetCurrentCameraId( currentCam );
            TVtEngProviderType providerType( CameraIdToType( currentCam ) );
            GetHighestSelectableProvider( providerType );
            TRAPD( err, SelectSourceL( providerType ) );
            if ( err != KErrNone )
                {
                TRAP( err, iObserver.HandleLocalVideoOperationCompleteL( err) );
                }
            }
        }
    __VTPRINTEXIT( "LocVid.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsActiveProvider
// Returns ETrue if given provider is already active.
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsActiveProvider( TVtEngProviderType aType ) const
    {
    __VTPRINTENTER( "LocVid.IsActiveProvider" )
    const TBool active(
        ( iActiveProvider && iActiveProvider->iType == aType ) ||
        ( iSelectedProvider && iSelectedProvider->iType == aType ) );
    __VTPRINTEXITR( "LocVid.IsActiveProvider %d", active )
    return active;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::DoCancel
// Does nothing.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::DoCancel()
    {
    __VTPRINTENTER( "LocVid.DoCancel" )
    __VTPRINTEXIT( "LocVid.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ActiveProvider
// Returns active provider.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType CVtEngLocalVideo::ActiveProvider() const
    {
    if ( iActiveProvider )
        {
        return iActiveProvider->iType;
        }
    return KVtEngProviderNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::StartViewFinderIfWasStartedL
// View finder image can be set.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::StartViewFinderIfWasStartedL()
    {
    __VTPRINTENTER( "LocVid.StartViewFinderIfWasStartedL" )
    if ( IsFlag( EFlagUIForeground ) && IsFlag( EFlagVFEnabled ) &&
         iState == ELocReady )
        {
        StartViewFinderL();
        }
    __VTPRINTEXIT( "LocVid.StartViewFinderIfWasStartedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::StartViewFinderL
// Starts view finder.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::StartViewFinderL( TBool aClientRequest )
    {
    __VTPRINTENTER( "LocVid.StartViewFinderL" )
    __ASSERT_ALWAYS( iVideoProviders->Count(),
        Panic ( EVtEngPanicNoVideoProvider ) );

    if ( aClientRequest )
        {
        SetFlag( EFlagVFEnabled );
        }
    if ( iRenderingMethod == EUnspecified || !iActiveProvider )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.startVF error")
        User::Leave( KErrNotReady );
        }
    CVSDataProvider* provider = iActiveProvider->iProvider;
    if ( !provider )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.startVF error no provider")
        User::Leave( KErrNotReady );
        }
    else if ( provider->ViewFinderActive() )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.startVF already started")
            iViewFinderState = ( iRenderingMethod == EWindowServer ) ?
                EVFReceivingBitmaps : EVFRenderingDsa;
        __VTPRINTEXIT( "LocVid.StartViewFinderL" )
        return;
        }
    else if ( !IsFlag( EFlagInitialized ) )
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.startVF not initialized")
        __VTPRINTEXIT( "LocVid.StartViewFinderL" )
        return;
        }
    else if ( iState != ELocReady )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_RETURN,
            "LocVideo.startVF incorrect iState=%d", iState )
        }
    else
        {
        // Lint requires this one
        __VTPRINT( DEBUG_MEDIA,
            "LocVideo.startVF unsupported clause" )
        }

    CheckOthersStopped( *provider );
    if ( iRenderingMethod == EWindowServer )
        {
        TSize size( iOptions.iSize );
        provider->StartViewFinderBitmapsL( size );
        iViewFinderState = EVFReceivingBitmaps;
        __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "                started" )
        }
    else
        {
        provider->StartViewFinderDirectL(
            *(iDsaOptions.iWs),
            *(iDsaOptions.iWsSD),
            *(iDsaOptions.iWindow),
            iDsaOptions.iRect,
            iDsaOptions.iClipRect );
        iViewFinderState = EVFRenderingDsa;
        }

    if( iActiveProvider->iType == CVtEngLocalVideo::KVtEngProviderCam1 &&
        !provider->ViewFinderMirror() )
        {
        provider->SetViewFinderMirrorL( ETrue );
        }

    __VTPRINTEXIT( "LocVid.StartViewFinderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::StopViewFinder
// Stops view finder.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::StopViewFinder( TBool aClientRequest )
    {
    __VTPRINTENTER( "LocVid.StopViewFinder" )
    if ( aClientRequest )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.stopVF clearing EFlagVFEnabled" )
        ClearFlag( EFlagVFEnabled );
        }
    if ( iActiveProvider &&
         iViewFinderState != EVFNone &&
         iActiveProvider->iProvider &&
         iActiveProvider->iProvider->ViewFinderActive() )
        {
#if 0
        TRAP_IGNORE( iOptions.iObserver->vtHandleFrameL(
            MVtEngFrameObserver::ELocalVideoFrame, NULL ) );
#endif
        iActiveProvider->iProvider->StopViewFinder();
        __VTPRINT( DEBUG_MEDIA, "LocVideo.stopVF Stopped" )
        if ( IsFlag( EFlagAutomaticSwitchCallback )  ||
            IsFlag( EFlagUIBackground ) )
            {
            // UI needs to be notified if stop done due to automatic
            // source switch (from vsProvidersChanged).
            ClearFlag( EFlagAutomaticSwitchCallback );
            CVtEngEventManager::NotifyEvent( KVtEngViewFinderStopped );
            __VTPRINT( DEBUG_MEDIA, "LocVideo.stopVF Notified" )
            }
        }
    iViewFinderState = EVFNone;
    __VTPRINTEXIT( "LocVid.StopViewFinder" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::FreezeL
// Freezes provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::FreezeL()
    {
    __VTPRINTENTER( "LocVid.FreezeL" )
    if( OkToFreeze() )
        {
        iActiveProvider->iProvider->FreezeL();
        SetFlag( EFlagFrozen );
        }
    __VTPRINTEXIT( "LocVid.FreezeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::UnfreezeL
// Unfreezes provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::UnfreezeL()
    {
    __VTPRINTENTER( "LocVid.UnfreezeL" )
    if( OkToUnfreeze() )
        {
        if ( !iLayoutChangeHandler.UnfreezeL() )
            {
            iActiveProvider->iProvider->UnfreezeL();
            if ( iDelayedSelect.IsDelayedSelectPending() )
                {
                TVtEngProviderType type( iDelayedSelect.DelayedSelectTarget() );
                __VTPRINT2( DEBUG_MEDIA,
                    "LocVideo.UnfreezeL delayed switch to %d", type )
                SelectSourceL( type );
                }
            else
                {
                CompleteOperation( KErrNone );
                }
            }
        ClearFlag( EFlagFrozen );
        }
    __VTPRINTEXIT( "LocVid.UnfreezeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CheckOthersStopped
// Stops view finder of other providers than the one given as parameter.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CheckOthersStopped( const CVSDataProvider& aProvider )
    {
    __VTPRINTENTER( "LocVid.CheckOthersStopped" )
    TInt count( iVideoProviders->Count() );
    while ( count-- )
        {
        TProviderItem& item = ( *iVideoProviders )[count];
        if ( item.iProvider &&
             item.iProvider != &aProvider &&
             item.iProvider->ViewFinderActive() )
            {
            item.iProvider->StopViewFinder();
            __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "LocVideo.OtherStop" )
            }
        }
    __VTPRINTEXIT( "LocVid.CheckOthersStopped" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::Source
// Returns video source.
// -----------------------------------------------------------------------------
//
MVTVideoSource* CVtEngLocalVideo::Source() const
    {
    return iSource;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::AudioSource
// Returns audio source.
// -----------------------------------------------------------------------------
//
MVTAudioSource* CVtEngLocalVideo::AudioSource() const
    {
    return iAudioSource;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetMediaState
// Returns media state.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetMediaState( TInt& aActiveMedia )
    {
    aActiveMedia = 0;
    if ( !IsFlag( EFlagAudioMuted ) )
        {
        aActiveMedia |= MVtEngMedia::EMediaAudio;
        // Check if the thing is muted
        }

    if ( iActiveProvider )
        {
        __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetMediaState provider type=%d",
            iActiveProvider->iType )
        switch ( iActiveProvider->iType )
            {
            case KVtEngProviderCam1:
            case KVtEngProviderCam2:
            case KVtEngProviderImage:
            case KVtEngProviderShareImage:
                aActiveMedia |= MVtEngMedia::EMediaVideo;
                break;
            default:
                break;
            }
        }
    __VTPRINT2( DEBUG_MEDIA | DEBUG_RETURN,
        "LocVideo.GetMediaState media state=%d", aActiveMedia )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetMediaSource
// Returns media source.
// -----------------------------------------------------------------------------
//
MVtEngMedia::TMediaSource CVtEngLocalVideo::GetMediaSource()
    {
    MVtEngMedia::TMediaSource source( MVtEngMedia::EMediaNone );
    if ( iActiveProvider )
        {
        switch ( iActiveProvider->iType )
            {
            case KVtEngProviderCam1:
            case KVtEngProviderCam2:
                source = MVtEngMedia::EMediaCamera;
                break;
            case KVtEngProviderShareImage:
                source = MVtEngMedia::EMediaShare;
                break;
            case KVtEngProviderImage:
            case KVtEngProviderDefaultStillImage:
                source = MVtEngMedia::EMediaStillImage;
                break;
            default:
                source = MVtEngMedia::EMediaNone;
                break;
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetMediaSource src=%d", (TInt) source )
    return source;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetSourceCaps
// Returns capability of providers .
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetSourceCaps( TInt& aCaps )
    {
    aCaps = 0;
    TInt count( iVideoProviders->Count() );
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetSourceCaps count=%d", count )
    while ( count-- )
        {
        switch ( iVideoProviders->At(count).iType )
            {
            case KVtEngProviderCam1:
                __VTPRINT( DEBUG_MEDIA,
                    "LocVideo.GetSourceCaps cam1" )
                if ( IsFlag( EFlagCamera1Available ) )
                    {
                    __VTPRINT( DEBUG_MEDIA,
                        "LocVideo.GetSourceCaps cam1 flagged" )
                    aCaps |= MVtEngMedia::ESourceCapsPrimaryCamera;
                    }
                break;
            case KVtEngProviderCam2:
                __VTPRINT( DEBUG_MEDIA, "LocVideo.GetSourceCaps cam2" )
                if ( IsFlag( EFlagCamera2Available ) )
                    {
                    __VTPRINT( DEBUG_MEDIA,
                        "LocVideo.GetSourceCaps cam2 flagged" )
                    aCaps |= MVtEngMedia::ESourceCapsSecondaryCamera;
                    }
                break;
            case KVtEngProviderImage:
                {
                __VTPRINT( DEBUG_MEDIA,
                    "LocVideo.GetSourceCaps image" )
                // Make sure that image is valid
                CVtEngSettings& settings = CVtEngUtility::Settings();
                const CVtEngSettings::TVtEngVideoCallConfig& config =
                settings.Config();
                if( config.iVideo.iImageIsValid )
                    {
                    aCaps |= MVtEngMedia::ESourceCapsStillImage;
                    }
                }
                break;
            default:
               break;
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetSourceCaps caps=%d", aCaps )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeOnly
// Sets boolean to indicate if the provider is to be initialized only.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeOnly( TBool aInitOnly )
    {
    InitializeOnlyEx( GetMediaSource() != MVtEngMedia::EMediaCamera &&
        aInitOnly );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeOnlyEx
// Sets boolean to indicate if the provider is to be initialized only. Does
// not check current media source.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeOnlyEx( TBool aInitOnly )
    {
    SetFlag( EFlagSignalSourceSelection );
    ClearFlag( EFlagInitializeOnly );
    if ( aInitOnly )
        {
        SetFlag( EFlagInitializeOnly );
        }
#ifdef VTDEBUG
    if ( IsFlag( EFlagInitializeOnly ) )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " initOnly SET" )
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, " initOnly CLEARED" )
        }
#endif
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetProviderByType
// Return provider of particular type.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetProviderByType(
    TVtEngProviderType aType,
    TProviderItem& aItem )
    {
    TInt ret( KErrNotFound );
    TInt count( iVideoProviders->Count() );
    while ( count-- )
        {
        if ( ( *iVideoProviders )[count].iType == aType )
            {
            aItem = ( *iVideoProviders )[count];
            ret = count;
            count = 0;
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetProviderByType idx=%d", ret )
    return ret;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetProviderByType
// Return provider of particular type.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetProviderByType(
    TVtEngProviderType aType,
    TProviderItem*& aItem )
    {
    TInt ret( KErrNotFound );
    TInt count( iVideoProviders->Count() );
    while ( count-- )
        {
        if ( ( *iVideoProviders )[count].iType == aType )
            {
            aItem = &( *iVideoProviders )[count];
            ret = count;
            count = 0;
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetProviderByType* idx=%d", ret )
    return ret;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetProviderByInstance
// Return provider with particular provider instance.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetProviderByInstance(
    const CVSDataProvider* aInstance,
    TProviderItem*& aItem ) const
    {
    TInt indexI( ProviderIndexByInstance( aInstance ) );
    if ( indexI != KErrNotFound )
        {
        aItem = &( *iVideoProviders )[ indexI ];
        }
    else
        {
        aItem = NULL;
        }
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetProviderByType* idx=%d", indexI )
    return indexI;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ProviderIndex
// Returns provider index by type.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::ProviderIndex( TVtEngProviderType aType ) const
    {
    for( TInt count = iVideoProviders->Count() - 1; count >= 0; count-- )
        {
        if ( ( *iVideoProviders )[ count ].iType == aType )
            {
            return count;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ProviderIndexByState
// Gets a provider index by state.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::ProviderIndexByState( TProviderState aState ) const
    {
    for( TInt count = iVideoProviders->Count() - 1; count >= 0; count-- )
        {
        if ( ( *iVideoProviders )[ count ].iProviderState == aState )
            {
            return count;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ProviderIndexByInstance
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::ProviderIndexByInstance(
    const CVSDataProvider* aInstance) const
    {
    for( TInt count = iVideoProviders->Count() - 1; count >= 0; count-- )
        {
        if ( ( *iVideoProviders )[ count ].iProvider == aInstance )
            {
            return count;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CreateDataProviderL
// Creates VideoSource provider by type.
// -----------------------------------------------------------------------------
//
CVSDataProvider* CVtEngLocalVideo::CreateDataProviderL(
    TVtEngProviderType aType )
    {
    __VTPRINTENTER( "LocVid.CreateDataProviderL" )
    __VTPRINT2( DEBUG_MEDIA, "  provider type=%d", aType )
    TInt index = KErrNotFound;
    switch ( aType )
        {
        case KVtEngProviderNone:
        case KVtEngProviderImage:
        case KVtEngProviderShareImage:
        case KVtEngProviderDefaultStillImage:
            __VTPRINT( DEBUG_MEDIA, "LocVideo.CreateDataProviderL Still" )
            index = iProviderInfoIndexStill;
            break;
        case KVtEngProviderCam1:
            __VTPRINT( DEBUG_MEDIA, "LocVideo.CreateDataProviderL Cam1" )
           index = iProviderInfoIndexCam1;
            break;
        case KVtEngProviderCam2:
            __VTPRINT( DEBUG_MEDIA, "LocVideo.CreateDataProviderL Cam2" )
            index = iProviderInfoIndexCam2;
            break;
        default:
            __VTPRINT( DEBUG_MEDIA, "LocVideo.CreateDataProviderL Mobile" )
            break;
        }

    CVSDataProvider* provider = NULL;
    if ( index != KErrNotFound )
        {
        __VTPRINT( DEBUG_MEDIA, "LocVideo.CreateDataProviderL provider is" )
        provider =
            iSourceController->CreateDataProviderL(
            index, this, iSource );
        }

    __VTPRINT2( DEBUG_MEDIA, " provider created=%d",
        ( TInt ) ( provider != NULL ) )
    __VTPRINTEXIT( "LocVid.CreateDataProviderL" )
    return provider;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CreateBlankProvider
// Creates blank provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CreateBlankProviderL( const TInt aIndex )
    {
    __VTPRINTENTER( "LocVid.CreateBlankProviderL" )
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.CreateBlankProviderL idx=%d", aIndex )
    CVSDataProvider* provider = NULL;
    TRAP_IGNORE( provider = iSourceController->CreateDataProviderL(
        aIndex, this, iSource ) );
    CleanupStack::PushL( provider );
    TProviderItem item;
    item.iProvider = provider;
    item.iType = KVtEngProviderNone;
    item.iInitialized = EFalse;
    item.iProviderState = EPermanent;
    iVideoProviders->AppendL( item );
    CleanupStack::Pop();
    __VTPRINTEXIT( "LocVid.CreateBlankProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CreateDefaultStillImageProviderL
// Creates default still image provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CreateDefaultStillImageProviderL( const TInt aIndex )
    {
    __VTPRINTENTER( "LocVid.CreateDefaultStillImageProviderL" )
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.CreateDefaultStillImageProviderL idx=%d", aIndex )
    CVSDataProvider* provider = NULL;
    TRAP_IGNORE( provider = iSourceController->CreateDataProviderL(
        aIndex, this, iSource ) );
    CleanupStack::PushL( provider );
    TProviderItem item;
    item.iProvider = provider;
    item.iType = KVtEngProviderDefaultStillImage;
    item.iInitialized = EFalse;
    item.iProviderState = EPermanent;
    iVideoProviders->AppendL( item );
    CleanupStack::Pop();
    __VTPRINTEXIT( "LocVid.CreateDefaultStillImageProviderL" )
    }


// -----------------------------------------------------------------------------
// CVtEngLocalVideo::UpdateAvailableProvidersL
// Updates current providers and their information.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType
    CVtEngLocalVideo::UpdateAvailableProvidersL()
    {
    __VTPRINTENTER( "LocVid.UpdateAvailableProvidersL" )
    TVtEngProviderType type( KVtEngProviderNone );
    TInt count( iSourceController->ProvidersAvailable() );
    iNumSupportedCameras = count - 1;
    TVSDataProviderInfo providerInfo;
    TBool primaryAvailable( EFalse );
    TBool secondaryAvailable( EFalse );
    TBool mobileAvailable( EFalse );
    TInt indexMobile( KErrNotFound );
    ClearFlag( EFlagCamera1Available );
    ClearFlag( EFlagCamera2Available );
    ResetIndices();
    while ( count-- )
        {
        iSourceController->ProviderInfoL( count, providerInfo );
        if ( providerInfo.iOrientation == TCameraInfo::EOrientationInwards ||
            // treat outwards camera as primary when only 1 cam in the system
             ( providerInfo.iOrientation == TCameraInfo::EOrientationOutwards &&
              iNumSupportedCameras == 1 ) )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP inw" )
            type = KVtEngProviderCam1;
            primaryAvailable = ETrue;
            SetFlag( EFlagCamera1Available );
            iProviderInfoIndexCam1 = count;
            }
        else if (
            providerInfo.iOrientation == TCameraInfo::EOrientationOutwards )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP outw" )
            secondaryAvailable = ETrue;
            SetFlag( EFlagCamera2Available );
            iProviderInfoIndexCam2 = count;
            }
        else if ( providerInfo.iOrientation == TCameraInfo::EOrientationMobile )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP mobile" )
            mobileAvailable = ETrue;
            indexMobile = count;
            }
        else if ( providerInfo.iOrientation == TCameraInfo::EOrientationUnknown )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP still" )
            CVtEngSettings& settings = CVtEngUtility::Settings();
            const CVtEngSettings::TVtEngVideoCallConfig& config =
                settings.Config();
            if ( config.iVideo.iImageIsValid )
                {
                iProviderInfoIndexStill = count;
                }
            }
        }
    if ( !primaryAvailable && mobileAvailable )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP 1 upd" )
        type = KVtEngProviderCam1;
        iProviderInfoIndexCam1 = indexMobile;
        primaryAvailable = ETrue;
        SetFlag( EFlagCamera1Available );
        }
    else if ( primaryAvailable && mobileAvailable )
        {
        // No 1, 2 and mobile.
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP 2 upd" )
        DeleteProvider( KVtEngProviderCam2 );
		secondaryAvailable = ETrue;
        iProviderInfoIndexCam2 = indexMobile;
        SetFlag( EFlagCamera2Available );
        }
    else if ( !primaryAvailable && secondaryAvailable )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP !1 & 2" )
        type = KVtEngProviderCam2;
        }
    else if ( type == KVtEngProviderNone )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP none" )
        CVtEngSettings& settings = CVtEngUtility::Settings();
        const CVtEngSettings::TVtEngVideoCallConfig& config =
            settings.Config();
        if ( config.iVideo.iImageIsValid )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.UAP valid image" )
            type = KVtEngProviderImage;
            }
        }

    __VTPRINTEXITR( "LocVid.UpdateAvailableProvidersL %d", type )
    return type;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SwitchProvider
// Switches to active provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SwitchProvider( TProviderItem* aSwitchToItem )
    {
    __VTPRINTENTER( "LocVid.SwitchProvider" )

    iState = ELocSwichingProvider;
    __VTPRINT( DEBUG_MEDIA, " STATE TO SWITCHING" )
    iFadingProvider = iActiveProvider;
    iActiveProvider = aSwitchToItem;
    TRAPD( err, iSource->SwitchDataProviderL( iActiveProvider->iProvider ) );
    if ( err != KErrNone )
        {
        iState = ELocReady; // go to AO
        __VTPRINT( DEBUG_MEDIA, " STATE TO READY" )
        }
    switch ( iState )
        {
        case ELocReady: // Switch done. Sync.
            {
            // Entered here if SwitchDataProviderL causes direct call to
            // vsProviderSwitchDone. Since provider switch should be async
            // from client point of view now start AO and signal in RunL.
            Cancel();
            TRequestStatus* status = &iStatus;
            SetActive();
            User::RequestComplete( status, err );
            __VTPRINT2( DEBUG_MEDIA,
                "LocVideo.SwitchProvider AO start %d", err )
            }
            // fall through
        case ELocSwichingProvider: // Async.
        case ELocNone:
            SetFlag( EFlagSignalSourceSelection );
            break;
        default:
            break;
        }
    __VTPRINTEXIT( "LocVid.SwitchProvider" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SwitchProvider
// Overloaded method that takes new active provider as a parameter.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SwitchProvider( TProviderItem& aProviderItem )
    {
    __VTPRINTENTER( "LocVid.SwitchProvider by aProviderItem" )
    StopViewFinder( EFalse );
    SwitchProvider( &aProviderItem );
    __VTPRINTEXIT( "LocVid.SwitchProvider by aProviderItem" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsInitialized
// Checks if a provider with given id is initialized.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::IsInitialized(
    MVtEngMedia::TCameraId aId,
    TBool& aInitialized )
    {
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.IsInitialized ID:%d", aId )
    TInt count( iVideoProviders->Count() );
    TVtEngProviderType type = CameraIdToType( aId );
    aInitialized = EFalse;
    TInt err( KErrNotFound );
    while ( count-- )
        {
        TProviderItem& item = ( *iVideoProviders )[count];
        if ( item.iType == type )
            {
            __VTPRINT( DEBUG_MEDIA, "LocVideo.IsInitialized found" )
            aInitialized = item.iInitialized;
            if ( ( !aInitialized && item.iProviderState < KErrNone ) ||
                item.iErrorCode < KErrNone )
                {
                __VTPRINT( DEBUG_MEDIA, "LocVideo.IsInitialized problems" )
                err = item.iErrorCode;
                }
            else
                {
                err = KErrNone;
                }
            count = 0; // break
            }
        }
    __VTPRINT3( DEBUG_MEDIA | DEBUG_RETURN,
        "LocVideo.IsInitialized isInitialized=%d,err=%d", aInitialized, err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::UpdateState
// Updates view finder state.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::UpdateState()
    {
    __VTPRINTENTER( "LocVid.UpdateState" )

    if ( iActiveProvider )
        {
        CVSDataProvider* provider = iActiveProvider->iProvider;
        
        if ( provider )
            {
            __VTPRINT2 ( DEBUG_MEDIA, "LocVideo.UpdateState, provider active",
                provider->ViewFinderActive() )
            if ( provider->ViewFinderActive() )
                {
                iViewFinderState = ( iRenderingMethod == EWindowServer ) ?
                    EVFReceivingBitmaps : EVFRenderingDsa;
                }
            }
        }
    
    ClearFlag( EFlagInitializeOnly );
    __VTPRINT2( DEBUG_MEDIA | DEBUG_RETURN,
        "LocVideo.UpdateState iViewFinderState %d", iViewFinderState )
    __VTPRINTEXIT( "LocVid.UpdateState" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializingOrSwitching
// Is either initializing or doing a provider switch.
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::InitializingOrSwitching() const
    {
    return ( iState == ELocInitializing ||
             iState == ELocSwichingProvider );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::UninitializeProviderL
// Uninitializes initialized non-permanent provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::UninitializeProviderL()
    {
    __VTPRINTENTER( "LocVid.UninitializeProviderL" )
    TInt count( iVideoProviders->Count() );
    while ( count-- )
        {
        TProviderItem& item = ( *iVideoProviders )[count];
        if ( item.iProviderState == EUndefined && item.iInitialized )
            {
            DeleteProvider( item );
            item.iProvider = CreateDataProviderL( item.iType );
            }
        }
    __VTPRINTEXIT( "LocVid.UninitializeProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetCameraInfo
// Gets camera info.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetCameraInfo( MVtEngMedia::TCameraId aId,
    MVtEngMedia::TCameraInfo& aInfo )
    {
    TBool initialized( EFalse );
    TInt err( IsInitialized( aId, initialized ) );
    TVSDataProviderInfo info;
    if ( initialized && err == KErrNone )
        {
        TVtEngProviderType type = CameraIdToType( aId );
        TProviderItem item;
        GetProviderByType( type, item );
        item.iProvider->ProviderInfo( info );
        aInfo.iMaxDigitalZoom = info.iMaxDigitalZoom;
        aInfo.iMaxZoom = info.iMaxZoom;
        aInfo.iMinZoom = info.iMinZoom;
        aInfo.iMajorZoomStepCount =
            CalculateMajorDigitalZoomStepCount( item.iProvider );
        }
    return initialized ? err : KErrNotReady;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetDigitalZoomFactor
// Gets current zoom step.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetDigitalZoomFactor( TInt& aCurrentStep )
    {
    if ( !iActiveProvider ||
         !iActiveProvider->iProvider )
        {
        return KErrNotReady;
        }
    aCurrentStep = iActiveProvider->iProvider->DigitalZoomFactor();
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.GetZ %d", aCurrentStep )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetMaxDigitalZoomStep
// Gets max. digital zoom step.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetMaxDigitalZoomStep( TInt& aMaxZoomStep )
    {
    if ( !iActiveProvider ||
         !iActiveProvider->iProvider )
        {
        return KErrNotReady;
        }
    aMaxZoomStep = iProviderInfo.iMaxDigitalZoom;
    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
        "LocVideo.MaxZStep %d", aMaxZoomStep )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetMajorDigitalZoomStepCount
// Gets number of major zoom level leaps.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::
    GetMajorDigitalZoomStepCount( TInt& aCount, TBool /*aProviderSupplied*/ )
    {
    /*__VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
        "LocVideo.GetMajZStepCount %d", aProviderSupplied )*/
    if ( !iActiveProvider || !iActiveProvider->iProvider )
        {
        return KErrNotReady;
        }
    aCount = CalculateMajorDigitalZoomStepCount( iProviderInfo );
    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
        "LocVideo.GetMajZStepCount %d", aCount )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetMajorDigitalZoomStep
// Maps major step to zoom step. Note: aIndex is given in range
// [0 .. CalculateMajorDigitalZoomStepCount() - 1]
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::GetMajorDigitalZoomStep( const TInt aIndex, TInt& aStep )
    {
    if ( !iActiveProvider || !iActiveProvider->iProvider )
        {
        // No active provider.
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "LocVideo.GetMajZStep NO active")
        return;
        }
    if ( iProviderInfo.iMaxDigitalZoom > KVtMaxZoomStep )
        {
        aStep = iProviderInfo.iMaxDigitalZoom *
            Min( KVtMaxZoomStep, Max( 0, aIndex ) ) / KVtMaxZoomStep;
        }
    else
        {
        aStep = Min( iProviderInfo.iMaxDigitalZoom, Max( 0, aIndex ) );
        }
    __VTPRINT3( DEBUG_MEDIA | DEBUG_DETAIL,
        "LocVideo.GetMajZStep idx=%d,step=%d", aIndex, aStep )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetFreezeSupported
// Gets freeze supported value from provider.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::GetFreezeSupported( TBool& aFreezeSupported ) const
    {
    __VTPRINTENTER( "LocVideo.GetFreezeSupported" )
    if ( !iActiveProvider ||
         !iActiveProvider->iProvider )
        {
        __VTPRINTEXIT( "LocVideo.GetFreezeSupported KErrNotReady")
        return KErrNotReady;
        }
    aFreezeSupported = iProviderInfo.iFreezeSupported;
    __VTPRINTEXITR( "LocVideo.GetFreezeSupported %d", aFreezeSupported )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::OkToFreeze
// Returns ETrue if it is ok to freeze provider (i.e. freeze is supported AND
// provider is not currently frozen)
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::OkToFreeze() const
    {
    __VTPRINTENTER( "LocVideo.OkToFreeze" )
    TBool isOk( EFalse );
    if ( iActiveProvider &&
         iActiveProvider->iProvider )
        {
        isOk = ( iProviderInfo.iFreezeSupported && !IsFlag( EFlagFrozen ) );
        }
    __VTPRINTEXITR( "LocVideo.OkToFreeze %d", isOk )
    return isOk;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::OkToUnfreeze
// Returns ETrue if it is ok to unfreeze provider (i.e. freeze is supported AND
// provider is currently frozen)
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::OkToUnfreeze() const
    {
    __VTPRINTENTER( "LocVideo.OkToUnfreeze" )
    TBool isOk( EFalse );
    if ( iActiveProvider &&
         iActiveProvider->iProvider )
        {
        isOk = ( iProviderInfo.iFreezeSupported && IsFlag( EFlagFrozen ) );
        }
    __VTPRINTEXITR( "LocVideo.OkToUnfreeze %d", isOk )
    return isOk;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::Mute
// Sets the audio state as muted.
// -----------------------------------------------------------------------------
void CVtEngLocalVideo::Mute()
    {
    SetFlag( EFlagAudioMuted );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::UnMute
// Sets the audio state on again.
// -----------------------------------------------------------------------------
void CVtEngLocalVideo::UnMute()
    {
    ClearFlag( EFlagAudioMuted );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetUIForeground
// Sets UI foreground state.
// -----------------------------------------------------------------------------
void CVtEngLocalVideo::SetUIForeground( TBool aForeground )
    {
    __VTPRINTENTER( "LocVideo.SetUIForeground" )
    __VTPRINT2( DEBUG_GEN, " foreground=%d", aForeground )

    //Error indicator
    TInt err = KErrNone;
    if ( aForeground )
        {
        if ( iDelayedSelect.IsDelayedSelectPending() )
            {
            TVtEngProviderType type( iDelayedSelect.DelayedSelectTarget() );
            __VTPRINT2( DEBUG_MEDIA,
                "LocVideo.SetUIForeground delayed switch to %d", type )
            SelectSourceL( type );
            }
        
        SetFlag( EFlagUIForeground );
        ClearFlag( EFlagUIBackground );
        if ( IsFlag( EFlagVFEnabled ) )
            {
            //Ignore error
            TRAP (err, StartViewFinderL( EFalse ));
            }
        else
            {
            MVtEngMedia::TShareObjectState sharingState;
            GetObjectSharingState( sharingState );
            if ( sharingState == MVtEngMedia::ESharingImage )
                {
                //Ignore error
                TRAP (err, StartViewFinderL( ETrue ));
                }
            }
        }
    else
        {
        ClearFlag( EFlagUIForeground );
        SetFlag( EFlagUIBackground );
        StopViewFinder( EFalse );
        ClearFlag( EFlagUIBackground );
        }
    __VTPRINTEXIT( "LocVideo.SetUIForeground" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetObjectSharingState
// Get current media object sharing state.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::GetObjectSharingState(
    MVtEngMedia::TShareObjectState& aObjectSharingState ) const
    {
    __VTPRINTENTER( "LocVid.GetObjectSharingState" )
    if( iActiveProvider && iActiveProvider->iProvider )
        {
        iShareImage->GetObjectSharingState( aObjectSharingState );
        }
    else
        {
        aObjectSharingState = MVtEngMedia::ENotAbleToShare;
        }
    __VTPRINTEXITR( "LocVid.GetObjectSharingState %d", aObjectSharingState )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::InitializeShareImageL
// Start Image sharing initialization.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::InitializeShareImageL(
    const MVtEngMedia::TShareObjectImageParameters& aParameters,
    TBool& aFirstTime )
    {
    __VTPRINTENTER( "LocVid.InitializeShareImageL" )

    iShareImage->InitializeL( aParameters,
    iActiveProvider->iType, aFirstTime );

    __VTPRINTEXIT( "LocVid.InitializeShareImageL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::StartShareImageL
// Stop sharing Image.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::StartShareImageL()
    {
    __VTPRINTENTER( "LocVid.StartShareImageL" )
    const TBool delayed( iDelayedSelect.IsDelayedSelectPending() );
    const TVtEngProviderType type( iDelayedSelect.DelayedSelectTarget() );
    iShareImage->ShareL();
    if ( delayed )
        {
        __VTPRINT2( DEBUG_MEDIA,
            "LocVid.StartShareImageL IsDelayedSelectPending type = %d",
             type )
        iShareImage->NotifyCameraChanged( type );
        }
    __VTPRINTEXIT( "LocVid.StartShareImageL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::StopShareImageL
// Stop sharing Image.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::StopShareImageL()
    {
    __VTPRINTENTER( "LocVid.StopShareImageL" )
    iShareImage->StopL();
    __VTPRINTEXIT( "LocVid.StopShareImageL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ShareError
// Report error to image sharer.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ShareError( TInt aError )
    {
    __VTPRINTENTER( "LocVid.ShareError" )
    iShareImage->Error( aError );
    __VTPRINTEXIT( "LocVid.ShareError" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ShareStoredSource
//
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType CVtEngLocalVideo::ShareStoredSource() const
    {
    __VTPRINTENTER( "LocVid.ShareStoredSource" )
    const TVtEngProviderType stored( iShareImage->StoredSource() );
    __VTPRINTEXITR( "LocVid.ShareStoredSource = %d", stored )
    return stored;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SettingsChanged
// Called when GS settings changed.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SettingsChanged()
    {
    __VTPRINTENTER( "LocVid.SettingsChanged" )
    iShareImage->SettingsChanged();
    __VTPRINTEXIT( "LocVid.SettingsChanged" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetIsPrepareCameraCalled
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetIsPrepareCameraCalled( TBool aCalled )
    {
    __VTPRINTENTER( "LocVid.SetIsPrepareCameraCalled" )
    iPrepareCameraCalled = aCalled;
    __VTPRINTEXITR( "LocVid.SetIsPrepareCameraCalled %d", iPrepareCameraCalled )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsPrepareCameraCalled
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsPrepareCameraCalled() const
    {
    __VTPRINTENTER( "LocVid.IsPrepareCameraCalled" )
    __VTPRINTEXITR( "LocVid.IsPrepareCameraCalled %d", iPrepareCameraCalled )
    return iPrepareCameraCalled;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::HandleLayoutChangeL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::HandleLayoutChangeL()
    {
    __VTPRINTENTER( "LocVid.HandleLayoutChangeL" )
    iLayoutChangeHandler.HandleLayoutChangeL();
    __VTPRINTEXIT( "LocVid.HandleLayoutChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsDelayedSelectPending
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsDelayedSelectPending() const
    {
    return iDelayedSelect.IsDelayedSelectPending();
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::DeleteProvider
// Deletes provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::DeleteProvider(
    TProviderItem& aItem )
    {
    __VTPRINTENTER( "LocVid.DeleteProvider" )
    aItem.iProviderState = EUndefined;
    if ( aItem.iProvider &&
         aItem.iInitialized &&
         aItem.iProvider->ViewFinderActive() )
        {
        aItem.iProvider->StopViewFinder();
        }
    delete aItem.iProvider;
    aItem.iInitialized = EFalse;
    aItem.iProvider = NULL;
    __VTPRINTEXIT( "LocVid.DeleteProvider" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::DeleteProvider
// Deletes provider.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::DeleteProvider( const TVtEngProviderType aType )
    {
    TProviderItem dummy;
    const TInt pos( GetProviderByType( aType, dummy ) );
    if ( pos >= 0 ) // valid index
        {
        TProviderItem& item = ( *iVideoProviders )[pos];
        DeleteProvider( item );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::Reset
// Resets local video handler.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::Reset()
    {
    __VTPRINTENTER( "LocVid.Reset" )

    __VTPRINT( DEBUG_MEDIA, "LocVideo.Reset" )
    DeleteProvider( KVtEngProviderCam1 );
    DeleteProvider( KVtEngProviderCam2 );
    DeleteAudioSource();

    ClearFlag( EFlagAudioMuted |
               EFlagSignalSourceSelection |
               EFlagInitialized |
               EFlagVFEnabled |
               EFlagFrozen );

    __VTPRINT( DEBUG_MEDIA, "LocVideo.Reset clearing EFlagVFEnabled and other" )

    StopViewFinder();
    __VTPRINTEXIT( "LocVid.Reset" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetDigitalZoomFactorL
// Sets video provider's digital zoom step.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetDigitalZoomFactorL( TInt aDigitalZoomFactor )
    {
    __VTPRINTENTER( "LocVid.SetDigitalZoomFactorL" )
    __VTPRINT2( DEBUG_MEDIA, "LocVideo.SetZoom %d", aDigitalZoomFactor )
    if ( !iActiveProvider ||
         !iActiveProvider->iProvider )
        {
        User::Leave( KErrNotReady );
        }
    CVSDataProvider* provider( iActiveProvider->iProvider );
    provider->SetDigitalZoomFactorL( aDigitalZoomFactor );
    iActiveProvider->iStoredZoomFactor = provider->DigitalZoomFactor();
    __VTPRINTEXIT( "LocVid.SetDigitalZoomFactorL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetFlag
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetFlag( const TInt aFlag )
    {
    iFlags |= aFlag;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ClearFlag
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ClearFlag( const TInt aFlag )
    {
    iFlags &= ~aFlag;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsFlag
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsFlag( const TInt aFlag ) const
    {
    return ( ( iFlags & aFlag ) != 0 );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CalculateMajorDigitalZoomStepCount
// Utility function to calculate amount of major zoom steps.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CalculateMajorDigitalZoomStepCount(
    CVSDataProvider* aProvider )
    {
    TVSDataProviderInfo info;
    aProvider->ProviderInfo( info );
    return CalculateMajorDigitalZoomStepCount( info );
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CalculateMajorDigitalZoomStepCount
// Utility function to calculate amount of major zoom steps.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CalculateMajorDigitalZoomStepCount(
    const TVSDataProviderInfo& aInfo ) const
    {
    TInt zStepCount( aInfo.iMaxDigitalZoom ?
        aInfo.iMaxDigitalZoom + 1 : 0 );
    TInt maxZStepCount( KVtMaxZoomStep + 1 );
    return zStepCount > maxZStepCount ? maxZStepCount : zStepCount;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::TProviderItem
// Constructor.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TProviderItem::TProviderItem() :
    iProvider( NULL ),
    iType( KVtEngProviderUndefined ),
    iInitialized( EFalse ),
	iErrorCode( 0 ),
    iProviderState( EUndefined ),
    iStoredZoomFactor( 0 ),
    iZFErr( KErrNone ),
    iCamPrefs( NULL ),
    iFirstTime( ETrue )
    {
    __VTPRINTENTER( "LocVid.TProviderItem.ctor" )
    ResetStoredValues();
    __VTPRINTEXITR( "LocVid.TProviderItem.ctor $%x", TInt( this ) )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::~TProviderItem
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TProviderItem::~TProviderItem()
    {
    __VTPRINTENTER( "LocVid.TProviderItem.~" )
    DetachFromCameraPreferences();
    __VTPRINTEXITR( "LocVid.TProviderItem.~ $%x", TInt( this ) )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::ResetStoredValues
// Resets stored values.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::ResetStoredValues()
    {
    __VTPRINTENTER( "LocVid.ResetStoredValues" )
    DetachFromCameraPreferences();
    __VTPRINTEXIT( "LocVid.ResetStoredValues" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::RestoreZoomFactor
// Tries to restore zoom factor.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::TProviderItem::RestoreZoomFactor()
    {
    __VTPRINTENTER( "LocVid.RestoreZoomFactor" )
    TRAP( iZFErr,
        {
        if ( iProvider && !iZFErr )
            {
            iProvider->SetDigitalZoomFactorL( iStoredZoomFactor );
            iStoredZoomFactor = iProvider->DigitalZoomFactor();
            __VTPRINT2( DEBUG_GEN, "LocVideo.RestoreZoomFactor ZF now = %d",
                iStoredZoomFactor )
            }
        } );
    __VTPRINTEXITR( "LocVid.RestoreZoomFactor %d", iZFErr )
    return iZFErr;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::RestorePreferences
// Tries to restore preferences.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::TProviderItem::RestorePreferences()
    {
    __VTPRINTENTER( "LocVid.RestorePreferences" )
    TRAPD( error,
        {
        if ( iCamPrefs && iProvider && !iFirstTime )
            {
            if ( !iCtErr )
                {
                iProvider->SetColorToneL(
                    CCamera::CCameraImageProcessing::TEffect( iColorTone ) );
                }
            if ( !iWbErr )
                {
                iProvider->SetWhiteBalanceL(
                    CCamera::TWhiteBalance( iWhiteBalance ) );
                }
            if ( !iBrErr )
                {
                iProvider->SetBrightnessL( iBrightness );
                }
            if ( !iCrErr )
                {
                iProvider->SetContrastL( iContrast );
                }
            }
        } );
    __VTPRINTEXITR( "LocVid.RestorePreferences %d", error )
    return error;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::Attach
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::Attach( MVtEngCameraPreferences&
    aCamPrefs )
    {
    __VTPRINTENTER( "LocVid.Attach" )
    iCamPrefs = &aCamPrefs;
    if ( iFirstTime )
        {
        iCtErr = iCamPrefs->GetColorTone( iColorTone );
        iWbErr = iCamPrefs->GetWhiteBalance( iWhiteBalance );
        iBrErr = iCamPrefs->GetBrightness( iBrightness );
        iCrErr = iCamPrefs->GetContrast( iContrast );
        iFirstTime = EFalse;
        }
    __VTPRINTEXITR( "LocVid.Attach $%x", TInt( iCamPrefs ) )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::ColorToneUpdated
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::ColorToneUpdated(
    MVtEngCameraPreferences::TColorTone aColorTone )
    {
    __VTPRINTENTER( "LocVid.ColorToneUpdated" )
    iCtErr = KErrNone;
    iColorTone = aColorTone;
    __VTPRINTEXITR( "LocVid.ColorToneUpdated %d", aColorTone )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::WhiteBalanceUpdated
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::WhiteBalanceUpdated(
    MVtEngCameraPreferences::TWhiteBalance aWhiteBalance )
    {
    __VTPRINTENTER( "LocVid.WhiteBalanceUpdated" )
    iWbErr = KErrNone;
    iWhiteBalance = aWhiteBalance;
    __VTPRINTEXITR( "LocVid.WhiteBalanceUpdated %d", aWhiteBalance )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::BrightnessUpdated
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::BrightnessUpdated( TInt aBrightness )
    {
    __VTPRINTENTER( "LocVid.BrightnessUpdated" )
    iBrErr = KErrNone;
    iBrightness = aBrightness;
    __VTPRINTEXITR( "LocVid.BrightnessUpdated %d", aBrightness )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::ContrastUpdated
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::ContrastUpdated( TInt aContrast )
    {
    __VTPRINTENTER( "LocVid.ContrastUpdated" )
    iCrErr = KErrNone;
    iContrast = aContrast;
    __VTPRINTEXITR( "LocVid.ContrastUpdated %d", aContrast )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::Detach
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::Detach( MVtEngCameraPreferences& )
    {
    __VTPRINTENTER( "LocVid.Detach" )
    iCamPrefs = NULL;
    __VTPRINTEXIT( "LocVid.Detach" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TProviderItem::DetachFromCameraPreferences
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TProviderItem::DetachFromCameraPreferences()
    {
    __VTPRINTENTER( "LocVid.DetachFromCameraPreferences" )
    if ( iCamPrefs )
        {
        static_cast< CVtEngCameraPreferences* >( iCamPrefs )->Detach( *this );
        iCamPrefs = NULL;
        }
    __VTPRINTEXIT( "LocVid.DetachFromCameraPreferences" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CreateAudioSource
// Creates audio source.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CreateAudioSource()
    {
    __VTPRINTENTER( "LocVid.CreateAudioSource" )
    TInt err = KErrNone;
    // Check if this really a target compilation
#if !defined( __WINS__ )
    iAudioSource = VTProtocolFactory::CreateAudioSource();
    if ( !iAudioSource )
        {
        err = KErrNoMemory;
        }
#endif // !(__WINS__)
    __VTPRINTEXITR( "LocVid.CreateAudioSource %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::DeleteAudioSource
// Deletes audio source.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::DeleteAudioSource()
    {
    __VTPRINTENTER( "LocVid.DeleteAudioSource" )
    if( iAudioSource )
        {
        VTProtocolFactory::DeletAudioSource(iAudioSource);
        iAudioSource = NULL;
        }
    __VTPRINTEXIT( "LocVid.DeleteAudioSource" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::PauseVideoSending
// Pause video sending.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::PauseVideoSending()
    {
    __VTPRINTENTER( "LocVid.PauseVideoSending" )
    if ( iSource )
        {
        iSource->PauseSending();
        }
    __VTPRINTEXIT( "LocVid.PauseVideoSending" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ResumeVideoSending
// Resume video sending.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ResumeVideoSending()
    {
    __VTPRINTENTER( "LocVid.ResumeVideoSending" )
    if ( iSource )
        {
        iSource->ResumeSending();
        }
    __VTPRINTEXIT( "LocVid.ResumeVideoSending" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ViewFinderStarted
// Is view finder started.
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::ViewFinderStarted() const
    {
    CVSDataProvider* provider = NULL;
    if( iActiveProvider )
        {
         provider = iActiveProvider->iProvider;
        }
    if ( !provider )
        {
        return EFalse;
        }
    return provider->ViewFinderActive();
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ViewFinderStarted
// Is view finder started.
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsFrozen() const
    {
    TBool result = IsFlag( EFlagFrozen );
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::ResetIndices
// Resets the provider info indices.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::ResetIndices()
    {
    iProviderInfoIndexCam1 = KErrNotFound;
    iProviderInfoIndexCam2 = KErrNotFound;
    iProviderInfoIndexStill = KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetCameraTracking
// Sets a listener for camera P & S data.
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::SetCameraTrackingL( TInt& aProviderIndex )
    {
    __VTPRINTENTER( "LocVideo.SetCameraTracking" )
    iCameraListener = CVtEngPubSubsListener::NewL( KPSUidVTMediaConfiguration,
        KPSPreferredCamera, this );

    __VTPRINTEXIT( "LocVideo.SetCameraTracking" )
    return iCameraListener->Get( aProviderIndex );
    }

// -----------------------------------------------------------------------------
// CVtEngInitializer::HandleNotifyPSL
// Notification of changed port value.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::HandleNotifyPSL(
    const TUid /*aUid */, const TInt& /* aKey */,
    const TRequestStatus& aStatus )
    {
    __VTPRINTENTER( "LocVideo.handleNotifyPSL " )
    if ( aStatus.Int() == KErrNone )
        {
        // Get the new camera value.
        TInt cameraId( EPSPreferredCameraUnknown );
        iCameraListener->Get( cameraId );
        // No need to check error, since MapCameraInternally will return
        // current camera, and no actions will be taken.
        MVtEngMedia::TCameraId camId( MapCameraInternally( cameraId ) );

        TVtEngProviderType providerType( KVtEngProviderUndefined );
        TInt result( KErrNone );
        switch( camId )
            {
            case MVtEngMedia::EPrimaryCamera:
                if( IsFlag( EFlagCamera1Available ) )
                    {
                    providerType = KVtEngProviderCam1;
                    }
                else
                    {
                    result = KErrNotFound;
                    }
                break;

            case MVtEngMedia::ESecondaryCamera:
                if( IsFlag( EFlagCamera2Available ) )
                    {
                    providerType = KVtEngProviderCam2;
                    }
                else
                    {
                    result = KErrNotFound;
                    }
                break;

            default:
                result = KErrNotSupported;
                break;
            }

        User::LeaveIfError( result );

        // If we're sharing, then notify camera change to image sharer
        if ( GetMediaSource() == MVtEngMedia::EMediaShare ||
             iShareImage->IsInitializingShare() )
            {
            iShareImage->NotifyCameraChanged( providerType );
            }
        // If current provider is camera
        else if ( GetMediaSource() == MVtEngMedia::EMediaCamera )
            {
            // If current camera is different than new
            if ( iCurrentCamera != camId )
                {
                // If VT is in the foreground and Provider is unfrozen
                if ( IsFlag( EFlagUIForeground ) && !IsFrozen() && !iObserver.IsVideoChannelClosed() )
                    {
                    // select camera..
                    SelectSourceL( providerType );
                    }
                else
                    {
                    iObserver.SetSelectedCameraId( camId );
                    // ..else notify delayed select
                    iLayoutChangeHandler.CancelHandleLayoutChange();
                    iDelayedSelect.NotifyDelayedSelect( providerType );
                    }
                }
            else
                {
                // else ignore
                }
            }
        // Else store camera type and on next SelectSourceL() use this instead
        else
            {
            iObserver.SetSelectedCameraId( camId );
            // If prepare camera has not been called, then just
            // update preferred camera
            if ( GetMediaSource()!= MVtEngMedia::EMediaNone && 
                    ( IsPrepareCameraCalled() || iObserver.IsVideoChannelClosed() ) )
                {
                iLayoutChangeHandler.CancelHandleLayoutChange();
                iDelayedSelect.NotifyDelayedSelect( providerType );
                }
            else
                {
                iCurrentCamera = camId;
                }
            }
        }
    __VTPRINTEXIT( "LocVideo.HandleNotifyPSL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::SetCurrentCameraId
//
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::SetCurrentCameraId( MVtEngMedia::TCameraId aCamId )
    {
    __VTPRINTENTER( "LocVideo.SetCurrentCameraId" )
    iCurrentCamera = aCamId;
    iObserver.SetSelectedCameraId( aCamId );
    __VTPRINTEXITR( "LocVideo.SetCurrentCameraId camId = %d", aCamId )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::MapCameraInternally
// Maps camera id to internal data structures.
// -----------------------------------------------------------------------------
//
MVtEngMedia::TCameraId CVtEngLocalVideo::MapCameraInternally( TInt aCameraId )
    {
    __VTPRINT2( DEBUG_GEN, "LocVideo.MapCameraInternally cameraId=%d",
        aCameraId )
    if ( aCameraId == EPSPreferredCameraInwards ||
         iNumSupportedCameras == 1 )
        {
        return MVtEngMedia::EPrimaryCamera;
        }
    else  if ( aCameraId == EPSPreferredCameraOutwards )
        {
        return MVtEngMedia::ESecondaryCamera;
        }
    __VTPRINT2( DEBUG_GEN, "LocVideo.MapCameraInternally curCam=%d", iCurrentCamera )
    return iCurrentCamera;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CameraAvailable
// Checks camera availabitility.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::
    CameraAvailable( CVtEngLocalVideo::TVtEngProviderType &aType )
    {
    __VTPRINT2( DEBUG_GEN, "LocVideo.CameraAvailable In.type=%d ", aType )
    if ( iCurrentCamera == MVtEngMedia::EPrimaryCamera )
        {
        if ( IsFlag( EFlagCamera1Available ) )
            {
            aType = KVtEngProviderCam1;
            }
        }
    else if ( iCurrentCamera == MVtEngMedia::ESecondaryCamera )
        {
        if ( IsFlag( EFlagCamera2Available ) )
            {
            aType = KVtEngProviderCam2;
            }
        }
    __VTPRINT2( DEBUG_GEN, "LocVideo.CameraAvailable Out.type=%d", aType )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::IsProviderWaiting
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::IsProviderWaiting( TVtEngProviderType aType ) const
    {
    __VTPRINTENTER( "LocVid.IsProviderWaiting" )
    TBool isWaiting( EFalse );
    TInt indexWaiting( ProviderIndexByState( EWaiting ) );
    if ( indexWaiting != KErrNotFound )
        {
        TProviderItem& item = ( *iVideoProviders )[ indexWaiting ];
        isWaiting = ( item.iType == aType );
        }
    __VTPRINTEXITR( "LocVid.IsProviderWaiting %d", isWaiting )
    return isWaiting;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::GetHighestSelectableProvider
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::GetHighestSelectableProvider(
    CVtEngLocalVideo::TVtEngProviderType& aPreferred ) const
    {
    __VTPRINTENTER( "LocVid.GetHighestSelectableProvider" )
    const TVtEngProviderType preferred = aPreferred;
    switch ( preferred )
        {
        case KVtEngProviderCam1:
            if ( IsFlag( EFlagCamera1Available ) )
                {
                // cam1 is selectable, jump out
                break;
                }
            // othwerwise fall through to next in priority order
        case KVtEngProviderCam2:
            if ( IsFlag( EFlagCamera2Available ) &&
            // Do not allow change to another camera without user permission,
            // i.e. test preferred was cam1 but it wasn't available.
                preferred != KVtEngProviderCam1 )
                {
                // cam2 is selectable
                aPreferred = KVtEngProviderCam2;
                break;
                }
            // othwerwise fall through to next in priority order
        case KVtEngProviderImage:
            if ( CVtEngUtility::Settings().Config().iVideo.iImageIsValid )
                {
                aPreferred = KVtEngProviderImage;
                break;
                }
            // othwerwise fall through to next in priority order
        case KVtEngProviderNone:
        default:
            // always available
            aPreferred = KVtEngProviderNone;
            break;
        }
    __VTPRINTEXITR( "LocVid.GetHighestSelectableProvider %d", aPreferred )
    return ( preferred == aPreferred );
    }

// Implementation of CVtEngLocalVideo::CVtEngShareImage

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::NewL
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngShareImage* CVtEngLocalVideo::CVtEngShareImage::NewL(
    CVtEngLocalVideo& aLocalVideo )
    {
    CVtEngShareImage* self = new ( ELeave ) CVtEngShareImage( aLocalVideo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::~CVtEngShareImage
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngShareImage::~CVtEngShareImage()
    {
    DeleteProviders();
    iBuffered.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::InitializeL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::InitializeL(
    const MVtEngMedia::TShareObjectImageParameters& aParameters,
    TVtEngProviderType aCurrent,
    TBool& aFirstTime )
    {
    __VTPRINTENTER( "LocVid.ShareImage.InitializeL" )
    if ( IsInitializing() )
        {
        // Already initializing.
        User::Leave( KErrNotReady );
        }


    CVtEngLocalVideo::TProviderItem* freeProviderItem = FetchProviderItem();
    if ( !freeProviderItem )
        {
        User::Leave( KErrInUse );
        }

    iParameters = aParameters;
    CreateProviderL( *freeProviderItem );
    InitializeProviderL( *freeProviderItem );

    // Only set stored type when sharing is started, if resharing (i.e. sharing
    // new image when already sharing other image), then don't change stored
    // type.
    if ( iStoredType == KVtEngProviderUndefined )
        {
        iSettingsChanged = EFalse;
        iOriginalStoredType = iStoredType = aCurrent;
        aFirstTime = ETrue;
        }
    else
        {
        aFirstTime = EFalse;
        }
    iInitializingProvider = freeProviderItem;
    SetState( EInitializing );
    __VTPRINTEXIT( "LocVid.ShareImage.InitializeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::ShareL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::ShareL()
    {
    __VTPRINTENTER( "LocVid.ShareImage.ShareL" )
    if ( !IsInitializing() && iInitializingProvider )
        {
        User::Leave( KErrNotReady );
        }
    iActiveProvider = iInitializingProvider;
    iInitializingProvider = NULL;
    iLocalVideo->SwitchProvider( *iActiveProvider );
    SetState( ESharing );
    __VTPRINTEXIT( "LocVid.ShareImage.ShareL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::StopL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::StopL()
    {
    __VTPRINTENTER( "LocVid.ShareImage.StopL" )
    iLocalVideo->InitializeOnly( EFalse );

    CVtEngLocalVideo::TVtEngProviderType target( StoredSource() );
    if ( iOriginalStoredType != target )
        {
        TProviderItem* item = NULL;
        if ( iLocalVideo->GetProviderByType( iOriginalStoredType, item )
             != KErrNotFound )
            {
            if ( item->iProviderState != EPermanent )
                {
                iLocalVideo->DeleteProvider( *item );
                }
            }
        }

    if ( iSettingsChanged )
        {
        if ( target == KVtEngProviderDefaultStillImage )
            {
            DeleteProvider( target );
            }
        DeleteProvider( KVtEngProviderImage );
        }

    __VTPRINT2( DEBUG_MEDIA, "LocVid.ShareImage.StopL select %d",
        target )
    iLocalVideo->SelectSourceL( target );

    if ( iStoredType == KVtEngProviderCam1 ||
         iStoredType == KVtEngProviderCam2 )
        {
        MVtEngMedia::TCameraId camId(
            iStoredType == KVtEngProviderCam1 ?
                MVtEngMedia::EPrimaryCamera :
                 MVtEngMedia::ESecondaryCamera );
        iLocalVideo->SetCurrentCameraId( camId );
        __VTPRINT2( DEBUG_MEDIA, "LocVid.ShareImage.StopL set camId %d",
            camId )
        }
    SetState( EStopped );
    __VTPRINTEXIT( "LocVid.ShareImage.StopL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::Error
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::Error( TInt
#ifdef _DEBUG
    aError
#endif // _DEBUG
    )
    {
    TState stateAfter( EStopped );
    __VTPRINTENTER( "LocVid.ShareImage.Error" )
    if ( IsInitializing() )
        {
        iInitializingProvider = NULL;
        stateAfter = iPreviousState;
        }
    else if ( iState == ESharing )
        {
        if ( iLocalVideo->iObserver.GetPendingCommand() != KVtEngInitializeShareImage )
            {
            CVtEngEventManager::NotifyEvent( KVtEngShareImageDecodingError );
            }
        stateAfter = ESharing;
        //iActiveProvider = NULL;
        }
    SetState( stateAfter );
#ifdef _DEBUG
    __VTPRINTEXITR( "LocVid.ShareImage.Error %d", aError )
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::StoredSource
//
// Returns stored source for this share session. Stored source may have changed
// during share (e.g. when GS still image is being sent but during share
// GS still image is disabled => still image cannot be used anymore and thus
// None provider must be activated). All these conditions are checked by this
// method.
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType
    CVtEngLocalVideo::CVtEngShareImage::StoredSource() const
    {
    __VTPRINTENTER( "LocVid.ShareImage.StoredSource" )
    CVtEngLocalVideo::TVtEngProviderType stored( iStoredType );

    if ( iOriginalStoredType == KVtEngProviderNone ||
         iOriginalStoredType == KVtEngProviderImage ||
         iOriginalStoredType == KVtEngProviderDefaultStillImage )
        {
        if ( CVtEngUtility::Settings().Config().iVideo.iImageIsValid )
            {
            stored = KVtEngProviderImage;
            }
        else if ( CVtEngUtility::Settings().IsDefaultStillImageDefined() )
            {
            stored = KVtEngProviderDefaultStillImage;
            }
        else
            {
            stored = KVtEngProviderNone;
            }
        }
    __VTPRINTEXITR( "LocVid.ShareImage.StoredSource = %d", stored )
    return stored;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::CVtEngShareImage
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngShareImage::CVtEngShareImage(
    CVtEngLocalVideo& aLocalVideo )
    : iLocalVideo( &aLocalVideo ), iState( EStopped ),
      iStoredType( KVtEngProviderUndefined ),
      iOriginalStoredType( KVtEngProviderUndefined ),
      iBuffered( 2 ), iSettingsChanged( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::ConstructL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::ConstructL()
    {
    __VTPRINTENTER( "LocVid.ShareImage.ConstructL" )
    for ( TInt i = 0; i < KVtEngNumProvidersBuffered; i++ )
        {
        TProviderItem* item = new (ELeave) TProviderItem();
        CleanupStack::PushL( item );
        User::LeaveIfError( iBuffered.Append( item ) );
        CleanupStack::Pop(); // item
        }
    __VTPRINTEXIT( "LocVid.ShareImage.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::Parameters
// -----------------------------------------------------------------------------
//
const MVtEngMedia::TShareObjectImageParameters&
    CVtEngLocalVideo::CVtEngShareImage::Parameters() const
    {
    __VTPRINTENTER( "LocVid.ShareImage.Parameters" )
    __VTPRINTEXIT( "LocVid.ShareImage.Parameters" )
    return iParameters;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::IsInitializing
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::CVtEngShareImage::IsInitializing() const
    {
    TBool isInitializing( iState == EInitializing );
    __VTPRINTENTER( "LocVid.ShareImage.IsInitializing" )
    __VTPRINTEXITR( "LocVid.ShareImage.IsInitializing %d", isInitializing )
    return isInitializing;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::SetState
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::SetState( const TState aNewState )
    {
    __VTPRINTENTER( "LocVid.ShareImage.SetState" )
    // Signal UI when entering or leaving initializing state
    if ( aNewState == EInitializing )
        {
        CVtEngEventManager::NotifyEvent( KVtEngShareImageInitializeBegin );
        }
    else if ( iState == EInitializing )
        {
        CVtEngEventManager::NotifyEvent( KVtEngShareImageInitializeEnd );
        }
    // Reset stored type when stopping
    if( aNewState == EStopped )
        {
        iOriginalStoredType = iStoredType = KVtEngProviderUndefined;
        iSettingsChanged = EFalse;
        }
    iPreviousState = iState;
    iState = aNewState;
    __VTPRINTEXITR( "LocVid.ShareImage.SetState iState = %d", iState )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::GetProviderItemL
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TProviderItem*
    CVtEngLocalVideo::CVtEngShareImage::FetchProviderItem() const
    {
    __VTPRINTENTER( "LocVid.ShareImage.FetchProviderItem" )
    for( TInt i = 0; i < iBuffered.Count(); i++ )
        {
        if( ( iBuffered[ i ] != iActiveProvider ) &&
            ( iBuffered[ i ] != iInitializingProvider ) )
            {
            __VTPRINTEXIT( "LocVid.ShareImage.FetchProviderItem" )
            return iBuffered[ i ];
            }
        }
    __VTPRINTEXIT( "LocVid.ShareImage.FetchProviderItem" )
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::CreateProviderL
// -----------------------------------------------------------------------------
void CVtEngLocalVideo::CVtEngShareImage::CreateProviderL(
    TProviderItem& aProviderItem )
    {
    __VTPRINTENTER( "LocVid.ShareImage.CreateProviderL" )
    iLocalVideo->ReCreateProviderL( aProviderItem, KVtEngProviderShareImage );
    __VTPRINTEXIT( "LocVid.ShareImage.CreateProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::InitializeProviderL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::InitializeProviderL(
    TProviderItem& aProviderItem )
    {
    __VTPRINTENTER( "LocVid.ShareImage.InitializeProviderL" )
    if( IsInitializing() )
        {
        User::Leave( KErrNotReady );
        }
    TPckgBuf< TVSStillImageDataProviderParams > pckg( Parameters() );
#ifdef _DEBUG
    RDebug::Print( _L( "LocVid.ShareImage.InitializeProviderL: %S" ),
        &( pckg().iFileName ) );
#endif // _DEBUG
    iLocalVideo->InitializeShareProviderL( aProviderItem, pckg );
    __VTPRINTEXIT( "LocVid.ShareImage.InitializeProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::DeleteProvider
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::DeleteProvider(
    TVtEngProviderType aProviderType )
    {
    __VTPRINTENTER( "LocVid.ShareImage.DeleteProvider" )
    TProviderItem* pItem;
    if ( iLocalVideo->GetProviderByType( aProviderType, pItem )
         != KErrNotFound )
        {
        iLocalVideo->DeleteProvider( *pItem );
        }
    __VTPRINTEXIT( "LocVid.ShareImage.DeleteProvider" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::GetObjectSharingState
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::GetObjectSharingState(
    MVtEngMedia::TShareObjectState& aObjectSharingState ) const
    {
    __VTPRINTENTER( "LocVid.ShareImage.GetObjectSharingState" )
    switch ( iState )
        {
        case EStopped:
            aObjectSharingState = MVtEngMedia::ENotSharing;
            break;

        case EInitializing:
            aObjectSharingState = MVtEngMedia::EInitializingShareImage;
            break;

        case ESharing:
            aObjectSharingState = MVtEngMedia::ESharingImage;
            break;

        default:
            aObjectSharingState = MVtEngMedia::ENotAbleToShare;
            break;
        }
    __VTPRINTEXIT( "LocVid.ShareImage.GetObjectSharingState" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::IsInitializingShare
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::CVtEngShareImage::IsInitializingShare() const
    {
    __VTPRINTENTER( "LocVid.ShareImage.IsInitializingShare" )
    const TBool result( iState == EInitializing );
    __VTPRINTEXITR( "LocVid.ShareImage.IsInitializingShare %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::SettingsChanged
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::SettingsChanged()
    {
    __VTPRINTENTER( "LocVid.ShareImage.SettingsChanged" )
    iSettingsChanged = ETrue;
    __VTPRINTEXIT( "LocVid.ShareImage.SettingsChanged" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::ProviderSwitchDone
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::ProviderSwitchDone(
    CVSDataProvider* aOldProvider )
    {
    __VTPRINTENTER( "LocVid.ShareImage.ProviderSwitchDone" )
    //if ( iState == EStopped )
        {
        for ( TInt i = iBuffered.Count() - 1; i >= 0; i-- )
            {
            if ( iBuffered[ i ]->iProvider == aOldProvider )
                {
                iLocalVideo->DeleteProvider( *iBuffered[ i ] );
                break;
                }
            }
        }
    __VTPRINTEXIT( "LocVid.ShareImage.ProviderSwitchDone" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::NotifyCameraChanged
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::NotifyCameraChanged(
    const TVtEngProviderType& aProviderType )
    {
    __VTPRINTENTER( "LocVid.ShareImage.NotifyCameraChanged" )
    if ( ( iState == EInitializing ) || ( iState == ESharing ) )
        {
        iStoredType = aProviderType;
        }
    __VTPRINTEXIT( "LocVid.ShareImage.NotifyCameraChanged" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::DeleteProviders
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::DeleteProviders()
    {
    __VTPRINTENTER( "LocVid.ShareImage.DeleteProviders" )
    for ( TInt i = 0; i < iBuffered.Count(); i++ )
        {
        iLocalVideo->DeleteProvider( *iBuffered[ i ] );
        }
    __VTPRINTEXIT( "LocVid.ShareImage.DeleteProviders" )
    }


// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngShareImage::CancelInitialize
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngShareImage::CancelInitialize()
    {
    __VTPRINTENTER( "CVtEngLocalVideo.CVtEngShareImage.CancelInitialize");
    if( iInitializingProvider )
        {
        if(iInitializingProvider->iProvider)
            {
            __VTPRINT( DEBUG_MEDIA,"iInitializingProvider->iProvider->CancelInitializing()");
            iInitializingProvider->iProvider->CancelInitializing();
            }
        }
    __VTPRINTEXIT( "CVtEngLocalVideo.CVtEngShareImage.CancelInitialize");
    }
// Implementation of CVtEngLocalVideo::TVtEngDelayedSelect

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngDelayedSelect::TVtEngDelayedSelect
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngDelayedSelect::TVtEngDelayedSelect()
    : iDelayedProviderType( KVtEngProviderUndefined )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngDelayedSelect::NotifyDelayedSelect
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngDelayedSelect::NotifyDelayedSelect(
    const TVtEngProviderType& aProviderType )
    {
    __VTPRINTENTER( "LocVid.DelSel.NotifyDelayedSelect" )
    iDelayedProviderType = aProviderType;
    __VTPRINTEXIT( "LocVid.DelSel.NotifyDelayedSelect" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngDelayedSelect::IsDelayedSelectPending
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::TVtEngDelayedSelect::IsDelayedSelectPending() const
    {
    __VTPRINTENTER( "LocVid.DelSel.IsDelayedSelectPending" )
    const TBool result( iDelayedProviderType != KVtEngProviderUndefined );
    __VTPRINTEXITR( "LocVid.DelSel.IsDelayedSelectPending %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngDelayedSelect::DelayedSelectTarget
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngProviderType
CVtEngLocalVideo::TVtEngDelayedSelect::DelayedSelectTarget()
    {
    __VTPRINTENTER( "LocVid.DelSel.DelayedSelectTarget" )
    TVtEngProviderType current( iDelayedProviderType );
    iDelayedProviderType = KVtEngProviderUndefined;
    __VTPRINTEXITR( "LocVid.DelSel.DelayedSelectTarget %d", current )
    return current;
    }

// Implementation of CVtEngLocalVideo::TVtEngLayoutChangeHandler

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::TVtEngLayoutChangeHandler
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::TVtEngLayoutChangeHandler::TVtEngLayoutChangeHandler(
     CVtEngLocalVideo& aLocalVideo )
    : iLocalVideo( &aLocalVideo ), iState( EIdle ),
      iStored( KVtEngProviderUndefined )
    {
    __VTPRINTEXIT( "LocVid.LayoutCH.Ctor" )
    __VTPRINTEXIT( "LocVid.LayoutCH.Ctor" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::UnfreezeL
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::TVtEngLayoutChangeHandler::UnfreezeL()
    {
    TBool result( EFalse );
    __VTPRINTENTER( "LocVid.LayoutCH.UnfreezeL" )
    if ( iState == ELayoutChangePending )
        {
        result = ETrue;
        DoHandleLayoutChangeL();
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.UnfreezeL %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::HandleLayoutChangeL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngLayoutChangeHandler::HandleLayoutChangeL()
    {
    __VTPRINTENTER( "LocVid.LayoutCH.HandleLayoutChangeL" )
    TInt error( KErrNone );
    if ( iState != EIdle )
        {
        if ( iState == ELayoutChangePending )
            {
            __VTPRINT( DEBUG_MEDIA,
                "LocVid.LayoutCH.HandleLayoutChangeL PENDING" )
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA,
                "LocVid.LayoutCH.HandleLayoutChangeL NOT ready" )
            error = KErrNotReady;
            }
        CompleteOperation( error );
        }
    else
        {
        // Only do reinitialize if camera is active source
        if ( iLocalVideo->GetMediaSource() == MVtEngMedia::EMediaCamera &&
             !iLocalVideo->IsDelayedSelectPending() )
            {
            if ( !iLocalVideo->IsFrozen() )
                {
                __VTPRINT( DEBUG_MEDIA,
                    "LocVid.LayoutCH.HandleLayoutChangeL camera active, reinitialize" )
                DoHandleLayoutChangeL();
                }
            else
                {
                iState = ELayoutChangePending;
                __VTPRINT( DEBUG_MEDIA,
                    "LocVid.LayoutCH.HandleLayoutChangeL flag set, frozen" )
                CompleteOperation( error );
                }
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA,
                "LocVid.LayoutCH.HandleLayoutChangeL no op, camera is not active" )
            CompleteL( error );
            }
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.HandleLayoutChangeL %d", error )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::CancelHandleLayoutChange
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngLayoutChangeHandler::CancelHandleLayoutChange()
    {
    __VTPRINTENTER( "LocVid.LayoutCH.CancelHandleLayoutChange" )
    if ( iState == ELayoutChangePending )
        {
        iState = EIdle;
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.CancelHandleLayoutChange iState = %d",
        iState )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::DoHandleLayoutChangeL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngLayoutChangeHandler::DoHandleLayoutChangeL()
    {
    __VTPRINTENTER( "LocVid.LayoutCH.DoHandleLayoutChangeL" )
    iStored = iLocalVideo->ActiveProvider();
    iState = ESwitchingBlank;
    TVtEngProviderType providerType( KVtEngProviderNone );
    // Use user defined image, if available
    if ( CVtEngUtility::Settings().IsDefaultStillImageDefined() )
        {
        providerType = KVtEngProviderDefaultStillImage;
        }
    iLocalVideo->SelectSourceL( providerType );
    __VTPRINTEXIT( "LocVid.LayoutCH.DoHandleLayoutChangeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::Complete
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::TVtEngLayoutChangeHandler::Complete( const TInt aError )
    {
    __VTPRINTENTER( "LocVid.LayoutCH.Complete" )
    TRAPD( error, CompleteL( aError ) );
    __VTPRINTEXITR( "LocVid.LayoutCH.Complete %d", error )
    return error;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::CompleteL
// Resets internal state and completes asynchronous operation.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngLayoutChangeHandler::CompleteL( const TInt aError )
    {
    __VTPRINTENTER( "LocVid.LayoutCH.CompleteL" )
    iStored = KVtEngProviderUndefined;
    iState = EIdle;
    CompleteOperation( aError );
    __VTPRINTEXIT( "LocVid.LayoutCH.CompleteL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::CompleteOperation
// Completes asynchronous operation.
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::TVtEngLayoutChangeHandler::CompleteOperation(
    const TInt aError )
    {
    __VTPRINTENTER( "LocVid.LayoutCH.CompleteOperation" )
    iLocalVideo->CompleteOperation( aError );
    __VTPRINTEXIT( "LocVid.LayoutCH.CompleteOperation" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderError
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderError( TInt aError )
    {
    TBool handled( EFalse );
    __VTPRINTENTER( "LocVid.LayoutCH.ProviderError" )
    if ( iState != EIdle && iState != ELayoutChangePending )
        {
        handled = ETrue;
        Complete( aError );
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.ProviderError %d", handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderReady
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderReady()
    {
    TBool handled( EFalse );
    __VTPRINTENTER( "LocVid.LayoutCH.ProviderReady" )
    switch ( iState )
        {
    case ESwitchingCamera:
    case EIdle:
        break;

    case EInitializingCamera:
        handled = ETrue;
        iState = ESwitchingCamera;
        TRAPD( error, iLocalVideo->SelectSourceL( iStored ) );
        if ( error )
            {
            Complete( error );
            }
        break;

    case ESwitchingBlank:
        handled = ETrue;
        break;
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.ProviderReady %d", handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderSwitchDone
// -----------------------------------------------------------------------------
//
TBool CVtEngLocalVideo::TVtEngLayoutChangeHandler::ProviderSwitchDone(
    CVSDataProvider* aOldProvider )
    {
    TBool handled( EFalse );
    __VTPRINTENTER( "LocVid.LayoutCH.ProviderSwitchDone" )
    switch ( iState )
        {
    case EIdle:
        // nothing to do
        break;

    case ESwitchingBlank:
        {
        __VTPRINT( DEBUG_MEDIA,
            "LocVid.LayoutCH.ProviderSwitchDone ESwitchingBlank" )
        handled = ETrue;
        iState = EInitializingCamera;
        TProviderItem* providerItem;
        const TInt result(
            iLocalVideo->GetProviderByInstance( aOldProvider, providerItem ) );
        if ( result != KErrNotFound )
            {
            iLocalVideo->DeleteProvider( *providerItem );
            iLocalVideo->iCameraPref->SetProvider( NULL,
                    *iLocalVideo->iActiveProvider );

            __VTPRINT( DEBUG_MEDIA,
                "LocVid.LayoutCH.ProviderSwitchDone initializing provider" )

            TRAPD( error, iLocalVideo->InitializeProviderL( *providerItem ) );

            if ( error )
                {
                __VTPRINT2( DEBUG_MEDIA,
                    "LocVid.LayoutCH.ProviderSwitchDone ERROR %d in initialize",
                    error )
                Complete( error );
                }
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA,
                "LocVid.LayoutCH.ProviderSwitchDone provider NOT found" )
            Complete( result );
            }
        }
        break;

    case EInitializingCamera:
        handled = ETrue;
        break;

    case ESwitchingCamera:
        __VTPRINT( DEBUG_MEDIA,
            "LocVid.LayoutCH.ProviderSwitchDone ESwitchingCamera" )
        TRAP_IGNORE( iLocalVideo->StartViewFinderIfWasStartedL() );
        //Re-Set provider to camera preference. Otherwise, after layout change, the provider in camera preference will be invalid
        iLocalVideo->iCameraPref->SetProvider( iLocalVideo->iActiveProvider->iProvider,
                    *iLocalVideo->iActiveProvider );
        handled = ETrue;
        __VTPRINT( DEBUG_MEDIA,
            "LocVid.LayoutCH.ProviderSwitchDone reinit done" )
        Complete( KErrNone );
        break;
        }
    __VTPRINTEXITR( "LocVid.LayoutCH.ProviderSwitchDone %d", handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::NewL
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngVFBitmapNGARender * 
CVtEngLocalVideo::CVtEngVFBitmapNGARender::NewL()
    {
    CVtEngVFBitmapNGARender* self 
        = new ( ELeave ) CVtEngVFBitmapNGARender();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::~CVtEngVFBitmapNGARender
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngVFBitmapNGARender::~CVtEngVFBitmapNGARender()
    {
    __VTPRINTENTER( "LocVid.NGARender.~" )
    Cancel();
    
    if ( iSurfaceCreated )
        {
        iWindow->RemoveBackgroundSurface(ETrue);
        iWs->UnregisterSurface(0, iSurfaceId);
        
        iSurfaceManager.CloseSurface(iSurfaceId);
        iSurfaceChunk->Close();
        delete iSurfaceChunk;
        iSurfaceChunk = NULL;
        
        iSurfaceManager.Close();
            
        iSurfaceUpdateSession.CancelAllUpdateNotifications();
        iSurfaceUpdateSession.Close();    
        }
    
    __VTPRINTEXIT( "LocVid.NGARender.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::UpdateBitmap
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CVtEngVFBitmapNGARender::UpdateBitmapL( CFbsBitmap& aFrame )
    {
    __VTPRINTENTER( "LocVid.NGARender.UpdateBitmapL" )
    // do nothing if we're already active, the newer bitmap will be ignored.
    if ( IsActive() )
        {
        __VTPRINTEXIT( "LocVid.NGARender.UpdateBitmapL 0" )
        return KErrNone;
        }
    else
        {
        if ( !iSurfaceCreated )
            {
            User::Leave( KErrNotReady );
            }
        aFrame.BeginDataAccess();
        __VTPRINT2( DEBUG_MEDIA, "LocVid.NGARender.bitmap size = %d", aFrame.DataSize() );
#if 0
        memcpy( iBuffer, aFrame.DataAddress(), aFrame.DataSize() );
#else
        TInt size = aFrame.DataSize();
        TPtr8 ptrBuffer( iBuffer, size );
        ptrBuffer.Copy( reinterpret_cast<TUint8*>(aFrame.DataAddress()), size );
#endif
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.bitmap step 1" );
        aFrame.EndDataAccess();
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.bitmap step 2" );
        iStatus = KRequestPending;
        SetActive();
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.bitmap step 3" );
        iSurfaceUpdateSession.NotifyWhenDisplayed( iStatus, iTimeStamp );
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.bitmap step 4" );
        iSurfaceUpdateSession.SubmitUpdate( KAllScreens, iSurfaceId, 0, NULL );
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.bitmap step 5" );

        __VTPRINTEXIT( "LocVid.NGARender.UpdateBitmapL 1" )

        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::CreateSurfaceL
// -----------------------------------------------------------------------------
//
TInt CVtEngLocalVideo::CVtEngVFBitmapNGARender::AttachSurfaceL( 
        RWsSession *aWs, 
        RWindow *aWindow )
    {
    __VTPRINTENTER( "LocVid.NGARender.AttachSurfaceL" )
    
    if ( aWs == NULL || aWindow == NULL )
        {
        User::Leave( KErrArgument );
        }
        
        if(iSurfaceCreated)
        	return KErrNone;
    
    if ( !iSurfaceCreated )
        {    
        TInt err;
            
        err = iSurfaceUpdateSession.Connect();    
        User::LeaveIfError(err);
            
        err = iSurfaceManager.Open();
        User::LeaveIfError(err);
        
        RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
        attributes().iPixelFormat           = iSurfaceFormat; 
        attributes().iSize.SetSize( KVtEngLocalVideoDefaultWidth, 
                                    KVtEngLocalVideoDefaultHeight );
        attributes().iBuffers               = EVtEngVFMaxBuffers;
        attributes().iStride                = attributes().iSize.iWidth*4;
        attributes().iOffsetToFirstBuffer   = 0;
        attributes().iAlignment             = 4;
        attributes().iContiguous            = EFalse;
        attributes().iMappable              = ETrue;
    
        err = iSurfaceManager.CreateSurface( attributes, iSurfaceId );
        User::LeaveIfError( err );  
        
        // Map to chunk
        iSurfaceChunk = new RChunk();
        User::LeaveIfNull( iSurfaceChunk );    
        err = iSurfaceManager.MapSurface( iSurfaceId, *iSurfaceChunk );
        User::LeaveIfError( err );    
    
        // Get the info from the surfaceManager
        RSurfaceManager::TInfoBuf info;
        err = iSurfaceManager.SurfaceInfo( iSurfaceId, info );
        User::LeaveIfError( err );    
    
        TInt offset;
        iSurfaceManager.GetBufferOffset( iSurfaceId, 0, offset );
        iBuffer = iSurfaceChunk->Base() + offset;
        
        iSurfaceCreated = ETrue;
        }
    else
        {
        if ( iWs == NULL || iWindow == NULL )
            {
            User::Leave( KErrGeneral );
            }
        
        iWindow->RemoveBackgroundSurface(ETrue);
        iWs->UnregisterSurface(0, iSurfaceId);
        
        __VTPRINT( DEBUG_MEDIA, "LocVid.NGARender.Surface exists, detach first!" )
        }

    iWs = aWs;
    iWindow = aWindow;
    iWs->RegisterSurface( 0, iSurfaceId );
    
    TSurfaceConfiguration surfaceConfig;
    surfaceConfig.SetSurfaceId( iSurfaceId );
    surfaceConfig.SetOrientation( CFbsBitGc::EGraphicsOrientationRotated270);
    iWindow->SetBackgroundSurface( surfaceConfig, ETrue );
    //iWindow->SetBackgroundSurface( iSurfaceId );
    
    __VTPRINTEXIT( "LocVid.NGARender.AttachSurfaceL" )
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::~CVtEngVFBitmapNGARender
// -----------------------------------------------------------------------------
//
CVtEngLocalVideo::CVtEngVFBitmapNGARender::CVtEngVFBitmapNGARender()
    : CActive( CActive::EPriorityStandard ),
      iSurfaceFormat( EUidPixelFormatXRGB_8888 )
    {
    iSurfaceCreated = EFalse;

    CActiveScheduler::Add( this );        
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::ConstructL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngVFBitmapNGARender::ConstructL( )
    {
    __VTPRINTENTER( "LocVid.NGARender.ConstructL" )  
    __VTPRINTEXIT( "LocVid.NGARender.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::RunL
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngVFBitmapNGARender::RunL()
    {
    __VTPRINTENTER( "LocVid.NGARender.RunL" )  
    __VTPRINTEXIT( "LocVid.NGARender.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CVtEngVFBitmapNGARender::DoCancel
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CVtEngVFBitmapNGARender::DoCancel()
    {
    __VTPRINTENTER( "LocVid.NGARender.DoCancel" )  
    __VTPRINTEXIT( "LocVid.NGARender.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CreateCameraPreferencesL
// -----------------------------------------------------------------------------
//
CVtEngCameraPreferences* CVtEngLocalVideo::CreateCameraPreferencesL()
    {
	__VTPRINTENTER( "LocVid.CreateCameraPreferencesL" )
    iCameraPref = CVtEngCameraPreferences::NewL();

	// Check if provider exist and set it
	// as provider to camera preferences
    if ( iActiveProvider )
    	{
    	__VTPRINT( DEBUG_MEDIA,
    	"LocVideo.CreateCameraPreferencesL SetProvider" )
    	iCameraPref->SetProvider( iActiveProvider->iProvider,
    	    *iActiveProvider );
    	}

    __VTPRINTEXIT( "LocVid.CreateCameraPreferencesL" )
	return iCameraPref;
    }

// -----------------------------------------------------------------------------
// CVtEngLocalVideo::CancelInitializeShareImage
// -----------------------------------------------------------------------------
//
void CVtEngLocalVideo::CancelInitializeShareImage()
    {
    __VTPRINTENTER( "CVtEngLocalVideo.CancelInitializeShareImage");
    iShareImage->CancelInitialize();
    __VTPRINTEXIT( "CVtEngLocalVideo.CancelInitializeShareImage");
    }

// End of File
