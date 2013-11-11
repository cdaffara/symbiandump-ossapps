/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution; and is available
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

#include <HbDeviceProfile>

#include "t_hsdomainmodel.h"
#include "hsdomainmodeldatastructures.h"
#include "hsconfiguration.h"


void TestHsDomainModel::testConfigurationConstruction()
{
    HsConfiguration *c = new HsConfiguration;

    QCOMPARE(c->mBounceEffect, 20);    
    QCOMPARE(c->mPageChangeAnimationDuration, 300);
    QCOMPARE(c->mPageChangeZoneAnimationDuration, 600);
    QCOMPARE(c->mPageChangeZoneReverseAnimationDuration, 200);
    QCOMPARE(c->mPageRemovedAnimationDuration, 1000);
    QCOMPARE(c->mNewPageAddedAnimationDuration, 1000);
    QCOMPARE(c->mWidgetDragEffectDuration, 250);
    QCOMPARE(c->mWidgetDropEffectDuration, 200);
    QCOMPARE(c->mDefaultPageId, 1);
    QCOMPARE(c->mMaximumPageCount, 8);
    QCOMPARE(c->mIsShortcutLabelVisible, true);
    QCOMPARE(c->mBounceFeedbackEffectDistance, qreal(3));
    QCOMPARE(c->mPageChangeZoneWidth, qreal(30));
    QCOMPARE(c->mPageIndicatorSpacing, qreal(8));
    QCOMPARE(c->mMaximumWidgetHeight, qreal(39));
    QCOMPARE(c->mMaximumWidgetWidth, qreal(48));
    QCOMPARE(c->mMinimumWidgetHeight, qreal(8.75));
    QCOMPARE(c->mMinimumWidgetWidth, qreal(8.75));
    QCOMPARE(c->mPageChangePanDistance, qreal(120));
    QCOMPARE(c->mPageChangeFeedbackEffect, HbFeedback::Sensitive);
    QCOMPARE(c->mWidgetPickFeedbackEffect, HbFeedback::ItemPick);
    QCOMPARE(c->mWidgetDropFeedbackEffect, HbFeedback::ItemDrop);
    QCOMPARE(c->mWidgetRepositionFeedbackEffect, HbFeedback::BounceEffect);
    QCOMPARE(c->mWidgetOverTrashbinFeedbackEffect, HbFeedback::ItemMoveOver);
    QCOMPARE(c->mWidgetDropToTrashbinFeedbackEffect, HbFeedback::ItemDrop);
    QCOMPARE(c->mShortcutWidgetTapFeedbackEffect, HbFeedback::BasicItem);
    QCOMPARE(c->mWidgetMoveBlockedFeedbackEffect, HbFeedback::Basic);
    QCOMPARE(c->mClockWidgetTapFeedbackEffect, HbFeedback::BasicItem);
    QCOMPARE(c->mWidgetSnappingFeedbackEffect, HbFeedback::ItemMoveOver);
    QCOMPARE(c->mIsSnapEnabled, true);
    QCOMPARE(c->mSnapTimeout, 300);
    QCOMPARE(c->mSnapForce, qreal(30));
    QCOMPARE(c->mSnapGap, qreal(6));
    QCOMPARE(c->mPageMargin, qreal(0));
    QCOMPARE(c->mSnapLineFadeInDuration, 100);
    QCOMPARE(c->mSnapLineFadeOutDuration, 100);
    QCOMPARE(c->mSceneType, HsConfiguration::PageWallpapers);

    delete c;
}

void TestHsDomainModel::testConfigurationStaticInterface()
{
    HsConfiguration *stored = HsConfiguration::takeInstance();

    HsConfiguration *c = new HsConfiguration;
    QVERIFY(!HsConfiguration::instance());
    HsConfiguration::setInstance(c);
    QVERIFY(HsConfiguration::instance() == c);
    HsConfiguration::setInstance(c);
    QVERIFY(HsConfiguration::instance() == c);
    HsConfiguration *c2 = HsConfiguration::takeInstance();
    QVERIFY(!HsConfiguration::instance());
    QVERIFY(c == c2);
    delete c;

    HsConfiguration::setInstance(stored);
}

void TestHsDomainModel::testConfigurationSizes()
{
    HsConfiguration *c = new HsConfiguration;

    QSizeF min1(c->mMinimumWidgetWidth, c->mMinimumWidgetHeight);
    QSizeF max1(c->mMaximumWidgetWidth, c->mMaximumWidgetHeight);

    QSizeF min2(c->minimumWidgetSizeInUnits());
    QSizeF max2(c->maximumWidgetSizeInUnits());

    QVERIFY(qFuzzyCompare(min1.width(), min2.width()));
    QVERIFY(qFuzzyCompare(min1.height(), min2.height()));
    QVERIFY(qFuzzyCompare(max1.width(), max2.width()));
    QVERIFY(qFuzzyCompare(max1.height(), max2.height()));

    HbDeviceProfile profile;
    qreal factor = profile.unitValue();

    min1 *= factor;
    max1 *= factor;

    min2 = c->minimumWidgetSizeInPixels();
    max2 = c->maximumWidgetSizeInPixels();

    QVERIFY(qFuzzyCompare(min1.width(), min2.width()));
    QVERIFY(qFuzzyCompare(min1.height(), min2.height()));
    QVERIFY(qFuzzyCompare(max1.width(), max2.width()));
    QVERIFY(qFuzzyCompare(max1.height(), max2.height()));

    delete c;
}

void TestHsDomainModel::testConfigurationImportAndExport()
{
    HsConfiguration *c = new HsConfiguration;

    QVERIFY(!c->exportToFile(QString()));
    QVERIFY(!c->importFromFile(QString()));

    QVERIFY(c->exportToFile("test_hsconfiguration.txt"));
    QVERIFY(c->importFromFile("test_hsconfiguration.txt"));

    QFile::remove("test_hsconfiguration.txt");

    QCOMPARE(c->mBounceEffect, 20);
    QCOMPARE(c->mPageChangeAnimationDuration, 300);
    QCOMPARE(c->mPageChangeZoneAnimationDuration, 600);
    QCOMPARE(c->mPageChangeZoneReverseAnimationDuration, 200);
    QCOMPARE(c->mPageRemovedAnimationDuration, 1000);
    QCOMPARE(c->mNewPageAddedAnimationDuration, 1000);
    QCOMPARE(c->mWidgetDragEffectDuration, 250);
    QCOMPARE(c->mWidgetDropEffectDuration, 200);
    QCOMPARE(c->mDefaultPageId, 1);
    QCOMPARE(c->mMaximumPageCount, 8);
    QCOMPARE(c->mIsShortcutLabelVisible, true);
    QCOMPARE(c->mBounceFeedbackEffectDistance, qreal(3));
    QCOMPARE(c->mPageChangeZoneWidth, qreal(30));
    QCOMPARE(c->mPageIndicatorSpacing, qreal(8));
    QCOMPARE(c->mMaximumWidgetHeight, qreal(39));
    QCOMPARE(c->mMaximumWidgetWidth, qreal(48));
    QCOMPARE(c->mMinimumWidgetHeight, qreal(8.75));
    QCOMPARE(c->mMinimumWidgetWidth, qreal(8.75));
    QCOMPARE(c->mPageChangePanDistance, qreal(120));
    QCOMPARE(c->mPageChangeFeedbackEffect, HbFeedback::Sensitive);
    QCOMPARE(c->mWidgetPickFeedbackEffect, HbFeedback::ItemPick);
    QCOMPARE(c->mWidgetDropFeedbackEffect, HbFeedback::ItemDrop);
    QCOMPARE(c->mWidgetRepositionFeedbackEffect, HbFeedback::BounceEffect);
    QCOMPARE(c->mWidgetOverTrashbinFeedbackEffect, HbFeedback::ItemMoveOver);
    QCOMPARE(c->mWidgetDropToTrashbinFeedbackEffect, HbFeedback::ItemDrop);
    QCOMPARE(c->mShortcutWidgetTapFeedbackEffect, HbFeedback::BasicItem);
    QCOMPARE(c->mWidgetMoveBlockedFeedbackEffect, HbFeedback::Basic);
    QCOMPARE(c->mClockWidgetTapFeedbackEffect, HbFeedback::BasicItem);
    QCOMPARE(c->mWidgetSnappingFeedbackEffect, HbFeedback::ItemMoveOver);
    QCOMPARE(c->mIsSnapEnabled, true);
    QCOMPARE(c->mSnapTimeout, 300);
    QCOMPARE(c->mSnapForce, qreal(30));
    QCOMPARE(c->mSnapGap, qreal(6));
    QCOMPARE(c->mPageMargin, qreal(0));
    QCOMPARE(c->mSnapLineFadeInDuration, 100);
    QCOMPARE(c->mSnapLineFadeOutDuration, 100);
    QCOMPARE(c->mSceneType, HsConfiguration::PageWallpapers);

    delete c;
}

void TestHsDomainModel::testConfigurationLoad()
{
    HsConfiguration *c = new HsConfiguration;
    QVERIFY(c->load());
    delete c;
}
