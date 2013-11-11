/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
 *
*/

#ifndef CCALENDARPLUGIN_H
#define CCALENDARPLUGIN_H

#include <e32base.h>
#include <cindexingplugin.h>
#include <calprogresscallback.h>
#include <calchangecallback.h>
#include <calcommon.h> // TCalLocalUid

#include <common.h>
#include "delayedcallback.h"

// FORWARD DECLARATIONS
class CCalSession;
class CCalIter;
class CCalEntryView;
class CCPixIndexer;


/** Field names */
_LIT(KCalendarSummaryField, "Summary");
_LIT(KCalendarDescriptionField, "Description");
_LIT(KCalendarLocationField, "Location");
_LIT(KCalendarStartTimeField, "StartTime");
_LIT(KCalendarEndTimeField, "EndTime");
_LIT(KCalenderCompletedField, "CompletedDate");
_LIT(KCalendarAttendeesField, "Attendees");
_LIT(KCalendarPriorityField, "Priority");
_LIT(KCalendarPriorityHigh, "High");
_LIT(KCalendarPriorityMedium, "Normal");
_LIT(KCalendarPriorityLow, "Low");

class CCalendarPlugin : public CIndexingPlugin, public MDelayedCallbackObserver, public MCalProgressCallBack, public MCalChangeCallBack2
{
public: // Constructors and destructor
    
    enum THarvesterState
            {
            EHarvesterIdleState,
            EHarvesterStartHarvest        
            };
    struct TRecord 
        {
        TCalLocalUid iLocalUid;
        TCPixActionType iActionType;
        };
    
	static CCalendarPlugin* NewL();
	static CCalendarPlugin* NewLC();
	virtual ~CCalendarPlugin();

public: // From CIndexingPlugin
	void StartPluginL();
	void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
	void PausePluginL();
	void ResumePluginL();
	
public: // From MCalProgressCallBack

	/** Progress callback.
	This calls the observing class with the percentage complete of the current operation.
	@param aPercentageCompleted The percentage complete. */
	void Progress(TInt aPercentageCompleted);
	
	/** Progress callback.
	This calls the observing class when the current operation is finished.
	@param aError The error if the operation failed, or KErrNone if successful. */
	void Completed(TInt aError);
	
	/** Asks the observing class whether progress callbacks are required.
	@return If the observing class returns EFalse, then the Progress() function will not be called. */
	TBool NotifyProgress();
	
public:	// From MCalChangeCallBack2
	
	void CalChangeNotification( RArray< TCalChangeEntry >& aChangeItems );	
	
public: // From MDelayedCallbackObserver

    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aError);
//#ifdef USE_HIGHLIGHTER
public: // AddExcerpt function
    
    /*
     * Add the value to excerpt field
     * @param TDesC& aExcerptValue excerpt value
     */
    void AddToFieldExcerptL(const TDesC& aExcerptValue);
    
 
private:
 
    /*
     * reset the excerpt
     */
    void ResetExcerpt();

//#endif
    /*
       * For date and time get discriptor value in the passed format
       * @param TDateTime& datetime: date time to be formatted
       * @param const TDesC& aFormat: to this format
       * @param TDes& dateString: formated date 
       */    
     
     void GetDateTimeDescriptorL(TDateTime& datetime, const TDesC& aFormat, TDes& dateString);
private: // New functions
	
	void HandleChangedEntryL(const TCalChangeEntry& changedEntry);
	/*
     * Indexes a calendar entry
     */
	void CreateEntryL( const TCalLocalUid& aLocalUid, TCPixActionType aActionType );
	/*
     * Adds/updates a calender entry to queue on pause of plugin
     * @param const TCalLocalUid& aLocalUid: unique UID of a calender entry
     * @param TCPixActionType aActionType: add/update or delete event
     */
	void OverWriteOrAddToQueueL(const TCalLocalUid& aLocalUid, TCPixActionType aActionType);
	/*
     * On resume, removes the entries from the queue and starts indexing
     */    
	void IndexQueuedItems();

private: // Constructors
	
	/**
	 * Default C++ constructor.
	 */
	CCalendarPlugin();
	
	/**
	 * Symbian 2nd phase constructor.
	 */
	void ConstructL();

private:
	
	/** The asynchronizer */
	CDelayedCallback* iAsynchronizer;	
	
	// CPix database 
    CCPixIndexer* iIndexer;
    
	/** Calendar session */
	CCalSession* iSession;
	
	/** Calendar iterator */
	CCalIter* iCalIterator;
	
	/** Calendar entry view */
	CCalEntryView* iEntryView;
	
	/** Are we positioned at first entry? */
	TBool iFirstEntry;
	
	//State of harvester either to pause/resume
    TBool iIndexState;    
    // Queue of documents to be indexed
    RArray<TRecord> iJobQueue;    
    //harvesting state
    THarvesterState iHarvestState;
	// Start harvesting
	TBool iStartHarvesting;
//#ifdef USE_HIGHLIGHTER	
	// Excerpt field
    HBufC* iExcerpt;
//#endif
	//for unit testing.
    #ifdef HARVESTERPLUGINTESTER_FRIEND
        friend class CHarvesterPluginTester;
    #endif

#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
    void UpdatePerformaceDataL(TCPixActionType);
#endif
};

#endif // CCALENDARPLUGIN_H
