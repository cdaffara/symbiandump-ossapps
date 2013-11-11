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

#ifndef __CALINSTANCEITERATORUID_H__
#define __CALINSTANCEITERATORUID_H__

#include "calinstanceiterator.h"
#include "caltime.h"

class CCalInstance;
class CCalSessionImpl;
class CAgnSimpleEntry;
class CCalLiteEntry;

/** Instance Iterator for iterating though all the instances that related to a UID.

@internalComponent
*/
NONSHARABLE_CLASS(CCalInstanceIteratorUid) : public CCalInstanceIterator
	{
public:
	static CCalInstanceIteratorUid* NewL(const CCalInstanceViewImpl& iInstanceViewImpl, const TDesC8& aUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	~CCalInstanceIteratorUid();
	
	CCalInstance* NextL();
	CCalInstance* PreviousL();
	TBool HasMore() const;
	TInt Count() const;
	
private:
	CCalInstanceIteratorUid(const CCalInstanceViewImpl& iInstanceViewImpl);
	void ConstructL(const TDesC8& aUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	
	TCalTime NonRptEntryInstanceTimeL(const CAgnSimpleEntry& aEntry);
	TBool HasMoreL() const;

protected:
	RPointerArray<CCalLiteEntry> 	iCalLiteEntries;
	TCalTime 						iCurrentIndexTime;
	TCalTime						iUndatedTodoTime;
	mutable RArray <TInt>			iEntryWithSameTime;//It stores the indexes of iCalLiteEntries. Those entries have the same instance time as the iCurrentIndexTime
	mutable TInt					iCurrentIndex;//It stores the index of entry in iEntryWithSameTime which has been returned last time when the clinet calls NextL or PrevoiusL. It will be set to -1 if there is no entry in the array has been returned to the user.
	};
	
#endif // __CALINSTANCEITERATORUID_H__
