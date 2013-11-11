/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/

// INCLUDE FILES
#include "DirectPrintApp.hrh"
#include "DirectPrintAppDocument.h"
#include "DirectPrintAppApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectPrintAppApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CDirectPrintAppApplication::CreateDocumentL()
	{
	// Create an DirectPrintApp document, and return a pointer to it
	return CDirectPrintAppDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CDirectPrintAppApplication::AppDllUid() const
	{
	// Return the UID for the DirectPrintApp application
	return KUidDirectPrintAppApp;
	}

// End of File
