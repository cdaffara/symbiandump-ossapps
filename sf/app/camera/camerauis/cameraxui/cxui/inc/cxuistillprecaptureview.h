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
#ifndef CXUISTILLPRECAPTUREVIEW_H
#define CXUISTILLPRECAPTUREVIEW_H

#include <QMetaType>

#include "cxuiprecaptureview.h"
#include "cxeautofocuscontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxeerror.h"
#include "cxenamespace.h"
class HbListWidgetItem;
class HbListWidget;
class CxuiCaptureKeyHandler;
class CxuiDocumentLoader;
class HbAction;
class HbDialog;
class CxuiSelfTimer;
class HbPushButton;
class HbLabel;
class CxuiSelfTimerRadioButtonList;
class HbToolBarExtension;
class HbWidget;

/**
 * Pre-capture view for still image mode
 */
class CxuiStillPrecaptureView : public CxuiPrecaptureView
{
    Q_OBJECT

public:
    CxuiStillPrecaptureView(QGraphicsItem *parent = 0);

    virtual ~CxuiStillPrecaptureView();

public:

    virtual void construct(HbMainWindow *mainwindow, 
                           CxeEngine *engine,
                           CxuiDocumentLoader *documentLoader, 
                           CxuiCaptureKeyHandler *keyHandler);

    virtual void loadDefaultWidgets();
    virtual void loadWidgets();

    void restoreActivity(const QString &activityId, const QVariant &data);
    void saveActivity();
    void clearActivity();

protected:

    void showEvent(QShowEvent *event);
    virtual bool allowShowControls() const;

public slots:

    // from CxuiPrecaptureView
    virtual void handleSettingValueChanged(const QString& key, QVariant newValue);

    void handleSceneChanged(const QVariant &newSceneData);

    // From CxuiPrecaptureView
    virtual void enterStandby();

protected slots:
    void focusAndCapture();
    void capture();
    void setCapturePending();
    void goToVideo();

    // Key events
    void handleCaptureKeyPressed();
    void handleAutofocusKeyPressed();
    void handleAutofocusKeyReleased();

    // Engine signals
    void handleAutoFocusStateChanged(CxeAutoFocusControl::State newState, CxeError::Id error);
    void handleVfStateChanged(CxeViewfinderControl::State newState, CxeError::Id error);
    void handleStillCaptureStateChanged(CxeStillCaptureControl::State newState, CxeError::Id error);
    void handleSnapshot(CxeError::Id error);

    void resetCapturePendingFlag();

    void launchSetting();
    void updateImagesLeftLabel();

protected:
    void initializeSettingsGrid();
    bool isPostcaptureOn() const;
    void updateSceneIcon(const QString& sceneId);
    void updateQualityIcon();
    void closeDialogs();
    void updateFaceTrackingIcon();
    void updateGeotaggingIcon();

private:
    void reloadIndicatorWidgets();

protected:
    CxuiSelfTimer *mSelfTimer;
    HbAction *mFlashSetting;
    HbDialog *mStillSettingsPopup;
    HbDialog *mSceneModePopup;

    /**
     * Capture key was pressed while engine was not ready... Capture when possible.
     * This flag is reset after a short delay at resetCapturePendingFlag().
     */
    bool mCapturePending;
    HbLabel *mImagesLeft;
    HbWidget *mImagesLeftContainer;

private:
    bool mPendingAfCanceling;
};

Q_DECLARE_METATYPE(QList<int>)

#endif // CXUISTILLPRECAPTUREVIEW_H
