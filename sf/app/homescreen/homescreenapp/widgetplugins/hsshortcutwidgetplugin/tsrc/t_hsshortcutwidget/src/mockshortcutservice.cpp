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
* Description:  Entry point for shortcut widget
*
*/

#include <QStateMachine>

#include "hsshortcutservice.h"
#include "hsshortcutservice_p.h"
#include "hscontentservice.h"
#include "hsmenueventfactory.h"
#include "hspage.h"
#include "hsdomainmodeldatastructures.h"

#include "canotifier.h"
#include "canotifierfilter.h"
#include "caservice.h"


HsShortcutService *HsShortcutService::mInstance(0);

HsShortcutServicePrivate::HsShortcutServicePrivate(
    QStateMachine* aStateMachine,
    QObject *parent)
    : QObject(parent),
    mStateMachine(aStateMachine)
{
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsShortcutServicePrivate::~HsShortcutServicePrivate()
{

}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsShortcutServicePrivate::executeCollectionAction(int shortcutId,
        const QString& collectionType)
{
    QEvent* menuEvent = HsMenuEventFactory::createOpenCollectionEvent(
            shortcutId, collectionType);
    mStateMachine->postEvent(menuEvent);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool HsShortcutServicePrivate::isItemShortcutWidget(int aItemId)
{
    Q_UNUSED(aItemId);
    return false;
}

HsShortcutService *HsShortcutService::instance(QStateMachine* machine)
{
    if(!mInstance && machine) {
        mInstance = new HsShortcutService(machine);
    }
    return mInstance;
}



/*!
    Contructor

    \a aManifestFileWithPath e.g c:/foo/bar/shortcutwidget.manifest
    \a aContentService Pointer to content service.
    \a aContentStore Pointer to content store.
    \a aMcsService Pointer to mcs service.
    \a aStateMachine Pointer to state machine to post events.
    \a parent Parent object.
*/
HsShortcutService::HsShortcutService(
    QStateMachine* aStateMachine,
    QObject *parent)
    : QObject(parent)
{
    mD.reset(new HsShortcutServicePrivate(aStateMachine));
}

/*!
    Destructor
*/
HsShortcutService::~HsShortcutService()
{

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsShortcutService::executeCollectionAction(int shortcutId,
        const QString &collectionType)
{
    mD->executeCollectionAction(shortcutId, collectionType);
}

bool HsShortcutService::isItemShortcutWidget(int aItemId)
{
    return mD->isItemShortcutWidget(aItemId);
}
