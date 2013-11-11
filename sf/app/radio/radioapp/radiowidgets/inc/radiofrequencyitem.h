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

#ifndef RADIOFREQUENCYITEM_H
#define RADIOFREQUENCYITEM_H

// System includes
#include <QPixmap>

// Forward declarations
class QGraphicsPixmapItem;

// Class declaration
class RadioFrequencyItem
{
public:

    RadioFrequencyItem( uint frequency );

    uint frequency() const;

    QPixmap updatePrimitive( QGraphicsPixmapItem* item );

    void setPixmap( QPixmap pixmap );

private: // data

    uint                        mFrequency;

    QPixmap                     mPixmap;

    QGraphicsPixmapItem*        mGraphicsItem;

};

#endif // RADIOFREQUENCYITEM_H
