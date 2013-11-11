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

#include "CVtImageScalerImpl.h"
#include "cvtimage.h"
#include "CVtImageBitmap.h"

// MACROS

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif


// ============================ MEMBER FUNCTIONS ===============================

// ========================== CVtImageScalerImpl ==============================

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::CVtImageScalerImpl()
// -----------------------------------------------------------------------------
CVtImageScalerImpl::CVtImageScalerImpl() : CActive( EPriorityStandard )
    {
    _IFDBG(PRINT( _L("ScaleImpl.Ctor<") ));
    CActiveScheduler::Add( this );
    _IFDBG(PRINT( _L("ScaleImpl.Ctor>") ));
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::~CVtImageScalerImpl()
// -----------------------------------------------------------------------------
CVtImageScalerImpl::~CVtImageScalerImpl()
    {
    _IFDBG(PRINT( _L("ScaleImpl.~<") ));
    Cancel();
    delete iScaler;
    _IFDBG(PRINT( _L("ScaleImpl.~>") ));
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::SetSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageScalerImpl::SetSourceTargetL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    iSource = iTarget = 0;
    LeaveIfNotValidSizeL( aSource.Size(), TSize( 2, 2 ) );
    LeaveIfNotValidSizeL( aTarget.Size() );
    ValidateSourceTargetL( aSource, aTarget );
    iSource = &aSource;
    iTarget = &aTarget;
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::NeedHeapLock() const
// -----------------------------------------------------------------------------
TBool CVtImageScalerImpl::NeedHeapLock() const
    {
    return iSource->NeedHeapLock() || iTarget->NeedHeapLock();
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::ScaleWithBitmapScalerL()
// -----------------------------------------------------------------------------
void CVtImageScalerImpl::ScaleWithBitmapScalerL(
    const CBitmapScaler::TQualityAlgorithm
#ifdef _DEBUG
    aQuality
#endif // _DEBUG
     )
    {
    _IFDBG(PRINT( _L("ScaleImpl.ScaleWithBitmapScalerL<") ));
    if ( IsActive() || iActiveSchedulerWait.IsStarted() )
        {
        _IFDBG(PRINT( _L("ScaleImpl.ScaleWithBitmapScalerL LEAVE KErrInUse>") ));
        User::Leave( KErrInUse );
        }
    _IFDBG(PRINT( _L("ScaleImpl::ScaleWithBitmapScalerL aQuality = %d"), aQuality ));
    CFbsBitmap& source( static_cast< CVtImageBitmap* >(
        const_cast< CVtImage* >( iSource ) )->Bitmap() );
    CFbsBitmap& target( static_cast< CVtImageBitmap* >( iTarget )->Bitmap() );
    iScaler = CBitmapScaler::NewL();
    SetActive();
    iScaler->Scale( &iStatus, source, target, EFalse );
    iActiveSchedulerWait.Start();
    delete iScaler; iScaler = NULL;
    _IFDBG(PRINT( _L("ScaleImpl.ScaleWithBitmapScalerL iStatus.Int() = %d"), iStatus.Int() ));
    User::LeaveIfError( iStatus.Int() );
    _IFDBG(PRINT( _L("ScaleImpl.ScaleWithBitmapScalerL>") ));
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::LeaveIfNotValidSizeL(
//  const TSize& aSize, const TSize aMinSize )
// -----------------------------------------------------------------------------
void CVtImageScalerImpl::LeaveIfNotValidSizeL(
    const TSize& aSize,
    const TSize aMinSize )
    {
    _IFDBG(PRINT( _L("ScaleImpl.LeaveIfNotValidSizeL<") ));
    if( ( aSize.iWidth < aMinSize.iWidth ) ||
        ( aSize.iHeight < aMinSize.iHeight ) )
        {
        User::Leave( KErrNotSupported );
        }
    _IFDBG(PRINT( _L("ScaleImpl.LeaveIfNotValidSizeL>") ));
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::RunL()
// -----------------------------------------------------------------------------
void CVtImageScalerImpl::RunL()
    {
    _IFDBG(PRINT( _L("CVtImageScalerImpl.RunL") ));
    iActiveSchedulerWait.AsyncStop();
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImpl::RunL()
// -----------------------------------------------------------------------------
void CVtImageScalerImpl::DoCancel()
    {
    _IFDBG(PRINT( _L("CVtImageScalerImpl.DoCancel") ));
    iScaler->Cancel();
    iActiveSchedulerWait.AsyncStop();
    }

// End of File


