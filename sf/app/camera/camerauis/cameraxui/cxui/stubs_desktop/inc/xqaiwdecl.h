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
*   Decls used by CxuiPostCaptureView
*/


#ifndef XQAIWDECL_H
#define XQAIWDECL_H

#include <QLatin1String>

/*!
 Video player interface and related operations
*/
#define XQI_VIDEO_PLAY QLatin1String("com.nokia.symbian.IVideoView")
#define XQOP_VIDEO_PLAY QLatin1String("playMedia(QString)")
#define XQOP_VIDEO_PLAY_PD QLatin1String("playMedia(QString, int)")
#define XQOP_VIDEO_CLOSE QLatin1String("closePlayer()")

/*!
 Camera capture interface and related operations
 Operation: capture(int mode, QVariantMap parameters)
  mode: image = 0, video = 1
  parameters: 
   CameraIndex: int:  primary = 0, secondary = 1
   Quality: int: 0 = default, 1 = lowest, 2 = highest
   AllowModeSwitch: bool
   AllowCamera_switch: bool
   allow_quality_change: bool
*/
#define XQI_CAMERA_CAPTURE QLatin1String("com.nokia.symbian.ICameraCapture")
#define XQOP_CAMERA_CAPTURE QLatin1String("capture(int,QVariantMap)")
#define XQCAMERA_INDEX QLatin1String("CameraIndex")
#define XQCAMERA_QUALITY QLatin1String("Quality")
#define XQCAMERA_MODE_SWITCH QLatin1String("AllowModeSwitch")
#define XQCAMERA_INDEX_SWITCH QLatin1String("AllowCameraSwitch")
#define XQCAMERA_QUALITY_CHANGE QLatin1String("AllowQualityChange")

/*!
 A key name for XQRequestInfo object to pass a window title
 to be shown in service application instead of the default title.
 Service application is responsible to handle the key value.
 \see XQApplicationMgr::XQRequestInfo::setInfo()
*/
#define XQINFO_KEY_WINDOW_TITLE QLatin1String("WindowTitle")


#endif
