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
* Description: Mocked classes for testing CalenDayContentWidget class
*
*/

#ifndef CALENSERVICES_H
#define CALENSERVICES_H

/*!
 Mocked class MCalenServices
 */
class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    ~MCalenServices() {
        
    }
};

#endif // CALENSERVICES_H


#ifndef CALENDAYITEMVIEW_H
#define CALENDAYITEMVIEW_H

#include <QGraphicsItem>
#include <QPointF>
#include <HbWidget>
#include <HbModelIterator>

/*!
 Mocked class CalenDayItemView
 */
class CalenDayItemView : public HbWidget
{
    Q_OBJECT
    
public:
    CalenDayItemView(MCalenServices &services, HbModelIterator *iterator, 
        QGraphicsItem *parent = NULL) : HbWidget(parent) {
        Q_UNUSED(services)
        Q_UNUSED(iterator)
        
        mCurrentPos = QPointF(0,0);
    } 
    ~CalenDayItemView() {
        
    }
    
    void scrollVertically(const QPointF &newPosition) {
        mCurrentPos = newPosition;
    }

signals:
    void scrollPositionChanged(const QPointF&);
    
public:
    QPointF mCurrentPos;
};

#endif // CALENDAYITEMVIEW_H

