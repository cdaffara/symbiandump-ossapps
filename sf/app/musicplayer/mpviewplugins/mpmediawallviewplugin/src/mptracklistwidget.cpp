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
* Description: Track List Widget for Music Player Media Wall.
*
*/


#include <QGraphicsSceneMouseEvent>

#ifndef UNIT_TESTING
#include <hbinstance.h>
#else
#include "hbinstancestub.h"
#endif

#include <hblistview.h>
#include <hbframeitem.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>
#include <hbscrollbar.h>
#include <hbcolorscheme.h>


#include "mptracklistwidget.h"


const int swipeAngleTolerance = 30; // angle is from 0 to 360
const int tracklistOutlineSize = 1; //

/*!
    \class MpTrackListWidget
    \brief Widget with a list of tracks for Media Wall.
    
    This widget provides a list with custom style and a background.

*/

/*!
    \fn void closed()

    This signal is emitted when the track list is closed by the user with a 
    left swipe gesture.
 */


/*!
   Creates the MpTrackListWidget. 
 */
MpTrackListWidget::MpTrackListWidget( QGraphicsItem *parent ) : HbWidget( parent ) 
{
    setFlag( QGraphicsItem::ItemHasNoContents, false );
    // Register the custorm css path for the list items.
    HbStyleLoader::registerFilePath( ":/css/mpcustomlistitem.css" );
    HbStyleLoader::registerFilePath( ":/css/mpcustomlistitem.hblistviewitem.widgetml" );    
    mList = new HbListView( this );
    // set layout name that matches the custom list item layout.
    mList->setLayoutName( "mpmwtracklist" );
    HbScrollBar *scrollbar = mList->verticalScrollBar();
    scrollbar->show();
    scrollbar->setInteractive( true );
    mList->setVerticalScrollBarPolicy( HbScrollArea::ScrollBarAlwaysOn );
    
    //grab the gesture for close.
    grabGesture(Qt::SwipeGesture);
    
    mFrameItem = new HbFrameItem( this );
    mFrameItem->frameDrawer().setFrameType( HbFrameDrawer::OnePiece );
    mFrameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_popup_c" );  //replace for qtg_graf_mediawall_list_bg when available.
    mFrameItem->setFlag(QGraphicsItem::ItemStacksBehindParent);
}

/*!
 Destructs the track list widget.
 */
MpTrackListWidget::~MpTrackListWidget()
{
}

/*!
    Returns the HbListView instance.
 */
HbListView *MpTrackListWidget::list()
{
    return mList;
} 

/*!
    \reimp
 */
void MpTrackListWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    mFrameItem->setGeometry( rect() );
    qreal margin = tracklistOutlineSize;
    mList->setGeometry( rect().adjusted( margin, margin, -margin, -margin ) );
    HbWidget::resizeEvent( event );
}

/*!
    \reimp
 */
void MpTrackListWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
    \reimp
 */
void MpTrackListWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
    \reimp
 */
void MpTrackListWidget::gestureEvent(QGestureEvent *event)
{
    QGesture* gesture = event->gesture(Qt::SwipeGesture);
    if (gesture) {
        QSwipeGesture* swipe = static_cast<QSwipeGesture *>(gesture);
        if (swipe->state() == Qt::GestureFinished && 
                swipeAngleToDirection (swipe->swipeAngle()) == QSwipeGesture::Left ) {
            //Left gesture is the direction in wich the track list slides to close.
            emit closed();
            event->accept(Qt::SwipeGesture);
        }
    }    
}

/*!
    \reimp
 */
void MpTrackListWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget )
    Q_UNUSED( option )
    painter->save();
    qreal outlineWidth(tracklistOutlineSize);
    painter->setPen(QPen(HbColorScheme::color("qtc_view_normal_secondary"), outlineWidth,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));
    painter->drawRect( rect().adjusted(outlineWidth/2,outlineWidth/2,-outlineWidth/2,-outlineWidth/2) );
    painter->restore();
}

/*!
    Maps swipe \a angle to QSwipeGesture::SwipeDirection based on a tolerance 
    parameter and orientation. This funtions helps to identify a swipe even 
    if it is not sharp movement from 180 to 0 degrees on the righ swipe 
    gesture for instance. Since gesture events are mesured on device 
    cordinates this also helps to correct the gesture to local cordinates 
    bases on device orientation, wich is done using QT transformations, 
    meaning local cordinates and device cordinates are not always aligned.
 */
QSwipeGesture::SwipeDirection MpTrackListWidget::swipeAngleToDirection(
        int angle )
{
    int delta = swipeAngleTolerance;
    if ( hbInstance->allMainWindows()[0]->orientation() == Qt::Horizontal ) {
    //correction for transformation on rotation.
#ifdef __WINS__ //wincw with forced rotation is to the right.
    angle += ( angle < 90 ) ? 270 : -90; 
#else//currently hardware rotations is to the left.
    angle += 90;
    if ( angle > 360 ) {
        angle -= 360;
    }
#endif    
      
    }
    QSwipeGesture::SwipeDirection direction( QSwipeGesture::NoDirection );
    if ( ( angle > 90 - delta) && ( angle < 90 + delta ) ) {
        direction = QSwipeGesture::Up;
    } else if ( ( angle > 270 - delta ) && (angle < 270 + delta ) ) {
        direction = QSwipeGesture::Down;
    } else if ( ( ( angle >= 0 ) && ( angle < delta ) ) 
            || ( ( angle > 360 - delta ) && ( angle <= 360 ) ) ) {
        direction = QSwipeGesture::Right;
    } else if ( ( angle > 180 - delta ) && ( angle < 180 + delta ) ) {
        direction = QSwipeGesture::Left;
    }
    return direction;    
}

//EOF
