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
* Description:  Day view control of calendar
*
*/

#ifndef CALENDAYEVENTSPANE_H_
#define CALENDAYEVENTSPANE_H_

// System includes
#include <QPen>
#include <HbWidget>

class CalenDayEventsPane : public HbWidget
{
    Q_OBJECT
	
public:
    CalenDayEventsPane(HbWidget *parent=0);
    virtual ~CalenDayEventsPane();
    
    void setDrawTopLine(bool drawTopLine = false);
    bool isTopLineDrawed() const;
    
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, 
        QWidget * widget);
    
private:
    QColor mHourLineColor;
    qreal mUnitInPixels;
    QPen mCustomDashedPen;
    bool mDrawTopLine;      //!< Flag indicates if top line should be drawn
};

#endif /* CALENDAYEVENTSPANE_H_ */
