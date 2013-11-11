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

#ifndef CXEQUALITYPRESETSDESKTOP_H
#define CXEQUALITYPRESETSDESKTOP_H

//  Include Files
#include "cxequalitypresets.h"

class CxeQualityPresetsDesktop : public CxeQualityPresets
{
public:
    CxeQualityPresetsDesktop();
    virtual ~CxeQualityPresetsDesktop();
    QList<CxeImageDetails> imageQualityPresets(Cxe::CameraIndex cameraId);
    QList<CxeVideoDetails> videoQualityPresets(Cxe::CameraIndex cameraId);
    int recordingTimeAvailable(const CxeVideoDetails &details, qint64 space);

};

#endif  // CXEQUALITYPRESETSDESKTOP_H
