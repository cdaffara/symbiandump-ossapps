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

#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_

#include <hbmessagebox.h>
#include <hbdeviceprogressdialog.h>


class Notifications
    {
public:

    /**
     * shows message box of the specified type with text and label
     */
    static void showMessageBox(
        HbMessageBox::MessageBoxType type, 
        const QString &text, 
        const QString &label, 
        int timeout = HbPopup::NoTimeout
        );
    
    /**
     * shows about box
     */
    static void about();

    /**
     * shows information type message box
     */
    static void showInformationNote(const QString &text);

    /**
     * shows warning type message box
     */
    static void showErrorNote(const QString &text);
    
     /**
     * shows progressbar
     */
    static HbDeviceProgressDialog* showWaitDialog(const QString &text);

    
    };



#endif // NOTIFICATIONS_H_
