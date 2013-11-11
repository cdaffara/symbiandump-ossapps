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
* Description:  Track List Widget for Music Player Media Wall.
*
*/
#ifndef MPTRACKLISTWIDGET_H
#define MPTRACKLISTWIDGET_H

#include <QGesture>

#include <hbwidget.h>

class HbFrameItem;
class HbListView;

class MpTrackListWidget : public HbWidget
{
    Q_OBJECT
  
public:
    
    MpTrackListWidget( QGraphicsItem *parent = 0 );
    virtual ~MpTrackListWidget();
    HbListView *list(); 



signals:

    void closed();

private:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void gestureEvent(QGestureEvent *event);
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, 
            QWidget *widget = 0 );
    QSwipeGesture::SwipeDirection swipeAngleToDirection( int angle );
    
private:
    HbListView      *mList;      // own - parented
    HbFrameItem     *mFrameItem; // own - parented
};

#endif // MPTRACKLISTWIDGET_H

// EOF
