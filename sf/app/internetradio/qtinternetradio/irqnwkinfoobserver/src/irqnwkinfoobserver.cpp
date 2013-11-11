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
#include "irqnwkinfoobserver.h"
#include "irqnwkinfoobserver_p.h"

IRQNwkInfoObserver * IRQNwkInfoObserver::mInstance = NULL;
QMutex IRQNwkInfoObserver::mMutex;

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::openInstance()
// Static function to get a singleton instance of IRQNwkInfoObserver
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver* IRQNwkInfoObserver::openInstance()
{
    mMutex.lock();

    if (NULL == mInstance) 
    {
        mInstance = new IRQNwkInfoObserver();

        if (!mInstance->mInitPrivateSuccess) 
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
    else
    {
        mInstance->mRefCount++;
    }

    mMutex.unlock();

    return mInstance;
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::closeInstance()
// Close a singleton instance of IRQNwkInfoObserver
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::closeInstance()
{
    mMutex.lock();
    if ((--mRefCount) == 0)
    {
        mInstance = NULL;
        delete this;
    }
    mMutex.unlock();
}


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver::IRQNwkInfoObserver():
        mRefCount(1), d_ptr(new IRQNwkInfoObserverPrivate(this)), mInitPrivateSuccess(false)
{
    if (NULL != d_ptr) 
    {
        mInitPrivateSuccess = d_ptr->Construct();
    }
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver::~IRQNwkInfoObserver()
{
    stopMonitorNwkInfo();
    
    delete d_ptr;
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::startMonitorNwkInfo()
// start monitoring network info change
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::startMonitorNwkInfo()
{
    if(d_ptr)
    {
        d_ptr->startNwkInfoMonitor();
    }
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::stopMonitorNwkInfo()
// stop monitoring network info change
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::stopMonitorNwkInfo()
{
    if(d_ptr)
    {
        d_ptr->Cancel();
    }
}
