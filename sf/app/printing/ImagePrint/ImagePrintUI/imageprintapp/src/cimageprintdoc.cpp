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


#include "cimageprintdoc.h"
#include "cimageprintappui.h"
#include "cimageprintengine.h"
#include "ciffactory.h"

CImagePrintDoc::CImagePrintDoc(
    CEikApplication& aApp )
    : CAknDocument(aApp)
    {
    }

// DESTRUCTION
CImagePrintDoc::~CImagePrintDoc()
    {
    delete iEngine;
    }

// default constructor
void CImagePrintDoc::InitializeEngineL()
    {
    iEngine = CImagePrintEngine::NewL( 0 );
    }

// Two-phased constructor.
CImagePrintDoc* CImagePrintDoc::NewL(
    CEikApplication& aApp )
    {
    CImagePrintDoc* self = new (ELeave) CImagePrintDoc( aApp );
    return self;
    }

// METHODS

//Creates AppUi class for application
CEikAppUi* CImagePrintDoc::CreateAppUiL()
    {
    return new (ELeave) CImagePrintAppUi;
    }

// Returns pointer to engine
CImagePrintEngine* CImagePrintDoc::Engine()
    {
    return iEngine;
    }

// Returns discovery interface
MDiscovery* CImagePrintDoc::Discovery()
    {
    return iEngine->InterfaceFactory().DiscoveryIF();
    }

// Returns print job interface
MPrintJob* CImagePrintDoc::PrintJob()
    {
    return iEngine->InterfaceFactory().PrintJobIF();
    }


// Returns print settings interface
MPrintSettings* CImagePrintDoc::PrintSettings()
    {
    return iEngine->InterfaceFactory().SettingsIF();
    }

// Set selected images to engine.
void CImagePrintDoc::SetImageArrayL(
    CDesCArrayFlat* aArray )
    {
    iEngine->SetImageArrayL( aArray );
    }

// Restarts Image Print engine
void CImagePrintDoc::RestartEngine()
    {
    iEngine->RestartEngine();
    }

//  End of File
