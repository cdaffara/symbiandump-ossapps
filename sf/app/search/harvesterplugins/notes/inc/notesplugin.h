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


#ifndef CNOTESPLUGIN_H
#define CNOTESPLUGIN_H

#include <e32base.h>
#include <cindexingplugin.h>
#include <calchangecallback.h>
#include <calcommon.h> // TCalLocalUid

#include <common.h>
#include "delayedcallback.h"

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntryView;
class CCPixIndexer;
class CCalInstanceView;
class CCalInstance;

#define KNotesDllUid 0x2001A9D8

class CNotesPlugin : public CIndexingPlugin, public MDelayedCallbackObserver, public MCalChangeCallBack2
{
public: // Constructors and destructo

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
    /*
     * NewL
     * @return instance of Notes plugin
     */
	static CNotesPlugin* NewL();
	/*
     * NewLC
     * @return instance of Notes plugin
     */
	static CNotesPlugin* NewLC();
	/*
     * Destructor
     */
	virtual ~CNotesPlugin();

public: // From CIndexingPlugin
	void StartPluginL();
	void StartHarvestingL(const TDesC& aQualifiedBaseAppClass);
    void PausePluginL();
    void ResumePluginL();
	
public:	// From MCalChangeCallBack2
	
	void CalChangeNotification( RArray< TCalChangeEntry >& aChangeItems );	
	
public: // From MDelayedCallbackObserver

    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aError);
    
private: 
    /*
     * @description Prepares the CPix action to be done.
     * @param TCalChangeEntry changedEntry details of the modified calendar entry
     * Leaves in case of errors.
     */
	void HandleNoteChangedEntryL(const TCalChangeEntry& changedEntry);
	
	/*
     * @description Prepares the search document and update the CPix engine 
     * according to the passed aActionType.
     * @param TCalLocalUid aLocalUid uid of the cal entry
     * @param TCPixActionType aActionType Add/update/remove option
     * Leaves in case of errors.
     */
	void CreateNoteEntryL( const TCalLocalUid& aLocalUid, TCPixActionType aActionType );
	/*
     * @description Reads the default start and end times from the config file.
     * @param TTime& aStartTime   Start time for creating calendar views
     * @param TTime& aEndTime  End time for creating calendar views   
     * Leaves in case of errors.
     */
	void InitTimeValuesL( TTime& aStartTime, TTime& aEndTime );
	
	void OverWriteOrAddToQueueL(const TCalLocalUid& aLocalUid, TCPixActionType aActionType);
	        
	void IndexQueuedItems();

private: // Constructors
	
	/**
	 * Default C++ constructor.
	 */
	CNotesPlugin();
	
	/**
	 * Symbian 2nd phase constructor.
	 */
	void ConstructL();

private:
	
	// The asynchronizer .owned
	CDelayedCallback* iAsynchronizer;	
	
	// CPix database .owned
    CCPixIndexer* iIndexer;
    
	// Calendar session .owned
	CCalSession* iSession;
	
	// Calendar entry view to fetch the details of Note entry for given LocalUid .owned
	CCalEntryView* iEntryView;
	
	//Calinstance view to retrive notes .owned
	CCalInstanceView* iNotesInstanceView;
	
	//Array of calinstances of Notes .owned
	RPointerArray<CCalInstance> iNotesInstanceArray;
	
	//Notes count
	TInt iNoteCount;
	
	//State of harvester either to pause/resume
    TBool iIndexState;    
    // Queue of documents to be indexed
    RArray<TRecord> iJobQueue;    
    //harvesting state
    THarvesterState iHarvestState;
	
	//for unit testing.
	#ifdef HARVESTERPLUGINTESTER_FRIEND
	    friend class CHarvesterPluginTester;
	#endif	
	
	
#ifdef __PERFORMANCE_DATA
    TTime iStartTime;
    TTime iCompleteTime;
    void UpdatePerformaceDataL();
#endif
	
};

#endif // CNOTESPLUGIN_H
