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
#include <QImage>
#include <QObject>
#include <fbs.h>
#include "cxestillcapturecontrolsymbian.h"
#include "cxecameradevicecontrol.h" // CxeCameraDevice
#include "cxeimagedataqueuesymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxutils.h"
#include "cxuifakestillcapturecontrol.h"
#include "cxesoundplayersymbian.h"
#include "cxestillimagesymbian.h"
#include "cxeviewfindercontrol.h"
#include "cxesettingsmappersymbian.h"


CxuiFakeStillCaptureControl::CxuiFakeStillCaptureControl(
    CxeCameraDevice &cameraDevice,
    CxeViewfinderControl &viewfinderControl,
    CxeCameraDeviceControl &cameraDeviceControl, CxeSoundPlayerSymbian &soundPlayer,
    CxeFilenameGenerator &nameGenerator)
    : CxeStillCaptureControlSymbian(cameraDevice,viewfinderControl, cameraDeviceControl, soundPlayer,nameGenerator),
      iCameraSnapshot(NULL),
      mCameraDevice(cameraDevice),
      mViewfinderControl(viewfinderControl),
      mCameraDeviceControl(cameraDeviceControl),
      mSoundPlayer(soundPlayer),
      mFilenameGenerator(nameGenerator)
{
    CX_DEBUG_IN_FUNCTION();
}


CxuiFakeStillCaptureControl::~CxuiFakeStillCaptureControl()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mImageDataQueue;
    delete iCameraSnapshot;

    qDeleteAll(mImages);

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiFakeStillCaptureControl::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Uninitialized) {
        prepare();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiFakeStillCaptureControl::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();
    setState( Uninitialized );
    CX_DEBUG_EXIT_FUNCTION();
}

int CxuiFakeStillCaptureControl::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT(mCameraDevice.camera());

    int err = KErrNone;
    int cameraIndex = mCameraDeviceControl.cameraIndex();

    TSize snapSize(480, 360);
    CCamera::TFormat imgFormat = CCamera::EFormatExif;
    CCamera::TFormat snapFormat = CCamera::EFormatFbsBitmapColor16MU;

    if ( cameraIndex == Cxe::SecondaryCameraIndex ) {
        CX_DEBUG(("Preparing secondary camera"));
        imgFormat = CCamera::EFormatFbsBitmapColor16M;
        snapSize.SetSize(320, 240);
    }

    setState(Ready);

    // Inform interested parties that image mode has been prepared for capture
    emit imagePrepareComplete(err);

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

void CxuiFakeStillCaptureControl::capture()
{
    CX_DEBUG_ENTER_FUNCTION();

    emit snapshotReady(0);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* Image Scene mode changed, needs updated
*/
void CxuiFakeStillCaptureControl::handleSceneChanged(CxeScene* scene)
{
    CX_DEBUG_IN_FUNCTION();
}


/*
* Ecam reference changing, release resources
*/

void CxuiFakeStillCaptureControl::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete iCameraSnapshot;
    iCameraSnapshot = NULL;

    setState(Uninitialized);

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiFakeStillCaptureControl::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Capturing) {
        mCameraDevice.camera()->CancelCaptureImage();
    }

    setState(Uninitialized);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* new camera available,
*/

void CxuiFakeStillCaptureControl::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(CxeStillCaptureControl::Uninitialized);

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiFakeStillCaptureControl::setFakeState(CxeStillCaptureControl::State aState)
{
    CX_DEBUG_ENTER_FUNCTION();
    setState(aState);
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
