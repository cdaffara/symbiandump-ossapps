/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "hsapp_defs.h"

#define SNAP_GAP_VALUE 6
#define SNAP_FORCE_VALUE 30
#define SNAP_ENABLED_VALUE true
#define SNAP_DISABLED_VALUE false

Q_DECLARE_METATYPE(QList<QRectF>)
Q_DECLARE_METATYPE(HsWidgetPositioningOnWidgetMove::Result)

void t_hsUtils::testWidgetPositioningOnWidgetMoveInstance()
{
    QVERIFY(!HsWidgetPositioningOnWidgetMove::instance());
    HsWidgetPositioningOnWidgetMove::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnWidgetMove::instance());
    HsWidgetPositioningOnWidgetMove *snapToLines = new HsSnapToLines;
    HsWidgetPositioningOnWidgetMove::setInstance(snapToLines);
    QVERIFY(HsWidgetPositioningOnWidgetMove::instance() == snapToLines);
    HsWidgetPositioningOnWidgetMove::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnWidgetMove::instance());
}

void t_hsUtils::testHsSnapToLinesConstruct()
{
    HsSnapToLines *sl = new HsSnapToLines();

    QVERIFY(sl->mContainerRect.isNull());

    QVERIFY(qFuzzyCompare(sl->mActiveRectWidth, 0));
    QVERIFY(qFuzzyCompare(sl->mActiveRectHeight, 0));

    QVERIFY(sl->mInactiveSnapRects.isEmpty());

    QVERIFY(sl->mInactiveRectToCompare.isNull());

    QVERIFY(sl->mInactiveSnapRectToCompare.rectangle.isNull());
    QVERIFY(sl->mMovingRect.isNull());

    QVERIFY(qFuzzyCompare(sl->mMinDistancePosition, 0));
    QVERIFY(qFuzzyCompare(sl->mHorizontalSnapPosition, 0));
    QVERIFY(qFuzzyCompare(sl->mVerticalSnapPosition, 0));

    QVERIFY(!sl->mHorizontalSnapFound);
    QVERIFY(sl->mVerticalLine.isNull());

    QVERIFY(!sl->mVerticalSnapFound);
    QVERIFY(sl->mHorizontalLine.isNull());

    QVERIFY(!sl->mRectLieAbove);
    QVERIFY(!sl->mLeftInRange);
    QVERIFY(!sl->mRightInRange);
    QVERIFY(!sl->mIsBetterFitHorizontalSnap);
    QVERIFY(qFuzzyCompare(sl->mDistanceVerticalEdges, 0));
    QVERIFY(qFuzzyCompare(sl->mVerticalEdgeToLeftOfInactiveRect, 0));
    QVERIFY(qFuzzyCompare(sl->mVerticalEdgeToRightOfInactiveRect, 0));
    QVERIFY(qFuzzyCompare(sl->mMinVerticalEdgesDistance, 0));
    QVERIFY(qFuzzyCompare(sl->mVerticalDistance, 0));
    QVERIFY(qFuzzyCompare(sl->mVerticalDistanceFromSelectedRect, 0));
    QVERIFY(qFuzzyCompare(sl->mContainerVerticalEdgeDistance, 0));

    QVERIFY(!sl->mRectLieLeft);
    QVERIFY(!sl->mTopInRange);
    QVERIFY(!sl->mBottomInRange);
    QVERIFY(!sl->mIsBetterFitVerticalSnap);
    QVERIFY(qFuzzyCompare(sl->mDistanceHorizontalEdges, 0));
    QVERIFY(qFuzzyCompare(sl->mHorizontalEdgeToTopOfInactiveRect, 0));
    QVERIFY(qFuzzyCompare(sl->mHorizontalEdgeToBottomOfInactiveRect, 0));
    QVERIFY(qFuzzyCompare(sl->mMinHorizontalEdgesDistance, 0));
    QVERIFY(qFuzzyCompare(sl->mHorizontalDistance, 0));
    QVERIFY(qFuzzyCompare(sl->mHorizontalDistanceFromSelectedRect, 0));
    QVERIFY(qFuzzyCompare(sl->mContainerHorizontalEdgeDistance, 0));

    QVERIFY(!sl->mSnapEnabled);
    QVERIFY(qFuzzyCompare(sl->mSnapForce, 0));
    QVERIFY(qFuzzyCompare(sl->mSnapGap, 0));

    QVERIFY(!sl->mRectVerticalEdgeLiesInLineWithVerticalLine);
    QVERIFY(!sl->mRectLieAboveVerticalLine);
    QVERIFY(!sl->mRectLieBelowVerticalLine);

    QVERIFY(!sl->mRectHorizontalEdgeLiesInLineWithHorizontalLine);
    QVERIFY(!sl->mRectLieLeftOfHorizontalLine);
    QVERIFY(!sl->mRectLiesRightOfHorizontalLine);

    delete sl;
}

void t_hsUtils::testHsSnapToLines_data()
{
    QTest::addColumn<QRectF>("containerArea");
    QTest::addColumn<QList<QRectF> >("inActiveRects");
    QTest::addColumn<QRectF>("activeRect");
    QTest::addColumn<QRectF>("movingRect");
    QTest::addColumn<HsWidgetPositioningOnWidgetMove::Result>("expectedResult");

    {
        QTest::newRow("widgetOverlap")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(250, 250, 500, 500) << QRectF(200, 200, 100, 100) << QRectF(700, 200, 100, 100)
                                << QRectF(700, 700, 100, 100) << QRectF(200, 700, 100, 100))
            << QRectF(0, 0, 800, 150)
            << QRectF(100, 100, 880, 165)
            << HsWidgetPositioningOnWidgetMove::Result();
    }

    {
        QTest::newRow("oneWidgetOutOfSnapDistance")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>())
            << QRectF(5, 5, 100, 100)
            << QRectF(3, 3, 110, 110)
            << HsWidgetPositioningOnWidgetMove::Result();
    }

    {
        HsWidgetPositioningOnWidgetMove::Result snapResultCenter;
        QTest::newRow("oneWidgetInCenterWithoutSnapping")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>())
            << QRectF(5, 5, 200, 200)
            << QRectF(390, 390, 200, 200)
            << snapResultCenter;
    }

    {
        //Data for snapping to Left Edge of Inactive Rectangle.
        HsWidgetPositioningOnWidgetMove::Result snapResultLeftEdge;
        snapResultLeftEdge.hasHorizontalSnap = true;
        snapResultLeftEdge.horizontalSnapPosition = 100;
        snapResultLeftEdge.verticalSnapLine = QLineF(100, 90, 100, 610);
        QTest::newRow("snapToLeftEdgeOfInactiveRect")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(100, 100, 100, 100) << QRectF(175, 75, 100, 150))
            << QRectF(0, 0, 500, 100)
            << QRectF(110, 500, 500, 100)
            << snapResultLeftEdge;
    }

    {
        //Data for snapping to Left Edge of Inactive Rectangle,
        //one inactive rectangle lies below the moving rectangle
        //one inactive rectangle lies above the moving rectangle. This rectangle does not offer big enough space for moving rectangle's width
        HsWidgetPositioningOnWidgetMove::Result snapResultLeftEdgeBelowRect;
        snapResultLeftEdgeBelowRect.hasHorizontalSnap = true;
        snapResultLeftEdgeBelowRect.horizontalSnapPosition = 10 - SNAP_GAP_VALUE;
        snapResultLeftEdgeBelowRect.verticalSnapLine = QLineF(560, 910, 560, 390);
        QTest::newRow("snapToLeftEdgeOfInactiveRectWhichLiesBelow")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(550, 200, 200, 150) << QRectF(560, 800, 200, 100))
            << QRectF(5, 0, 550, 100)
            << QRectF(5, 400, 550, 100)
            << snapResultLeftEdgeBelowRect;
    }

    {
        //Data for snapping to Left Edge of Inactive Rectangle.
        //The data is similar to one above, except that the rectangle can fit into the space now.
        HsWidgetPositioningOnWidgetMove::Result snapResultLeftEdgeRectLiesAbove;
        snapResultLeftEdgeRectLiesAbove.hasHorizontalSnap = true;
        snapResultLeftEdgeRectLiesAbove.horizontalSnapPosition = 50 - SNAP_GAP_VALUE;
        snapResultLeftEdgeRectLiesAbove.verticalSnapLine = QLineF(550, 510, 550, 190);
        QTest::newRow("snapToLeftEdgeOfInactiveRectWhichLiesAbove")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(560, 800, 200, 100) << QRectF(550, 200, 200, 150))
            << QRectF(5, 0, 500, 100)
            << QRectF(55, 400, 500, 100)
            << snapResultLeftEdgeRectLiesAbove;
    }

    {
        //Data for snapping to the left Edge of Inactive rect which are already alligned
        //and the bottom of already alligned widgets on the bottom
        HsWidgetPositioningOnWidgetMove::Result snapResultRectsAllignedOnLeftAndBottom;
        snapResultRectsAllignedOnLeftAndBottom.hasHorizontalSnap = true;
        snapResultRectsAllignedOnLeftAndBottom.horizontalSnapPosition = 100;
        snapResultRectsAllignedOnLeftAndBottom.verticalSnapLine = QLineF(100, 910, 100, 90);
        snapResultRectsAllignedOnLeftAndBottom.hasVerticalSnap = true;
        snapResultRectsAllignedOnLeftAndBottom.verticalSnapPosition = 820; //The width of the rectangle is 90
        snapResultRectsAllignedOnLeftAndBottom.horizontalSnapLine = QLineF(80, 910, 910, 910);
        QTest::newRow("snapToLeftAndBottomEdgeOfTwoAllignedRects")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(100, 400, 150, 125) << QRectF(100, 100, 100, 100) << QRectF(450, 820, 100, 90) << QRectF(750, 750, 150, 160))
            << QRectF(50, 750, 100, 90)
            << QRectF(90, 800, 120, 100)
            << snapResultRectsAllignedOnLeftAndBottom;
    }

    {
        //Data for snapping to the Right side of already alligned widgets on the right side
        //and top of already alligned widgets
        HsWidgetPositioningOnWidgetMove::Result snapResultRectsAllignedOnRightAndTop;
        snapResultRectsAllignedOnRightAndTop.hasHorizontalSnap = true;
        snapResultRectsAllignedOnRightAndTop.horizontalSnapPosition = 540;
        snapResultRectsAllignedOnRightAndTop.verticalSnapLine = QLineF(600, 60, 600, 910);
        snapResultRectsAllignedOnRightAndTop.hasVerticalSnap = true;
        snapResultRectsAllignedOnRightAndTop.verticalSnapPosition = 50;
        snapResultRectsAllignedOnRightAndTop.horizontalSnapLine = QLineF(40, 50, 620, 50);
        QTest::newRow("snapToRightAndTopEdgeOfTwoAllignedRects")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(300, 50, 50, 300) << QRectF(500, 500, 100, 100) << QRectF(50, 50, 150, 150) << QRectF(400, 800, 200, 100))
            << QRectF(500, 50, 60, 100)
            << QRectF(550, 70, 60, 100)
            << snapResultRectsAllignedOnRightAndTop;
    }

    {
        //Data for snapping to the left Edge of one Inactive rect and top edge of another Inactive rect
        //distance of both edges of the rectangle are at the same distance from the edge under comparison
        HsWidgetPositioningOnWidgetMove::Result snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge;
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.hasHorizontalSnap = true;
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.horizontalSnapPosition = 100;
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.verticalSnapLine = QLineF(100, 450, 100, 90);
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.hasVerticalSnap = true;
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.verticalSnapPosition = 380;
        snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge.horizontalSnapLine = QLineF(70, 380, 690, 380);
        QTest::newRow("snapToLeftEdgeAndTopEdgeOfInactiveRectDistanceToMovingRectsEdgesSame")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(100, 100, 200, 100) << QRectF(600, 380, 80, 300))
            << QRectF(0, 0, 40, 40)
            << QRectF(80, 400, 40, 40)
            << snapResultSameDistanceToMovingEdgesSnapToLeftAndTopEdge;
    }

    {
        //Data for snapping when the inactive widgets edges are equal distance from the Left and top edge of Moving Rect
        HsWidgetPositioningOnWidgetMove::Result snapResultMovingRectLeftAndTopEdgeSameDistance;
        snapResultMovingRectLeftAndTopEdgeSameDistance.hasHorizontalSnap = true;
        snapResultMovingRectLeftAndTopEdgeSameDistance.horizontalSnapPosition = 80;
        snapResultMovingRectLeftAndTopEdgeSameDistance.verticalSnapLine = QLineF(80, 90, 80, 450);
        snapResultMovingRectLeftAndTopEdgeSameDistance.hasVerticalSnap = true;
        snapResultMovingRectLeftAndTopEdgeSameDistance.verticalSnapPosition = 80;
        snapResultMovingRectLeftAndTopEdgeSameDistance.horizontalSnapLine = QLineF(90, 80, 450, 80);
        QTest::newRow("movingRectsLeftAndtopEdgeAreEqualDistanceFromInactiveRectsEdges")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(280, 400, 40, 40) << QRectF(80, 400, 40, 40) << QRectF(400, 180, 40, 40) << QRectF(400, 80, 40, 40))
            << QRectF(50, 50, 200, 100)
            << QRectF(100, 100, 200, 100)
            << snapResultMovingRectLeftAndTopEdgeSameDistance;
    }

    {
        //Data for snapping the Right and Bootom edge of moving rect which are equal distance from Inactive rects edges
        HsWidgetPositioningOnWidgetMove::Result snapResultMovingRectRightAndBottomEdgeSameDistance;
        snapResultMovingRectRightAndBottomEdgeSameDistance.hasHorizontalSnap = true;
        snapResultMovingRectRightAndBottomEdgeSameDistance.horizontalSnapPosition = 690;
        snapResultMovingRectRightAndBottomEdgeSameDistance.verticalSnapLine = QLineF(840, 190, 840, 950);
        snapResultMovingRectRightAndBottomEdgeSameDistance.hasVerticalSnap = true;
        snapResultMovingRectRightAndBottomEdgeSameDistance.verticalSnapPosition = 710;
        snapResultMovingRectRightAndBottomEdgeSameDistance.horizontalSnapLine = QLineF(190, 860, 860, 860);
        QTest::newRow("movingRectsRightAndBottomEdgeAreEqualDistanceFromInactiveRectsEdges")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(680, 200, 40, 40) << QRectF(800, 200, 40, 40) << QRectF(800, 900, 40, 40)
                                << QRectF(200, 670, 40, 40) << QRectF(200, 820, 40, 40))
            << QRectF(650, 640, 150, 150)
            << QRectF(700, 690, 150, 150)
            << snapResultMovingRectRightAndBottomEdgeSameDistance;
    }

    {
        //Data for snapping to Right Edge of Inactive Rectangle, inactive rect lies above the Moving rect,
        //distance of moving rect's right edge from the inactive widget's right edge is equal to
        //the distance of moving widget's left edge from the inactive widget's left edge.
        HsWidgetPositioningOnWidgetMove::Result snapResultRightEdge;
        snapResultRightEdge.hasHorizontalSnap = true;
        snapResultRightEdge.horizontalSnapPosition = 200;
        snapResultRightEdge.verticalSnapLine = QLineF(900, 90, 900, 610);
        QTest::newRow("snapToRightEdgeOfInactiveRect")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(725, 75, 100, 150) << QRectF(800, 100, 100, 100))
            << QRectF(225, 500, 700, 100)
            << QRectF(225, 500, 700, 100)
            << snapResultRightEdge;
    }

    {
        //Data for snapping to Top Edge of Inactive Rectangle.
        HsWidgetPositioningOnWidgetMove::Result snapResultTopEdge;
        snapResultTopEdge.hasVerticalSnap = true;
        snapResultTopEdge.verticalSnapPosition = 100;
        snapResultTopEdge.horizontalSnapLine = QLineF(90, 100, 630, 100);
        QTest::newRow("snapToTopEdgeOfInactiveRect")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(100, 100, 100, 100) << QRectF(75, 175, 150, 100) << QRectF(100, 600, 100, 100) << QRectF(75, 675, 150, 100))
            << QRectF(400, 120, 200, 400)
            << QRectF(400, 120, 220, 440)
            << snapResultTopEdge;
    }

    {
        //Data for snapping to Bottom Edge of Inactive Rectangle.
        HsWidgetPositioningOnWidgetMove::Result snapResultBottomEdge;
        snapResultBottomEdge.hasVerticalSnap = true;
        snapResultBottomEdge.verticalSnapPosition = 315;
        snapResultBottomEdge.horizontalSnapLine = QLineF(65, 800, 855, 800);
        QTest::newRow("snapToBottomEdgeOfInactiveRect")
            << QRectF(0, 0, 1000, 1000)
            << (QList<QRectF>() << QRectF(100, 100, 100, 100) << QRectF(75, 175, 150, 100) << QRectF(100, 600, 100, 100) << QRectF(75, 675, 150, 125))
            << QRectF(600, 300, 220, 485)
            << QRectF(625, 325, 220, 485)
            << snapResultBottomEdge;
    }

}

void t_hsUtils::testHsSnapToLines()
{
    QFETCH(QRectF, containerArea);
    QFETCH(QList<QRectF>, inActiveRects);
    QFETCH(QRectF, activeRect);
    QFETCH(QRectF, movingRect);
    QFETCH(HsWidgetPositioningOnWidgetMove::Result, expectedResult);

    HsWidgetPositioningOnWidgetMove::setInstance(new HsSnapToLines);

    QVariantHash snapConfiguration;
    snapConfiguration[Hs::snapEnabled] = QString::number(SNAP_ENABLED_VALUE);
    snapConfiguration[Hs::snapForce] = QString::number(SNAP_FORCE_VALUE);
    snapConfiguration[Hs::snapGap] = QString::number(SNAP_GAP_VALUE);

    HsWidgetPositioningOnWidgetMove::instance()->setConfiguration(snapConfiguration);
    HsWidgetPositioningOnWidgetMove::instance()->setPagePresentation(containerArea, inActiveRects, activeRect);
    HsWidgetPositioningOnWidgetMove::Result convertedResult = HsWidgetPositioningOnWidgetMove::instance()->run(movingRect);

    QVERIFY(hsResultCompare(convertedResult, expectedResult));
}

void t_hsUtils::testHsSnapToLinesSnapDisabled_data()
{
    QTest::addColumn<QRectF>("containerArea");
    QTest::addColumn< QList<QRectF> >("inActiveRects");
    QTest::addColumn<QRectF>("activeRect");
    QTest::addColumn<QRectF>("movingRect");
    QTest::addColumn<HsWidgetPositioningOnWidgetMove::Result>("expectedResult");

    QTest::newRow("SnapDisabled")
        << QRectF(0, 0, 1000, 1000)
        << (QList<QRectF>())
        << QRectF(5, 5, 200, 200)
        << QRectF(380, 380, 200, 200)
        << HsWidgetPositioningOnWidgetMove::Result();
}

void t_hsUtils::testHsSnapToLinesSnapDisabled()
{
    QFETCH(QRectF, containerArea);
    QFETCH(QList<QRectF>, inActiveRects);
    QFETCH(QRectF, activeRect);
    QFETCH(QRectF, movingRect);
    QFETCH(HsWidgetPositioningOnWidgetMove::Result, expectedResult);

    HsWidgetPositioningOnWidgetMove::setInstance(new HsSnapToLines);

    QVariantHash snapConfiguration;
    snapConfiguration[Hs::snapEnabled] = QString::number(SNAP_DISABLED_VALUE);

    HsWidgetPositioningOnWidgetMove::instance()->setConfiguration(snapConfiguration);
    HsWidgetPositioningOnWidgetMove::instance()->setPagePresentation(containerArea, inActiveRects, activeRect);
    HsWidgetPositioningOnWidgetMove::Result convertedResult = HsWidgetPositioningOnWidgetMove::instance()->run(movingRect);

    QVERIFY(hsResultCompare(convertedResult, expectedResult));
}

bool t_hsUtils::hsLineCompare(QLineF l1, QLineF l2)
{
    if ((l1.p1() == l2.p1() || l1.p1() == l2.p2())
        && (l1.p2() == l2.p1() || l1.p2() == l2.p2())) {
        return true;
    }
    else {
        return false;
    }
}

bool t_hsUtils::hsResultCompare(const HsWidgetPositioningOnWidgetMove::Result& sr1, const HsWidgetPositioningOnWidgetMove::Result& sr2)
{
    bool resultsAreSame = false;
    bool horizontalLinesSame = false;
    bool verticalLinesSame = false;

    qDebug() << "Converted Result = " <<sr1.hasHorizontalSnap <<sr1.horizontalSnapPosition<<sr1.hasVerticalSnap<<sr1.verticalSnapPosition;
    qDebug() << "Expectted Result = " <<sr2.hasHorizontalSnap <<sr2.horizontalSnapPosition<<sr2.hasVerticalSnap<<sr2.verticalSnapPosition;

    if ((sr1.hasHorizontalSnap == sr2.hasHorizontalSnap)
        && (sr1.horizontalSnapPosition == sr2.horizontalSnapPosition)
        && (sr1.hasVerticalSnap == sr2.hasVerticalSnap)
        && (sr1.verticalSnapPosition == sr2.verticalSnapPosition)) {
        //Check if Horizontal Snap Position Exists and compare the vertical line
        if (sr1.hasHorizontalSnap) {
            QLineF l1 = sr1.verticalSnapLine;
            QLineF l2 = sr2.verticalSnapLine;
            verticalLinesSame = hsLineCompare(l1, l2);
            qDebug() << "Converted Vertical Line = " << l1.x1() << l1.y1() << l1.x2() << l1.y2();
            qDebug() << "Expected Vertical Line = " << l2.x1() << l2.y1() << l2.x2() << l2.y2();
            qDebug() << "Vertical lines are same = " << verticalLinesSame;
        }
        else {
            verticalLinesSame = true;
        }

        //Check if Vertical Snap Position Exists and compare the horizontal line
        if (sr1.hasVerticalSnap) {
            QLineF l1 = sr1.horizontalSnapLine;
            QLineF l2 = sr2.horizontalSnapLine;
            horizontalLinesSame = hsLineCompare(l1, l2);
            qDebug() << "Converted Horizontal Line = " << l1.x1() << l1.y1() << l1.x2() << l1.y2();
            qDebug() << "Expected Horizontal Line = " << l2.x1() << l2.y1() << l2.x2() << l2.y2();
            qDebug() << "Horizontal lines are same = " << horizontalLinesSame;
        }
        else {
            horizontalLinesSame = true;
        }

        if (horizontalLinesSame && verticalLinesSame) {
            resultsAreSame = true;
        }
    }

    if ( !resultsAreSame) {
        qDebug() << "results are not the same!!!";
    }
    return resultsAreSame;
}
#endif
