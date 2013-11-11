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
* Description: Trash bin widget.
*
*/

#include <QGraphicsLinearLayout>

#include <HbIconItem>

#include "hstrashbinwidget.h"

namespace
{
    const char gNormalIconName[]    = "qtg_graf_hs_delete_normal";
    const char gHighlightIconName[] = "qtg_graf_hs_delete_highlight";
}

/*!
    \internal
    \class HsTrashBinWidget
    \ingroup group_hsutils
    \brief Trash bin widget.
*/

/*!
    \internal
    Constructor.
    
    \a parent Owner.
*/
HsTrashBinWidget::HsTrashBinWidget(QGraphicsItem *parent)
    : HbWidget(parent),
      mIconItem(0),
      mNormalIcon(0),
      mHighlightIcon(0),
      mNormalIconVisible(false)
{
    mNormalIcon = new HbIcon(gNormalIconName);
    mHighlightIcon = new HbIcon(gHighlightIconName);
    
    mIconItem = new HbIconItem;
    mIconItem->setIcon(*mNormalIcon);
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mIconItem);
    setLayout(layout);
}

/*!
    \internal
    Destructor.    
*/
HsTrashBinWidget::~HsTrashBinWidget()
{
}

/*!
    \internal
*/
void HsTrashBinWidget::activate()
{
    if (mNormalIconVisible) {
        mIconItem->setIcon(*mHighlightIcon);
        mNormalIconVisible = false;
    }
}

/*!
    \internal
*/
void HsTrashBinWidget::deactivate()
{
    if (!mNormalIconVisible) {
        mIconItem->setIcon(*mNormalIcon);
        mNormalIconVisible = true;
    }
}
