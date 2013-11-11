// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calcalendariterator.h"
#include "calcalendariteratorimpl.h"

/** Two phase constructor for the calendar iterator.

This class can be used to find all the calendar file names
known by the calendar. 

If calendar info has been set on the file then the CCalCalendarInfo
will be valid and other properties on the CCalCalendarInfo class
would have been set.

The iterator is a snap shot of the files that exist when the iterator
is created. If a file is created or deleted after the iterator was
created then the iterator will be invlid and should be recreated.
*/
EXPORT_C CCalCalendarIterator* CCalCalendarIterator::NewL(CCalSession& aSession)
    {
    CCalCalendarIterator* self = new(ELeave) CCalCalendarIterator();
    CleanupStack::PushL(self);
    self->ConstructL(aSession);
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarIterator::CCalCalendarIterator()
    {
    }

void CCalCalendarIterator::ConstructL(CCalSession& aSession)
    {
    iImpl = CCalCalendarIteratorImpl::NewL(aSession);
    }

/** Destructor for the iterator.

The iterator must be destroyed befre closing the session.
*/
EXPORT_C CCalCalendarIterator::~CCalCalendarIterator()
    {
    delete iImpl;
    }

/** Sets the iterator index to the first element and returns the calendar info.

If there are no files to iterate through, this API will return NULL. 
*/
EXPORT_C CCalCalendarInfo* CCalCalendarIterator::FirstL()
    {
    return iImpl->FirstL();
    }

/** Increments the iterator index and then returns the calendar info

If there are no more files to iterate through, this API will return NULL. 
*/
EXPORT_C CCalCalendarInfo* CCalCalendarIterator::NextL()
    {
    return iImpl->NextL();
    }

/** Returns the calendar info at the index the iterator is currently pointing to.
*/
EXPORT_C CCalCalendarInfo* CCalCalendarIterator::CurrentL()
    {
    return iImpl->CurrentL();
    }

/** Returns the number of files that there are to iterate through
*/
EXPORT_C TInt CCalCalendarIterator::Count() const
    {
    return iImpl->Count();
    }

