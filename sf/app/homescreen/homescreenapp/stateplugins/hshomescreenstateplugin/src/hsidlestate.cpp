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

#include <QStateMachine>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QApplication>
#include <QVariantHash>
#include <QInputContext>

#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbAction>
#include <HbMessageBox>
#include <HbInstance>
#include <HbLabel>
#include <HbInstantFeedback>
#include <HbContinuousFeedback>
#include <HbPanGesture>
#include <HbTapAndHoldGesture>
#include <HbVkbHostBridge>
#include <HbInputFocusObject>

#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <startupdomainpskeys.h>
#endif

#include "hsidlestate.h"
#include "hsidlewidget.h"
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hspagevisual.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswallpaper.h"
#include "hswallpaperselectionstate.h"
#include "hstrashbinwidget.h"
#include "hspageindicator.h"
#include "hsmenueventfactory.h"
#include "hshomescreenstatecommon.h"
#include "hstitleresolver.h"
#include "hsmenuservice.h"
#include "hsconfiguration.h"
#include "hsmessageboxwrapper.h"
#include "hspropertyanimationwrapper.h"
#include "hsgui.h"


namespace
{
  
    //User adds a new page to home screen
    const char hsLocTextId_OptionsMenu_AddPage[] = "txt_homescreen_opt_add_page";

    //User removes page from home screen
    const char hsLocTextId_OptionsMenu_RemovePage[] = "txt_homescreen_opt_remove_page";

    //Sends request to all widgets to enable data connections
    const char hsLocTextId_OptionsMenu_HsToOnline[] = "txt_homescreen_opt_home_screen_to_online";

    //Sends request to all widgets to disable data connections
    const char hsLocTextId_OptionsMenu_HsToOffline[] = "txt_homescreen_opt_home_screen_to_offline";

    //Home screen canvas menu item for opening picture gallery
    const char hsLocTextId_ContextMenu_ChangeWallpaper[] = "txt_homescreen_list_change_wallpaper";

    //Home screen canvas menu item for opening Application library
    const char hsLocTextId_ContextMenu_AddContent[] = "txt_homescreen_opt_add_content";

    //Home screen options menu item for opening Task switcher
    const char hsLocTextId_OptionsMenu_TaskSwitcher[] = "txt_homescreen_opt_task_switcher";

    //Heading text in confirmation dialog while removing page with content
    const char hsLocTextId_Title_RemovePage[] = "txt_homescreen_title_remove_page";

    //Text in confirmation dialog while removing page with content
    const char hsLocTextId_Confirmation_RemovePage[] = "txt_homescreen_info_page_and_content_will_be_remov";
}

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));



/*!
    \class HsIdleState
    \ingroup group_hshomescreenstateplugin
    \brief Controller part of the home screen idle state.

    Controls the home screen idle state execution. See the
    state chart below for the state structure.

    \imageDEPRECATED html hsidlestate_statechart.png
*/

/*!
    Constructs a new idle state with the given \a parent.
*/
HsIdleState::HsIdleState(QState *parent)
  : QState(parent),
    mNavigationAction(0), 
    mTitleResolver(0),
    mAllowZoneAnimation(false),
    mContinuousFeedback(0),
    mTrashBinFeedbackAlreadyPlayed(false),
    mDeltaX(0),
    mWidgetX(0),
    mWidgetY(0),
    mSceneMenu(0)
#ifdef Q_OS_SYMBIAN
    ,mSettingsMgr(0)
#endif
    ,mMessageBoxWrapper(0)
    
{
    setupStates();
    mTitleResolver = new HsTitleResolver(this);
     
    // TODO: Uncomment when updated API available
    //mContinuousFeedback = new HbContinuousFeedback;
    //mContinuousFeedback->setContinuousEffect(HbFeedback::ContinuousSmooth);
    //mContinuousFeedback->setIntensity(HbFeedback::IntensityFull );

    mVerticalSnapLineTimer.setSingleShot(true);
    mHorizontalSnapLineTimer.setSingleShot(true);
}

/*!
    Destroys this idle state.
*/
HsIdleState::~HsIdleState()
{
    // TODO: Uncomment when updated API available
    //delete mContinuousFeedback;
}

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsIdleState::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
        case QEvent::ApplicationActivate:
            action_idle_layoutNewWidgets();
            break;
        case QEvent::ApplicationDeactivate:
            emit event_waitInput();
            break;
        default:
            break;
    }
    return QState::eventFilter(watched, event);
}

/*!
    \fn HsIdleState::event_applicationLibrary()

    This signal initiates a transition to application library.
*/

/*!
    \fn HsIdleState::event_waitInput()

    This signal initiates a transition to the waitInput state.
*/

/*!
    \fn HsIdleState::event_moveWidget()

    This signal initiates a transition to the moveWidget state.
*/

/*!
    \fn HsIdleState::event_moveScene()

    This signal initiates a transition to the moveScene state.
*/

/*!
    \fn HsIdleState::event_selectWallpaper()

    This signal initiates a transition to the selectSceneWallpaper state.
*/

/*!
    \fn HsIdleState::event_addPage()

    This signal initiates a transition to the addPage state.
*/

/*!
    \fn HsIdleState::event_removePage()

    This signal initiates a transition to the removePage state.
*/

/*!
    \fn HsIdleState::event_toggleConnection()

    This signal initiates a transition to the toggleConnection state.
*/

/*!
    Creates the internal state structure and connects
    state entry and exit actions.
*/
void HsIdleState::setupStates()
{
    // States

    QState *state_waitInput = new QState(this);
    setInitialState(state_waitInput);

    QState *state_moveWidget = new QState(this);
    QState *state_moveWidgetMoving = new QState(state_moveWidget);
    QState *state_moveWidgetDelete = new QState(state_moveWidget);
    state_moveWidget->setInitialState(state_moveWidgetMoving);
    
    QState *state_moveScene = new QState(this);
    HsWallpaperSelectionState *state_wallpaperSelectionState =
        new HsWallpaperSelectionState(this);
    QState *state_addPage = new QState(this);
    QState *state_preRemovePage = new QState(this);
    QState *state_removePage = new QState(this);
    QState *state_toggleConnection = new QState(this);

    // Transitions

    state_waitInput->addTransition(
        this, SIGNAL(event_addPage()), state_addPage);
    state_waitInput->addTransition(
        this, SIGNAL(event_preRemovePage()), state_preRemovePage);
    state_waitInput->addTransition(
        this, SIGNAL(event_toggleConnection()), state_toggleConnection);
    state_waitInput->addTransition(
        this, SIGNAL(event_selectWallpaper()), state_wallpaperSelectionState);
    state_waitInput->addTransition(
        this, SIGNAL(event_moveScene()), state_moveScene);
    state_waitInput->addTransition(
        this, SIGNAL(event_moveWidget()), state_moveWidget);

    state_moveWidgetMoving->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);
    state_moveWidgetMoving->addTransition(
        this, SIGNAL(event_MoveWidgetDelete()), state_moveWidgetDelete);
    state_moveWidgetDelete->addTransition(state_waitInput);

    state_moveScene->addTransition(
        this, SIGNAL(event_waitInput()), state_waitInput);

    state_wallpaperSelectionState->addTransition(
        state_wallpaperSelectionState, SIGNAL(event_waitInput()), state_waitInput);

    state_addPage->addTransition(state_waitInput);
    
    state_preRemovePage->addTransition(
        this,SIGNAL(event_removePage()),state_removePage);
    
    state_preRemovePage->addTransition(
        this,SIGNAL(event_waitInput()),state_waitInput);

    state_removePage->addTransition(
        this,SIGNAL(event_waitInput()),state_waitInput);

    state_toggleConnection->addTransition(state_waitInput);

    // Actions

    ENTRY_ACTION(this, action_idle_setupView)
    ENTRY_ACTION(this, action_idle_layoutNewWidgets)
    ENTRY_ACTION(this, action_idle_showActivePage)
    ENTRY_ACTION(this, action_idle_connectOrientationChangeEventHandler)
    ENTRY_ACTION(this, action_idle_setupTitle)
    ENTRY_ACTION(this, action_idle_installEventFilter)
    EXIT_ACTION(this, action_idle_cleanupTitle)
    EXIT_ACTION(this, action_idle_cleanupView)
    EXIT_ACTION(this, action_idle_disconnectOrientationChangeEventHandler)
    EXIT_ACTION(this, action_idle_uninstallEventFilter)

    ENTRY_ACTION(state_waitInput, action_waitInput_updateOptionsMenu)
    ENTRY_ACTION(state_waitInput, action_waitInput_connectGestureHandlers)
    ENTRY_ACTION(state_waitInput, action_waitInput_publishIdleKey)
    EXIT_ACTION(state_waitInput, action_waitInput_disconnectGestureHandlers)
    EXIT_ACTION(state_waitInput, action_waitInput_resetNewWidgets)

    ENTRY_ACTION(state_moveWidget, action_moveWidget_reparentToControlLayer)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_startWidgetDragEffect)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectGestureHandlers)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_connectOrientationChangeEventHandler)
    ENTRY_ACTION(state_moveWidget, action_moveWidget_setWidgetSnap)
    EXIT_ACTION(state_moveWidget, action_moveWidget_reparentToPage)
    EXIT_ACTION(state_moveWidget, action_moveWidget_startWidgetDropEffect)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disableInteractive)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectGestureHandlers)
    EXIT_ACTION(state_moveWidget, action_moveWidget_disconnectOrientationChangeEventHandler)
    EXIT_ACTION(state_moveWidget, action_moveWidget_preventZoneAnimation)
    EXIT_ACTION(state_moveWidget, action_moveWidget_deleteWidgetSnap)

    ENTRY_ACTION(state_moveWidgetDelete, action_moveWidgetDelete_deleteWidgetOnTrashbin)

    ENTRY_ACTION(state_moveScene, action_moveScene_connectGestureHandlers)
    EXIT_ACTION(state_moveScene, action_moveScene_moveToNearestPage)
    EXIT_ACTION(state_moveScene, action_moveScene_disconnectGestureHandlers)
    
    ENTRY_ACTION(state_addPage, action_addPage_addPage)
    
    ENTRY_ACTION(state_preRemovePage, action_preRemovePage_showQuery)
    EXIT_ACTION(state_preRemovePage, action_preRemovePage_exit);

    ENTRY_ACTION(state_removePage, action_removePage_startRemovePageAnimation)
    EXIT_ACTION(state_removePage, action_removePage_removePage)

    ENTRY_ACTION(state_toggleConnection, action_toggleConnection_toggleConnection)
}

/*!
    Computes the page layer x position based on the given \a pageIndex.
*/
qreal HsIdleState::pageLayerXPos(int pageIndex) const
{
    return -pageIndex * HsGui::instance()->layoutRect().width();
}

/*!
    Starts the page change animation based on the given \a targetPageIndex
    and \a duration.
*/
void HsIdleState::startPageChangeAnimation(int targetPageIndex, int duration)
{
    if (!HsScene::instance()->activeWidget()) {
        hbInstance->allMainWindows().first()->setInteractive(false);
    }
    HsPropertyAnimationWrapper *animation = HsGui::instance()->pageChangeAnimation();
    if (animation->isRunning()) {
        animation->stop();
    }        
    animation->disconnect(this);
    animation->setEndValue(pageLayerXPos(targetPageIndex));
    animation->setDuration(duration);
    
    if (abs(mDeltaX) < HSCONFIGURATION_GET(pageChangePanDistance)) {
        animation->setEasingCurve(HSCONFIGURATION_GET(bounceAnimationEasingCurve)); 
    }
    else {
        animation->setEasingCurve(HSCONFIGURATION_GET(pageChangeAnimationEasingCurve));
    }

    connect(animation,
            SIGNAL(finished()),
            SLOT(pageChangeAnimationFinished()), 
            Qt::UniqueConnection);
    animation->start();
    
    HbInstantFeedback::play(HSCONFIGURATION_GET(pageChangeFeedbackEffect));
    
    HsGui::instance()->idleWidget()->showPageIndicator();
    HsGui::instance()->idleWidget()->setActivePage(targetPageIndex);
}

/*!
    Starts the page change zone animation (crawling) based on the given a\ duration
*/
void HsIdleState::startPageChangeZoneAnimation(int duration)
{
    HsScene *scene = HsScene::instance();

    int targetPageIndex = scene->activePageIndex();

    if (isInLeftPageChangeZone() && 0 < targetPageIndex) {
        --targetPageIndex;
    } else if (isInRightPageChangeZone() &&
        targetPageIndex < HSCONFIGURATION_GET(maximumPageCount) - 1) {
        ++targetPageIndex;
    } else {
        return;
    }

    int bounceEffect = HSCONFIGURATION_GET(bounceEffect);
    
    HsPropertyAnimationWrapper *pageCrawlingAnimation =
        HsGui::instance()->pageCrawlingAnimation();

    if (isInLeftPageChangeZone()) {
        pageCrawlingAnimation->setEndValue(pageLayerXPos(scene->activePageIndex()) + bounceEffect);
    } else {
        pageCrawlingAnimation->setEndValue(pageLayerXPos(scene->activePageIndex()) - bounceEffect);
    }
    pageCrawlingAnimation->setDuration(duration);
    pageCrawlingAnimation->setForward();

    connect(pageCrawlingAnimation,
            SIGNAL(finished()),
            SLOT(zoneAnimationFinished()),
            Qt::UniqueConnection);
    
    pageCrawlingAnimation->start();
}

/*!
    Checks if the active widget is located inside the left or right
    page change zone.
*/
bool HsIdleState::isInPageChangeZone()
{
    return isInLeftPageChangeZone() ||
           isInRightPageChangeZone();
}

/*!
    Checks if touch point in the active widget is located inside the left
    page change zone.
*/
bool HsIdleState::isInLeftPageChangeZone()
{
    return mWidgetHotSpot.x() < HSCONFIGURATION_GET(pageChangeZoneWidth);
}

/*!
    Checks if touch point in the active widget is located inside the right
    page change zone.
*/
bool HsIdleState::isInRightPageChangeZone()
{
    qreal pageWidth = HsGui::instance()->layoutRect().width();
    return mWidgetHotSpot.x() > pageWidth - HSCONFIGURATION_GET(pageChangeZoneWidth);
}

/*!
    Inserts new page at index position \a pageIndex in the scene.
*/
void HsIdleState::addPageToScene(int pageIndex)
{
    HsPageData data;
    data.indexPosition = pageIndex;
    HsPage *page = HsPage::createInstance(data);
    page->load();
    HsScene::instance()->addPage(page);
    HsGui::instance()->idleWidget()->insertPage(pageIndex, page);
}

/*!
    Checks if page change zone animation needs to be started or stopped
*/
void HsIdleState::updateZoneAnimation()
{
    HsPropertyAnimationWrapper *pageCrawlingAnimation = 
        HsGui::instance()->pageCrawlingAnimation();

    if (!mAllowZoneAnimation) {
        pageCrawlingAnimation->stop();
    } else if (isInPageChangeZone()) {
        // should we start it
        if (!pageCrawlingAnimation->isRunning() &&
            !HsGui::instance()->pageChangeAnimation()->isRunning()) {
            startPageChangeZoneAnimation(HSCONFIGURATION_GET(pageChangeZoneAnimationDuration));
        } 
    } else if (pageCrawlingAnimation->isRunning()) {
        // Not in zone, but still running     
        if (pageCrawlingAnimation->isForward()){
            // reverse
            pageCrawlingAnimation->setDuration(HSCONFIGURATION_GET(pageChangeZoneReverseAnimationDuration));
            pageCrawlingAnimation->setBackward();
        } 
    } 
}

/*!
    Selects which trash bin icon is shown or not shown at all
*/
void HsIdleState::showTrashBin()
{
    if (HsGui::instance()->idleWidget()->trashBin()->isUnderMouse()) {
        if (!mTrashBinFeedbackAlreadyPlayed) {
            HbInstantFeedback::play(HSCONFIGURATION_GET(widgetOverTrashbinFeedbackEffect));
            mTrashBinFeedbackAlreadyPlayed = true;
        }
        HsGui::instance()->idleWidget()->trashBin()->activate();
    } else {
        HsGui::instance()->idleWidget()->trashBin()->deactivate();
        mTrashBinFeedbackAlreadyPlayed = false;
    }

    if (!HsGui::instance()->idleWidget()->pageIndicator()->isAnimationRunning()) {
        HsGui::instance()->idleWidget()->showTrashBin();
    }
}

/*!
    If called for the first time, setups the idle view.
    Updates the soft key action and sets the idle view
    as the current view to the main window.
*/
void HsIdleState::action_idle_setupView()
{
    HsGui::instance()->setupIdleUi();
    connect(HsGui::instance(),SIGNAL(navigateToApplibrary()),SIGNAL(event_applicationLibrary()),Qt::UniqueConnection);

}

/*!
    Sets the idle view's title.
*/
void HsIdleState::action_idle_setupTitle()
{    
    onTitleChanged(mTitleResolver->title());
    connect(mTitleResolver, 
        SIGNAL(titleChanged(QString)), 
        SLOT(onTitleChanged(QString)));
}

/*!
    Updates the idle view's title.
*/
void HsIdleState::onTitleChanged(QString title)
{
    qDebug() << "HsIdleState::onTitleChanged() to title: " << title;
    HsGui::instance()->idleView()->setTitle(title);
}

/*!

*/
void HsIdleState::onAddContentFromContextMenuActionTriggered()
{
    /* include position information in homescreen data,
       which passed around in applib and returned when adding content
    */
    QVariant homescreenData(mPageHotSpot);
    machine()->postEvent(
        HsMenuEventFactory::createOpenAppLibraryEvent(Hs::AddHsMenuMode, homescreenData));
    mPageHotSpot = QPointF();
}

/*!

*/
void HsIdleState::onAddContentFromOptionsMenuActionTriggered()
{
    machine()->postEvent(
         HsMenuEventFactory::createOpenAppLibraryEvent(Hs::AddHsMenuMode));
}

/*!
    Disconnects the idle view's title.
*/
void HsIdleState::action_idle_cleanupTitle()
{
    mTitleResolver->disconnect(this);
}

/*!

*/
void HsIdleState::onPageTapAndHoldFinished(QGestureEvent *event)
{
    mPageHotSpot = qobject_cast<HbTapAndHoldGesture *>(
        event->gesture(Qt::TapAndHoldGesture))->scenePosition();

    mSceneMenu = new HbMenu();
    mSceneMenu->setObjectName("hs_ctx_menu");
    mSceneMenu->setAttribute(Qt::WA_DeleteOnClose);

    HbAction *action = mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent), this,
	                      SLOT(onAddContentFromContextMenuActionTriggered()));
    action->setObjectName("action_add_content_ctx_menu");

    HsScene *scene = HsScene::instance();
    if (scene->pages().count() < HSCONFIGURATION_GET(maximumPageCount)) {
        action = mSceneMenu->addAction(hbTrId(hsLocTextId_OptionsMenu_AddPage), this, SIGNAL(event_addPage()));
        action->setObjectName("action_add_page_ctx_menu");
    }

    action = mSceneMenu->addAction(hbTrId(hsLocTextId_ContextMenu_ChangeWallpaper), this, SIGNAL(event_selectWallpaper()));
    action->setObjectName("action_change_wallpaper_ctx_menu");
    mSceneMenu->setPreferredPos(mPageHotSpot);
    mSceneMenu->open();
}

void HsIdleState::onPagePanStarted(QGestureEvent *event)
{
    Q_UNUSED(event)
    emit event_moveScene();
}
 
void HsIdleState::onPagePanUpdated(QGestureEvent *event)
{
    HbPanGesture *gesture = qobject_cast<HbPanGesture *>(
        event->gesture(Qt::PanGesture));

    qreal temp = gesture->sceneOffset().x();
    // for small movement take into consideration also previous delta to remove "shaking" effect
    if (abs(mDeltaX - temp) < 10) {
        mDeltaX = 0.9 * temp + 0.1 * mDeltaX;
    } else {
        mDeltaX = temp;
    }

    HsScene *scene = HsScene::instance();
    int bounceEffect = HSCONFIGURATION_GET(bounceEffect);
    qreal x = qBound(pageLayerXPos(scene->pages().count() - 1) - bounceEffect / 2 / HsGui::instance()->idleWidget()->parallaxFactor(),
                     pageLayerXPos(scene->activePageIndex()) + mDeltaX,
                     pageLayerXPos(0) + (bounceEffect / 2 / HsGui::instance()->idleWidget()->parallaxFactor()));

    HsGui::instance()->idleWidget()->setSceneX(x);
}
 
void HsIdleState::onPagePanFinished(QGestureEvent *event)
{
    HbPanGesture *gesture = qobject_cast<HbPanGesture *>(
        event->gesture(Qt::PanGesture));
    mDeltaX = gesture->sceneOffset().x();
    emit event_waitInput();
}

void HsIdleState::onWidgetTapStarted(QPointF point, HsWidgetHost *widget)
{
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    QMetaObject::invokeMethod(page, "updateZValues", Qt::QueuedConnection);
    HbVkbHost::HbVkbStatus status = HbVkbHostBridge::instance()->keypadStatus();
    if ( status == HbVkbHost::HbVkbStatusOpened && !isEditor(point, widget) ) {
        closeVirtualKeyboard();
    }
}
 
void HsIdleState::onWidgetTapAndHoldFinished(QGestureEvent *event, HsWidgetHost *widget)
{
    HsScene *scene = HsScene::instance();
    scene->setActiveWidget(widget);
    
    mWidgetHotSpot = qobject_cast<HbTapAndHoldGesture *>(
        event->gesture(Qt::TapAndHoldGesture))->scenePosition();
    mWidgetHotSpotOffset = mWidgetHotSpot - widget->visual()->pos();
    
    emit event_moveWidget();
}
 
void HsIdleState::onWidgetMoveUpdated(const QPointF &scenePos, HsWidgetHost *widget)
{
    /* If move was interrupted by a popup and and we plan to move second widget.
     * Then the erroneous first widget instance is received here, as there is no cancel gesture
       -> we need to use active widget that was saved during TapAndHold.
    */
    widget = HsScene::instance()->activeWidget();

    QRectF widgetRect = widget->visual()->geometry();

    // Move widget to updated position.
    mWidgetHotSpot = scenePos;
    widgetRect.moveTopLeft(mWidgetHotSpot - mWidgetHotSpotOffset);

    // When moving widget can go over the pages left border.
    qreal lowerBoundX = -widgetRect.width();

    // When moving widget can go over the pages right border.
    QRectF pageRect = HsGui::instance()->idleView()->rect();
    qreal upperBoundX = pageRect.width();

    // When moving widget can go under the chrome at the pages upper border.
    // Notice that chrome height is 64 pixels.
    qreal lowerBoundY = qreal(64) - widgetRect.height();

    // When moving widget can go over the pages down border.
    qreal upperBoundY = pageRect.height();

    qreal widgetX = qBound(lowerBoundX, widgetRect.x(), upperBoundX);
    qreal widgetY = qBound(lowerBoundY, widgetRect.y(), upperBoundY);

    // for small movement take into consideration also previous value to remove "shaking" effect
    if ((abs(mWidgetX - widgetX) < 10) && (abs(mWidgetY - widgetY) < 10)) {
        mWidgetX = 0.9 * widgetX + 0.1 * mWidgetX;
        mWidgetY = 0.9 * widgetY + 0.1 * mWidgetY;
    } else {
        mWidgetX = widgetX;
        mWidgetY = widgetY;
    }

    // If using ItemClipsChildrenToShape-flag in widgethost then
    // setPos does not update position here, however setGeometry does it, QT bug?
    widget->visual()->setGeometry(mWidgetX, mWidgetY, widgetRect.width(), widgetRect.height());
    
    if (HSCONFIGURATION_GET(isSnapEnabled)) {
        mSnapResult = HsWidgetPositioningOnWidgetMove::instance()->run(widget->visual()->sceneBoundingRect());

        bool isTrashbinOpen = HsGui::instance()->idleWidget()->trashBin()->isUnderMouse();

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            if (mSnapResult.hasHorizontalSnap && !isTrashbinOpen) {
                showVerticalLine();
            } else {
                hideVerticalLine();
            }
            if (mSnapResult.hasVerticalSnap && !isTrashbinOpen) {
                showHorizontalLine();
            } else {
                hideHorizontalLine();
            }
        }

        mPreviousSnapResult = mSnapResult;
    }

    // Handle effects:
    // User is indicated by a tactile feedback if he/she is trying to move
    // widget over the first or the last page.
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    int bounceFeedbackEffectDistance = HSCONFIGURATION_GET(bounceFeedbackEffectDistance);
    if( (page == scene->pages().first() && mWidgetHotSpot.x() < bounceFeedbackEffectDistance ) ||
        (page == scene->pages().last() && scene->pages().count() == HSCONFIGURATION_GET(maximumPageCount)
         && mWidgetHotSpot.x() > pageRect.width() - bounceFeedbackEffectDistance)) {
             HbInstantFeedback::play(HSCONFIGURATION_GET(widgetMoveBlockedFeedbackEffect));
            // TODO: use code below when Orbit has updated ContinuousFeedback API
            //if (!mContinuousFeedback->isPlaying()) {
            //    mContinuousFeedback->play();
            //}
        } else /*if (mContinuousFeedback->isPlaying())*/ {
            //mContinuousFeedback->stop();
        }

    updateZoneAnimation();
    showTrashBin();
}
 
void HsIdleState::onWidgetMoveFinished(const QPointF &scenePos, HsWidgetHost *widget)
{
    Q_UNUSED(scenePos)
    Q_UNUSED(widget)
    emit event_MoveWidgetDelete();
}

/*!
    Lays out the active page's new widgets that were added
    from the application library.
*/
void HsIdleState::action_idle_layoutNewWidgets()
{
    HsPage *page = HsScene::instance()->activePage();
    if (!page) {
        return;
    }

    QList<HsWidgetHost *> widgets = page->newWidgets();
    if (widgets.isEmpty()) {
        return;
    }

    foreach (HsWidgetHost *widget, widgets) {
        widget->startWidget();
    }

    page->layoutNewWidgets();

}

/*!
    Moves the scene and page layers so that the active
    page is shown.
*/
void HsIdleState::action_idle_showActivePage()
{   
    HsScene *scene = HsScene::instance();
    QApplication::instance()->installEventFilter(scene);
    scene->activePage()->showWidgets();
    qreal x = pageLayerXPos(scene->activePageIndex());
    HsGui::instance()->idleWidget()->setSceneX(x);
}

/*!
    Connects the orientation change event handler.
*/
void HsIdleState::action_idle_connectOrientationChangeEventHandler()
{
    connect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(action_idle_orientationChanged()));
}

void HsIdleState::action_idle_orientationChanged()
{
    HsScene::instance()->activePage()->resetNewWidgets();
    stopPageChangeAnimations();
    action_idle_showActivePage();
}

/*!
    Installs the event filter.
*/
void HsIdleState::action_idle_installEventFilter()
{
    QCoreApplication::instance()->installEventFilter(this);
}

/*!
    Publishes the idle key
*/
void HsIdleState::action_waitInput_publishIdleKey()
{
#ifdef Q_OS_SYMBIAN
    qDebug() << "HsIdleState::action_waitInput_publishIdleKey";
    if (!mSettingsMgr){
        // All done, let others know it too
        qDebug() << "Setting the idle key";
        mSettingsMgr = new XQSettingsManager(this);
        XQPublishAndSubscribeSettingsKey key(KPSUidStartup.iUid, KPSIdlePhase1Ok);
        mSettingsMgr->writeItemValue(key, EIdlePhase1Ok);

        // Done, we can disconnect this signal.
        disconnect(sender(), SIGNAL(entered()), this, SLOT(action_waitInput_publishIdleKey()));
    }
#endif
}

/*!
    Cleans up the idle view.
*/
void HsIdleState::action_idle_cleanupView()
{
    HsScene *scene = HsScene::instance();
    QApplication::instance()->removeEventFilter(scene);
    scene->activePage()->hideWidgets();
    
    HsGui::instance()->disconnect(this,SIGNAL(event_applicationLibrary()));
    //Close options menu on view change
    HsGui::instance()->idleView()->setMenu(NULL);

    //Close context menu on view change
    if (mSceneMenu) {
        mSceneMenu->close();
    }
}

/*!
    Disconnects the orientation change event handler.
*/
void HsIdleState::action_idle_disconnectOrientationChangeEventHandler()
{
    disconnect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(action_idle_orientationChanged()));
}

/*!
    Disconnects the event filter.
*/
void HsIdleState::action_idle_uninstallEventFilter()
{
    QCoreApplication::instance()->removeEventFilter(this);
}

/*!
    Updates the options menu content.
*/
void HsIdleState::action_waitInput_updateOptionsMenu()
{
    HsScene *scene = HsScene::instance();

    HbMenu *menu = new HbMenu();
    menu->setObjectName("hs_menu");
    // Task switcher
    menu->addAction(hbTrId(hsLocTextId_OptionsMenu_TaskSwitcher),
        this, SLOT(openTaskSwitcher()));
    
    // Add content
    menu->addAction(hbTrId(hsLocTextId_ContextMenu_AddContent),
        this, SLOT(onAddContentFromOptionsMenuActionTriggered()));
    // Add page
    if (scene->pages().count() < HSCONFIGURATION_GET(maximumPageCount)) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_AddPage),
            this, SIGNAL(event_addPage()));
    
    }

    // Change wallpaper
    menu->addAction(hbTrId(hsLocTextId_ContextMenu_ChangeWallpaper),
        this, SIGNAL(event_selectWallpaper()));
    
    // Remove page
    if (scene->activePage()->isRemovable()) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_RemovePage),
            this, SIGNAL(event_preRemovePage()));
    }

    // Online / Offline
    if (scene->isOnline()) {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_HsToOffline),
            this, SIGNAL(event_toggleConnection()));
    } else {
        menu->addAction(hbTrId(hsLocTextId_OptionsMenu_HsToOnline),
            this, SIGNAL(event_toggleConnection()));
    }

    HsGui::instance()->idleView()->setMenu(menu);
}

void HsIdleState::action_waitInput_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();

    connect(scene, 
        SIGNAL(pageTapFinished(QGestureEvent*)), 
        SLOT(closeVirtualKeyboard()),
        Qt::UniqueConnection);
    connect(scene, 
        SIGNAL(pagePanStarted(QGestureEvent*)), 
        SLOT(closeVirtualKeyboard()),
        Qt::UniqueConnection);

    connect(scene, 
        SIGNAL(pageTapAndHoldFinished(QGestureEvent*)), 
        SLOT(onPageTapAndHoldFinished(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene,
        SIGNAL(pagePanStarted(QGestureEvent*)),
        SLOT(onPagePanStarted(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene, 
        SIGNAL(widgetTapStarted(QPointF, HsWidgetHost*)), 
        SLOT(onWidgetTapStarted(QPointF, HsWidgetHost*)),
        Qt::UniqueConnection);
    
    connect(scene, 
        SIGNAL(widgetTapAndHoldFinished(QGestureEvent*,HsWidgetHost*)), 
        SLOT(onWidgetTapAndHoldFinished(QGestureEvent*,HsWidgetHost*)),
        Qt::UniqueConnection);
}

void HsIdleState::action_waitInput_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

/*!
    Resets new widget layout.
*/
void HsIdleState::action_waitInput_resetNewWidgets()
{
    HsScene::instance()->activePage()->resetNewWidgets();
}

/*!
    Reparents the active widget to the control layer.
*/
void HsIdleState::action_moveWidget_reparentToControlLayer()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);
    widget->visual()->setParentItem(HsGui::instance()->idleWidget()->controlLayer());

    HsGui::instance()->idleWidget()->showTrashBin();
}

/*!
    Starts the widget drag effect.
*/
void HsIdleState::action_moveWidget_startWidgetDragEffect()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    Q_ASSERT(widget);
    widget->startDragEffect();
    mAllowZoneAnimation = true;
}

/*!
*/
void HsIdleState::action_moveWidget_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
        
    connect(scene, 
        SIGNAL(widgetMoveUpdated(const QPointF&,HsWidgetHost*)), 
        SLOT(onWidgetMoveUpdated(const QPointF&,HsWidgetHost*)),
        Qt::UniqueConnection);
    
    connect(scene, 
        SIGNAL(widgetMoveFinished(const QPointF&,HsWidgetHost*)), 
        SLOT(onWidgetMoveFinished(const QPointF&,HsWidgetHost*)),
        Qt::UniqueConnection);
}

/*!
    Connects the SIGNAL for changing the orientation for moveWidget state 
    to update the snapping algorithm accordingly.
*/
void HsIdleState::action_moveWidget_connectOrientationChangeEventHandler()
{
    connect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        SLOT(updateSnapAlgorithmParameters()));
}

/*!
    Initializes the presentation to the snapping algorithm
    Connects the SIGNAL for changing the presentation on active page changed
    Connects the timer for showing the snap lines
*/
void HsIdleState::action_moveWidget_setWidgetSnap()
{
    if (HSCONFIGURATION_GET(isSnapEnabled)) {

        QVariantHash snapConfiguration;
        snapConfiguration[Hs::snapEnabled] = QString::number(HSCONFIGURATION_GET(isSnapEnabled));
        snapConfiguration[Hs::snapForce] = QString::number(HSCONFIGURATION_GET(snapForce));
        snapConfiguration[Hs::snapGap] = QString::number(HSCONFIGURATION_GET(snapGap));
        HsWidgetPositioningOnWidgetMove::instance()->setConfiguration(snapConfiguration);

        updatePagePresentationToWidgetSnap();

        connect(HsScene::instance(), SIGNAL(activePageChanged()),
            SLOT(updateSnapAlgorithmParameters()));

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            mVerticalSnapLineTimer.setInterval(HSCONFIGURATION_GET(snapTimeout));
            mHorizontalSnapLineTimer.setInterval(HSCONFIGURATION_GET(snapTimeout));

            connect(&mVerticalSnapLineTimer, SIGNAL(timeout()),
                SLOT(onVerticalSnapLineTimerTimeout()));
            connect(&mHorizontalSnapLineTimer, SIGNAL(timeout()),
                SLOT(onHorizontalSnapLineTimerTimeout()));

        }
    }
}

/*!
    Create the list of inactive rectangles on the page.
*/
QList<QRectF> HsIdleState::createInactiveWidgetRects()
{
    HsScene *scene = HsScene::instance();
    HsPage *activePage = scene->activePage();
    HsWidgetHost *activeWidget = scene->activeWidget();

    QList<QRectF> incativeWidgetRects;

    foreach (HsWidgetHost* widget, activePage->widgets()) {
        //If the above function returns the active widget, skip over it as widget and mMovingRect represent same widget.
        if (widget == activeWidget) {
           continue;
        }
        QRectF widgetRect = widget->visual()->geometry();
        incativeWidgetRects.append(widgetRect);
    }
    return incativeWidgetRects;
}

/*!
    Reparents the active widget to the active page.
*/
void HsIdleState::action_moveWidget_reparentToPage()
{
    HsPropertyAnimationWrapper *pageCrawlingAnimation =
        HsGui::instance()->pageCrawlingAnimation();
    if (pageCrawlingAnimation->isRunning()&& 
        pageCrawlingAnimation->isForward()) {
        pageCrawlingAnimation->setDuration(HSCONFIGURATION_GET(pageChangeZoneReverseAnimationDuration));
        pageCrawlingAnimation->setBackward();
    }

    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();
    HsWidgetHost *widget = scene->activeWidget();
    
    if (widget) {
        if (widget->page() != page) {
            widget->page()->removeWidget(widget);
            page->addExistingWidget(widget);
            if (HsGui::instance()->orientation() == Qt::Horizontal) {
                widget->removePresentation(Qt::Vertical);
            } else {
                widget->removePresentation(Qt::Horizontal);
            }
        }

        //Set the snap position of the widget and save the position
        QRectF widgetRect = widget->visual()->geometry();
        if (mSnapResult.hasHorizontalSnap) {
            widgetRect.moveLeft(mSnapResult.horizontalSnapPosition);
        }
        if (mSnapResult.hasVerticalSnap) {
            widgetRect.moveTop(mSnapResult.verticalSnapPosition);
        }

        QPointF adjustedWidgetPosition = page->adjustedWidgetPosition(widgetRect);
        widget->visual()->setPos(adjustedWidgetPosition);

        widget->savePresentation();
        page->updateZValues();

        widget->visual()->setParentItem(HsScene::instance()->activePage()->visual());
    }
    HsGui::instance()->idleWidget()->showPageIndicator();
    
    emit event_waitInput();
}


/*!
    Reparents the active widget to the active page.
*/
void HsIdleState::action_moveWidgetDelete_deleteWidgetOnTrashbin()
{
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activeWidget();

    if ( widget && HsGui::instance()->idleWidget()->trashBin()->isUnderMouse() ) {
        HbInstantFeedback::play(HSCONFIGURATION_GET(widgetDropToTrashbinFeedbackEffect));
        widget->page()->removeWidget(widget);
        widget->remove();
        scene->setActiveWidget(0);
    }
}

/*!
    Starts the widget drop effect.
*/
void HsIdleState::action_moveWidget_startWidgetDropEffect()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    if (widget) {
        widget->startDropEffect();
    }
}

/*!
    Set interactive to false.
*/
void HsIdleState::action_moveWidget_disableInteractive()
{
    HsWidgetHost *widget = HsScene::instance()->activeWidget();
    if (widget) {
        HsPropertyAnimationWrapper *animation = HsGui::instance()->pageChangeAnimation();
        if (animation->isRunning()) {
            hbInstance->allMainWindows().first()->setInteractive(false);
        }
        HsScene::instance()->setActiveWidget(0);
    }
}

/*!
    Disconnects gesture handlers
*/
void HsIdleState::action_moveWidget_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

/*!
    Disconnects orientation change on moveWidget state
*/
void HsIdleState::action_moveWidget_disconnectOrientationChangeEventHandler()
{
    disconnect(HsGui::instance(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(updateSnapAlgorithmParameters()));

}

/*!
    Resets the snap position
    Disconnect the Active Page Changed SIGNAL
    Disconnects the timers to show snap lines
*/
void HsIdleState::action_moveWidget_deleteWidgetSnap()
{
    if (HSCONFIGURATION_GET(isSnapEnabled)) {
        resetSnapPosition();

        disconnect(HsScene::instance(), SIGNAL(activePageChanged()),
                    this, SLOT(updateSnapAlgorithmParameters()));

        if (HSCONFIGURATION_GET(isSnapEffectsEnabled)) {
            disconnect(&mVerticalSnapLineTimer, SIGNAL(timeout()),
                this, SLOT(onVerticalSnapLineTimerTimeout()));
            disconnect(&mHorizontalSnapLineTimer, SIGNAL(timeout()),
                this, SLOT(onHorizontalSnapLineTimerTimeout()));
        }
    }
}

/*!
    Prevents page change zone animation.
*/
void HsIdleState::action_moveWidget_preventZoneAnimation()
{
    mAllowZoneAnimation = false;
}

void HsIdleState::action_moveScene_connectGestureHandlers()
{
    HsScene *scene = HsScene::instance();

    connect(scene,
        SIGNAL(pagePanUpdated(QGestureEvent*)),
        SLOT(onPagePanUpdated(QGestureEvent*)),
        Qt::UniqueConnection);

    connect(scene,
        SIGNAL(pagePanFinished(QGestureEvent*)),
        SLOT(onPagePanFinished(QGestureEvent*)),
        Qt::UniqueConnection);
}

/*!
    Moves to the nearest page.
*/
void HsIdleState::action_moveScene_moveToNearestPage()
{
    QList<HsPage *> pages = HsScene::instance()->pages();
    QSizeF pageSize = pages.first()->visual()->size();

    int pageIndex = HsScene::instance()->activePageIndex();

    if (mDeltaX < -HSCONFIGURATION_GET(pageChangePanDistance)) {
        pageIndex = qMin(pageIndex + 1, pages.count() - 1);
    } else if (HSCONFIGURATION_GET(pageChangePanDistance) < mDeltaX) {
        pageIndex = qMax(pageIndex - 1, 0);
    }

    HsScene::instance()->setActivePageIndex(pageIndex);

    HsPage *page = HsScene::instance()->activePage();
    if (page) {
        QList<HsWidgetHost *> widgets = page->newWidgets();
        if (!widgets.isEmpty()) {
            foreach (HsWidgetHost *widget, widgets) {
                widget->startWidget();
            }
        }
    page->layoutNewWidgets();
    }

    startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(pageChangeAnimationDuration));
}

void HsIdleState::action_moveScene_disconnectGestureHandlers()
{
    HsScene *scene = HsScene::instance();
    scene->disconnect(this);
}

/*!
    Adds a new page to the scene.
*/
void HsIdleState::action_addPage_addPage()
{
    HsScene *scene = HsScene::instance();
	int pageIndex = scene->activePageIndex() + 1;
    addPageToScene(pageIndex);
    scene->setActivePageIndex(pageIndex);
    startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(newPageAddedAnimationDuration));
    HsGui::instance()->idleWidget()->pageIndicator()->addItem(pageIndex);
    HsGui::instance()->idleWidget()->showPageIndicator();
}

/*!
    Displays a confirmation query before page is removed if there are widgets on page 
*/
void HsIdleState::action_preRemovePage_showQuery()
{
    HsScene *scene = HsScene::instance();
    HsPage *page = scene->activePage();

    if (!page->widgets().isEmpty()) {
        if (!mMessageBoxWrapper) {
            mMessageBoxWrapper = new HsMessageBoxWrapper(this);
            mMessageBoxWrapper->setHeader(hbTrId(hsLocTextId_Title_RemovePage));
            mMessageBoxWrapper->setQueryText(hbTrId(hsLocTextId_Confirmation_RemovePage));
            connect(mMessageBoxWrapper,SIGNAL(accepted()),SIGNAL(event_removePage())); 
            connect(mMessageBoxWrapper,SIGNAL(rejected()),SIGNAL(event_waitInput())); 
        }
        mMessageBoxWrapper->show();
    } else {
        // Empty page can be removed without confirmation
        emit event_removePage();
    }
}

/*!
    Make sure confirmation query gets closed if user exits preRemovePage state e.g. by pressing application key
*/
void HsIdleState::action_preRemovePage_exit()
{
    if (mMessageBoxWrapper) {
        mMessageBoxWrapper->close();
    }
}

/*!
    Start remove page animation.
*/
void HsIdleState::action_removePage_startRemovePageAnimation()
{
    HsScene *scene = HsScene::instance();
   
    int pageToRemoveIndex = scene->activePageIndex();
    bool isLastPage(scene->activePage() == scene->pages().last());
    
    int nextPageIndex(pageToRemoveIndex);
   
    if (isLastPage) {
        nextPageIndex--; 
    } else {
        nextPageIndex++;
    }

    HsPropertyAnimationWrapper *animation = HsGui::instance()->pageChangeAnimation();
    if (animation->isRunning()) {
        animation->stop();
    }
    animation->disconnect(this);
    connect(animation,
            SIGNAL(finished()),
            SIGNAL(event_waitInput()), 
            Qt::UniqueConnection);
    animation->setEndValue(pageLayerXPos(nextPageIndex));
    animation->setDuration(HSCONFIGURATION_GET(pageRemovedAnimationDuration));
   
    animation->start();
    
    HbInstantFeedback::play(HSCONFIGURATION_GET(pageChangeFeedbackEffect));
    
}

/*!
    Removes an existing page from the scene.
*/
void HsIdleState::action_removePage_removePage()
{
    HsIdleWidget *idleWidget = HsGui::instance()->idleWidget();
    HsPropertyAnimationWrapper *animation = HsGui::instance()->pageChangeAnimation();
    HsScene *scene = HsScene::instance();
    HsPage *pageToRemove = scene->activePage();

    if (animation->isRunning()) {
        animation->stop();
    }
    // remove from ui
    idleWidget->removePage(pageToRemove->pageIndex());
    idleWidget->showPageIndicator();
    // update data model
    scene->removePage(pageToRemove);
    // Take new active page (previous was removed) and move scene to right position
    qreal x = pageLayerXPos(scene->activePageIndex());
    idleWidget->setSceneX(x);
    // delete it   
    pageToRemove->deleteLater();
   
}
/*!
    Toggles the homescreen online/offline state.
*/
void HsIdleState::action_toggleConnection_toggleConnection()
{
    HsScene *scene = HsScene::instance();
    scene->setOnline(!scene->isOnline());
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Open task switcher.
 \retval true if operation is successful.
 */
bool HsIdleState::openTaskSwitcher()
{
    return HsMenuService::launchTaskSwitcher();
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Page change zone animation has been finished.
 */
void HsIdleState::zoneAnimationFinished()
{
    HsScene *scene = HsScene::instance();
    int pageIndex = scene->activePageIndex();
    HsGui *gui(HsGui::instance());
    if (gui->pageCrawlingAnimation()->isForward()) {
        if (isInLeftPageChangeZone() &&
            0 < pageIndex) {
            --pageIndex;
        } else if (isInRightPageChangeZone() &&
                   pageIndex < scene->pages().count()) {
            ++pageIndex;
        }
        if (pageIndex == scene->pages().count()) {
            if (pageIndex < HSCONFIGURATION_GET(maximumPageCount)) {
                addPageToScene(pageIndex);
                gui->idleWidget()->showPageIndicator();
                gui->idleWidget()->pageIndicator()->addItem(pageIndex);
            }
        }
        scene->setActivePageIndex(pageIndex);
        startPageChangeAnimation(pageIndex, HSCONFIGURATION_GET(pageChangeAnimationDuration));
    } else {
        scene->setActivePageIndex(pageIndex);
        gui->idleWidget()->setActivePage(pageIndex);
    }
}

/*!
    Page change animation has been finished.
 */
void HsIdleState::pageChangeAnimationFinished()
{
    hbInstance->allMainWindows().first()->setInteractive(true);
    updateZoneAnimation();
}


/*!
    Provides the page presentation to the Widget Snap algorithm
*/
void HsIdleState::updatePagePresentationToWidgetSnap()
{
    QRectF containerRect = HsScene::instance()->activePage()->contentGeometry();
    HsWidgetHost *activeWidget = HsScene::instance()->activeWidget();
    HsWidgetPositioningOnWidgetMove::instance()->setPagePresentation(
        containerRect, createInactiveWidgetRects(), activeWidget->visual()->geometry());
}

/*!
    Reset Snap position, hide the snap lines
*/
void HsIdleState::resetSnapPosition()
{
    mSnapResult = HsWidgetPositioningOnWidgetMove::Result();
    mPreviousSnapResult = HsWidgetPositioningOnWidgetMove::Result();
    hideVerticalLine();
    hideHorizontalLine();
}

/*!
    Show the Vertical line for Snap guidance
*/
void HsIdleState::showVerticalLine()
{
    if (mPreviousSnapResult.verticalSnapLine.x1() != mSnapResult.verticalSnapLine.x1()) {
        hideVerticalLine();
        mVerticalSnapLineTimer.start();
    }
    else {
        //As the Vertical Line position is at the same place,
        //the timer was started when vertical line positions were at different locations
        //the line will be shown when the timer expires.
        //If timer has already expired, just show the line, which is redrawn to new geometry.
        if (!mVerticalSnapLineTimer.isActive()) {
            HsGui::instance()->idleWidget()->showVerticalSnapLine(mSnapResult.verticalSnapLine);
        }
    }
}

/*!
    Timer for showing the Vertical line expires
*/
void HsIdleState::onVerticalSnapLineTimerTimeout()
{
    HsGui::instance()->idleWidget()->showVerticalSnapLine(mSnapResult.verticalSnapLine);
}

/*!
    Hide the Vertical line for Snap guidance
*/
void HsIdleState::hideVerticalLine()
{
    HsGui::instance()->idleWidget()->hideVerticalSnapLine();
    mVerticalSnapLineTimer.stop();
}

/*!
    Show the Horizontal line for Snap guidance
*/
void HsIdleState::showHorizontalLine()
{
    if (mPreviousSnapResult.horizontalSnapLine.y1() != mSnapResult.horizontalSnapLine.y1()) {
        hideHorizontalLine();
        mHorizontalSnapLineTimer.start();
    }
    else {
        if (!mHorizontalSnapLineTimer.isActive()) {
            HsGui::instance()->idleWidget()->showHorizontalSnapLine(mSnapResult.horizontalSnapLine);
        }
    }
}

/*!
    Timer for showing the Horizontal line expires
*/
void HsIdleState::onHorizontalSnapLineTimerTimeout()
{
    HsGui::instance()->idleWidget()->showHorizontalSnapLine(mSnapResult.horizontalSnapLine);
}

/*!
    Hide the Horizontal line for Snap guidance
*/
void HsIdleState::hideHorizontalLine()
{
    HsGui::instance()->idleWidget()->hideHorizontalSnapLine();
    mHorizontalSnapLineTimer.stop();
}

/*!
    Handles updating the Snap algorithm with page presentation on page change
*/
void HsIdleState::updateSnapAlgorithmParameters()
{
    updatePagePresentationToWidgetSnap();
    resetSnapPosition();
}

/*!
    Closes virtual keyboard
*/
void HsIdleState::closeVirtualKeyboard()
{
    HbVkbHost::HbVkbStatus status = HbVkbHostBridge::instance()->keypadStatus();
    if (status == HbVkbHost::HbVkbStatusOpened ) {
        QInputContext *ic = qApp->inputContext();
        if (ic) {
            QEvent *event = new QEvent(QEvent::CloseSoftwareInputPanel);
            ic->filterEvent(event);
            delete event;
        }
    }
}

/*!
    Returns true if \a widget has editor field under \a point.
*/
bool HsIdleState::isEditor(const QPointF &point, HsWidgetHost *widget)
{
    bool isWidgetEditor = false;
    QList<QGraphicsItem *> items;
    if ( widget->visual()->scene() ) {
        items = widget->visual()->scene()->items(point);
        }
    int count = items.count();
    for (int i=0; i<count && !isWidgetEditor; i++ ) {
        QGraphicsItem *item = items.at(i);
        if (item->isWidget()) {
            isWidgetEditor = HbInputFocusObject::isEditor(item->toGraphicsObject()) && 
                qobject_cast<QGraphicsWidget *>(widget->visual()->widget())->isAncestorOf(item);
        }
    }
    return isWidgetEditor;
}

/*!
    Stops page change and page crawling animations.
*/
void HsIdleState::stopPageChangeAnimations()
{
    HsPropertyAnimationWrapper *animation = HsGui::instance()->pageChangeAnimation();
    if (animation->isRunning()) {
        animation->stop();
        hbInstance->allMainWindows().first()->setInteractive(true);
    }        
    animation->disconnect(this);
    
    HsPropertyAnimationWrapper *pageCrawlingAnimation = 
        HsGui::instance()->pageCrawlingAnimation();

    if (pageCrawlingAnimation->isRunning()) {
        pageCrawlingAnimation->stop();
    }
}

