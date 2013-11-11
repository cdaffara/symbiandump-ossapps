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
#include "loadgen_document.h"
#include "loadgen_appui.h"
#include "loadgen_model.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CLoadGenDocument::CLoadGenDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------

// destructor
CLoadGenDocument::~CLoadGenDocument()
    {
    delete iModel;
    }

// ----------------------------------------------------

// EPOC default constructor can leave.
void CLoadGenDocument::ConstructL()
    {
    iModel = CLoadGenModel::NewL();
    }

// ----------------------------------------------------

// Two-phased constructor.
CLoadGenDocument* CLoadGenDocument::NewL(CEikApplication& aApp)
    {
    CLoadGenDocument* self = new(ELeave) CLoadGenDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CLoadGenDocument::CreateAppUiL()
// constructs CLoadGenAppUi
// ----------------------------------------------------
//
CEikAppUi* CLoadGenDocument::CreateAppUiL()
    {
    return new (ELeave) CLoadGenAppUi;
    }

// ----------------------------------------------------
// CLoadGenDocument::OpenFileL
// Overrides CAknDocument::OpenFileL to support document file
// ----------------------------------------------------
//
CFileStore* CLoadGenDocument::OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs)
    {
    return CEikDocument::OpenFileL(aDoOpen, aFilename, aFs);
    }

// ----------------------------------------------------

// End of File  
