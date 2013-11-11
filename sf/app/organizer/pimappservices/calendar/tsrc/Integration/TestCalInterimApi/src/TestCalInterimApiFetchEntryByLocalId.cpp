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

#include "TestCalInterimApiFetchEntryByLocalId.h"
#include <calentry.h>
#include <calentryview.h>

_LIT(KGuid,							"guid");
_LIT(KLocalId,						"localid");
_LIT(KAllocTest,					"alloctest");

_LIT(KInfoGuidAndLocalId,			"Guid of the entry : %S , Local Id of the entry : %d");

_LIT(KErrEntryNotFound,				"The entry is not found for the local Id %d");
_LIT(KErrInFetchLocalId,			"Entry is not fetched correctly");


CTestCalInterimApiFetchEntryByLocalId::CTestCalInterimApiFetchEntryByLocalId()
:	CTestCalInterimApiSuiteStepBase()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiFetchEntryByLocalId);
	}

CTestCalInterimApiFetchEntryByLocalId::~CTestCalInterimApiFetchEntryByLocalId()
/**
 * Destructor
 */
	{
	delete iCalEntryView;
	}


/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiFetchEntryByLocalId::doTestStepL()
	{
	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	TInt	localId;
	TBool	allocTest;

	TESTL(GetIntFromConfig(ConfigSection(), KLocalId, localId));
	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);

	CCalEntry*	entry = NULL;
	if ( allocTest )
		{
		entry = OOMForFetchEntryByLocalIdL(localId);
		}
	else
		{
		// Fetch the entry by passing the local id of the entry
		TRAPD( err, entry = iCalEntryView->FetchL(localId) );
		TESTL( err == KErrNone );
		}
		
	CheckEntryL(entry, localId);
	delete entry;
	entry = NULL;
	return TestStepResult();
	}

/** Check the retrieved entry's GUID with the expected Guid
@param aEntry Pointer to the CCalEntry
*/	
void CTestCalInterimApiFetchEntryByLocalId::CheckEntryL(CCalEntry* aEntry, const TInt aLocalId)
	{
	TPtrC	gUid;
	GetStringFromConfig(ConfigSection(), KGuid, gUid);
	HBufC8*	entryGuid = HBufC8::NewLC(gUid.Length());
	entryGuid->Des().Copy(gUid);
	
	// If entry is present, then check the GUID of the entry
	if ( aEntry != NULL )
		{
		if ( aEntry->UidL() == entryGuid->Des() )
			{
			INFO_PRINTF3(KInfoGuidAndLocalId, &gUid, aLocalId);
			}
		else
			{
			ERR_PRINTF1(KErrInFetchLocalId);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KErrEntryNotFound, aLocalId);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(entryGuid);
	}

/** Perform OOM testing for fetch entry by local id
@param aEntry Pointer to the CCalEntry
@param aLocalId	localid of the entry to be fetched
*/	
CCalEntry* CTestCalInterimApiFetchEntryByLocalId::OOMForFetchEntryByLocalIdL(const TInt aLocalId)
	{
	CCalEntry* entry = NULL;
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAPD( err, entry = iCalEntryView->FetchL(aLocalId) );
				
		if ( err == KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for FetchEntryByLocalId is done"));
			break;
			}
		if ( err != KErrNoMemory )
			{
			SetTestStepResult(EFail);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}	
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	return entry;
	}
	
