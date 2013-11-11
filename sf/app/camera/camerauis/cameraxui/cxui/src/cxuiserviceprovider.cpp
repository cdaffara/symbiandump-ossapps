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

#include <QApplication>

#include "cxutils.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"

#include "cxuiserviceprovider.h"
#include "cxesettings.h"

#include <xqaiwdecl.h>

CxuiServiceProvider* CxuiServiceProvider::mInstance = NULL;

/*!
 * Constructor.
 */
CxuiServiceProvider::CxuiServiceProvider(CxeEngine *engine)
:
    XQServiceProvider("cxui." + XQI_CAMERA_CAPTURE),
    mRequestIndex(-1),
    mEngine(engine),
    mRequestedMode(Cxe::ImageMode),
    mCameraIndex(0),
    mQuality(0),
    mAllowModeSwitching(true),
    mAllowQualityChange(true),
    mAllowCameraSwitching(true),
    mWindowTitle("")
{
    CX_DEBUG_ENTER_FUNCTION();
    publishAll();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Destructor.
 */
CxuiServiceProvider::~CxuiServiceProvider()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mRequestIndex != -1) {
        CX_DEBUG(("Request still active..."));
        // Complete request now, this will return error to client
        completeRequest(mRequestIndex, QString());
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Creates singleton instance of service provider.
 */
void CxuiServiceProvider::create(CxeEngine* engine)
{
    if (mInstance == NULL) {
        mInstance = new CxuiServiceProvider(engine);
    }
}

/*!
 * Returns singleton instance of service provider.
 *
 */
CxuiServiceProvider* CxuiServiceProvider::instance()
{
    return mInstance;
}

/*!
 * Deletes singleton instance of service provider.
 */
void CxuiServiceProvider::destroy()
{
    delete mInstance;
    mInstance = NULL;
}

/*!
 * Returns true if camera is started by another application.
 *
 * "Embedded" in camera means that the camera is started as a service
 * by another application using QtHighway. It doesn't necessarily
 * mean that camera is actually embedded (=window groups chained). The
 * distinction between embedded and non-embedded service is not relevant on
 * camera side, it's more meaningful to the client app.
 *
 */
bool CxuiServiceProvider::isCameraEmbedded()
{
    return mInstance != NULL;
}

/*!
 * Returns camera mode requested by client.
 */
Cxe::CameraMode CxuiServiceProvider::requestedMode() const
{
    return mRequestedMode;
}

/*!
 * Returns boolean to indicate whether client app allows camera mode switching.
 */
bool CxuiServiceProvider::allowModeSwitching() const
{
    return mAllowModeSwitching;
}

/*!
 * Returns boolean to indicate whether client app allows image/video quality to be
 * changed.
 */
bool CxuiServiceProvider::allowQualityChange() const
{
    return mAllowQualityChange;
}

/*!
 * Returns boolean to indicate whether client app allows switching
 * between primary and secondary camera.
 */
bool CxuiServiceProvider::allowCameraSwitching() const
{
    return mAllowCameraSwitching;
}

/*!
 * Returns window title provided by client application.
 */
QString CxuiServiceProvider::windowTitle() const
{
    return mWindowTitle;
}

/*!
 * Function that is called when request arrives. Sets up camera to requested state.
 */
void CxuiServiceProvider::capture(int mode, const QVariantMap &parameters)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mRequestIndex != -1) {
        // only one request can be active at once
        CX_DEBUG(("Request already active, ignoring"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // read options from map
    if (!readParameters(parameters)) {
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    if (mCameraIndex < 0 || mCameraIndex > 1) {
        CX_DEBUG(("Invalid camera index"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    CX_DEBUG(("Getting request info"));
    XQRequestInfo info = requestInfo();

    QString windowTitle = info.info(XQINFO_KEY_WINDOW_TITLE).toString();
    if (!windowTitle.isNull() && !windowTitle.isEmpty()) {
        CX_DEBUG(("Got window title=%s", qPrintable(windowTitle)));
        mWindowTitle = windowTitle;
    }
    mRequestIndex = setCurrentRequestAsync();

    mEngine->cameraDeviceControl().switchCamera(static_cast<Cxe::CameraIndex> (mCameraIndex));


    if (mode == 0) {
        CX_DEBUG(("Image capture requested"));
        mRequestedMode = Cxe::ImageMode;
    }
    else {
        CX_DEBUG(("Video capture requested"));
        mRequestedMode = Cxe::VideoMode;
    }

    if (mQuality == 1) {
        // set image quality to lowest
        int imageQualityIndex = mEngine->stillCaptureControl().supportedImageQualities().size() - 1;
        CX_DEBUG(("Setting image quality to lowest, index=%d", imageQualityIndex));
        mEngine->settings().set(CxeSettingIds::IMAGE_QUALITY, imageQualityIndex);

        // set video quality to lowest
        int videoQualityIndex = mEngine->videoCaptureControl().supportedVideoQualities().size() - 1;
        CX_DEBUG(("Setting video quality to lowest, index=%d", videoQualityIndex));
        mEngine->settings().set(CxeSettingIds::VIDEO_QUALITY, videoQualityIndex);
    } else if (mQuality == 2) {
        // set image quality to highest
        CX_DEBUG(("Setting image quality to highest, index=0"));
        mEngine->settings().set(CxeSettingIds::IMAGE_QUALITY, 0);

        // set video quality to highest
        CX_DEBUG(("Setting video quality to highest, index=0"));
        mEngine->settings().set(CxeSettingIds::VIDEO_QUALITY, 0);
    }

    mEngine->initMode(mRequestedMode);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Sends filename of captured image/video to client and exits camera app.
 */
void CxuiServiceProvider::sendFilenameToClientAndExit(QString filename)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mRequestIndex == -1) {
        CX_DEBUG(("CxuiServiceProvider: no request in progress"));
        QCoreApplication::instance()->quit();
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    connect(this, SIGNAL(returnValueDelivered()), QCoreApplication::instance(), SLOT(quit()));

    CX_DEBUG(("CxuiServiceProvider: completing request"));
    if (!completeRequest(mRequestIndex, QVariant(filename))) {
        // if request completion fails call quit immediately because signal is not coming
        CX_DEBUG(("completeRequest() failed, quitting now"));
        QCoreApplication::instance()->quit();
    }
    mRequestIndex = -1;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Helper function to read request parameters.
 */
bool CxuiServiceProvider::readParameters(const QVariantMap& parameters)
{

    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("Reading parameters"));
    bool ok;
    mCameraIndex = parameters[XQCAMERA_INDEX].toInt(&ok);
    if (!ok) {
        CX_DEBUG(("Error reading parameter %s", XQCAMERA_INDEX.latin1()));
        CX_DEBUG_EXIT_FUNCTION();
        return false;
    }
    mQuality = parameters[XQCAMERA_QUALITY].toInt(&ok);
    if (!ok) {
        CX_DEBUG(("Error reading parameter %s", XQCAMERA_QUALITY.latin1()));
        CX_DEBUG_EXIT_FUNCTION();
        return false;
    }

    // ignore possible errors on these parameters. default values will be false
    mAllowModeSwitching = parameters[XQCAMERA_MODE_SWITCH].toBool();
    mAllowQualityChange = parameters[XQCAMERA_QUALITY_CHANGE].toBool();
    mAllowCameraSwitching = parameters[XQCAMERA_INDEX_SWITCH].toBool();

    CX_DEBUG_EXIT_FUNCTION();
    return true;
}

