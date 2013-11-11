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
* Description: Time label for oled digital clock.
*
*/

#ifndef SNSROLEDTIMELABEL_H
#define SNSROLEDTIMELABEL_H

#include <HbTextItem>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrClockWidgets)

class SnsrOledTimeLabel : public HbTextItem
{
    Q_OBJECT

    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    
public:
    
    SnsrOledTimeLabel(QGraphicsItem *parent = 0);
    ~SnsrOledTimeLabel();
    
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
        
    Q_DISABLE_COPY(SnsrOledTimeLabel)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrClockWidgets)
};
    
#endif //  SNSROLEDTIMELABEL_H
