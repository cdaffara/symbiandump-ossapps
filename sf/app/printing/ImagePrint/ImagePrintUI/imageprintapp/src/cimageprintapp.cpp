/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include 	<eikstart.h>
#include    "cimageprintapp.h"
#include    "cimageprintdoc.h"

LOCAL_C CApaApplication* NewApplication()
    {
    return new CImagePrintApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

TUid CImagePrintApp::AppDllUid() const
    {
    return KUidImagePrint;
    }

CApaDocument* CImagePrintApp::CreateDocumentL()
    {
    return CImagePrintDoc::NewL( *this );
    }

// End of File
