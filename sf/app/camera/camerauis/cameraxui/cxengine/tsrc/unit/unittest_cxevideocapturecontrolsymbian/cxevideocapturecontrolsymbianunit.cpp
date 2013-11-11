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
#include <AudioPreference.h>    // For KAudioPriorityVideoRecording

#include "cxeerrormappingsymbian.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbianunit.h"
#include "cxefakeVideorecorderutility.h"
#include "cxutils.h"
#include "cxediskmonitor.h"

CxeVideoCaptureControlSymbianUnit::CxeVideoCaptureControlSymbianUnit(
                                   CxeCameraDevice &cameraDevice,
                                   CxeViewfinderControl &viewfinderControl,
                                   CxeSnapshotControl &snapshotControl,
                                   CxeCameraDeviceControl &cameraDeviceControl,
                                   CxeFilenameGenerator &nameGenerator,
                                   CxeSettings &settings,
                                   CxeQualityPresets &qualityPresets,
                                   CxeDiskMonitor &diskMonitor)
    : CxeVideoCaptureControlSymbian(cameraDevice, viewfinderControl, snapshotControl,
                                    cameraDeviceControl, nameGenerator,
                                    settings, qualityPresets, diskMonitor)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeVideoCaptureControlSymbianUnit::~CxeVideoCaptureControlSymbianUnit()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeVideoCaptureControlSymbianUnit::createVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();

    // init video recoder
    if (state() == CxeVideoCaptureControl::Idle) {
        if (mVideoRecorder == NULL) {
            TRAPD(initErr, mVideoRecorder =
                new CxeFakeVideoRecorderUtility( *this ,
                            KAudioPriorityVideoRecording,
                            TMdaPriorityPreference( KAudioPrefVideoRecording )));
            if (initErr) {
                CX_DEBUG(("WARNING - VideoRecorderUtility could not be reserved. Failed with err:%d", initErr));
                mVideoRecorder = NULL;
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
