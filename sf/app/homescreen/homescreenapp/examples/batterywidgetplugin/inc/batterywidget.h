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
* Description:  Example of home screen widget
*
*/

#ifndef BatteryWidget_H
#define BatteryWidget_H

#include <HbWidget>

#include <qsysteminfo.h>
QTM_USE_NAMESPACE


class BatteryWidget : public HbWidget
{
    Q_OBJECT
// Start of snippet 1
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
// End of snippet 1

public:
    BatteryWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~BatteryWidget();
    
// Start of snippet 2
public:
    QString rootPath() const;
    void setRootPath(QString &rootPath);
// End of snippet 2

public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    
public slots: // from home screen widget model
    void onShow();
    void onHide();
    void onInitialize();
    void onUninitialize();

public slots:
    void batteryLevelChanged(int level);
    void powerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void updateChargingBatteryValue();

signals:
    void error();

private:
    void drawBatteryImage();
    void stateChanged();
    
private:
    Q_DISABLE_COPY(BatteryWidget)
    QString mRootPath;
    QSystemDeviceInfo *mDeviceInfo;
    int mBatteryValue;
    QPixmap mBatteryBackgroundBitmap;
    QPixmap mUnifiedBatteryBitmap;
    QTimer *mChargingTimer;
};

#endif // BatteryWidget_H
