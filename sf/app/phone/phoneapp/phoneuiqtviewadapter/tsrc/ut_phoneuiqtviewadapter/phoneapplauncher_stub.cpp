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

#include <devicelockaccessapi.h>

#include "phoneapplauncher.h"
#include "qtphonelog.h"

/*!
    PhoneAppLauncher::PhoneAppLauncher.
 */
PhoneAppLauncher::PhoneAppLauncher(QObject *parent)
    : QObject(parent)
{
    PHONE_TRACE;
}


/*!
    PhoneAppLauncher::~PhoneAppLauncher.
 */
PhoneAppLauncher::~PhoneAppLauncher()
{
    PHONE_TRACE;
}

/*!
    PhoneAppLauncher::launchMessaging.
 */
void PhoneAppLauncher::launchMessaging(
        const QString &toField,
        const QString &name,
        const QString &messageBody)
{
    Q_UNUSED(toField);
    Q_UNUSED(name);
    Q_UNUSED(messageBody);
}

/*!
    PhoneAppLauncher::launchContacts.
 */
void PhoneAppLauncher::launchContacts()
{

}

/*!
    PhoneAppLauncher::launchLogs.
 */
void PhoneAppLauncher::launchLogs(
        int activatedView,
        bool showDialpad,
        const QString &dialpadText)
{
    Q_UNUSED(activatedView);
    Q_UNUSED(showDialpad);
    Q_UNUSED(dialpadText);
}

/*!
    PhoneAppLauncher::sendServiceRequest.
 */
void PhoneAppLauncher::sendServiceRequest(
        const QString &service, 
        const QString &interface,
        const QString &function,
        const QList<QVariant> &arguments,
        const bool foreground)
{
    Q_UNUSED(service);
    Q_UNUSED(interface);
    Q_UNUSED(function);
    Q_UNUSED(arguments);
    Q_UNUSED(foreground);
}

// end of file
