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

#include <QGraphicsLinearLayout>
#include <QPainter>
#include <QTimer>
#include "BatteryWidget.h"


/*!
    \ingroup group_helloworld_widget
    \class BatteryWidget
    \brief Example implementation for home screen widget. 
    
    BatteryWidget is derived from the HbWidget. It implements
    needed functions for the home screen widget.
    BatteryWidget uses <a href="http://doc.qt.nokia.com/qtmobility-1.0/qsystemdeviceinfo.html">QSystemDeviceInfo</a> 
    to get device's battery status. 
    
    Battery widget demonstrates the following home screen widget features:
    <ol>
    <li> widget shape, see \ref ssection_shape </li>
    <li> root path property, see \ref sssection_rootPath and
         widget resources, see \ref section_widget_resources </li>
    <li> error signal, see \ref sssection_error </li>
    <li> onInitialize slot, see \ref sssection_oninitialize </li>
    <li> onShow slot, see \ref sssection_onshow </li>
    <li> onHide slot, see \ref sssection_onhide </li>
    </ol>
    
*/


/*!
    \fn BatteryWidget::error()
    Signal emitted if onInitialize fails to load batterywidget image.
*/


/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
BatteryWidget::BatteryWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags)
{
    mDeviceInfo = new QSystemDeviceInfo(this);
    mChargingTimer = new QTimer(this);
    mBatteryValue = mDeviceInfo->batteryLevel();
}

/*!
    Destructor
*/
BatteryWidget::~BatteryWidget()
{
}

/*!
    Returns rootPath property.
*/
// Start of snippet 1
QString BatteryWidget::rootPath() const
{
    return mRootPath;
}
// End of snippet 1

/*!
    Sets rootPath property as \a rootPath.
*/
// Start of snippet 2
void BatteryWidget::setRootPath(QString &rootPath)
{
    mRootPath = rootPath;
}
// End of snippet 2


/*!
    Return bounding rect. 
*/
QRectF BatteryWidget::boundingRect() const
{   
    QRectF currRect = rect();
    currRect.setHeight(mBatteryBackgroundBitmap.height());
    currRect.setWidth(mBatteryBackgroundBitmap.width());
    return currRect;
}

/*!
    Return shape.
*/
// Start of snippet 3
QPainterPath BatteryWidget::shape() const
{   
    QPainterPath path;
    QRectF ownRect = boundingRect();
    path.moveTo(ownRect.bottomRight());
    QRectF ownRect2 = ownRect.adjusted(-2,-1,1, ownRect.height());
    path.arcTo(ownRect2, 0, 180);
    path.lineTo(ownRect2.right(), ownRect2.center().y());
    return path;
}
// End of snippet 3

/*!
    Paint battery image.
*/
void BatteryWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (!mUnifiedBatteryBitmap.isNull()) {
        painter->drawPixmap(QPoint(0,0),mUnifiedBatteryBitmap);
        }
}

/*!
    Called when widget is shown in the home screen.
*/
// Start of snippet 4
void BatteryWidget::onShow()
{
    stateChanged();
    prepareGeometryChange();    
    resize(mBatteryBackgroundBitmap.size());
}
// End of snippet 4

/*!
    Called when widget is hidden from the home screen.
*/
// Start of snippet 5
void BatteryWidget::onHide()
{
    mDeviceInfo->disconnect(this);
    mChargingTimer->stop();
    mChargingTimer->disconnect(this);
}
// End of snippet 5

/*!
    Called when widget is initalized. Widget can start using its resources,
    for example rootPath is set by system before it calls this.
*/
// Start of snippet 6
void BatteryWidget::onInitialize()
{
    mBatteryBackgroundBitmap.load(mRootPath + "/batterywidget_bg.png");
    if (mBatteryBackgroundBitmap.isNull()) {
        // emit error if loading of the image failed. This will remove widget from the home screen.
        emit error();
    }
}
// End of snippet 6

/*!
    Called when widget is unintialized, nothing to do here
*/
void BatteryWidget::onUninitialize()
{
}

/*!
    Called when phone's battery level is changed.
*/
void BatteryWidget::batteryLevelChanged ( int level )
{
    mBatteryValue=level;
    drawBatteryImage();
    update();
}

/*!
    Called when phone's power state is changed.
*/
void BatteryWidget::powerStateChanged(QSystemDeviceInfo::PowerState powerState)
{
    Q_UNUSED(powerState);
    stateChanged();    
}

/*!
    Animates pointer when phone is charging. (TODO: animation fw?)
*/
void BatteryWidget::updateChargingBatteryValue()
{
    mBatteryValue+=4;
    if (mBatteryValue>100) mBatteryValue = 1;
    drawBatteryImage();
    update();
}

/*!
    Draws pointer on top of battery image
*/
void BatteryWidget::drawBatteryImage()
{
    QRectF rect = mBatteryBackgroundBitmap.rect();
    
    mUnifiedBatteryBitmap = QPixmap(rect.size().toSize());
    mUnifiedBatteryBitmap.fill(QColor(Qt::transparent));
    
    QPainter painter(&mUnifiedBatteryBitmap);
    
    painter.drawPixmap(rect.toRect(), mBatteryBackgroundBitmap);
    
    static const QPoint pointerHand[3] = {
        QPoint(3, 3),
        QPoint(-3, 3),
        QPoint(0, -80)
    };

    painter.translate(QPointF(rect.width()/2,rect.height()-10));
    painter.rotate(180.0 * (mBatteryValue)/100 + 270);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawConvexPolygon(pointerHand, 3);
}

/*!
    Handles battery widget states.
*/
void BatteryWidget::stateChanged()
{
    mDeviceInfo->disconnect(this);
    mChargingTimer->stop();
    mChargingTimer->disconnect(this);
    connect(mDeviceInfo, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)), SLOT(powerStateChanged(QSystemDeviceInfo::PowerState)));
    if (mDeviceInfo->currentPowerState() == QSystemDeviceInfo::WallPowerChargingBattery) {
        connect(mChargingTimer, SIGNAL(timeout()), this, SLOT(updateChargingBatteryValue()));
        mChargingTimer->start(500);
    } else {
        connect(mDeviceInfo, SIGNAL(batteryLevelChanged(int)), SLOT(batteryLevelChanged(int)));
        mBatteryValue = mDeviceInfo->batteryLevel();
    }
    drawBatteryImage();
    update();
}

