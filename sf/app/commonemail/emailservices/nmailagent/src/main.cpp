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

#include "emailtrace.h"

#include "nmmailagentheaders.h"
#include "nmmailagent.h"
#include "ssastartupwatcher.h"

#ifdef __WINSCW__
const int NmStartupDelay = 4000; // 4s
#else
const int NmStartupDelay = 1000; // 1s
#endif

NmMailAgent *agent = NULL;

/*!
    Callback for reporting startup state
*/
static void startupCallback(int status)
{
    NM_FUNCTION;
    NM_COMMENT(QString("nmailagent: startupCallback(): status=%1").arg(status));

    Q_UNUSED(status);

    // either it is an error or 'non critical startup' state has been reached
    // Start the agent in both cases
    QTimer::singleShot(NmStartupDelay, agent, SLOT(delayedStart()));
}

/*!
	main
*/
int main(int argc, char *argv[])
{
    NM_FUNCTION;

    QCoreApplication app(argc, argv);

    agent = new NmMailAgent;

    CSSAStartupWatcher *startupWatcher = CSSAStartupWatcher::New(startupCallback);
    if (!startupWatcher) {
        NM_ERROR(1,"nmmailagent: watcher start failed");
        QTimer::singleShot(NmStartupDelay, agent, SLOT(delayedStart()));
    }

    int retValue = app.exec();
    delete startupWatcher;
    delete agent;
    return retValue;
}
