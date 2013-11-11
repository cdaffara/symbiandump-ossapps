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

#include <QApplication>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QGraphicsLinearLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneResizeEvent>
#include <QGesture>
#include <QGraphicsScene>

#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbInstantFeedback>
#include <HbTouchArea>

#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswidgettoucharea.h"
#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include "hsconfiguration.h"
#include "hscontentservice.h"
#include "testwidget.h"

// Helper macros for connecting state entry and exit actions.
#define ENTRY_ACTION(state, action) \
    connect(state, SIGNAL(entered()), SLOT(action()));
#define EXIT_ACTION(state, action) \
    connect(state, SIGNAL(exited()), SLOT(action()));

QTM_USE_NAMESPACE

HsWidgetHost::HsWidgetHost(int databaseId, QObject *parent)
  : QObject(parent),
    mDatabaseId(databaseId),
    mVisual(new HsWidgetHostVisual),    
    mStateMachine(0),
    mWidget(0),
    mPage(0),
    mComponent(0),
    mIsFinishing(false)
{
    setupStates();
}

HsWidgetHost::~HsWidgetHost()
{
    delete mVisual;
}

HsWidgetHost *HsWidgetHost::createInstance(HsWidgetData &widgetData, 
                                           const QVariantHash &preferences)
{
    Q_UNUSED(widgetData);
    Q_UNUSED(preferences);
    HsWidgetHost *host = NULL;
    host = new HsWidgetHost(-1);
    return host;
}

int HsWidgetHost::databaseId() const
{
    return mDatabaseId;
}

bool HsWidgetHost::setPage(HsPage *page)
{
    mPage = page;
    return true;
}
 
HsPage *HsWidgetHost::page() const
{
    return mPage;
}

bool HsWidgetHost::loadPresentation()
{
    return true;
}

bool HsWidgetHost::loadPresentation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
    return true;
}

bool HsWidgetHost::savePresentation()
{
    return true;
}

bool HsWidgetHost::savePresentation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
    return true;
}

bool HsWidgetHost::savePresentation(HsWidgetPresentationData &presentation)
{
    Q_UNUSED(presentation)
    return true;
}

bool HsWidgetHost::getPresentation(HsWidgetPresentationData &presentation)
{
    Q_UNUSED(presentation)
    return true;
}

bool HsWidgetHost::removePresentation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
    return true;
}


void HsWidgetHost::startWidget(bool show)
{
    if (show) {
        emit event_startAndShow();
    } else {
        emit event_startAndHide();
    }
}
 
void HsWidgetHost::showWidget()
{
    emit event_show();
}
    
void HsWidgetHost::hideWidget()
{
    emit event_hide();
}

void HsWidgetHost::setOnline(bool online)
{
    mIsOnlineProperty.write(mWidget, online);
}

void HsWidgetHost::remove()
{
    emit event_remove();
}
 
void HsWidgetHost::close()
{
    emit event_close();
}



void HsWidgetHost::setupStates()
{
    // State machine

    mStateMachine = new QStateMachine(this);
    mStateMachine->setAnimated(false);
    
    // States

    QState *state_component = new QState;
    QState *state_unloaded = new QState(state_component);
    QState *state_running = new QState(state_component);
    QState *state_show = new QState(state_running);
    QState *state_hide = new QState(state_running);
    QState *state_finished = new QState;
    QState *state_faulted = new QState;
    QState *state_remove = new QState;
    QFinalState *state_final = new QFinalState;

    mStateMachine->addState(state_component);
    mStateMachine->addState(state_finished);
    mStateMachine->addState(state_faulted);
    mStateMachine->addState(state_remove);
    mStateMachine->addState(state_final);

    mStateMachine->setInitialState(state_component);
    state_component->setInitialState(state_unloaded);
    state_running->setInitialState(state_hide);

    // Transitions

    state_component->addTransition(
        this, SIGNAL(event_close()), state_final);
    state_component->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_component->addTransition(
        this, SIGNAL(event_finished()), state_finished);
    state_component->addTransition(
        this, SIGNAL(event_faulted()), state_faulted);

    state_unloaded->addTransition(
        this, SIGNAL(event_startAndShow()), state_show);
    state_unloaded->addTransition(
        this, SIGNAL(event_startAndHide()), state_hide);

    state_running->addTransition(
        this, SIGNAL(event_unload()), state_unloaded);
    
    state_show->addTransition(
        this, SIGNAL(event_hide()), state_hide);

    state_hide->addTransition(
        this, SIGNAL(event_show()), state_show);

    state_finished->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_finished->addTransition(
        this, SIGNAL(event_close()), state_final);

    state_faulted->addTransition(
        this, SIGNAL(event_remove()), state_remove);
    state_faulted->addTransition(
        this, SIGNAL(event_close()), state_final);

    state_remove->addTransition(state_final);

    // Actions

    ENTRY_ACTION(state_component, action_connectComponent)
    EXIT_ACTION(state_component, action_disconnectComponent)

    ENTRY_ACTION(state_running, action_load)
    ENTRY_ACTION(state_running, action_initialize)
    EXIT_ACTION(state_running, action_uninitialize)
    EXIT_ACTION(state_running, action_unload)

    ENTRY_ACTION(state_show, action_show)

    ENTRY_ACTION(state_hide, action_hide)

    ENTRY_ACTION(state_finished, action_finished)

    ENTRY_ACTION(state_faulted, action_faulted)

    ENTRY_ACTION(state_remove, action_notifyRemove)
    ENTRY_ACTION(state_remove, action_remove)

    // Delete on finish.

    connect(mStateMachine, SIGNAL(finished()), SLOT(deleteLater()), 
            Qt::QueuedConnection);
}

bool HsWidgetHost::setProperty(const char *name, QMetaProperty &property)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfProperty(name);
    property = object->property(index);
    return index >= 0;
}

bool HsWidgetHost::setMethod(const char *signature, QMetaMethod &method)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfMethod(
        QMetaObject::normalizedSignature(signature));
    method = object->method(index);
    return index >= 0;
}

bool HsWidgetHost::hasSignal(const char *signature)
{
    const QMetaObject *object = mWidget->metaObject();
    int index = object->indexOfSignal(
        QMetaObject::normalizedSignature(signature));
    return index >= 0;
}


bool HsWidgetHost::setPreferencesToWidget()
{
    return true;
}

void HsWidgetHost::action_connectComponent()
{    
}

void HsWidgetHost::action_disconnectComponent()
{
}

void HsWidgetHost::action_load()
{   

}

void HsWidgetHost::action_unload()
{
    delete mWidget;
    mWidget = 0;

    mOnInitializeMethod = QMetaMethod();
    mOnShowMethod = QMetaMethod();
    mOnHideMethod = QMetaMethod();
    mOnUninitializeMethod = QMetaMethod();    
    mIsOnlineProperty = QMetaProperty();
	mRootPathProperty = QMetaProperty();    
}

void HsWidgetHost::action_initialize()
{    
    setPreferencesToWidget();
    setOnline(HsScene::instance()->isOnline());
    mOnInitializeMethod.invoke(mWidget);
}

void HsWidgetHost::action_uninitialize()
{
    mOnUninitializeMethod.invoke(mWidget);
}

void HsWidgetHost::action_show()
{
    if (!mIsFinishing) {
        mOnShowMethod.invoke(mWidget);
    }
}

void HsWidgetHost::action_hide()
{
    if (!mIsFinishing) {
        mOnHideMethod.invoke(mWidget);
    }
}

void HsWidgetHost::action_finished()
{
    emit finished();
}

void HsWidgetHost::action_faulted()
{
    emit faulted();
}

void HsWidgetHost::action_remove()
{   
    mDatabaseId = -1;
}

void HsWidgetHost::action_notifyRemove()
{
    //Empty at mock
}

void HsWidgetHost::onFinished()
{
    mIsFinishing = true;
    emit event_finished();
}
 
void HsWidgetHost::onError()
{
    mIsFinishing = true;
    emit event_faulted();
}

void HsWidgetHost::onSetPreferences(const QStringList &names)
{
    Q_UNUSED(names)
}
/*!
    Starts the drag effect.
*/
void HsWidgetHost::startDragEffect()
{   
}

/*!
    Starts the drop effect.
*/
void HsWidgetHost::startDropEffect()
{    
}
HsWidgetHostVisual *HsWidgetHost::visual() const
{
    return mVisual;
}

