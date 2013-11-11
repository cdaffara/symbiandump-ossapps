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

#include "CVtImageRotatorImpl.h"
#include "cvtimage.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ========================== CVtImageRotatorImpl ==============================

// -----------------------------------------------------------------------------
// CVtImageRotatorImpl::SetSourceTargetL( 
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImpl::SetSourceTargetL( 
    const CVtImage& aSource, 
    CVtImage& aTarget )
    {
    iSource = iTarget = 0;
    ValidateSourceTargetL( aSource, aTarget );
    iSource = &aSource;
    iTarget = &aTarget;
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImpl::NeedHeapLock() const
// -----------------------------------------------------------------------------
TBool CVtImageRotatorImpl::NeedHeapLock() const
    {
    return iSource->NeedHeapLock() || iTarget->NeedHeapLock();
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImpl::SetAngle( 
//  const CVtImageRotator::TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
void CVtImageRotatorImpl::SetAngle( const CVtImageRotator::TRotationAngle& aAngle )
    {
    iAngle = aAngle;
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImpl::CVtImageRotatorImpl( 
//  const CVtImageRotator::TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
CVtImageRotatorImpl::CVtImageRotatorImpl( const CVtImageRotator::TRotationAngle& aAngle )
: iAngle( aAngle )
    {
    }

// End of File  


