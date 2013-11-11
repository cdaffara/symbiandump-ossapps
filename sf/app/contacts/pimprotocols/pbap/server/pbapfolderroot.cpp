// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "pbapfolderclient.h"
#include "pbapappheader.h"
#include "pbapfolderroot.h"

#include "btaccesshostlog.h"

// constants
_LIT(KFolderRoot, "/");


/*static*/ CFolderRoot* CFolderRoot::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CFolderRoot(aClient);
	}


CFolderRoot::CFolderRoot(MVirtualFolderClient& aClient)
: CFolderBase(aClient, KFolderRoot())
	{
	LOG_FUNC
	}


TInt CFolderRoot::Get(const TDesC& /*aObjectName*/, CPbapAppHeader* aGetData)
	{
	LOG_FUNC

	// we need to take ownership of aGetData but don't require it
	delete aGetData;

	return KErrNotReady;
	}

void CFolderRoot::CancelGet()
	{
	LOG_FUNC
	// do nothing
	}

void CFolderRoot::GetComplete()
	{
	LOG_FUNC
	// do nothing
	}

