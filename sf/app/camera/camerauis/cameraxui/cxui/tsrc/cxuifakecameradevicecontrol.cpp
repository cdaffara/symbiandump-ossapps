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
#include "cxecameradevicecontrolsymbian.h"
#include "cxuifakecameradevicecontrol.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "CxeSettingsModel.h"
#include "cxutils.h"
#include "cxenamespace.h"

#ifdef __WINSCW__
#include "cxedummycamera.h"
#endif


CxuiFakeCameraDeviceControl::CxuiFakeCameraDeviceControl()
    : CxeCameraDeviceControlSymbian(),
    mCameraDevice(NULL),
    mCameraIndex(Cxe::PrimaryCameraIndex),
    mCameraMode(Cxe::ImageMode)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiFakeCameraDeviceControl::~CxuiFakeCameraDeviceControl()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxuiFakeCameraDeviceControl::reserve()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Idle) {
        setState(Initializing);
    }

    setState( Ready );
    emit deviceReady();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiFakeCameraDeviceControl::powerOn()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT(state() == Initializing);
    emit deviceReady();

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
