/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCoeControl to provide RWindow for videoplayback.
*              QGraphicsView derived class can not be used for this purpose,
*              because the Qt OpenVG context may interfere with the one being
*              used by the renderer.
*
*/

#ifndef SVPBNATIVEWINDOW_H
#define SVPBNATIVEWINDOW_H

#include <coecntrl.h>
#include "svpbsurfacecontainer.h"

class CMediaClientVideoDisplay;

class CSvpbNativeWindow : public CCoeControl, public SvpbSurfaceContainer
{
public:
    static CSvpbNativeWindow* NewL(const TRect& displayRect);
    virtual ~CSvpbNativeWindow();

public: // From SvpbSurfaceContainer
    int attachSurface(const TSurfaceId &surfaceId, const TRect &cropRect, const TVideoAspectRatio &aspectRatio);
    void detachSurface();

private:
    CSvpbNativeWindow();
    void ConstructL(const TRect& displayRect);
    int setupSurface(const TRect &cropRect, const TVideoAspectRatio &aspectRatio);

private:
    TSurfaceId mSurfaceId;
    CMediaClientVideoDisplay *mDisplay;
};

#endif  // SVPBNATIVEWINDOW_H
