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
* Description: This module contains the implementation of application  
* main function.
*
*/

// INCLUDE FILES
#include <eikstart.h>
#include "UITestServerStarterAppUi.h"
#include "UITestServerStarterApplication.h"
#include <eikenv.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

/**
 * Application factory function.
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CUITestServerStarterApplication;
	}

// ================= MEMBER FUNCTIONS =========================================
/**
 * Application main function.
 */
GLDEF_C TInt E32Main()
	{
	
	RDebug::Printf( "UITESTING: UITestServerStarter - E32Main" );
	
	TInt ret = EikStart::RunApplication( NewApplication );
	
    RDebug::Printf( "UITESTING: UITestServerStarter - E32Main End" );
    
    return ret;
	}

// End of File
