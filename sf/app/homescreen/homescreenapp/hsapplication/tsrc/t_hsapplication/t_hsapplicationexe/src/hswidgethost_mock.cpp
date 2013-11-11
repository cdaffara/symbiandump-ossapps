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

#include <QGesture>

#include <HbTouchArea>

#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hswidgettoucharea.h"

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
}

HsWidgetHost::~HsWidgetHost()
{
}

HsWidgetHost *HsWidgetHost::createInstance(HsWidgetData &widgetData,
                                           const QVariantHash &preferences)
{
    Q_UNUSED(widgetData);
    Q_UNUSED(preferences);
    return 0;
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
    Q_UNUSED(orientation);
    return true;
}

bool HsWidgetHost::savePresentation()
{
    return true;
}

bool HsWidgetHost::savePresentation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    return true;
}

bool HsWidgetHost::savePresentation(HsWidgetPresentationData &presentation)
{
    Q_UNUSED(presentation);
    return true;
}

bool HsWidgetHost::getPresentation(HsWidgetPresentationData &presentation)
{
    Q_UNUSED(presentation);
    return true;
}

bool HsWidgetHost::removePresentation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
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
    Q_UNUSED(online);
}

void HsWidgetHost::remove()
{
}

void HsWidgetHost::close()
{
}

void HsWidgetHost::startDragEffect()
{
}

void HsWidgetHost::startDropEffect()
{
}

void HsWidgetHost::setupStates()
{
}

bool HsWidgetHost::setProperty(const char *name, QMetaProperty &property)
{
    Q_UNUSED(name);
    Q_UNUSED(property);
    return true;
}

bool HsWidgetHost::setMethod(const char *signature, QMetaMethod &method)
{
    Q_UNUSED(signature);
    Q_UNUSED(method);
    return true;
}

bool HsWidgetHost::hasSignal(const char *signature)
{
    Q_UNUSED(signature);
    return true;
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
}

void HsWidgetHost::action_initialize()
{
}

void HsWidgetHost::action_uninitialize()
{
}

void HsWidgetHost::action_show()
{
}

void HsWidgetHost::action_hide()
{
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
}

void HsWidgetHost::action_notifyRemove()
{
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
    Q_UNUSED(names);
}

HsWidgetHostVisual *HsWidgetHost::visual() const
{
    return mVisual;
}
