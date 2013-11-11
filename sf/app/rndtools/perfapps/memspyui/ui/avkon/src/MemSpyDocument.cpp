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

#include "MemSpyDocument.h"

// Engine includes
#include <memspy/engine/memspyengine.h>

// User includes
#include "MemSpyAppUi.h"
#include "MemSpySettings.h"

//cigasto
#include <memspysession.h>


CMemSpyDocument::CMemSpyDocument(CEikApplication& aApp)
:   CAknDocument(aApp)    
    {
    }


CMemSpyDocument::~CMemSpyDocument()
    {
    delete iSettings;
    if( iMemSpySession != NULL )
    	{
		iMemSpySession->Close();
		delete iMemSpySession;
    	}
    }


void CMemSpyDocument::ConstructL()
    {
    RFs& fsSession = CCoeEnv::Static()->FsSession();
    //cigasto
    //iEngine = CMemSpyEngine::NewL( fsSession );
    
    iMemSpySession = new (ELeave) RMemSpySession();    
    
    TInt error = iMemSpySession->Connect();	
	if(error != KErrNotFound)
		{
		User::LeaveIfError( error );
		}		
    
    //iSettings = CMemSpySettings::NewL( fsSession, *iEngine );
	iSettings = CMemSpySettings::NewL( fsSession, *iMemSpySession );
    }


CMemSpyDocument* CMemSpyDocument::NewL(CEikApplication& aApp)
    {
    CMemSpyDocument* self = new (ELeave) CMemSpyDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
CMemSpyEngine& CMemSpyDocument::Engine()
    {
    return *iEngine;
    }


const CMemSpyEngine& CMemSpyDocument::Engine() const
    {
    return *iEngine;
    }
*/

CMemSpySettings& CMemSpyDocument::Settings()
    {
    return *iSettings;
    }


const CMemSpySettings& CMemSpyDocument::Settings() const
    {
    return *iSettings;
    }


CEikAppUi* CMemSpyDocument::CreateAppUiL()
    {
    //return new (ELeave) CMemSpyAppUi( *iEngine );
	return new (ELeave) CMemSpyAppUi( *iMemSpySession );
    }

