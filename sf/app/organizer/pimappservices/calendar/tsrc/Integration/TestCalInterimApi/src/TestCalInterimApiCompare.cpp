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

#include "TestCalInterimApiCompare.h"

/**
 * Constructor
 */
CTestCalInterimApiCompare::CTestCalInterimApiCompare()
: iEntryView(NULL), iCopyEntry(NULL)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCompare);
	}

/**
 * Destructor
 */
CTestCalInterimApiCompare::~CTestCalInterimApiCompare()
	{
	delete iEntryView;
	delete iCopyEntry;
	}

/**
Base class pure virtual
Compares two entries (taken from ini file) 
*/
TVerdict CTestCalInterimApiCompare::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiCompare::doTestStepL() start"));

	TPtrC firstEntry;
	TPtrC parentOfChildEntry1;
	TBool comparevalue = EFalse;

	TESTL(GetStringFromConfig(ConfigSection(), KEntry1, firstEntry));
	GetStringFromConfig(ConfigSection(), KParentOfChildEntry1, parentOfChildEntry1);

	TESTL(GetBoolFromConfig(ConfigSection(), KCompareValue, comparevalue));

	CCalEntry* entry1 = NULL;

	HBufC8* uidentry;

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	if (parentOfChildEntry1 != KNullDesC)
		{
		entry1 = GetEntryL(parentOfChildEntry1, firstEntry, ETrue);
		CleanupStack::PushL(entry1);
		uidentry = entry1->UidL().AllocLC();

		iCopyEntry = CCalEntry::NewL(entry1->EntryTypeL(), uidentry,
	                                entry1->MethodL(), entry1->SequenceNumberL(),
	                                entry1->RecurrenceIdL(), entry1->RecurrenceRangeL());
	    CleanupStack::Pop(uidentry);
		}
	else
		{
		entry1 = GetEntryL(firstEntry, parentOfChildEntry1, EFalse);
		CleanupStack::PushL(entry1);
		uidentry = entry1->UidL().AllocLC();

		iCopyEntry = CCalEntry::NewL(entry1->EntryTypeL(), uidentry,
	                                  entry1->MethodL(), entry1->SequenceNumberL());
	    CleanupStack::Pop(uidentry);
		}
	
	TESTL(entry1 != NULL);
	TESTL(iCopyEntry != NULL);

	iCopyEntry->CopyFromL(*entry1);

	TInt trans;
	TBool hasTransp;
	hasTransp = GetIntFromConfig(ConfigSection(), KTrans, trans);

	if ( hasTransp )
		{
		TRAPD(err, iCopyEntry->SetTimeTransparencyL(CCalEntry::TTransp(trans)));	
		TESTL(err == KErrNone);
		}
	
	TPtrC dataLat;
	TPtrC dataLong;
	
	GetStringFromConfig(ConfigSection(), KLat, dataLat);
	GetStringFromConfig(ConfigSection(), KLong, dataLong);
	
	if (dataLat != KNullDesC || dataLong != KNullDesC)
		{
		SetGeoValueEntryL(dataLat, dataLong);
		}	

	if (comparevalue)
		{
		TESTL(iCopyEntry->CompareL(*entry1));
		// Compare all the properties of the entry after copy 
		CompareEntriesL(entry1, iCopyEntry);
		}
	else
		{
		TESTL(!(iCopyEntry->CompareL(*entry1)));
		}

	CleanupStack::PopAndDestroy(entry1);

	INFO_PRINTF1(_L("CTestCalInterimApiCompare::doTestStepL() finish"));
	return TestStepResult();	
	}
	
/** Returns the specific child/parent entry from database
@param	aUid The uid of parent entry that the child entry belongs to
@param	aRecuurenceId The recurrence id of the child entry we are intrested in
@param	aGetChild Determines whether to get child or parent entry
@return	Child Entry
*/
CCalEntry* CTestCalInterimApiCompare::GetEntryL(const TPtrC& aUid, const TPtrC& aRecuurenceId, TBool aGetChild)
	{
	CCalEntry* entry = NULL;
	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	HBufC8*	entryGuid = HBufC8::NewLC(aUid.Length());
	entryGuid->Des().Copy(aUid);

	iEntryView->FetchL(entryGuid->Des(), fetchEntryArray);
	CleanupStack::PopAndDestroy(entryGuid);

	for (TInt i = 0; i < fetchEntryArray.Count(); ++i)
		{
		if (aGetChild)
			{
			if (fetchEntryArray[i]->RecurrenceIdL().TimeUtcL() == FormatDateTime(aRecuurenceId))
				{
				entry = fetchEntryArray[i];
				fetchEntryArray.Remove(i);
				break; // break the loop if the entry is fetched
				}
			}
		else
			{
			if (fetchEntryArray[i]->RecurrenceIdL().TimeUtcL() == Time::NullTTime())
				{
				entry = fetchEntryArray[i];
				fetchEntryArray.Remove(i);
				break; // break the loop if the entry is fetched
				}
			}
		}

	CleanupStack::PopAndDestroy(&fetchEntryArray); // entryArray.reset & entryArray.destroy;
	return entry;
	}
	
/** Sets the geo value on the copied entry
@param	aCopyEntry on return, sets the geo value on the copied entry
@param	aDataLat The geo latitude value to set
@param	aDataLong The geo longitude value to set
*/
void CTestCalInterimApiCompare::SetGeoValueEntryL(const TDesC& aDataLat, const TDesC& aDataLong)
	{
	TLex lex(aDataLat);
	TReal  geoLat = 0.0;
	User::LeaveIfError(lex.Val(geoLat));
	
	TLex lexlong(aDataLong);
	TReal  geoLong = 0.0;
	User::LeaveIfError(lexlong.Val(geoLong));
		
	CCalGeoValue* geoValue = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValue);

	TRAPD(err, geoValue->SetLatLongL(geoLat, geoLong));
	TESTL(err == KErrNone);
		
	TRAP(err, iCopyEntry->SetGeoValueL(*geoValue));	
	TESTL(err == KErrNone);
		
	CleanupStack::PopAndDestroy(geoValue);	
	}
