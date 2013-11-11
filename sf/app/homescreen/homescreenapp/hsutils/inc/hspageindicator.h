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

#ifndef HSPAGEINDICATOR_H
#define HSPAGEINDICATOR_H

#include <HbWidget>

#include "hsutils_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HsPageIndicatorItem;

class HSUTILS_EXPORT HsPageIndicator : public HbWidget
{
    Q_OBJECT

public:
    HsPageIndicator(qreal spacing, QGraphicsItem *parent = 0);
    ~HsPageIndicator();

    void initialize(int itemCount, int activeItemIndex);
    
    int itemCount() const;

    void setActiveItemIndex(int index);
    int activeItemIndex() const;

    void addItem(int activeItemIndex);
    void removeItem(int activeItemIndex);

    bool isAnimationRunning() const;

    void setSpacing(qreal spacing);

private:
    Q_DISABLE_COPY(HsPageIndicator)
    void layoutItems();    

private:
    QList<HsPageIndicatorItem *> mItems;    
    int mActiveItemIndex;
    qreal mSpacing;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSPAGEINDICATOR_H
