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

#ifndef HSWALLPAPERSELECTIONSTATE_H
#define HSWALLPAPERSELECTIONSTATE_H

#include <QState>

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HsWallpaper;
class HsSpinnerDialog;

#ifdef Q_OS_SYMBIAN
class HsImageHandlingClient;
#else
class XQAIWGetImageClient;
#endif

class HsWallpaperSelectionState : public QState
{
    Q_OBJECT

public:
    HsWallpaperSelectionState(QState *parent = 0);
    ~HsWallpaperSelectionState();

signals:
    void event_assignImage();
    void event_error();
    void event_waitInput();
    
private:
    Q_DISABLE_COPY(HsWallpaperSelectionState)        
    void setupStates();

private slots:    
    void action_selectingImage_start();
    void action_imageHandler_cleanup();
    void action_editingImage_start();
    void action_assigningImage_showWaitDialog();
    void action_assigningImage_start();
    void action_assigningImage_cleanup();
    void action_assigningImage_hideWaitDialog();
    void action_errorMessage_show();

    void onFetchCompleted(const QString &imagePath);
    void onFetchFailed(int errorCode, const QString &errorMessage);
    void onImageSet();
    void onImageSetFailed();
    void onEditorCompleted();
private:    
#ifdef Q_OS_SYMBIAN
    HsImageHandlingClient *mImageHandler;
#else
    XQAIWGetImageClient *mImageHandler;
#endif
    QString mImagePath;
    HsWallpaper *mWallpaper;    
    HsSpinnerDialog *mWaitDialog;
    
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSWALLPAPERSELECTIONSTATE_H
