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
#include <QImage>
#include <QGraphicsView>

#include "mpsnapshotwidget.h"
#include "mptrace.h"

/*!
    \class MpSnapshotWidget
    \brief Custom HB widget that grabs a snapshot of the graphics view.

    This widget grabs a snapshot of the graphics view so it can be drawn as 
    a decoy for transitios or fast ui response simulation.
*/


/*!
 Constructs the snapshot widget.
 */
MpSnapshotWidget::MpSnapshotWidget( QGraphicsItem *parent )
    : HbWidget(parent),
      mSnapshot(0)
{
    TX_ENTRY_ARGS( " Parent=" << (void *)parent )
    setFlag( QGraphicsItem::ItemHasNoContents, false );
    TX_EXIT
}

/*!
 Destructs the snapshot widget.
 */
MpSnapshotWidget::~MpSnapshotWidget()
{
    TX_ENTRY
    if (mSnapshot)
     delete mSnapshot;
    TX_EXIT
}

/*!
 Takes a snapshot of an \a item within \a graphicsView, also sets its position
  and geometry so it can be used as a decoy for that itme.
 */
void MpSnapshotWidget::capture(QGraphicsView *graphicsView, QGraphicsItem *item)
{
    TX_ENTRY
    if (mSnapshot){
        delete mSnapshot;
        mSnapshot = 0;
    }
    setGeometry(QRect(QPoint(0,0),item->sceneBoundingRect().toRect().size()));
    setPos(item->sceneBoundingRect().toRect().topLeft());
    mSnapshot = new QImage(
            item->sceneBoundingRect().toRect().size() , 
            QImage::Format_ARGB32_Premultiplied);
    QPainter p( mSnapshot);
    graphicsView->render( &p, rect(), item->sceneBoundingRect().toRect());
    TX_EXIT
}

/*!
    \reimp
 */
void MpSnapshotWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    TX_ENTRY
    Q_UNUSED(widget)
    Q_UNUSED(option)
    if (mSnapshot) {
        painter->drawImage( rect(), *mSnapshot );
    }
    TX_EXIT
}
