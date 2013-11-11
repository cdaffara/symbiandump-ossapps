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
* Description:
*
*/

#include <HbMainWindow>
#include <HbTransparentWindow>
#include <HbView>
#include <HbLabel>
#include <QGraphicsLinearLayout>
#include "cxeviewfindercontroldesktop.h"
#include "cxecameradevicedesktop.h"
#include "cxeviewfinderwidgetdesktop.h"
#include "cxutils.h"

const int KResWidth = 640;
const int KResHeight = 360;

CxeViewfinderControlDesktop::CxeViewfinderControlDesktop(CxeCameraDeviceDesktop &cameraDevice) :
        mState(Uninitialized),
        mResolution(KResWidth, KResHeight),
        mCameraDevice(cameraDevice),
        mViewfinderWidget(0)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeViewfinderControlDesktop::~CxeViewfinderControlDesktop()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Set Window ID
*/
void CxeViewfinderControlDesktop::setWindow(WId windowId)
{
    CX_DEBUG_ENTER_FUNCTION();
    mWindowId = windowId;

    if (state() != Running) {
        mState = Ready;
        emit stateChanged(mState, CxeError::None);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Start viewfinder
* @return CxeEngine specific error code
*/
CxeError::Id CxeViewfinderControlDesktop::start()
{
    CX_DEBUG_ENTER_FUNCTION();

    createViewfinderWidget();

    if (mState != Running) {
        mState = Running;
        emit stateChanged(mState, CxeError::None);
        mCameraDevice.start();
    }
    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}

/*!
* Stop viewfinder
*/
void CxeViewfinderControlDesktop::stop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mState = Ready;
    emit stateChanged(mState, CxeError::None);
    mCameraDevice.stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Current viewfinder state
*/
CxeViewfinderControl::State CxeViewfinderControlDesktop::state() const
{
    return mState;
}

/*!
* Returns Device's Display resolution
*/
QSize CxeViewfinderControlDesktop::deviceDisplayResolution() const
{
    return mResolution;
}

/*!
* Create the viewfinder widget.
*/
void CxeViewfinderControlDesktop::createViewfinderWidget()
{
    CX_DEBUG_ENTER_FUNCTION();
    HbMainWindow* mainWindow = qobject_cast<HbMainWindow*>(QWidget::find(mWindowId));
    if (mainWindow) {
        CX_DEBUG(("Found main window."));
        // Remove the current viewfinder from previous view scene.
        // Ownership is returned, so delete it now.
        if (mViewfinderWidget) {
            mViewfinderWidget->setParentItem(0);
            mainWindow->scene()->removeItem(mViewfinderWidget);
            delete mViewfinderWidget;
            mViewfinderWidget = 0;
        }

        // Find the visible transparent window and place our viewfinder widget as it's child.
        HbView* view = mainWindow->currentView();
        const QList<QGraphicsItem *> itemList(view->scene()->items());
        foreach (QGraphicsItem *item, itemList) {
            if (item->type() == Hb::ItemType_TransparentWindow) {
                CX_DEBUG(("Found transparent window, name: %s",
                          item->toGraphicsObject()
                          ? qPrintable(item->toGraphicsObject()->objectName())
                          : qPrintable(QString("<unknown>"))));
                bool visible(item->isVisible());
                CX_DEBUG(("Is found transparent window visible: %s", qPrintable(QVariant::fromValue(visible).toString())));
                if (visible) {
                    mViewfinderWidget = new CxeViewfinderWidgetDesktop();
                    connect(&mCameraDevice, SIGNAL(imageChanged(QPixmap)), mViewfinderWidget, SLOT(handleImageChange(QPixmap)));
                    mViewfinderWidget->setParentItem(item);
                    break;
                }
            }
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
