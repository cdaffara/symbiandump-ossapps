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
#ifndef CXUISERVICEPROVIDER_H_
#define CXUISERVICEPROVIDER_H_


#include <xqserviceprovider.h>
#include "cxeerror.h"
#include "cxenamespace.h"


class CxeEngine;

class CxuiServiceProvider : public XQServiceProvider
{

Q_OBJECT

public:

    static void create(CxeEngine* engine);
    static CxuiServiceProvider* instance();
    static void destroy();
    static bool isCameraEmbedded();

    Cxe::CameraMode requestedMode() const;

    bool allowModeSwitching() const;
    bool allowQualityChange() const;
    bool allowCameraSwitching() const;

    QString windowTitle() const;

public slots:

    /*
     * Mode: image = 0, video = 1
     * Parameters:
     *
     * camera_index: int:  primary = 0, secondary = 1
     * quality: int: 0 = default, 1 = lowest, 2 = highest
     * allow_mode_switch: bool
     * allow_camera_switch: bool
     * allow_quality_change: bool
     */
    void capture(int mode, const QVariantMap &parameters);

    void sendFilenameToClientAndExit(QString filename = "");

private:

    CxuiServiceProvider(CxeEngine *engine);
    ~CxuiServiceProvider();

    bool readParameters(const QVariantMap& parameters);

    static CxuiServiceProvider *mInstance;
    int mRequestIndex;
    CxeEngine *mEngine;
    Cxe::CameraMode mRequestedMode;
    int mCameraIndex;
    int mQuality;
    bool mAllowModeSwitching;
    bool mAllowQualityChange;
    bool mAllowCameraSwitching;
    QString mWindowTitle;

};

#endif /* CXUISERVICEPROVIDER_H_ */
