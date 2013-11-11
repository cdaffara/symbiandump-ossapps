/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image Transforms subsystem.
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <fbs.h>

#include "CVtImageScaler.h"
#include "CVtImageScalerImpl.h"
#include "CVtImageScalerImplNearest.h"
#include "CVtImageScalerImplWeightedAverage.h"
#include "CVtImageScalerImplBilinear.h"

// CONSTANTS

_LIT( KModuleName, "CVtImageScaler" );

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtImageScaler::NewL( MVtImageScalerObserver& aObserver, TQuality aQuality,
//   TPriority aPriority )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageScaler* CVtImageScaler::NewL(
    MVtImageScalerObserver& aObserver,
    TQuality aQuality,
    TPriority aPriority )
    {
    CVtImageScaler* self = new ( ELeave ) CVtImageScaler( aObserver, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aQuality );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::~CVtImageScaler()
// -----------------------------------------------------------------------------
EXPORT_C CVtImageScaler::~CVtImageScaler()
    {
	Cancel();
    delete iAsyncCallBack;
    delete iHeapLock;
    delete iScalerImpl;
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::SetQualityL( TQuality aQuality )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageScaler::SetQualityL( TQuality aQuality )
    {
    delete iScalerImpl;
    iScalerImpl = 0;

    switch( aQuality )
        {
        case ENearest:
            iScalerImpl = new ( ELeave ) CVtImageScalerImplNearest();
            break;

        case EWeightedAverage:
            iScalerImpl = new ( ELeave ) CVtImageScalerImplWeightedAverage();
            break;

        case EBilinear:
            iScalerImpl = new ( ELeave ) CVtImageScalerImplBilinear();
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::ScaleL( const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageScaler::ScaleL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    if( !IsActive() )
        {
        if( !iScalerImpl )
            {
            User::Leave( KErrNotReady );
            }
        iScalerImpl->SetSourceTargetL( aSource, aTarget );
        iCancelled = EFalse;
        Activate();
        iAsyncCallBack->CallBack();
        }
    else
        {
        User::Panic( KModuleName(), EBusy );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::Scale( const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageScaler::Scale(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    TRAPD( result, ScaleL( aSource, aTarget ) );
    if( result != KErrNone )
        {
        Activate();
        Signal( result );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::ScaleNowL( const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageScaler::ScaleNowL(
    const CVtImage& aSource, CVtImage& aTarget )
    {
    if( !iScalerImpl )
        {
        User::Leave( KErrNotReady );
        }

    iScalerImpl->SetSourceTargetL( aSource, aTarget );

    TInt result;
    TBool more;
    TBool needsLock( iScalerImpl->NeedHeapLock() );
    do
        {

        if ( needsLock )
            {
            iHeapLock->LockHeap( ETrue );
            }

        result = iScalerImpl->Scale( more );

        if ( needsLock )
            {
            iHeapLock->UnlockHeap( ETrue );
            }

        } while ( more && ( result == KErrNone ) );

    User::LeaveIfError( result );
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::CVtImageScaler()
// -----------------------------------------------------------------------------
CVtImageScaler::CVtImageScaler( MVtImageScalerObserver& aObserver, TPriority aPriority )
: CActive( aPriority ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::ConstructL( TQuality aQuality )
// -----------------------------------------------------------------------------
void CVtImageScaler::ConstructL( TQuality aQuality )
    {
    SetQualityL( aQuality );
    iHeapLock = new ( ELeave ) CFbsBitmap();
    iHeapLock->Create( TSize( 1, 1 ), EColor256 );
    iAsyncCallBack = new ( ELeave )
        CAsyncCallBack( TCallBack( StaticScale, this ), Priority() );
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::Activate
// -----------------------------------------------------------------------------
void CVtImageScaler::Activate()
    {
    iStatus = KRequestPending;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::Signal( TInt aError )
// -----------------------------------------------------------------------------
void CVtImageScaler::Signal( TInt aError )
    {
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aError );
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::RunL()
// -----------------------------------------------------------------------------
void CVtImageScaler::RunL()
    {
    iObserver.ScalingFinished( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::DoCancel()
// -----------------------------------------------------------------------------
void CVtImageScaler::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        iCancelled = ETrue;
        Signal( KErrCancel );
        iObserver.ScalingFinished( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::StaticScale()
// -----------------------------------------------------------------------------
TInt CVtImageScaler::StaticScale( TAny* aPtr )
    {
    CVtImageScaler* self = reinterpret_cast< CVtImageScaler* >( aPtr );
    self->DoScale();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtImageScaler::DoScale
// -----------------------------------------------------------------------------
void CVtImageScaler::DoScale()
    {
    if ( !iCancelled )
        {
        TBool more;
        TBool needsLock = iScalerImpl->NeedHeapLock();
        if ( needsLock )
            {
            iHeapLock->LockHeap( ETrue );
            }
        TInt result = iScalerImpl->Scale( more );
        if ( needsLock )
            {
            iHeapLock->UnlockHeap( ETrue );
            }
        if ( !more || ( result != KErrNone ) )
            {
            Signal( result );
            }
        else
            {
            iAsyncCallBack->CallBack();
            }
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File


