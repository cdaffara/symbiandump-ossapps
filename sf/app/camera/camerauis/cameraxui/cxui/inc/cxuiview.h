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
* Description: This is a header file for a CxuiView class
*
*/

#ifndef CXUIVIEW_H
#define CXUIVIEW_H

#include <QObject>
#include <QTimer>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hbframedrawer.h>
#include <hbinstantfeedback.h>

class CxuiDocumentLoader;
class CxuiCaptureKeyHandler;
class CxuiZoomSlider;
class CxuiCaptureKeyHandler;
class CxeEngine;
class HbWidget;

// CONSTANTS
const int CXUI_HIDE_CONTROLS_TIMEOUT     = 6000; // 6 seconds

/**
 * A base class for all views in the Camera UI
 * The class is for deriving only
 */
class CxuiView : public HbView
{
    Q_OBJECT

public:
    CxuiView(QGraphicsItem *parent = 0);
    virtual ~CxuiView();

    virtual void construct(HbMainWindow *mainWindow, CxeEngine *engine,
                           CxuiDocumentLoader *documentLoader,
                           CxuiCaptureKeyHandler * keyHandler);

public:
    virtual bool isStandbyModeSupported() const;
    virtual void updateOrientation(Qt::Orientation orientation);

    virtual void restoreActivity(const QString &activityId, const QVariant &data);
    virtual void saveActivity();
    virtual void clearActivity();

protected:
    virtual bool allowShowControls() const;
    virtual bool isFeedbackEnabled() const;
    virtual void toggleControls();
    virtual void showToolbar();
    virtual void hideZoom();
    virtual void showZoom();
    virtual void hideIndicators();
    virtual void showIndicators();
    virtual void createWidgetBackgroundGraphic(HbWidget *widget,
                                       const QString &graphicName,
                                       HbFrameDrawer::FrameType frameType =
                                       HbFrameDrawer::NinePieces);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:
    void zoomDisappearEffectFinished(const HbEffect::EffectStatus &status);
    void zoomAppearEffectFinished(const HbEffect::EffectStatus &status);
    virtual void enterStandby();
    virtual void exitStandby();

protected slots:
    virtual void launchNotSupportedNotification();
    virtual void launchPhotosApp();
    virtual void launchVideosApp();
    virtual void releaseCamera();

    virtual void launchScenesView();

    // Control visibility of all UI items at the same time: toolbar, zoom and titlepane items
    virtual void hideControls();
    virtual void showControls();

    virtual void hideToolbar();


signals:
    void viewCloseEvent();
    void showScenesView();

protected: //common data
    HbMainWindow *mMainWindow; // not own
    CxeEngine *mEngine; // not own
    CxuiDocumentLoader *mDocumentLoader; // not own
    CxuiCaptureKeyHandler* mCaptureKeyHandler;

    CxuiZoomSlider *mSlider; // zoom slider, not own, owned by the graphics scene
    HbToolBar *mToolbar; // not own, owned by the graphics scene
    HbWidget *mIndicators;

    bool   mControlsVisible;
    bool   mZoomVisible;
    QTimer mHideControlsTimeout;

    HbInstantFeedback mControlsFeedback;
};

#endif // CXUIVIEW_H
