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
#include "pbapserver.h"
#include "pbappbhandlecache.h"
#include "btaccesshostlog.h"


/*static*/ CPbapPbHandleCache* CPbapPbHandleCache::NewL(CContactDatabase& aDatabase, MPbHandleCacheObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CPbapPbHandleCache* self=new(ELeave) CPbapPbHandleCache(aDatabase, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CPbapPbHandleCache::CPbapPbHandleCache(CContactDatabase& aDatabase, MPbHandleCacheObserver& aObserver)
	: iDatabase(aDatabase), iObserver(aObserver)
	{
	LOG_FUNC
	}


CPbapPbHandleCache::~CPbapPbHandleCache()
	{
	LOG_FUNC
	iHandleList.Close();
	delete iChangeNotifier;
	}	


void CPbapPbHandleCache::ConstructL()
	{
	LOG_FUNC
	iChangeNotifier = CContactChangeNotifier::NewL(iDatabase, this);
	CreateHandleListL();
	}

	
TInt CPbapPbHandleCache::HandleAtL(TUint aIndex) const
	{
	LOG_FUNC
	if(aIndex >= Count())
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicIndexOutOfBoundry));
		User::Leave(KErrOverflow);
		}
	return iHandleList[aIndex].iHandle;
	}
	
TContactItemId CPbapPbHandleCache::ContactIdAtL(TUint aIndex) const
	{
	LOG_FUNC
	if(aIndex >= Count())
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicIndexOutOfBoundry));
		User::Leave(KErrOverflow);
		}
	return iHandleList[aIndex].iContactId;
	}


TInt CPbapPbHandleCache::FindHandle(TInt aHandle) const
	{
	LOG_FUNC
	// the array is ordered on handle so can use fast binary search
	TPbHandleAndId item(aHandle, KNullContactId);
	return iHandleList.FindInOrder(item, TLinearOrder<TPbHandleAndId>(CompareHandles));
	}


TInt CPbapPbHandleCache::FindContactId(TContactItemId aContactId) const
	{
	LOG_FUNC
	// slow sequential search of array required for contact ids
	TPbHandleAndId item(KErrNotFound, aContactId);
	return iHandleList.Find(item, TIdentityRelation<TPbHandleAndId>(MatchContactIds));
	}


/*static*/
TInt CPbapPbHandleCache::CompareHandles(const TPbHandleAndId& aFirst, const TPbHandleAndId& aSecond)
	{
	LOG_STATIC_FUNC
	return (aFirst.iHandle - aSecond.iHandle);
	}

/*static*/	
TBool CPbapPbHandleCache::MatchContactIds(const TPbHandleAndId& aFirst, const TPbHandleAndId& aSecond)
	{
	LOG_STATIC_FUNC
	return (aFirst.iContactId == aSecond.iContactId);
	}

void CPbapPbHandleCache::CreateHandleListL()
	{
	LOG_FUNC
	
	iHandleList.Reset();
	
	// add own card handle item
	TContactItemId ownCardId = iDatabase.OwnCardId();
	TPbHandleAndId ownCardItem(KOwnCardHandle, ownCardId);
	iHandleList.AppendL(ownCardItem);

	// only include contact cards and the own card when building list
	iDatabase.SetDbViewContactType(KUidContactCard);

	// iterate through database building sorted list of handles
	// (there should be a faster way of getting all the ids in the database,
	// calling CContactDatabase::SortedItemsL() without a sort order used to
	// be the quickest method but that has been deprecated)
	TContactIter iter(iDatabase);
	TContactItemId contactId = iter.FirstL();
	TInt handle = 1;
	while (contactId != KNullContactId)
		{
		if (contactId != ownCardId)
			{
			TPbHandleAndId item(handle, contactId); 
			iHandleList.AppendL(item);
			++handle; // increment handle
			}
		contactId = iter.NextL();
		}
	}

	
void CPbapPbHandleCache::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	LOG_FUNC
	switch(aEvent.iType)
		{
		case EContactDbObserverEventContactChanged:
			{
			// notify observer that a contact in the cache has changed
			iObserver.HandleCacheChanged();
			}
			break;
		case EContactDbObserverEventContactAdded:
			{
			// add contact id to cache (ignoring insert errors) and notify observer
			HandleContactAddedL(aEvent.iContactId);
			iObserver.HandleCacheChanged();
			}
			break;
		case EContactDbObserverEventContactDeleted:
			{	
			// remove contact id from cache and notify observer 
			HandleContactDeleted(aEvent.iContactId);
			iObserver.HandleCacheChanged();
			}
			break;
		case EContactDbObserverEventOwnCardChanged:
			{
			// the own card has been modified or another contact
			// has been assigned as the own card
			HandleOwnCardChangedL();
			iObserver.HandleCacheChanged();			
			}
			break;
		case EContactDbObserverEventOwnCardDeleted:
			{	
			// reset own card id back to KNullContactId
			HandleOwnCardDeleted();
			iObserver.HandleCacheChanged();
			}
			break;
		case EContactDbObserverEventUnknownChanges:
			{
			// the database has changed but we do not know which
			// contacts were affected, only choice is to rebuild handles cache  
			#ifdef _DEBUG
				TRAPD(error, CreateHandleListL());
				if(error != KErrNone)
					{
					__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicUnexpectedError));
					User::Leave(error);
					}
			#else
				TRAP_IGNORE(CreateHandleListL());
			#endif // _DEBUG
			iObserver.HandleCacheChanged();
			}
			break;
		default:
			break;
		}
	}


void CPbapPbHandleCache::HandleContactAddedL(TContactItemId aContactId)
	{
	LOG_FUNC
	TPbHandleAndId item(NextAvailableHandleL(), aContactId);
	iHandleList.AppendL(item);
	}

	
void CPbapPbHandleCache::HandleContactDeleted(TContactItemId aContactId)
	{
	LOG_FUNC
	TInt index= FindContactId(aContactId);
	if (index != KErrNotFound)
		{
		iHandleList.Remove(index);
		}
	}


void CPbapPbHandleCache::HandleOwnCardChangedL()
	{
	LOG_FUNC
	TContactItemId ownCardId=iDatabase.OwnCardId();
	TInt ownCardItemIndex = FindHandle(KOwnCardHandle); // always present
	if(ownCardItemIndex == KErrNotFound)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicOwnCardNotFound));
		User::Leave(KErrNotFound);
		}
	TContactItemId currentOwnCardId = ContactIdAtL(ownCardItemIndex);
	if (ownCardId != currentOwnCardId)
		{
		if (currentOwnCardId != KNullContactId)
			{	
			// current own card now just another contact so assign to a new handle
			TPbHandleAndId item(NextAvailableHandleL(), currentOwnCardId);
			iHandleList.Append(item); // ignore the append error
			}

		// the new own card may already exist in the handle list, if so 
		// remove it
		if (ownCardId != KNullContactId)
			{
			TInt index = FindContactId(ownCardId);
			if (index != KErrNotFound)
				{
				iHandleList.Remove(index);
				}
			}
					
		// replace contact id associated with own card handle
		ownCardItemIndex = FindHandle(KOwnCardHandle);
		iHandleList[ownCardItemIndex].iContactId = ownCardId;
		}			
	}
	

void CPbapPbHandleCache::HandleOwnCardDeleted()
	{
	LOG_FUNC
	// reset own card id to null id
	TInt ownCardItemIndex = FindHandle(KOwnCardHandle);
	iHandleList[ownCardItemIndex].iContactId = KNullContactId;	
	}
	
	
TInt CPbapPbHandleCache::NextAvailableHandleL() const
	{
	LOG_FUNC
	TInt lastHandle = HandleAtL(iHandleList.Count()-1);
	return (lastHandle + 1); // just increment handle
	}
