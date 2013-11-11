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
#include "pbapfoldertree.h"
#include "pbapfoldertelecom.h"
#include "btaccesshostlog.h"

// constants
_LIT(KFolderTelecom, "telecom");


/*static*/CFolderTelecom* CFolderTelecom::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CFolderTelecom(aClient);
	}


CFolderTelecom::CFolderTelecom(MVirtualFolderClient& aClient)
: CFolderBase(aClient, KFolderTelecom())
	{
	LOG_FUNC
	}

	
/**
Handle get request from server.
The file representations of the phonebooks (i.e. pb.vcf,
 ich.vcf, mch.vcf, och.vcf and cch.vcf) are stored in the telecom folder
However for simplicity (and less code duplication) valid get requests on
these files have been converted into get requests on their corresponding
folder representations 
*/
TInt CFolderTelecom::Get(const TDesC& /*aObjectName*/, CPbapAppHeader* aGetData)
	{
	LOG_FUNC

	TInt error = KErrNotReady;
	
	// pull vcard not allowed from telecom folder 
	if (aGetData->Operation() == EPullVCard)
		{
		error = KErrNotSupported;
		}

	// we need to take ownership of aGetData but don't require it
	delete aGetData;

	return error;
	}


void CFolderTelecom::CancelGet()
	{
	LOG_FUNC
	// do nothing
	}

void CFolderTelecom::GetComplete()
	{
	LOG_FUNC
	// do nothing
	}
