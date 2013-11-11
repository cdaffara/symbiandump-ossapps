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
* Description: Oled Clock Widget.
*
*/

#ifndef SNSROLEDCLOCKWIDGET_H
#define SNSROLEDCLOCKWIDGET_H

#include <hbwidget.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrClockWidgets)

class HbIconItem;
class HbTextItem;


class SnsrOledClockWidget: public HbWidget
{
    Q_OBJECT

public:

    explicit SnsrOledClockWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrOledClockWidget();

public slots:

    void tick();

private:

    void resizeEvent (QGraphicsSceneResizeEvent *event);
    void createPrimitives();
    void updatePrimitives();
    void polish( HbStyleParameters& params );

private:

    HbIconItem *mClockBackground;
    HbIconItem *mClockHourHand;
    HbIconItem *mClockMinuteHand;
    HbTextItem *mClockAmPmLabel;
    
    Q_DISABLE_COPY(SnsrOledClockWidget)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrClockWidgets)

};

#endif // SNSROLEDCLOCKWIDGET_H
