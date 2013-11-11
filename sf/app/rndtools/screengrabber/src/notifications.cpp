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

#include <hbdevicemessagebox.h>
#include <hblabel.h>
#include <hbdeviceprogressdialog.h>

#include "notifications.h"



// ---------------------------------------------------------------------------

void Notifications::imageCaptured()
{
    HbDeviceMessageBox::information("Screen shot saved to Media Gallery");
}

// ---------------------------------------------------------------------------

void Notifications::about()
{
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    messageBox->setText("Version 5.0.0 - March 10th 2010. Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. Licensed under Eclipse Public License v1.0.");
    HbLabel *header = new HbLabel("About Screen Grabber", messageBox);
    messageBox->setHeadingWidget(header);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->open();
}

// ---------------------------------------------------------------------------

void Notifications::error(const QString& errorMessage)
{
    HbDeviceMessageBox::warning(errorMessage);

}

// ---------------------------------------------------------------------------

void Notifications::sequantialImagesCaptured(int amount){
    // Code below launches a global note
    QString text;
    text.setNum(amount, 10);
    text.append(" screen shots saved to Media Gallery");
    
    HbDeviceMessageBox::information(text);
 
}

// ---------------------------------------------------------------------------

void Notifications::videoCaptured()
{
    HbDeviceMessageBox::information("Video saved to Media Gallery");

}

// ---------------------------------------------------------------------------

HbDeviceProgressDialog* Notifications::showProgressBar(int max)
{
    HbDeviceProgressDialog *note = new HbDeviceProgressDialog(HbProgressDialog::ProgressDialog);
    note->setText("Saving...");
    note->setMaximum(max);
    note->show();
    return note;
}

// ---------------------------------------------------------------------------
