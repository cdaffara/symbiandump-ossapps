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

#ifndef __TCAL_OBSERVER_H__
#define __TCAL_OBSERVER_H__

#include <e32test.h>
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calrrule.h>

#include "caltestlib.h"

_LIT(KChunkName, "tcal_observer_chunk");
_LIT(KTestCaseChunkName, "tcal_test_case_observer_chunk");
_LIT(KCalendarFile, "tcal_observer");
_LIT(KCalendarFileOther, "tcal_observer_other");
_LIT(KSemName, "tcal_observer_sem");
_LIT(KThreadName, "tcal_observer__modifier_thread");

const TTime KRangeStartTime(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0));
const TTime KRangeEndTime(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));

TInt RunTestThread(TAny* /*aArgs*/); // modifying thread entry point

class CTestApp : public CBase, public MCalProgressCallBack
	{
protected:
	enum TTestCase
		{
		EEntryOutOfRange,				// add entry that is out of filter range
		EEntryOutOfRange1,				// add child entry that is out of filter range
		EApptInsideRange1,				// add
		EApptInsideRange2,				// update
		EApptInsideRange3,				// delete
		EApptOutsideRange1,				// add
		EApptOutsideRange2,				// update
		EApptOutsideRange3,				// delete
		ENotifyNonResponsive,			// test nonresponsive client				
		ENotifyNonResponsive1,			// delete
		ETodoInsideRange1,				// disable notification and add 
		ETodoInsideRange2,				// re-enable notification		
		ETodoInsideRange3,				// update
		ETodoInsideRange4,				// delete
		ETodoOutsideRange1,				// add
		ETodoOutsideRange2,				// update
		ETodoOutsideRange3,				// delete
		EAddUndatedTodo,				// adding undated todo
		EUpdateUndatedTodo,				// update undated todo
		EClearUndatedTodo,				// delete undated todo
		EMultipleInAndOutside1,			// add 2 todo and 2 events, each with 1 inside and 1 outside
		EMultipleInAndOutside2,			// delete
		EBulkAdd,					// Add bulk entries - Random number of notifications are received
		EBulkDelete,					// Delete bulk entries - Random number of notifications are received
		EBulkAddToOverflowNotificationBuffer,	// Add bulk entries and wait to overflow the notification buffer
		EBulkDeleteToOverflowNotificationBuffer,// Delete bulk entries and wait to overflow the notification buffer
		ERepeatingEitherSideOfRange1,	// add entry with instances before and after range, but not inside
		ERepeatingEitherSideOfRange2,	// update
		ERepeatingEitherSideOfRange3,	// delete
		ERepeatingAcrossStartBoundary1,	// add on start boundary
		ERepeatingAcrossStartBoundary2, // move to outside range
		ERepeatingAcrossStartBoundary3,	// delete
		ERepeatingAcrossStartBoundary4,	// add outside range
		ERepeatingAcrossStartBoundary5,	// move to on start boundary
		ERepeatingAcrossStartBoundary6,	// delete
		ERepeatingAcrossEndBoundary1,	// add on end boundary
		ERepeatingAcrossEndBoundary2,	// move to ouside range
		ERepeatingAcrossEndBoundary3,	// delete
		ERepeatingAcrossEndBoundary4,	// add outside range
		ERepeatingAcrossEndBoundary5,	// move to end boundary
		ERepeatingAcrossEndBoundary6,	// delete
		EFilterOnlyEventEntries1,		// change filter to only events
		EFilterOnlyEventEntries2,		// add todo entry
		EFilterOnlyEventEntries3,		// add event entry
		EFilterOnlyEventEntries4,		// delete
		EFilterOnlyTodoEntries1,		// change filter to only todos
		EFilterOnlyTodoEntries2,		// add todo entry
		EFilterOnlyTodoEntries3,		// add event entry
		EFilterOnlyTodoEntries4,		// delete
		EFilterOnlyTodoEntries5,		// restore filter
		EAddAndUpdateParentAndChild1,	// add the parent entry
		EAddAndUpdateParentAndChild2,	// update the parent entry
		EAddAndUpdateParentAndChild3,	// add a child entry
		EAddAndUpdateParentAndChild4,	// update the child
		EAddAndUpdateParentAndChild5,	// store the parent again
		EAddAndUpdateParentAndChild6,	// add the child again
		EAddAndUpdateParentAndChild7,	// delete the child
		EAddAndUpdateParentAndChild8,	// add the child again
		EAddAndUpdateParentAndChild9,	// delete the parent
		EDeleteMultipleEntriesSetup,	// create entry for delete test
		EDeleteMultipleEntriesFail,		// delete multiple entries, the second will fail
		EChangesToOtherFile,			// open another file and add entry to it (test stops notification)
		EAmount,						// used for looping through the test cases  
		};
public:
	virtual void RunTestL() = 0;
	~CTestApp();
	void ConstructL();
	
private: // from MCalProgressCallBack
	void Progress(TInt ) {;}
	TBool NotifyProgress() {return EFalse;}
	void Completed(TInt aError);
	
protected:
	CCalTestLibrary*	iCalTestLibrary;
	CPeriodic*			iTimer;
	TBool				iActiveSchedulerStarted;
	RChunk				iChunk;
	RSemaphore			iSemaphore;
	TInt*				iNumExpectedChanges;
	TCalChangeEntry*	iExpectedChanges;
	TInt*				iTestCase;
	};
	
#endif // __TCAL_OBSERVER_H__
