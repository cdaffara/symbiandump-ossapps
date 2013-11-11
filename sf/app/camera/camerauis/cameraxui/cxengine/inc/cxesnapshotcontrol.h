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

#ifndef CXESNAPSHOTCONTROL_H
#define CXESNAPSHOTCONTROL_H

#include <QObject>
#include "cxeerror.h"
#include "cxenamespace.h"

class CxeCameraDevice;
class CxeSnapshotControlPrivate;

/*!
* Snapshot control class.
*/
class CxeSnapshotControl : public QObject
{
    Q_OBJECT

public:

    // State Machine
    enum State {
        //! Idle, snapshot not active.
        Idle   = 0x01,
        //! Active, snapshot events provided after successful capture.
        Active = 0x02
    };


    explicit CxeSnapshotControl(CxeCameraDevice &device);
    virtual ~CxeSnapshotControl();

public:
    State state() const;
    QSize calculateSnapshotSize(const QSize& displaySize, Cxe::AspectRatio aspectRatio) const;
    void start(const QSize &size);
    void stop();

signals:
    void stateChanged(CxeSnapshotControl::State newState, CxeError::Id status);
    void snapshotReady(CxeError::Id status, const QImage &snapshot);

public slots:
    void handleCameraEvent(int id, int error);

private:
    CxeSnapshotControlPrivate *d;

    Q_DISABLE_COPY(CxeSnapshotControl)
    friend class CxeSnapshotControlPrivate;
};

Q_DECLARE_METATYPE(CxeSnapshotControl::State)

#endif // CXESNAPSHOTCONTROL_H
