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

#include <HbDeviceProfile>

#include "hsconfiguration.h"

HsConfiguration::HsConfiguration(QObject *parent)
  : QObject(parent),
    mBounceEffect(20),
    mPageChangeAnimationDuration(200),
    mPageChangeZoneAnimationDuration(800),
    mPageChangeZoneReverseAnimationDuration(200),
    mPageRemovedAnimationDuration(200),
    mNewPageAddedAnimationDuration(200),
    mWidgetDragEffectDuration(200),
    mWidgetDropEffectDuration(200),
    mDefaultPageId(1),
    mMaximumPageCount(8),
    mIsShortcutLabelVisible(true),
    mBounceFeedbackEffectDistance(3),
    mPageChangeZoneWidth(60),
    mPageIndicatorSpacing(8),
    mMaximumWidgetHeight(39),
    mMaximumWidgetWidth(48),
    mMinimumWidgetHeight(8.75),
    mMinimumWidgetWidth(8.75),
    mPageChangePanDistance(17.91),
    mPageChangeFeedbackEffect(HbFeedback::Sensitive),
    mWidgetPickFeedbackEffect(HbFeedback::ItemPick),
    mWidgetDropFeedbackEffect(HbFeedback::ItemDrop),
    mWidgetRepositionFeedbackEffect(HbFeedback::BounceEffect),
    mWidgetOverTrashbinFeedbackEffect(HbFeedback::ItemMoveOver),
    mWidgetDropToTrashbinFeedbackEffect(HbFeedback::ItemDrop),
    mShortcutWidgetTapFeedbackEffect(HbFeedback::BasicItem),
    mWidgetMoveBlockedFeedbackEffect(HbFeedback::Basic),
    mClockWidgetTapFeedbackEffect(HbFeedback::BasicItem),
    mWidgetSnappingFeedbackEffect(HbFeedback::ItemMoveOver),
    mIsSnapEnabled(true),
    mSnapTimeout(100),
    mSnapForce(30),
    mSnapGap(6),
    mPageMargin(0),
    mSnapLineFadeInDuration(200),
    mSnapLineFadeOutDuration(100),
    mIsSnapEffectsEnabled(true),    
    mSceneType(PageWallpapers),
    mBounceAnimationEasingCurve(QEasingCurve::Linear),
    mPageChangeAnimationEasingCurve(QEasingCurve::Linear)
{    
}

HsConfiguration::~HsConfiguration()
{
}

QSizeF HsConfiguration::minimumWidgetSizeInUnits() const
{
    return QSizeF(mMinimumWidgetWidth, mMinimumWidgetHeight);
}

QSizeF HsConfiguration::maximumWidgetSizeInUnits() const
{
    return QSizeF(mMaximumWidgetWidth, mMaximumWidgetHeight);
}

QSizeF HsConfiguration::minimumWidgetSizeInPixels() const
{
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    return factor * minimumWidgetSizeInUnits();
}

QSizeF HsConfiguration::maximumWidgetSizeInPixels() const
{
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    return factor * maximumWidgetSizeInUnits();
}

bool HsConfiguration::load()
{
    return true;
}

bool HsConfiguration::importFromFile(const QString &path)
{
    Q_UNUSED(path);
    return true;
}

bool HsConfiguration::exportToFile(const QString &path)
{
    Q_UNUSED(path);
    return true;
}

HsConfiguration *HsConfiguration::instance()
{
    return mInstance;
}

HsConfiguration *HsConfiguration::takeInstance()
{
    HsConfiguration *instance = mInstance;
    mInstance = 0;
    return instance;
}

void HsConfiguration::setInstance(HsConfiguration *instance)
{
    if (mInstance != instance) {
        delete mInstance;
        mInstance = instance;
    }
}

HsConfiguration *HsConfiguration::mInstance(0);
