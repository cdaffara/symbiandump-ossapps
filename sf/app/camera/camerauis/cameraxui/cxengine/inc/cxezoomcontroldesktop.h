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

#ifndef CXEZOOMCONTROLDESKTOP_H
#define CXEZOOMCONTROLDESKTOP_H

#include "cxezoomcontrol.h"
#include "cxestatemachine.h"
#include "cxecameradevicecontrol.h"

class CxeCameraDeviceControl;
class CxeSettings;
class CxeFeatureManager;

class CxeZoomControlDesktop : public CxeZoomControl
{
    Q_OBJECT

public: // constructors

    CxeZoomControlDesktop(CxeCameraDeviceControl &cameraDeviceControl);

    virtual ~CxeZoomControlDesktop();

public:
    // from CxeZoomControl
    CxeZoomControl::State state() const;
    int min() const;
    int max() const;
    void zoomTo(int value);

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
    void finalizeZoomPreparation(int error);

private:
    // private data

    CxeCameraDeviceControl &mCameraDeviceControl;

    // Zoom levels
    int mCurrentZoomLevel;
    int mMinZoomLevel;
    int mMaxZoomLevel;

    // values ready from feature manager.
    QList<int> mStillMaxZoomLimits;
    QList<int> mVideoMaxZoomLimits;

    CxeZoomControl::State mState;
};

#endif // CXEZOOMCONTROLDESKTOP_H
