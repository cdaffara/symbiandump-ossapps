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

#ifndef CXESNAPSHOTCONTROLPRIVATE_H
#define CXESNAPSHOTCONTROLPRIVATE_H


#include <QObject>
#include "cxeerror.h"
#include "cxestatemachine.h"

class CxeSnapshotControl;
class CxeCameraDevice;


/*!
* Snapshot control private implementation.
*/
class CxeSnapshotControlPrivate : public CxeStateMachine
{
private:
    CxeSnapshotControlPrivate(CxeSnapshotControl *parent, CxeCameraDevice &device);
    virtual ~CxeSnapshotControlPrivate();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

private:
    CxeSnapshotControl::State state() const;
    void initializeStates();
    QSize calculateSnapshotSize(const QSize &displaySize, Cxe::AspectRatio aspectRatio) const;
    void start(const QSize &size);
    void stop();
    void handleCameraEvent(int id, int error);
    QImage snapshot();

private:
    CxeSnapshotControl *q;
    CxeCameraDevice &mDevice;

    friend class CxeSnapshotControl;
};

#endif // CXESNAPSHOTCONTROLPRIVATE_H
