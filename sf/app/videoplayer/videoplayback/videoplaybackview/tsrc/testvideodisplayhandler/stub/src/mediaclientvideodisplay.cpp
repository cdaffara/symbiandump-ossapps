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
* Description:   Stub implementation for CMediaClientVideoDisplay
*
*/

// Version : %version: 2 %

//  Include Files

#include "mediaclientvideodisplay.h"


// -------------------------------------------------------------------------------------------------
// CAlfCompositionSource::NewL()
// -------------------------------------------------------------------------------------------------
//
CMediaClientVideoDisplay* CMediaClientVideoDisplay::NewL( TInt /*aDisplayId*/ )
{
    CMediaClientVideoDisplay* self = new (ELeave) CMediaClientVideoDisplay();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::~CMediaClientVideoDisplay()
// -------------------------------------------------------------------------------------------------
//
CMediaClientVideoDisplay::~CMediaClientVideoDisplay()
{
}

// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::CMediaClientVideoDisplay()
// -------------------------------------------------------------------------------------------------
//
CMediaClientVideoDisplay::CMediaClientVideoDisplay()
{
}

// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
void CMediaClientVideoDisplay::RemoveSurface()
{
}

// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
void CMediaClientVideoDisplay::AddDisplayWindowL(const RWindowBase* /*aWindow*/,
	                                               const TRect& /*aClipRect*/,
	                                               const TRect& /*aCropRegion*/,
							                                   const TRect& /*aVideoExtent*/,
							                                   TReal32 /*aScaleWidth*/,
							                                   TReal32 /*aScaleHeight*/,
							                                   TVideoRotation /*aRotation*/,
							                                   TAutoScaleType /*aAutoScaleType*/,
							                                   TInt /*aHorizPos*/,
							                                   TInt /*aVertPos*/,
							                                   RWindow* /*aWindow2*/)
{
}

// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
TInt CMediaClientVideoDisplay::SurfaceCreated(const TSurfaceId& /*aSurfaceId*/,
	                                            const TRect& /*aCropRect*/,
	                                            TVideoAspectRatio /*aAspectRatio*/,
	                                            const TRect& /*aCropRegion*/)
{
    return KErrNone;
}


// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
TInt CMediaClientVideoDisplay::SurfaceParametersChanged(const TSurfaceId& /*aSurfaceId*/,
	                                                      const TRect& /*aCropRect*/,
	                                                      TVideoAspectRatio /*aAspectRatio*/)
{
    return KErrNone;
}


// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
TInt CMediaClientVideoDisplay::RedrawWindows(const TRect& /*aCropRegion*/)
{
    return KErrNone;
}


// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
void CMediaClientVideoDisplay::SetVideoExtentL(const RWindowBase& /*aWindow*/,
	                                             const TRect& aVideoExtent,
	                                             const TRect& /*aCropRegion*/)
{
    iVideoExtent = aVideoExtent;
}


// -------------------------------------------------------------------------------------------------
// CMediaClientVideoDisplay::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
void CMediaClientVideoDisplay::SetAutoScaleL(TAutoScaleType /*aScaleType*/,
	                                           TInt /*aHorizPos*/,
	                                           TInt /*aVertPos*/,
	                                           const TRect& /*aCropRegion*/)
{
}

// EOF
