/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXEVIDEOCAPTURECONTROLSYMBIAN_H_
#define CXEVIDEOCAPTURECONTROLSYMBIAN_H_

#include <QTime>
#include <QPixmap>
#include <QString>
#include <driveinfo.h>
#include <videorecorder.h>

#include "cxesettings.h"
#include "cxequalitypresets.h"
#include "cxequalitypresetssymbian.h"
#include "cxestillimagesymbian.h"
#include "cxevideocapturecontrol.h"
#include "cxestatemachine.h"
#include "cxenamespace.h"


// forward declarations
class CxeSettings;
class CxeCameraDevice;
class CxeQualityPresets;
class CxeFilenameGenerator;
class CxeViewfinderControl;
class CxeSnapshotControl;
class CxeStillImageSymbian;
class CxeSoundPlayerSymbian;
class CxeCameraDeviceControl;
class CxeVideoRecorderUtility;
class CxeDiskMonitor;


class CxeVideoCaptureControlSymbian : public CxeVideoCaptureControl,
                                      public CxeStateMachine,
                                      public MVideoRecorderUtilityObserver
{
    Q_OBJECT

public:  // constructors

    CxeVideoCaptureControlSymbian(CxeCameraDevice &cameraDevice,
                                  CxeViewfinderControl &viewfinderControl,
                                  CxeSnapshotControl &snapshotControl,
                                  CxeCameraDeviceControl &cameraDeviceControl,
                                  CxeFilenameGenerator &nameGenerator,
                                  CxeSettings &settings,
                                  CxeQualityPresets &qualityPresets,
                                  CxeDiskMonitor &diskMonitor);

    virtual ~CxeVideoCaptureControlSymbian();

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

public: // functions from MVideoRecorderUtilityObserver
    void MvruoOpenComplete(TInt aError);
    void MvruoPrepareComplete(TInt aError);
    void MvruoRecordComplete(TInt aError);
    void MvruoEvent(const TMMFEvent& aEvent);

signals:
    void videoPrepareComplete(CxeError::Id error);
    void prepareZoomForVideo();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

public slots:
    //! Notification for when camera device is ready and we're entering video mode
    void init();
    //! Notification for when we're exiting video mode
    void deinit();
    //! Notification that videocapture sound has been played
    void handleSoundPlayed();
    //! Snapshot has been received from adaptiation.
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot);

protected slots:
    // notifications when ECam reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);
    void prepareForRelease();
    // settings call back
    void handleSettingValueChanged(const QString &settingId, QVariant newValue);
    // scene mode change
    void handleSceneChanged(const QVariant &scene);
    // Disk space change
    void handleDiskSpaceChanged();
    void hintUseCase();

private: // helper methods
    void releaseResources();
    void initializeStates();
    void updateVideoCaptureParameters();
    void initVideoRecorder();
    void open();
    void prepare();
    void prepareSnapshot();
    virtual void createVideoRecorder();
    int calculateRemainingTime(const CxeVideoDetails &videoDetails);
    void updateRemainingRecordingTimeCounter();
    void generateFilename();
    void handlePrepareFailed();
    void handleComposeFailed(int error);
    void emulateNormalStopping();

protected: // protected data
    //! Video Recorder
    CxeVideoRecorderUtility *mVideoRecorder;

private: // private data

    //! Video Recorder controller UID.
    TUid mVideoControllerUid;
    //! Video Recorder format UID.
    TUid mVideoFormatUid;
    CxeCameraDevice &mCameraDevice;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeViewfinderControl &mViewfinderControl;
    CxeSnapshotControl &mSnapshotControl;
    CxeFilenameGenerator &mFilenameGenerator;
    CxeSettings &mSettings;
    CxeQualityPresets &mQualityPresets;
    CxeDiskMonitor &mDiskMonitor;
    //! Snapshot image.
    QPixmap mSnapshot;
    //! Soundplayers, own
    CxeSoundPlayerSymbian *mVideoStartSoundPlayer;
    CxeSoundPlayerSymbian *mVideoStopSoundPlayer;
    //! Current video file name
    QString mCurrentFilename;
    //video resolutions supported by ICM
    QList<CxeVideoDetails> mIcmSupportedVideoResolutions;
    //current video quality details in use
    CxeVideoDetails mCurrentVideoDetails;
};

#endif /* CXEVIDEOCAPTURECONTROLSYMBIAN_H_ */
