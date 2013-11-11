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


#include <hblabel.h>
#include <hbdeviceprogressdialog.h>
#include <hbmessagebox.h>


#include "notifications.h"

// ---------------------------------------------------------------------------

void Notifications::showMessageBox(HbMessageBox::MessageBoxType type, const QString &text, const QString &label, int timeout )
{
    HbMessageBox *messageBox = new HbMessageBox(type);
    messageBox->setText(text);
    if(label.length())
        {
        HbLabel *header = new HbLabel(label, messageBox);
        messageBox->setHeadingWidget(header);
        }
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(timeout);
    messageBox->open();
}

// ---------------------------------------------------------------------------

void Notifications::about()
{
    Notifications::showMessageBox( 
        HbMessageBox::MessageTypeInformation, 
        "Version 4.0.1 - 21st May 2010. Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. Licensed under Eclipse Public License v1.0.", 
        "About Launcher"
        );
}

// ---------------------------------------------------------------------------

HbDeviceProgressDialog* Notifications::showWaitDialog(const QString &text)
{
    HbDeviceProgressDialog *note = new HbDeviceProgressDialog( HbProgressDialog::WaitDialog );
    note->setText( text );
    note->setAction(0);
    note->show();
    return note;
}

// ---------------------------------------------------------------------------

void Notifications::showInformationNote(const QString &text)
{
    Notifications::showMessageBox( 
        HbMessageBox::MessageTypeInformation,
        text, 
        "", // no label
        3000
        );
}

// ---------------------------------------------------------------------------

void Notifications::showErrorNote(const QString &text)
{
    Notifications::showMessageBox( 
        HbMessageBox::MessageTypeWarning,
        text, 
        "", // no label
        3000
        );
}

// ---------------------------------------------------------------------------
