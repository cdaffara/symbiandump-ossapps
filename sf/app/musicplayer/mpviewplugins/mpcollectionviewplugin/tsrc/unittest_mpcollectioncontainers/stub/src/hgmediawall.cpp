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
* Description: HgMediawall stub for testing mpcollectioncontainers
*
*/

#include "hgmediawall.h"


/*!
 Constructs the mediawall widget stub.
 */
HgWidget::HgWidget( QGraphicsItem *parent )
{
    Q_UNUSED(parent);
}

/*!
 Destructs the mediawall widget stub.
 */
HgWidget::~HgWidget()
{
}

/*!
 Stub function
 */
void HgWidget::setScrollBarPolicy( ScrollBarPolicy policy )
{
    Q_UNUSED(policy);
}

/*!
 Stub function
 */
void HgWidget::setModel( QAbstractItemModel *model )
{
    Q_UNUSED(model);
}

/*!
 Stub function
 */
void HgWidget::scrollTo( const QModelIndex &index )
{
    Q_UNUSED(index);
}

/*!
 Stub function
 */
QModelIndex HgWidget::currentIndex() const
{
    return QModelIndex();
}

/*!
 Stub function
 */
QSizeF HgWidget::itemSize() const
{
    return QSizeF();
}

/*!
 Stub function
 */
void HgWidget::setDefaultImage(QImage defaultImage)
{
    Q_UNUSED(defaultImage);
}


//===========================================================================
//===========================================================================

/*!
 Constructs the mediawall widget stub.
 */
HgMediawall::HgMediawall( QGraphicsItem *parent )
{
    Q_UNUSED(parent);
    mReflectionEnabled = true;
}

/*!
 Destructs the mediawall widget stub.
 */
HgMediawall::~HgMediawall()
{
}

/*!
 Stub function
 */
void HgMediawall::setTitlePosition( LabelPosition position )
{
    Q_UNUSED(position);
}

/*!
 Stub function
 */
void HgMediawall::setDescriptionPosition( LabelPosition position )
{
    Q_UNUSED(position);
}

/*!
 Stub function
 */
void HgMediawall::setTitleFontSpec( const HbFontSpec &fontSpec )
{
    Q_UNUSED(fontSpec);
}

/*!
 Stub function
 */
void HgMediawall::setDescriptionFontSpec( const HbFontSpec &fontSpec )
{
    Q_UNUSED(fontSpec);
}

/*!
 Stub function
 */
void HgMediawall::enableReflections( bool enabled )
{
    mReflectionEnabled = enabled;
}

/*!
 Stub function
 */
bool HgMediawall::reflectionsEnabled() const
{
    return mReflectionEnabled;
}

