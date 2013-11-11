// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALSORTEDINSTANCEITERATOR_H__
#define __CALSORTEDINSTANCEITERATOR_H__

#include "calinstanceiteratorall.h"

class CCalInstanceViewImpl;
class CCalFindInstanceSettings;
class CCalInstance;
class CCalSortCriteria;

/** Instance Iterator for iterating though all the instances in the calendar.

@internalComponent
*/
NONSHARABLE_CLASS(CCalSortedInstanceIterator) : public CCalInstanceIteratorAll
	{
public:
	static CCalSortedInstanceIterator* NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl);
	static CCalSortedInstanceIterator* NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	~CCalSortedInstanceIterator();

	// From CCalInstanceIterator
	CCalInstance* NextL();
	CCalInstance* PreviousL();
	TBool HasMore() const;
	TInt Count() const;

private:
	CCalSortedInstanceIterator(const CCalInstanceViewImpl& aInstanceViewImpl);
	void ConstructL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId);
	void ConstructL(const CCalFindInstanceSettings& aSettings);
		
	TInt iIteratorId;
	TUint iStartOfIndex;
	CCalSortCriteria* iSortCriteria;
	};

#endif /*__CALSORTEDINSTANCEITERATOR_H__*/
