/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Interface class for instantiation of telephony information storage
*
*/

#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"

// -----------------------------------------------------------------------------
// CPEEngineInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEEngineInfo* CPEEngineInfo::NewL()
    {
    return CPEEngineInfoImpl::NewL();
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPEEngineInfo::CPEEngineInfo()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPEEngineInfo::~CPEEngineInfo()
    {
    }

// End of File
