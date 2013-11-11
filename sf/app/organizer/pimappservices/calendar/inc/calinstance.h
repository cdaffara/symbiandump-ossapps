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

#ifndef __CALINSTANCE_H__
#define __CALINSTANCE_H__

#include <caltime.h>
#include <calcommon.h>

class CCalEntry;
class CCalInstanceImpl;

/** Instance ID to identify an instance uniquely

@publishedAll
@released
 */
struct TCalInstanceId
	{
	/* Entry Local ID */
	TCalLocalUid		iEntryLocalId;
	/* Instance time */
	TCalTime 	 		iInstanceTime;
	/* Collection Id that the instance belongs to */
	TCalCollectionId 	iCollectionId;	
	};

/** Class representing an instance of a calendar entry.

A calendar entry (CCalEntry) can have any number of instances. 
A non-repeating entry will only have one instance.
Any further instances will appear if the entry has recurrence data.

The time of an instance is the start time of that instance of the entry.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalInstance) : public CBase
	{
public:
	static CCalInstance* NewL(CCalEntry* aEntry, const TCalTime& iTime);
	IMPORT_C ~CCalInstance();
	
	IMPORT_C CCalEntry& Entry() const;
	IMPORT_C TCalTime Time() const;
	IMPORT_C TCalTime StartTimeL() const;
	IMPORT_C TCalTime EndTimeL() const;
	IMPORT_C TCalInstanceId InstanceIdL() const;

private:
	CCalInstance();
	void ConstructL(CCalEntry* aEntry, const TCalTime& aTime);
	
private:
	CCalInstanceImpl* iImpl;
	};

#endif // __CALINSTANCE_H__
