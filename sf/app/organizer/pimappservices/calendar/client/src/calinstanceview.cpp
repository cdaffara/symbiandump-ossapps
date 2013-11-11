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

#include "calsessionimpl.h"
#include "calinstanceviewimpl.h"
#include "calentryimpl.h"
#include "agmsimpleentry.h"
#include "calinstanceiteratorall.h"
#include "calinstanceiteratoruid.h"
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
#include <asshdalarm.h>
#endif //SYMBIAN_SKIPPED_CALENDAR_ALARMS
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <asshdalarmcal.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "agmsortcriteria.h"

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
const TInt KDefaultSortCriteriaGranularity = 1;
#endif
// CCalInstanceView::TCalSearchParams //

/** Constructor for the search parameter data structure.

@param aText The text to search for. This string cannot be longer than 32 characters. 
Note that ownership is not taken, so the string passed in should not be deleted before the TCalSearchParams is used.
@param aBehaviour The type of text search to do.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstanceView::TCalSearchParams::TCalSearchParams(const TDesC& aText, CalCommon::TCalSearchBehaviour aBehaviour)
 : iSearchBehaviour(aBehaviour)
	{
	iSearchText.Set(aText);
	}

/** Get the search text.
@return The search text.

@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalInstanceView::TCalSearchParams::Text() const
	{
	return iSearchText;
	}

/** Get the search behaviour.
@return The search behaviour.

@publishedAll
@released
@capability None
*/
EXPORT_C CalCommon::TCalSearchBehaviour CCalInstanceView::TCalSearchParams::Behaviour() const
	{
	return iSearchBehaviour;
	}

// CCalInstanceView //

CCalInstanceView::CCalInstanceView()
	{
	}

void CCalInstanceView::ConstructL(CCalSession& aSession, MCalProgressCallBack* aProgressCallBack)
	{
	iImpl = CCalInstanceViewImpl::NewL(aSession, aProgressCallBack);
	}

/** Allocates and constructs an instance view.

The instance view can not be used until the Completed() function is called on the progress
callback with the value KErrNone.

@param aSession The calendar session from which to create the instance view.
@param aProgressCallBack Reference to a callback object which reports the progress of the operation.
@return Pointer to the newly created instance view.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstanceView* CCalInstanceView::NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack)
	{
	CCalInstanceView* self = new (ELeave) CCalInstanceView();
	CleanupStack::PushL(self);
	self->ConstructL(aSession, &aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}
	
/** Allocates and constructs an instance view.

The instance view is created synchronously.

@param aSession The calendar session from which to create the instance view.
@return Pointer to the newly created instance view.

@publishedAll
@released
@capability None
*/

EXPORT_C CCalInstanceView* CCalInstanceView::NewL(CCalSession& aSession)
	{
	CCalInstanceView* self = new (ELeave) CCalInstanceView();
	CleanupStack::PushL(self);
	self->ConstructL(aSession, NULL);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs an instance view.
 The instance view is created synchronously.
 
@pre Every CCalSession has to be instantiated.
@pre All CCalSession objects share the same handle to the Calendar server. 
@pre A correspondent file has been opened by each CCalSession

@param aSessions An array of CCalSession objects from which to create the instance view.
@param aProgressCallBack Reference to a callback object which reports the progress of the operation.
@return Pointer to the newly created instance view.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstanceView* CCalInstanceView::NewL(RPointerArray<CCalSession>& aSessions)
	{
	CCalInstanceView* self = new (ELeave) CCalInstanceView();
	CleanupStack::PushL(self);
	self->ConstructL(aSessions, NULL);
	CleanupStack::Pop(self);
	return self;
	}

void CCalInstanceView::ConstructL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack* aProgressCallBack)
	{
	iImpl = CCalInstanceViewImpl::NewL(aSessions, aProgressCallBack);
	}

/** Allocates and constructs an instance view.
 The instance view is created asynchronously.
 
@pre Every CCalSession has to be instantiated.
@pre All CCalSession objects share the same handle to the Calendar server. 
@pre A correspondent file has been opened by each CCalSession

@param aSessions An array of CCalSession objects from which to create the instance view.
@return Pointer to the newly created instance view.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstanceView* CCalInstanceView::NewL(RPointerArray<CCalSession>& aSessions, MCalProgressCallBack& aProgressCallBack)
	{
	CCalInstanceView* self = new (ELeave) CCalInstanceView();
	CleanupStack::PushL(self);
	self->ConstructL(aSessions, &aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}
/** Destructor for the view. Clear away any resources.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstanceView::~CCalInstanceView()
	{
	delete iImpl;
	}

/** Gets all instances found in the specified date range, filtered as required.

@param aInstanceList On return, this array contains all instances that were found.
@param aCalViewFilter Filters instances based on the values in TCalViewFilter.
@param aTimeRange Specifies the start and end dates between which instances should be searched for.
Any matching instances on these dates will be included.

@publishedAll
@released
@capability ReadUserData
*/
EXPORT_C void CCalInstanceView::FindInstanceL(RPointerArray<CCalInstance>& aInstanceList,
									CalCommon::TCalViewFilter aCalViewFilter,
									const CalCommon::TCalTimeRange& aTimeRange) const
	{
	iImpl->FindInstanceL(aInstanceList, aCalViewFilter, aTimeRange);
	}

/** Deletes this instance, and other instances of this entry as required.

This function is intended to be used together with CCalInstanceView::FindInstanceL().
FindInstanceL() may be called to get an instance, and it can then be passed to this function to be deleted.

If all instances of an entry are deleted, then the entry itself will be removed.

@param aInstance The instance to be deleted. This function takes ownership of the instance.
@param aWhichInstances Indicates whether only this instance should be deleted, or future/past/all
instances of the same entry should also be deleted.

@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData
*/
EXPORT_C void CCalInstanceView::DeleteL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aWhichInstances)
	{
	iImpl->DeleteL(aInstance, aWhichInstances);
	}

/** Gets all unexcepted instances found in the specified date range, filtered as required,
with the specified text.

@param aMatchedInstanceList On return, this array contains all instances that match the search criteria specified
.
@param aCalViewFilter Filters instances based on the values in TCalViewFilter.
@param aTimeRange Specifies the start and end dates between which instances should be searched for. Any
matching instances on these dates will be included.
@param aSearchParams Contains the text to search for and flags for use with text searching. This is optional and the text can be an empty descriptor.
@leave KErrArgument The search text in aSearchParams contains more than 32 characters

@publishedAll
@released
@capability ReadUserData
*/
EXPORT_C void CCalInstanceView::FindInstanceL(RPointerArray<CCalInstance>& aMatchedInstanceList, CalCommon::TCalViewFilter aCalViewFilter,
												const CalCommon::TCalTimeRange& aTimeRange, const TCalSearchParams& aSearchParams) const
	{
	iImpl->FindInstanceL(aMatchedInstanceList, aCalViewFilter, aTimeRange, aSearchParams);
	}

/** Creates an instance iterator that contains instances which match the given search criteria.

@param aSettings The search settings. These include a time range, an entry type filter and an optional text
search. @see CCalFindInstanceSettings

@return The instance iterator. Ownership is returned.

@pre The instance view has been created and there is an open calendar session.

@publishedPartner
@released

@capability ReadUserData
*/
EXPORT_C CCalInstanceIterator* CCalInstanceView::FindInstanceL(const CCalFindInstanceSettings& aSettings) const
	{
	return iImpl->FindInstanceL(aSettings);
	}

/** Creates an instance iterator that contains instances which match the given search criteria, starting from a specified instance.

The instances in this iterator should be treated as if they were a snapshot from when the iterator was created.
Note that if a view is opened with more than one file and attempts to use this method then a panic "CalInterimAPI" 15 (enum EAmbiguousInstance) will be raised.

@param aSettings The search settings. These include a time range, an entry type filter and an optional text
search. @see CCalFindInstanceSettings
@param aStartingInstance The instance that iterator should start iterating from.

@return The instance iterator. Ownership is returned.

@leave KErrArgument If the starting instance does not match the filter requirements. i.e. It is outside
of the search range or is the wrong entry type.
@leave KErrNotFound If the starting instance does not exist in the database.

@pre The instance view has been created and there is an open calendar session.

@publishedPartner
@released

@capability ReadUserData
@deprecated
*/
EXPORT_C CCalInstanceIterator* CCalInstanceView::FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalLocalUid  aLocalId, const TCalTime& aInstanceTime) const
	{
	return iImpl->FindInstanceL(aSettings, aLocalId, aInstanceTime);
	}

/** Creates an instance iterator that contains instances relating to a UID.

Note that if a view is opened with more than one file and attempts to use this method then a panic "CalInterimAPI" 15 (enum EAmbiguousInstance) will be raised.
 
The iterator is a snapshot of the instances that existed when it was created.  If any changes are made to
the entries after the iterator was created they will not be seen in the results of the iterator. 

@param aUid All instances have.
@param aInstanceTime The instance time that iterator should start iterating from.

@return The instance iterator containing the results. Ownership is returned.

@pre The instance view has been created and there is an open calendar session.

@publishedPartner
@deprecated
@capability ReadUserData
*/
EXPORT_C CCalInstanceIterator* CCalInstanceView::FindInstanceByUidL(const TDesC8& aUid, const TCalTime& aInstanceTime) const
	{
	return iImpl->FindInstanceByUidL(aUid, aInstanceTime);
	}

/** Creates an instance iterator that contains instances relating to a UID.

The iterator is a snapshot of the instances that existed when it was created.  If any changes are made to
the entries after the iterator was created they will not be seen in the results of the iterator. 

@param aUid All instances have.
@param aInstanceTime The instance time that iterator should start iterating from.
@param aCollectionId The Calendar Collection ID which is associated with the instance (belongs to which file) that iterator should start iterating from.

@return The instance iterator containing the results. Ownership is returned.

@pre The instance view has been created and there is an open calendar session.

@publishedPartner
@deprecated
@capability ReadUserData
*/
EXPORT_C CCalInstanceIterator* CCalInstanceView::FindInstanceByUidL(TCalCollectionId aCollectionId, const TDesC8& aUid, const TCalTime& aInstanceTime) const
	{
	return iImpl->FindInstanceByUidL(aCollectionId, aUid, aInstanceTime);
	}

/** Creates an instance iterator that contains instances which match the given search criteria, starting from a specified instance.

The instances in this iterator should be treated as if they were a snapshot from when the iterator was created.

@param aSettings The search settings. These include a time range, an entry type filter and an optional text
search. @see CCalFindInstanceSettings
@param aInstanceId The instance id @see TCalInstanceId

@return The instance iterator. Ownership is returned.

@leave KErrArgument If the starting instance does not match the filter requirements. i.e. It is outside
of the search range or is the wrong entry type.
@leave KErrNotFound If the starting instance does not exist in the database.

@pre The instance view has been created and there is an open calendar session.

@publishedPartner
@released

@capability ReadUserData
*/
EXPORT_C CCalInstanceIterator* CCalInstanceView::FindInstanceL(const CCalFindInstanceSettings& aSettings, TCalInstanceId& aInstanceId) const
	{
	return iImpl->FindInstanceL(aSettings, aInstanceId);
	}

/** Creates an instance iterator starting from a specified instance.
 Note that if a view is opened with more than one file and attempts to use this method then a panic "CalInterimAPI" 15 (enum EAmbiguousInstance) will be raised.
 
@param aInstanceTime The time from which the iterator should start iterating instances.
@return The instance. The ownership is returned.
@pre The instance view has been created and there is an open calendar session.
@deprecated
*/
EXPORT_C CCalInstance* CCalInstanceView::FindInstanceL(TCalLocalUid aLocalId, const TCalTime& aInstanceTime) const
	{
	return iImpl->FindInstanceL(aLocalId, aInstanceTime);
	}

/** Creates an instance iterator based on the TCalInstanceId

@pre The instance view has been created and there is an open calendar session.
@param aInstanceId The instance id.
@return The instance.  
*/
EXPORT_C CCalInstance* CCalInstanceView::FindInstanceL(TCalInstanceId& aInstanceId) const
	{
	return iImpl->FindInstanceL(aInstanceId);
	}

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
/** Gets all instances which have alarms within the time range specified in aAlarmedInstanceParams.

@param aAlarmedInstanceList On return, a list of alarmed Calendar instances 
that meet the parameters of the search given in aAlarmedInstanceParams.
@param aAlarmedInstanceParams The parameters for the search.
@see KSkippedAlarmInstancesPubSubKey
@publishedPartner
@released
*/
EXPORT_C void CCalInstanceView::FindAlarmedInstanceL(RPointerArray<CCalInstance>& aAlarmedInstanceList,    
								   					 TASShdAlarmedInstanceParams aAlarmedInstanceParams) const
	{
	iImpl->FindAlarmedInstanceL(aAlarmedInstanceList, aAlarmedInstanceParams);					   
	}
#endif

/** Gets the next day with an instance on it that matches the filter.

@param aCalViewFilter Filters instances based on the values in TCalViewFilter.
@param aStartDate Specifies the start date from which instances should be searched. Any
matching instances on this date will NOT be included.
@return The next day that contains at least one suitable instance, or Time::NullTime() if none is found.

@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalInstanceView::NextInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const
 	{
 	return iImpl->NextInstanceL(aCalViewFilter, aStartDate);
	}

/** Gets the previous day with an instance on it that matches the filter.

@param aCalViewFilter Filters instances based on the values in TCalViewFilter.
@param aStartDate Specifies the start date before which instances should be searched. Any
matching instances on this date will NOT be included.
@return The previous day that contains at least one suitable instance, or Time::NullTime() if none is found.

@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalInstanceView::PreviousInstanceL(CalCommon::TCalViewFilter aCalViewFilter, const TCalTime& aStartDate) const
 	{
 	return iImpl->PreviousInstanceL(aCalViewFilter, aStartDate);
	}

// CCalFindInstanceSettings //


/** Create a new search settings object for use in CCalInstanceView::FindInstanceL.
@param aFilter Filters instances based on the values in TCalViewFilter.
@param aTimeRange Specifies the start and end dates between which instances should be searched for. Any
matching instances on these dates will be included.
@return A pointer to the new search settings object. Ownership is returned.
@pre None
@post A new search settings object is constructed and initialised using the given parameters.
@capability None
*/
EXPORT_C CCalFindInstanceSettings* CCalFindInstanceSettings::NewL(CalCommon::TCalViewFilter aFilter, const CalCommon::TCalTimeRange& aTimeRange)
	{
	CCalFindInstanceSettings* self = new (ELeave) CCalFindInstanceSettings(aFilter,aTimeRange);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	
	}

CCalFindInstanceSettings::CCalFindInstanceSettings(CalCommon::TCalViewFilter aFilter, const CalCommon::TCalTimeRange& aTimeRange) :
	iTimeRange(aTimeRange), iFilter(aFilter), iPriorityRange(0, KMaxTUint), iFavouriteFilter(0, 0)
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
, iSortCriteria(NULL)
#endif
	{
	}

void CCalFindInstanceSettings::ConstructL()
	{
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	iSortCriteria = CCalSortCriteria::NewL();
#endif
	}

/** Destructor.
@pre The search settings object has been constructed.
@post The object is destroyed.
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalFindInstanceSettings::~CCalFindInstanceSettings()
	{
	delete iSearchText;
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	delete iSortCriteria;
#endif
	}

/** Specifies text to be searched for in CCalInstanceView::FindInstanceL.
@param aSearchParams Contains the text to search for and flags for use with text searching. 
@leave KErrArgument The search text in aSearchParams contains more than 32 characters
@pre None
@post The text search criteria of the CCalFindInstanceSettings have been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalFindInstanceSettings::SetTextSearchL(const CCalInstanceView::TCalSearchParams& aSearchParams)
	{
	delete iSearchText;
	iSearchText = NULL;
	iSearchText = aSearchParams.Text().AllocL();
	iSearchBehaviour = aSearchParams.Behaviour();
	}

/** Filters which types of entries will be found in CCalInstanceView::FindInstanceL.
@param aFilter A selection of flags defining the filter, as defined in CalCommon::TCalViewFilter.
@pre None
@post The filter of this CCalFindInstanceSettings object has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalFindInstanceSettings::SetFilter(CalCommon::TCalViewFilter aFilter)
	{
	iFilter = aFilter;
	}

/** Specifies the time range to be searched in CCalInstanceView::FindInstanceL.
@param aTimeRange The start and end of the time range to be searched. 
@pre None
@post The time range setting of this CCalFindInstanceSettings object has been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalFindInstanceSettings::SetTimeRange(const CalCommon::TCalTimeRange& aTimeRange)
	{
	iTimeRange = aTimeRange;
	}

CalCommon::TCalSearchBehaviour CCalFindInstanceSettings::Behaviour() const
	{
	return iSearchBehaviour;
	}

CalCommon::TCalViewFilter CCalFindInstanceSettings::Filter() const
	{
	return iFilter;
	}

const CalCommon::TCalTimeRange& CCalFindInstanceSettings::TimeRange() const
	{
	return iTimeRange;
	}
	
const TDesC& CCalFindInstanceSettings::Text() const
	{
	if(iSearchText)
		{
		return *iSearchText;	
		}
	else
		{
		return KNullDesC;	
		}
	}

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
/** Specifies the priority range to be searched in CCalInstanceView::FindInstanceL.
@param aPriorityRange The priority range that will be searched.
@pre None
@post The filter of this CCalFindInstanceSettings object has been updated.
@publishedAll
@prototype
@capability None
*/
EXPORT_C void CCalFindInstanceSettings::SetPriorityRange(const CalCommon::TCalPriorityRange& aPriorityRange)
	{
	iPriorityRange = aPriorityRange;
	}
#endif

const CalCommon::TCalPriorityRange& CCalFindInstanceSettings::PriorityRange() const
	{
	return iPriorityRange;
	}

/** Specifies the favourite filter in CCalInstanceView::FindInstanceL.
@param aUserIntValue The value of favourite attribute that will be filtered.
@param aUserIntMask Specifies which bits are validate in the search.
@pre None
@post The favourite filter of this CCalFindInstanceSettings object has been updated.
@publishedPartner
@prototype
@capability None
*/
EXPORT_C void CCalFindInstanceSettings::SetFavouriteFilter(
		const CalCommon::TCalFavouriteFilter& aFavouriteFilter)
	{
	iFavouriteFilter = aFavouriteFilter;
	}
	
const CalCommon::TCalFavouriteFilter& CCalFindInstanceSettings::FavouriteFilter() const
	{
	return iFavouriteFilter;
	}

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT


/** Return the reference of sort criteria which defines the sort order of the instances 
    returned by CCalInstanceView::FindInstanceL. If sort criteria is not defined the 
    default sort order will apply.
@return The reference to sort criteria object    
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C CCalSortCriteria& CCalFindInstanceSettings::SortCriteria()
	{
	return *iSortCriteria;
	}


/** Return the reference of sort criteria which defines the sort order of the instances 
    returned by CCalInstanceView::FindInstanceL. If sort criteria is not defined the 
    default sort order will apply.
@return The reference to sort criteria object    
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C const CCalSortCriteria& CCalFindInstanceSettings::SortCriteria() const
	{
	return *iSortCriteria;
	}


// CCalSortCriteria //

/** Create CCalSortCriteria object.
@return Pointer to new created CCalSortCriteria object 
@pre None
@post A new CCalSortCriteria object is created.
@internalComponent
@prototype
@capability None
*/
EXPORT_C CCalSortCriteria* CCalSortCriteria::NewL()
	{
	CCalSortCriteria* self = new (ELeave) CCalSortCriteria();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/** CCalSortCriteria destructor.
@pre The sort criteria object has been constructed.
@post The object is destroyed.
@internalComponent
@prototype
@capability None
*/
EXPORT_C CCalSortCriteria::~CCalSortCriteria()
	{
	iSortCriterionList.ResetAndDestroy();
	delete iSortCriteria;
	}

CCalSortCriteria::CCalSortCriteria()
	: iSortCriterionList(KDefaultSortCriteriaGranularity)
	{
	}

void CCalSortCriteria::ConstructL()
	{
	iSortCriteria = CAgnSortCriteria::NewL();
	}

const CAgnSortCriteria& CCalSortCriteria::SortCriteria() const
	{
	return *iSortCriteria;
	}

/** Append sort criterion to the end of sort criteria list.
@param aAttribute Sort attribute of the sort criterion
@param aAttribute Sort direction of the sort criterion
@pre None
@post the new sort criterion has been added into the sort criteria list.
@publishedAll
@prototype
@capability None
*/
EXPORT_C void CCalSortCriteria::AppendCriterionL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir)
	{
	//reserve the memory space in case the failure when calling iSortCriterionList.AppendL later
	const TInt KNumOfCriteria(1);
	iSortCriterionList.ReserveL(KNumOfCriteria);
	
	TInt index = iSortCriteria->AppendCriterionL(aAttr, aDir);
	CCalSortCriterion* sortCriterion = CCalSortCriterion::NewL(iSortCriteria->AtL(index));
	iSortCriterionList.Append(sortCriterion);
	}

/** Get the number of sort criteria.
@return The number of sort criteria 
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C TInt CCalSortCriteria::Count() const
	{
	return iSortCriteria->Count();
	}

/** Get the sort criterion at specified index.
@param aIndex The index of wanted sort criterion  
@return Reference to the sort criterion 
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C const CCalSortCriterion& CCalSortCriteria::AtL(TInt aIndex) const
	{
	if((aIndex < 0) || (aIndex >= iSortCriteria->Count()))
		{
		//avoid the panic mightly raised by iSortCriteria[aIndex] 
		User::Leave(KErrArgument);
		}
	
	return *iSortCriterionList[aIndex]; 
	}

/** Set the sort orders which are used to sort instances type.
@param aTypeOrder Reference to an array of CCalEntry::TType  
@pre None
@post The sort order of entry types have been updated.
@publishedAll
@prototype
@capability None
*/
EXPORT_C void CCalSortCriteria::SetEntryTypeOrderL(const RArray<CCalEntry::TType>& aTypeOrder)
	{
	iSortCriteria->SetEntryTypeOrderL(aTypeOrder);
	}

/** Get the sort order which are used to sort entry type.
@return Reference to the array of CCalEntry::TType stored in the sort criteria 
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C const RArray<CCalEntry::TType>& CCalSortCriteria::EntryTypeOrder() const
	{
	return iSortCriteria->EntryTypeOrder();
	}

CCalSortCriterion* CCalSortCriterion::NewL(const TAgnSortCriterion& aSortCriterion)
	{
	CCalSortCriterion* self = new (ELeave) CCalSortCriterion(aSortCriterion);
	return self;
	}

CCalSortCriterion::CCalSortCriterion(const TAgnSortCriterion& aSortCriterion) 
	: iSortCriterion(aSortCriterion)
	{
	}

CCalSortCriterion::~CCalSortCriterion()
	{
	}

/** Get the sort attribute.
@return The sort attribute of the sort criterion 
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C CalCommon::TCalSortAttribute CCalSortCriterion::SortAttribute() const
	{
	return iSortCriterion.iSortAttribute;
	}

/** Get the sort direction.
@return The sort direction of the sort criterion 
@pre None
@post None.
@publishedAll
@prototype
@capability None
*/
EXPORT_C CalCommon::TCalSortDirection CCalSortCriterion::SortDirection() const
	{
	return iSortCriterion.iSortDirection;
	}
#endif
