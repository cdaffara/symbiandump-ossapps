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
 * Description: Dialog controller
 *
 */

#include <HbAction>
#include <QString>
#include <HbMessageBox>

#include "hsdialogcontroller.h"

/*!
 \class HsDialogController
 \ingroup group_hsworkerstateplugin
 \brief Dialog utility.

 Manages dialog lifetime depending on entry removal or
 signal arrival, forwards dialog originating actions to user specified callbacks.
 */

/*!
    \fn void acceptActionTriggered(QAction* action)
    \param action Pointer to action which triggered the signal.
    Emitted when controlled  dialog closed with \a accept action.
*/


/*!
    \fn rejectActionTriggered(QAction* action)
    \param action Pointer to action which triggered the signal.
    Emitted when controlled dialog is closed with \a reject action.
*/

/*!
   \fn void dialogCompleted()
   Emitted whenever controlled dialog is closed.
*/

/*!
  \param message Dialog's message.
  \param options Dialog type indicator.
  \see HsMenuDialogFactory
  Creates dialog controller instance with prediefinded dialog type.
  */
HsDialogController::HsDialogController(
        const QString &message,
        HsMenuDialogFactory::Options options):
    mDialog(HsMenuDialogFactory().create(message, options)),
    mAcceptActionIndex(HsMenuDialogFactory::acceptActionIndex()),
    mRejectActionIndex(HsMenuDialogFactory::rejectActionIndex())
{
}

/*!
  \param dialog Instance of the dialog to be displayed.
  Ownership of the dialog is taken by this object.
  \param acceptActionIndex Index of an \a accept action in a dialog action list
     Actions indices are 0 based. (\see QGraphicsWidget).
  \param rejectActionIndex Index of a \a reject action in a dialog action list.
  Actions indices are 0 based.(\see QGraphicsWidget).
  Creates dialog controller instance with custom dialog type.

  */
HsDialogController::HsDialogController(HbDialog *dialog,
                                       int acceptActionIndex,
                                       int rejectActionIndex):
    mDialog(dialog),
    mAcceptActionIndex(acceptActionIndex),
    mRejectActionIndex(rejectActionIndex)
{
    Q_ASSERT(mDialog != NULL);
    mDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/*!
  Destroys controlled dialog.
  */
HsDialogController::~HsDialogController()
{
    QT_TRY {
        mDialog->deleteLater();
    } QT_CATCH (...) {
    }
}


/*!
  \param entryId Id of the entry which removal will close the dialog.
  Opens dialog.
 */
void HsDialogController::openDialog(int entryId)
{
    HsMenuEntryRemovedHandler *entryRemovedHandler =
        new HsMenuEntryRemovedHandler(entryId, mDialog, SLOT(close()));

    QT_TRY {
        // this object should be deleted when controlled dialog is deleted
        setParent(mDialog);
    } QT_CATCH (const std::bad_alloc &) {
        delete entryRemovedHandler;
        QT_RETHROW;
    }

    mDialog->open(this, SLOT(onDialogFinished(HbAction*)));
}

/*!
  This slot closes dialog.
  */
void HsDialogController::dismissDialog()
{
    mDialog->close();
}

/*!
  \param action Identifies dialog's action.
  Sends signals appropriate for \a action. Irrespective of the action,
  always sends \a dialogCompleted signal.
  */
void HsDialogController::onDialogFinished(HbAction* action)
{
    mDialog->disconnect();

    const int actionIndex = mDialog->actions().indexOf(action);

    if (actionIndex == mAcceptActionIndex)
    {
        emit acceptActionTriggered(action);
    } else if (actionIndex == mRejectActionIndex) {
        emit rejectActionTriggered(action);
    }

    emit dialogCompleted();

    mDialog->disconnect();
}


