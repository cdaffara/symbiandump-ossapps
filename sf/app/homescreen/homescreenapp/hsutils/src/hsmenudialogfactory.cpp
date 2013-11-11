/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Menu Dialog
*
*/

#include <QString>
#include <QStringList>
#include <QList>
#include <QAction>

#include <HbAction>
#include <HbMessageBox>
#include "hsmenudialogfactory.h"

/*!
    \class HsMenuDialogFactory
    \ingroup group_hsutils

    \brief Creates dialog instances for Application Library.

    \sa HsMenuEvent
*/

/*! \enum HsMenuDialogFactory::Options
 Describes what actions the dialog will have.
 */

/*! \var HsMenuDialogFactory::Options HsMenuDialogFactory::OkCancel
 Requested dialog should have two actions in the following order: Ok (confirm) and Cancel (reject).
 */
/*! \var HsMenuDialogFactory::Options HsMenuDialogFactory::Close
 Requested dialog should have one action: Close.
 */
/*!
    Creates dialog.
    \param text to be displayed in the dialog.
    \param options specify dialog actions.
    \return requested dialog.
*/
HbMessageBox *HsMenuDialogFactory::create(const QString &text,
                                 Options options) const
{

    QStringList actionIdentifiers; // head of the list will be first action in dialog
    HbMessageBox::MessageBoxType type(HbMessageBox::MessageTypeNone);

    switch (options) {

    case OkCancel:
        actionIdentifiers << hbTrId("txt_common_button_ok");
        actionIdentifiers << hbTrId("txt_common_button_cancel");
        type = HbMessageBox::MessageTypeQuestion;
        break;

    case Close:
        actionIdentifiers << hbTrId("txt_common_button_close");
        type = HbMessageBox::MessageTypeInformation;
        break;

    case DeleteCancel:
        actionIdentifiers << hbTrId("txt_common_button_delete");
        actionIdentifiers << hbTrId("txt_common_button_cancel");
        type = HbMessageBox::MessageTypeQuestion;
        break;
        
    default:
        break;
    }

    QScopedPointer<HbMessageBox> box(new HbMessageBox(type));

    setUpActions(box.data(), actionIdentifiers);

    box->setText(text);
    box->setAttribute(Qt::WA_DeleteOnClose);

    return box.take();
}


/*!
    Destructor.
*/
HsMenuDialogFactory::~HsMenuDialogFactory() {}

/*!
    Sets up dialog actions
    \param box dialog to operate on.
    \param actionIdentifiers list of translation identifiers for actions. Order of
    actions in the dialog will reflect the identifiers order in the list.
*/
void HsMenuDialogFactory::setUpActions(HbMessageBox *box,
                                    const QStringList &translationIdentifiers) const
{
    box->clearActions();

    foreach (QString identifier, translationIdentifiers) {
        QScopedPointer<HbAction> action(new HbAction(identifier));
        action->setParent(box);
        box->addAction(action.take());
    }
}

int HsMenuDialogFactory::acceptActionIndex()
{
    return 0;
}

int HsMenuDialogFactory::rejectActionIndex()
{
    return 1;
}

