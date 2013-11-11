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
#ifndef CXUISELFTIMER_H
#define CXUISELFTIMER_H

#include <QObject>
#include <QTimer>
#include <QSound>
#include <QVariant>

class CxeSettings;
class CxuiDocumentLoader;
class HbLabel;
class HbPushButton;
class HbWidget;

class CxuiSelfTimer : public QObject
{
    Q_OBJECT

public:
    explicit CxuiSelfTimer(CxeSettings &mSettings);
    ~CxuiSelfTimer();

    void loadSelftimerWidgets(CxuiDocumentLoader *documentLoader);
    bool isEnabled();
    bool isOngoing();

    int getTimeout() const;
signals:
    void timerFinished();
    void cancelled();

public slots:
    void changeTimeOut(int seconds);
    void startTimer();
    void reset(bool update = true);
    void cancel();
    void enableSound(long int uid, unsigned long int key, QVariant value);

protected slots:
    void timeout();

private:
    void updateWidgets();
    void showWidgets();
    void hideWidgets();
    void playSound();

private:
    Q_DISABLE_COPY(CxuiSelfTimer)

    int mDelay;
    int mCounter;
    QTimer mTimer;
    int mOldPostCaptureTimeOut;

    // UI widgets, not own
    HbWidget *mWidgetContainer;
    HbLabel *mTimerLabel;
    HbPushButton *mCancelButton;
    HbPushButton *mStartButton;

    // settings, not own
    CxeSettings &mSettings;

    QSound mSound;
    bool mUseSound;
};

#endif // CXUISELFTIMER_H
