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
* Description:  AppDoc implemetation
*
*/


#include "HtiAdminAppUi.h"
#include "HtiAdminDocument.h"

// Standard Symbian OS construction sequence
CHtiAdminDocument* CHtiAdminDocument::NewL(CEikApplication& aApp)
    {
    CHtiAdminDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CHtiAdminDocument* CHtiAdminDocument::NewLC(CEikApplication& aApp)
    {
    CHtiAdminDocument* self = new (ELeave) CHtiAdminDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CHtiAdminDocument::ConstructL()
    {
    // no implementation required
    }

CHtiAdminDocument::CHtiAdminDocument(CEikApplication& aApp) : CAknDocument(aApp)
    {
    // no implementation required
    }

CHtiAdminDocument::~CHtiAdminDocument()
    {
    // no implementation required
    }

CEikAppUi* CHtiAdminDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CHtiAdminAppUi;
    return appUi;
    }

