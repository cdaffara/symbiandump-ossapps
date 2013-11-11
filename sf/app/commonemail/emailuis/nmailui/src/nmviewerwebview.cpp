/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: NMailUI web viewer. Inherits from
 *              QGrapohicsWebView and handles gestures, etc.
 */

#include "nmuiheaders.h"

/*!
    Constructor.
*/
NmMailViewerWK::NmMailViewerWK()
: QGraphicsWebView(),
  mContent(),
  mSuppressRelease(false)
{
    // Subscribe this widget to tap and pinch gestures.
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::PinchGesture);
    // Prevent this widget from accepting focus.
    setFocusPolicy(Qt::NoFocus); 
}

/*!
    Destructor.
 */
NmMailViewerWK::~NmMailViewerWK()
{
    mContent.clear();
}

/*!
    Adds content into web view.
 */
void NmMailViewerWK::addContent(QString key, QVariant val, NmId partId, bool isFetched) 
{
    mContent[key] = NmMailViewerWkContentItem(val, partId, isFetched);
}

/*!
    Returns resource from added content.
 */
QVariant NmMailViewerWK::loadResource(int type, const QUrl &name, NmId &partId, bool &isFetched)
{
    NM_FUNCTION;
    
    if (type == QTextDocument::ImageResource) {
        QString key = '<' + name.path() + '>';
        if (!mContent.contains(key)) {
            key = name.path();
        }
        if (mContent.contains(key)) {
            partId = mContent[key].mPartId;
            isFetched = mContent[key].mIsFetched;
            return mContent[key].mData;
        }
    }
    return 0;
}

/*!
    This is the main event handler that processes all incoming events in an
    appropriate manner.
 */
bool NmMailViewerWK::event(QEvent* event)
{
    bool consumed = false;
    if (event) {
        switch (event->type()) {
        case QEvent::Gesture:
            // Handle gesture events.
            gestureEvent(static_cast<QGestureEvent*>(event));
            consumed = event->isAccepted();
            break;
        case QEvent::GraphicsSceneContextMenu:
            // Handle context-menu events.
            // contextMenuEvent() is invoked directly in order to override
            // text selection in QWebPage.
            contextMenuEvent(static_cast<QGraphicsSceneContextMenuEvent*>(event));
            consumed = event->isAccepted();
            break;
        default:
            // Invoke base class' event handler.
            consumed = QGraphicsWebView::event(event);
            break;
        }
    }
    return consumed;
}

/*!
    Handles context-menu events.
 */
void NmMailViewerWK::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (event) {
        // Suppress context-menu invocations.
        event->accept();
    }
}

/*!
    Handles gesture events.
 */
void NmMailViewerWK::gestureEvent(QGestureEvent* event)
{
    if (event) {
        if (QTapGesture* tap = static_cast<QTapGesture*>(event->gesture(Qt::TapGesture))) {
            switch (tap->state()) {
            case Qt::GestureCanceled:
                // Tap cancellation suppresses the following mouse release.
                mSuppressRelease = true;
                break;
            default:
                // Other states disclose the following mouse release.
                mSuppressRelease = false;
                break;
            }
            event->accept();
        }
        if (QPinchGesture* pinch = static_cast<QPinchGesture*>(event->gesture(Qt::PinchGesture))) {
            // Handle pinch (zoom).
            // At the moment we simply consume the event.
            event->accept();
        }
    }
}

/*!
    Handles double-click events.
 */
void NmMailViewerWK::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event) {
        // Handle double clicks (instant zoom).
        // At the moment we simply consume the event.
        event->accept();
    }
}

/*!
    Handles mouse-move events.
 */
void NmMailViewerWK::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event) {
        // Suppress drag selection.
        event->accept();
    }
}

/*!
    Handles mouse-release events.
 */
void NmMailViewerWK::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event) {
        // Suppress mouse release if the previous tap was cancelled.
        // Otherwise, invoke the base class' event handler.
        if (mSuppressRelease) {
            event->accept();
        } else {
            QGraphicsWebView::mouseReleaseEvent(event);
        }
    }
}
