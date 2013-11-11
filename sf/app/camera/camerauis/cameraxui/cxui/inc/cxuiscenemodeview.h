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
#ifndef CXUISCENEMODEVIEW_H
#define CXUISCENEMODEVIEW_H

#include <QMetaType>
#include <QPropertyAnimation>
#include <QTimer>
#include <hbframedrawer.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <hbpushbutton.h>

#include "cxeerror.h"
#include "cxenamespace.h"
#include "cxuisettingsinfo.h"
#include "cxuienums.h"
#include "cxeengine.h"
#include "cxuiview.h"

class CxuiCaptureKeyHandler;
class CxuiDocumentLoader;
class HbPushButton;
class HbLabel;
class HbView;
class CxuiSettingsInfo;


/**
 * Scene mode view class for both video and still sides
 */


class CxuiSceneModeView : public CxuiView
{
    Q_OBJECT

public:
    CxuiSceneModeView(QGraphicsItem *parent = 0);

    virtual ~CxuiSceneModeView();

public:

    void construct(HbMainWindow *mainwindow,
                   CxeEngine *engine,
                   CxuiDocumentLoader *documentLoader,
                   CxuiCaptureKeyHandler *keyHandler);

    void loadDefaultWidgets();
    void loadBackgroundImages();
    void saveActivity();
signals:
    void viewCloseEvent();

public slots:
    void handleCaptureKeyPressed();
    void handleAutofocusKeyPressed();

private slots:
    void handleSceneRadiobuttonPress(int index);
    void finishScenesTransition();
    void handleOkButtonPress();
    void handleCancelButtonPress();

protected:
    void showEvent(QShowEvent *event);
    bool allowShowControls() const;
    bool isFeedbackEnabled() const;

private:
    QString backgroundForScene(const QString& sceneId);
    void startBackgroundTransition();

    void connectSignals();
    void closeView();

private:
    CxuiSettingsInfo *mSettingsInfo;

    HbLabel* mScenesBackground;
    HbLabel* mScenesBackground2;
    CxuiSettingRadioButtonList* mScenesList;
    QList<CxUiSettings::SettingItem> mSettingPairList;
    HbPushButton* mScenesOkButton;
    HbPushButton* mScenesCancelButton;
    HbWidget* mScenesContainer;
    HbLabel *mScenesHeading;

    QTimer mCameraReleaseTimer;

    QPropertyAnimation* mTransitionAnimation; //for mScenesBackground

};

#endif // CXUISCENEMODEVIEW_H
