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
* Description:
*
*/
#include "notifications.h"

#include <HbLabel>
#include <HbProgressDialog>
#include <HbMessageBox>

// ---------------------------------------------------------------------------

void Notifications::showMessageBox(HbMessageBox::MessageBoxType type, const QString &text, const QString &label, int timeout)
{
    HbMessageBox *messageBox = new HbMessageBox(type);
    messageBox->setText(text);
    if (label.length()) {
        HbLabel *header = new HbLabel(label, messageBox);
        messageBox->setHeadingWidget(header);
    }
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(timeout);
    messageBox->open();
}

void Notifications::showAboutNote()
{
    showMessageBox(HbMessageBox::MessageTypeInformation,
                   "Version 5.1.1 - 26th July 2010. Copyright © 2010 Nokia Corporation"
                        "and/or its subsidiary(-ies). All rights reserved."
                        "Licensed under Eclipse Public License v1.0.",
                   "About File Browser"
        );
}

// ---------------------------------------------------------------------------

void Notifications::showInformationNote(const QString &text, const QString &title)
{
    showMessageBox(HbMessageBox::MessageTypeInformation, text, title, 3000);
}

// ---------------------------------------------------------------------------

void Notifications::showErrorNote(const QString &text, bool noTimeout)
{
    showMessageBox(HbMessageBox::MessageTypeWarning, text, "",
                   noTimeout ? HbPopup::NoTimeout : 3000 /*HbPopup::StandardTimeout*/);
}

// ---------------------------------------------------------------------------

void Notifications::showConfirmationNote(const QString &text, bool noTimeout)
{
    showMessageBox(HbMessageBox::MessageTypeInformation, text, "",
                   noTimeout ? HbPopup::NoTimeout : 3000 /*HbPopup::ConfirmationNoteTimeout*/);
}

// ---------------------------------------------------------------------------

bool Notifications::showConfirmationQuery(const QString &aText)
{
    Q_UNUSED(aText);
    return false; //HbMessageBox::question(aText);
}

// ---------------------------------------------------------------------------
