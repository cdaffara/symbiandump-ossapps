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
* Description:  Album Cover Widget.
*
*/
#include <QPainter>
#include <QGraphicsSceneMouseEvent>


#include "mpalbumcoverwidget.h"

/*!
    \class MpAlbumCoverWidget
    \brief Album Cover Widget.
    This widget provides up scaled rendering of images and SVG rendering.

    This widget can be used instead of HbLabel or HbIconItem when up scaling 
    and downscaling during painting is desired, there is no need to preprocess
    images to the target size, this is taken care of during paint in a more 
    efficient manor; this widget renders images flush with the geometry.
    Default icon placeholder is also supported.

*/

/*!
    \fn void clicked( )

    This signal is emitted when the item is clicked.
 */

/*!
    Constructs the album cover widget
 */
MpAlbumCoverWidget::MpAlbumCoverWidget( QGraphicsItem *parent ) : 
    HbWidget( parent )
{
    setFlag( QGraphicsItem::ItemHasNoContents, false );
    grabGesture(Qt::TapGesture);
}

/*!
 Destructs the album cover widget.
 */
MpAlbumCoverWidget::~MpAlbumCoverWidget()
{
}

/*!
    Sets the \a icon as current album cover.
 */
void MpAlbumCoverWidget::setIcon( const HbIcon &icon )
{
    if (icon != mIcon) {
        mIcon = icon;
        mPixmap = QPixmap();
        update();
    }
}

/*!
    Sets the \a icon as default cover, to be used in case the album cover is null icon.
 */
void MpAlbumCoverWidget::setDefaultIcon( const HbIcon &icon )
{
    mDefaultIcon = icon;
    update();
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget )
    Q_UNUSED( option )
    if ( isEnabled() ){
        if ( !mIcon.isNull() ) {
            if ( mPixmap.isNull() ) {
                mPixmap = mIcon.qicon().pixmap( size().toSize() );
            }
            //We paint directly to stretch up/down if necesary.
            painter->drawPixmap( rect(), mPixmap, QRectF() );
        }
        else {
            //We use HbIcon paint to render vector graphics.
            mDefaultIcon.setSize(size());
            mDefaultIcon.paint(painter,rect());
        }
    }
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
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
void MpAlbumCoverWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        emit clicked();
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::gestureEvent(QGestureEvent *event)
{
    QGesture* gesture = event->gesture(Qt::TapGesture);
    if (gesture) {
         event->accept(Qt::TapGesture);
    }    
}

//EOF
