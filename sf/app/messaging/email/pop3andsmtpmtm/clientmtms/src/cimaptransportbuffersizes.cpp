// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimaptransportbuffersizes.h"
#include "MIUT_ERR.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#endif

const TInt KFetchRequestSizeMultiple = 1024;
const TInt KMaxMaxOutstandingFetchResponses = 8;

/**
Factory constructor.
*/
EXPORT_C CImapTransportBufferSizes* CImapTransportBufferSizes::NewL()
	{
	CImapTransportBufferSizes* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

/**
Factory constructor.
*/
EXPORT_C CImapTransportBufferSizes* CImapTransportBufferSizes::NewLC()
	{
	CImapTransportBufferSizes* self = new(ELeave) CImapTransportBufferSizes();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Class constructor
*/
CImapTransportBufferSizes::CImapTransportBufferSizes()
	{
	}

/**
Second phase constructor
*/
void CImapTransportBufferSizes::ConstructL()
	{
	}

/**
Class destructor
*/
CImapTransportBufferSizes::~CImapTransportBufferSizes()
	{
	Reset();
	}

/**
Make copy of an instance of the class.

@return Pointer to new class on the heap. Ownership is transferred to the caller.
*/
EXPORT_C CImapTransportBufferSizes* CImapTransportBufferSizes::CopyL()
	{
	CImapTransportBufferSizes* dest = CopyLC();
	CleanupStack::Pop();
	return dest;
	}

/**
Make new copy of an instance of the class.
On return the copy will be on the cleanup stack.

@return Pointer to new class on the heap. Ownership is transferred to the caller.
*/
EXPORT_C CImapTransportBufferSizes* CImapTransportBufferSizes::CopyLC()
	{
	CImapTransportBufferSizes* dest = NewLC();

	TImapTransportBufferSizes newSizes;
	TInt numSizes = iSizesList.Count();

	for (TInt size = 0; size < numSizes; ++size)
		{
		newSizes.iBearerTypes = iSizesList[size].iBearerTypes;
		newSizes.iFetchRequestSize = iSizesList[size].iFetchRequestSize;
		newSizes.iMaxOutstandingFetchResponses = iSizesList[size].iMaxOutstandingFetchResponses;

		dest->iSizesList.AppendL(newSizes);
		}

	return dest;
	}

/**
Adds a transport buffer sizes entry

The position for the new entry is specified in the parameter list. If the
position is greater than the number of entries already in the list then the
new entry is added at the end of the list. If the position is less than the
number of entries already in the list, then the new entry is inserted at that
position and all subsequent entries moved down by one position. The actual
position where the entry was added is given by the return value.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aPos Position in the list to add the entry.
@param aBearerTypes Bearer types that will use the entry.
@param aFetchRequestSize Fetch request size in bytes. Minimum value 1024. Must
       be a multiple of 1024.
@param aMaxOutstandingFetchResponses Maximum number of outstanding fetch responses
       allowed. Minimum value 1, Maximum value 8.

@return The position in the list where the entry was added, or KErrOverflow
        if the list is full up, or KErrAlreadyExists if there is already an
        entry for one of the bearer types.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapTransportBufferSizes::AddSizesL(TInt aPos, TUint32 aBearerTypes, TInt aFetchRequestSize, TInt aMaxOutstandingFetchResponses)
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapTransportBufferSizes, gPanic(EImcmTransportBufferSizesInvalidPos1));
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmTransportBufferSizesInvalidBearerTypes1));
	__ASSERT_DEBUG(aFetchRequestSize > 0 &&
	               (aFetchRequestSize % KFetchRequestSizeMultiple == 0),
	               gPanic(EImcmTransportBufferSizesInvalidFetchRequestSize));
	__ASSERT_DEBUG(aMaxOutstandingFetchResponses > 0 &&
	               aMaxOutstandingFetchResponses <= KMaxMaxOutstandingFetchResponses,
	               gPanic(EImcmTransportBufferSizesInvalidMaxOutstandingFetchResponses));

	if (aPos < 0 || aPos >= KMaxImapTransportBufferSizes || aBearerTypes == 0 ||
	    aFetchRequestSize <= 0 ||
	    (aFetchRequestSize % KFetchRequestSizeMultiple != 0) ||
	    aMaxOutstandingFetchResponses <= 0 ||
	    aMaxOutstandingFetchResponses > KMaxMaxOutstandingFetchResponses)
		{
		User::Leave(KErrArgument);
		}

	TInt numEntries = iSizesList.Count();

	if (numEntries >= KMaxImapTransportBufferSizes)
		{
		return KErrOverflow;
		}

	for (TInt entry = 0; entry < numEntries; ++entry)
		{
		if ((iSizesList[entry].iBearerTypes & aBearerTypes) != 0)
			{
			return KErrAlreadyExists;
			}
		}

	TImapTransportBufferSizes newEntry;
	newEntry.iBearerTypes = aBearerTypes;
	newEntry.iFetchRequestSize = aFetchRequestSize;
	newEntry.iMaxOutstandingFetchResponses = aMaxOutstandingFetchResponses;

	TInt insertedPos = aPos;

	if (aPos < numEntries)
		{
		iSizesList.InsertL(newEntry, aPos);
		}
	else
		{
		iSizesList.AppendL(newEntry);
		insertedPos = numEntries;
		}

	return insertedPos;
	}

/**
Deletes a transport buffer sizes entry from a given position

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aPos Position in the list to add the entry.

@return ETrue if the entry was deleted, EFalse otherwise

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TBool CImapTransportBufferSizes::DeleteSizesL(TInt aPos)
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapTransportBufferSizes, gPanic(EImcmTransportBufferSizesInvalidPos2));

	if (aPos < 0 || aPos >= KMaxImapTransportBufferSizes)
		{
		User::Leave(KErrArgument);
		}

	if (aPos < iSizesList.Count())
		{
		iSizesList.Remove(aPos);
		return ETrue;
		}
	
	return EFalse;
	}

/**
Deletes bearer types from the transport buffer sizes entries.

Every entry is examined to see if the bearer types associated with that entry
contain all of the passed in bearer types. If there is a match then the bearer
types are removed from the entry. If this results in all the bearer types for
the entry being removed then the entry is deleted and all subsequent entries
are moved up one position.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aBearerTypes Bearer types to remove.
@param aSizesRemoved If the bearer types are removed, this flag indicates if
       the removal resulted in the entry being removed as well.

@return The position of the entry in the list that contained all the bearer types,
        or KErrNotFound if no entry was found.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapTransportBufferSizes::DeleteBearerTypesL(TUint32 aBearerTypes, TBool& aSizesRemoved)
	{
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmTransportBufferSizesInvalidBearerTypes2));

	if (aBearerTypes == 0)
		{
		User::Leave(KErrArgument);
		}

	aSizesRemoved = EFalse;

	TInt numEntries = iSizesList.Count();

	for (TInt entry = 0; entry < numEntries; ++entry)
		{
		if ((iSizesList[entry].iBearerTypes & aBearerTypes) == aBearerTypes)
			{
			iSizesList[entry].iBearerTypes &= ~aBearerTypes;

			if (iSizesList[entry].iBearerTypes == 0)
				{
				iSizesList.Remove(entry);
				aSizesRemoved = ETrue;
				}

			return entry;
			}
		}

	return KErrNotFound;
	}

/**
Gets the transport buffer sizes entry at a given position.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aPos Position of the entry in the list
@param aBearerTypes On successful completion, this holds the bearer types for
       the entry
@param aFetchRequestSize On successful completion, this holds the fetch request
       size in bytes.
@param aMaxOutstandingFetchResponses On successful completion, this holds the
       maximum number of outstanding fetch responses allowed.

@return ETrue if the entry was found, EFalse otherwise

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TBool CImapTransportBufferSizes::GetSizesL(TInt aPos, TUint32& aBearerTypes, TInt& aFetchRequestSize, TInt& aMaxOutstandingFetchResponses) const
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapTransportBufferSizes, gPanic(EImcmTransportBufferSizesInvalidPos3));

	if (aPos < 0 || aPos >= KMaxImapTransportBufferSizes)
		{
		User::Leave(KErrArgument);
		}

	if (aPos < iSizesList.Count())
		{
		aBearerTypes = iSizesList[aPos].iBearerTypes;
		aFetchRequestSize = iSizesList[aPos].iFetchRequestSize;
		aMaxOutstandingFetchResponses = iSizesList[aPos].iMaxOutstandingFetchResponses;
		return ETrue;
		}

	return EFalse;
	}

/**
Gets the transport buffer sizes entry for a bearer type.

An entry is returned only if the bearer types associated with the entry contain
all the bearer types specified in the passed in value.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aBearerTypes The bearer types to search for
@param aFetchRequestSize On successful completion, this holds the fetch request
       size in bytes.
@param aMaxOutstandingFetchResponses On successful completion, this holds the
       maximum number of outstanding fetch responses allowed.

@return The position of the entry in the list, or KErrNotFound if no matching
        bearer type was found.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapTransportBufferSizes::GetSizesL(TUint32 aBearerTypes, TInt& aFetchRequestSize, TInt& aMaxOutstandingFetchResponses) const
	{
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmTransportBufferSizesInvalidBearerTypes3));

	if (aBearerTypes == 0)
		{
		User::Leave(KErrArgument);
		}

	TInt numEntries = iSizesList.Count();

	for (TInt entry = 0; entry < numEntries; ++entry)
		{
		if ((iSizesList[entry].iBearerTypes & aBearerTypes) == aBearerTypes)
			{
			aFetchRequestSize = iSizesList[entry].iFetchRequestSize;
			aMaxOutstandingFetchResponses = iSizesList[entry].iMaxOutstandingFetchResponses;
			return entry;
			}
		}

	return KErrNotFound;
	}

/**
Reset the entries list so that it is empty
*/
EXPORT_C void CImapTransportBufferSizes::Reset()
	{
	iSizesList.Reset();
	}
