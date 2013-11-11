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
* Description:   
*
*/



// INCLUDE FILES
#include "profiler_gui_app.h"
#include "profiler_gui_document.h"
#include <piprofiler/ProfilerTraces.h>

#include <eikstart.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProfilerGuiApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CProfilerGuiApp::AppDllUid() const
    {
    return KUidProfilerGui;
    }
   
// ---------------------------------------------------------
// CProfilerGuiApp::CreateDocumentL()
// Creates CProfilerGuiDocument object
// ---------------------------------------------------------
//
CApaDocument* CProfilerGuiApp::CreateDocumentL()
    {
    return CProfilerGuiDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CProfilerGuiApp;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File  

