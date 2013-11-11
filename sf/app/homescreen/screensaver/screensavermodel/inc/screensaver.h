/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all screensavers.
*
*/

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QGraphicsWidget>

#include "screensavermodel_global.h"

class ScreensaverPrivate;
class HbIndicatorInterface;

enum ScreensaverState
{
    ScreensaverStateConstructed = 0,
    ScreensaverStateInitialized,
    ScreensaverStateBackground,
    ScreensaverStateForeground,
    ScreensaverStatePartialForeground,
    ScreensaverStatePowerSave,
    ScreensaverStateClosed
};

class SREENSAVERMODEL_EXPORT Screensaver : public QObject
{
    Q_OBJECT

public:

    Q_ENUMS(ScreenPowerMode)
    
    enum ScreenPowerMode {
        ScreenModeOff = 0,
        ScreenModeLowPower,
        ScreenModeFullPower
    };
    
    Screensaver(QObject *parent = 0);
    virtual ~Screensaver();

    ScreensaverState currentState();
    virtual Screensaver::ScreenPowerMode currentPowerMode() = 0;
    virtual void getActiveScreenRows(int *firstActiveRow, int *lastActiveRow) = 0;
    virtual void updateLayout() = 0;

public slots:

    void initialize();
    void foreground();
    void partialForeground();
    void background();
    void powerSave();
    void close();
    
    void handleActiveIndicators(const QList<HbIndicatorInterface*> &activeIndicators);
    // TODO: change to const reference or something or leave it?
    void handleActivatedIndicator(HbIndicatorInterface *activatedIndicator);
    void handleDeactivatedIndicator(HbIndicatorInterface *deactivatedIndicator);

protected:

    virtual bool onInitialize();
    virtual bool onForeground() = 0;
    virtual bool onPartialForeground() = 0;
    virtual bool onBackground() = 0;
    virtual bool onPowerSave() = 0;
    virtual bool onClose();
    
    virtual void onHandleActiveIndicators(const QList<HbIndicatorInterface*> &activeIndicators);
    // TODO: change to const reference or something or leave it?
    virtual void onHandleActivatedIndicator(HbIndicatorInterface *activatedIndicator);
    virtual void onHandleDeactivatedIndicator(HbIndicatorInterface *deactivatedIndicator);

signals:

    void faulted();
    void viewChanged(QGraphicsWidget *widget);
    void unlockRequested();
    void activeAreaMoved();

private:

    Q_DISABLE_COPY(Screensaver)

private:

    ScreensaverPrivate * const m_d;
    friend class ScreensaverPrivate;

};

#endif // SCREENSAVER_H
