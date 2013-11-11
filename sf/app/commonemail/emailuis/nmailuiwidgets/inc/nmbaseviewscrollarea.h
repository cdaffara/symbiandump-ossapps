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
* Description: Class for handling the background scrollarea of the
* mail editor and viewer
*
*/

#ifndef NMBASEVIEWSCROLLAREA_H_
#define NMBASEVIEWSCROLLAREA_H_

#include <hbscrollarea.h>
#include "nmailuiwidgetsdef.h"

class QGraphicsItem;

class NMAILUIWIDGETS_EXPORT NmBaseViewScrollArea : public HbScrollArea
{
    Q_OBJECT
public:
    NmBaseViewScrollArea(QGraphicsItem *parentView=0);
    virtual ~NmBaseViewScrollArea();

signals:
    void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void handleLongPressGesture(const QPointF &point);

public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

protected:
    void longPressGesture(const QPointF &point);
};


#endif /* NMBASEVIEWSCROLLAREA_H_ */
