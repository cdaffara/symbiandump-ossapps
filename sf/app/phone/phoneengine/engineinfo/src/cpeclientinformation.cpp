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
* Description: Interface class for instantiation of call client information
*
*/

#include "cpeclientinformation.h"
#include "cpeclientinformationimpl.h"

// -----------------------------------------------------------------------------
// CPEClientInformation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEClientInformation* CPEClientInformation::NewL()
    {
    return CPEClientInformationImpl::NewL();
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CPEClientInformation::CPEClientInformation()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPEClientInformation::~CPEClientInformation()
    {
    }

// End of File
