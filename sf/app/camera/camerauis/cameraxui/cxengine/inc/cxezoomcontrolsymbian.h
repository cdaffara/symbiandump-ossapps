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
* Description:
*
*/
#ifndef CXEZOOMCONTROLSYMBIAN_H
#define CXEZOOMCONTROLSYMBIAN_H

#include "cxezoomcontrol.h"
#include "cxecameradevice.h" // CxeCameraDevice
#include "cxestatemachine.h"
#include "cxecameradevicecontrol.h"

class CxeCameraDeviceControl;
class CxeSettings;
class CxeFeatureManager;

class CxeZoomControlSymbian : public CxeZoomControl, public CxeStateMachine
{
Q_OBJECT

public: // constructors

    CxeZoomControlSymbian(CxeCameraDevice &cameraDevice,
                          CxeCameraDeviceControl &cameraDeviceControl,
                          CxeSettings &settings,
                          CxeFeatureManager &featureManager);

    virtual ~CxeZoomControlSymbian();

public:
    // from CxeZoomControl
    CxeZoomControl::State state() const;
    int min() const;
    int max() const;
    void zoomTo(int value);

protected:
    // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

protected slots:

    // Slot to react to still capture signal:
    // prepares the zoom control for still.
    void prepareZoomForStill(int ecamStillResolutionIndex);
    
    // Slot to react to video capture signal:
    // prepares the zoom control for video.
    void prepareZoomForVideo();

    // Slot to prepare for camera delete signal.
    void reset();

private:
    // private member functions
    void init();
    void initializeStates();
    void finalizeZoomPreparation(int error);

private:
    // private data

    CxeCameraDevice &mCameraDevice;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeSettings &mSettings;

    // Zoom levels
    int mCurrentZoomLevel;
    int mMinZoomLevel;
    int mMaxZoomLevel;

    // The RArray is used to map the integers between "min" and "max" to adaptation understandable values.
    RArray<TInt> mDigitalZoomValues;
    TValueInfo mDigitalZoomValueInfo;

    // values ready from feature manager.
    QList<int> mStillMaxZoomLimits;
    QList<int> mVideoMaxZoomLimits;
};

#endif // CXEZOOMCONTROLSYMBIAN_H
