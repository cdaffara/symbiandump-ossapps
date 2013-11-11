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
 * Description: Menu delete collection item state
 *
 */

#include <HbMessageBox>
#include <hbaction.h>
#include <HbParameterLengthLimiter>
#include <hsmenuservice.h>
#include <hsmenueventfactory.h>

#include "hsdialogcontroller.h"
#include "hsdeletecollectionitemstate.h"
#include "hsmenuevent.h"

/*!
 \class HsDeleteCollectionItemState
 \ingroup group_hsworkerstateplugin
 \brief Delete Collection Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Parent state. 
 */
HsDeleteCollectionItemState::HsDeleteCollectionItemState(QState *parent) :
    QState(parent),
    mItemId(0),
    mCollectionId(0)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionItemState::~HsDeleteCollectionItemState()
{
    QT_TRY {
        emit exit();
    } QT_CATCH (...) {
    }
}

/*!
 Constructs contained objects.
 */
void HsDeleteCollectionItemState::construct()
{
    setObjectName("/DeleteCollectionItemState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsDeleteCollectionItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionItemState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    mItemId = data.value(Hs::itemIdKey).toInt();
    mCollectionId = data.value(Hs::collectionIdKey).toInt();

    QString message;
    message.append(
        HbParameterLengthLimiter(
                "txt_applib_dialog_remove_1_from_collection").arg(
                HsMenuService::getName(mItemId)));

    QScopedPointer<HsDialogController> dialogController(
            new HsDialogController(message));

    connect(dialogController.data(),
            SIGNAL(acceptActionTriggered(QAction*)),
            this,
            SLOT(removeApplicationFromCollection()));

    connect(dialogController.data(),
            SIGNAL(dialogCompleted()),
            this,
            SIGNAL(exit()));

    // ensure dialog is dismissed on app key pressed
    connect(this, SIGNAL(exited()),
            dialogController.data(),
            SLOT(dismissDialog()));

    dialogController.take()->openDialog();

    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionItemState::onEntry");
}


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
/*!
 Slot. Removes application from collection, both specified by event
 passed to the state on entry.
 \retval void
 */
void HsDeleteCollectionItemState::removeApplicationFromCollection()
{
    HsMenuService::removeApplicationFromCollection(mItemId, mCollectionId);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsDeleteCollectionItemState::cleanUp()
{
    mItemId = 0;
    mCollectionId = 0;
}
