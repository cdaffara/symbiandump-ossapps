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

#include <QGraphicsWidget>
#include <graphics/surfaceconfiguration.h>
#include <mediaclientvideodisplay.h>
#include "svpbnativewindow.h"
#include "trace.h"

CSvpbNativeWindow* CSvpbNativeWindow::NewL(const TRect& displayRect)
{
    FUNC_LOG;

    CSvpbNativeWindow* self = new (ELeave) CSvpbNativeWindow;
    CleanupStack::PushL(self);
    self->ConstructL(displayRect);
    CleanupStack::Pop();
    return self;
}

CSvpbNativeWindow::~CSvpbNativeWindow()
{
    FUNC_LOG;

    detachSurface();
    CloseWindow();
}

int CSvpbNativeWindow::attachSurface(
        const TSurfaceId &surfaceId,
        const TRect &cropRect,
        const TVideoAspectRatio &aspectRatio)
{
    FUNC_LOG;

    INFO("Surface id:" << surfaceId.iInternal[0]
        << surfaceId.iInternal[1]
        << surfaceId.iInternal[2]
        << surfaceId.iInternal[3]);

    if (mSurfaceId.IsNull()) {
        mSurfaceId = surfaceId;
        return setupSurface(cropRect, aspectRatio);
    }
    else if (mSurfaceId == surfaceId) {
        detachSurface();
        return setupSurface(cropRect, aspectRatio);
    }
    else {
        detachSurface();
    }

    return 0;
}

void CSvpbNativeWindow::detachSurface()
{
    FUNC_LOG;

    if (mDisplay) {
        mDisplay->RemoveSurface();
        mDisplay->RemoveDisplayWindow( *(DrawableWindow()));
        delete mDisplay;
        mDisplay = 0;
    }

    mSurfaceId = TSurfaceId::CreateNullId();
}

CSvpbNativeWindow::CSvpbNativeWindow() :
    mSurfaceId(TSurfaceId::CreateNullId()),
    mDisplay(0)
{
    FUNC_LOG;
}

void CSvpbNativeWindow::ConstructL(const TRect& displayRect)
{
    FUNC_LOG;

    CreateWindowL();
    SetRect(displayRect);
    DrawableWindow()->SetOrdinalPosition(-1); // Send to background
    ActivateL();
}

int CSvpbNativeWindow::setupSurface(
    const TRect &cropRect, const TVideoAspectRatio &aspectRatio)
{
    FUNC_LOG;

    TInt displayId = CCoeEnv::Static()->ScreenDevice()->GetScreenNumber();
    INFO("Display id:" << displayId);
    QT_TRAP_THROWING(mDisplay = CMediaClientVideoDisplay::NewL(displayId));

    RWindowBase *videoWindow = DrawableWindow();
    TRect displayRect = Rect();
    TRAPD(err, mDisplay->AddDisplayWindowL(videoWindow,
        displayRect, // Window clipping rectangle, relative to the window
        cropRect,    // The dimensions of the crop region, relative to the video image
        displayRect, // Video extent on the screen
        100.0f,
        100.0f,
        displayRect.Width() < displayRect.Height() ? EVideoRotationClockwise90 : EVideoRotationNone,
        EAutoScaleStretch,
        EHorizontalAlignCenter,
        EVerticalAlignCenter,
        (RWindow*)videoWindow));
    HANDLE_ERROR_NEG(err);

    if (err == 0) {
        QT_TRAP_THROWING(mDisplay->SurfaceCreated(mSurfaceId, cropRect, aspectRatio, cropRect));
    }

    return err;
}
