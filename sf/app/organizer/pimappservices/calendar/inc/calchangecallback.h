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

#ifndef __CALCHANGECALLBACK_H__
#define __CALCHANGECALLBACK_H__

#include <calcommon.h>

class TCalChangeEntry;

class MCalChangeCallBack
/** A call back class for observing changes in the model view.

A client will not be notified of a change if it is the source of
the change.

@publishedAll
@deprecated
*/
	{
public:
	/** Instance view change type
	@publishedAll
	@deprecated
	*/
	enum TChangeType
		{
		/**	Entries are added */
		EChangeAdd,
		/** Entries are deleted */
		EChangeDelete,
		/** Entries are modified */
		EChangeModify,
		/** Undefined entries */
		EChangeUndefined,
		/** Entries cause overflow error */
		EChangeOverflowError
		};

	/** Instance view entry type
	@publishedAll
	@deprecated
	*/
	enum TChangeEntryType
		{
		/** Entry type is todo */
		EChangeEntryTodo,
		/** Entry type is event */
		EChangeEntryEvent,
		/** All entry types */
		EChangeEntryAll
		};
	/** A call back function for change notification
	@param aChangeEntryType Filters notifications to only notify about entries of this type (event/todo/all).

	*/
	virtual void CalChangeNotification(TChangeEntryType aChangeEntryType) = 0;
	};

/** A call back class for observing changes in the model view.

The call back returns an array containing information about at least
one change to the database. The information includes the type of change
and the identity of the item being changed.

A client will not be notified of a change if it is the source of
the change.

@publishedAll
@released
*/
class MCalChangeCallBack2
	{
public:
	/**
	Type of change to the entry.
	*/
	enum TChangeType
		{
		/**	Calendar entry added. */
		EChangeAdd,
		/** Calendar entry deleted. */
		EChangeDelete,
		/** Calendar entry modified. */
		EChangeModify,
		/** Undefined Calendar entry change. */
		EChangeUndefined,
		/** The time zone rules for one or more Calendar entries have changed. */
		EChangeTzRules,
        /** Backup Starts
        * When the backup event is in process, the Calendar file is locked. 
        * Most of operations are not permitted when it happens. However, few
        * operations, such as notification request, enabling broadcast
        * and closing Calendar is still permitted.
        * */
        EBackupStart,
        /** Restore Starts 
        * When the restore event is in process, the Calendar file is locked. 
        * Most of operations are not permitted when it happens. However, few
        * operations, such as notification request, enabling broadcast
        * and closing Calendar is still permitted.
        */
        ERestoreStart,       
        /** Backup Ends 
        * When the backup event is completed, the Calendar file is unlocked. 
        * The application can continue to operate on the current Calendar file.
        */
        EBackupEnd,
        /** Restore Ends
        * When the restore event is completed, the Calendar file is unlocked. 
        * The application can operate on the restored Calendar file after the application
        * is notified with the message. However, if the application has cached any
        * Calendar objects, such as Calendar file ID, Calendar entries, Calendar instances,
        * they are not necessarily synched with the data in the newly restored Calendar file. 
        */
        ERestoreEnd,
        /** The restored Calendar file can not be opened 
        * When the restore event is completed, the newly restored Calendar file can not
        * be opened. It is because either the file version is not supported or the data is corrupted.
        * Apart from closing the session and delete the file, no any other operation is permitted
        * when it happens.
        */
        ERestoredFileCanNotBeOpened
		};

	/**
	Type of entry.
	*/
	enum TChangeEntryType
		{
		/** Entry type is todo. */
		EChangeEntryTodo,
		/** Entry type is event. */
		EChangeEntryEvent,
		/** All entry types. */
		EChangeEntryAll,
		/** Entry type is note */
		EChangeEntryNote
		};

	/**
	A call back function for change notification.
	
	@param aChangeItems List of changes since the last notification.
	*/
	virtual void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems) = 0;
	};

/**
A structure for storing Calendar change notification data.  This contains the
Calendar entry local UID, the type of change and the entry type.

The iChangeEntryType member may be set to MCalChangeCallBack2::EChangeEntryTodo
or MCalChangeCallBack2::EChangeEntryEvent.  MCalChangeCallBack2::EChangeEntryAll
is provided for call back registration only.

If the iChangeType member is MCalChangeCallBack2::EChangeUndefined or
MCalChangeCallBack2::EChangeTzRules, iEntryId and iEntryType are undefined and
should not be used by clients.

When the iChangeType member is MCalChangeCallBack2::EChangeTzRules a change in
time zone rules has been detected and processed by the Calendar server.  The
client should refresh any instance data they own as a result of this change.

@publishedAll
@released
*/
struct TCalChangeEntry
	{
	/** Local UID of the entry. */
	TCalLocalUid iEntryId;
	/** Type of change to the entry. */
	MCalChangeCallBack2::TChangeType iChangeType;
	/** Type of entry. */
	MCalChangeCallBack2::TChangeEntryType iEntryType;
	};

/** Class to encapsulate call back registration parameters.

The type of entries, the date range and whether undated todos are included
can be specified.

@publishedAll
@released
*/
class CCalChangeNotificationFilter
	{
public:
	IMPORT_C static CCalChangeNotificationFilter* NewL(MCalChangeCallBack2::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, const CalCommon::TCalTimeRange& aTimeRange);
	IMPORT_C ~CCalChangeNotificationFilter();

	MCalChangeCallBack2::TChangeEntryType ChangeEntryType() const;
	TCalTime StartTime() const;
	TCalTime EndTime() const;
	TBool IncludeUndatedTodos() const;

private:
	CCalChangeNotificationFilter(MCalChangeCallBack2::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, const CalCommon::TCalTimeRange& aTimeRange);

	MCalChangeCallBack2::TChangeEntryType iChangeEntryType;
	CalCommon::TCalTimeRange iTimeRange;
	TBool iIncludeUndatedTodos;
	};

#endif // __CALCHANGECALLBACK_H__
