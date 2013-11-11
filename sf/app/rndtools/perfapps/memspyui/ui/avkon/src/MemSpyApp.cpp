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

#include "MemSpyApp.h"

// System includes
#include <eikstart.h>

// User includes
#include "MemSpyDocument.h"



TUid CMemSpyApp::AppDllUid() const
    {
    return KUidMemSpy;
    }


CApaDocument* CMemSpyApp::CreateDocumentL()
    {
    return CMemSpyDocument::NewL( *this );
    }


EXPORT_C CApaApplication* NewApplication()
    {
    return new CMemSpyApp;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

