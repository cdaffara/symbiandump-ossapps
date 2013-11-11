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

#include "pbapcontactdbviews.h"
#include <cntfldst.h>
#include <cntitem.h>
#include <cntfield.h>
#include "pbapserver.h"
#include "btaccesshostlog.h"

_LIT(KNameFieldSeparator, ";");

	

/*static*/ CPbapContactDbViews* CPbapContactDbViews::NewL(CContactDatabase& aDatabase)
	{
	LOG_STATIC_FUNC
	CPbapContactDbViews* self=new(ELeave) CPbapContactDbViews(aDatabase);
	return self;
	}


CPbapContactDbViews::CPbapContactDbViews(CContactDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	LOG_FUNC
	}


CPbapContactDbViews::~CPbapContactDbViews()
	{
	LOG_FUNC
	CloseAllViews();
	}	


/**
Close all currently open views and abort current asynchronous search and sort request. 
This function should be called when the PBAP session ends (gracefully or otherwise)
to save memory while there is no PBAP session open
*/
void CPbapContactDbViews::CloseAllViews()
	{
	LOG_FUNC
	
	CancelAndCleanup();
	if (iNameView)
		{
		iNameView->Close(*this);
		iNameView=NULL;
		}
	if (iPhoneticView)
		{
		iPhoneticView->Close(*this);
		iPhoneticView=NULL;
		}
	iNameViewReady = EFalse;
	iPhoneticViewReady = EFalse;	
	}
	
/**
Get contact name string with fields separated by a semi-colon. The fields are read from the name
view when it is ready, otherwise they are read from the database
*/
HBufC* CPbapContactDbViews::GetContactNameFromIdL(TContactItemId aContactId) const
	{
	LOG_FUNC

	HBufC* name=NULL;
	
	// check to see if the view of the underlying database is ready, i.e. HandleContactViewEvent
	// has been called back after we created iNameView
	if (iNameViewReady)
		{
		// find the contact id in the name view
		TInt index=iNameView->FindL(aContactId);
		User::LeaveIfError(index); //leave with KErrNotFound if contact id not found

		// read concatenated name from name view
		name=iNameView->AllFieldsLC(index, KNameFieldSeparator);
		}
	else
		{
		// create buffer large enough to contain name fields with separators
		// NB We can't use the 'separator' functionality of 
		// TContactTextDefItem because the separator is elided when there is 
		// no text in the field following, which isn't what we want.
		name=HBufC::NewLC(5*KCntMaxTextFieldLength+4);

		CContactItem* item = iDatabase.ReadMinimalContactLC(aContactId);
		const CContactItemFieldSet& fieldSet = item->CardFields();

		AppendField(name, KUidContactFieldFamilyName, fieldSet);
		AppendField(name, KUidContactFieldGivenName, fieldSet);
		AppendField(name, KUidContactFieldAdditionalName, fieldSet);
		AppendField(name, KUidContactFieldPrefixName, fieldSet);
		AppendField(name, KUidContactFieldSuffixName, fieldSet, ETrue); // ETrue- last field: don't add ;

		CleanupStack::PopAndDestroy(item);
		}
	CleanupStack::Pop(name); //ownership passed
	return name;
	}

// Append the field aUid from field set aFieldSet to aBuf. 
// Also append KNameFieldSeparator to that, unless aLast.
void CPbapContactDbViews::AppendField(HBufC* aBuf, TUid aUid, const CContactItemFieldSet& aFieldSet, TBool aLast) const
	{
	LOG_FUNC
	TPtr ptr(aBuf->Des());
	const TInt index = aFieldSet.Find(aUid);
	if ( index != KErrNotFound )
		{
		ptr.Append(aFieldSet[index].TextStorage()->Text());
		}
	if ( !aLast )
		{
		ptr.Append(KNameFieldSeparator);
		}
	}

/**
Finds contacts matching search parameters and returns the ids sorted in the requested order
*/		
void CPbapContactDbViews::GetContactIdsMatchingCriteriaL(SymbianPBAP::TOrder aOrder, SymbianPBAP::TSearchAttribute aSearchAttribute,
						const TDesC& aSearchValue, CContactIdArray& aResults, MPbapContactDbViewsCallback& aCallback)
	{
	LOG_FUNC
	
	// the only supported sort orders are alphabetical and phonetical
	if (aOrder != SymbianPBAP::EAlphabetical && aOrder != SymbianPBAP::EPhonetical)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicInvalidViewSortOrder));
		// leave if sort order not supported
		User::Leave(KErrNotSupported);
		}
		
	// store callback and results array
	iCallback = &aCallback;
	iResults = &aResults;

	// abort previous request and cleanup
	CancelAndCleanup();
	
	// store sort and search parameters
	iOrder = aOrder;
	iSearchAttribute = aSearchAttribute;
	iSearchValue = aSearchValue.AllocL();
	
	if (!iNameView)
		{
		// first request so create name view now
		CreateNameViewL();
		}
		
	if (!iPhoneticView && IsPhoneticViewRequired())
		{
		// phonetic search or sort required so create phonetic name view
		CreatePhoneticViewL();
		}
	
	iOpState = EPendingSearch;
	if (iNameViewReady && (iPhoneticViewReady || !IsPhoneticViewRequired()))
 		{
		// views are ready (or not required depending on sort order) so start search and sort now
		TCallBack callback(SearchAndSortCallback,this);
		iAsyncCallback = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
		iAsyncCallback->CallBack();
		}
	}


void CPbapContactDbViews::CancelSearchAndSortRequest()
	{
	LOG_FUNC
	CancelAndCleanup();
	iCallback=NULL;
	}
	
	
void CPbapContactDbViews::CreateNameViewL()
	{
	LOG_FUNC
	// check name view does not already exist
	if(iNameView)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNameViewExists));
		User::Leave(KErrAlreadyExists);
		}

	RContactViewSortOrder viewSortOrder;
   	CleanupClosePushL(viewSortOrder);
   	viewSortOrder.AppendL(KUidContactFieldFamilyName);
   	viewSortOrder.AppendL(KUidContactFieldGivenName);
   	viewSortOrder.AppendL(KUidContactFieldAdditionalName);
   	viewSortOrder.AppendL(KUidContactFieldPrefixName);
   	viewSortOrder.AppendL(KUidContactFieldSuffixName);

	const TContactViewPreferences prefs = static_cast<TContactViewPreferences>(EContactsOnly | EUnSortedAtEnd);
    iNameView = CContactLocalView::NewL(*this, iDatabase, viewSortOrder, prefs);
    CleanupStack::PopAndDestroy();  // viewSortOrder
	}


void CPbapContactDbViews::CreatePhoneticViewL()
	{
	LOG_FUNC
	// check phonetic view does not already exist
	if(iPhoneticView)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicPhoneticViewExists));
		User::Leave(KErrAlreadyExists);
		}

	RContactViewSortOrder viewSortOrder;
   	CleanupClosePushL(viewSortOrder);
   	viewSortOrder.AppendL(KUidContactFieldFamilyNamePronunciation);
   	viewSortOrder.AppendL(KUidContactFieldGivenNamePronunciation);

	const TContactViewPreferences prefs = static_cast<TContactViewPreferences>(EContactsOnly | EUnSortedAtEnd);
    iPhoneticView = CContactLocalView::NewL(*this, iDatabase, viewSortOrder, prefs);
    CleanupStack::PopAndDestroy();  // viewSortOrder
	}
	
	
TBool CPbapContactDbViews::IsPhoneticViewRequired() const
	{
	LOG_FUNC
	return (iOrder==SymbianPBAP::EPhonetical || iSearchAttribute==SymbianPBAP::ESound);
	}


/*static*/ TInt CPbapContactDbViews::SearchAndSortCallback(TAny* aAny)
	{
	LOG_STATIC_FUNC
	CPbapContactDbViews* self = static_cast<CPbapContactDbViews*>(aAny);
	self->DoSearchAndSortCallback();
	return KErrNone;	
	}

void CPbapContactDbViews::DoSearchAndSortCallback()
	{
	LOG_FUNC
	if (iOpState == EPendingSearch)
		{
		iOpState = ESearching;
		TRAPD(error, DoSearchAndSortL());
		if (error != KErrNone)
			{
			LOG1(_L("Error %d from DoSearchAndSortL"), error);
			// NotifySearchAndSortComplete will also cancel and cleanup search
			NotifySearchAndSortComplete(error);
			}
		}	
	}


void CPbapContactDbViews::DoSearchAndSortL()
	{
	LOG_FUNC
	if (!iSearchValue->Length())
		{
		// no search value so just sort
		DoSortL();
		}
	else
		{
		if (iSearchAttribute==SymbianPBAP::ENumber)
			{
			FindInPhoneFieldsL();
			}
		else if (iSearchAttribute==SymbianPBAP::EName)
			{
			FindInViewL(*iNameView);
			}
		else if (iSearchAttribute==SymbianPBAP::ESound)
			{
			FindInViewL(*iPhoneticView);
			}
		}
	}
	
	
void CPbapContactDbViews::FindInViewL(CContactViewBase& aView)
	{
	LOG_FUNC
	// check search values in correct state
	if(iSearchValueArray || iFindView)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicInvalidViewSearchState));
		User::Leave(KErrAlreadyExists);
		}

	iSearchValueArray = new(ELeave) CPtrCArray(1);
	iSearchValueArray->AppendL(*iSearchValue);

	// start asynchronous search of view
	iFindView = CContactFindView::NewL(iDatabase, aView, *this, iSearchValueArray, CContactViewBase::EPrefixSearch);
	}


void CPbapContactDbViews::FindInPhoneFieldsL()
	{
	LOG_FUNC
	// check search values in correct state
	if(iFindFieldDef || iIdleFinder)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicInvalidFieldSearchState));
		User::Leave(KErrAlreadyExists);
		}

	iFindFieldDef = new(ELeave) CContactItemFieldDef;
	iFindFieldDef->AppendL(KUidContactFieldPhoneNumber);

	// start an asynchronous search of phone fields in database
	iIdleFinder = iDatabase.FindAsyncL(*iSearchValue, iFindFieldDef, this);
	}


void CPbapContactDbViews::IdleFindCallback()
	{
	LOG_FUNC
	TInt idleFindError = iIdleFinder->Error();
	if(idleFindError == KErrNone)
		{
		// this callback will be called for every 16 items searched to allow users to update their search
		// status but we are only concerned when the entire search has completed
		if(iIdleFinder->IsComplete())
			{
			// find completed successfully
			iOpState = ESorting;
			
			TRAPD(error, HandleIdleFindCompleteL());
			if (error != KErrNone)
				{
				LOG1(_L("Error %d from HandleIdleFindCompleteL"), error);
				// NotifySearchAndSortComplete will also cancel and cleanup search
				NotifySearchAndSortComplete(error);
				}	
			}
		}
	else
		{
		LOG1(_L("Idle finder error %d"), idleFindError);
		// error occurred during find notify the observer (and abort find)	
		NotifySearchAndSortComplete(idleFindError);
		}
	}


void CPbapContactDbViews::HandleIdleFindCompleteL()
	{
	LOG_FUNC
	CContactIdArray* contactIds=iIdleFinder->TakeContactIds();
	LOG1(_L("Idle finder returned %d contacts"), contactIds->Count());
	CleanupStack::PushL(contactIds);
	DoSortSearchResultsL(*contactIds);
	CleanupStack::PopAndDestroy(contactIds);	
	}


void CPbapContactDbViews::HandleFindInViewCompleteL()
	{
	LOG_FUNC
	CContactIdArray* contactIds = GetContactIdsInViewL(*iFindView);
	LOG1(_L("Find view contains %d contacts"), contactIds->Count());
	CleanupStack::PushL(contactIds);
	DoSortSearchResultsL(*contactIds);
	CleanupStack::PopAndDestroy(contactIds);
	}


void CPbapContactDbViews::DoSortL()
	{
	LOG_FUNC
	// get all the ids from the view
	CContactIdArray* contactIds=NULL;
	switch (iOrder)
		{
		case SymbianPBAP::EAlphabetical:
			contactIds = GetContactIdsInViewL(*iNameView);
			break;
		case SymbianPBAP::EPhonetical:
			contactIds = GetContactIdsInViewL(*iPhoneticView);
			break;
		default:
			Panic(EPbapServerPanicInvalidViewSortOrder);
		}

	if (contactIds)
		{
		CleanupStack::PushL(contactIds);
		CopyToResultsArrayL(*contactIds);
		CleanupStack::PopAndDestroy(contactIds);
		}

	// the search and sort request is complete notify the observer
	NotifySearchAndSortComplete(KErrNone);
	}


CContactIdArray* CPbapContactDbViews::GetContactIdsInViewL(CContactViewBase &aView)
	{
	LOG_FUNC
	CContactIdArray* contactIds = CContactIdArray::NewLC();
	CArrayFixFlat<TInt>* indexArray = new(ELeave) CArrayFixFlat<TInt>(32);	
	CleanupStack::PushL(indexArray);
	TInt count=aView.CountL();
	for (TInt index=0; index < count; ++index)
		{
		indexArray->AppendL(index);
		}

	aView.GetContactIdsL(*indexArray, *contactIds);
	CleanupStack::PopAndDestroy(indexArray);
	CleanupStack::Pop(contactIds);
	return contactIds; //ownership passed
	}


void CPbapContactDbViews::DoSortSearchResultsL(CContactIdArray& aContactIds)
	{
	LOG_FUNC

	if (iOrder == SymbianPBAP::EAlphabetical)
		{
		if (iSearchAttribute != SymbianPBAP::EName)
			{
			// the find view was sorted on the phonetic name so we need to resort the
			// ids in the find view to match the name view ordering
			ResortIdsInViewOrderL(aContactIds, *iNameView);
			}
		}
	else if (iOrder == SymbianPBAP::EPhonetical)
		{	
		if (iSearchAttribute != SymbianPBAP::ESound)
			{
			// the find view was sorted on the name so we need to resort the
			// ids in the find view to match the phonetic name view ordering
			ResortIdsInViewOrderL(aContactIds, *iPhoneticView);
			}
		}

	CopyToResultsArrayL(aContactIds);

	// the search and sort is complete notify the observer
	NotifySearchAndSortComplete(KErrNone);
	}

/**
aView will be sorted in a specific order and this function re-orders the contact ids in 
aContectIds to be in the same order
*/
void CPbapContactDbViews::ResortIdsInViewOrderL(CContactIdArray& aContactIds, CContactViewBase& aView)
	{
	LOG_FUNC
	// create sorted array of indexes of ids in the view  
	RArray<TInt> indexes;
	CleanupClosePushL(indexes);
	TInt count = aContactIds.Count();
	for (TInt i = 0; i < count; ++i)
		{
		TInt pos = aView.FindL(aContactIds[i]);
		if (pos != KErrNotFound)
			{
			indexes.InsertInOrderL(pos);
			}
		}
		
	// now replace id array contents with ids sorted in same order as view	
	aContactIds.Reset();
	count = indexes.Count();
	for (TInt j = 0; j < count; ++j)
		{
		aContactIds.AddL(aView.AtL(indexes[j]));
		}
	CleanupStack::PopAndDestroy(); //indexes	
	}


void CPbapContactDbViews::CopyToResultsArrayL(const CContactIdArray& aContactIds)
	{
	LOG_FUNC
	const TInt count = aContactIds.Count();
	for (TInt i=0; i < count; ++i)
		{
		iResults->AddL(aContactIds[i]);
		}
	}

/**
this function is part of MContactViewObserver and is called back as a result
of an event occurring on a specific view, in our case iNameView or iPhoneticView
*/
void CPbapContactDbViews::HandleContactViewEvent(const CContactViewBase &aView,
												 const TContactViewEvent &aEvent)
	{
	LOG_FUNC
	TInt error=KErrNone;
	switch(aEvent.iEventType)
		{
		case TContactViewEvent::ESortOrderChanged:
		case TContactViewEvent::EReady:
			{
			if (&aView == iNameView)
				{
				iNameViewReady = ETrue;
				}
			else if (&aView == iPhoneticView)
				{
				iPhoneticViewReady = ETrue;
				}
			else if (&aView == iFindView && iOpState == ESearching)
				{
				iOpState = ESorting;
				TRAP(error, HandleFindInViewCompleteL());
				if (error != KErrNone)
					{
					LOG1(_L("Error %d from HandleFindInViewCompleteL"), error);
					// NotifySearchAndSortComplete will also cancel and cleanup search
					NotifySearchAndSortComplete(error);
					}
				return;	
				}
					
			if (iNameViewReady && (iPhoneticViewReady || !IsPhoneticViewRequired())
				&& iOpState == EPendingSearch)
				{
				// necessary views are ready now so start the find and sort
				iOpState = ESearching;
				TRAP(error, DoSearchAndSortL());
				if (error != KErrNone)
					{
					LOG1(_L("Error %d from DoSearchAndSort"), error);
					// NotifySearchAndSortComplete will also cancel and cleanup search
					NotifySearchAndSortComplete(error);
					}				
				}
			}
			break;
		case TContactViewEvent::EUnavailable:	
		case TContactViewEvent::EServerError:
		case TContactViewEvent::ESortError:	
			{
			if (iOpState != EIdle)
				{
				LOG1(_L("Error %d reported by contact views"), aEvent.iInt);
				// error during find and sort request
				NotifySearchAndSortComplete(aEvent.iInt);
				}
			}
			break;
		default:
			break;
		}
	}
	
	
void CPbapContactDbViews::NotifySearchAndSortComplete(TInt aError)
	{
	LOG_FUNC
	if (iCallback)
		{
		iCallback->HandleSearchAndSortComplete(aError);
		iCallback = NULL;
		}
	CancelAndCleanup();
	}


void CPbapContactDbViews::CancelAndCleanup()
	{
	LOG_FUNC
	iOpState=EIdle;

	if (iFindView)
		{
		iFindView->Close(*this);
		iFindView=NULL;	
		}
	delete iSearchValue;
	iSearchValue=NULL;
	delete iSearchValueArray;
	iSearchValueArray=NULL;
	delete iFindFieldDef;
	iFindFieldDef=NULL;

	if (iIdleFinder)
		{
		iIdleFinder->Cancel();
		delete iIdleFinder;
		iIdleFinder=NULL;
		}	
	if (iAsyncCallback)
		{
		iAsyncCallback->Cancel();
		delete iAsyncCallback;
		iAsyncCallback=NULL;
		}
	}
