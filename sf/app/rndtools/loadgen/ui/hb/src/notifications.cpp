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
#include <hblabel.h>
#include <QString>

#include "notifications.h"


// ---------------------------------------------------------------------------

void Notifications::about()
{
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    messageBox->setText("Version 1.5.2 - September 23rd 2010. Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. Licensed under Eclipse Public License v1.0.");
    HbLabel *header = new HbLabel("About LoadGen", messageBox);
    messageBox->setHeadingWidget(header);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->open();
}

// ---------------------------------------------------------------------------

void Notifications::error(const QString& errorMessage)
{
	showGlobalNote(errorMessage, HbMessageBox::MessageTypeWarning );
}

// ---------------------------------------------------------------------------

void Notifications::showGlobalNote(const QString& text, HbMessageBox::MessageBoxType type, HbPopup::DefaultTimeout timeout)
{
    HbMessageBox *messageBox = new HbMessageBox(type);
    messageBox->setText(text);
    messageBox->setTimeout(timeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->open();
}

// ---------------------------------------------------------------------------
