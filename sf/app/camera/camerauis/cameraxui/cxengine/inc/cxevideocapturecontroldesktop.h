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

#ifndef CXEVIDEOCAPTURECONTROLDESKTOP_H_
#define CXEVIDEOCAPTURECONTROLDESKTOP_H_

#include <QTime>
#include <QPixmap>
#include <QTimer>

#include "cxesettings.h"
#include "cxevideocapturecontrol.h"
#include "cxestatemachine.h"
#include "cxenamespace.h"


// Custom command for setting a new filename without closing & reopening the controller
enum TCamCControllerCustomCommands
    {
    ECamCControllerCCNewFilename = 0,
    ECamCControllerCCVideoStopAsync
    };


// forward declarations
class CxeCameraDeviceDesktop;
class CxeQualityPresets;
class CxeFilenameGenerator;
class CxeViewfinderControl;
class CxeCameraDeviceControl;


class CxeVideoCaptureControlDesktop : public CxeVideoCaptureControl
{
    Q_OBJECT

public:  // constructors

    CxeVideoCaptureControlDesktop( CxeCameraDeviceDesktop &cameraDevice,
                                   CxeViewfinderControl &viewfinderControl,
                                   CxeCameraDeviceControl &cameraDeviceControl,
                                   CxeFilenameGenerator &nameGenerator,
                                   CxeQualityPresets &qualityPresets);

    virtual ~CxeVideoCaptureControlDesktop();

public:  // functions from CxeVideoCaptureControl
    CxeVideoCaptureControl::State state() const;
    void record();
    void pause();
    void stop();
    void remainingTime(int &time);
    bool elapsedTime(int &time);
    void reset();
    QString filename() const;
    QPixmap snapshot() const;
    QList<CxeVideoDetails> supportedVideoQualities();

public:  // public member functions, not in client API
    void handleSnapshotEvent(CxeError::Id error);

signals:
    void videoPrepareComplete(CxeError::Id error);
    void prepareZoomForVideo();

public slots:
    //! Notification for when camera device is ready and we're entering video mode
    void init();
    //! Notification for when we're exiting video mode
    void deinit();
    //! Notification that videocapture sound has been played
    void handleSoundPlayed();

protected slots:
    // notifications when ECam reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);
    void prepareForRelease();
    // ECam events
    void handleCameraEvent(int eventUid, int error);
    // settings call back
    void handleSettingValueChanged(const QString &settingId, QVariant newValue);
    void handleSceneChanged(CxeScene &scene);

private: // helper methods
    void releaseResources();
    void initializeStates();
    int prepareVideoSnapshot();
    void initVideoRecorder();
    void open();
    void prepare();
    void createVideoRecorder();
    void setupElapseTimer();
    void setState(CxeVideoCaptureControl::State stateId, CxeError::Id error = CxeError::None);

private slots:
    void handleElapseTimeout();
    void startViewfinder();

private: // private data

    CxeCameraDeviceDesktop &mCameraDevice;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeViewfinderControl &mViewfinderControl;
    CxeFilenameGenerator &mFilenameGenerator;
    CxeQualityPresets &mQualityPresets;
    //! Snapshot image.
    QPixmap mSnapshot;
    //! New file name generated for the video prepare.
    QString mNewFileName;
    //! Current video file name
    QString mCurrentFilename;
    CxeVideoCaptureControl::State mState;
    //current video quality details in use
    CxeVideoDetails mCurrentVideoDetails;
    
    //timer for simulating elapsing time
    QTimer mRecordElapseTimer;
    QTimer mViewFinderStartTimer;
    
    //elapsed recording time in seconds
    int mElapsedTime;
};

#endif /* CXEVIDEOCAPTURECONTROLDESKTOP_H_ */
