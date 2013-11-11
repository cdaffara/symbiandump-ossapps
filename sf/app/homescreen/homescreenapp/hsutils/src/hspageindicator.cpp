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

#include "hspageindicator.h"
#include "hspageindicatoritem.h"


/*!
    \class HsPageIndicator
    \ingroup group_hsutils
    \brief 
*/

/*!

*/
HsPageIndicator::HsPageIndicator(qreal spacing, QGraphicsItem *parent)
  : HbWidget(parent),
    mActiveItemIndex(-1),
    mSpacing(spacing)
{
}

/*!

*/
HsPageIndicator::~HsPageIndicator()
{
    qDeleteAll(mItems);
}

/*!

*/
void HsPageIndicator::initialize(int itemCount, int activeItemIndex)
{
    qDeleteAll(mItems);
    mItems.clear();
    mActiveItemIndex = -1;

    if (itemCount < 1 || activeItemIndex < 0 ||
        itemCount <= activeItemIndex) {
        return;
    }

    for (int i = 0; i < itemCount; ++i) {
        mItems << new HsPageIndicatorItem(i == activeItemIndex);
    }
    mActiveItemIndex = activeItemIndex;
    layoutItems();
}

/*!

*/
int HsPageIndicator::itemCount() const
{
    return mItems.count();
}

/*!

*/
void HsPageIndicator::setActiveItemIndex(int activeItemIndex)
{
    if (activeItemIndex < 0 || itemCount()-1 < activeItemIndex) {
        return;
    }

    mActiveItemIndex = activeItemIndex;
    for (int i = 0; i < mItems.count(); ++i) {
        mItems[i]->setActive(i == activeItemIndex);
    }
}

/*!

*/
int HsPageIndicator::activeItemIndex() const
{
    return mActiveItemIndex;
}

/*!

*/
void HsPageIndicator::addItem(int activeItemIndex)
{
    if (activeItemIndex < 0 || itemCount() < activeItemIndex) {
        return;
    }

    mItems.append(new HsPageIndicatorItem);
    layoutItems();
    setActiveItemIndex(activeItemIndex);
}

/*!

*/
void HsPageIndicator::removeItem(int activeItemIndex)
{
    if (activeItemIndex < 0 || itemCount() <= activeItemIndex) {
        return;
    }

    if (activeItemIndex == itemCount()-1) {
        activeItemIndex--;
    }

    delete mItems.last();
    mItems.removeLast();
    layoutItems();
    
    setActiveItemIndex(activeItemIndex);
}

/*!

*/
bool HsPageIndicator::isAnimationRunning() const
{
    if (0 <= mActiveItemIndex && mActiveItemIndex <= itemCount()){
        return mItems.at(mActiveItemIndex)->isAnimationRunning();
    }
    return false;
}

/*!

*/
void HsPageIndicator::layoutItems()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(mSpacing);
    layout->addStretch();
    foreach (HsPageIndicatorItem *item, mItems) {
        item->setPreferredSize(preferredHeight(), preferredHeight());
        layout->addItem(item);
    }
    layout->addStretch();
    setLayout(layout);
}

/*!

*/
void HsPageIndicator::setSpacing(qreal spacing)
{
    QGraphicsLinearLayout *linearLayout = static_cast<QGraphicsLinearLayout*>(layout());
    if (linearLayout) {
        linearLayout->setSpacing(spacing);
    }
}
