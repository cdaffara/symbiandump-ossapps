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

#ifndef HSIDLESTATE_H
#define HSIDLESTATE_H

#include <QState>
#include <QTimer>
#include <QPointF>
#include <QPointer>
#include <QRectF>

#include "hswidgetpositioningonwidgetmove.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class QGraphicsItem;
class QGraphicsSceneMouseEvent;

class QGestureEvent;
class HbAction;
class HbContinuousFeedback;

class HsTitleResolver;
class QParallelAnimationGroup;
class HbMenu;
#ifdef Q_OS_SYMBIAN
class XQSettingsManager;
#endif

class HsWidgetHost;
class HsPage;
class HsMessageBoxWrapper;


class HsIdleState : public QState
{
    Q_OBJECT

public:
    HsIdleState(QState *parent = 0);
    ~HsIdleState();

signals:
    void event_applicationLibrary();
    void event_waitInput();
    void event_moveWidget();
    void event_moveScene();
    void event_selectWallpaper();
    void event_addPage();
    void event_preRemovePage();
    void event_removePage();
    void event_toggleConnection();
    void event_MoveWidgetDelete();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(HsIdleState)
    void setupStates();
    qreal pageLayerXPos(int pageIndex) const;
    void startPageChangeAnimation(int targetPageIndex, int duration);
    void startPageChangeZoneAnimation(int duration);
    bool isInPageChangeZone();
    bool isInLeftPageChangeZone();
    bool isInRightPageChangeZone();
    void addPageToScene(int pageIndex);    
    void updateZoneAnimation();
    void showTrashBin();
    QList<QRectF> createInactiveWidgetRects();
    void updatePagePresentationToWidgetSnap();
    void resetSnapPosition();
    void showVerticalLine();
    void showHorizontalLine();
    void hideVerticalLine();
    void hideHorizontalLine();

private slots:    
    void action_idle_setupView();
    void action_idle_layoutNewWidgets();
    void action_idle_showActivePage();
    void action_idle_connectOrientationChangeEventHandler();
    void action_idle_orientationChanged();
    void action_idle_installEventFilter();
    void action_idle_cleanupView();
    void action_idle_disconnectOrientationChangeEventHandler();
    void action_idle_uninstallEventFilter();
    void action_waitInput_updateOptionsMenu();
    void action_waitInput_connectGestureHandlers();
    void action_waitInput_publishIdleKey();
    void action_waitInput_disconnectGestureHandlers();
    void action_waitInput_resetNewWidgets();
    void action_moveWidget_reparentToControlLayer();
    void action_moveWidget_startWidgetDragEffect();
    void action_moveWidget_connectGestureHandlers();
    void action_moveWidget_connectOrientationChangeEventHandler();
    void action_moveWidget_setWidgetSnap();
    void action_moveWidget_reparentToPage();
    void action_moveWidgetDelete_deleteWidgetOnTrashbin();
    void action_moveWidget_startWidgetDropEffect();
    void action_moveWidget_disableInteractive();
    void action_moveWidget_disconnectGestureHandlers();
    void action_moveWidget_disconnectOrientationChangeEventHandler();
    void action_moveWidget_preventZoneAnimation();
    void action_moveWidget_deleteWidgetSnap();
    void action_moveScene_connectGestureHandlers();
    void action_moveScene_moveToNearestPage();
    void action_moveScene_disconnectGestureHandlers();    
    void action_addPage_addPage();
    void action_preRemovePage_showQuery();
    void action_preRemovePage_exit();
    void action_removePage_startRemovePageAnimation();
    void action_removePage_removePage();
    void action_toggleConnection_toggleConnection();
    void action_idle_setupTitle();
    void action_idle_cleanupTitle();
    
    void onPageTapAndHoldFinished(QGestureEvent *event);
    void onPagePanStarted(QGestureEvent *event);
    void onPagePanUpdated(QGestureEvent *event);
    void onPagePanFinished(QGestureEvent *event);
    void onWidgetTapStarted(QPointF point, HsWidgetHost *widget);
    void onWidgetTapAndHoldFinished(QGestureEvent *event, HsWidgetHost *widget);
    void onWidgetMoveUpdated(const QPointF &scenePos, HsWidgetHost *widget);
    void onWidgetMoveFinished(const QPointF &scenePos, HsWidgetHost *widget);
    void onTitleChanged(QString title);
    void onAddContentFromContextMenuActionTriggered();
    void onAddContentFromOptionsMenuActionTriggered();
    bool openTaskSwitcher();
    void zoneAnimationFinished();
    void pageChangeAnimationFinished();
    void onVerticalSnapLineTimerTimeout();
    void onHorizontalSnapLineTimerTimeout();
    void updateSnapAlgorithmParameters();
    
    void closeVirtualKeyboard();

private:
    bool isEditor(const QPointF &point, HsWidgetHost *widget);
    void stopPageChangeAnimations();
    
private:
    HbAction *mNavigationAction;
    
    HsTitleResolver *mTitleResolver;
  
    bool mAllowZoneAnimation;
    
    HbContinuousFeedback *mContinuousFeedback;
    bool mTrashBinFeedbackAlreadyPlayed;    
    QPointF mPageHotSpot;
    QPointF mWidgetHotSpot;
    QPointF mWidgetHotSpotOffset;
    qreal mDeltaX;
    qreal mWidgetX;
    qreal mWidgetY;
    QPointer<HbMenu> mSceneMenu;
    HsWidgetPositioningOnWidgetMove::Result mSnapResult;
    HsWidgetPositioningOnWidgetMove::Result mPreviousSnapResult;
    QTimer mVerticalSnapLineTimer;
    QTimer mHorizontalSnapLineTimer; 
#ifdef Q_OS_SYMBIAN    
    XQSettingsManager *mSettingsMgr;
#endif    
    HsMessageBoxWrapper *mMessageBoxWrapper;

    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};

#endif // HSIDLESTATE_H
