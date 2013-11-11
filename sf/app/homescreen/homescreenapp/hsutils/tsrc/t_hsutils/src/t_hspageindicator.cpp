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

#ifndef ONLY_MENU_TESTCASES

#include "t_hsutils.h"
#include "hspageindicator.h"
#include "hspageindicatoritem.h"

#include <QGraphicsLayout>
#include <QGraphicsLinearLayout>

void  t_hsUtils::testPageIndicatorConstruction()
{
    HsPageIndicator pi(8);
    QCOMPARE(pi.itemCount(), 0);
    QCOMPARE(pi.activeItemIndex(), -1);

    pi.initialize(-1, -1);
    QCOMPARE(pi.itemCount(), 0);
    QCOMPARE(pi.activeItemIndex(), -1);

    pi.initialize(0, -1);
    QCOMPARE(pi.itemCount(), 0);
    QCOMPARE(pi.activeItemIndex(), -1);

    pi.initialize(1, -1);
    QCOMPARE(pi.itemCount(), 0);
    QCOMPARE(pi.activeItemIndex(), -1);

    pi.initialize(1, 2);
    QCOMPARE(pi.itemCount(), 0);
    QCOMPARE(pi.activeItemIndex(), -1);

    pi.initialize(3, 1);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.initialize(3, 2);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 2);
}

void t_hsUtils::testPageIndicatorActiveItem()
{
    HsPageIndicator pi(8);
    pi.initialize(3, 1);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.setActiveItemIndex(1);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.setActiveItemIndex(0);
    QCOMPARE(pi.activeItemIndex(), 0);

    pi.setActiveItemIndex(2);
    QCOMPARE(pi.activeItemIndex(), 2);

    pi.setActiveItemIndex(-1);
    QCOMPARE(pi.activeItemIndex(), 2);

    pi.setActiveItemIndex(3);
    QCOMPARE(pi.activeItemIndex(), 2);

    QCOMPARE(pi.mItems.at(0)->isActive(), false);
    QCOMPARE(pi.mItems.at(1)->isActive(), false);
    QCOMPARE(pi.mItems.at(2)->isActive(), true);
}

void t_hsUtils::testPageIndicatorAddRemoveItem()
{
    HsPageIndicator pi(8);
    pi.initialize(3, 1);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.addItem(-1);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.addItem(4);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 1);

    pi.addItem(0);
    QCOMPARE(pi.itemCount(), 4);
    QCOMPARE(pi.activeItemIndex(), 0);

    pi.addItem(4);
    QCOMPARE(pi.itemCount(), 5);
    QCOMPARE(pi.activeItemIndex(), 4);

    pi.addItem(6);
    QCOMPARE(pi.itemCount(), 5);
    QCOMPARE(pi.activeItemIndex(), 4);

    pi.removeItem(-1);
    QCOMPARE(pi.itemCount(), 5);
    QCOMPARE(pi.activeItemIndex(), 4);

    pi.removeItem(5);
    QCOMPARE(pi.itemCount(), 5);
    QCOMPARE(pi.activeItemIndex(), 4);

    pi.removeItem(4);
    QCOMPARE(pi.itemCount(), 4);
    QCOMPARE(pi.activeItemIndex(), 3);

    pi.removeItem(3);
    QCOMPARE(pi.itemCount(), 3);
    QCOMPARE(pi.activeItemIndex(), 2);

    pi.removeItem(0);
    QCOMPARE(pi.itemCount(), 2);
    QCOMPARE(pi.activeItemIndex(), 0);
}

void t_hsUtils::testPageIndicatorAnimations()
{
    HsPageIndicator pi(8);
    pi.initialize(3, 1);
    pi.setSpacing(8);

    QCOMPARE(pi.isAnimationRunning(), false);
    pi.setActiveItemIndex(0);
    QCOMPARE(pi.isAnimationRunning(), true);

    int i = 5;
    do {
        QTest::qWait(1000);
    } while (pi.isAnimationRunning() && --i);
    if (!i) {
        QFAIL("testPageIndicatorAnimations() failed");
    }
}

#endif // ONLY_MENU_TESTCASES
