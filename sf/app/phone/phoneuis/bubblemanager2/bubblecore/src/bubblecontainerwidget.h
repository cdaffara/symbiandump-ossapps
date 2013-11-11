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
* Description: Bubble container
*
*/

#ifndef BUBBLECONTAINERWIDGET_H
#define BUBBLECONTAINERWIDGET_H

#include <hbwidget.h>

class HbFrameItem;

class BubbleContainerWidget : public HbWidget
{
    Q_OBJECT

public:
    BubbleContainerWidget(QGraphicsItem* item=0);
    virtual ~BubbleContainerWidget();

signals:
    void clicked();

protected:
    void createPrimitives();
    void updatePrimitives();

    void mousePressEvent( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    void showEvent(QShowEvent *event);

private:
    HbFrameItem* mBackground;
    bool mPressed;
};

#endif // BUBBLECONTAINERWIDGET_H
