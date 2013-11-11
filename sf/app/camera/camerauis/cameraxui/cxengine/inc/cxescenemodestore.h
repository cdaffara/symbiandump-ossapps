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
#ifndef CXESCENEMODESTORE_H
#define CXESCENEMODESTORE_H

#include <QObject>

#include "cxenamespace.h"
#include "cxeerror.h"

class CxeSceneModeStore
{

public:

    CxeSceneModeStore();
    ~CxeSceneModeStore();

    CxeError::Id sceneSettingValue(Cxe::CameraMode cameraMode, const QString &key, QVariant &value) const;
    CxeError::Id setSceneSettingValue(Cxe::CameraMode cameraMode, const QString &key, const QVariant &newValue);

    QString currentSceneId(Cxe::CameraMode cameraMode) const;
    CxeScene& currentScene(Cxe::CameraMode cameraMode);
    const CxeScene& currentScene(Cxe::CameraMode cameraMode) const;

    void setCurrentScene(Cxe::CameraMode cameraMode, const QString &sceneId);
    CxeScene getScene(Cxe::CameraMode cameraMode, const QString &sceneId) const;
private:

    void loadImageScenes();
    void loadVideoScenes();


private:

    QHash<QString, CxeScene> mImageSceneModes;
    QHash<QString, CxeScene> mVideoSceneModes;

    CxeScene mCurrentImageScene;
    CxeScene mCurrentVideoScene;
};



#endif /* CXESCENEMODESTORE_H */
