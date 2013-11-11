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

//EOF
