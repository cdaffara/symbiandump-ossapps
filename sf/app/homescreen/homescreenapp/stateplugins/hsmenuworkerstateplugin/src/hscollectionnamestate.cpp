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
 * Description: Menu name collection state
 *
 */

#include <hbaction.h>
#include <hblineedit.h>
#include <hsmenuservice.h>

#include "hscollectionnamestate.h"
#include "hsmenuevent.h"
#include "hscollectionnamedialog.h"

/*!
 \class HsCollectionNameState
 \ingroup group_hsworkerstateplugin
 \brief Rename Collection State.
 Collection name state
 */

/*!
 \fn void HsCollectionNameState::commit(const QString &collectionName);
 Signal emitted when collection name is given.
 \param collectionName name of collection.
 */

/*!
 \fn void HsCollectionNameState::commitCheckList(const QString &collectionName);
 Signal emitted when collection name is given - version to trigger
 transition to HsAppsCheckListState.
 \param collectionName name of collection.
 */

/*!
 \fn void HsCollectionNameState::cancel();
 Signal emitted when user selects cancel.
 */

/*!
 Constructor
 \param parent Parent state.
 \retval void
 */
HsCollectionNameState::HsCollectionNameState(QState *parent) :
    QState(parent),
    mItemId(0), mCollectionNameDialog(NULL)
{
    construct();
}

/*!
 Destructor
 \retval void
 */
HsCollectionNameState::~HsCollectionNameState()
{
    cleanUp();
}

/*!
 Constructs contained objects.
 */
void HsCollectionNameState::construct()
{
    setObjectName("/collectionnamestate");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsCollectionNameState::onEntry(QEvent *event)
{
    qDebug("CollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsCollectionNameState::onEntry");
    QState::onEntry(event);

    mItemId = 0;
    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();

        mItemId = data.value(Hs::itemIdKey).toInt();
    }
    mCollectionNameDialog = new HsCollectionNameDialog(mItemId);
    mCollectionNameDialog->open(this, SLOT(dialogFinished(HbAction*)));

    HSMENUTEST_FUNC_EXIT("HsCollectionNameState::onEntry");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameState::dialogFinished(HbAction* finishedAction)
{
    if (finishedAction == mCollectionNameDialog->actions().value(0)) {
        QString newName(mCollectionNameDialog->uniqueCollectionName());
        if (mItemId) {
            if (newName != HsMenuService::getName(mItemId)) {
                HsMenuService::renameCollection(mItemId, newName);
            }
        } else {
            HsMenuService::createCollection(newName);
        }
    }
    //set to NULL since this will be deleted atfer close
    mCollectionNameDialog = NULL;
    emit exit();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameState::cleanUp()
{
    if (mCollectionNameDialog) {
        disconnect(mCollectionNameDialog, SIGNAL(finished(HbAction*)),
                   this, SLOT(dialogFinished(HbAction*)));
        mCollectionNameDialog->close();
        mCollectionNameDialog = NULL;
    }
}
