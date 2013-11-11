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
* Description: This file contains CAppUIApp class definition.
*
*/

// INCLUDE FILES
#include    "AppUIApp.h"
#include    "AppUIDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSTIFAppUIApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CAppUIApp::AppDllUid() const
    {
    return KUidAppUI;
    }

   
// ---------------------------------------------------------
// CSTIFAppUIApp::CreateDocumentL()
// Creates CSTIFAppUIDocument object
// ---------------------------------------------------------
//
CApaDocument* CAppUIApp::CreateDocumentL()
    {
    return CAppUIDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//

	#include <eikstart.h>
	// ---------------------------------------------------------
	// NewApplication() 
	// Exported function
	// Returns: CApaApplication: 
	// ---------------------------------------------------------
	//
	LOCAL_C CApaApplication* NewApplication()
		{
		return new CAppUIApp;
		}
	// ---------------------------------------------------------
	// E32Main() 
	// EXE Entry point
	// Returns: KErrNone: No error
	// ---------------------------------------------------------
	//
	GLDEF_C TInt E32Main()
		{
		return EikStart::RunApplication(NewApplication);
		}

// End of File
