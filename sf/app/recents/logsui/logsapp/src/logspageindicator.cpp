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

#include <QGraphicsLinearLayout>

#include "logspageindicator.h"
#include "logspageindicatoritem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsPageIndicator::LogsPageIndicator(QGraphicsItem *parent)
  : HbWidget(parent),
    mActiveItemIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsPageIndicator::~LogsPageIndicator()
{
    qDeleteAll(mItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicator::initialize(int itemCount, int activeItemIndex)
{
    qDeleteAll(mItems);
    mItems.clear();
    mActiveItemIndex = -1;

    if (itemCount < 1 || activeItemIndex < 0 || 
        itemCount <= activeItemIndex) {
        return;
    }

    for (int i = 0; i < itemCount; ++i) {
        mItems << new LogsPageIndicatorItem(i == activeItemIndex);
    }    
    mActiveItemIndex = activeItemIndex;
    layoutItems();
}
 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsPageIndicator::itemCount() const
{
    return mItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicator::setActiveItemIndex(int activeItemIndex)
{
    if (activeItemIndex < 0 || itemCount() <= activeItemIndex) {
        return;
    }

    mActiveItemIndex = activeItemIndex;
    for (int i = 0; i < mItems.count(); ++i) {
        mItems[i]->setActive(i == activeItemIndex);
    }
}
 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsPageIndicator::activeItemIndex() const
{
    return mActiveItemIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsPageIndicator::layoutItems()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    foreach (LogsPageIndicatorItem *item, mItems) {
        layout->addItem(item);
    }
    layout->addStretch();
    setLayout(layout);
}
