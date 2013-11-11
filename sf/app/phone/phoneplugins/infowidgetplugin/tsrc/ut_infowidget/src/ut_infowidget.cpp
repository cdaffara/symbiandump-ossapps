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
#include <QString>
#include <QPainter>
#include <QEvent>
#include <QGesture>
#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <HbMarqueeItem>
#include <HbPushButton>
#include <HbCheckBox>
#include <HbEvent>
#include <HbColorScheme>
#include <hbframedrawer.h>
#include "ut_infowidget.h"
#include "qtestmains60.h"
#include <HbTapGesture>
#include <HbAction>

// mocked dependencies
#include "infowidgetpreferences.h"

#define private friend class UT_InfoWidget;private
#include "infowidget.h"


const QString KPreferenceOn("On"); 
const QString KPreferenceOff("Off"); 
const QString KSpnName("testprovider");
const QString KMcnName("tnw1: 3233255435"); 
const QString KSatText("satdisplaytesttext"); 


// Stubbed functions 
void QGraphicsWidget::resize( 
        const QSizeF & size ) 
{
    Q_UNUSED(size); 
}
void QGraphicsWidget::updateGeometry() 
{
}

/*!
  UT_InfoWidget::UT_InfoWidget
 */
UT_InfoWidget::UT_InfoWidget() 
    : m_infoWidget(0)
{
    m_modelData.setActiveLine(1);
    m_modelData.setHomeZoneIndicatorType(1);
    m_modelData.setMcnName(KMcnName);
    m_modelData.setServiceProviderName(KSpnName);
    m_modelData.setSatDisplayText(KSatText);
    
    SmcDefaultValue<const InfoWidgetEngine::ModelData &>::SetL(m_modelData);
    SmcDefaultValue<QColor>::SetL(m_qcolor);
    SmcDefaultValue<QGraphicsWidget*>::SetL(&m_qgraphicswidget);
}

/*!
  UT_InfoWidget::~UT_InfoWidget
 */
UT_InfoWidget::~UT_InfoWidget()
{
    delete m_infoWidget;
}

/*!
  UT_InfoWidget::init
 */
void UT_InfoWidget::init()
{
    initialize();
    
    SmcDefaultValue<QString>::SetL("On");
    SmcDefaultValue<InfoWidgetLayoutManager::DisplayRole>::SetL(
        InfoWidgetLayoutManager::InfoDisplay);
    m_infoWidget = new InfoWidget();
    // Give time for Qt & Orbit method calls invoked in InfoWidget's
    // constructor to complete.
    //QTest::qWait(300);
    SmcDefaultValue<QString>::SetL("");

    QVERIFY(verify());
}

/*!
  UT_InfoWidget::cleanup
 */
void UT_InfoWidget::cleanup()
{
    reset();
    
    delete m_infoWidget;
    m_infoWidget = 0;
}

/*!
  UT_InfoWidget::t_boundingRect
 */
void UT_InfoWidget::t_boundingRect()
{
    m_infoWidget->boundingRect();
}

/*!
  UT_InfoWidget::t_shape
 */
void UT_InfoWidget::t_shape()
{
    m_infoWidget->shape();
}


/*!
  UT_InfoWidget::t_sizePolicy
 */
void UT_InfoWidget::t_sizePolicy()
{
    QVERIFY(m_infoWidget->sizePolicy() == 
        QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

/*!
  UT_InfoWidget::t_setSpnDisplay
 */
void UT_InfoWidget::t_setSpnDisplay()
{
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySpn, KPreferenceOn);
    
    m_infoWidget->setSpnDisplay(KPreferenceOn);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_setMcnDisplay
 */
void UT_InfoWidget::t_setMcnDisplay()
{
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayMcn, KPreferenceOn);
    
    m_infoWidget->setMcnDisplay(KPreferenceOn);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_setSatDisplay
 */
void UT_InfoWidget::t_setSatDisplay()
{
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySatText, KPreferenceOn);
    
    m_infoWidget->setSatDisplay(KPreferenceOn);
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_paint
 */
void UT_InfoWidget::t_paint()
{
    QScopedPointer<QPainter> painter(new QPainter());
    QStyleOptionGraphicsItem *option = NULL;
    QWidget *widget = NULL;
    
    m_infoWidget->paint(painter.data(), option, widget);
    
    // layout change ongoing while paint requested
    m_infoWidget->startChanges();
    m_infoWidget->paint(painter.data(), option, widget);
}


/*!
  UT_InfoWidget::t_updateInfoDisplay
 */
void UT_InfoWidget::t_updateInfoDisplay()
{
    HbMarqueeItem spnMarqueeItem; 
    HbMarqueeItem mcnMarqueeItem;
    HbMarqueeItem satMarqueeItem; 
    QGraphicsWidget* pointerSpnMarqueeItem = qobject_cast<QGraphicsWidget *>(&spnMarqueeItem);
    QGraphicsWidget* pointerMcnMarqueeItem = qobject_cast<QGraphicsWidget *>(&mcnMarqueeItem);
    QGraphicsWidget* pointerSatMarqueeItem = qobject_cast<QGraphicsWidget *>(&satMarqueeItem);

    // Precondition: 
    m_infoWidget->m_initialized = true; 
    
    InfoWidgetEngine::ModelData modelData;
    modelData.setMcnName("testmcn"); 
    modelData.setServiceProviderName("testspn");  
    modelData.setSatDisplayText("testsat"); 
    const InfoWidgetEngine::ModelData& modelDataConstRef(modelData); 
    
    // Test 1: text data has been initialized to model (in UT_InfoWidget::UT_InfoWidget())
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole).returns(InfoWidgetLayoutManager::InfoDisplay);
    EXPECT(InfoWidgetEngine, modelData).returns(modelDataConstRef);

    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSpnMarqueeItem)
        .returns(pointerSpnMarqueeItem);
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(false);

    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleMcnMarqueeItem)
        .returns(pointerSpnMarqueeItem);
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(false);

    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSatMarqueeItem)
        .returns(pointerSatMarqueeItem);
    
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(false);
 
    m_infoWidget->updateInfoDisplay();
    QVERIFY(m_infoWidget->m_animatingItems.count() == 3); 
    
    QVERIFY(verify());
    
    // Test 2: empty data, verify items are not appended to m_animatingItems
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole).returns(InfoWidgetLayoutManager::InfoDisplay);
    EXPECT(InfoWidgetEngine, modelData).returns(modelDataConstRef);

    m_modelData.setServiceProviderName("testspn");
    m_modelData.setMcnName("testmcn");
    m_modelData.setSatDisplayText("testsat");
    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSpnMarqueeItem)
        .returns(pointerSpnMarqueeItem);
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(true);
    
    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleMcnMarqueeItem)
        .returns(pointerSpnMarqueeItem);
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(true);

    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSatMarqueeItem)
        .returns(pointerSatMarqueeItem);
    EXPECT(HbMarqueeItem, setText);
    EXPECT(HbMarqueeItem, setTextColor);
    EXPECT(QGraphicsWidget, adjustSize); 
    EXPECT(InfoWidgetLayoutManager, textFitsToRect)
        .returns(true);

    m_infoWidget->updateInfoDisplay();
    QVERIFY(m_infoWidget->m_animatingItems.count() == 0); 
    
    QVERIFY(verify());
    
    // Change state: 
    m_infoWidget->m_initialized = false; 
    m_infoWidget->updateInfoDisplay();
    QVERIFY(m_infoWidget->m_animatingItems.count() == 0); 
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_readPersistentPreferences
 */
void UT_InfoWidget::t_readPersistentPreferences()
{    
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayHomeZone, QString(""));

    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayMcn, QString(""));
    
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayActiveLine, QString(""));
    
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySatText, QString(""));

    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySpn, QString(""));
    
    EXPECT(InfoWidgetPreferences, validate);

    m_infoWidget->readPersistentPreferences();
    
    QVERIFY(verify());
    
    // one visible item initially
    EXPECT(InfoWidgetPreferences, validate).returns(true);
    EXPECT(InfoWidgetPreferences, storePreferences);
    
    m_infoWidget->readPersistentPreferences();
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_onInitialize
 */
void UT_InfoWidget::t_onInitialize()
{    
    QGraphicsWidget *contentWidget = NULL; 

    // Test onInitialize sequence where 
    // meta-object properties have been 
    // initialized 
    EXPECT(InfoWidgetPreferences, validate)
        .returns(true);
    EXPECT(InfoWidgetPreferences, storePreferences)
        .returns(true);
    EXPECT(InfoWidgetLayoutManager, contentWidget)
        .returns(contentWidget);
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay)
        .returns(contentWidget);
      
    m_infoWidget->onInitialize();
    
    // Test onInitialize sequence where 
    // meta-object properties are 
    // uninitialized 
    EXPECT(InfoWidgetPreferences, validate)
        .returns(false);
    EXPECT(InfoWidgetLayoutManager, contentWidget)
        .returns(contentWidget);
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay)
        .returns(contentWidget);
    m_infoWidget->onInitialize();
}

/*!
  UT_InfoWidget::t_onUninitialize
 */
void UT_InfoWidget::t_onUninitialize()
{
    m_infoWidget->onUninitialize(); 
}

/*!
  UT_InfoWidget::t_onShow
 */
void UT_InfoWidget::t_onShow()
{
    
    m_infoWidget->m_animationState = InfoWidget::AnimationIdle; 
    EXPECT(InfoWidgetEngine, resume);
    m_infoWidget->onShow();
    
    m_infoWidget->m_animationState = InfoWidget::AnimationOngoing;
    EXPECT(InfoWidgetEngine, resume);
    m_infoWidget->onShow();
    
    m_infoWidget->m_animationState = InfoWidget::AnimationStarting;
    EXPECT(InfoWidgetEngine, resume);
    m_infoWidget->onShow(); 

}

/*!
  UT_InfoWidget::t_onHide
 */
void UT_InfoWidget::t_onHide()
{
    m_infoWidget->onHide(); 
}

/*!
  UT_InfoWidget::t_readModel
 */
void UT_InfoWidget::t_readModel()
{
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole)
        .returns(InfoWidgetLayoutManager::InfoDisplay);
    m_infoWidget->readModel();
    
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole)
        .returns(InfoWidgetLayoutManager::SettingsDialog);
    m_infoWidget->readModel();
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_handleModelError
 */
void UT_InfoWidget::t_handleModelError()
{
    int operation(0);
    int errorCode(0); 
    m_infoWidget->handleModelError(operation, errorCode); 
}

void UT_InfoWidget::t_spnDisplaySettingChanged()
{
    // user enables home zone setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySpn, KPreferenceOn);
    int setting(Qt::Checked);
    m_infoWidget->spnDisplaySettingChanged(setting);
    
    // user disables home zone setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySpn, KPreferenceOff);
    setting = Qt::Unchecked;
    m_infoWidget->spnDisplaySettingChanged(setting);
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_mcnDisplaySettingChanged()
{
    // user enables MCN setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayMcn, KPreferenceOn);
    int setting(Qt::Checked);
    m_infoWidget->mcnDisplaySettingChanged(setting);
    
    // user disables MCN setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplayMcn, KPreferenceOff);
    setting = Qt::Unchecked;
    m_infoWidget->mcnDisplaySettingChanged(setting);
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_satDisplaySettingChanged()
{
    // user enables SAT setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySatText, KPreferenceOn);
    int setting(Qt::Checked);
    m_infoWidget->satDisplaySettingChanged(setting);
    
    // user disables SAT setting
    EXPECT(InfoWidgetPreferences, setPreference)
        .with(InfoWidgetPreferences::DisplaySatText, KPreferenceOff);
    setting = Qt::Unchecked;
    m_infoWidget->satDisplaySettingChanged(setting);
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_settingsEditingFinished()
{
    EXPECT(InfoWidgetPreferences, validate)
        .returns(true);
    EXPECT(InfoWidgetPreferences, storePreferences)
        .returns(false);

    QGraphicsWidget *contentWidget = NULL; 
    EXPECT(InfoWidgetLayoutManager, contentWidget)
        .returns(contentWidget);

    m_infoWidget->settingsEditingFinished(); 
}

void UT_InfoWidget::t_updateItemsVisibility()
{
    // all items visible
    EXPECT(InfoWidgetPreferences, preference)
        .with(InfoWidgetPreferences::DisplaySpn)
        .returns(KPreferenceOn);
    EXPECT(InfoWidgetPreferences, preference)
        .with(InfoWidgetPreferences::DisplayMcn)
        .returns(KPreferenceOn);
    EXPECT(InfoWidgetPreferences, preference)
        .with(InfoWidgetPreferences::DisplaySatText)
        .returns(KPreferenceOn);
    
    m_infoWidget->updateItemsVisibility();
    QVERIFY(verify());

}

void UT_InfoWidget::t_layoutInfoDisplay()
{
    QGraphicsWidget *contentWidget = NULL; 
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay)
        .returns(contentWidget);
    m_infoWidget->layoutInfoDisplay();
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_layoutSettingsDialog()
{
    QGraphicsWidget *contentWidget = NULL; 
    EXPECT(InfoWidgetLayoutManager, layoutSettingsDialog)
        .returns(contentWidget);
    m_infoWidget->layoutSettingsDialog();
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_initializeSettingsDialogItems()
{
    QGraphicsWidget spnCheckBox;
    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSpnCheckBox)
        .returns(&spnCheckBox);

    QGraphicsWidget mcnCheckBox;
    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleMcnCheckBox)
        .returns(&mcnCheckBox);
    
    QGraphicsWidget satTextCheckBox;
    EXPECT(InfoWidgetLayoutManager, getWidget)
        .with(InfoWidgetLayoutManager::RoleSatTextCheckBox)
        .returns(&satTextCheckBox);
    
    m_infoWidget->initializeSettingsDialogItems();
    
    QVERIFY(verify());
}

void UT_InfoWidget::t_startChanges()
{
    m_infoWidget->startChanges();
}

void UT_InfoWidget::t_endChanges()
{
    m_infoWidget->endChanges();
}

/*!
  UT_InfoWidget::t_changeEvent
 */
void UT_InfoWidget::t_changeEvent()
{
    /*
     * Not work from commmand prompt.
     */
    HbEvent event(HbEvent::ThemeChanged);
    EXPECT(HbWidget, changeEvent).with(&event);
    m_infoWidget->changeEvent(&event);
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_sizeHint
 */
void UT_InfoWidget::t_sizeHint()
{   
    const QSizeF KMinimumSize(33.5, 160);
    const QSizeF KPreferredSize(134, 160);
    const QSizeF KDefaultSizeInfoDisplay(200, 100);
    const QSizeF KDefaultSizeSettingsDisplay(230, 220);
    QScopedPointer<QGraphicsWidget> contentWidget(NULL); 
    
    // Test: initial size after construction  
    Qt::SizeHint sizeHint = Qt::PreferredSize;
    QSizeF constraint = QSizeF();
    QVERIFY(KDefaultSizeInfoDisplay == m_infoWidget->sizeHint(
            sizeHint, constraint));

    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay)
        .returns(contentWidget.data());
    // Test: size after onInitialize is called 
    // and current display is InfoDisplay 
    m_infoWidget->onInitialize();

    QVERIFY(verify());
    
    contentWidget.reset(new QGraphicsWidget);
    contentWidget->setMinimumSize(KMinimumSize); 
    EXPECT(InfoWidgetLayoutManager, contentWidget)
        .returns(contentWidget.data());
    EXPECT(InfoWidgetPreferences, visibleItemCount).
            returns(1);
    
    // No strict verification.
    // Data in docml is in "un" format and strict comparison to pixels won't work  
    QVERIFY(KDefaultSizeInfoDisplay != m_infoWidget->sizeHint(sizeHint, constraint));
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_startMarquees
 */
void UT_InfoWidget::t_startMarquees()
{
    bool started(false);  
    HbMarqueeItem spnMarqueeItem;
    HbMarqueeItem mcnMarqueeItem;
    HbMarqueeItem satMarqueeItem; 
    
    // Initialize:
    m_infoWidget->m_animatingItems.append(&spnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&mcnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&satMarqueeItem); 
    
    // Test 1: 
    m_infoWidget->m_animationState = InfoWidget::AnimationIdle;
    started = m_infoWidget->startMarquees();
    QVERIFY(started);
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationStarting);
    
    // Test 2:
    m_infoWidget->m_animationState = InfoWidget::AnimationStarting; 
    started = m_infoWidget->startMarquees();
    QVERIFY(!started);
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationStarting); 

    // Test 3:
    m_infoWidget->m_animationState = InfoWidget::AnimationOngoing; 
    started = m_infoWidget->startMarquees();
    QVERIFY(!started);
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationOngoing); 

    QVERIFY(verify());   
}

/*!
  UT_InfoWidget::t_stopMarquees
 */
void UT_InfoWidget::t_stopMarquees()
{
    HbMarqueeItem spnMarqueeItem;
    HbMarqueeItem mcnMarqueeItem;
    HbMarqueeItem satMarqueeItem; 
    
    // Initialize:
    m_infoWidget->m_animatingItems.append(&spnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&mcnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&satMarqueeItem); 
 
    // Test 1: 
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    m_infoWidget->m_animationState = InfoWidget::AnimationOngoing; 
    m_infoWidget->stopMarquees();
    
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationIdle); 
    QVERIFY(m_infoWidget->m_animatingItems.count() == 0); 
    QVERIFY(!m_infoWidget->m_animatingItem); 

 
    // Test 2:
    // Re-initialize:
    m_infoWidget->m_animatingItems.append(&spnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&mcnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&satMarqueeItem); 
    
    EXPECT(HbMarqueeItem, isAnimating).
            returns(true);
    EXPECT(HbMarqueeItem, isAnimating).
            returns(true);
    EXPECT(HbMarqueeItem, isAnimating).
            returns(true);

    m_infoWidget->m_animationState = InfoWidget::AnimationOngoing; 
    m_infoWidget->stopMarquees();
    
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationIdle); 
    QVERIFY(m_infoWidget->m_animatingItems.count() == 0); 
    QVERIFY(!m_infoWidget->m_animatingItem); 

    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_marqueeNext
 */
void UT_InfoWidget::t_marqueeNext()
{
    
    HbMarqueeItem spnMarqueeItem;
    spnMarqueeItem.setObjectName("id:spnMarqueeItem");
    HbMarqueeItem mcnMarqueeItem;
    spnMarqueeItem.setObjectName("id:mcnMarqueeItem");
    HbMarqueeItem satMarqueeItem; 
    spnMarqueeItem.setObjectName("id:satMarqueeItem");
    
    // Initialize:
    m_infoWidget->m_animatingItems.append(&spnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&mcnMarqueeItem);
    m_infoWidget->m_animatingItems.append(&satMarqueeItem); 
    m_infoWidget->m_animationState = InfoWidget::AnimationOngoing;
 
    // Test 1: verify loop functionality
    m_infoWidget->m_animatingItem = &spnMarqueeItem;
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    m_infoWidget->marqueeNext();
    QVERIFY(m_infoWidget->m_animatingItem == &mcnMarqueeItem); 

    // Test 2: verify loop functionality
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    m_infoWidget->marqueeNext();
    QVERIFY(m_infoWidget->m_animatingItem == &satMarqueeItem); 

    // Test 3: verify loop functionality, loop to begin
    EXPECT(HbMarqueeItem, isAnimating).
            returns(false);
    m_infoWidget->marqueeNext();
    QVERIFY(m_infoWidget->m_animatingItem == &spnMarqueeItem); 
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_timerEvent
 */
void UT_InfoWidget::t_timerEvent()
{
    // Test 1: 
    QScopedPointer<QTimerEvent> timerEvent(new QTimerEvent(0));
    m_infoWidget->m_animationState = InfoWidget::AnimationStarting; 
    HbMarqueeItem animatingItem; 
    if (!m_infoWidget->m_animatingItem) {
        m_infoWidget->m_animatingItem = &animatingItem; 
    }
    
    m_infoWidget->timerEvent(timerEvent.data());
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationOngoing); 

    // Test 2: 
    m_infoWidget->m_animationState = InfoWidget::AnimationStarting; 
    m_infoWidget->m_animatingItem = NULL;  
    m_infoWidget->timerEvent(timerEvent.data());
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationStarting); 

    // Test 3: 
    m_infoWidget->m_animationState = InfoWidget::AnimationIdle; 
    QVERIFY(m_infoWidget->m_animationState == InfoWidget::AnimationIdle); 
    
    QVERIFY(verify());
}

/*!
  UT_InfoWidget::t_gestureEvent
 */
void UT_InfoWidget::t_gestureEvent()
{
    QList<QGesture *> gestures;
    HbTapGesture gesture;
    gestures.append(&gesture);
    QGestureEvent event(gestures); 

    EXPECT(QGestureEvent, gesture).returns(&gesture);
    EXPECT(QGesture, state).returns(Qt::GestureFinished);
    EXPECT(HbTapGesture, tapStyleHint).returns(HbTapGesture::Tap);
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole).returns(
        InfoWidgetLayoutManager::InfoDisplay);
    m_infoWidget->gestureEvent(&event);
    QVERIFY(verify());

    EXPECT(QGestureEvent, gesture).returns(&gesture);
    EXPECT(QGesture, state).returns(Qt::GestureFinished);
    EXPECT(HbTapGesture, tapStyleHint).returns(HbTapGesture::Tap);
    EXPECT(InfoWidgetLayoutManager, currentDisplayRole).returns(
        InfoWidgetLayoutManager::SettingsDialog);
    m_infoWidget->gestureEvent(&event);
    QVERIFY(verify());

    EXPECT(QGestureEvent, gesture).returns(&gesture);
    EXPECT(QGesture, state).returns(Qt::GestureFinished);
    EXPECT(HbTapGesture, tapStyleHint).returns(HbTapGesture::TapAndHold);
    m_infoWidget->gestureEvent(&event);
    QVERIFY(verify());

    EXPECT(QGestureEvent, gesture).returns(&gesture);
    EXPECT(QGesture, state).returns(Qt::NoGesture);
    m_infoWidget->gestureEvent(&event);
    QVERIFY(verify());

    EXPECT(QGestureEvent, gesture).returns<HbTapGesture *>(0);
    m_infoWidget->gestureEvent(&event);
    QVERIFY(verify());
}

void UT_InfoWidget::t_setHomeZoneDisplay()
{
    m_infoWidget->setHomeZoneDisplay(QString(""));
}

void UT_InfoWidget::t_setActiveLineDisplay()
{
    m_infoWidget->setActiveLineDisplay(QString(""));
}

void UT_InfoWidget::t_initializeCheckBoxStates()
{
    EXPECT(InfoWidgetLayoutManager, getWidget).returns<QGraphicsWidget *>(0);
    EXPECT(InfoWidgetLayoutManager, getWidget).returns<QGraphicsWidget *>(0);
    EXPECT(InfoWidgetLayoutManager, getWidget).returns<QGraphicsWidget *>(0);
    m_infoWidget->initializeCheckBoxStates();
    QVERIFY(verify());

    HbCheckBox box;

    EXPECT(InfoWidgetLayoutManager, getWidget).returns(&box);
    EXPECT(InfoWidgetPreferences, isPreferenceSet).returns(true);
    EXPECT(HbAbstractButton, setChecked);
    EXPECT(InfoWidgetLayoutManager, getWidget).returns(&box);
    EXPECT(InfoWidgetPreferences, isPreferenceSet).returns(true);
    EXPECT(HbAbstractButton, setChecked);
    EXPECT(InfoWidgetLayoutManager, getWidget).returns(&box);
    EXPECT(InfoWidgetPreferences, isPreferenceSet).returns(true);
    EXPECT(HbAbstractButton, setChecked);
    m_infoWidget->initializeCheckBoxStates();
    QVERIFY(verify());
}

void UT_InfoWidget::t_settingsEditingCancelled()
{
    m_infoWidget->settingsEditingCancelled();
}

void UT_InfoWidget::t_settingsDialogClosed()
{
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay).returns<QGraphicsWidget *>(0);
    m_infoWidget->settingsDialogClosed(0);
    QVERIFY(verify());

    HbAction action;
    EXPECT(QAction, text).returns(hbTrId("txt_common_button_ok"));
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay).returns<QGraphicsWidget *>(0);
    m_infoWidget->settingsDialogClosed(&action);
    QVERIFY(verify());

    EXPECT(QAction, text).returns(hbTrId("txt_common_button_cancel"));
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay).returns<QGraphicsWidget *>(0);
    m_infoWidget->settingsDialogClosed(&action);
    QVERIFY(verify());

    EXPECT(QAction, text).returns(hbTrId("blah"));
    EXPECT(InfoWidgetLayoutManager, layoutInfoDisplay).returns<QGraphicsWidget *>(0);
    m_infoWidget->settingsDialogClosed(&action);
    QVERIFY(verify());

}

void UT_InfoWidget::t_settingsValidationFailed()
{
    m_infoWidget->m_initialized = true;
    m_infoWidget->settingsValidationFailed();

    m_infoWidget->m_initialized = false;
    m_infoWidget->settingsValidationFailed();
}

QTEST_MAIN_S60(UT_InfoWidget)
