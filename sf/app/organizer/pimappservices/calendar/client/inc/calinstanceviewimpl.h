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

#ifndef __CALINSTANCEVIEWIMPL_H__
#define __CALINSTANCEVIEWIMPL_H__

#include "agmids.h"
#include "agmfilter.h"
#include "calinstanceview.h"
#include "calprogresscallback.h"
class CAgnEntry;
class CAgnEntryModel;
class CAgnSimpleEntry;
class CCalLiteEntry;
class CCalSession;
class CCalSessionImpl;
class CCalInstance;
class MCalProgressCallBack;
class RAgendaServ;

/** Implementation class for the CCalInstanceView.

@internalComponent
*/

NONSHARABLE_CLASS(CCalInstanceViewImpl) : public CBase, public MCalProgressCallBack
    {
public:
	static CCalInstanceViewImpl* NewL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack);
	~CCalInstanceViewImpl();
	static CCalInstanceViewImpl* NewL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack);
	CCalSessionImpl& CalSessionImpl() const;

	void FindInstanceL(RPointerArray<CCalInstance>& aInstanceList, CalCommon::TCalViewFilter aCalViewFilter, const CalCommon::TCalTimeRange& aTimeRange) const;
	void FindInstanceL(RPointerArray<CCalInstance>& aMatchedInstanceList, CalCommon::TCalViewFilter aCalViewFilter, const CalCommon::TCalTimeRange& aTimeRange, const CCalInstanceView::TCalSearchParams& aSearchParams) const;
	
	CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings) const;
	CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime) const;
	CCalInstanceIterator* FindInstanceByUidL(const TDesC8& aUid, const TCalTime& aInstanceTime) const;
	CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalInstanceId& aInstanceId) const;
	CCalInstanceIterator* FindInstanceByUidL(TCalCollectionId aCollectionId, const TDesC8& aUid, const TCalTime& aInstanceTime) const;
	
	CCalInstance* FindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime) const;
	CCalInstance* FindInstanceL(TCalInstanceId& aInstanceId) const;

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
    void FindAlarmedInstanceL(RPointerArray<CCalInstance>& aAlarmedInstanceList,    
    						  TASShdAlarmedInstanceParams aAlarmedInstanceParams) const;
#endif    
	
	void DeleteL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aWhichInstances);
					   
	TCalTime NextInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const;
	TCalTime PreviousInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const;
	
	static void DestroySimpleEntryArray(TAny* aPtr); //Cleanup function used to delete CAgnSimpleEntries.
	static void CleanCalLiteEntries(TAny* aPtr);  //Cleanup function used to delete CCalLiteEntries.
	CArrayFixSeg<TAgnInstance>* FindInstanceIdL(const CCalFindInstanceSettings& aCalFindInstanceSettings) const;
	CCalInstance* CreateInstanceFromLiteEntryL(CCalLiteEntry& aLiteEntry, TAgnInstanceId& aInstanceId, TBool aCheckInstanceTime) const;
	void FetchLiteEntriesFromIdsL(RArray<TAgnInstance>& aEntryIds, RPointerArray<CCalLiteEntry>& aCalLiteEntries, TLinearOrder<CCalLiteEntry>* aOrder) const;

	CCalInstance* CreateInstanceL(CCalLiteEntry& aLiteEntry, const TCalTime& aInstanceTime) const;
	CCalInstance* CreateInstanceL(const TAgnInstance& aInstanceId) const;
	TBool IsValidInstanceL(const CAgnSimpleEntry& aEntry, const TCalTime& aInstanceTime) const;
	void GetFileIdLC(RArray<TInt64>& aFiles)const;
	void GetShortFileIdLC(RArray<TInt>& aFiles)const;
	RAgendaServ& GetServ() const;
	void IncrementsessionReferenceCount();
	void DecrementSessionReferenceCount();
	
	
private:
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	CCalInstanceViewImpl(RAgendaServ& aAgendaServer);
	void ConstructL(MCalProgressCallBack* aProgressCallBack, CCalSession& aSession);
	void ConstructL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack);
	
	void DeleteInstanceL(CAgnEntry& aEntry, const TAgnCalendarTime& aInstanceTime, CalCommon::TRecurrenceRange aWhichInstances);
	void DoDeleteInstanceL(CAgnEntry& aEntry, const TAgnCalendarTime& aInstanceTime, CalCommon::TRecurrenceRange aWhichInstances);
	
	void FindInstancesFromIdsL(CArrayFixSeg<TAgnInstance>* aInstanceIds, RPointerArray<CCalInstance>& aInstanceArray) const;
	static TInt CompareInstanceId(const TAgnInstance& aLeft, const TAgnInstance& aRight);
	CCalSessionImpl* FindSession(TUint8 aShortFileId) const;

	static TBool DefaultSortOnly(const CCalFindInstanceSettings& aSettings); 
	
	CCalInstanceIterator* DoFindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId) const;
	CCalInstance* DoFindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime, TUint8 aShortFileId) const;

private: 

	RAgendaServ&		iAgendaServer;
	TInt 					iCurrentFileToOpen;
	MCalProgressCallBack* 	iCalProgressCallBack;
	RPointerArray<CCalSessionImpl> iCalSessions;//Doesn't own the objects
	};


/**
This Class is used for cleaning up sortentries when a leave happens,
SimpleEntry should be deleted using SimpleEntryAllocator.
Cleanup function DestroySimpleEntryArray() uses an object of this class.
*/
class TCleanSimpleEntryArray
{
public:
	TCleanSimpleEntryArray(RPointerArray<CAgnSimpleEntry>& aSimpleEntryArray, RAgendaServ& aAgendaServ);
	CAgnSimpleEntry* GetSimpleEntryAt(TInt aPos);
	RAgendaServ& GetAgendaServ();
	TInt GetCount() const;
	void CloseArray();
private:
	RPointerArray<CAgnSimpleEntry>& iSimpleEntryArray;
	RAgendaServ&			iAgendaServer;
};	

#endif // __CALINSTANCEVIEW_H__
