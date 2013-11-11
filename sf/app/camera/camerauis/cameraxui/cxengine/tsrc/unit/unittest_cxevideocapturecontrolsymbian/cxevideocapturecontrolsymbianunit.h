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
#ifndef CXEVIDEOCAPTURECONTROLSYMBIANUNIT_H
#define CXEVIDEOCAPTURECONTROLSYMBIANUNIT_H

//  Include Files

#include "cxevideocapturecontrolsymbian.h"


/**
 * Camera device control class for unit testing. Identical to
 * CxeCameraDeviceControlSymbian except for that CxeFakeCameraDevice is used
 * instead of the real implementation.
 */
class CxeVideoCaptureControlSymbianUnit : public CxeVideoCaptureControlSymbian
{
	Q_OBJECT
public:

    CxeVideoCaptureControlSymbianUnit(CxeCameraDevice &cameraDevice,
                                   CxeViewfinderControl &viewfinderControl,
                                   CxeSnapshotControl &snapshotControl,
                                   CxeCameraDeviceControl &cameraDeviceControl,
                                   CxeFilenameGenerator &nameGenerator,
                                   CxeSettings &settings,
                                   CxeQualityPresets &qualityPresets,
                                   CxeDiskMonitor &diskMonitor);
    virtual ~CxeVideoCaptureControlSymbianUnit();
    void createVideoRecorder();

};

#endif  // CXEVIDEOCAPTURECONTROLSYMBIANUNIT_H
