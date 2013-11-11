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
* Description:
*
*/

#ifndef CXEFAKEVIDEOCAPTURECONTROL_H
#define CXEFAKEVIDEOCAPTURECONTROL_H

#include <QObject>
#include <QMetaType>
#include "cxeerror.h"
#include "cxevideocapturecontrol.h"
#include "cxestatemachine.h"

class QPixmap;

class CxeFakeVideoCaptureControl : public CxeVideoCaptureControl,
                                   public CxeStateMachine
{
    Q_OBJECT
public:
    CxeFakeVideoCaptureControl();
    virtual ~CxeFakeVideoCaptureControl();

    State state() const;
    void record();
    void pause();
    void stop();
    void remainingTime(int &time);
    bool elapsedTime(int &time);
    void reset();
    QString filename() const;
    QPixmap snapshot() const;
    QList<CxeVideoDetails> supportedVideoQualities();

protected:
    void handleStateChanged(int newStateId, CxeError::Id error);
    void initializeStates();

public slots:
    void init();
    void deinit();

public: // methods for unit testing
    enum MethodIndex
    {
        Capture,
        Reset,
        CancelAll,
        SetMode,
        Init,
        Deinit
    };

    void setState(CxeVideoCaptureControl::State newState);
    QList<MethodIndex> callHistory() const;
    void resetCallHistory();

private: // data
    QList<MethodIndex> mCallHistory;
};

#endif // CXEFAKEVIDEOCAPTURECONTROL_H
