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

#ifndef __CALENTRYVIEW_H__
#define __CALENTRYVIEW_H__

#include <badesca.h>
#include <calentry.h>

class MCalProgressCallBack;
class CCalSession;
class CCalEntryViewImpl;

/** View class for accessing calendar entries.

Once a session to a calendar file has been created, a view to the session
can be created.

This view can be used to fetch, store and delete entries.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalEntryView) : public CBase
    {
public:
		
	IMPORT_C static CCalEntryView* NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack);
	IMPORT_C static CCalEntryView* NewL(CCalSession& aSession);

	IMPORT_C ~CCalEntryView();
	
	IMPORT_C void FetchL(const TDesC8& aUid, RPointerArray<CCalEntry>& aCalEntryArray) const;
	IMPORT_C CCalEntry* FetchL(TCalLocalUid aId) const;
	
	IMPORT_C void GetIdsModifiedSinceDateL(const TCalTime& aTime, RArray<TCalLocalUid>& aIds) const;
	
	IMPORT_C void StoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	IMPORT_C void UpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry);
	
	IMPORT_C void DeleteL(const CDesC8Array& aUidList);
	IMPORT_C void DeleteL(const CCalEntry& aCalEntry);
	IMPORT_C void DeleteL(const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter, MCalProgressCallBack& aProgressCallBack);
	IMPORT_C void DeleteL(const RArray<TCalLocalUid>& aIds, TInt& aNumSuccessfulDeleted);

private:
	CCalEntryView();
	void ConstructL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack);
private:
	CCalEntryViewImpl* iImpl;
	};
	
#endif // __CALENTRYVIEW_H__
