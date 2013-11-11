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
#include <xqservicerequest.h>
#include <xqappmgr.h>
#include <xqrequestinfo.h>
#include <xqaiwdecl.h>
#include <QThreadPool>

#include "phoneapplauncher.h"
#include "qtphonelog.h"

// runnable object
void PhoneAppLauncherTask::run()
{
    XQApplicationManager appManager;
    QScopedPointer<XQAiwRequest> request( 
        mService.isEmpty() ? 
        appManager.create(mInterface, mOperation, false) :
        appManager.create(mService, mInterface, mOperation, false) );
    if (request == NULL) {
        PHONE_TRACE1("service not found");
        return;
    }

    XQRequestInfo info;
    info.setForeground(true);
    request->setInfo(info);
    request->setArguments(mArguments);
    QVariant retValue(-1);
    if (!request->send(retValue)) {
        int error = request->lastError();
        PHONE_TRACE2("send failed, error %d", request->lastError());
    }
}


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
    QList<QVariant> arguments;
    arguments.append(QVariant(toField));
    arguments.append(QVariant(name));
    arguments.append(QVariant(messageBody));
    sendServiceRequest(
        "messaging",
        XQI_MESSAGE_SEND,
        "send(QString,QString,QString)",
        arguments,
        false);
}

/*!
    PhoneAppLauncher::launchContacts.
 */
void PhoneAppLauncher::launchContacts()
{
    QList<QVariant> arguments;
    sendServiceRequest(
        "com.nokia.services.phonebookappservices",
        "Launch",
        "launch()",
        arguments,
        true);
}

/*!
    PhoneAppLauncher::launchLogs.
 */
void PhoneAppLauncher::launchLogs(
        int activatedView,
        bool showDialpad,
        const QString &dialpadText)
{
    QVariantMap map;
    map.insert(XQLOGS_VIEW_INDEX, QVariant(activatedView));
    map.insert(XQLOGS_SHOW_DIALPAD, QVariant(showDialpad));
    map.insert(XQLOGS_DIALPAD_TEXT, QVariant(dialpadText));
    QList<QVariant> args;
    args.append(QVariant(map));
    // Circular wait condition possible -> launc new thread
    PhoneAppLauncherTask *task = new PhoneAppLauncherTask();
    task->mService = "logs";
    task->mInterface = XQI_LOGS_VIEW;
    task->mOperation = XQOP_LOGS_SHOW;
    task->mArguments = args;
    
    QThreadPool::globalInstance()->start(task);
}

/*!
    PhoneAppLauncher::sendServiceRequest.
 */
void PhoneAppLauncher::sendServiceRequest(
        const QString &service, 
        const QString &interface,
        const QString &operation,
        const QList<QVariant> &arguments,
        const bool foreground)
{
    PHONE_TRACE
    int err = -1;
    TRAP_IGNORE(
        // Allow application launch only when device is unlocked
        // If locked ask devicelock query
        CDevicelockAccessApi *devicelockAccessApi = CDevicelockAccessApi::NewL();
        err = devicelockAccessApi->DisableDevicelock();
        delete devicelockAccessApi;
    )
    
    if ((err == KErrNone) || (err == KErrAlreadyExists)) {
        PHONE_DEBUG3(service, operation, arguments);
        XQApplicationManager appManager;
        QScopedPointer<XQAiwRequest> request( 
            service.isEmpty() ? 
            appManager.create(interface, operation, false) :
            appManager.create(service, interface, operation, false) );
        if (request == NULL) {
            PHONE_TRACE1("service not found");
            return;
        }
        if (foreground) {
            XQRequestInfo info;
            info.setForeground(true);
            request->setInfo(info);
        }
        
        request->setArguments(arguments);
        request->setSynchronous(false);
        QVariant retValue(-1);
        if (!request->send(retValue)) {
            int error = request->lastError();
            PHONE_TRACE2("send failed, error %d", request->lastError());
        }
    }
}

// end of file

