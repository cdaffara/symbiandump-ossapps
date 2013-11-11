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
* Description:  Main view of the SimpleVideoPlayback MPX view plugin.
*
*/

#include <hbmainwindow.h>
#include <hbtapgesture.h>
#include "svpbnativewindow.h"
#include "svpbview.h"
#include "trace.h"

SvpbView::SvpbView(QGraphicsItem *parent) :
    HbView(parent),
    mNativeWindow(0)
{
    FUNC_LOG;

    grabGesture(Qt::TapGesture);
}

SvpbView::~SvpbView()
{
    FUNC_LOG;

    delete mNativeWindow;
}

void SvpbView::activate()
{
    FUNC_LOG;
    HANDLE_ERROR_BOOL(mNativeWindow == 0);

    setContentFullScreen();
    setItemVisible(Hb::AllItems, false);
    mainWindow()->setOrientation(Qt::Horizontal, false);

    RWindowBase *rootWindow = mainWindow()->winId()->DrawableWindow();
    TRect displayRect(rootWindow->Position(), rootWindow->Size());

    QT_TRAP_THROWING(mNativeWindow = CSvpbNativeWindow::NewL(displayRect));

    mainWindow()->winId()->DrawableWindow()->SetSurfaceTransparency(ETrue); // This is required fow HW
}

void SvpbView::deactivate()
{
    FUNC_LOG;

//  If this is called, surface transparency can not be enabled again
//  mainWindow()->winId()->DrawableWindow()->SetSurfaceTransparency(EFalse);

    delete mNativeWindow;
    mNativeWindow = 0;
}

SvpbSurfaceContainer *SvpbView::surfaceContainer() const
{
    FUNC_LOG;

    return mNativeWindow;
}

// this needs to be implemented for gesture framework to work
void SvpbView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void SvpbView::gestureEvent(QGestureEvent *event)
{
    FUNC_LOG;

    bool eventHandled(false);
    HbTapGesture *tap = 0;
    if (QGesture *gesture = event->gesture(Qt::TapGesture)) {
        tap = static_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
        if (tap->tapStyleHint() == HbTapGesture::TapAndHold) {
            eventHandled = handleLongTap(tap->state());

        } else {
            eventHandled = handleTap(tap->state());
        }
    }

    eventHandled ? event->accept() : event->ignore();
}

bool SvpbView::handleTap(Qt::GestureState state)
{
    FUNC_LOG;

    if (state == Qt::GestureFinished) {
        emit tapped();
        return true;
    }

    return false;
}

bool SvpbView::handleLongTap(Qt::GestureState state)
{
    FUNC_LOG;

    if (state == Qt::GestureFinished) {
        emit longTapped();
        return true;
    }

    return false;
}
