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

#include <QSize>
#include "cxutils.h"
#include "cxesnapshotcontrol.h"
#include "cxesnapshotcontrolprivate.h"

/*!
* Constructor.
* @param device Camera device interface, used for getting hold of adaptation snapshot API.
*/
CxeSnapshotControl::CxeSnapshotControl(CxeCameraDevice& device)
{
    d = new CxeSnapshotControlPrivate(this, device);
    CX_ASSERT_ALWAYS(d);
}

/*!
* Destructor.
*/
CxeSnapshotControl::~CxeSnapshotControl()
{
    delete d;
}

/*!
* Get the state of Snapshot Control.
* @return The current state.
*/
CxeSnapshotControl::State CxeSnapshotControl::state() const
{
    return d->state();
}

/*!
* Calculate snapshot size based on diplay size and image / video output resolution.
* @param displaySize Display size in pixels.
* @param outputResolution Resolution of the output image / video in pixels.
* @return Proposed best snapshot size.
*/
QSize CxeSnapshotControl::calculateSnapshotSize(const QSize& displaySize, Cxe::AspectRatio aspectRatio) const
{
    return d->calculateSnapshotSize(displaySize, aspectRatio);
}

/*!
* Start snapshot events. Snapshots will be signalled
* through snapshotReady() signal.
* @param size Size of the snapshot image.
*/
void CxeSnapshotControl::start(const QSize& size)
{
    d->start(size);
}

/*!
* Stop snapshot events.
*/
void CxeSnapshotControl::stop()
{
    d->stop();
}

/*!
* Handle camera events.
* @param id Event uid.
* @param error Status code of the event.
*/
void CxeSnapshotControl::handleCameraEvent(int eventUid, int error)
{
    d->handleCameraEvent(eventUid, error);
}

// end of file
