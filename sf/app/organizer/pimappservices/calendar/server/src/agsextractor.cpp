// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsextractor.h"

#include "agmsimpleentry.h"
#include "agsentrymodel.h"
#include "agmrptdef.h"
#include "agmutil.h"
#include "agssortinstance.h"
#include "agsindex.h"
#include "agssort.h"

// This represents the number of instances that have been found by a next or previous instance
// call that could, when sorted into order, appear before the search from instance
TInt gPossibleInstancesOutsideRange(0);

// ----------------------------CAgnInstanceExtractor------------------------
CAgnInstanceExtractor::CAgnInstanceExtractor(TAgnIterator* aIterator, CAgnEntryModel& aEntryModel)
	:iEntryModel(aEntryModel), iIterator(aIterator)
	{
	__ASSERT_ALWAYS(aIterator, Panic(EAgmErrNullPointer));
	}

CAgnInstanceExtractor::~CAgnInstanceExtractor()
	{
	delete iIterator;
	}

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
/** Determines whether to continue the Find Instances search.
@param aAlarmedInstanceSearch Whether this is an alarmed instance search
@param aSearchRangeEndLocal The local time of the end of the search range
@internalComponent
*/
TBool CAgnInstanceExtractor::ContinueSearchL(const TBool aAlarmInstanceSearch, const TTime& aSearchRangeEndLocal) const
	{
	TBool ret(ETrue);
	if (aAlarmInstanceSearch)
		{
		ret = !iIterator->AtEnd();
		}
	else
		{
		ret = !iIterator->AtEnd() 
			&& ( (iIterator->CurrentElement().ValidFromTimeLocalL() == Time::NullTTime()) 
					|| iIterator->CurrentElement().ValidFromTimeLocalL() <= aSearchRangeEndLocal);
		}
	return ret;
	}

/** Inserts an instance into an instance list.
@param aAlarmInstanceSearch Whether this is an alarmed instance search
@param aInstances The instance is inserted into this list
@param aInstance The instance to insert
@param aFindInstanceParams Defines the search range
@internalComponent
*/
void CAgnInstanceExtractor::InsertInstanceL(const TBool aAlarmInstanceSearch, 
											CArrayFix<TAgnSortInstance>& aInstances, 
											TAgnSortInstance& aInstance,
											const TFindInstanceParams& aFindInstanceParams) const
	{
	if (aAlarmInstanceSearch)
		{	
		 if (aInstance.CheckAlarmTimeWithinL(aFindInstanceParams))
			 {
			 TAgnAlarmSortKey sortKey;	// Only needed for function call below
			 aInstances.InsertIsqAllowDuplicatesL(aInstance, sortKey);
			 }
		}
	else
		{
		if (aInstance.CheckStartAndEndDateOverlap(aFindInstanceParams))
			{
			aInstances.AppendL(aInstance);
			}
		}
	}
#endif

/** Finds all instances in a time range that match the filter requirements

@param aInstances The instance list to append instances to
@param aSearchParams The filter requirements for the instance search

@internalComponent
*/
void CAgnInstanceExtractor::FindInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
	{		  	
	TTime searchRangeStartLocal(aSearchParams.iRangeStart.LocalL());
	TTime searchRangeEndLocal(aSearchParams.iRangeEnd.LocalL());
	
	TTime startTime(searchRangeStartLocal);	
	
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	TBool alarmInstanceSearch(EFalse);		
	alarmInstanceSearch = aSearchParams.iFilter.IsAlarmedInstanceSearch();
	if (alarmInstanceSearch)
		{
		iIterator->StartL();
		}
	else
		{
		TTime startTime(searchRangeStartLocal);
		iIterator->GotoL(startTime);
		}

	TFindInstanceParams findInstanceParams(aSearchParams);
	
	for (; ContinueSearchL(alarmInstanceSearch, searchRangeEndLocal); iIterator->NextL())
		{
		const CAgnSimpleEntry& KSimpleEntry(iIterator->CurrentElement());
		 
		if (alarmInstanceSearch)
			{
			searchRangeStartLocal = aSearchParams.iRangeStart.LocalL() + KSimpleEntry.AlarmOffset();
			searchRangeEndLocal = aSearchParams.iRangeEnd.LocalL() + KSimpleEntry.AlarmOffset();
			findInstanceParams.iRangeStart.SetLocalL(searchRangeStartLocal);
			findInstanceParams.iRangeEnd.SetLocalL(searchRangeEndLocal);
			}
				
		if(iEntryModel.MatchFullEntryL(KSimpleEntry.EntryId(), aSearchParams))
   			{
   			const CAgnRptDef* KRptDef(KSimpleEntry.RptDef());
   			if (KRptDef)
   				{	
   				TTime instanceTime;   				
   				if (Filter()->RptNextInstanceOnly())
   					{
   					// if this flag is set we only want the next instance after the start time
 					if (KRptDef->NudgeNextInstanceL(searchRangeStartLocal, instanceTime, ETrue) && instanceTime <= searchRangeEndLocal)
   						 {
	   					 TAgnSortInstance instance(KSimpleEntry);
	   					 instance.SetL(instanceTime, UndatedTodoTime());
	   					 InsertInstanceL(alarmInstanceSearch, aInstances, instance, findInstanceParams);
	   					 }
	   				}
   				else
   					{
   					// nudge backwards from the end of the search range
 					TTime fromTime(AgnDateTime::ConvertToLocalTimeL(KSimpleEntry.DurationPlusL(AgnDateTime::ConvertToUtcTimeL(searchRangeEndLocal)) + TTimeIntervalMicroSeconds(1)));
   					while (KRptDef->NudgePreviousUnexceptedInstanceL(fromTime, instanceTime)
 							&& (KSimpleEntry.DurationPlusL(instanceTime) >= searchRangeStartLocal) )
   						{
   						TAgnSortInstance instance(KSimpleEntry);
   						instance.SetL(instanceTime, UndatedTodoTime());
   						InsertInstanceL(alarmInstanceSearch, aInstances, instance, findInstanceParams);
   						fromTime = instanceTime;
   						}
   					}
   				}
   			else
   				{
   				// It is a non-repeating entry
   			    TAgnSortInstance instance(KSimpleEntry);
   			    instance.SetL(KSimpleEntry.EntryTime().LocalL(), UndatedTodoTime());
   			    InsertInstanceL(alarmInstanceSearch, aInstances, instance, findInstanceParams);
   				}
   			}
		}
	
#else	
	for (iIterator->GotoL(startTime); !iIterator->AtEnd() && ( (iIterator->CurrentElement().ValidFromTimeLocalL() == Time::NullTTime()) || iIterator->CurrentElement().ValidFromTimeLocalL() <= searchRangeEndLocal) ; iIterator->NextL())
		{
		const CAgnSimpleEntry& KSimpleEntry(iIterator->CurrentElement());
		if(iEntryModel.MatchFullEntryL(KSimpleEntry.EntryId(), aSearchParams))
   			{
   			const CAgnRptDef* KRptDef(KSimpleEntry.RptDef());
   			if (KRptDef)
   				{	
   				TTime instanceTime;   				
   				if (Filter()->RptNextInstanceOnly())
   					{
   					// if this flag is set we only want the next instance after the start time
 					if (KRptDef->NudgeNextInstanceL(searchRangeStartLocal, instanceTime, ETrue) && instanceTime <= searchRangeEndLocal)
   						 {
   						 TAgnSortInstance instance(KSimpleEntry);
   						 instance.SetL(instanceTime, UndatedTodoTime());
   						 if (instance.CheckStartAndEndDateOverlap(aSearchParams))
   							{
   						  	aInstances.AppendL(instance);
   						  	}	
   						 }
   					}
   				else
   					{
   					// nudge backwards from the end of the search range
 					TTime fromTime(AgnDateTime::ConvertToLocalTimeL(KSimpleEntry.DurationPlusL(AgnDateTime::ConvertToUtcTimeL(searchRangeEndLocal)) + TTimeIntervalMicroSeconds(1)));
   					while (KRptDef->NudgePreviousUnexceptedInstanceL(fromTime, instanceTime)
 							&& (KSimpleEntry.DurationPlusL(instanceTime) >= searchRangeStartLocal) )
   						{
   						TAgnSortInstance instance(KSimpleEntry);
   						instance.SetL(instanceTime, UndatedTodoTime());
   						if (instance.CheckStartAndEndDateOverlap(aSearchParams))
   							{
   						  	aInstances.AppendL(instance);
   						  	}	
   						fromTime = instanceTime;
   						}
   					}
   				}
   			else
   				{
   				// It is a non-repeating entry
   			    TAgnSortInstance instance(KSimpleEntry);
   			    instance.SetL(KSimpleEntry.EntryTime().LocalL(), UndatedTodoTime());
   			    if (instance.CheckStartAndEndDateOverlap(aSearchParams))
   			    	{
   			 	   	aInstances.AppendL(instance);
   			 	   	}	
   				}
   			}
		}
#endif
	}	

		
// ----------------------------TAgnInstanceExtractor----------------------------

/** Instance extractor for all entry types

@internalComponent
*/
TAgnInstanceExtractor::TAgnInstanceExtractor(CAgnSimpleEntryTable& aEntryInfoTable)
	:iSimpleEntryTable(&aEntryInfoTable)
	{
	}

/** Instances are obtained from each index selected by the filter by mean of an extractor.

The instances are returned in the order defined by TAgnDaySortKey

@internalComponent
*/ 
void TAgnInstanceExtractor::FindInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
    {

	RArray<TInt> indexList;
	CleanupClosePushL(indexList);

	// Get the indexes of interest as per filter
	//
	iSimpleEntryTable->GetFilteredIndexListL(indexList, aSearchParams.iFilter);
	
   	// Extract instances from each index (both floating and fixed time modes).
   	// Instances are extracted from each index of interest using a "extractor".
   	// The extractor is initialized with the particular index when "GetExtractor" is called.
   	//
   	const TInt KIndexCount = indexList.Count();
	for (TInt i = 0; i < KIndexCount; ++i)
   		{
		iSimpleEntryTable->GetExtractor(indexList[i], MAgnCalendarTimeMode::EFloating, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->FindInstancesL(aInstances, aSearchParams);
		iSimpleEntryTable->GetExtractor(indexList[i], MAgnCalendarTimeMode::EFixedUtc, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->FindInstancesL(aInstances, aSearchParams);
   		}

	CleanupStack::PopAndDestroy(&indexList); // Close()
	}

void TAgnInstanceExtractor::NextPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
	{
	RArray<TInt> indexList;
	CleanupClosePushL(indexList);
	
	// Get the indices of interest as per filter
	iSimpleEntryTable->GetFilteredIndexListL(indexList, aSearchParams.iFilter);
	
	gPossibleInstancesOutsideRange = 0;

	const TInt KIndexCount = indexList.Count();
	for (TInt ii = 0; ii < KIndexCount; ++ii)
		{
		iSimpleEntryTable->GetExtractor(indexList[ii], MAgnCalendarTimeMode::EFloating, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->NextInstancesL(aInstances, aSearchParams);
		iSimpleEntryTable->GetExtractor(indexList[ii], MAgnCalendarTimeMode::EFixedUtc, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->NextInstancesL(aInstances, aSearchParams);
		}
	CleanupStack::PopAndDestroy(&indexList); // Close()
	}

void TAgnInstanceExtractor::PreviousPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
	{
	RArray<TInt> indexList;
	CleanupClosePushL(indexList);
	
	// Get the indices of interest as per filter
	iSimpleEntryTable->GetFilteredIndexListL(indexList, aSearchParams.iFilter);
	
	gPossibleInstancesOutsideRange = 0;
	
   	const TInt KIndexCount = indexList.Count();
	for (TInt ii = 0 ; ii < KIndexCount ; ++ii) 
		{
		iSimpleEntryTable->GetExtractor(indexList[ii], MAgnCalendarTimeMode::EFloating, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->PreviousInstancesL(aInstances, aSearchParams);
		iSimpleEntryTable->GetExtractor(indexList[ii], MAgnCalendarTimeMode::EFixedUtc, aSearchParams.iFilter, aSearchParams.iUndatedTodoTimeLocal)->PreviousInstancesL(aInstances, aSearchParams);
		}
	CleanupStack::PopAndDestroy(&indexList); // Close()
	}
void CAgnInstanceExtractor::NextInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
	{
	const TTime KSearchFromTimeLocal(aSearchParams.iInstance.iId.Date().IsSet() ? aSearchParams.iInstance.iId.Date().LocalL() : aSearchParams.iUndatedTodoTimeLocal);
	const TTime KSearchRangeEndLocal(aSearchParams.iRangeEnd.LocalL());
	
	TTime searchFromTimeLocal(KSearchFromTimeLocal);
	TTime lastMatchedTimeLocal(KSearchFromTimeLocal);
	
	TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), UndatedTodoTime());
	
	// This time is used so that we do not add instances to the list that are after
	// the Nth one that has been found because we know that we will not need them
	TTime NthInstanceTimeLocal(AgnDateTime::MaxDate());

	TInt numInstancesFound(0);

	for (iIterator->GotoL(searchFromTimeLocal) ; !iIterator->AtEnd() && (iIterator->CurrentElement().ValidFromTimeLocalL() <= KSearchRangeEndLocal) && (iIterator->CurrentElement().ValidFromTimeLocalL() <= NthInstanceTimeLocal) ; iIterator->NextL())
		{
		if ( aInstances.Count() > (aSearchParams.iNumInstances + gPossibleInstancesOutsideRange) )
			{
			// We don't need instances after aSearchParams.iNumInstances
			NthInstanceTimeLocal = aInstances.At(aSearchParams.iNumInstances + gPossibleInstancesOutsideRange).InstanceDate();
			}

		const CAgnSimpleEntry& KSimpleEntry(iIterator->CurrentElement());
		
		if (iEntryModel.MatchFullEntryL(KSimpleEntry.EntryId(), aSearchParams))
		    {
		    // find out where this instance is in the list
		        	    		    
		    if (KSimpleEntry.RptDef())
		    	{
    			// nudge backwards to find instances that over lap into the start of the range
    			// add a microsecond to the search from time so that we find instances that start exactly on the start range
		    	TTime fromTime = AgnDateTime::ConvertToLocalTimeL(AgnDateTime::ConvertToUtcTimeL(KSearchFromTimeLocal) + TTimeIntervalMicroSeconds(1));
    			TTime instanceTime;
    			
    			while (KSimpleEntry.RptDef()->NudgePreviousUnexceptedInstanceL(fromTime, instanceTime))
    				{
    				TAgnSortInstance instance(KSimpleEntry);
    				instance.SetL(instanceTime, UndatedTodoTime());
    				
    	    		if (instance.CheckStartAndEndDateOverlap(aSearchParams))
    	    			{
    	    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
    	    			
    	    			// it is possible that this instance appears before
		    			// the starting instance when sorted into order
    	    			++gPossibleInstancesOutsideRange;
    	    			
    	    			if (!aSearchParams.iInstance.iId.IsNullId())
	    			    	{
	    			    	// We are searching from a particular instance
	    			    	// so one nudge backwards is good enough to get the search from instance
	    			    	break;
	    			    	}
    	    			}
    	    		else
    	    			{
    	    			// There are no more instances in the repeat that overlap into the start of the search range
    	    			break;
    	    			}
    	    		
    				fromTime = instanceTime;
    				}
    			
    			// Try to get the next n number of instances from the repeat rule
    			numInstancesFound = 0;
    			fromTime = KSearchFromTimeLocal;
    			while ( KSimpleEntry.RptDef()->NudgeNextInstanceL(fromTime, instanceTime, ETrue)
    					&& aSearchParams.iNumInstances >= ++numInstancesFound
    					&& KSimpleEntry.DurationMinusL(instanceTime) <= KSearchRangeEndLocal
    					&& KSimpleEntry.DurationMinusL(instanceTime) <= NthInstanceTimeLocal )
    				{
    				TAgnSortInstance instance(KSimpleEntry);
    				instance.SetL(instanceTime, UndatedTodoTime());
    				
    	    		if (instance.CheckStartAndEndDateOverlap(aSearchParams))
    	    			{
    	    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
    	    			}
    	    		
    				fromTime = instanceTime;
    				}
		    	}
		    else
		    	{
		    	// It is a non-repeating entry
		    	
				TAgnSortInstance instance(KSimpleEntry);
				instance.SetL(KSimpleEntry.EntryTime().LocalL(), UndatedTodoTime());
				
			    if ( lastMatchedTimeLocal > KSearchFromTimeLocal )
					{
					// We now have any instances that are the same as the search from time
					
					if (++numInstancesFound > aSearchParams.iNumInstances)
						{
						// We now have the number of requested instances
						
						if (iIterator->CurrentKey() > lastMatchedTimeLocal)
							{
							// Break when all the instances that are at
							// the same time as the last instance have been found  
							break;
							}
						}
					}
			    
				// Add the instance to the list
			    if (instance.CheckStartAndEndDateOverlap(aSearchParams))
			    	{
	    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
	    			lastMatchedTimeLocal = instance.InstanceDate();
	    			
	    			if (lastMatchedTimeLocal == KSearchFromTimeLocal)
		    			{
		    			// it is possible that this instance appears before
		    			// the starting instance when sorted into order
		    			++gPossibleInstancesOutsideRange;
		    			}
			    	}
		    	}
		    }
		}
	}

/** Finds the previous n number of instance that match the filter requirements

The filter includes the time to search from and the search time range.

@param aInstances The instance list to append instances to
@param aSearchParams The filter requirements for the instance search.  This includes how many instances to find.

@internalComponent
*/
void CAgnInstanceExtractor::PreviousInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams)
	{
	const TTime KSearchFromTimeLocal(aSearchParams.iInstance.iId.Date().IsSet() ? aSearchParams.iInstance.iId.Date().LocalL() : aSearchParams.iUndatedTodoTimeLocal);
	const TTime KSearchRangeStartLocal(aSearchParams.iRangeStart.LocalL());
	
	TTime searchFromTime(KSearchFromTimeLocal);
	TTime lastMatchedTime(KSearchFromTimeLocal);
	
	TAgnDaySortKey sortKey(AgnDateTime::MaxDate(), UndatedTodoTime());
	
	// This time is used so that we do not add instances to the list that are after
	// the Nth one that has been found because we know that we will not need them
	TTime NthInstanceTime(AgnDateTime::MinDate());
	
	TInt numInstancesFound(0);
	
	for (iIterator->GotoLessOrEqualL(searchFromTime) ; !iIterator->AtStart() ; iIterator->PreviousL())
		{
		if ( aInstances.Count() > (aSearchParams.iNumInstances + gPossibleInstancesOutsideRange) )
			{
			// We don't need instances after aSearchParams.iNumInstances
			NthInstanceTime = aInstances.At( aInstances.Count() - 1 - (aSearchParams.iNumInstances + gPossibleInstancesOutsideRange) ).InstanceDate();
			}

		const CAgnSimpleEntry& KSimpleEntry(iIterator->CurrentElement());
		
		if(iEntryModel.MatchFullEntryL(KSimpleEntry.EntryId(), aSearchParams))
		    {
		    if (KSimpleEntry.RptDef())
		    	{
				// find instances that overlap into the end of the range
				TTime fromTime = AgnDateTime::ConvertToLocalTimeL(AgnDateTime::ConvertToUtcTimeL(KSearchFromTimeLocal) - TTimeIntervalMicroSeconds(1));
				TTime instanceTime;
				
				while (KSimpleEntry.RptDef()->NudgeNextInstanceL(fromTime, instanceTime, ETrue))
					{
					// get instances that are the same as the start time
					TAgnSortInstance instance(KSimpleEntry);
					instance.SetL(instanceTime, UndatedTodoTime());
					if (instance.CheckStartAndEndDateOverlap(aSearchParams))
		    			{
		    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
		    			
		    			++gPossibleInstancesOutsideRange;
		    			
    	    			if (!aSearchParams.iInstance.iId.IsNullId())
    			    		{
    			    		// We are searching from a particular instance
    			    		// so one nudge forwards is good enough to get the search from instance
    			    		break;
    			    		}
		    			}
					else
						{
						break;
						}
					
					fromTime = instanceTime;
					}
				
				// find n number of instances in the past
				numInstancesFound = 0;
				fromTime = KSearchFromTimeLocal;
					
				while (KSimpleEntry.RptDef()->NudgePreviousUnexceptedInstanceL(fromTime, instanceTime)
						&& ++numInstancesFound <= aSearchParams.iNumInstances
						&&  iIterator->CurrentElement().DurationPlusL(instanceTime) >= KSearchRangeStartLocal
						&&  iIterator->CurrentElement().DurationPlusL(instanceTime) >= NthInstanceTime)
					{
					// get previous n number of instances before the start time
					TAgnSortInstance instance(KSimpleEntry);
					instance.SetL(instanceTime, UndatedTodoTime());
					
					if (instance.CheckStartAndEndDateOverlap(aSearchParams))
		    			{
		    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
		    			}
								
					fromTime = instanceTime;
					}
			    }
		    else
		    	{
		    	// This is a non-repeating entry

	    		TAgnSortInstance instance(KSimpleEntry);
	    		instance.SetL(KSimpleEntry.EntryTime().LocalL(), UndatedTodoTime());	
	    		
	    		if (lastMatchedTime > KSearchFromTimeLocal)
	    			{
	    			// We now have all instances that are the same as the end time
	    			if (++numInstancesFound > aSearchParams.iNumInstances)
	    				{
	    				if (lastMatchedTime > instance.InstanceDate())
	    					{
	    					// We now have all instances that are the same as
	    					// the nth instances before the end time
	    					break;
	    					}
	    				}
	    			}
	    			
	    		// Add the instance to the list
	    		if (instance.CheckStartAndEndDateOverlap(aSearchParams))
	    			{
	    			aInstances.InsertIsqAllowDuplicatesL(instance, sortKey);
	    			lastMatchedTime = instance.InstanceDate();
	    			
	    			if (lastMatchedTime == KSearchFromTimeLocal)
	    				{
	    				++gPossibleInstancesOutsideRange;
	    				}
	    			}
	    		}
		    }
		}
	}

