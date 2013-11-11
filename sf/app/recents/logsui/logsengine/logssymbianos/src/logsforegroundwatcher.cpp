/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "logsforegroundwatcher.h"
#include "logslogger.h"
#include <apacmdln.h>


// -----------------------------------------------------------------------------
// Currently no other way to monitor first app foreground switch in case
// mainwindow is not made yet visible.
// -----------------------------------------------------------------------------
//
LogsForegroundWatcher::LogsForegroundWatcher(QObject* parent) : QObject(parent)
{
    CCoeEnv* env = CCoeEnv::Static();
    if (env) {
        TRAP_IGNORE(env->AddForegroundObserverL(*this));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsForegroundWatcher::~LogsForegroundWatcher()
{
    CCoeEnv* env = CCoeEnv::Static();
    if (env) {
        env->RemoveForegroundObserver(*this);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsForegroundWatcher::ensureBackgroundStartup()
{
    CApaCommandLine* commandLine = 0;
    CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
    if ( commandLine ){
        TApaCommand command = commandLine->Command();
        if (command != EApaCommandBackground) {
            LOGS_QDEBUG( "logs [UI] -> LogsForegroundWatcher::ensureBackgroundStartup");
            TRAPD( err, commandLine->SetCommandL( EApaCommandBackground ) );
            if ( !err ){
                RProcess thisProcess;
                TRAP_IGNORE( commandLine->SetProcessEnvironmentL(thisProcess) )
            }
            LOGS_QDEBUG( "logs [UI] <- LogsForegroundWatcher::ensureBackgroundStartup");
        }
        delete commandLine;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsForegroundWatcher::HandleGainingForeground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsForegroundWatcher::HandleGainingForeground" );
    emit gainingForeground();
    LOGS_QDEBUG( "logs [UI] <- LogsForegroundWatcher::HandleGainingForeground" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsForegroundWatcher::HandleLosingForeground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsForegroundWatcher::HandleLosingForeground" );
    emit losingForeground();
    LOGS_QDEBUG( "logs [UI] <- LogsForegroundWatcher::HandleLosingForeground" );
}

