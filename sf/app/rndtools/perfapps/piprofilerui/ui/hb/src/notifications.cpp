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

#include <hbmessagebox.h>
#include <hbpopup.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbdialog.h>
#include <hbaction.h>
#include <hbtextitem.h>

#include "notifications.h"

// ---------------------------------------------------------------------------

void Notifications::showInformationNote(const QString &text)
{
    
    // Create new dialog 
     HbDialog *infoNote = new HbDialog();
     infoNote->setAttribute(Qt::WA_DeleteOnClose);

     // set dismiss policy
     infoNote->setTimeout(0); //

     // create text item for popup
     HbTextItem *label = new HbTextItem(text, infoNote);
     label->setAlignment(Qt::AlignCenter);
     infoNote->setContentWidget(label);
     infoNote->setPrimaryAction(new HbAction("Ok"));

     infoNote->show();
}

// ---------------------------------------------------------------------------

void Notifications::showMessageBox(const QString &text)
{
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation );
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setTimeout(0);
    note->setText(text);
    note->show();

}

// ---------------------------------------------------------------------------

void Notifications::showErrorNote(const QString &text)
{
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeWarning );
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setTimeout(0);
    note->setText(text);
    note->show();

}

// ---------------------------------------------------------------------------
