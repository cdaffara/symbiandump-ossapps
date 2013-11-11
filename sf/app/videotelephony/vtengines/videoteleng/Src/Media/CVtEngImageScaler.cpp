/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image scaler
*
*/



// INCLUDE FILES
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "CVtEngImageScaler.h"
#include    <cvtimagebitmap.h>
#include    <fbs.h>
#include    <videotelephonyvariant.hrh>
#include    <cvtlogger.h>

// CONSTANTS
// The number of used bitmaps as scaling targets.
const TInt KVtEngTargetBitmapCount = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngImageScaler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MVtEngImageScaler* CVtEngImageScaler::NewL(
    MVtEngScalerObserver& aObserver )
    {
    __VTPRINTENTER( "Scaler.NewL" )
    CVtEngImageScaler* scaler = new ( ELeave ) CVtEngImageScaler( aObserver );
    CleanupStack::PushL( scaler );
    scaler->ConstructL();
    CleanupStack::Pop( scaler );
    __VTPRINTEXIT( "Scaler.NewL" )
    return scaler;
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngImageScaler::ConstructL()
    {
    iVtScaler = CVtImageScaler::NewL( *this );
    GetScalingMethodL();
    iVtScaler->SetQualityL( iScalingMethod );
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::ScaleL
// Scaling method.
// -----------------------------------------------------------------------------
//
void CVtEngImageScaler::ScaleL(
            TInt aSourceIndex )
    {
    __VTPRINTENTER( "Scaler.ScaleL" )
    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ScaleL index=%d",
        aSourceIndex )
    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ScaleL target index=%d",
    iTargetBitmapInUse )
    if ( iVtScaler->IsActive() )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ScaleL NActive" )
        User::Leave( KErrInUse );
        }

    // If there is no bitmaps done, do leave.
    // The caller should call ConfigureL.
    if ( iSources.Count() <= 0  || iTargets.Count() <= 0 )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ScaleL: not conf" )
        User::Leave( KErrNotReady );
        }
    iVtScaler->ScaleL( *iSources[ aSourceIndex ] ,
         *iTargets[iTargetBitmapInUse] );
    __VTPRINTEXIT( "Scaler.ScaleL" )
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::Configure
// Configure scaling.
// -----------------------------------------------------------------------------
//
TBool CVtEngImageScaler::ConfigureL(
    TDisplayMode aDisplayMode,
    TSize aSize,
    TInt aSourceHandle1,
    TInt aSourceHandle2 )
    {
    __VTPRINTENTER( "Scaler.ConfigureL" )
    __VTPRINT( DEBUG_MEDIA , "================== Scaler.Config ==================" )
    __VTPRINT2( DEBUG_MEDIA, "    aSize.iWidth:%d ", aSize.iWidth)
    __VTPRINT2( DEBUG_MEDIA, "    aSize.iHeight:%d ", aSize.iHeight )
    __VTPRINT2( DEBUG_MEDIA, "    aDisplayMode:%d", aDisplayMode )
    __VTPRINT( DEBUG_MEDIA , "================== Scaler.Config ==================" )

    TInt sourceHandle1( 0 );
    if ( iSources.Count() > 0 )
        {
        CVtImageBitmap* bmp = iSources[ EFirstBitmap ];
        if ( bmp )
            {
            sourceHandle1 = bmp->Bitmap().Handle();
            }
        }
    TInt sourceHandle2( 0 );
    if ( iSources.Count() > 1 )
        {
        CVtImageBitmap* bmp = iSources[ ESecondBitmap ];
        if ( bmp )
            {
            sourceHandle2 = bmp->Bitmap().Handle();
            }
        }

    // Do we have to cancel scaling first?
    TBool cancelled( iVtScaler->IsActive() );

    if ( aSourceHandle1 != sourceHandle1 || aSourceHandle2 != sourceHandle2
        || aSize != iSize || iMode != aDisplayMode )
        {
        if ( cancelled )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:Config cancel" )
            Cancel();
            }
        }
    else
        {
        // Do we need the update => NO
        __VTPRINTEXITR( "Scaler.ConfigureL %d", EFalse )
        return EFalse;
        }

    // Store new mode and size.
    iMode = aDisplayMode;
    iSize = aSize;

    // The RVD source images have changed.
    if ( aSourceHandle1 != sourceHandle1 || aSourceHandle2 != sourceHandle2 )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
            "Scaler:ConfigureL create bitmaps" )
        CVtImageBitmap* vtBitmap = CVtImageBitmap::NewL( aSourceHandle1 );
        CleanupStack::PushL( vtBitmap );
        CVtImageBitmap* vtBitmap2 = CVtImageBitmap::NewL( aSourceHandle2 );
        CleanupStack::PushL( vtBitmap2 );

	    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ConfigureL source reset" )
        iSources.ResetAndDestroy();
        iSources.AppendL( vtBitmap );
        iSources.AppendL( vtBitmap2 );
	    CleanupStack::Pop( KVtEngTargetBitmapCount ); // vtBitMaps
        }

    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ConfigureL create targets" )
    CVtImageBitmap* vtBitmapT =  CVtImageBitmap::NewL( aSize, aDisplayMode );
    CleanupStack::PushL( vtBitmapT );
    CVtImageBitmap* vtBitmapT2 = CVtImageBitmap::NewL( aSize, aDisplayMode );
    CleanupStack::PushL( vtBitmapT2 );

    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ConfigureL target reset" )
    iTargets.ResetAndDestroy();
    iTargets.AppendL( vtBitmapT );
    iTargets.AppendL( vtBitmapT2 );
    CleanupStack::Pop( KVtEngTargetBitmapCount ); // vtBitMapTs

    __VTPRINTEXITR( "Scaler.ConfigureL %d", cancelled )
    return cancelled;
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::ScalingFinished
// Callback telling that scaling is done.
// -----------------------------------------------------------------------------
//
void CVtEngImageScaler::ScalingFinished( TInt aError )
    {
    __VTPRINTENTER( "Scaler.ScalingFinished" )

    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:ScaleFin ", aError )

    if ( aError == KErrNone )
        {
        CFbsBitmap& bitmap = iTargets [ iTargetBitmapInUse ]->Bitmap();
        __VTPRINT2( DEBUG_MEDIA, "Scaler.ScalingFin index", iTargetBitmapInUse )

        if ( iTargetBitmapInUse == EFirstBitmap )
            {
            iTargetBitmapInUse = ESecondBitmap;
            }
        else
            {
            iTargetBitmapInUse = EFirstBitmap;
            }
        // Notify client of done scaling
        iObserver.ScalingCompleted( &bitmap, aError );
        }
    else
        {
        iObserver.ScalingCompleted( NULL, aError );
        }
    __VTPRINTEXIT( "Scaler.ScalingFinished" )
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::~CVtEngImageScaler
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngImageScaler::~CVtEngImageScaler()
    {
    __VTPRINTENTER( "Scaler.~" )

    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "Scaler:Dtor" )
    iTargets.ResetAndDestroy();
    iTargets.Close();
    iSources.ResetAndDestroy();
    iSources.Close();
    delete iVtScaler;
    __VTPRINTEXIT( "Scaler.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::CVtEngImageScaler
// C++ constructor.
// -----------------------------------------------------------------------------
//
CVtEngImageScaler::CVtEngImageScaler(
    MVtEngScalerObserver& aObserver ) :
    iObserver( aObserver ), iTargetBitmapInUse ( EFirstBitmap ),
    iScalingMethod( CVtImageScaler::EWeightedAverage )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::Release
// Interface function implementation to delete the implementation.
// -----------------------------------------------------------------------------
//
void CVtEngImageScaler::Release()
    {
    // Delete myself
  __VTPRINTENTER( "Scaler.Release" )
    delete this;
    __VTPRINTEXIT( "Scaler.Release" )
    }

// -----------------------------------------------------------------------------
// VtEngScalerFactory::CreateScalerL
// Creates the image scaler.
// -----------------------------------------------------------------------------
//
MVtEngImageScaler* VtEngScalerFactory::CreateScalerL(
    MVtEngScalerObserver& aObserver )
    {
    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
        "VtEngScalerFactory::CreateScalerL" )
    return CVtEngImageScaler::NewL( aObserver );
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::Cancel
// Cancels the conversion.
// -----------------------------------------------------------------------------
//
TInt CVtEngImageScaler::Cancel()
    {
    __VTPRINTENTER( "Scaler.Cancel" )
    iVtScaler->Cancel();
    __VTPRINTEXIT( "Scaler.Cancel" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngImageScaler::GetScalingMethodL
// Gets the used scaling method from local variation.
// -----------------------------------------------------------------------------
//
void CVtEngImageScaler::GetScalingMethodL()
    {
    __VTPRINTENTER( "Scaler.GetScalingMethodL" )
    // Check if the scaling method is non-default
    CVtEngSettings& settings = CVtEngUtility::Settings();
    if( settings.CheckBits(
        KVTLVFlagScalingMethodENearest ) )
        {
        iScalingMethod = CVtImageScaler::ENearest;
        }
    else if ( settings.CheckBits(
        KVTLVFlagScalingMethodEBilinear ) )
        {
        iScalingMethod = CVtImageScaler::EBilinear;
        }
    __VTPRINTEXITR( "Scaler.GetScalingMethodL %d", (TInt) iScalingMethod )
    }

//  End of File
