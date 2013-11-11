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

class HbProgressDialog;

class Notifications
{
public:
    static void showAboutNote();
    static void showMessageBox(HbMessageBox::MessageBoxType type, const QString &text, const QString &label, int timeout = HbPopup::NoTimeout);
    static void showInformationNote(const QString &text, const QString &title = QString());
    static void showErrorNote(const QString &text, bool noTimeout = false);
    static void showConfirmationNote(const QString &text, bool noTimeout = false);
    static bool showConfirmationQuery(const QString &aText);
};

#endif // NOTIFICATIONS_H_
