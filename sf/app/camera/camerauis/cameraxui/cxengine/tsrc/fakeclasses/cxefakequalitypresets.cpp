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
#include <QList>
#include "cxequalitydetails.h"
#include "cxefakequalitypresets.h"
#include "cxequalitypresetssymbian.h"


CxeFakeQualityPresets::CxeFakeQualityPresets()
{
}

CxeFakeQualityPresets::~CxeFakeQualityPresets()
{
}

QList<CxeImageDetails> CxeFakeQualityPresets::imageQualityPresets(Cxe::CameraIndex cameraId)
{
    Q_UNUSED(cameraId);

    QList<CxeImageDetails> list;
    list.append(fakeImageDetails());
    return list;
}


QList<CxeVideoDetails> CxeFakeQualityPresets::videoQualityPresets(Cxe::CameraIndex cameraId)
{
    Q_UNUSED(cameraId);

    QList<CxeVideoDetails> list;
    list.append(fakeVideoDetails());
    return list;
}

int CxeFakeQualityPresets::recordingTimeAvailable(const CxeVideoDetails& details, qint64 space)
{
    int time(0);
    if (details.mMaximumSizeInBytes > 0 && details.mMaximumSizeInBytes < space) {
        time = details.mMaximumSizeInBytes / 1000; // 1 kB/s
    } else {
        time = space / 1000;
    }
    return time;
}

CxeImageDetails CxeFakeQualityPresets::fakeImageDetails()
{
    CxeImageDetails quality;

    quality.mWidth = 640;
    quality.mHeight = 360;
    quality.mEstimatedSize = 300000;
    quality.mMpxCount = QString::number(qreal(0.2));
    quality.mPossibleImages = 1000;
    quality.mAspectRatio = Cxe::AspectRatio16to9;

    return quality;
}

CxeVideoDetails CxeFakeQualityPresets::fakeVideoDetails()
{
    CxeVideoDetails quality;

    quality.mWidth = 640;
    quality.mHeight = 360;
    quality.mRemainingTime = 1000;
    quality.mAudioBitRate = 0;
    quality.mVideoBitRate = 3072000;
    quality.mVideoFrameRate = 30;
    quality.mAudioType = QString("");
    quality.mVideoFileMimeType = QString("video/mp4");
    quality.mPreferredSupplier = QString("Nokia");
    quality.mVideoCodecMimeType = QString("video/mp4v-es; profile-level-id=4");
    quality.mAspectRatio = Cxe::AspectRatio16to9;

    return quality;
}

