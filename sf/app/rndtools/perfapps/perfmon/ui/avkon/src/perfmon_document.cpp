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
#include "perfmon_document.h"
#include "perfmon_appui.h"
#include "perfmon_model.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CPerfMonDocument::CPerfMonDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------

// destructor
CPerfMonDocument::~CPerfMonDocument()
    {
    delete iModel;
    }

// ----------------------------------------------------

// EPOC default constructor can leave.
void CPerfMonDocument::ConstructL()
    {
    iModel = CPerfMonModel::NewL();
    }

// ----------------------------------------------------

// Two-phased constructor.
CPerfMonDocument* CPerfMonDocument::NewL(CEikApplication& aApp)
    {
    CPerfMonDocument* self = new(ELeave) CPerfMonDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CPerfMonDocument::CreateAppUiL()
// constructs CPerfMonAppUi
// ----------------------------------------------------
//
CEikAppUi* CPerfMonDocument::CreateAppUiL()
    {
    return new (ELeave) CPerfMonAppUi;
    }

// ----------------------------------------------------
// CPerfMonDocument::OpenFileL
// Overrides CAknDocument::OpenFileL to support document file
// ----------------------------------------------------
//
CFileStore* CPerfMonDocument::OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs)
    {
    return CEikDocument::OpenFileL(aDoOpen, aFilename, aFs);
    }

// ----------------------------------------------------

// End of File  
