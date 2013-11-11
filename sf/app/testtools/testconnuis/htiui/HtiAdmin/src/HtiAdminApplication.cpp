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
* Description:  Application implemetation
*
*/


#include "HtiAdminDocument.h"
#include "HtiAdminApplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidHtiAdminApp = {0x1020DEB5};

CApaDocument* CHtiAdminApplication::CreateDocumentL()
    {
    //HTI_LOG_TEXT("CreateDocumentL");
    // Create an HtiAdmin document, and return a pointer to it
    CApaDocument* document = CHtiAdminDocument::NewL(*this);
    return document;
    }

TUid CHtiAdminApplication::AppDllUid() const
    {
    // Return the UID for the HtiAdmin application
    return KUidHtiAdminApp;
    }

