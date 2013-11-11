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
#include "hswidgetpositioningonorientationchange.h"

Q_DECLARE_METATYPE(QList<QRectF>)

void t_hsUtils::testWidgetPositioningOnOrientationChangeInstance()
{
    QVERIFY(!HsWidgetPositioningOnOrientationChange::instance());
    HsWidgetPositioningOnOrientationChange::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnOrientationChange::instance());
    HsWidgetPositioningOnOrientationChange *c = new HsAdvancedWidgetPositioningOnOrientationChange;
    HsWidgetPositioningOnOrientationChange::setInstance(c);
    QVERIFY(HsWidgetPositioningOnOrientationChange::instance() == c);
    HsWidgetPositioningOnOrientationChange::setInstance(0);
    QVERIFY(!HsWidgetPositioningOnOrientationChange::instance());
}

void t_hsUtils::testAdvancedWidgetPositioningOnOrientationChange_data()
{
    QTest::addColumn<QRectF>("fromRect");
    QTest::addColumn<QList<QRectF> >("fromGeometries");
    QTest::addColumn<QRectF>("toRect");
    QTest::addColumn<QList<QRectF> >("toGeometries");

    QTest::newRow("portrait to landscape1")
        << QRectF(0, 0, 100, 200)
        << (QList<QRectF>() << QRectF(0, 100, 10, 10) << QRectF(90, 0, 10, 10) << QRectF(90, 190, 10, 10) << QRectF(50, 150, 10, 10))
        << QRectF(0, 0, 200, 100)
        << (QList<QRectF>() << QRectF(100, 0, 10, 10) << QRectF(90, 0, 10, 10) << QRectF(190, 90, 10, 10) << QRectF(150, 50, 10, 10));

    QTest::newRow("portrait to landscape2")
        << QRectF(0, 0, 100, 200)
        << (QList<QRectF>() << QRectF(0, 94, 10, 10) << QRectF(0, 95, 10, 10) << QRectF(0, 96, 10, 10) << QRectF(-5, 95, 10, 10) << QRectF(-5, 96, 10, 10))
        << QRectF(0, 0, 200, 100)
        << (QList<QRectF>() << QRectF(0, 90, 10, 10) << QRectF(100, 0, 10, 10) << QRectF(100, 0, 10, 10) << QRectF(95, 0, 10, 10) << QRectF(95, 0, 10, 10));

    QTest::newRow("landscape to portrait1")
        << QRectF(0, 0, 200, 100)
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(100, 0, 10, 10) << QRectF(90, 0, 10, 10) << QRectF(190, 90, 10, 10) << QRectF(150, 50, 10, 10))
        << QRectF(0, 0, 100, 200)
        << (QList<QRectF>() << QRectF(0, 0, 10, 10) << QRectF(90, 100, 10, 10) << QRectF(90, 0, 10, 10) << QRectF(90, 190, 10, 10) << QRectF(90, 150, 10, 10));

    QTest::newRow("landscape to portrait2")
        << QRectF(0, 0, 200, 100)
        << (QList<QRectF>() << QRectF(94, 0, 10, 10) << QRectF(95, 0, 10, 10) << QRectF(96, 0, 10, 10) << QRectF(95, -5, 10, 10) << QRectF(96, -5, 10, 10))
        << QRectF(0, 0, 100, 200)
        << (QList<QRectF>() << QRectF(90, 0, 10, 10) << QRectF(90, 100, 10, 10) << QRectF(90, 100, 10, 10) << QRectF(90, 95, 10, 10) << QRectF(90, 95, 10, 10));

    QTest::newRow("illegal from points")
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(-50, -50, 10, 10))
        << QRectF(0, 0, 100, 100)
        << (QList<QRectF>() << QRectF(0, 0, 10, 10));

}

void t_hsUtils::testAdvancedWidgetPositioningOnOrientationChange()
{
    QFETCH(QRectF, fromRect);
    QFETCH(QList<QRectF>, fromGeometries);
    QFETCH(QRectF, toRect);
    QFETCH(QList<QRectF>, toGeometries);

    HsWidgetPositioningOnOrientationChange *converter =
        new HsAdvancedWidgetPositioningOnOrientationChange;
    QList<QRectF> convertedGeometries =
        converter->convert(fromRect, fromGeometries, toRect);

    QCOMPARE(convertedGeometries.count(), toGeometries.count());

    for (int i = 0; i < toGeometries.count(); ++i) {
        QVERIFY(qFuzzyCompare(1 + toGeometries.at(i).x(), 1 + convertedGeometries.at(i).x()));
        QVERIFY(qFuzzyCompare(1 + toGeometries.at(i).y(), 1 + convertedGeometries.at(i).y()));
        QVERIFY(qFuzzyCompare(1 + toGeometries.at(i).width(), 1 + convertedGeometries.at(i).width()));
        QVERIFY(qFuzzyCompare(1 + toGeometries.at(i).height(), 1 + convertedGeometries.at(i).height()));
    }

    delete converter;
}

#endif // ONLY_MENU_TESTCASES
