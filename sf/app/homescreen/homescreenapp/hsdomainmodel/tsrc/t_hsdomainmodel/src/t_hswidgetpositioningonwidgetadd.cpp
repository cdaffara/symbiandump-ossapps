/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <HbMainWindow>
#include <HbInstance>

#include "t_hsdomainmodel.h"
#include "hsconfiguration.h"
#include "hsgui.h"
#include "hswidgetpositioningonwidgetadd.h"

inline bool hsRectCompare(const QRectF& p1, const QRectF& p2)
{
    const qreal offset = 0.1;
    bool rectsAreSame = (qAbs(p1.x()-p2.x()) < offset) && (qAbs(p1.y()-p2.y()) < offset) && (qAbs(p1.width()-p2.width()) < offset) && (qAbs(p1.height()-p2.height()) < offset);
    if ( !rectsAreSame) {
        qDebug() << "rects are not the same: actual" << p1 << "expected" << p2;
    }
    return rectsAreSame;
}

Q_DECLARE_METATYPE(QList<QRectF>)

void TestHsDomainModel::testWidgetPositioningOnWidgetAddChangeInstance()
{
    QVERIFY(!HsWidgetPositioningOnWidgetAdd::instance());
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnWidgetAdd::instance());
    HsWidgetPositioningOnWidgetAdd *c = new HsAnchorPointInBottomRight;
    HsWidgetPositioningOnWidgetAdd::setInstance(c);
    QVERIFY(HsWidgetPositioningOnWidgetAdd::instance() == c);
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnWidgetAdd::instance());
}

void TestHsDomainModel::testHsAnchorPointInBottomRight_data()
{
    QTest::addColumn<QRectF>("contentArea");
    QTest::addColumn<QList<QRectF> >("widgets");
    QTest::addColumn<QList<QRectF> >("expectedWidgets");

    QTest::newRow("square")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(50, 50, 10, 10) << QRectF(50, 50, 20, 10) << QRectF(50, 50, 10, 10))
        << (QList<QRectF>() << QRectF(45, 45, 10, 10) << QRectF(30.8579, 30.8579, 10, 10) << QRectF(6.71573, 16.7157, 20, 10) << QRectF(2.57359, 2.57359, 10, 10));

    QTest::newRow("portrait")
        << QRectF(0, 0, 50, 100)
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(50, 50, 10, 10) << QRectF(50, 50, 10, 20) << QRectF(50, 50, 10, 10))
        << (QList<QRectF>() << QRectF(20, 50, 10, 10) << QRectF(8.45299, 26.906, 10, 10) << QRectF(40,80, 10, 20) << QRectF(28.453, 66.906, 10, 10));

    QTest::newRow("landscape")
        << QRectF(0, 0, 100, 50)
        << (QList<QRectF>() << QRectF(0, 0, 20, 10) << QRectF(50, 50, 20, 10) << QRectF(50, 50, 10, 20) << QRectF(50, 50, 10, 10))
        << (QList<QRectF>() << QRectF(40, 20, 20, 10) << QRectF(23.6701, 11.835, 20, 10) << QRectF(90, 30, 10, 20) << QRectF(73.6701, 31.835, 10, 10));

    QTest::newRow("out of rect")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(0, 0, 200, 200) )
        << (QList<QRectF>() << QRectF(0, 0, 200, 200) );
}

void TestHsDomainModel::testHsAnchorPointInBottomRight()
{
    QFETCH(QRectF, contentArea);
    QFETCH(QList<QRectF>, widgets);
    QFETCH(QList<QRectF>, expectedWidgets);

    HsWidgetPositioningOnWidgetAdd *converter =
        new HsAnchorPointInBottomRight;

    HsWidgetPositioningOnWidgetAdd::Result result;
    result = converter->convert(contentArea, QList<QRectF>(),
                                widgets, QPointF());

    QCOMPARE(result.calculatedRects.count(),
             expectedWidgets.count());

    for (int i = 0; i < expectedWidgets.count(); ++i) {
        QVERIFY(hsRectCompare(result.calculatedRects.at(i),expectedWidgets.at(i)));
    }

    delete converter;
}

void TestHsDomainModel::testHsWidgetOrganizerCase1_data()
{
    // Case 1: anchorDistance = 5, searchRowByRow, portrait
    QTest::addColumn<QRectF>("contentArea");
    QTest::addColumn<QList<QRectF> >("existingWidgets");
    QTest::addColumn<QList<QRectF> >("widgets");
    QTest::addColumn<QList<QRectF> >("expectedWidgets");

    // empty parameters
    QTest::newRow("case1.1")
        << QRectF() << QList<QRectF>() << QList<QRectF>() << QList<QRectF>();

    // invalid parameters
    QTest::newRow("case1.2")
        << QRectF(54, 763, 24, 5434)
        << (QList<QRectF>() << QRectF(343, 321, 1034, 102) << QRectF(3320, 3420, 1430, 1043) << QRectF(0, 43, 10, 997))
        << (QList<QRectF>() << QRectF(322, 12, 835, 623) << QRectF(624, 56, 536, 895) << QRectF(31, 423, 32, 9823))
        << QList<QRectF>();

    // basic set of similar size rects to simple content area's first row
    QTest::newRow("case1.3")
        << QRectF(0, 0, 100, 100) << QList<QRectF>()
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10))
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(15, 0, 10, 10) << QRectF(30, 0, 10, 10));

    // set of similar height rects to simple content area on multiple rows
    QTest::newRow("case1.4")
        << QRectF(0, 0, 100, 100) << QList<QRectF>()
        << (QList<QRectF>() << QRectF(0, 0, 40, 10) << QRectF(0, 0, 50, 10) << QRectF(0, 0, 95, 10) << QRectF(0, 0, 50, 10) << QRectF(0, 0, 10, 10))
        << (QList<QRectF>() << QRectF(0, 0, 40, 10) << QRectF(45, 0, 50, 10) << QRectF(0, 15, 95, 10) << QRectF(0, 30, 50, 10) << QRectF(55, 30, 10, 10));

    // set of random size rects to simple content area on multiple rows
    QTest::newRow("case1.5")
        << QRectF(0, 0, 100, 100) << QList<QRectF>()
        << (QList<QRectF>() << QRectF(0, 0, 13, 22) << QRectF(0, 0, 59, 39) << QRectF(0, 0, 93, 8) << QRectF(0, 0, 44, 17) << QRectF(0, 0, 22, 31))
        << (QList<QRectF>() << QRectF(85, 0, 13, 22) << QRectF(0, 0, 59, 39) << QRectF(0, 60, 93, 8) << QRectF(0, 40, 44, 17) << QRectF(60, 0, 22, 31));

    // set of random size rects to simple content area on multiple rows with existing widgets
    QTest::newRow("case1.6")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(42, 16, 14, 11) << QRectF(9, 12, 13, 62) << QRectF(55, 65, 20, 20) << QRectF(70, 20, 13, 11) << QRectF(30, 90, 22, 8))
        << (QList<QRectF>() << QRectF(0, 0, 13, 22) << QRectF(0, 0, 42, 34) << QRectF(0, 0, 54, 8) << QRectF(0, 0, 32, 17) << QRectF(0, 0, 27, 24))
        << (QList<QRectF>() << QRectF(25, 0, 13, 22) << QRectF(25, 30, 42, 34) << QRectF(0, 75, 54, 8) << QRectF(60, 0, 32, 17) << QRectF(70, 35, 27, 24));

}

void TestHsDomainModel::testHsWidgetOrganizerCase1()
{
    QFETCH(QRectF, contentArea);
    QFETCH(QList<QRectF>, existingWidgets);
    QFETCH(QList<QRectF>, widgets);
    QFETCH(QList<QRectF>, expectedWidgets);

    HsWidgetOrganizer *widgetOrganizer =
        new HsWidgetOrganizer(5, HsConfiguration::SearchRowByRow);

    HsWidgetPositioningOnWidgetAdd::Result result;
    result = widgetOrganizer->convert(contentArea, existingWidgets,
                                      widgets, QPointF());
    // Verify amount
    QCOMPARE(result.calculatedRects.count(), expectedWidgets.count());
    // Compare calculated and expected values
    for (int i = 0; i < expectedWidgets.count(); ++i) {
        QVERIFY(hsRectCompare(result.calculatedRects.at(i), expectedWidgets.at(i)));
    }

    delete widgetOrganizer;
}

void TestHsDomainModel::testHsWidgetOrganizerCase2_data()
{
    // CASE 2: anchorDistance = 5, searchColumnByColumn, landscape
    QTest::addColumn<QRectF>("contentArea");
    QTest::addColumn<QList<QRectF> >("existingWidgets");
    QTest::addColumn<QList<QRectF> >("widgets");
    QTest::addColumn<QList<QRectF> >("expectedWidgets");

    // basic set of similar size rects to simple content area's first column
    QTest::newRow("case2.1")
        << QRectF(0, 0, 100, 100) << QList<QRectF>()
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10))
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(0, 15, 10, 10) << QRectF(0, 30, 10, 10));

    // set of random size rects to simple content area on multiple columns with existing widgets
    QTest::newRow("case2.2")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(20, 0, 14, 18) << QRectF(68, 0, 13, 21) << QRectF(32, 30, 14, 53) << QRectF(62, 60, 31, 13) << QRectF(90, 33, 9, 8))
        << (QList<QRectF>() << QRectF(0, 0, 43, 12) << QRectF(0, 0, 20, 16) << QRectF(0, 0, 14, 25) << QRectF(0, 0, 22, 47) << QRectF(0, 0, 24, 11))
        << (QList<QRectF>() << QRectF(15, 85, 43, 12) << QRectF(35, 0, 20, 16) << QRectF(0, 70, 14, 25) << QRectF(0, 20, 22, 47) << QRectF(50, 25, 24, 11));
}

void TestHsDomainModel::testHsWidgetOrganizerCase2()
{
    QFETCH(QRectF, contentArea);
    QFETCH(QList<QRectF>, existingWidgets);
    QFETCH(QList<QRectF>, widgets);
    QFETCH(QList<QRectF>, expectedWidgets);

    mainWindow.setOrientation(Qt::Horizontal);
    HsWidgetOrganizer *widgetOrganizer =
        new HsWidgetOrganizer(5, HsConfiguration::SearchColumnByColumn);

    HsWidgetPositioningOnWidgetAdd::Result result;
    result = widgetOrganizer->convert(contentArea, existingWidgets,
                                      widgets, QPointF());
    QCOMPARE(result.calculatedRects.count(), expectedWidgets.count());
    for (int i = 0; i < expectedWidgets.count(); ++i) {
        QVERIFY(hsRectCompare(result.calculatedRects.at(i), expectedWidgets.at(i)));
    }

    delete widgetOrganizer;
    // set orientation back to normal
    mainWindow.setOrientation(Qt::Vertical);
}

void TestHsDomainModel::testHsWidgetOrganizerCase3_data()
{
    // CASE 3: anchorDistance = 1, searchRowByRow, portrait
    QTest::addColumn<QRectF>("contentArea");
    QTest::addColumn<QList<QRectF> >("existingWidgets");
    QTest::addColumn<QList<QRectF> >("widgets");
    QTest::addColumn<QList<QRectF> >("expectedWidgets");

    // basic set of similar size rects to simple content area's first column
    QTest::newRow("case3.1")
        << QRectF(0, 0, 100, 100) << QList<QRectF>()
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10))
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(11, 0, 10, 10) << QRectF(22, 0, 10, 10));

    // set of random size rects to simple content area on multiple columns with existing widgets
    QTest::newRow("case3.2")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(10, 0, 14, 22) << QRectF(64, 0, 14, 34) << QRectF(0, 54, 34, 14) << QRectF(74, 69, 26, 12) << QRectF(41, 80, 11, 20))
        << (QList<QRectF>() << QRectF(0, 0, 46, 16) << QRectF(0, 0, 40, 19) << QRectF(0, 0, 38, 53) << QRectF(0, 0, 20, 40) << QRectF(0, 0, 38, 25))
        << (QList<QRectF>() << QRectF(53, 82, 46, 16) << QRectF(0, 80, 40, 19) << QRectF(25, 0, 38, 53) << QRectF(79, 0, 20, 40) << QRectF(35, 54, 38, 25));

}

void TestHsDomainModel::testHsWidgetOrganizerCase3()
{
    QFETCH(QRectF, contentArea);
    QFETCH(QList<QRectF>, existingWidgets);
    QFETCH(QList<QRectF>, widgets);
    QFETCH(QList<QRectF>, expectedWidgets);

    HsWidgetOrganizer *widgetOrganizer =
        new HsWidgetOrganizer(1, HsConfiguration::SearchRowByRow);

    HsWidgetPositioningOnWidgetAdd::Result result;
    result = widgetOrganizer->convert(contentArea, existingWidgets,
                                                widgets, QPointF());
    QCOMPARE(result.calculatedRects.count(), expectedWidgets.count());
    for (int i = 0; i < expectedWidgets.count(); ++i) {
        QVERIFY(hsRectCompare(result.calculatedRects.at(i), expectedWidgets.at(i)));
    }

    delete widgetOrganizer;
}

#endif // ONLY_MENU_TESTCASES
