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
* Description:
*
*/

// System includes

// User includes
#include "radiofrequencyitem.h"
#include "radiofrequencystripbase.h"
#include "radiofrequencystripdef.h"

/*!
 *
 */
RadioFrequencyItem::RadioFrequencyItem( uint frequency ) :
    mFrequency( frequency == 0 ? 0 : frequency * FrequencyStrip::ONE_HERTZ ),
    mGraphicsItem( 0 )
{
}

/*!
 *
 */
uint RadioFrequencyItem::frequency() const
{
    return mFrequency;
}

/*!
 *
 */
QPixmap RadioFrequencyItem::updatePrimitive( QGraphicsPixmapItem* item )
{
    mGraphicsItem = item;
    return mPixmap;
}

/*!
 *
 */
void RadioFrequencyItem::setPixmap( QPixmap pixmap )
{
    mPixmap = pixmap;
    if ( mGraphicsItem ) {
        mGraphicsItem->setPixmap( mPixmap );
    }
}
