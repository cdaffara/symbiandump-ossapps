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
#ifndef CXEENGINE_H
#define CXEENGINE_H

#include <QObject>
#include "cxenamespace.h"
#include "cxengine_global.h"
#include "cxeerror.h"

class CxeCameraDeviceControl;
class CxeViewfinderControl;
class CxeSnapshotControl;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;
class CxeAutoFocusControl;
class CxeZoomControl;
class CxeImageDataQueue;
class CxeFeatureManager;
class CxeSettings;
class CxeMemoryMonitor;
class CxeGeoTaggingTrail;

/**
 * CxeEngine provides the the entry point into CameraX engine. This is the
 * first class the client needs to initialize. CxeEngine then provides
 * references to other CameraX engine objects.
 */
class CxeEngine : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new engine instance. This is the first call the client needs to
     * do when using CxeEngine.
     */
    CAMERAX_ENGINE_EXPORT static CxeEngine* createEngine();

    virtual ~CxeEngine();

    /**
     * Access the camera device control.
     */
    virtual CxeCameraDeviceControl &cameraDeviceControl() = 0;

    /**
     * Access the viewfinder control.
     */
    virtual CxeViewfinderControl &viewfinderControl() = 0;

    /**
     * Access the snapshot control.
     */
    virtual CxeSnapshotControl &snapshotControl() = 0;

    /**
     * Access the still capture control.
     */
    virtual CxeStillCaptureControl &stillCaptureControl() = 0;

    /**
     * Access the video capture control.
     */
    virtual CxeVideoCaptureControl &videoCaptureControl() = 0;

    /**
     * Access the auto-focus control.
     */
    virtual CxeAutoFocusControl &autoFocusControl() = 0;

    /**
     * Access the zoom control.
     */
    virtual CxeZoomControl &zoomControl() = 0;

    /**
     * Access the settings model.
     */
    virtual CxeSettings &settings() = 0;

    /**
     * Access the feature manager.
     */
    virtual CxeFeatureManager &featureManager() = 0;

    /**
     * Memory monitor utility.
     */
    virtual CxeMemoryMonitor &memoryMonitor() = 0;
    
    /**
     * Geotagging trail utility.
     */
    virtual CxeGeoTaggingTrail &geoTaggingTrail() = 0;
    
    /**
     * Get current camera mode.
     */
    virtual Cxe::CameraMode mode() const = 0;

    /**
     * Set current camera mode.
     */
    virtual void setMode(Cxe::CameraMode mode) = 0;

    /**
     * Initialize the camera in either still or video mode. This method is also
     * used to switch between modes.
     */
    virtual void initMode( Cxe::CameraMode cameraMode ) = 0;

    //! returns if the engine is ready or not
    virtual bool isEngineReady() = 0;

signals:
    void fileSaved(CxeError::Id status, const QString &filename);

protected:
    CxeEngine();

private:
    Q_DISABLE_COPY( CxeEngine )
};

#endif // CXEENGINE_H
