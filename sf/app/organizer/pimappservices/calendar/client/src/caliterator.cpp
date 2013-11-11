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

#include <caliterator.h>
#include "caliteratorimpl.h"
#include <calsession.h>

CCalIter::CCalIter()
	{
	}

void CCalIter::ConstructL(CCalSession& aSession)
	{
	iCalIteratorImpl = new (ELeave) CCalIteratorImpl(aSession.Impl());
	}

/** The destructor frees all resources owned by the iterator, prior to its destruction. 
@publishedAll
@released
@capability None
*/
EXPORT_C CCalIter::~CCalIter()
	{
	delete iCalIteratorImpl;
	}

/** Allocates and constructs an iterator for iterating though all the entries in the file.
@publishedAll
@released
@capability None
@return CCalIter pointer
@param aSession The session to the calendar file.
*/
EXPORT_C CCalIter* CCalIter::NewL(CCalSession& aSession)
	{
	CCalIter* self = new (ELeave) CCalIter();
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	CleanupStack::Pop(self);
	return self;
	}

/** Sets the iterator to the first entry which is a parent (parent and its children 
have the same Uid) and retrieves its Uid. 
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability None
@return a descriptor which contains the Uid of the first entry. 
The return value can be KNullDesC8 if there is no entry in the file. 
*/
EXPORT_C const TDesC8& CCalIter::FirstL()
	{
	return iCalIteratorImpl->FirstL();
	}

/** Moves the entry iterator to the next entry which is a parent (parent and its 
children have the same Uid) and retrieves its Uid.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability None
@return a descriptor which contains the Uid of the first entry. 
The return value can be KNullDesC8 if there is no entry in the file.
*/
EXPORT_C const TDesC8& CCalIter::NextL()
	{
	return iCalIteratorImpl->NextL();
	}

