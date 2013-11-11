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
#include "hsuinstallfailedstate.h"
#include "hsmenuevent.h"
#include "hsapp_defs.h"
#include "hsdialogcontroller.h"

const int installerBusyError = 4;

/*!
 Constructor.
 /param parent Parent state.
 */
HsUninstallFailedState::HsUninstallFailedState(QState *parent) :
    QState(parent)
{
    construct();
}

/*!
 Destructor
 */
HsUninstallFailedState::~HsUninstallFailedState()
{
    QT_TRY {
        emit exit();
    } QT_CATCH (...) {
    }
}

/*!
 Construction
 */
void HsUninstallFailedState::construct()
{
    setObjectName("/HsUninstallFailedState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
}


/*!
 Sets entry event.
 \param event entry event.
 */
void HsUninstallFailedState::onEntry(QEvent *event)
{    
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();
    int error = data.value(Hs::uninstallError).toInt();
    QScopedPointer<HbMessageBox> dialog;
    if (error == installerBusyError) {
        // Installer is in use
        dialog.reset(new HbMessageBox(HbMessageBox::MessageTypeInformation));
        dialog->setText(hbTrId("txt_applib_info_installer_is_currently_busy"));
    } else {
        // other errors
        dialog.reset(new HbMessageBox(HbMessageBox::MessageTypeWarning));
        dialog->setText(hbTrId("txt_applib_info_uninstallation_failed"));
    }


    dialog->setStandardButtons(HbMessageBox::Close);

    QScopedPointer<HsDialogController> dialogController(
        new HsDialogController(dialog.take(),
            HsMenuDialogFactory::acceptActionIndex(),
            HsMenuDialogFactory::rejectActionIndex()));

    connect(dialogController.data(),
            SIGNAL(dialogCompleted()),
            this,
            SIGNAL(exit()));

    // ensure dialog is dismissed on app key pressed
    connect(this, SIGNAL(exited()),
            dialogController.data(),
            SLOT(dismissDialog()));

    dialogController.take()->openDialog();
}



