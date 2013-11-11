// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__E32STD_H__)
#include <e32std.h>		// TSwap, TKey
#endif


//
// TQueuedOperationSwap and TQueuedOperationKey : Classes needed for
// a QuickSort in ExpungeDeletedOperationsL. They sort folders from lower
// to higher ids, and within each folder from high line numbers to low
// line numbers. That way the highest numbers get deleted first, maintaining
// the validity of the other removals.

class TQueuedOperationSwap  : public TSwap
	{
public:
	IMPORT_C TQueuedOperationSwap(CImQueuedList& aList);
	void Swap(TInt aLeft,TInt aRight) const;
private:	
	CImQueuedList& iList;
	};

class TQueuedOperationKey : public TKey
	{
public:
	IMPORT_C TQueuedOperationKey(CImQueuedList& aList);
	TInt Compare(TInt aLeft,TInt aRight) const;
	TAny *At(TInt anIndex) const;
private:
	CImQueuedList& iList;
	};
