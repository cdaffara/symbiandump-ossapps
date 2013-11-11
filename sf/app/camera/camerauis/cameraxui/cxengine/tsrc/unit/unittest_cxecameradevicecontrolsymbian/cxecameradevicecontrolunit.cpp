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
#include "cxecameradevicecontrolunit.h"
#include "cxefakecameradevice.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerrormappingsymbian.h"
#include "cxedummycamera.h"

CxeCameraDeviceControlUnit::CxeCameraDeviceControlUnit()
    : CxeCameraDeviceControlSymbian()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeCameraDeviceControlUnit::~CxeCameraDeviceControlUnit()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeError::Id CxeCameraDeviceControlUnit::newCamera(Cxe::CameraIndex cameraIndex)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeCameraDeviceControlUnit: using camera index %d", cameraIndex));

    CX_DEBUG_ASSERT(cameraIndex == Cxe::PrimaryCameraIndex || cameraIndex == Cxe::SecondaryCameraIndex);

    CCamera* camera = NULL;
    TRAPD(err, camera = CxeDummyCamera::NewL(*this, cameraIndex, 100, 2));

    if (!err) {
        // CCamera object created, so make sure that appropriate index is used
        mCameraIndex = cameraIndex;

        if (mCameraDevice == NULL) { // creating for first time.
            mCameraDevice = new CxeFakeCameraDevice();
        } else {
            mCameraDevice->setCamera(camera);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

// end of file
