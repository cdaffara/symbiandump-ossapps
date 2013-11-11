/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSFWImplementation.
*
*/

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>

#include "CFSFWImplementation.h"
//<cmail>
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailPluginManager.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSFWImplementation::CFSFWImplementation
// -----------------------------------------------------------------------------
CFSFWImplementation::CFSFWImplementation()
{
    NM_FUNCTION;
    
	iPluginManager = NULL;
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::~CFSFWImplementation
// -----------------------------------------------------------------------------
CFSFWImplementation::~CFSFWImplementation()
{
    NM_FUNCTION;
    
	delete iPluginManager;
}
// -----------------------------------------------------------------------------
// CFSFWImplementation::NewLC
// -----------------------------------------------------------------------------
CFSFWImplementation* CFSFWImplementation::NewLC(TInt aConfiguration)
{
    NM_FUNCTION;
    
    CFSFWImplementation* impl = new (ELeave) CFSFWImplementation();
    CleanupStack:: PushL(impl);
    impl->ConstructL(aConfiguration);
    return impl;
} 

// -----------------------------------------------------------------------------
// CFSFWImplementation::NewL
// -----------------------------------------------------------------------------
CFSFWImplementation* CFSFWImplementation::NewL(TInt aConfiguration)
{
    NM_FUNCTION;
    
    CFSFWImplementation* impl =  CFSFWImplementation::NewLC(aConfiguration);
    CleanupStack:: Pop(impl);
    return impl;
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::ConstructL
// -----------------------------------------------------------------------------
void CFSFWImplementation::ConstructL(TInt aConfiguration)
{
    NM_FUNCTION;
    
	// create plugin manager
	iPluginManager = CFSMailPluginManager::NewL(aConfiguration);
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::GetPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager& CFSFWImplementation::GetPluginManager( )
{
    NM_FUNCTION;
    
	return *iPluginManager;
}

