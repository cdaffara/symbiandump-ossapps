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

#ifndef PERFMONDATAPOPUPDIALOG_P_H
#define PERFMONDATAPOPUPDIALOG_P_H

#include <QObject>
#include <QVariantMap>

#include <hbdevicedialoginterface.h>
#include <hbdevicedialog.h>
#include <hbnotificationdialog.h>

class PerfMonDataPopupWidget;

class PerfMonDataPopupDialog : public HbDialog, public HbDeviceDialogInterface
{
    Q_OBJECT
    Q_PROPERTY(Location location READ location WRITE setLocation)
    Q_PROPERTY(QStringList lines READ lines WRITE setLines)

    Q_ENUMS(Location)

public:
    enum Location
    {
        LocationUndefined = -1,
        LocationTopRight = 0,
        LocationBottomMiddle
    };

public:
    PerfMonDataPopupDialog(const QVariantMap &parameters);
    virtual ~PerfMonDataPopupDialog();

    bool setDeviceDialogParameters(const QVariantMap &parameters);
    int deviceDialogError() const;
    void closeDeviceDialog(bool byClient);
    HbPopup *deviceDialogWidget() const;

protected:
//    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
//               QWidget *widget=0);

protected:
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void deviceDialogClosed();
    void deviceDialogData(QVariantMap data);

private:
    Location location() const;
    void setLocation(Location location);

    QStringList lines() const;
    void setLines(const QStringList &lines);

private:
    Q_DISABLE_COPY(PerfMonDataPopupDialog)

    int mLastError;
    bool mShowEventReceived;

    Location mLocation;

    PerfMonDataPopupWidget *mWidget;
};

#endif // PERFMONDATAPOPUPDIALOG_P_H
