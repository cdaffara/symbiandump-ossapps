// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calcategorymanager.h>

#include "calcategorymanagerimpl.h"

CCalCategoryManager::CCalCategoryManager()
	{
	}

void CCalCategoryManager::ConstructL(CCalSession& aSession)
	{
	iCCalCategoryManagerImpl = new (ELeave) CCalCategoryManagerImpl(aSession);
	}

/** Allocates and constructs a category manager.
@publishedAll
@released
@capability None
@return CCalCategoryManager pointer
@param aSession A file handle to the calendar file.
*/
EXPORT_C CCalCategoryManager* CCalCategoryManager::NewL(CCalSession& aSession)
	{
	CCalCategoryManager* self = new (ELeave) CCalCategoryManager();
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	CleanupStack::Pop(self);
	return self;
	}

/** The destructor frees all resources owned by the entry, prior to its destruction.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalCategoryManager::~CCalCategoryManager()
	{
	delete iCCalCategoryManagerImpl;
	}

/** Gets the number of categories used in the calendar file.

These are the 12 built-in categories defined in CCalCategory::TCalCategoryType, 
and any user-defined categories (of type CCalCategory::ECalExtended) that have 
been added.
@publishedAll
@released
@capability None
@return The number of categories used in the agenda file.
*/
EXPORT_C TInt CCalCategoryManager::CategoryCountL() const
	{
	return iCCalCategoryManagerImpl->CategoryCountL();
	}

/** Allocates and returns a new category based on the one stored at the specified 
index in the server's category list.
@publishedAll
@released
@capability None
@param aIndex The index of the category in the server's category list.
@return A pointer to the indexed category. The caller takes ownership.
*/
EXPORT_C CCalCategory* CCalCategoryManager::CategoryL(TInt aIndex) const
	{
	return iCCalCategoryManagerImpl->CategoryL(aIndex);
	}

	/** Adds a new user-defined category to the server's category list.

The category list is held in the calendar file. There are 12 standard categories 
defined in CAgnCategory::TAgnCategoryType, and any user-defined categories 
that have been added.
@publishedAll
@released
@capability WriteUserData
@param aCategory The name of the new user-defined category to add to the server's 
category list.
@leave KErrAlreadyExists The category is already in the list.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
*/
EXPORT_C void CCalCategoryManager::AddCategoryL(const CCalCategory& aCategory)
	{
	iCCalCategoryManagerImpl->AddCategoryL(aCategory);
	}

/** Gets a list of all entries that belong to the specified category, asynchronously.

Note that the operation is asynchronous. The callback parameter is used to observe the 
progress and completion. After each step, the callback object's Progress() function is called.
The aEntries will be filled in with entries when MCalProgressCallBack::Progress() or 
MCalProgressCallBack::Completed() is called. If the function leaves, neither callback will be called.
This method will lock the calendar file.  Any request made, by any client, before this has completed will leave with KErrLocked.
@publishedAll
@released 
@capability ReadUserData
@param aCategory The category to filter on.
@param aEntries On return, contains all entries that belong to the specified category.
@param aProgressCallBack The progress callback class.
*/
EXPORT_C void CCalCategoryManager::FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries, MCalProgressCallBack& aProgressCallBack)
	{
	iCCalCategoryManagerImpl->FilterCategoryL(aCategory,aEntries,&aProgressCallBack);
	}

/** Gets a list of all entries that belong to the specified category, asynchronously.

@publishedAll
@released 
@capability ReadUserData
@param aCategory The category to filter on.
@param aEntries On return, contains all entries that belong to the specified category.
*/
EXPORT_C void CCalCategoryManager::FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries)
	{
	iCCalCategoryManagerImpl->FilterCategoryL(aCategory,aEntries,NULL);
	}


/** Deletes a user-defined category from the server's category list and updates all entries 
that belong to it, asynchronously.

Note that the operation is asynchronous. The callback parameter is used to observe the 
progress and completion. After each step, the callback object's Progress() function is called.
MCalProgressCallBack::Completed() is get called when the process is finished. If the function 
leaves, neither callback will be called.
This method will lock the calendar file. Any request made, by any client, before this has completed will leave with KErrLocked.
@publishedAll
@released 
@capability WriteUserData
@param aCategory The category to delete from the list.
@param aProgressCallBack The progress callback class.
@leave KErrLocked If a previous asynchronous request has been made which is not complete yet.
*/
EXPORT_C void CCalCategoryManager::DeleteCategoryL(const CCalCategory& aCategory, MCalProgressCallBack& aProgressCallBack)
	{
	iCCalCategoryManagerImpl->DeleteCategoryL(aCategory,aProgressCallBack);
	}
