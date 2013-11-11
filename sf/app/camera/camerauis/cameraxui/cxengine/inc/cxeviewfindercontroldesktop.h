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
#ifndef CXEVIEWFINDERCONTROLDESKTOP_H_
#define CXEVIEWFINDERCONTROLDESKTOP_H_

#include "cxeviewfindercontrol.h"

class CxeCameraDeviceDesktop;
class CxeViewfinderWidgetDesktop;

class CxeViewfinderControlDesktop : public CxeViewfinderControl
{
    Q_OBJECT

public:
    CxeViewfinderControlDesktop(CxeCameraDeviceDesktop &cameraDevice);
    virtual ~CxeViewfinderControlDesktop();

    virtual void setWindow(WId windowId);

    virtual CxeError::Id start();
    
    virtual void stop();
    
    virtual CxeViewfinderControl::State state() const;

    virtual QSize deviceDisplayResolution() const;

private:
    void createViewfinderWidget();

private:
    State mState;
    WId mWindowId;
    QSize mResolution;
    CxeCameraDeviceDesktop &mCameraDevice;
    CxeViewfinderWidgetDesktop *mViewfinderWidget;

};

#endif /*CXEVIEWFINDERCONTROLDESKTOP_H_*/
