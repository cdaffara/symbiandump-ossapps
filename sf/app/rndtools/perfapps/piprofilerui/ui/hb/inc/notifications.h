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

#include <QString> 

/**
 * Notifications class for viewing notification messages.
 */

class Notifications
{
public:

    static void showInformationNote(const QString &text);
    static void showErrorNote(const QString &text);
    static void showMessageBox(const QString &text);

private:

};

#endif // NOTIFICATIONS_H_
