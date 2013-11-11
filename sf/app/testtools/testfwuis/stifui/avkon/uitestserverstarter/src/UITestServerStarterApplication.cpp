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
* Description: This module contains the implementation of 
* CUITestServerStarterApplication class member functions.
*
*/

// INCLUDE FILES
#include "UITestServerStarter.hrh"
#include "UITestServerStarterDocument.h"
#include "UITestServerStarterApplication.h"

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


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterApplication

     Method: CreateDocumentL 

     Creates CUITestServerStarterDocument document object.

	 Creates CUITestServerStarterDocument document object. The returned
	 pointer in not owned by the CUITestServerStarterApplication object.

     Parameters: None
     
     Return Values: A pointer to the created document object.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CApaDocument* CUITestServerStarterApplication::CreateDocumentL ()
	{
	return (static_cast<CApaDocument*> (CUITestServerStarterDocument::NewL ( *this) ) );
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterApplication

     Method: AppDllUid

     Gets application Uid

	 Gets application Uid

     Parameters: None
     
     Return Values: Application Uid.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TUid CUITestServerStarterApplication::AppDllUid() const
	{
	return KUidUITestServerStarterApp;
	}

/*
-------------------------------------------------------------------------------

     Class: PreDocConstructL

     Method: AppDllUid

     Preconstructs document.

     Preconstructs document.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterApplication::PreDocConstructL()
	{
	// This call allow us to run multiple instances of UI application in same time.
	CEikApplication::PreDocConstructL();
	}

// End of File
