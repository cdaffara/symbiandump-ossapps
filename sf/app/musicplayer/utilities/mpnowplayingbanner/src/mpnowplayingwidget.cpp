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
* Description: Music Player now playing widget.
*
*/

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <hbicon.h>
#include <hbevent.h>
#include <hbframeitem.h>
#include <hbinstantfeedback.h>

#include "mpnowplayingwidget.h"
#include "mpnowplayingwidget_p.h"
#include "mptrace.h"

/*!
    \class MpNowPlayingWidget
    \brief Custom HB widget that displays currently playing song information.

    This is a composite widget made out of HB widgets, it displays information
    for the current playing song on the playback engine indicated at 
    construction time.
*/

/*!
    \fn MpNowPlayingWidget::clicked()

    MpNowPlayingWidget emits this signal when there is a mouseReleaseEvent on
    top of the widget.
*/

/*!
    \fn MpNowPlayingWidget::playbackAttachmentChanged(bool attached)

    MpNowPlayingWidget emits this signal with a true value if the widget is 
    ready to display relevant data for the current playback, it is emmited 
    with false value if there is not playback instance to bind with. It is 
    recommended to use this signal to place and remove the widget from the UI.
*/


/*!
 Constructs the now playing widget.
 */
MpNowPlayingWidget::MpNowPlayingWidget( QGraphicsItem *parent )
    : HbWidget(parent),
    d_ptr ( new MpNowPlayingWidgetPrivate( this ) ),
    mFrameItem(0)
{
    TX_ENTRY_ARGS( " Parent=" << (void *)parent )
    TX_EXIT
    mFrameItem = new HbFrameItem( this );
    mFrameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
    mFrameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
    mFrameItem->setZValue(-1);
}

/*!
 Destructs the now playing widget.
 */
MpNowPlayingWidget::~MpNowPlayingWidget()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

/*!
 If \a enabled is true, the item is enabled; otherwise, it is disabled.
 Item is enabled by default.
 */
void MpNowPlayingWidget::setEnabled( bool enabled )
{
    TX_ENTRY
    d_ptr->setEnabled(enabled);
    TX_EXIT
}

/*!
    \reimp
 */
void MpNowPlayingWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    TX_ENTRY
    mFrameItem->setGeometry( rect() );
    HbWidget::resizeEvent( event );
    TX_EXIT
}

/*!
 Returns if banner is attached or not
 */
bool MpNowPlayingWidget::isBannerAttached()
{
    TX_ENTRY
    return d_ptr->isBannerAttached();
}
/*!
    \reimp
 */
void MpNowPlayingWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    HbInstantFeedback::play( HbFeedback::Basic );

    if ( event->button() == Qt::LeftButton ) {
        d_ptr->handleMousePressEvent( event, true );
        update();
        event->accept();
    }
    else {
        event->ignore();
    }
    TX_EXIT
}

/*!
    \reimp
 */
void MpNowPlayingWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    if ( event->button() == Qt::LeftButton ) {
        d_ptr->handleMousePressEvent( event, false );
        update();
        if ( !d_ptr->handleClickEvent( event ) ) {
            emit clicked();
        }
        event->accept();
    }
    else {
        event->ignore();
    }
    TX_EXIT
}

/*!
    \reimp
 */
void MpNowPlayingWidget::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    d_ptr->handleMouseMoveEvent( event );
    update();
    event->ignore();
    TX_EXIT
}


