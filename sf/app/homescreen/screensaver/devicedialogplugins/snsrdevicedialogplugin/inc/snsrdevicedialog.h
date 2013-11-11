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
* Description: Default Screensaver runtime.
*
*/

#ifndef SNSRDEVICEDIALOG_H
#define SNSRDEVICEDIALOG_H

#include <HbPopup>
#include <HbDeviceDialogInterface>



#include "screensaver.h"
#include "snsrtest_global.h"

class Screensaver;
class QGraphicsLinearLayout;
class HbIndicatorInterface;
class SnsrDeviceDialogDisplayControl;
class QTimer;

SCREENSAVER_TEST_CLASS(T_SnsrDeviceDialogPlugin)


class SnsrDeviceDialog: public HbPopup, public HbDeviceDialogInterface
{

Q_OBJECT

public: // constants
    enum DisplayType {
        DisplayTypeRegular = 0,
        DisplayTypeOled,
        DisplayTypeRegularAndOled
    };
    
    enum ViewType {
        ViewTypeInitial = 0, // default initial state, depends on the configuration
        ViewTypeActive,
        ViewTypeStandby
    };
    
    static const char *dataKeyUnlock;

public: // methods

    SnsrDeviceDialog(const QVariantMap &parameters, QGraphicsItem *parent = 0);
    ~SnsrDeviceDialog();

    virtual bool setDeviceDialogParameters(const QVariantMap &parameters);
    virtual int deviceDialogError() const;
    virtual void closeDeviceDialog(bool byClient);
    virtual HbPopup *deviceDialogWidget() const;

signals:

    void deviceDialogClosed();
    void deviceDialogData(QVariantMap);

private slots:

    void changeView(QGraphicsWidget *widget);
    void screensaverFaulted();
    void handleOrientationChange(Qt::Orientation);
    void changeLayout();
    void updateDisplayMode();
    void updateDisplayModeDeferred();
    void requestUnlock();
    // slots for signals emitted by HbIndicatorPluginManager
    void indicatorsActivated(const QList<HbIndicatorInterface*> &activatedIndicators);
    void indicatorActivated(HbIndicatorInterface *activatedIndicator);
    void indicatorDeactivated(HbIndicatorInterface *deactivatedIndicator);

private:

    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void closeEvent(QCloseEvent *event);
    bool event(QEvent *event);

private:

    Screensaver *mScreensaver;
    QGraphicsLinearLayout *mLayout;
    SnsrDeviceDialogDisplayControl *mDisplayControl;
    QTimer *mDisplayModeTimer;
    bool mHasFocus;
    
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrDeviceDialogPlugin)
};

#endif // SNSRDEVICEDIALOG_H
