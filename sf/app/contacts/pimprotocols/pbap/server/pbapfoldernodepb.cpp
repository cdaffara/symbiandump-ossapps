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

#include <cntdb.h>

#include "pbapfoldernodepb.h"
#include "pbaprequest.h"
#include "pbapfolderclient.h"
#include "pbapexporter.h"
#include "pbaperrorreporter.h"
#include "pbapserver.h"
#include "pbapappheader.h"

#include "btaccesshostlog.h"

// constants
_LIT(KFolderPb, "pb");


/*static*/ CFolderNodePb* CFolderNodePb::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	CFolderNodePb* self = new (ELeave) CFolderNodePb(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CFolderNodePb::CFolderNodePb(MVirtualFolderClient& aClient)
: CFolderNode(aClient, KFolderPb())
	{
	LOG_FUNC
	}

		
void CFolderNodePb::ConstructL()
	{
	LOG_FUNC
	iHandleCache = CPbapPbHandleCache::NewL(iClient.ContactDB(), *this);
	}


CFolderNodePb::~CFolderNodePb()
	{
	LOG_FUNC

	delete iHandleCache;
	delete iAsyncExporter;		
	delete iSearchResults;
	iCacheIndexesToExport.Close();
	}
	
	
void CFolderNodePb::CancelGet()
	{
	LOG_FUNC
	// cancel asynchronous searching and sorting of contacts
	iClient.ContactDbViews().CancelSearchAndSortRequest();
	
	// cancel any asynchronous export operations and avoid iAsyncExporter is deleted
	// by itself
	CPbapPbExporter* tmpExporter = iAsyncExporter;
	iAsyncExporter = NULL;
	delete tmpExporter;
	
	// reset for next get request
	Reset();
	}

void CFolderNodePb::GetComplete()
	{
	LOG_FUNC
	
	if (iAsyncExporter)
		{
		delete iAsyncExporter;
		iAsyncExporter = NULL;
		}
	}


 void CFolderNodePb::Reset()
 	{
 	LOG_FUNC
	
 	delete 	iSearchResults;
	iSearchResults = NULL;
 
	delete iAppHeader;
	iAppHeader = NULL;
	
	iCacheIndexesToExport.Reset();	
 	}
 
 				
TInt CFolderNodePb::DoGetItem(TInt aHandle)
	{
	LOG_FUNC
	// check that state has been reset after previous get request
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderPbExportAlreadyExists));
		return KErrAlreadyExists;
		}

	TInt error = KErrNone;
	TInt index = 0;
				
	if (iPhonebookChanged)
		{
		// the Pb phonebook has been modified since the last get request
		// report precondition failed error to obex until next listing
		// request completes (or new Pbap session initiated)
		iClient.ErrorReporter().SendPreconditionFailedError();
		// error already reported above so don't update error
		
		delete iAppHeader;
		iAppHeader = NULL;
		}
	// search for the requested handle in the cache
	else if ((index = iHandleCache->FindHandle(aHandle)) != KErrNotFound)
		{
		// export the contact associated with this handle
		TRAP(error, iAsyncExporter = CPbapPbItemExporter::NewL(iClient, *this, *iHandleCache, index, 1, iAppHeader->VCardVersion(), iAppHeader->Filter()));
		}
	else 
		{
		// the handle does not exist must report not found error to Obex
		iClient.ErrorReporter().SendNotFoundError();
		// error already reported above so don't update error
		
		delete iAppHeader;
		iAppHeader = NULL;
		}
	return error;
	}


TInt CFolderNodePb::DoGetListing()
	{
	LOG_FUNC
	
	TInt error = KErrNone;	
	if (iAppHeader->SearchValue().Length() || iAppHeader->Order() != SymbianPBAP::EIndexed)
		{
		// search or non-indexed sort requested
		TRAP(error, SearchAndSortItemsL());
		}
	else
		{
		// the order is indexed and no search was requested so export listing now
		TRAP(error, ExportIndexSortedListingL());
		}
	
	if (error != KErrNone)
		{
		// error occurred so reset state ready for next get request
		Reset();
		}
	return error;	
	}


/**
 Use database views to search and sort contacts
*/
void CFolderNodePb::SearchAndSortItemsL()
	{
	LOG_FUNC
	if(iSearchResults)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderSearchInProgress));
		User::Leave(KErrAlreadyExists);
		}
	
	// The views only support alphabetical and phonetic sort orders so for index ordered
	// searches use alphabetical order and then resort the results when search complete			
	SymbianPBAP::TOrder order = (iAppHeader->Order() == SymbianPBAP::EIndexed ? SymbianPBAP::EAlphabetical : iAppHeader->Order());

	// create empty array to store results
	iSearchResults = CContactIdArray::NewL();
				
	// start the asynchronous search and sort				
	iClient.ContactDbViews().GetContactIdsMatchingCriteriaL(order, iAppHeader->SearchAttribute(), iAppHeader->SearchValue(), *iSearchResults, *this);
	}


void CFolderNodePb::ExportIndexSortedListingL()
	{
	LOG_FUNC
	// check that state has been reset after previous get request
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderPbExportAlreadyExists));
		User::Leave(KErrAlreadyExists);
		}
	
	if(iCacheIndexesToExport.Count() != 0)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderCachedIndexes));
		User::Leave(KErrAlreadyExists);
		}
	
	// build an array of indexes to cache items (handle/contact Id pairs) to export.
	// Since the cache is ordered on handle the indexes match the ordering of the cache

	// determine the last handle to export and then start the append loop from iListStartOffset
	TInt maxHandle = Min(iAppHeader->MaxListCount() + iAppHeader->ListStartOffset(), HandleCount());
	for (TInt index = iAppHeader->ListStartOffset(); index < maxHandle; ++index)
		{
		iCacheIndexesToExport.AppendL(index);
		}
			
	// export array of cache items as listing
	iAsyncExporter = CPbapPbListingExporter::NewL(iClient, *this, *iHandleCache, iCacheIndexesToExport);
	}


TInt CFolderNodePb::DoGetCount()
	{
	LOG_FUNC
	// check that state has been reset correctly
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderPbExportAlreadyExists));
		return KErrAlreadyExists;
		}
	
	// export the Pb phonebook size. This is equal to the number of handles in the
	// cache (assuming that 0.vcf is always counted even if no owner card is set)
	TRAPD(error, iAsyncExporter = CPbapPbCountExporter::NewL(iClient, *this, HandleCount()));
	return error;
	}


TInt CFolderNodePb::DoGetFolder()
	{
	LOG_FUNC
	// check that state has been reset correctly
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderPbExportAlreadyExists));
		return KErrAlreadyExists;
		}
	
	// setup range of items to export
	TInt count = 0;
	if (iAppHeader->ListStartOffset() < HandleCount())
		{
		// limit the number of items to export to the number of handles available from
		// the offset
		count = Min(iAppHeader->MaxListCount(), HandleCount() - iAppHeader->ListStartOffset());
		}
				
	// export the contacts associated with range of handle cache indexes (these
	// are aleady sorted in increasing handle order)
	TRAPD(error, iAsyncExporter = CPbapPbItemExporter::NewL(iClient, *this, *iHandleCache, iAppHeader->ListStartOffset(), count, iAppHeader->VCardVersion(), iAppHeader->Filter()));
	return error;
	}


void CFolderNodePb::HandleCacheChanged()
	{
	LOG_FUNC
		
	// the contact item a handle refers to has changed. All PullvCard requests will
	// now be blocked until the next listing request (or session close)
	iPhonebookChanged = ETrue;
	// check to see if there is an export in progress
	if (iSearchResults || iAsyncExporter)
	
		{
		// cancel the export and report error according to the error reporting
		// scheme for modified/deleted handles
		CancelGet();
		}
	}


void CFolderNodePb::HandleSearchAndSortComplete(TInt aError)
	{
	LOG_FUNC
	
	if (aError == KErrNone)
		{
		// sorting and searching completed successfully
		TRAP(aError, ExportSearchAndSortResultsL());
		}
	
	if (aError != KErrNone)
		{
		// get request failed report error to obex and reset
		iClient.ErrorReporter().SendServiceUnavailableError();
		Reset();
		}
	}


void CFolderNodePb::ExportSearchAndSortResultsL()
	{
	LOG_FUNC	
	
	// check that state has been reset after previous get request
	if(iAsyncExporter)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderPbExportAlreadyExists));
		User::Leave(KErrAlreadyExists);
		}
	if(iCacheIndexesToExport.Count() != 0)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderCachedIndexes));
		User::Leave(KErrAlreadyExists);
		}

	TInt count = iSearchResults->Count();
	
	// if the order is indexed then to get here means that a search value has been specified
	if (iAppHeader->Order() == SymbianPBAP::EIndexed)
		{
		// sort the search result into index order
		RArray<TInt> sortedIndexes;
		CleanupClosePushL(sortedIndexes);
		for (TInt ii = 0; ii < count; ++ii)
			{
			TContactItemId contactId = (*iSearchResults)[ii];
			TInt index = iHandleCache->FindContactId(contactId);
			if (index != KErrNotFound)
				{
				// insert index in order to match handle ordering of cache
				sortedIndexes.InsertInOrderL(index);
				}
			}
		
		// results array no longer required
		delete iSearchResults;
		iSearchResults = NULL;
		
		// extract range of cache indexes to export
		count = Min(iAppHeader->MaxListCount() + iAppHeader->ListStartOffset(), sortedIndexes.Count());		
		for (TInt jj = iAppHeader->ListStartOffset(); jj < count; ++jj)
			{
			iCacheIndexesToExport.AppendL(sortedIndexes[jj]);
			}
		CleanupStack::PopAndDestroy(); // sortedIndexes	
		}
	// to get here means it's a non-index order sort or search
	else
		{
		// the search results array is sorted in the correct (alphabetic or
		// phonetic) order. Build array of indexes of cache items which match the
		// contact ids 
		TInt maxAndOffset = iAppHeader->MaxListCount() + iAppHeader->ListStartOffset();
		count = Min(count, maxAndOffset);
		
		// decide whether there is a OwnCard
		TBool hasOwnCard = ETrue;
		TInt ownCardItemIndex = iHandleCache->FindHandle(KOwnCardHandle); // always present
		TContactItemId contactId = iHandleCache->ContactIdAtL(ownCardItemIndex);
		if(contactId == KNullContactId)
			{
			hasOwnCard = EFalse;
			}

		// fill the array iCacheIndexsToExport
		for (TInt ii = iAppHeader->ListStartOffset(); ii < count; ++ii)
			{
			TContactItemId contactId = (*iSearchResults)[ii];
			TInt index = iHandleCache->FindContactId(contactId);
			if (index!=KErrNotFound)
				{
				iCacheIndexesToExport.AppendL(index);
				}
			}

		// append an empty own card (0.vcf) when doing sorting on alphabetic or phonetic order
		// search value should be empty when doing sorting
		if(iAppHeader->SearchValue().Size() == 0 && !hasOwnCard)
			{
			// "count < maxAndOffset" means that 
			// "iSearchResults->Count()" < "iAppHeader->MaxListCount() + iAppHeader->ListStartOffset()" 
			// in which case, we need to append an empty OwnCard entry to iCacheIndexesToExport.
			if(count < maxAndOffset)
				{
				iCacheIndexesToExport.AppendL(KOwnCardHandle);
				}
			}
		// results array no longer required
		delete iSearchResults;
		iSearchResults = NULL;			
		}
		
	// export items associated with list of handle cache indexes
	iAsyncExporter = CPbapPbListingExporter::NewL(iClient, *this, *iHandleCache, iCacheIndexesToExport);
	}


 void CFolderNodePb::HandleExportComplete(TInt aError)
 	{
 	LOG_FUNC
 		
 	if (aError)
 		{
 		if(iPhonebookChanged)
 			{
	 		iClient.ErrorReporter().SendPreconditionFailedError();
 			}
 		else
 			{
	 		// error occured during export report service unavailable error to obex 
	 		iClient.ErrorReporter().SendServiceUnavailableError();
 			}
 		}
 	
 	// a successful listing request allows the pb phonebook to be read from again
 	// after it has been modified
 	if (aError == KErrNone && iAppHeader->Operation() == EPullVCardListing)
 		{
 		iPhonebookChanged = EFalse;
 		}
 		
 	// get request complete	so reset state ready for next one
 	Reset();
 	}


 TInt CFolderNodePb::HandleCount()
 	{
 	return iHandleCache->Count();
 	}

