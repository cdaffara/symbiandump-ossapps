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

#ifndef __CALINSTANCEVIEW_H__
#define __CALINSTANCEVIEW_H__

#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <calinstanceiterator.h> //this is publishedPartner so should not be included here in the SF version.
#endif

class CCalFindInstanceSettings;
class CCalInstance;
class CCalInstanceIterator;
class CCalInstanceViewImpl;
class CCalSession;
class MCalProgressCallBack;
class TCalTime;
class CCalLiteEntry;
class TAgnInstanceId;
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
struct TASShdAlarmedInstanceParams;
#endif

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
class CCalSortCriteria;
class CCalSortCriterion;
class RWriteStream;
class CAgnSortCriteria;
class TAgnSortCriterion;
#endif

/** View class for accessing instances of calendar entries.

Once a session to a calendar file has been created, a view to the session
can be created.

An instance is a calendar entry, a repeated instance of a calendar entry, 
or an RDate added to an entry.

This view can be used to search for instances and also to delete instances.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalInstanceView) : public CBase
    {	
public:

	/** Parameter struct for storing search data.
	This contains a descriptor to store search text and an enum to describe the text search behaviour.

	@publishedAll
	@released
	*/
	class TCalSearchParams
		{
	public:
		IMPORT_C TCalSearchParams(const TDesC& aText, CalCommon::TCalSearchBehaviour aBehaviour);
		IMPORT_C const TDesC& Text() const;
		IMPORT_C CalCommon::TCalSearchBehaviour Behaviour() const;
	
	private:
		TPtrC iSearchText;
		CalCommon::TCalSearchBehaviour iSearchBehaviour;
		};
		
	IMPORT_C static CCalInstanceView* NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack);
	IMPORT_C static CCalInstanceView* NewL(CCalSession& aSession);
	IMPORT_C static CCalInstanceView* NewL(RPointerArray<CCalSession>& aSessions);
	IMPORT_C static CCalInstanceView* NewL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack& aProgressCallBack);
	IMPORT_C ~CCalInstanceView();
	
	IMPORT_C void FindInstanceL(RPointerArray<CCalInstance>& aInstanceList, 
					   			CalCommon::TCalViewFilter aCalViewFilter, 
					   			const CalCommon::TCalTimeRange& aTimeRange) const;
	
	IMPORT_C void FindInstanceL(RPointerArray<CCalInstance>& aMatchedInstanceList,
								CalCommon::TCalViewFilter aCalViewFilter, 
								const CalCommon::TCalTimeRange& aTimeRange, 
								const TCalSearchParams& aSearchParams) const;
					   
	IMPORT_C CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings) const;
	
	IMPORT_C CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid  aLocalId, const TCalTime& aInstanceTime) const;
	IMPORT_C CCalInstanceIterator* FindInstanceByUidL(const TDesC8& aUid, const TCalTime& aInstanceTime) const;
	
	IMPORT_C CCalInstanceIterator* FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalInstanceId& aInstanceId) const;
	IMPORT_C CCalInstanceIterator* FindInstanceByUidL(TCalCollectionId aCollectionId, const TDesC8& aUid, const TCalTime& aInstanceTime) const;
	
	IMPORT_C CCalInstance* FindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime) const;
	IMPORT_C CCalInstance* FindInstanceL(TCalInstanceId& aInstanceId) const;

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
    IMPORT_C void FindAlarmedInstanceL(RPointerArray<CCalInstance>& aAlarmedInstanceList,    
    								   TASShdAlarmedInstanceParams aAlarmedInstanceParams) const;	
#endif    
	
	IMPORT_C void DeleteL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aWhichInstances);
								
	IMPORT_C TCalTime NextInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const;
	IMPORT_C TCalTime PreviousInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const;
	
private:
	CCalInstanceView();
	void ConstructL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack);
	void ConstructL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack);
	
private: 
	CCalInstanceViewImpl* iImpl;
	};
	
	
/** Class for specifying the search criteria for use in CCalInstanceView::FindInstanceL.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalFindInstanceSettings) : public CBase
	{
public:
	IMPORT_C static CCalFindInstanceSettings* NewL(CalCommon::TCalViewFilter aFilter, const CalCommon::TCalTimeRange& aTimeRange);
	IMPORT_C ~CCalFindInstanceSettings();

	IMPORT_C void SetFilter(CalCommon::TCalViewFilter aFilter);
	IMPORT_C void SetTimeRange(const CalCommon::TCalTimeRange& aTimeRange);
	IMPORT_C void SetTextSearchL(const CCalInstanceView::TCalSearchParams& aSearchParams);
	
	CalCommon::TCalSearchBehaviour Behaviour() const;
	CalCommon::TCalViewFilter Filter() const;
	const CalCommon::TCalTimeRange& TimeRange() const;
	const TDesC& Text() const;

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	IMPORT_C void SetPriorityRange(const CalCommon::TCalPriorityRange& aPriorityRange);
	
	IMPORT_C CCalSortCriteria& SortCriteria();
	IMPORT_C const CCalSortCriteria& SortCriteria() const;
#endif
	
	IMPORT_C void SetFavouriteFilter(
			const CalCommon::TCalFavouriteFilter& aFavouriteFilter);

	const CalCommon::TCalPriorityRange& PriorityRange() const;
	const CalCommon::TCalFavouriteFilter& FavouriteFilter() const;

private:
	CCalFindInstanceSettings(CalCommon::TCalViewFilter aFilter, const CalCommon::TCalTimeRange& aTimeRange);
	void ConstructL();

private:
	CalCommon::TCalTimeRange		iTimeRange;
	CalCommon::TCalViewFilter		iFilter;
	HBufC*							iSearchText;
	CalCommon::TCalSearchBehaviour	iSearchBehaviour;
	CalCommon::TCalPriorityRange	iPriorityRange; 
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	CCalSortCriteria*  				iSortCriteria;
#endif
	CalCommon::TCalFavouriteFilter iFavouriteFilter;
	};

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
/** Class for specifying the sort criteria for use in CCalInstanceView::FindInstanceL.

@publishedAll
@prototype
*/
NONSHARABLE_CLASS(CCalSortCriteria) : public CBase
	{
public:
	IMPORT_C static CCalSortCriteria* NewL();
	IMPORT_C ~CCalSortCriteria();

	IMPORT_C void AppendCriterionL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir);
	IMPORT_C TInt Count() const;
	IMPORT_C const CCalSortCriterion& AtL(TInt aIndex) const;
	IMPORT_C void SetEntryTypeOrderL(const RArray<CCalEntry::TType>& aTypeOrder);
	IMPORT_C const RArray<CCalEntry::TType>& EntryTypeOrder() const;

	const CAgnSortCriteria& SortCriteria() const;
	
private:
	CCalSortCriteria();
	void ConstructL();
	
private:
	CAgnSortCriteria* iSortCriteria;
    RPointerArray<CCalSortCriterion> iSortCriterionList;
	};

NONSHARABLE_CLASS(CCalSortCriterion) : public CBase
	{
public:
	static CCalSortCriterion* NewL(const TAgnSortCriterion& aSortCriterion);
	~CCalSortCriterion();
	
	IMPORT_C CalCommon::TCalSortAttribute SortAttribute() const;
	IMPORT_C CalCommon::TCalSortDirection SortDirection() const;
	
private:
	CCalSortCriterion(const TAgnSortCriterion& aSortCriterion);

private:
	const TAgnSortCriterion& iSortCriterion;
	};
#endif

#endif // __CALINSTANCEVIEW_H__
