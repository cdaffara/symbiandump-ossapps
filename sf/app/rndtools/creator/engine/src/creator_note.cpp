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


#include "engine.h"
#include "enginewrapper.h"

#include "creator_note.h" 
#include "creator_traces.h"

//----------------------------------------------------------------------------

CNotepadParameters::CNotepadParameters()
    {
    LOGSTRING("Creator: CNotepadParameters::CNotepadParameters");

    iNoteText = HBufC::New(KNotepadFieldLength);
    }

CNotepadParameters::~CNotepadParameters()
    {
    LOGSTRING("Creator: CNotepadParameters::~CNotepadParameters");

    delete iNoteText;
    }

//----------------------------------------------------------------------------

CCreatorNotepad* CCreatorNotepad::NewL(CCreatorEngine* aEngine)
    {
    CCreatorNotepad* self = CCreatorNotepad::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorNotepad* CCreatorNotepad::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorNotepad* self = new (ELeave) CCreatorNotepad;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorNotepad::CCreatorNotepad() : iFs ( CEikonEnv::Static()->FsSession() )
    {
    }

void CCreatorNotepad::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorNotepad::ConstructL");

    iEngine = aEngine;
    iNotepadWrapper = CCreatorNotepadWrapper::NewL();
    }

CCreatorNotepad::~CCreatorNotepad()
    {
    LOGSTRING("Creator: CCreatorNotepad::~CCreatorNotepad");
    
    if (iParameters)
        {
        delete iParameters;
        iParameters = NULL;
        }
    
    if (iNotepadWrapper)
    	{
		delete iNotepadWrapper;
		iNotepadWrapper = NULL;
    	}
    }

//----------------------------------------------------------------------------

TBool CCreatorNotepad::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorNotepad::AskDataFromUserL");

    CCreatorModuleBase::AskDataFromUserL(aCommand);
        
    if ( aCommand == ECmdDeleteNotes )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Notes?"), this, ECreatorModuleDelete );
        }
    
    // By Creator not supported because 
    // note id is not available via Notepad API

    return iEngine->GetEngineWrapper()->EntriesQueryDialog( &iEntriesToBeCreated, _L("How many entries to create?"), EFalse,  this, ECreatorModuleStart );
    }


//----------------------------------------------------------------------------

TInt CCreatorNotepad::CreateNoteEntryL(CNotepadParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorNotepad::CreateNoteEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    // random data needed if no predefined data available
    if (!aParameters)
        {
        iParameters = new(ELeave) CNotepadParameters;
        }
    else
        {
        iParameters = aParameters;
        }

    TPtr noteTextPtr = iParameters->iNoteText->Des();
    noteTextPtr.Copy( iEngine->RandomString(CCreatorEngine::EMessageText).Left( noteTextPtr.MaxLength() ) );
    
    return iNotepadWrapper->CreateNoteL( noteTextPtr );
    }

//----------------------------------------------------------------------------
void CCreatorNotepad::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorNotepad::DeleteAllL");
    iNotepadWrapper->DeleteAllL();
    }

//----------------------------------------------------------------------------
void CCreatorNotepad::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorNotepad::DeleteAllCreatedByCreatorL");
    // Not supported because note id is not available via Notepad API
    User::Leave( KErrNotSupported );
    }
