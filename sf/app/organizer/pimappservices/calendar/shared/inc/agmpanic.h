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

#ifndef __AGMPANIC_H__
#define __AGMPANIC_H__

/**
@internalAll
@released
*/
enum TAgmPanic
	{
	// server-specific panic codes (for BC)
	EBadRequest =	-1,
	EBadDescriptor = -2,
	EBadStore =		-3,
	EFileCorrupt =	-4,
	EFileUnlocked = -5,
	EBadExtractor = -6,
	EIndexError	  = -7,
	
	// all panic codes
	EAgmErrBadEntry = 0,
	EAgmErrBadTypeEntry = 1,
	EAgmErrOutsideArrayRange = 2,	// attempted to access element of array out side range
	EAgmErrCompoundIteratorOutOfRange = 3,
	EAgmErrParamHasNullDate = 4,
	EAgmErrInvalidInstance = 5, // i.e. tried to fetch an instance that couldn't be generated from the repeat details
	EAgnErrEntryManagerInternalError = 6, //something has gone wrong with the entry cluster management i.e
	EAgmInvalidPriorityValue=7,
	EAgmNoNotesStreamId = 8,
	EAgmNullDescriptor = 9, 
	EParentWithoutGuid = 10,
	EAgmErrBadRepeat = 11,
	EAgmErrBadTime= 12,
	EAgmErrNotChildEntry= 13,
	EAgmErrChildWithoutParent= 14,
	EAgmErrNotParentEntry= 15,
	EAgmErrUpdateInvalid= 16,
	EAgmErrBadAttachmentType = 17,
	EAgmNoAlarmsInList					= 18,
	EAgmAgnAlarmActiveNotAdded			= 19,
	EAgmNoAllocatorMemoryPages			= 20,
	EAgmAllocatorMemoryPageNotFound		= 21,
	EAgmErrInvalidStatus				= 22,
	EAgmEntryHasNoStartDate				= 23,
	EAgmErrOnlyOneEntryAllowedInBuffer	= 24,
	EAgmErrStreamStoreIsNull			= 25,
	EAgmErrStreamIdIsNull				= 26,
	EAgmErrExpectedEntriesInStore		= 27,
	EAgmErrFailedToRestoreBuffer		= 28, 
	EAgmErrInvalidEntryManagerIndex		= 29,
	EAgmErrEntryBufferExceedsMaxStreamSize = 30,
	EAgmErrEntryIdNotUnique				= 31,
	EAgmErrInvalidPartialId				= 32,
	EAgmErrNoMorePartialIdsAvailable	= 33,
	EAgmErrPartialIdOverFlow			= 34,
	EAgmErrWrongEntryDeleted			= 35,
	EAgmErrInvalidRecurrenceRange		= 36,
	EAgmErrCategoryOutOfRange			= 37,
	EAgmErrRptExtractorContainsNonRpt	= 38,
	EAgmErrCorruptFile					= 39,
	EAgmErrInvalidWeekNumber			= 40,
	EAgmErrNoRepeatData					= 41,
	EAgmErrNoRepeatRule					= 42,
	EAgmErrCouldNotFindNewEntryId		= 43,
	EAgmErrStreamIdIndexOutOfRange		= 44,
	EAgmErrCorruptDelete				= 45,
	EAgmErrUnsupportedTimeMode			= 46,
	EAgmErrInvalidBufferType			= 47,
	EAgmErrInvalidWeekDay				= 48,
	EAgmErrInvalidMonthDay				= 49,
	EAgmErrInvalidRptType				= 50,
	EAgmErrCategoryListNotInitialised	= 51,
	EAgmErrTimeNotSet					= 52,
	EAgmErrInvalidIndexUpdate			= 53,
	EAgmErrRollbackFailed				= 54,
	EAgmErrAddingSecondRepeatingChildEntry = 55,
	EAgmErrAddingChildEntryToNonRepeatingParent = 56,
	EAgmErrCategoryCountCorrupt 		= 57,
	EAgmErrInvalidSimpleEntryIndex		= 58,
	EAgmErrIndexKeyNotSpecified			= 59,
	EAgmErrInvalidAttachmentSortOrder	= 60,
	EAgmErrTwoAsyncTasksStarted			= 61,
	EAgmErrTidyByDateNoFileOpen			= 62,
	EAgmErrAttachmentCountCorrupt		= 63,
	EAgmErrCalendarFileNotFound			= 64,
	EAgmErrFileTimerAlreadyActive		= 65,
	EAgmErrNullPointer				    = 66,
	EAgmErrTzRuleIsNotFound				= 67,
	EAgmErrInstanceSorterArgument       = 68,
	EAgmErrInstanceIteratorMgrNotInitialised = 69,
	EAgmErrInstanceIteratorLocatedIndexFault = 70,
	};

IMPORT_C void Panic(TAgmPanic aPanic);

#endif
