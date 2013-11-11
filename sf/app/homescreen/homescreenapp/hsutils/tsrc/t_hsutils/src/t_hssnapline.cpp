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
#include "hssnapline.h"
#include "hsapp_defs.h"

void t_hsUtils::testSnapLineConstruction()
{
    HsSnapLine sl = HsSnapLine();

    QVERIFY(sl.mFadeInAnimation);
    QVERIFY(sl.mFadeOutAnimation);

    QVERIFY(qFuzzyCompare(sl.mOpacity, 0));
    QCOMPARE(sl.mFadeInAnimationDuration,0);
    QCOMPARE(sl.mFadeOutAnimationDuration,0);
}

void t_hsUtils::testSnapLineSetConfiguration()
{
    HsSnapLine sl = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    sl.setConfiguration(configuration);

    QCOMPARE(sl.mFadeInAnimationDuration, 1000);
    QCOMPARE(sl.mFadeOutAnimationDuration , 500);
}

void t_hsUtils::testShowAndHideHorizontalLine()
{
    HsSnapLine slHorizontal = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slHorizontal.setConfiguration(configuration);

    //before showing the line, no animation should be running
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    QLineF horizontalLine(QPointF(100,100), QPointF(200,100));
    slHorizontal.showLine(horizontalLine);
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    slHorizontal.hideLine();
    QVERIFY(slHorizontal.isFadeOutAnimationRunning());
    QTest::qWait(1000);
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());
}

void t_hsUtils::testShowAndHideVerticalLine()
{
    HsSnapLine slVertical = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slVertical.setConfiguration(configuration);

    //before showing the line, fade in animation should not be running
    QVERIFY(!slVertical.isFadeInAnimationRunning());
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    QLineF verticalLine(QPointF(100, 100), QPointF(100, 200));
    slVertical.showLine(verticalLine);
    QVERIFY(slVertical.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slVertical.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    slVertical.hideLine();
    QVERIFY(slVertical.isFadeOutAnimationRunning());
    QTest::qWait(1000);
    QVERIFY(!slVertical.isFadeOutAnimationRunning());
}

void t_hsUtils::testHideHorizontalLineWhileFadeInAnimationRunning()
{
    HsSnapLine slHorizontal = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slHorizontal.setConfiguration(configuration);

    //before showing the line, no animation should be running
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    QLineF horizontalLine(QPointF(100,100), QPointF(200,100));
    slHorizontal.showLine(horizontalLine);
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    //Wait only 250ms and call the hideLine()
    QTest::qWait(250);

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    slHorizontal.hideLine();
    QVERIFY(slHorizontal.isFadeOutAnimationRunning());
    QTest::qWait(1000);
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());
}

void t_hsUtils::testHideVerticalLineWhileFadeInAnimationRunning()
{
    HsSnapLine slVertical = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slVertical.setConfiguration(configuration);

    //before showing the line, fade in animation should not be running
    QVERIFY(!slVertical.isFadeInAnimationRunning());
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    QLineF verticalLine(QPointF(100, 100), QPointF(100, 200));
    slVertical.showLine(verticalLine);
    QVERIFY(slVertical.isFadeInAnimationRunning());
    //Wait only 250ms and call the hideLine()
    QTest::qWait(250);

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    slVertical.hideLine();
    QVERIFY(slVertical.isFadeOutAnimationRunning());
    QTest::qWait(1000);
    QVERIFY(!slVertical.isFadeOutAnimationRunning());
}

void t_hsUtils::testShowHorizontalLineWhileFadeOutAnimationRunning()
{
    HsSnapLine slHorizontal = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slHorizontal.setConfiguration(configuration);

    //before showing the line, no animation should be running
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    QLineF horizontalLine(QPointF(100,100), QPointF(200,100));
    slHorizontal.showLine(horizontalLine);
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    slHorizontal.hideLine();
    QVERIFY(slHorizontal.isFadeOutAnimationRunning());
    //ShowLine is called to show the line at the same location, where the line was displayed before.
    //Wait only 250ms and call the showline()
    QTest::qWait(250);

    slHorizontal.showLine(horizontalLine);
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    QTest::qWait(2000);
}

void t_hsUtils::testShowVerticalLineWhileFadeOutAnimationRunning()
{
    HsSnapLine slVertical = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slVertical.setConfiguration(configuration);

    //before showing the line, fade in animation should not be running
    QVERIFY(!slVertical.isFadeInAnimationRunning());
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    QLineF verticalLine(QPointF(100, 100), QPointF(100, 200));
    slVertical.showLine(verticalLine);
    QVERIFY(slVertical.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slVertical.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    slVertical.hideLine();
    QVERIFY(slVertical.isFadeOutAnimationRunning());
    //ShowLine is called to show the line at the same location, where the line was displayed before.
    //Wait only 250ms and call the showline()
    QTest::qWait(250);

    slVertical.showLine(verticalLine);
    QVERIFY(!slVertical.isFadeOutAnimationRunning());
    QVERIFY(slVertical.isFadeInAnimationRunning());
    QTest::qWait(2000);
}

void t_hsUtils::testShowHorizontalLineAtNewPositionWhileFadeOutAnimationRunning()
{
    HsSnapLine slHorizontal = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slHorizontal.setConfiguration(configuration);

    //before showing the line, no animation should be running
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    QLineF horizontalLine(QPointF(100,100), QPointF(200,100));
    slHorizontal.showLine(horizontalLine);
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());

    slHorizontal.hideLine();
    QVERIFY(slHorizontal.isFadeOutAnimationRunning());
    QTest::qWait(250);

    //construct a new line which is 100 pixels horizontally away from previous horizontal line
    QLineF newHorizontalLine(QPointF(100,200), QPointF(200,200));
    slHorizontal.showLine(newHorizontalLine);
    QVERIFY(!slHorizontal.isFadeOutAnimationRunning());
    QVERIFY(slHorizontal.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slHorizontal.isFadeInAnimationRunning());
    //As the line is moved by 1 pixel before display, here the line is translated for comparison
    newHorizontalLine.translate(0.0, -1.0);
    QVERIFY(newHorizontalLine == slHorizontal.line());
}

void t_hsUtils::testShowVerticalLineAtNewPositionWhileFadeOutAnimationRunning()
{
    HsSnapLine slVertical = HsSnapLine();
    QVariantHash configuration;
    configuration[Hs::snapLineFadeinDuration] = QString::number(1000);
    configuration[Hs::snapLineFadeoutDuration] = QString::number(500);

    slVertical.setConfiguration(configuration);

    //before showing the line, fade in animation should not be running
    QVERIFY(!slVertical.isFadeInAnimationRunning());
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    QLineF verticalLine(QPointF(100, 100), QPointF(100, 200));
    slVertical.showLine(verticalLine);
    QVERIFY(slVertical.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slVertical.isFadeInAnimationRunning());

    //before hiding the line, fade out animation should not be running
    QVERIFY(!slVertical.isFadeOutAnimationRunning());

    slVertical.hideLine();
    QVERIFY(slVertical.isFadeOutAnimationRunning());
    QTest::qWait(250);

    //construct a new line which is 100 pixels vertically away from previous vertical line
    QLineF newVerticalLine(QPointF(200, 100), QPointF(200, 200));
    slVertical.showLine(newVerticalLine);
    QVERIFY(!slVertical.isFadeOutAnimationRunning());
    QVERIFY(slVertical.isFadeInAnimationRunning());
    QTest::qWait(2000);
    QVERIFY(!slVertical.isFadeInAnimationRunning());
    //As the line is moved by 1 pixel before display, here the line is translated for comparison
    newVerticalLine.translate(-1.0, 0.0);
    QVERIFY(newVerticalLine == slVertical.line());
}

#endif // ONLY_MENU_TESTCASES
