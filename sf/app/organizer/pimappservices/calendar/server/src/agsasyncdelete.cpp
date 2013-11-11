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

#include "agsasyncdelete.h"

#include "agsfilemanager.h"
#include "agmentry.h"
#include "agsentrymodel.h"
#include "agmrptdef.h"
#include "agmutil.h"
#include "agssess.h"
#include "agmdebug.h"

const TUint KTidyCallbackRate = 24;
const TInt KTidyArrayGranularity = 16;

/**
Used to sort a list of TAgnEntryIds by their stream id component - used when tidying
*/
TInt CCalAsyncDelete::TStreamIdKey::Compare(TInt aLeft,TInt aRight) const
	{
	TUint32 left = static_cast<TAgnEntryId*>(At(aLeft))->StreamId().Value();
	TUint32 right = static_cast<TAgnEntryId*>(At(aRight))->StreamId().Value();
	TInt r = -1;
	if (left == right)
		{
		r=0;
		}
	else if (left > right)
		{
		r=1;
		}
	return (r);
	}

CCalAsyncDelete::CCalAsyncDelete(CAgnEntryModel& aModel, TAgnChangeFilter& aChangeFilter, CAgnSimpleEntryTable& aEntryTable)
	:iModel(aModel), iChangeFilter(aChangeFilter), iEntryTable(aEntryTable)
	{
	}

void CCalAsyncDelete::ConstructL()
	{
	iAlreadyTidied = new(ELeave)CArrayFixFlat<TAgnEntryId>(KTidyArrayGranularity);
	iTidyDeleteArray = new(ELeave)CArrayFixFlat<TAgnEntryId>(KTidyArrayGranularity);
	}

CCalAsyncDelete* CCalAsyncDelete::NewL(CAgnEntryModel& aModel, TAgnChangeFilter& aChangeFilter, CAgnSimpleEntryTable& aEntryTable)
	{
	CCalAsyncDelete* self = new(ELeave) CCalAsyncDelete(aModel, aChangeFilter, aEntryTable);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCalAsyncDelete::~CCalAsyncDelete()
	{
	delete iTidyIter;
	delete iAlreadyTidied;
	delete iTidyDeleteArray;
	}

/**
Determines if an instance of an entry falls within the tidy range.
*/
TBool CCalAsyncDelete::IsEntryValidToTidyL(const CAgnSimpleEntry& aSimpleEntry)
	{
	// non-repeating entries must fall in the time range because otherwise the entry iterator wouldn't find them
	if (aSimpleEntry.RptDef())
		{
		TTime instance;
		if ( ! aSimpleEntry.RptDef()->NudgeNextInstanceL(iTidyStartDate, instance, ETrue))
		    {
            // If the parent is repeating but all its instances are excepted it doesn't mean 
            // that it cannot be deleted. It can if its time range is falling 
            // within the time filter, meaning the parent is safe to tidy as 
            // the child entries are already in the tidy up list.
            if (aSimpleEntry.RptDef()->FirstInstanceL().LocalL() < iTidyStartDate || 
                aSimpleEntry.RptDef()->LastInstanceL().LocalL() > iTidyEndDate)
                {
                return (EFalse);
                }
            else
                {
                return (ETrue);
                }
            }
		
        if (instance < iTidyStartDate || instance > iTidyEndDate)
            {
            return (EFalse);
            }
        }
    return (ETrue);
    }


/**
Calculate the number of entries to be tidied (so the percentage tidied so far
can be determined) and create and required objects.
@capability WriteUserData
@capability ReadUserData
*/
TBool CCalAsyncDelete::SetUpDeleteL(const TAgnFilter& aFilter, const TTime& aTodaysDate, const TTime& aStartDate, const TTime& aEndDate)
	{
	iTidyStartDate = aStartDate;
	iTidyEndDate = aEndDate;
	iTidyDate = iTidyStartDate;

	#if defined (__CAL_ASYNC_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		{
		TBuf<KMinTTimeStrLength> tidyEndDateTimeBuf;
		AgmDebug::TTimeStrL(iTidyEndDate, tidyEndDateTimeBuf);
		TBuf<KMinTTimeStrLength> tidyStartDateTimeBuf;
		AgmDebug::TTimeStrL(iTidyStartDate, tidyStartDateTimeBuf);
		AgmDebug::DebugLog("Async Delete: Range From %S To %S", &tidyStartDateTimeBuf, &tidyEndDateTimeBuf);
		}
	#endif
		
	iNumEntriesToProcess = 0;
	delete iTidyIter;
	iTidyIter = NULL;
	
	iTidyIter = new(ELeave) TAgnEntryDateIterator(iEntryTable, aTodaysDate, aFilter);
	
	for (iTidyIter->GotoL(iTidyStartDate); !iTidyIter->AtEnd() && iTidyIter->CurrentKey() <= iTidyEndDate; iTidyIter->NextL())
		{
		if (IsEntryValidToTidyL(iTidyIter->CurrentElement()))
			{
			++iNumEntriesToProcess;
			}
		}
	
	if (!iNumEntriesToProcess)
		{ // there are no entries to tidy that match the date/filter criteria
		return (EFalse); 
		}

	iNumEntriesProcessed = 0;
	return (ETrue);
	}

TBool CCalAsyncDelete::CheckDateIteratorEnd() const
	{
	return ( ! iTidyIter->AtEnd() && iTidyIter->CurrentKey() <= iTidyEndDate);
	}

/**
Tidy the agenda model and return the progress as a percentage of the number of entries tidied so far.
Returns 0 when the tidy has completed.
The callback frequency and percentage completed are based on the number of entries tidied so far out
of the total number of entreis that match the tidy filter and date range. But as the iterator may be
position half-way through the entries for a day when a callback is due to be made then its necessary
to continue processing till the end of that day before the callback is made - this is so that next time
round the same entries aren't processed twice.

@capability WriteUserData
@capability ReadUserData
*/
TInt CCalAsyncDelete::DoDeleteStepL()
	{
	iTidyIter->GotoL(iTidyDate);	

	TInt pos = 0;
	TKeyArrayFix alreadyTidiedKey(0, ECmpTUint32);
	while(CheckDateIteratorEnd())
		{
		const CAgnSimpleEntry& simpleEntry = iTidyIter->CurrentElement();

		if (IsEntryValidToTidyL(simpleEntry))
			{
			TBool alreadyTidied = EFalse;
			
			// If the entry is repeating or if it spans more than one instant then the iterator may return the
			// same entry more than once (due to the fact that the call backs interrupt the sequence and the
			// iterator is reset on each chunk of processing) so add then to an array to prevent this
			if ( simpleEntry.RptDef() || simpleEntry.StartTime() != simpleEntry.EndTime())
				{
				if (iAlreadyTidied->Find(simpleEntry.EntryId(), alreadyTidiedKey, pos) == 0)
					{
					alreadyTidied = ETrue;
					}
				else
					{
					iAlreadyTidied->AppendL(simpleEntry.EntryId());
					}
				}
			
			if ( ! alreadyTidied)
				{
				TidyEntryL(simpleEntry);
				
				if ((++iNumEntriesProcessed % KTidyCallbackRate)==0)
					{
					DeleteTidiedEntriesL();
					
					TInt percentage = (iNumEntriesProcessed * KAgnPercentageComplete) / iNumEntriesToProcess;
					return (percentage < 1 ? 1 : percentage);
					}

				__ASSERT_DEBUG(iNumEntriesProcessed <= iNumEntriesToProcess, Panic(EAgmErrCorruptDelete));
				}
			}

		iTidyIter->NextL();	
		}
	
	DeleteTidiedEntriesL();
	return KAgnPercentageComplete;
	}

/**
Delete entries whose id's have been placed in iTidyDeleteArray during the tidying process
@capability WriteUserData
@capability ReadUserData
*/
void CCalAsyncDelete::DeleteTidiedEntriesL()
	{
	if (iTidyDeleteArray && iTidyDeleteArray->Count())
		{
		// sort the entries in the tidy array by their entry ID (and therefore by their stream)
		// this ensures entries in the same stream will be deleted together
		TStreamIdKey key;
		iTidyDeleteArray->Sort(key);
		
		const TInt KDeleteArrayCount = iTidyDeleteArray->Count();
		for (TInt ii = 0; ii < KDeleteArrayCount; ++ii)
			{
			CAgnEntry* entry = iModel.FetchEntryL((*iTidyDeleteArray)[ii]);
			if (entry)
				{
				CleanupStack::PushL(entry);
				iModel.DeleteEntryL(*entry, ETrue, NULL);
				CleanupStack::PopAndDestroy(entry);
				}
			}
		iModel.ResetDeleteRollbackArray();
		iTidyDeleteArray->Reset();
		iModel.FlushL();
		}
	}

/**
Tidies (i.e. deletes) the entry identified by aSimpleEntry from the agenda if its eligible according to 
its dates. 
If it is non-repeating then for it to be tidied its instance date must fall within iTidyStartDate and
iTidyEndDate. 
If it is repeating then both its repeat start and until dates must fall within the range. 
If it is an undated todo then it would not have been selected by the iterator if today does not fall within the tidy
range hence it can be added without further checks.
*/
void CCalAsyncDelete::TidyEntryL(const CAgnSimpleEntry& aSimpleEntry)
	{
	CAgnEntry* entry = iModel.FetchEntryL(aSimpleEntry.EntryId());
	if (entry == NULL)
		{
		// already deleted
		return;
		}
	
	CleanupStack::PushL(entry);
	
	TBool todelete = EFalse;
	if(entry->GsDataType() == CGsData::EParent)
		{
		// Only parents need to be tidied
		todelete = FallInTimeRangeL(aSimpleEntry);
		// If this Entry is a parent and it has children
		if (todelete)
			{
			//Find out whether all children fall in the time range to tidy
			const RArray<TGsChildRefData>& KChildIds = entry->ChildIds();
		   
			const TInt KCount = KChildIds.Count();
			// Check each child to see if they are all fall in the date range
			for (TInt i = 0; i < KCount && todelete; ++i)
				{
				const CAgnSimpleEntry* KChildEntry = iModel.GetSimpleEntryFromIndexes(KChildIds[i].ChildId());
				if(KChildEntry != NULL)
				    {
                    todelete = FallInTimeRangeL(*KChildEntry);
				    }
				
				}
			}

	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		if(todelete)
			{
			AgmDebug::DebugLog("TidyEntryL: Async - Tidying - Deleting an entry");
			AgmDebug::DebugLogEntryL(*entry,EDumpEntryIDs);
			}
	#endif
		}
	CleanupStack::PopAndDestroy(entry);
	
	if(todelete)
		{
		iTidyDeleteArray->AppendL(aSimpleEntry.EntryId());
		if(aSimpleEntry.Type() == CCalEntry::ETodo)
			{
			iChangeFilter.SetPubSubChange(TAgnChangeFilter::ETodoChanged);
			}
		else
			{
			iChangeFilter.SetPubSubChange(TAgnChangeFilter::EEventChanged);
			}
		}
	}

TBool CCalAsyncDelete::FallInTimeRangeL(const CAgnSimpleEntry& aSimpleEntry)
	{
	if (aSimpleEntry.RptDef())
		{ 
		TTime firstUnexceptedInstanceDate;
		TTime lastUnexceptedInstanceDate;
		if(!aSimpleEntry.RptDef()->NudgeNextInstanceL(aSimpleEntry.RptDef()->FirstInstanceL().LocalL(), firstUnexceptedInstanceDate, ETrue))
			{
			firstUnexceptedInstanceDate = aSimpleEntry.RptDef()->FirstInstanceL().LocalL(); //we don't want the last date - we want the first date
			}
		if(!aSimpleEntry.RptDef()->NudgePreviousUnexceptedInstanceL(aSimpleEntry.RptDef()->LastInstanceL().LocalL(), lastUnexceptedInstanceDate))
			{
			lastUnexceptedInstanceDate = aSimpleEntry.RptDef()->LastInstanceL().LocalL(); // we don't want the first date - we want the last date
			}
		if ((firstUnexceptedInstanceDate >= iTidyStartDate) && (lastUnexceptedInstanceDate <= iTidyEndDate))

			{
			return ETrue;
			}
		}
	else
		{
		if (aSimpleEntry.Type()==CCalEntry::ETodo)
			{
			// check due date only for Todos
			if (aSimpleEntry.EndTime().LocalL() <= iTidyEndDate && aSimpleEntry.EndTime().LocalL() >= iTidyStartDate)
				{
				return ETrue;
				}
			}
		else
			{
			if (aSimpleEntry.EndTime().LocalL() <= iTidyEndDate && aSimpleEntry.StartTime().LocalL() >= iTidyStartDate)
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}
