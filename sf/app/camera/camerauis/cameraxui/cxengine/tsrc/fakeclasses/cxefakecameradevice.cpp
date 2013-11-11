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
#include "cxefakecameradevice.h"
#include "cxeerrormappingsymbian.h"
#include "cxutils.h"
#include "cxedummycamera.h"

CxeFakeCameraDevice::CxeFakeCameraDevice() : CxeCameraDevice()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeCameraDevice::~CxeFakeCameraDevice()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeError::Id CxeFakeCameraDevice::newCamera(Cxe::CameraIndex cameraIndex, MCameraObserver2 *observer) {

    CX_DEBUG_ENTER_FUNCTION();

    CCamera* camera = NULL;
    TRAPD(err, camera = CxeDummyCamera::NewL(*observer, cameraIndex, 100, 2));

    if (!err) {
        setCamera(camera);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

// end of file
