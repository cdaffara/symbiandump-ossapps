/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// INCLUDE FILES
#include "loadgen_app.h"
#include "loadgen_document.h"

#include <eikstart.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLoadGenApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CLoadGenApp::AppDllUid() const
    {
    return KUidLoadGen;
    }

// ---------------------------------------------------------
// CDictionaryStore* CLoadGenApp::OpenIniFileLC(RFs& aFs) const
// overrides CAknApplication::OpenIniFileLC to enable INI file support
// ---------------------------------------------------------
//
CDictionaryStore* CLoadGenApp::OpenIniFileLC(RFs& aFs) const
    {
    return CEikApplication::OpenIniFileLC(aFs);
    }
   
// ---------------------------------------------------------
// CLoadGenApp::CreateDocumentL()
// Creates CLoadGenDocument object
// ---------------------------------------------------------
//
CApaDocument* CLoadGenApp::CreateDocumentL()
    {
    return CLoadGenDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CLoadGenApp;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }
   

// End of File  

