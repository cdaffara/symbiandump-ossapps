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



// INCLUDES
#include "glxloggingdocument.h"
#include "glxloggingapplication.h"

// CONSTANTS
static const TUid KUidGlxLoggingApp = {0x2000A7BD};

// -----------------------------------------------------------------------------
// CGlxLoggingApplication::CreateDocumentL
// -----------------------------------------------------------------------------
CApaDocument* CGlxLoggingApplication::CreateDocumentL()
    {  
    // Create an GlxLogging document, and return a pointer to it
    CApaDocument* document = CGlxLoggingDocument::NewL(*this);
    return document;
    }

// -----------------------------------------------------------------------------
// CGlxLoggingApplication::AppDllUid
// -----------------------------------------------------------------------------
TUid CGlxLoggingApplication::AppDllUid() const
    {
    // Return the UID for the GlxLogging application
    return KUidGlxLoggingApp;
    }

