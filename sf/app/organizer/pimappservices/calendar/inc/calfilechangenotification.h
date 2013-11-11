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

#ifndef __CALFILECHANGENOTIFICATION_H__
#define __CALFILECHANGENOTIFICATION_H__

class CAgnFileChangeInfo;
class CCalFileChangeInfo;

/** The mixin/interface class used for recieving calendar file change notifications.

The call back returns an array containing information about at least
one change to the calendar file. The information includes the file name and type of change.

A client will not be notified of a change if it has register with the CCalSession.

@publishedAll
@prototype
*/
class MCalFileChangeObserver
	{
public:

	/** An enum to describe the types of file changes that may occur */
	enum TChangeType
		{
		/** A calendar file has been created */
		ECalendarFileCreated,
		/** A calendar file has been deleted */
		ECalendarFileDeleted,
		/** Calendar info has been set on a calendar file */
		ECalendarInfoCreated,
		/** A calendar file had its calendar info updated */
		ECalendarInfoUpdated,
		/** Calendar info has been deleted on a calendar file */
		ECalendarInfoDeleted
		};

public:

	/** The callback that will recieve 1 or more file change notifications */
	virtual void CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries) = 0;
	};
	
/** This class provides information about the calendar file change

@publishedAll
@prototype
*/
class CCalFileChangeInfo : public CBase
	{
public:
	static CCalFileChangeInfo* NewL(CAgnFileChangeInfo* aAgnFileChangeInfo);
	~CCalFileChangeInfo();
	
	/** Get the file name of the file that has changed */
	IMPORT_C const TDesC& FileNameL() const;
	
	/** Get the type of change that happened to the file */
	IMPORT_C MCalFileChangeObserver::TChangeType ChangeType() const;
	
private:
	CCalFileChangeInfo(CAgnFileChangeInfo* aAgnFileChangeInfo);

private:
    CAgnFileChangeInfo* iImpl;
	};
#endif
