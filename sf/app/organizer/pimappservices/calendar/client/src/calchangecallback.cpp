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

#include <calchangecallback.h>

/** Construction function for the filter. 

@param aChangeEntryType Filters notifications to only notify about entries of this type (event/todo/all).
@param aIncludeUndatedTodos Filters notifications to notify about undated todos.
@param aTimeRange Filters notifications to only notify about entries within this time range. 
This will not include entries starting on the end time of this range, or ending on the start time of this range.
@return Pointer to the newly created filter class.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalChangeNotificationFilter* CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, const CalCommon::TCalTimeRange& aTimeRange)
	{
	return new (ELeave) CCalChangeNotificationFilter(aChangeEntryType, aIncludeUndatedTodos, aTimeRange);
	}

CCalChangeNotificationFilter::CCalChangeNotificationFilter(MCalChangeCallBack2::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, const CalCommon::TCalTimeRange& aTimeRange)
: iChangeEntryType(aChangeEntryType), iTimeRange(aTimeRange), iIncludeUndatedTodos(aIncludeUndatedTodos)
	{
	}

/** Destructor for the filter. Clear away any resources.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalChangeNotificationFilter::~CCalChangeNotificationFilter()
	{
	}

MCalChangeCallBack2::TChangeEntryType CCalChangeNotificationFilter::ChangeEntryType() const
	{
	return iChangeEntryType;
	}

TCalTime CCalChangeNotificationFilter::StartTime() const
	{
	return iTimeRange.StartTime();
	}

TCalTime CCalChangeNotificationFilter::EndTime() const
	{
	return iTimeRange.EndTime();
	}
	
TBool CCalChangeNotificationFilter::IncludeUndatedTodos() const
	{
	return iIncludeUndatedTodos;
	}
