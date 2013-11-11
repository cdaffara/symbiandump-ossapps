/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Qt wrapper class implementation for CIRReportGenerator
*
*/

#include "irqstatisticsreporter.h"
#ifdef Q_OS_SYMBIAN
#include "irqstatisticsreporter_symbian_p.h"
#endif

QMutex IRQStatisticsReporter::mMutex;
int IRQStatisticsReporter::mRef = 0;
IRQStatisticsReporter *IRQStatisticsReporter::mInstance = NULL;

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::openInstance()
// Static function to get a singleton instance of IRQStatisticsReporter
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter* IRQStatisticsReporter::openInstance()
{
    mMutex.lock();
    if( NULL == mInstance )
    {
        mInstance = new IRQStatisticsReporter();
    }       
    if( mInstance != NULL )
    {
        mRef++;
    }
    mMutex.unlock();
    return mInstance;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::closeInstance()
// Close a singleton instance of IRQStatisticsReporter
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::closeInstance()
{
    if( mInstance != NULL )
    {
        mMutex.lock();
        mRef--;
        if( 0 == mRef )
        {
            delete mInstance;
            mInstance = NULL;
        }        
        mMutex.unlock();
    }
}


// ---------------------------------------------------------------------------
// IRQStatisticsReporter::IRQStatisticsReporter()
// Constructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter::IRQStatisticsReporter() : d_ptr(new IRQStatisticsReporterPrivate())
{
    d_ptr->init();
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::~IRQStatisticsReporter()
// Destructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter::~IRQStatisticsReporter()
{
    delete d_ptr;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::markSessionStart()
// marks the start of a session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::markSessionStart()
{
    d_ptr->markSessionStart();
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::sessionStarted()
// starts a new session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::sessionStarted(const int aChanneldId, const IRConnectedFrom aConnectedFrom)
{
    d_ptr->sessionStarted(aChanneldId, aConnectedFrom);
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::suspendSession()
// function handle session suspend
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::suspendSession()
{
    d_ptr->suspendSession();
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::restartSession()
// function handle session restart
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::restartSession()
{
    d_ptr->restartSession();
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::sessionEnded()
// ends the session
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::sessionEnded(const IRTerminatedType aTerminatedBy)
{
    d_ptr->sessionEnded(aTerminatedBy);
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logServerResult()
// logs server connection result
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::logServerResult(const QString& aUrl, const IRServerResult aServerResult)
{
    d_ptr->logServerResult(aUrl, aServerResult);
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logSongRecogEvent()
// logs songRecog event
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::logSongRecogEvent()
{
    d_ptr->logSongRecogEvent();
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logNmsEvents()
// logs nmsEvents
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::logNmsEvent(const IRNmsType aNmsType, const int aChanneldId)
{
    d_ptr->logNmsEvent(aNmsType, aChanneldId);
}
