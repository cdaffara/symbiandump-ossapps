/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging utility application for MC Photos
*
*/



#include "glxloggingappui.h"
#include "glxloggingdocument.h"


CGlxLoggingDocument* CGlxLoggingDocument::NewL(CEikApplication& aApp)
    {
    CGlxLoggingDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CGlxLoggingDocument* CGlxLoggingDocument::NewLC(CEikApplication& aApp)
    {
    CGlxLoggingDocument* self = new (ELeave) CGlxLoggingDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CGlxLoggingDocument::ConstructL()
    {
	// no implementation required
    }    

CGlxLoggingDocument::CGlxLoggingDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }

CGlxLoggingDocument::~CGlxLoggingDocument()
    {
	// no implementation required
    }

CEikAppUi* CGlxLoggingDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CGlxLoggingAppUi;
    return appUi;
    }

