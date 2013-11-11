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

#include <calsession.h>

#include "calsessionimpl.h"

CCalSession::CCalSession()
	{
	}

void CCalSession::ConstructL()
	{
	iImpl = CCalSessionImpl::NewL();
	}

CCalSessionImpl& CCalSession::Impl() const
	{
	return *iImpl;
	}

/** Allocates and constructs a session to the Calendar server.
@publishedAll
@released
@capability None

@return Pointer to the newly created session.
*/
EXPORT_C CCalSession* CCalSession::NewL()
	{
	CCalSession* self = new (ELeave) CCalSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor for the session. Clear away any resources.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalSession::~CCalSession()
	{
	delete iImpl;
	}

/** Creates a new calendar file.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData

@param aFileName The name of the file to create. This is a combination of drive letter 
and file name, in other words, DriveLetter:FileName, but not a full path. 

It leaves if the file with the name specified already exists.
*/
EXPORT_C void CCalSession::CreateCalFileL(const TDesC&  aFileName) const
	{
	iImpl->CreateCalFileL(aFileName);
	}

/** Creates a new calendar file.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData

@param aFileName The name of the file to create. This is a combination of drive letter 
and file name, in other words, DriveLetter:FileName, but not a full path. 

@param aCalendarInfo The calendar info to set on the file.

It leaves if the file with the name specified already exists.
*/
EXPORT_C void CCalSession::CreateCalFileL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const
    {
    iImpl->CreateCalFileL(aFileName, aCalendarInfo);
    }

/** Set calendar info on the currently open calendar file.
@leave KErrNotReady If there is no calendar file open on this CCalSession.
@publishedAll
@released
@capability WriteUserData

@param aCalendarInfo The calendar info to set on the file.
*/
EXPORT_C void CCalSession::SetCalendarInfoL(const CCalCalendarInfo& aCalendarInfo) const
    {
    iImpl->SetCalendarInfoL(aCalendarInfo);
    }

/** Get calendar info on the currently open calendar file.
@leave KErrNotReady If there is no calendar file open on this CCalSession.
@publishedAll
@released
@capability ReadUserData

@return The calendar file's calendar info.  If none has been set on the
file, CCalCalendarInfo::IsValid will return EFalse. 
*/
EXPORT_C CCalCalendarInfo* CCalSession::CalendarInfoL() const
    {
    return iImpl->CalendarInfoL();
    }

/** Opens an calendar file using the server. 

If another agenda file is open in the same server session, it is closed. 
@publishedAll
@released
@capability ReadUserData

@leave KErrEof If the calendar file is corrupt and cannot be opened because an unexpected end of file was reached.
@leave KErrInUse If there is an undeleted view, entry or instance from the file opened with this calendar session. 
These must all be deleted before a new file can be opened.
@leave KErrNotFound If the file has not been created. Call CCalSession::CreateCalFileL to create the file.
@leave KErrCorrupt If the calendar file is corrupt and cannot be opened.
@leave KErrNotSupported If the file name is not specified in the format DriveLetter:FileName.
@leave KErrNotSupported If the file is not a supported version. The parameter aFileSupportStatus will be set to EUnsupportedFileVersion in this case.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.

@param aFileName The agenda file to open. This is a combination of drive letter 
and file name, in other words, DriveLetter:FileName, but not a full path. It 
leaves if a path is included explicitly. A default file is opened if aFileName is KNullDesC. 
*/
EXPORT_C void CCalSession::OpenL(const TDesC& aFileName) const
	{
	CalCommon::TCalFileVersionSupport status;
	iImpl->OpenL(aFileName,status);
	}

/** Opens a calendar file using the server. 

If another calendar file is open in the same server session, it is closed. 
@param aFileName The agenda file to open. This is a combination of drive letter and file name, 
in other words, DriveLetter:FileName, but not a full path. The default file is opened if aFileName is KNullDesC.
@param aFileSupportStatus On return indicates whether or not the calendar file needs to be converted from an 
older version. File conversion takes place when opening the first instance view or entry view.

@leave KErrEof If the calendar file is corrupt and cannot be opened because an unexpected end of file was reached.
@leave KErrInUse If there is an undeleted view, entry or instance from the file opened with this calendar session. 
These must all be deleted before a new file can be opened.
@leave KErrNotFound If the file has not been created. Call CCalSession::CreateCalFileL to create the file.
@leave KErrCorrupt If the calendar file is corrupt and cannot be opened.
@leave KErrNotSupported If the file name is not specified in the format DriveLetter:FileName.
@leave KErrNotSupported If the file is not a supported version. The parameter aFileSupportStatus will be set to EUnsupportedFileVersion in this case.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
Otherwise one of the system wide error codes.

@pre The file to be opened has been created.
@post The file has been opened and it is now possible to build an entry view or instance view. If the file requires conversion, the
conversion will take place while building the view.
@publishedPartner
@released
@capability ReadUserData
*/
EXPORT_C void CCalSession::OpenL(const TDesC& aFileName, CalCommon::TCalFileVersionSupport& aFileSupportStatus) const
	{
	iImpl->OpenL(aFileName, aFileSupportStatus);
	}

/**
Gets the name of the default calendar file held by the server. 
@publishedAll
@released
@capability None

@return The default filename. This is a combination of a drive letter and a file name, in other 
words, DriveLetter:FileName but not a full path. 
*/
EXPORT_C const TDesC& CCalSession::DefaultFileNameL() const
	{
	return iImpl->DefaultFileNameL();
	}

/** Deletes a calendar file with the supplied filename. 
If the file is opened, it will be closed first

This also deletes all calendar attachment files referenced from this calendar store - these are the
ones moved when the calendar has taken ownership of the file through the CCalAttachment::NewL APIs.
Attachment files stored on a different drive to the calendar file will not be deleted.

@publishedAll
@released

@capability WriteUserData
@param aFileName The combination of drive letter and file name, in other 
words, DriveLetter:FileName, but not a full path 

@leave KErrNotSupported aFileName includes a path explicitly.
@leave KErrNotFound The agenda file has not been found.
@leave KErrBadArgument The length of aFileName is zero.
@leave KErrInUse One of the attachment file handles from this calendar store is still open.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
*/
EXPORT_C void CCalSession::DeleteCalFileL(const TDesC& aFileName) const
	{
	iImpl->DeleteCalFileL(aFileName);
	}

/** Gets a list of all calendar files held by the server.
The caller takes ownership of the returned descriptor array.
@publishedAll
@released
@capability None

@return A list of filenames of the Agenda files held by the server.
This is NULL if no file has been found.

Note that:
- The calendar files can be stored on any drive.
- Since the directories are not seen by clients, the filenames returned are a 
combination of drive letter and file name, in other words DriveLetter:FileName
- The files are ordered alphabetically by drive letter. Files on the same drive 
are ordered by filename, for instance a:agenda, c:agenda, c:calendar, d:agenda, d:calendar.
*/
EXPORT_C CDesCArray* CCalSession::ListCalFilesL() const
	{
	return iImpl->ListCalFilesL();
	}

/** Start notification of any changes to calendar entries. 
These changes can be filtered, and when a change occurs the MCalChangeCallBack callback function is called
with the type of entry which has changed (event or todo).

@param aCallBack The class to notify when a change occurs.
@param aChangeEntryType Filter notifications to only notify on entries of the specified type (event/todo/all).
@param aIncludeUndatedTodos Filter notifications to include undated todos.
@param aFilterStartTime Filter notification to only include entries that are wholly or partly covered by a time range. 
This specifies the start time of the time range.
@param aFilterEndTime Filter notification to only include entries that are wholly or partly covered by a time range. 
This specifies the end time of the time range.
@publishedAll
@deprecated
@capability None
*/
EXPORT_C void CCalSession::StartChangeNotification(MCalChangeCallBack* aCallBack, MCalChangeCallBack::TChangeEntryType aChangeEntryType, TBool aIncludeUndatedTodos, TTime aFilterStartTime, TTime aFilterEndTime)
	{
	TRAP_IGNORE(iImpl->StartChangeNotificationL(aCallBack, aChangeEntryType, aIncludeUndatedTodos, aFilterStartTime, aFilterEndTime));
	}
	
/** Start notification of any changes to calendar entries. 
These changes can be filtered, and when a change occurs the MCalChangeCallBack2 callback function is called
with details about the entry which has changed (TCalLocalUid and entry type) and the type of change (add/delete/modify).

@param aCallBack The class to notify when a change occurs.
@param aFilter Filter notifications to only notify on certain types of entries.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalSession::StartChangeNotification(MCalChangeCallBack2& aChangeObserver, const CCalChangeNotificationFilter& aFilter)
	{
	TRAP_IGNORE(iImpl->StartChangeNotificationL(aChangeObserver, aFilter));
	}

/** Start notification of changes to calendar files. 
When a change occurs, the MCalFileChangeObserver callback function is called
with change types (see MCalFileChangeObserver::TChangeType) about the files.

@param aCallBack The class to notify when a change occurs.
@publishedAll
@prototype
@capability None
*/
EXPORT_C void CCalSession::StartFileChangeNotificationL(MCalFileChangeObserver& aCallBack)
    {
    iImpl->StartFileChangeNotificationL(aCallBack);
    }

/** Stop file change notification from this session.
@publishedAll
@released
@capability None
*/  
EXPORT_C void CCalSession::StopFileChangeNotification()
    {
    iImpl->StopFileChangeNotification();  
    }

/** Stop entry notifications from this session.
@publishedAll
@released
@capability None
*/	
EXPORT_C void CCalSession::StopChangeNotification()
	{
	iImpl->StopChangeNotification();
	}

/** Disable all notifications from changes made within this session. 
Changes made within other sessions will still be notified. This can be done before a bulk operation, 
to prevent a large number of notifications from appearing.
Note that this call does not disable publish and subscribe notifications. That must be done from DisablePubSubNotificationsL.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalSession::DisableChangeBroadcast()
	{
	iImpl->DisableChangeBroadcast();
	}

/** Re-enable all notifications from changes made within this session.
This function has the opposite effect of DisableChangeBroadcast.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalSession::EnableChangeBroadcast()
	{
	iImpl->EnableChangeBroadcast();
	}

/** Gets the file ID of the currently open Calendar file. This is unique to the file. 
@publishedAll
@released
@capability None
@param aCalFileId On return will be the file ID of the currently open Calendar file. The file ID will be set to KNullFileId if the file is not opened.
*/	
EXPORT_C void CCalSession::FileIdL(TCalFileId& aCalFileId) const
	{
	iImpl->FileIdL(aCalFileId);
	}

/** Gets the Collection ID of the currently open Calendar file.
Note that:

 This ID should not be persisted because it is re-generated whenever the session is connected.
 This ID identifies the same Calendar file as the file ID retrieved form CCalSession::FileIdL(TCalFileId& aCalFileId)

@publishedAll
@released
@capability None
@return Return the Collection ID of the currently open Calendar file, 0 if the file has not been opened.
*/	
EXPORT_C TCalCollectionId CCalSession::CollectionIdL() const
	{
	return iImpl->CollectionId();
	}

/** Simulates a heap allocation failure for the server-side heap. Debug only.

The failure occurs on subsequent calls to new or any of the functions which
allocate memory from the server-side heap.

The timing of the allocation failure depends on the type of
allocation failure requested, i.e. on the value of aFail.

The simulation of heap allocation failure is cancelled
if aFail is given the value RAllocator::ENone.

Notes:

1. If the failure type is RAllocator::EFailNext, the next attempt to allocate from
   the server-side heap fails; however, no further failures will occur.
	
2. For failure types RAllocator::EFailNext and RAllocator::ENone, set aRate to 1.

@test
@param aFail An enumeration which indicates how to simulate heap allocation failure.
@param aRate The rate of failure; when aType is RAllocator::EDeterministic, heap allocation fails every aRate attempt.
@pre There is an open calendar session.
@post The calendar server will fail memory allocation at the specified point.
@released
@publishedPartner
*/
#ifdef _DEBUG
EXPORT_C void CCalSession::_DebugSetHeapFailL(RAllocator::TAllocFail aFail, TInt aRate)
	{
	iImpl->_DebugSetHeapFailL(aFail, aRate);
	}
#else
EXPORT_C void CCalSession::_DebugSetHeapFailL(RAllocator::TAllocFail /*aFail*/, TInt /*aRate*/)
	{
	}
#endif
	
/** Gets the total number of cells allocated on the server-side heap. Debug only.
@test
@return The number of cells allocated on the heap.
@pre There is an open calendar session.
@post None
@released
@publishedPartner
*/
EXPORT_C TInt CCalSession::_DebugRequestAllocatedCellsL()
	{
#ifdef _DEBUG
	return iImpl->_DebugRequestAllocatedCellsL();
#else
	return 0;
#endif
	}

/** Enable Publish and Subscribe notifications for changes made by this client only.
@publishedAll
@released
@capability None
*/	
EXPORT_C void CCalSession::EnablePubSubNotificationsL()
	{
	iImpl->EnablePubSubNotificationsL();
	}
	
/** Disable Publish and Subscribe notifications for changes made by this client only.
@publishedAll
@released
@capability None
*/	
EXPORT_C void CCalSession::DisablePubSubNotificationsL()
	{
	iImpl->DisablePubSubNotificationsL();
	}

/** Retrieve a calendar file name from its hash value. 
The hash value comes from the Publish and Subscribe CalInterimAPI data in TCalPubSubData.
@publishedAll
@released
@capability None
@param aPubSubData The publish and subscribe data.
@param aFileName On return will be the full file name.
*/	
EXPORT_C void CCalSession::GetFileNameL(TCalPubSubData aPubSubData, TDes& aFileName) const
	{
	iImpl->GetFileNameL(aPubSubData, aFileName);
	}
	
/** Test if the full file name passed is the one that was changed. 
@publishedAll
@released
@return ETrue if the full file name passed is the one that was changed, EFalse if not.
@capability None
@param aPubSubData The publish and subscribe data.
@param aFileName The full file name to test.
*/	
EXPORT_C TBool CCalSession::IsFileNameL(TCalPubSubData aPubSubData, const TDesC& aFileName) const
	{
	return iImpl->IsFileNameL(aPubSubData, aFileName);
	}
	
/** Test if the publish and subscribe data relates to the currently open calendar file. 
@publishedAll
@released
@return ETrue if the publish and subscribe data relates to the currently open calendar file, EFalse if not.
@capability None
@param aPubSubData The publish and subscribe data.
*/	
EXPORT_C TBool CCalSession::IsOpenedFileL(TCalPubSubData aPubSubData) const
	{
	return iImpl->IsOpenedFileL(aPubSubData);
	}

void CCalSession::ConstructL(CCalSession& aCalSession)
	{
	iImpl = CCalSessionImpl::NewL(aCalSession.Impl());
	}

/** Allocates and constructs a session to the Calendar server

@pre At least one object of CalSession has been created (which means a connection to server has been established).

@publishedAll
@released
@capability None
@param aCalSession A reference to CalSession which is created beforehand.
@return Pointer to the newly created session which shares the handle to the Calendar server with aCalSession.
*/
EXPORT_C CCalSession* CCalSession::NewL(CCalSession& aCalSession)
	{
	CCalSession* self = new (ELeave) CCalSession();
	CleanupStack::PushL(self);
	self->ConstructL(aCalSession);
	CleanupStack::Pop(self);
	return self;
	}
/** Clears timezone information cached at client side.
This API should be used for debug purpose.
@internalComponent
@released
@capability None
@param aRestartCaching Set to ETrue if caching needs to be restarted after clearing current cache.
*/	
#if defined(_DEBUG)

EXPORT_C void CCalSession::__dbgClearTzClientCacheL(TBool aRestartCaching)
	{
	iImpl->__dbgClearTzClientCacheL(aRestartCaching);
	}
	
#else

EXPORT_C void CCalSession::__dbgClearTzClientCacheL(TBool /*aRestartCaching*/)	{}

#endif


EXPORT_C TInt CCalSession::_DebugRequestAllocatedHeapSizeL()
	{
#ifdef _DEBUG
	return iImpl->_DebugRequestAllocatedHeapSizeL();
#else
	return 0;
#endif
	}
