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
#ifndef CXEFAKECAMERADEVICECONTROL_H
#define CXEFAKECAMERADEVICECONTROL_H

//  Include Files

#include "cxecameradevicecontrolsymbian.h"
#include <QObject>
//#include "cxecameradevice.h"
//#include "cxestatemachine.h"¨

class CxeCameraDeviceControlSymbian;


/**
 * Camera device control class for unit testing. Identical to
 * CxeCameraDeviceControlSymbian except for that CxeFakeCameraDevice is used
 * instead of the real implementation.
 */
class CxeCameraDeviceControlUnit : public CxeCameraDeviceControlSymbian 
{
    Q_OBJECT

public:

    CxeCameraDeviceControlUnit();
    virtual ~CxeCameraDeviceControlUnit();

protected: // from CxeCameraDeviceControlSymbian
    CxeError::Id  newCamera(Cxe::CameraIndex cameraIndex);
};

#endif  // CXEFAKECAMERADEVICECONTROL_H

