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

#include "cvtimagerotator.h"
#include "CVtImageRotatorImplMirrorFlip.h"
#include "CVtImageRotatorImplClockwise.h"

// CONSTANTS

_LIT( KModuleName, "CVtImageRotator" );

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtImageRotator::NewL( MVtImageRotatorObserver& aObserver,
//  TPriority aPriority )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageRotator* CVtImageRotator::NewL(
    MVtImageRotatorObserver& aObserver,
    TPriority aPriority)
    {
    CVtImageRotator* self = new ( ELeave ) CVtImageRotator( aObserver, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::~CVtImageRotator()
// -----------------------------------------------------------------------------
EXPORT_C CVtImageRotator::~CVtImageRotator()
    {
	Cancel();
    delete iAsyncCallBack;
    delete iHeapLock;
	delete iRotatorImpl;
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::ScaleL( const CVtImage& aSource,
//  CVtImage& aTarget, const TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageRotator::RotateL( const CVtImage& aSource,
    CVtImage& aTarget, const TRotationAngle& aAngle )
    {
    if ( !IsActive() )
        {
        CreateRotatorL( aAngle );
        iRotatorImpl->SetSourceTargetL( aSource, aTarget );
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
// CVtImageRotator::Rotate
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageRotator::Rotate(
    const CVtImage& aSource,
    CVtImage& aTarget, const TRotationAngle& aAngle )
    {
    TRAPD( result, RotateL( aSource, aTarget, aAngle ) );
    if ( result != KErrNone )
        {
        Activate();
        Signal( result );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::CVtImageRotator( MVtImageRotatorObserver& aObserver,
//  TPriority aPriority )
// -----------------------------------------------------------------------------
CVtImageRotator::CVtImageRotator( MVtImageRotatorObserver& aObserver,
                                  TPriority aPriority )
: CActive( aPriority ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::ConstructL()
// -----------------------------------------------------------------------------
void CVtImageRotator::ConstructL()
    {
    iHeapLock = new ( ELeave ) CFbsBitmap();
    iHeapLock->Create( TSize( 1, 1 ), EColor256 );
    iAsyncCallBack = new ( ELeave )
        CAsyncCallBack( TCallBack( StaticRotate, this ), Priority() );
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::CreateRotatorL( const TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
void CVtImageRotator::CreateRotatorL( const TRotationAngle& aAngle )
    {
    if( iRotatorImpl )
        {
        if( iRotatorImpl->SupportsRotationAngle( aAngle ) )
            {
            iRotatorImpl->SetAngle( aAngle );
            return;
            }
        delete iRotatorImpl;
        iRotatorImpl = 0;
        }

    switch( aAngle )
        {
        case E90DegreesClockwise:
        case E270DegreesClockwise:
            iRotatorImpl =
                new ( ELeave ) CVtImageRotatorImplClockwise( aAngle );
            break;

        case EMirrorHorizontalAxis:
        case EFlipVerticalAxis:
        case E180DegreesClockwise:
            iRotatorImpl =
                new ( ELeave ) CVtImageRotatorImplMirrorFlip( aAngle );
            break;

        default:
            User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::Activate
// -----------------------------------------------------------------------------
void CVtImageRotator::Activate()
    {
    iStatus = KRequestPending;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::Signal( TInt aError )
// -----------------------------------------------------------------------------
void CVtImageRotator::Signal( TInt aError )
    {
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aError );
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::RunL()
// -----------------------------------------------------------------------------
void CVtImageRotator::RunL()
    {
    iObserver.RotationFinished( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::DoCancel()
// -----------------------------------------------------------------------------
void CVtImageRotator::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        iCancelled = ETrue;
        Signal( KErrCancel );
        iObserver.RotationFinished( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::StaticRotate()
// -----------------------------------------------------------------------------
TInt CVtImageRotator::StaticRotate( TAny* aPtr )
    {
    CVtImageRotator* self = reinterpret_cast< CVtImageRotator* >( aPtr );
    self->DoRotate();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtImageRotator::DoRotate
// -----------------------------------------------------------------------------
void CVtImageRotator::DoRotate()
    {
    if ( !iCancelled )
        {
        TBool more;
        TBool needsLock = iRotatorImpl->NeedHeapLock();
        if( needsLock )
            {
            iHeapLock->LockHeap( ETrue );
            }
        TInt result = iRotatorImpl->Rotate( more );
        if( needsLock )
            {
            iHeapLock->UnlockHeap( ETrue );
            }
        if( !more || ( result != KErrNone ) )
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


