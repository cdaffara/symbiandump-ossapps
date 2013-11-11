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
* CUITestServerStarterDocument class member functions.
*
*/

// INCLUDE FILES
#include "UITestServerStarterAppUi.h"
#include "UITestServerStarterDocument.h"


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

     Class: CUITestServerStarterDocument

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: CEikApplication& aApp: in: Reference to application..
     
     Return Values: Pointer to new CUITestServerStarterDocument object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterDocument* CUITestServerStarterDocument::NewL( CEikApplication& aApp )
	{
	CUITestServerStarterDocument* self = NewLC (aApp);
	CleanupStack::Pop (self);
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterDocument

     Method: NewLC

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: CEikApplication& aApp: in: Reference to application.
     
     Return Values: Pointer to new CUITestServerStarterDocument object.

     Errors/Exceptions: Leave if one of the called functions leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterDocument* CUITestServerStarterDocument::NewLC (CEikApplication& aApp)
	{
	CUITestServerStarterDocument* self = new ( ELeave ) CUITestServerStarterDocument( aApp );

	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterDocument

     Method: ConstructL

     Description: This is second phase of two-phased constructor.

     This is second phase of two-phased constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterDocument::ConstructL()
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterDocument

     Method: CUITestServerStarterDocument

     Description: Constructor.

     Constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterDocument::CUITestServerStarterDocument( CEikApplication& aApp ) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterDocument

     Method: ~CUITestServerStarterDocument

     Description: Destructor.

     Destructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterDocument::~CUITestServerStarterDocument ()
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterDocument

     Method: CreateAppUiL

     Description: Creates AppUi object.

     Creates AppUi object.

     Parameters: None.
     
     Return Values: Pointer to AppUi object.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CEikAppUi* CUITestServerStarterDocument::CreateAppUiL ()
	{
	return ( static_cast <CEikAppUi*>( new ( ELeave ) CUITestServerStarterAppUi ) );
	}

// End of File
