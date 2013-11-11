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
#ifndef CXESETTINGSCONTROLSYMBIAN_H
#define CXESETTINGSCONTROLSYMBIAN_H

#include <QObject>
#include "cxenamespace.h"

class CxeCameraDevice;
class CxeSettings;

/*!
* Class to handle setting value changes.
* Changed settings are updated to camera device.
*/
class CxeSettingsControlSymbian : public QObject
{
    Q_OBJECT

public:

    CxeSettingsControlSymbian(CxeCameraDevice &cameraDevice, CxeSettings &settings);
    ~CxeSettingsControlSymbian();

public slots:

    void handleSettingValueChanged(const QString& settingId, QVariant newValue);
    void handleSceneChanged(const QVariant &scene);

private:

    void updateColorToneSetting(QVariant newValue);
    void updateWhiteBalanceSetting(QVariant newValue);
    void updateLightSensitivitySetting(QVariant newValue);
    void updateSharpnessSetting(QVariant newValue);
    void updateContrastSetting(QVariant newValue);
    void updateBrightnessSetting(QVariant newValue);
    void updateExposureModeSetting(QVariant newValue);
    void updateExposureCompensationSetting(QVariant newValue);
    void updateFlashSetting(QVariant newValue);
    void updateFaceTrackingSetting(QVariant newValue);

private:
    CxeCameraDevice &mCameraDevice;
    CxeSettings &mSettings;
};

#endif // CXESETTINGSCONTROLSYMBIAN_H
