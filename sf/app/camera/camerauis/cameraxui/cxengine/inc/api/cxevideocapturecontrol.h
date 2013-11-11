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
#ifndef CXEVIDEOCAPTURECONTROL_H_
#define CXEVIDEOCAPTURECONTROL_H_

#include <QObject>
#include <QMetaType>
#include "cxeerror.h"
#include "cxequalitydetails.h"

class QPixmap;

class CxeVideoCaptureControl : public QObject
{
    Q_OBJECT
public:
    // State Machine
    enum State {
        //! Not Initialized
        Idle                = 0x01,

        //! Video Recorder Initialized, but not ready for recording
        Initialized         = 0x02,

        //! Preparing for recording
        Preparing           = 0x04,

        //! Ready to record
        Ready               = 0x08,

        //! Recording
        Recording           = 0x10,

        // Paused
        Paused              = 0x20,

        // Stopping and finalizing output file
        Stopping            = 0x40,

        // Playing capture sound before recording
        PlayingStartSound   = 0x80
    };

    virtual State state() const = 0;
    virtual void record() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void remainingTime(int &time) = 0;
    virtual bool elapsedTime(int &time) = 0;

    /**
     * Releases the current video snapshot and resets the file name.
     */
    virtual void reset() = 0;

    /**
     * Get the filename for the current/previous video clip.
     */
    virtual QString filename() const = 0;

    /**
     * Get the video snapshot image.
     *
     * @return Snapshot pixmap. May be null pixmap if no snapshot is available.
     */
    virtual QPixmap snapshot() const = 0;

    /*
    * Returns supported list of video qualities, sorted in descending order
    * i.e. the first element in the list has the highest video resolution first and so on
    */
    virtual QList<CxeVideoDetails> supportedVideoQualities() = 0;

public slots:
    virtual void init() = 0;
    virtual void deinit() = 0;

signals:
    void videoComposed(CxeError::Id error, const QString &filename);
    void stateChanged(CxeVideoCaptureControl::State newState, CxeError::Id error);
    void snapshotReady(CxeError::Id error, const QImage &snapshot, const QString &filename);
    void remainingTimeChanged();

protected:
    CxeVideoCaptureControl() {}

private:
    Q_DISABLE_COPY( CxeVideoCaptureControl )
};

Q_DECLARE_METATYPE(CxeVideoCaptureControl::State)

#endif /* CXEVIDEOCAPTURECONTROL_H_ */
