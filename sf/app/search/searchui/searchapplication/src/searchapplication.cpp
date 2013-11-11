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
 * Description:  Search application main class.
 *
 */

#include "searchapplication.h"
#include <qstatemachine.h>
#include <searchruntimeprovider.h>
#include <searchruntime.h>
#include <hbapplication.h>

// ---------------------------------------------------------------------------
// SearchApplication::SearchApplication
// ---------------------------------------------------------------------------
//
SearchApplication::SearchApplication(int argc, char *argv[]) :
    HbApplication(argc, argv), mRuntime(NULL)
    {
    SEARCH_FUNC_ENTRY("SEARCH::SearchApplication::SearchApplication"); SEARCH_FUNC_EXIT("SEARCH::SearchApplication::SearchApplication");
    }

// ---------------------------------------------------------------------------
// SearchApplication::~SearchApplication()
// ---------------------------------------------------------------------------
//
SearchApplication::~SearchApplication()
    {
    if (mRuntime)
        {
        mRuntime->stop();
        }
    if (mRuntime)
        {
        disconnect(mRuntime, SIGNAL(started()), this,
                SLOT(handleRuntimeStarted()));
        disconnect(mRuntime, SIGNAL(stopped()), this,
                SLOT(handleRuntimeStopped()));
        disconnect(mRuntime, SIGNAL(faulted()), this,
                SLOT(handleRuntimeFaulted()));

        delete mRuntime;
        }
    }
// ---------------------------------------------------------------------------
// SearchApplication::start()
// ---------------------------------------------------------------------------
//
void SearchApplication::start()
    {
    SEARCH_FUNC_ENTRY("SEARCH::Search::start");
    SearchRuntimeProvider *interface = new SearchRuntimeProvider();
    mRuntime = interface->createPlugin();
    if (mRuntime)
        {
        mRuntime->setParent(this);
        connect(mRuntime, SIGNAL(started()), this,
                SLOT(handleRuntimeStarted()));
        connect(mRuntime, SIGNAL(stopped()), this,
                SLOT(handleRuntimeStopped()));
        connect(mRuntime, SIGNAL(faulted()), this,
                SLOT(handleRuntimeFaulted()));
        connect(mRuntime, SIGNAL(sentMattiEventSignal()), this,
                SLOT(sentMattiEventSlot()));
        }
    if (mRuntime)
        {
        mRuntime->start();
        }
    else
        {
        emit exit();
        } SEARCH_FUNC_EXIT("SEARCH::Search::start");
    }

// ---------------------------------------------------------------------------
// SearchApplication::stop()
// ---------------------------------------------------------------------------
//
void SearchApplication::stop()
    {
    SEARCH_FUNC_ENTRY("SEARCH::Search::stop");
    if (mRuntime)
        {
        mRuntime->stop();
        }

    SEARCH_FUNC_EXIT("SEARCH::Search::stop");
    }

// ---------------------------------------------------------------------------
// SearchApplication::handleRuntimeStarted()
// ---------------------------------------------------------------------------
//
void SearchApplication::handleRuntimeStarted()
    {
    }

// ---------------------------------------------------------------------------
// SearchApplication::handleRuntimeStopped()
// ---------------------------------------------------------------------------
//
void SearchApplication::handleRuntimeStopped()
    {
    emit exit();
    }

// ---------------------------------------------------------------------------
// SearchApplication::handleRuntimeFaulted()
// ---------------------------------------------------------------------------
//
void SearchApplication::handleRuntimeFaulted()
    {
    emit exit();
    }

void SearchApplication::sentMattiEventSlot()
    {
    emit applicationReady();
    }
