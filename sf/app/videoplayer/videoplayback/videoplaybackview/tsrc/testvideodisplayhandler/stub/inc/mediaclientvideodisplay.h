// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:  Stub declaration for CMediaClientVideoDisplay
//


#ifndef MEDIACLIENTVIDEODISPLAY_H
#define MEDIACLIENTVIDEODISPLAY_H

#include <w32std.h>
#include <mmf/common/mmfvideosurfacecustomcommands.h>

class CMediaClientVideoDisplay : public CBase
{
    public:

        static CMediaClientVideoDisplay* NewL(TInt aDisplayId);

        ~CMediaClientVideoDisplay();

        CMediaClientVideoDisplay();

        void AddDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion,
                                           const TRect& aVideoExtent, TReal32 aScaleWidth, TReal32 aScaleHeight,
                                           TVideoRotation aRotation, TAutoScaleType aAutoScaleType,
                                           TInt aHorizPos, TInt aVertPos, RWindow* aWindow2);

        void RemoveSurface();

        TInt SurfaceCreated(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, const TRect& aCropRegion);

        TInt SurfaceParametersChanged(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);

        TInt RedrawWindows(const TRect& aCropRegion);

        void SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent, const TRect& aCropRegion);

        void SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion);

    public:
        TRect iVideoExtent;
};

#endif // MEDIACLIENTVIDEODISPLAY_H

