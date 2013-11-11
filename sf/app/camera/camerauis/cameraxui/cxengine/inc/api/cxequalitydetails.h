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
#ifndef CXEQUALITYDETAILS_H
#define CXEQUALITYDETAILS_H

#include "cxenamespace.h"


/*!
* data structure that holds ALL details of Image.
*/
struct CxeImageDetails
{
    static const int UNKNOWN = -1;

    int mWidth;
    int mHeight;
    int mEstimatedSize; // size in bytes
    QString mMpxCount; // mega pixel count
    int mPossibleImages; // no of images that can be taken, with current resolution
    QString mImageFileExtension;
    Cxe::AspectRatio mAspectRatio;
    // for sorting list of CxeImageDetails.
    bool operator<(const CxeImageDetails &s1) const;
};


/*!
data structure that holds ALL details of Video.
*/
struct CxeVideoDetails
{
    static const int UNKNOWN = -1;

    int mWidth;
    int mHeight;
    int mRemainingTime;
    int mMaximumSizeInBytes; // zero means no limit
    int mAudioBitRate;
    int mVideoBitRate;
    qreal mVideoFrameRate;
    QString mAudioType;
    QString mVideoFileMimeType;
    QString mPreferredSupplier;
    QString mVideoCodecMimeType;
    Cxe::AspectRatio mAspectRatio;

    // for sorting list of CxeVideoDetails.
    bool operator<(const CxeVideoDetails &s1) const;
};


#endif  // CXEQUALITYDETAILS_H
