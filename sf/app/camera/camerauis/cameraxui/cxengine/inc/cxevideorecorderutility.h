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
#ifndef CXEVIDEORECORDERUTILITY_H_
#define CXEVIDEORECORDERUTILITY_H_

#include <QObject>
#include <QString>
#include <QSize>

/*!
* Video Recorder Utility interface.
*/
class CxeVideoRecorderUtility
{
public:

    virtual ~CxeVideoRecorderUtility() {}

    virtual void open(int cameraHandle,
                      const QString &filename,
                      const QString &fileMimeType,
                      const QString &supplier,
                      const QString &videoType,
                      const QString &aAudioType) = 0;
    virtual void setVideoFrameSize(const QSize& size) = 0;
    virtual void setVideoFrameRate(int rate) = 0;
    virtual void setVideoBitRate(int rate) = 0;
    virtual void setAudioEnabled(bool enable) = 0;
    virtual void setVideoMaxSize(int sizeInBytes) = 0;
    virtual void close() = 0;
    virtual void prepare() = 0;
    virtual void record() = 0;
    virtual void stop(bool asynchronous = false) = 0;
    virtual void pause() = 0;
    virtual int availableRecordingTime() = 0;
    virtual int duration() = 0;

protected:
    CxeVideoRecorderUtility() {}

private:
    Q_DISABLE_COPY( CxeVideoRecorderUtility )
};


#endif /*CXEVIDEORECORDERUTILITY_H_*/
