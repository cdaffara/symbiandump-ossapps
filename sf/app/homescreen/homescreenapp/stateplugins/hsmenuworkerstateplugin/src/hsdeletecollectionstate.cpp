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
 * Description: Menu delete collection state
 *
 */

#include <QStateMachine>
#include <HbMessageBox>
#include <hbaction.h>
#include <hsmenuservice.h>
#include <HbParameterLengthLimiter>
#include <hsshortcutservice.h>
#include <hsmenueventfactory.h>

#include "hsdeletecollectionstate.h"
#include "hsdialogcontroller.h"
#include "hsmenuevent.h"

/*!
 \class HsDeleteCollectionState
 \ingroup group_hsworkerstateplugin
 \brief Delete Collection State.
 Deletes collections.
 */

/*!
 Constructor.
 \param parent Parent state.
 */
HsDeleteCollectionState::HsDeleteCollectionState(QState *parent) :
    QState(parent),
    mItemId(0)
{
    construct();
}

/*!
 Destructor.
 */
HsDeleteCollectionState::~HsDeleteCollectionState()
{
    QT_TRY {
        emit exit();
    } QT_CATCH (...) {
    }
}

/*!
 Constructs contained objects.
 */
void HsDeleteCollectionState::construct()
{
    setObjectName("/DeleteCollectionState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsDeleteCollectionState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsDeleteCollectionState::onEntry");
    QState::onEntry(event);
    qDebug("CollectionState::onEntry()");

    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    mItemId = data.value(Hs::itemIdKey).toInt();

    QString message;
    if (HsShortcutService::instance()->isItemShortcutWidget(mItemId)) {
        message.append(HbParameterLengthLimiter(
                           "txt_applib_dialog_deletes_1_also_from_home_screen"). arg(
                           HsMenuService::getName(mItemId)));
    } else {
        message.append(HbParameterLengthLimiter("txt_applib_dialog_delete_1").arg(
                           HsMenuService::getName(mItemId)));
    }

    QScopedPointer<HsDialogController> dialogController(
            new HsDialogController(message, HsMenuDialogFactory::DeleteCancel));

    connect(dialogController.data(),
            SIGNAL(acceptActionTriggered(QAction*)),
            this,
            SLOT(deleteCollection()));

    connect(dialogController.data(),
            SIGNAL(dialogCompleted()),
            this,
            SIGNAL(exit()));

    // ensure dialog is dismissed on app key pressed
    connect(this, SIGNAL(exited()),
            dialogController.data(),
            SLOT(dismissDialog()));

    dialogController.take()->openDialog();

    HSMENUTEST_FUNC_EXIT("HsDeleteCollectionState::onEntry");
}

/*!
 Slot. Removes collection specified by event passed on entry to the state.
 \retval void
 */
void HsDeleteCollectionState::deleteCollection()
{
    HsMenuService::removeCollection(mItemId);
    machine()->postEvent(
        HsMenuEventFactory::createCollectionDeletedEvent());
}


/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsDeleteCollectionState::cleanUp()
{
    mItemId = 0;
}
