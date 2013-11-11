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


#ifndef CALENDAYSTATUSSTRIP_H
#define CALENDAYSTATUSSTRIP_H

#include <QTime>
#include <HbWidget>

class CalenDayStatusStrip : public HbWidget
{
    Q_OBJECT
    Q_ENUMS(DrawingStyle)
public:
    /*!
      \brief It specifiy how status Strip should be drawed.
      
      OnlyFrame -- draw not filled rectangle
      StripWithLines -- draw filled rectangle with transparent diagonal lines
      Filled -- draw filled rectangle     
     */
    enum DrawingStyle
    {
        OnlyFrame,
        StripWithLines,
        Filled
    };
    
    
    CalenDayStatusStrip(HbWidget *parent = 0);
    ~CalenDayStatusStrip();
    
    qreal range() const;

    CalenDayStatusStrip::DrawingStyle drawingStyle() const;
    
    QPair<QTime,QTime> startEndTime() const;
    
    void setRange(qreal range);
    
    void setDrawingStyle(CalenDayStatusStrip::DrawingStyle mDrawingStyle);
    
    void setStartEndTime(const QTime &startTime, const QTime &endTime);

    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, 
               QWidget *widget = 0);
    
    QPolygonF diagonalLine(QPointF startPoint, qreal dx, qreal dy);
    

    QPair<QTime,QTime> calculateStartEndPostion(
                                                    const QTime &startTime, 
                                                    const QTime &endTime
                                                   );
    
    qreal calculateMinuteHeight(const QTime &start, const QTime &end);
    
private:
    qreal mRange; //!<Distance between lines
    CalenDayStatusStrip::DrawingStyle mDrawingStyle;//!<Specify style of drawing
    /*!
       Original start and end time of event
     */
    QPair<QTime,QTime> mStartEndEventTime;
    
    static const qreal mMinute;//!<It keeps info how much seconds are in minute
    static const qreal mMinimumTime;//!<It keeps info about minimum timeline in minutes
};

Q_DECLARE_METATYPE(CalenDayStatusStrip::DrawingStyle)

#endif // CALENDAYSTATUSSTRIP_H
