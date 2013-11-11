/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxememorymonitorprivate.h"

#ifdef Q_OS_SYMBIAN
#include <hal.h>
#endif // Q_OS_SYMBIAN

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"

namespace
{
    const int FREEING_RETRIES = 2;
    const int MONITORING_INTERVAL = 10*1000; // in milliseconds
}

/*!
* Constructor
*/
CxeMemoryMonitorPrivate::CxeMemoryMonitorPrivate(CxeFeatureManager &features)
    : mFeatures(features)
{
    CX_DEBUG_ENTER_FUNCTION();

    mFeatures.configuredValues(CxeVariationKeys::FREE_MEMORY_LEVELS, mLevels);

    CX_DEBUG(("CxeMemoryMonitorPrivate - trigger level: %d bytes", mLevels.value(Cxe::FreeMemoryTrigger)));
    CX_DEBUG(("CxeMemoryMonitorPrivate - target level:  %d bytes", mLevels.value(Cxe::FreeMemoryTarget)));

#ifdef Q_OS_SYMBIAN
    // Request free memory as defined in Feature Manager.
    // If we cannot have enough free memory, throw error.
    bool ok = requestFreeMemory(mLevels.value(Cxe::FreeMemoryTarget));
    qt_symbian_throwIfError(ok ? KErrNone : KErrNoMemory);
#endif // Q_OS_SYMBIAN

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxeMemoryMonitorPrivate::~CxeMemoryMonitorPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();
    stopMonitoring();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Get the amount of free memory (RAM).
* @return Free RAM in bytes.
*/
int CxeMemoryMonitorPrivate::free() const
{
#ifdef Q_OS_SYMBIAN
    int ram(0);
    HAL::Get(HALData::EMemoryRAMFree, ram);
    return ram;
#else
    //!@todo: real implementation for desktop etc?
    return 1024*1024*1024;
#endif // Q_OS_SYMBIAN
}


/*!
* Request to free memory (RAM) to achieve asked level.
* @param required Required free memory after this call.
* @return Was it possible to achieve the required level of free memory.
*/
bool CxeMemoryMonitorPrivate::requestFreeMemory(int required)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (free() < required) {
#ifdef Q_OS_SYMBIAN
    qt_symbian_throwIfError(mOomMonitor.Connect());

    int i(0);
    int status(KErrGeneral);
    do {
        status = mOomMonitor.RequestFreeMemory(required);
    } while (status != KErrNone && ++i < FREEING_RETRIES);

    mOomMonitor.Close();
#endif // Q_OS_SYMBIAN
    }

    // If we have the free RAM we were asked for, all good.
    bool ok = free() >= required;

    CX_DEBUG_EXIT_FUNCTION();
    return ok;
}

/*!
* Start monitoring free memory.
*/
void CxeMemoryMonitorPrivate::startMonitoring()
{
    if (!mTimer.isActive()) {
        connect(&mTimer, SIGNAL(timeout()), this, SLOT(checkMemory()), Qt::UniqueConnection);
        mTimer.start(MONITORING_INTERVAL);
    }
}

/*!
* Stop monitoring free memory.
*/
void CxeMemoryMonitorPrivate::stopMonitoring()
{
    mTimer.stop();
}

/*!
* Check free memory. Try to release memory if below trigger level.
*/
void CxeMemoryMonitorPrivate::checkMemory()
{
    CX_DEBUG_ENTER_FUNCTION();

    if(free() < mLevels.value(Cxe::FreeMemoryTrigger)) {
        // Memory too low, request more free memory.
        CX_DEBUG(("[WARNING] CxeMemoryMonitorPrivate - memory low, requesting more.."));
        requestFreeMemory(mLevels.value(Cxe::FreeMemoryTarget));
    }
    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
