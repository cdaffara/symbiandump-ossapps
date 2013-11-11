/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The application class for ConnTest
*
*/

// INCLUDE FILES
#include <eikstart.h>
#include    "ConnTestApp.h"
#include    "ConnTestDocument.h"

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CConnTestApp
// Returns: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CConnTestApp;
    }

GLDEF_C TInt E32Main()
    {
    RDebug::Print(_L("ConnTest: E32Main()"));
    return EikStart::RunApplication(NewApplication);
    }

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CConnTestApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CConnTestApp::AppDllUid() const
    {
    return KUidConnTest;
    }

   
// ---------------------------------------------------------
// CConnTestApp::CreateDocumentL()
// Creates CConnTestDocument object
// ---------------------------------------------------------
//
CApaDocument* CConnTestApp::CreateDocumentL()
    {
    return CConnTestDocument::NewL( *this );
    }

// ---------------------------------------------------------
// E32Dll(TDllReason) 
// Entry point function for EPOC Apps
// Returns: KErrNone: No error
// ---------------------------------------------------------
//
/*
GLDEF_C TInt E32Dll( TDllReason )
{
    return KErrNone;
}
*/
// End of File  

