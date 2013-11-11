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

#ifndef HSCONFIGURATION_H
#define HSCONFIGURATION_H

#include <QObject>
#include <QVariant>
#include <QEasingCurve>

#include <HbFeedback>

#include "hsdomainmodel_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

#define HSCONFIGURATION_GET(getter) \
    HsConfiguration::instance()->getter()
#define HSCONFIGURATION_SET(setter, value) \
    HsConfiguration::instance()->setter(value)

class HSDOMAINMODEL_EXPORT HsConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int bounceEffect READ bounceEffect WRITE setBounceEffect)
    Q_PROPERTY(int pageChangeAnimationDuration READ pageChangeAnimationDuration WRITE setPageChangeAnimationDuration)
    Q_PROPERTY(int pageChangeZoneAnimationDuration READ pageChangeZoneAnimationDuration WRITE setPageChangeZoneAnimationDuration)
    Q_PROPERTY(int pageChangeZoneReverseAnimationDuration READ pageChangeZoneReverseAnimationDuration WRITE setPageChangeZoneReverseAnimationDuration)
    Q_PROPERTY(int pageRemovedAnimationDuration READ pageRemovedAnimationDuration WRITE setPageRemovedAnimationDuration)
    Q_PROPERTY(int newPageAddedAnimationDuration READ newPageAddedAnimationDuration WRITE setNewPageAddedAnimationDuration)
    Q_PROPERTY(int widgetDragEffectDuration READ widgetDragEffectDuration WRITE setWidgetDragEffectDuration)
    Q_PROPERTY(int widgetDropEffectDuration READ widgetDropEffectDuration WRITE setWidgetDropEffectDuration)
    Q_PROPERTY(int defaultPageId READ defaultPageId WRITE setDefaultPageId)
    Q_PROPERTY(int maximumPageCount READ maximumPageCount WRITE setMaximumPageCount)
    Q_PROPERTY(bool isShortcutLabelVisible READ isShortcutLabelVisible WRITE setShortcutLabelVisible)
    Q_PROPERTY(qreal bounceFeedbackEffectDistance READ bounceFeedbackEffectDistance WRITE setBounceFeedbackEffectDistance)
    Q_PROPERTY(qreal pageChangeZoneWidth READ pageChangeZoneWidth WRITE setPageChangeZoneWidth)
    Q_PROPERTY(qreal pageIndicatorSpacing READ pageIndicatorSpacing WRITE setPageIndicatorSpacing)
    Q_PROPERTY(qreal maximumWidgetHeight READ maximumWidgetHeight WRITE setMaximumWidgetHeight)
    Q_PROPERTY(qreal maximumWidgetWidth READ maximumWidgetWidth WRITE setMaximumWidgetWidth)
    Q_PROPERTY(qreal minimumWidgetHeight READ minimumWidgetHeight WRITE setMinimumWidgetHeight)
    Q_PROPERTY(qreal minimumWidgetWidth READ minimumWidgetWidth WRITE setMinimumWidgetWidth)
    Q_PROPERTY(qreal pageChangePanDistance READ pageChangePanDistance WRITE setPageChangePanDistance)
    Q_PROPERTY(HbFeedback::InstantEffect pageChangeFeedbackEffect READ pageChangeFeedbackEffect WRITE setPageChangeFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetPickFeedbackEffect READ widgetPickFeedbackEffect WRITE setWidgetPickFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetDropFeedbackEffect READ widgetDropFeedbackEffect WRITE setWidgetDropFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetRepositionFeedbackEffect READ widgetRepositionFeedbackEffect WRITE setWidgetRepositionFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetOverTrashbinFeedbackEffect READ widgetOverTrashbinFeedbackEffect WRITE setWidgetOverTrashbinFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetDropToTrashbinFeedbackEffect READ widgetDropToTrashbinFeedbackEffect WRITE setWidgetDropToTrashbinFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect shortcutWidgetTapFeedbackEffect READ shortcutWidgetTapFeedbackEffect WRITE setShortcutWidgetTapFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetMoveBlockedFeedbackEffect READ widgetMoveBlockedFeedbackEffect WRITE setWidgetMoveBlockedFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect clockWidgetTapFeedbackEffect READ clockWidgetTapFeedbackEffect WRITE setClockWidgetTapFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetSnappingFeedbackEffect READ widgetSnappingFeedbackEffect WRITE setWidgetSnappingFeedbackEffect)
    Q_PROPERTY(bool isSnapEnabled READ isSnapEnabled WRITE setSnapEnabled)
    Q_PROPERTY(int snapTimeout READ snapTimeout WRITE setSnapTimeout)
    Q_PROPERTY(qreal snapForce READ snapForce WRITE setSnapForce)
    Q_PROPERTY(qreal snapGap READ snapGap WRITE setSnapGap)
    Q_PROPERTY(qreal pageMargin READ pageMargin WRITE setPageMargin)
    Q_PROPERTY(int snapLineFadeInDuration READ snapLineFadeInDuration WRITE setSnapLineFadeInDuration)
    Q_PROPERTY(int snapLineFadeOutDuration READ snapLineFadeOutDuration WRITE setSnapLineFadeOutDuration)
    Q_PROPERTY(bool isSnapEffectsEnabled READ isSnapEffectsEnabled WRITE setSnapEffectsEnabled)
    Q_PROPERTY(SceneType sceneType READ sceneType WRITE setSceneType)
    Q_PROPERTY(QEasingCurve::Type bounceAnimationEasingCurve READ bounceAnimationEasingCurve WRITE setBounceAnimationEasingCurve)
    Q_PROPERTY(QEasingCurve::Type pageChangeAnimationEasingCurve READ pageChangeAnimationEasingCurve WRITE setPageChangeAnimationEasingCurve)       
    Q_PROPERTY(WidgetOrganizerSearchSequence widgetOrganizerSearchSequence READ widgetOrganizerSearchSequence WRITE setWidgetOrganizerSearchSequence)        

    Q_ENUMS(SceneType)
    Q_ENUMS(WidgetOrganizerSearchSequence)

public:
    HsConfiguration(QObject *parent = 0);
    ~HsConfiguration();

    enum SceneType
    {
        PageWallpapers,
        SceneWallpaper
    };
    
    enum WidgetOrganizerSearchSequence
    {
        SearchRowByRow,
        SearchColumnByColumn
    };    

#define SETVALUE(member, name) \
    if (member != value) { \
        member = value; \
        emit propertyChanged(name); \
    }

    int bounceEffect() const { return mBounceEffect; }
    void setBounceEffect(int value) { SETVALUE(mBounceEffect, "bounceEffect") }
    int pageChangeAnimationDuration() const { return mPageChangeAnimationDuration; }
    void setPageChangeAnimationDuration(int value) { SETVALUE(mPageChangeAnimationDuration, "pageChangeAnimationDuration") }
    int pageChangeZoneAnimationDuration() const { return mPageChangeZoneAnimationDuration; }
    void setPageChangeZoneAnimationDuration(int value) { SETVALUE(mPageChangeZoneAnimationDuration, "pageChangeZoneAnimationDuration") }
    int pageChangeZoneReverseAnimationDuration() const { return mPageChangeZoneReverseAnimationDuration; }
    void setPageChangeZoneReverseAnimationDuration(int value) { SETVALUE(mPageChangeZoneReverseAnimationDuration, "pageChangeZoneReverseAnimationDuration") }
    int pageRemovedAnimationDuration() const { return mPageRemovedAnimationDuration; }
    void setPageRemovedAnimationDuration(int value) { SETVALUE(mPageRemovedAnimationDuration, "pageRemovedAnimationDuration") }
    int newPageAddedAnimationDuration() const { return mNewPageAddedAnimationDuration; }
    void setNewPageAddedAnimationDuration(int value) { SETVALUE(mNewPageAddedAnimationDuration, "newPageAddedAnimationDuration") }
    int widgetDragEffectDuration() const { return mWidgetDragEffectDuration; }
    void setWidgetDragEffectDuration(int value) { SETVALUE(mWidgetDragEffectDuration, "widgetDragEffectDuration") }
    int widgetDropEffectDuration() const { return mWidgetDropEffectDuration; }
    void setWidgetDropEffectDuration(int value) { SETVALUE(mWidgetDropEffectDuration, "widgetDropEffectDuration") }
    int defaultPageId() const { return mDefaultPageId; }
    void setDefaultPageId(int value) { SETVALUE(mDefaultPageId, "defaultPageId") }
    int maximumPageCount() const { return mMaximumPageCount; }
    void setMaximumPageCount(int value) { SETVALUE(mMaximumPageCount, "maximumPageCount") }
    bool isShortcutLabelVisible() const { return mIsShortcutLabelVisible; }
    void setShortcutLabelVisible(bool value) { SETVALUE(mIsShortcutLabelVisible, "isShortcutLabelVisible") }
    qreal bounceFeedbackEffectDistance() const { return mBounceFeedbackEffectDistance; }
    void setBounceFeedbackEffectDistance(qreal value) { SETVALUE(mBounceFeedbackEffectDistance, "bounceFeedbackEffectDistance") }
    qreal pageChangeZoneWidth() const { return mPageChangeZoneWidth; }
    void setPageChangeZoneWidth(qreal value) { SETVALUE(mPageChangeZoneWidth, "pageChangeZoneWidth") }
    qreal pageIndicatorSpacing() const { return mPageIndicatorSpacing; }
    void setPageIndicatorSpacing(qreal value) { SETVALUE(mPageIndicatorSpacing, "pageIndicatorSpacing") }
    qreal maximumWidgetHeight() const { return mMaximumWidgetHeight; }
    void setMaximumWidgetHeight(qreal value) { SETVALUE(mMaximumWidgetHeight, "maximumWidgetHeight") }
    qreal maximumWidgetWidth() const { return mMaximumWidgetWidth; }
    void setMaximumWidgetWidth(qreal value) { SETVALUE(mMaximumWidgetWidth, "maximumWidgetWidth") }
    qreal minimumWidgetHeight() const { return mMinimumWidgetHeight; }
    void setMinimumWidgetHeight(qreal value) { SETVALUE(mMinimumWidgetHeight, "minimumWidgetHeight") }
    qreal minimumWidgetWidth() const { return mMinimumWidgetWidth; }
    void setMinimumWidgetWidth(qreal value) { SETVALUE(mMinimumWidgetWidth, "minimumWidgetWidth") }
    qreal pageChangePanDistance() const { return mPageChangePanDistance; }
    void setPageChangePanDistance(qreal value) { SETVALUE(mPageChangePanDistance, "pageChangePanDistance") }
    HbFeedback::InstantEffect pageChangeFeedbackEffect() const { return mPageChangeFeedbackEffect; }
    void setPageChangeFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mPageChangeFeedbackEffect, "pageChangeFeedbackEffect") }
    HbFeedback::InstantEffect widgetPickFeedbackEffect() const { return mWidgetPickFeedbackEffect; }
    void setWidgetPickFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetPickFeedbackEffect, "widgetPickFeedbackEffect") }
    HbFeedback::InstantEffect widgetDropFeedbackEffect() const { return mWidgetDropFeedbackEffect; }
    void setWidgetDropFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetDropFeedbackEffect, "widgetDropFeedbackEffect") }
    HbFeedback::InstantEffect widgetRepositionFeedbackEffect() const { return mWidgetRepositionFeedbackEffect; }
    void setWidgetRepositionFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetRepositionFeedbackEffect, "widgetRepositionFeedbackEffect") }
    HbFeedback::InstantEffect widgetOverTrashbinFeedbackEffect() const { return mWidgetOverTrashbinFeedbackEffect; }
    void setWidgetOverTrashbinFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetOverTrashbinFeedbackEffect, "widgetOverTrashbinFeedbackEffect") }
    HbFeedback::InstantEffect widgetDropToTrashbinFeedbackEffect() const { return mWidgetDropToTrashbinFeedbackEffect; }
    void setWidgetDropToTrashbinFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetDropToTrashbinFeedbackEffect, "widgetDropToTrashbinFeedbackEffect") }
    HbFeedback::InstantEffect shortcutWidgetTapFeedbackEffect() const { return mShortcutWidgetTapFeedbackEffect; }
    void setShortcutWidgetTapFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mShortcutWidgetTapFeedbackEffect, "shortcutWidgetTapFeedbackEffect") }
    HbFeedback::InstantEffect widgetMoveBlockedFeedbackEffect() const { return mWidgetMoveBlockedFeedbackEffect; }
    void setWidgetMoveBlockedFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetMoveBlockedFeedbackEffect, "widgetMoveBlockedFeedbackEffect") }
    HbFeedback::InstantEffect clockWidgetTapFeedbackEffect() const { return mClockWidgetTapFeedbackEffect; }
    void setClockWidgetTapFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mClockWidgetTapFeedbackEffect, "clockWidgetTapFeedbackEffect") }
    HbFeedback::InstantEffect widgetSnappingFeedbackEffect() const { return mWidgetSnappingFeedbackEffect; }
    void setWidgetSnappingFeedbackEffect(HbFeedback::InstantEffect value) { SETVALUE(mWidgetSnappingFeedbackEffect, "widgetSnappingFeedbackEffect") }
    bool isSnapEnabled() const { return mIsSnapEnabled; }
    void setSnapEnabled(bool value) { SETVALUE(mIsSnapEnabled, "isSnapEnabled") }
    int snapTimeout() const { return mSnapTimeout; }
    void setSnapTimeout(int value) { SETVALUE(mSnapTimeout, "snapTimeout") }
    qreal snapForce() const { return mSnapForce; }
    void setSnapForce(qreal value) { SETVALUE(mSnapForce, "snapForce") }
    qreal snapGap() const { return mSnapGap; }
    void setSnapGap(qreal value) { SETVALUE(mSnapGap, "snapGap") }
    qreal pageMargin() const { return mPageMargin; }
    void setPageMargin(qreal value) { SETVALUE(mPageMargin, "pageMargin") }
    int snapLineFadeInDuration() const { return mSnapLineFadeInDuration; }
    void setSnapLineFadeInDuration(int value) { SETVALUE(mSnapLineFadeInDuration, "snapLineFadeInDuration") }
    int snapLineFadeOutDuration() const { return mSnapLineFadeOutDuration; }
    void setSnapLineFadeOutDuration(int value) { SETVALUE(mSnapLineFadeOutDuration, "snapLineFadeOutDuration") }
    bool isSnapEffectsEnabled() const { return mIsSnapEffectsEnabled; }
    void setSnapEffectsEnabled(bool value) { SETVALUE(mIsSnapEffectsEnabled, "isSnapEffectsEnabled") }
    SceneType sceneType() const { return mSceneType; }
    void setSceneType(SceneType value) { SETVALUE(mSceneType, "sceneType") }
    QEasingCurve::Type bounceAnimationEasingCurve() const { return mBounceAnimationEasingCurve; }
    void setBounceAnimationEasingCurve(QEasingCurve::Type value) { SETVALUE(mBounceAnimationEasingCurve, "bounceAnimationEasingCurve") }
    QEasingCurve::Type pageChangeAnimationEasingCurve() const { return mPageChangeAnimationEasingCurve; }
    void setPageChangeAnimationEasingCurve(QEasingCurve::Type value) { SETVALUE(mPageChangeAnimationEasingCurve, "pageChangeAnimationEasingCurve") }
    WidgetOrganizerSearchSequence widgetOrganizerSearchSequence() const { return mWidgetOrganizerSearchSequence; }
    void setWidgetOrganizerSearchSequence(WidgetOrganizerSearchSequence value) { SETVALUE(mWidgetOrganizerSearchSequence, "widgetOrganizerSearchSequence") }        

#undef SETVALUE

    QSizeF minimumWidgetSizeInUnits() const;
    QSizeF maximumWidgetSizeInUnits() const;
    QSizeF minimumWidgetSizeInPixels() const;
    QSizeF maximumWidgetSizeInPixels() const;

    bool load();
    bool importFromFile(const QString &path);
    bool exportToFile(const QString &path);

    static HsConfiguration *instance();
    static HsConfiguration *takeInstance();
    static void setInstance(HsConfiguration *instance);

signals:
    void propertyChanged(const QString &name);

private:
    Q_DISABLE_COPY(HsConfiguration)

private:
    int mBounceEffect;
    int mPageChangeAnimationDuration;
    int mPageChangeZoneAnimationDuration;
    int mPageChangeZoneReverseAnimationDuration;
    int mPageRemovedAnimationDuration;
    int mNewPageAddedAnimationDuration;
    int mWidgetDragEffectDuration;
    int mWidgetDropEffectDuration;
    int mDefaultPageId;
    int mMaximumPageCount;
    bool mIsShortcutLabelVisible;
    qreal mBounceFeedbackEffectDistance;
    qreal mPageChangeZoneWidth;
    qreal mPageIndicatorSpacing;
    qreal mMaximumWidgetHeight;
    qreal mMaximumWidgetWidth;
    qreal mMinimumWidgetHeight;
    qreal mMinimumWidgetWidth;
    qreal mPageChangePanDistance;
    HbFeedback::InstantEffect mPageChangeFeedbackEffect;
    HbFeedback::InstantEffect mWidgetPickFeedbackEffect;
    HbFeedback::InstantEffect mWidgetDropFeedbackEffect;
    HbFeedback::InstantEffect mWidgetRepositionFeedbackEffect;
    HbFeedback::InstantEffect mWidgetOverTrashbinFeedbackEffect;
    HbFeedback::InstantEffect mWidgetDropToTrashbinFeedbackEffect;
    HbFeedback::InstantEffect mShortcutWidgetTapFeedbackEffect;
    HbFeedback::InstantEffect mWidgetMoveBlockedFeedbackEffect;
    HbFeedback::InstantEffect mClockWidgetTapFeedbackEffect;
    HbFeedback::InstantEffect mWidgetSnappingFeedbackEffect;
    bool mIsSnapEnabled;
    int mSnapTimeout;
    qreal mSnapForce;
    qreal mSnapGap;
    qreal mPageMargin;
    int mSnapLineFadeInDuration;
    int mSnapLineFadeOutDuration;
    bool mIsSnapEffectsEnabled;
    SceneType mSceneType;
    QEasingCurve::Type mBounceAnimationEasingCurve;
    QEasingCurve::Type mPageChangeAnimationEasingCurve;
    WidgetOrganizerSearchSequence mWidgetOrganizerSearchSequence;
    
    static HsConfiguration *mInstance;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSCONFIGURATION_H
