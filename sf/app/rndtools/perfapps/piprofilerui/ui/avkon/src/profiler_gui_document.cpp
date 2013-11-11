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
#include "profiler_gui_document.h"
#include "profiler_gui_appui.h"
#include "profiler_gui_model.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CProfilerGuiDocument::CProfilerGuiDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------

// destructor
CProfilerGuiDocument::~CProfilerGuiDocument()
    {
    delete iModel;
    }

// ----------------------------------------------------

// EPOC default constructor can leave.
void CProfilerGuiDocument::ConstructL()
    {
    iModel = CProfilerGuiModel::NewL();
    }

// ----------------------------------------------------

// Two-phased constructor.
CProfilerGuiDocument* CProfilerGuiDocument::NewL(CEikApplication& aApp)
    {
    CProfilerGuiDocument* self = new(ELeave) CProfilerGuiDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CProfilerGuiDocument::CreateAppUiL()
// constructs CProfilerGuiAppUi
// ----------------------------------------------------
//
CEikAppUi* CProfilerGuiDocument::CreateAppUiL()
    {
    return new (ELeave) CProfilerGuiAppUi;
    }

// End of File  
