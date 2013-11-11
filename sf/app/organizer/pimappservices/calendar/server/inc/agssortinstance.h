// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSSORTINSTANCE_H__
#define __AGSSORTINSTANCE_H__

#include "agmids.h"

class CAgnSimpleEntry;
class TFindInstanceParams;
class TAgnSortInstance

/**
@internalAll
@released
*/
	{
public:
	friend class TAgnDaySortKey;

	TAgnSortInstance(const CAgnSimpleEntry& aInfo);
	
	void SetL(const TTime& aInstanceTimeLocal, const TTime& aUndatedTodoTime);
	
	TTime InstanceDate() const;
	TTime InstanceAlarmDateTime() const;
	
	TAgnInstanceId InstanceIdL() const;
	
	TBool CheckStartAndEndDateOverlap(const TFindInstanceParams& aSearchParams);
	
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS		   						 
	TBool CheckAlarmTimeWithinL(const TFindInstanceParams& aSearchParams) const;
#endif	
	
	const CAgnSimpleEntry& SimpleEntry() const;
	
	inline const TTime& StartTimeLocal() const;
	inline const TTime& EndTimeLocal() const;
	
private:
	void SetTodoTimesL(const TTime& aUndatedTodoTime);
	
public:
	const CAgnSimpleEntry& iSimpleEntry;	// info entry
	TTime iStartTimeLocal;					// instance start date
	TTime iEndTimeLocal;					// instance end date
	};

inline const TTime& TAgnSortInstance::StartTimeLocal() const
	{
	return iStartTimeLocal;
	}

inline const TTime& TAgnSortInstance::EndTimeLocal() const
	{
	return iEndTimeLocal;
	}

#endif
