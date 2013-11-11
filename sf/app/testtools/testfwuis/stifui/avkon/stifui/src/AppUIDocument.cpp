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
* Description: This file contains CAppUIDocument class definition.
*
*/

// INCLUDE FILES
#include "AppUIDocument.h"
#include "AppUIAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CAppUIDocument::CAppUIDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CAppUIDocument::~CAppUIDocument()
    {
    }

// Symbian OS default constructor can leave.
void CAppUIDocument::ConstructL()
    {
    }

// Two-phased constructor.
CAppUIDocument* CAppUIDocument::NewL(
        CEikApplication& aApp)     // CAppUIApp reference
    {
    CAppUIDocument* self = new (ELeave) CAppUIDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CAppUIDocument::CreateAppUiL()
// constructs CAppUIAppUi
// ----------------------------------------------------
//
CEikAppUi* CAppUIDocument::CreateAppUiL()
    {
    return new (ELeave) CAppUIAppUi;
    }

// End of File
