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

#ifndef __CALSESSION_H__
#define __CALSESSION_H__

#include <badesca.h>

#include <calchangecallback.h>
#include <calnotification.h>

class CCalSessionImpl;
class MCalChangeCallBack2;
class CCalSession;
class CCalCalendarInfo;
class MCalFileChangeObserver;

/** A handle to the calendar file.

When the client instantiates a CCalSession, it will connect to the calendar server. 
In turn, the client can use its APIs, for instance to create or open a calendar file.

The agenda data in the file can be accessed as entries. To access the entries, clients should
use class CCalEntryView or CCalInstanceView to process the entry data. However, both view classes 
require the handle to the file which is represented by this class.

A calendar file can be opened on any writable drive. Note that if the file is opened 
on a removable media and that media is removed, operations will leave with KErrNotReady.
This error can also happen when trying to access file attachments on removed media.
When this error occurs, the user should close the CCalSession and re-open it once the media 
has been replaced. 
It is recommended that when using a calendar file on removable media, the user should watch for when 
the media is removed / replaced using the RFs::NotifyChange API.

The session object must not be destroyed until all the objects which where created by referencing it 
have been destroyed.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalSession) : public CBase
    {
public:
	IMPORT_C static CCalSession* NewL();
	IMPORT_C static CCalSession* NewL(CCalSession& aCalSession);
	IMPORT_C ~CCalSession();

	IMPORT_C void CreateCalFileL(const TDesC& aFileName) const;
	IMPORT_C void CreateCalFileL(const TDesC& aFileName, const CCalCalendarInfo& aCalendarInfo) const;
	IMPORT_C void SetCalendarInfoL(const CCalCalendarInfo& aCalendarInfo) const;
	IMPORT_C CCalCalendarInfo* CalendarInfoL() const;

	IMPORT_C void OpenL(const TDesC& aFileName) const;
	IMPORT_C void OpenL(const TDesC& aFileName, CalCommon::TCalFileVersionSupport& aFileSupportStatus) const;
	IMPORT_C const TDesC& DefaultFileNameL() const; 
	IMPORT_C void DeleteCalFileL(const TDesC& aFileName) const;
	IMPORT_C CDesCArray* ListCalFilesL() const;
	IMPORT_C void StartChangeNotification(MCalChangeCallBack2& aCallBack, const CCalChangeNotificationFilter& aFilter);
	IMPORT_C void StartFileChangeNotificationL(MCalFileChangeObserver& aCallBack);
	IMPORT_C void StopChangeNotification();
	IMPORT_C void StopFileChangeNotification();
	IMPORT_C void DisableChangeBroadcast();
	IMPORT_C void EnableChangeBroadcast();
	
	IMPORT_C void EnablePubSubNotificationsL();
	IMPORT_C void DisablePubSubNotificationsL();
	
	IMPORT_C void FileIdL(TCalFileId& aCalFileId) const;
	IMPORT_C void GetFileNameL(TCalPubSubData aPubSubData, TDes& aFileName) const;
	IMPORT_C TBool IsFileNameL(TCalPubSubData aPubSubData, const TDesC& aFileName) const;
	IMPORT_C TBool IsOpenedFileL(TCalPubSubData aPubSubData) const;
	IMPORT_C TCalCollectionId CollectionIdL() const;
	
	// debug only
/**
@publishedPartner
@released
*/
	IMPORT_C void _DebugSetHeapFailL(RAllocator::TAllocFail aFail, TInt aRate);

/**
@publishedPartner
@released
*/
	IMPORT_C TInt _DebugRequestAllocatedCellsL();

	// deprecated
	IMPORT_C void StartChangeNotification(MCalChangeCallBack* aCallBack, MCalChangeCallBack::TChangeEntryType aChangeEntryType,	TBool aIncludeUndatedTodos,	TTime aFilterStartTime,	TTime aFilterEndTime);
	
	IMPORT_C void __dbgClearTzClientCacheL(TBool aRestartCaching);

	IMPORT_C TInt _DebugRequestAllocatedHeapSizeL();
	
public:
	CCalSessionImpl& Impl() const;
	  
private:
	CCalSession();
	void ConstructL();
	void ConstructL(CCalSession& aCalSession);
private:
	CCalSessionImpl* iImpl;
	};
	
#endif // __CALSESSION_H__
