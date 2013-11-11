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

#include "cxememorymonitor.h"
#include "cxememorymonitorprivate.h"

/*!
* Constructor
*/
CxeMemoryMonitor::CxeMemoryMonitor(CxeFeatureManager &features)
{
    p = new CxeMemoryMonitorPrivate(features);
}

/*!
* Destructor
*/
CxeMemoryMonitor::~CxeMemoryMonitor()
{
    delete p;
}

/*!
* Get the amount of free memory (RAM).
* @return Free RAM in bytes.
*/
int CxeMemoryMonitor::free() const
{
    return p->free();
}

/*!
* Request to free memory (RAM) to achieve asked level.
* @param required Required free memory after this call.
* @return Was it possible to achieve the required level of free memory.
*/
bool CxeMemoryMonitor::requestFreeMemory(int required)
{
    return p->requestFreeMemory(required);
}

/*!
* Slot to start monitoring free memory.
*/
void CxeMemoryMonitor::startMonitoring()
{
    p->startMonitoring();
}

/*!
* Slot to stop monitoring free memory.
*/
void CxeMemoryMonitor::stopMonitoring()
{
    p->stopMonitoring();
}

// end of file
