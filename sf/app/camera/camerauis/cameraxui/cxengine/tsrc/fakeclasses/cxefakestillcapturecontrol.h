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
#ifndef CXEFAKESTILLCAPTURECONTROL_H
#define CXEFAKESTILLCAPTURECONTROL_H

#include "cxestillcapturecontrol.h"
#include "cxestatemachine.h"
#include "cxenamespace.h"
#include "cxequalitydetails.h"

class CxeSoundPlayerSymbian;
class CxeFilenameGenerator;
class CxeCameraDevice;
class CxeStillImageSymbian;
class CxeImageDataQueue;
class CxeImageDataQueueSymbian;
class CxeViewfinderControl;
class CxeCameraDeviceControl;

class CxeFakeStillCaptureControl : public CxeStillCaptureControl,
                                   public CxeStateMachine

{
    Q_OBJECT

public:  // constructors
    CxeFakeStillCaptureControl();
    virtual ~CxeFakeStillCaptureControl();

public: // from CxeStillCaptureControl
    void capture();
    State state() const;
    int imageCount() const;
    void reset();
    void cancelAll();
    void setMode(CaptureMode mode);
    CaptureMode mode() const;
    CxeStillImage &operator[](int index);
    CxeImageDataQueue &imageDataQueue();
    QList<CxeImageDetails> supportedImageQualities();
    int imagesLeft();

public slots:
    void init();
    void deinit();

signals:
    void imagePrepareComplete(CxeError::Id error);

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

    void setState(CxeStillCaptureControl::State newState);
    QList<MethodIndex> callHistory() const;
    void resetCallHistory();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);
    void initializeStates();

private: // data
    QList<MethodIndex> mCallHistory;
};

#endif // CXEFAKESTILLCAPTURECONTROL_H
