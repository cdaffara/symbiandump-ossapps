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
#ifndef CXUIVIEWMANAGER_H
#define CXUIVIEWMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>

#include "cxeviewfindercontrol.h"
#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxuiapplicationstate.h"

class QGraphicsSceneMouseEvent;
class HbMainWindow;
class CxuiApplication;
class CxuiCaptureKeyHandler;
class CxuiPrecaptureView;
class CxeEngine;
class CxuiDocumentLoader;
class CxuiErrorManager;
class CxuiView;

class CxuiViewManager : public QObject
{
    Q_OBJECT

public:
    CxuiViewManager(CxuiApplication &application, HbMainWindow &mainWindow, CxeEngine &engine);
    ~CxuiViewManager();

public slots:
    void changeToPostcaptureView();
    void changeToPrecaptureView();
    void switchCamera();
    void showScenesView();

private slots:
    void toForeground();
    void handleApplicationStateChanged(CxuiApplicationState::State newState,
                                       CxuiApplicationState::State oldState);
    void startStandbyTimer();
    void stopStandbyTimer();

signals:
    void normalStateEntered();
    void normalStateExited();
    void standbyExitRequested();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    CxuiView *currentView() const;
    void initStartupView();
    CxuiView *createView(const QString &viewName);
    CxuiPrecaptureView* getPrecaptureView(Cxe::CameraMode mode, Cxe::CameraIndex camera);

    void connectSignals(CxuiView *view);
    void disconnectSignals(CxuiView *view = NULL);
    void connectPreCaptureSignals(CxuiPrecaptureView *view);
    void connectPostCaptureSignals();
    void connectSceneModeSignals();
    void connectCaptureKeySignals(CxuiView *view);

    void handleExitingNormalState();
    void clearAllActivities();

private:

    //data
    CxuiApplication &mApplication;
    HbMainWindow &mMainWindow;
    QMap<QString, CxuiView*> mViews;
    QMap<QString, QString> mDocmlFilesByView;

    CxeEngine &mEngine;
    CxuiCaptureKeyHandler *mKeyHandler;
    CxuiDocumentLoader *mCameraDocumentLoader;
    CxuiApplicationState *mApplicationState;
    CxuiErrorManager *mErrorManager;
    QTimer mStandbyTimer;
};

#endif // CXUIVIEWMANAGER_H
