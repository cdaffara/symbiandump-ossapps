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

#include "cxutils.h"
#include "cxuiapplication.h"
#include "cxesettings.h"
#include "cxuiapplicationframeworkmonitor.h"
#include "cxuiapplicationframeworkmonitorprivate.h"


/*!
* Constructor
*/
CxuiApplicationFrameworkMonitor::CxuiApplicationFrameworkMonitor(CxuiApplication &application, CxeSettings &settings)
{
    CX_DEBUG_ENTER_FUNCTION();
    p = new CxuiApplicationFrameworkMonitorPrivate(this, application, settings);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxuiApplicationFrameworkMonitor::~CxuiApplicationFrameworkMonitor()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete p;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Current foreground owning state of this application.
* @return Foreground owning state.
*/
CxuiApplicationFrameworkMonitor::ForegroundState CxuiApplicationFrameworkMonitor::foregroundState() const
{
    return p->foregroundState();
}

/*!
* Is USB connected in mass memory mode?
* @return True if USB mass memory mode is active and connected, false otherwise.
*/
bool CxuiApplicationFrameworkMonitor::isUsbMassMemoryModeActive() const
{
    return p->isUsbMassMemoryModeActive();
}

// end of file
