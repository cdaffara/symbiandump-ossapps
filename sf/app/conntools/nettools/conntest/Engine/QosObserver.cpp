/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CQosObserver.
*
*/

// INCLUDE FILES
#include "QosObserver.h"
#include "UINotify.h"


// ================= MEMBER FUNCTIONS =========================================


// ----------------------------------------------------------------------------
// CQosObserver::CQosObserver(...)
// Constructor
// ----------------------------------------------------------------------------
//
CQosObserver::CQosObserver(MUINotify& aConsole)
: iConsole(aConsole)
{
}

// ----------------------------------------------------------------------------
// CQosObserver::~CQosObserver()
// Destructor
// ----------------------------------------------------------------------------
//
CQosObserver::~CQosObserver()
{
}


// ---------------------------------------------------------
// CQosObserver::Event(const CQoSEventBase& aQosEvent)
// QoS event receiver.
// ---------------------------------------------------------
//
void CQosObserver::Event(const CQoSEventBase& aQosEvent)
{
    iConsole.PrintNotify(_L("KK"));
}


