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
* Description:  Analog Clock Widget.
*
*/

#ifndef SNSRANALOGCLOCKWIDGET_H
#define SNSRANALOGCLOCKWIDGET_H

#include <hbwidget.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrClockWidgets)

class HbIconItem;
class HbTextItem;

class SnsrAnalogClockWidget: public HbWidget
{
    Q_OBJECT

public:

    explicit SnsrAnalogClockWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrAnalogClockWidget();

public slots:

    void tick();
    
protected:

    virtual void changeEvent(QEvent *event);

private:

    void resizeEvent (QGraphicsSceneResizeEvent *event);
    void createPrimitives();
    void updatePrimitives();
    void polish( HbStyleParameters& params );

private:

    HbIconItem *mClockBackground;
    HbIconItem *mClockHourHand;
    HbIconItem *mClockMinuteHand;
    HbIconItem *mClockSecondHand;
    HbTextItem *mClockAmPmLabel;

    Q_DISABLE_COPY(SnsrAnalogClockWidget)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrClockWidgets)

};

#endif // SNSRANALOGCLOCKWIDGET_H
