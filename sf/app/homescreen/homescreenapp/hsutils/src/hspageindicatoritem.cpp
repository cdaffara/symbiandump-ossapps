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
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>

#include <HbIconItem>

#include "hspageindicatoritem.h"

namespace
{
    const char gNormalIconName[]    = "qtg_graf_hspage_normal";
    const char gHighlightIconName[] = "qtg_graf_hspage_highlight";
}


/*!
    \class HsPageIndicatorItem
    \ingroup group_hsutils
    \brief 
*/

/*!

*/
HsPageIndicatorItem::HsPageIndicatorItem(bool active, QGraphicsItem *parent)
  : HbWidget(parent),
    mIconItem(0),
    mIsActive(active)
{
    mIconItem = new HbIconItem;
    if (active) {
        mIconItem->setIcon(HbIcon(gHighlightIconName));
    } else {
        mIconItem->setIcon(HbIcon(gNormalIconName));
    }
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(mIconItem);
    setLayout(layout);

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
    effect->setColor(Qt::white);
    effect->setStrength(0);
    effect->setEnabled(false);
    setGraphicsEffect(effect);
}

/*!

*/
HsPageIndicatorItem::~HsPageIndicatorItem()
{
}

/*!

*/
void HsPageIndicatorItem::setActive(bool active)
{
    if (mIsActive != active) {
        mIsActive = active;
        if (mIsActive) {
            mIconItem->setIcon(HbIcon(gHighlightIconName));
            startAnimation();
        } else {
            mIconItem->setIcon(HbIcon(gNormalIconName));
        }
    }
}

/*!

*/
bool HsPageIndicatorItem::isActive() const
{
    return mIsActive;
}

/*!

*/
bool HsPageIndicatorItem::isAnimationRunning() const
{
    return graphicsEffect()->isEnabled();
}

/*!

*/
void HsPageIndicatorItem::startAnimation()
{
    graphicsEffect()->setEnabled(true);
    setTransformOriginPoint(rect().center());
    QPropertyAnimation *animation = 
        new QPropertyAnimation(graphicsEffect(), "strength");
    
    animation->setDuration(800);
    animation->setKeyValueAt(0.2, 1);
    animation->setEndValue(0);
    
    connect(this,SIGNAL(destroyed()),animation,SLOT(stop()));
    connect(animation, SIGNAL(finished()), SLOT(animationFinished()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!

*/
void HsPageIndicatorItem::animationFinished()
{
    graphicsEffect()->setEnabled(false);
}
