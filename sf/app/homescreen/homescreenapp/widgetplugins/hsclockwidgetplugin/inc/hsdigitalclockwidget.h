/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Homescreen themable clock label.
*
*/

#ifndef HSDIGITALCLOCKWIDGET_H
#define HSDIGITALCLOCKWIDGET_H

#include <HbWidget>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HbFrameItem;
class HbIconItem;
class QGestureEvent;

class HsDigitalClockWidget : public HbWidget
{
    Q_OBJECT

public:
    explicit HsDigitalClockWidget(bool useAmPm = true, QGraphicsItem *parent = 0);
    ~HsDigitalClockWidget();

    QPainterPath shape() const;
signals:
    void clockTapped();

public slots:
    void tick();
    void setAmPm(bool useAmPm);

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void gestureEvent(QGestureEvent *event);

private:
    Q_DISABLE_COPY(HsDigitalClockWidget)
    void createPrimitives();
    void updatePrimitives();

private:
    HbFrameItem *mBackground;
    HbIconItem *mDigit1;
    HbIconItem *mDigit2;
    HbIconItem *mDigit3;
    HbIconItem *mDigit4;
    HbIconItem *mDigit5;
    HbIconItem *mDigit6;
    HbIconItem *mAmPm;
    bool mUseAmPm;
    QMap<QChar, QString> mDigitMap;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
};

#endif // HSDIGITALCLOCKWIDGET_H
