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
#ifndef CXEQUALITYPRESETS_H
#define CXEQUALITYPRESETS_H


#include "cxequalitydetails.h"


/**!
* Interface Class for CxeQualityPresetsSymbian
* Queries supported image and video qualities from ICM and creates the related
* data and wraps up the quality settings for the client.
*/
class CxeQualityPresets
{
public:
    virtual ~CxeQualityPresets() {};
    virtual QList<CxeImageDetails> imageQualityPresets(Cxe::CameraIndex cameraId) = 0;
    virtual QList<CxeVideoDetails> videoQualityPresets(Cxe::CameraIndex cameraId) = 0;
    virtual int recordingTimeAvailable(const CxeVideoDetails& details, qint64 space) = 0;
};

#endif  // CXEQUALITYPRESETS_H
