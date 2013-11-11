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

#ifndef __CALENTRYVIEWIMPL_H__
#define __CALENTRYVIEWIMPL_H__

#include <badesca.h>
#include "calcommon.h"

class CCalEntry;
class CCalSession;
class CCalSessionImpl;
class MCalProgressCallBack;
class RAgendaServ;
class TAgnEntryId;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CCalEntryViewImpl) : public CBase
	{
public:
	static CCalEntryViewImpl* NewL(CCalSession& aCalSession, MCalProgressCallBack* aProgressCallBack);

	~CCalEntryViewImpl();
	
	void FetchL(const TDesC8& aUid, RPointerArray<CCalEntry>& aCalEntryArray) const;
	CCalEntry* FetchL(TCalLocalUid aId) const;

	void GetIdsModifiedSinceDateL(const TCalTime& aTime, RArray<TCalLocalUid>& aIds) const;
	
	void StoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	void UpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	
	void DeleteL(const CDesC8Array& aUidList);
	void DeleteL(const CCalEntry& aCalEntry);
	void DeleteL(const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter, MCalProgressCallBack& aProgressCallBack);
	void DeleteL(const RArray<TCalLocalUid>& aIds, TInt& aNumSuccessfulDeleted);

	void StopAlarmUpdatesLC();
	static void ResetAlarmUpdates(TAny* aPtr);
	
	void StopNotificationLC();
	static void ResetNotification(TAny* aPtr);
	
private:
	CCalEntryViewImpl(CCalSession& aCalSession);
	void ConstructL(MCalProgressCallBack* aProgressCallBack);
	
	void DoStoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	void DoUpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	void DoDeleteByGuidArrayL(const CDesC8Array& aUidList);
	void DoDeleteEntryL(const TAgnEntryId& aEntryId);
	void DoDeleteByLocalUidArrayL(const RArray<TCalLocalUid>& aIds);
	
private:
	CCalSessionImpl&	iCalSessionImpl;	
	RAgendaServ&		iAgendaServer;
	};
	
#endif // __CALENTRYVIEWIMPL_H__
