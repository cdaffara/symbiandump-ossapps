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


#include "notesplugin.h"
#include "harvesterserverlogger.h"
#include <common.h>
#include <csearchdocument.h>
#include <ccpixindexer.h>
#include <e32base.h> 
#include <calsession.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <calentryview.h>
#include <e32std.h>
#include <centralrepository.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notespluginTraces.h"
#endif


_LIT(KMimeTypeNotes , NOTES_MIMETYPE);
_LIT(KMimeTypeField , CPIX_MIMETYPE_FIELD);

const TUid KCPIXrepoUidMenu = {0x20029ac7};
/** The delay between harvesting chunks. */
const TInt KHarvestingDelay = 2000;
//Length of date string in cenrep
const TInt KDateStringLength = 10;
//Key value for start date in cenrep
const TUint32 KStartDateKey = 0x1;
//Key value for end date in cenrep
const TUint32 KEndDateKey = 0x2;
/** Field names */
//Memo Field name in CPIX indexer for notepad record
_LIT(KNpdMemo , "Memo");
//date Field name in CPIX indexer for notepad record
_LIT(KNpdUpdateTime , "Date");

//Reference from CPix calender harvester plugin.
_LIT(KNotesTimeFormat,"%04d %02d %02d %02d %02d");

//_LIT(KExcerptDelimiter, " ");
// ---------------------------------------------------------------------------
// CNotesPlugin::NewL
// ---------------------------------------------------------------------------
//  
CNotesPlugin* CNotesPlugin::NewL()
	{
	CNotesPlugin* instance = CNotesPlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::NewLC
// ---------------------------------------------------------------------------
//  
CNotesPlugin* CNotesPlugin::NewLC()
	{
	CNotesPlugin* instance = new (ELeave) CNotesPlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::CNotesPlugin
// ---------------------------------------------------------------------------
//  
CNotesPlugin::CNotesPlugin()
	{
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::~CNotesPlugin
// ---------------------------------------------------------------------------
//  
CNotesPlugin::~CNotesPlugin()
	{
    if (iAsynchronizer)
        iAsynchronizer->CancelCallback();
	delete iAsynchronizer;
	delete iIndexer;
    delete iNotesInstanceView;
	delete iEntryView;
	iJobQueue.Reset();
	iJobQueue.Close();
	iNotesInstanceArray.ResetAndDestroy();
	if( iSession )
		{
		iSession->StopChangeNotification();
		}
	delete iSession;
	}
	
// ---------------------------------------------------------------------------
// CNotesPlugin::ConstructL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::ConstructL()
	{
    iIndexState = ETrue;
	iAsynchronizer = CDelayedCallback::NewL( CActive::EPriorityIdle );
	iSession = CCalSession::NewL();
	TRAPD ( err , iSession->OpenL( iSession->DefaultFileNameL() ) );
    if ( err == KErrNotFound)
        {
        iSession->CreateCalFileL( iSession->DefaultFileNameL() );
        iSession->OpenL( iSession->DefaultFileNameL() );
        }
	iNotesInstanceView = CCalInstanceView::NewL( *iSession );	
	iEntryView = CCalEntryView::NewL( *iSession );	
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::StartPluginL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::StartPluginL()
	{
    OstTraceFunctionEntry0( CNOTESPLUGIN_STARTPLUGINL_ENTRY );
    CPIXLOGSTRING("CNotesPlugin::StartPluginL: Enter");
	// Define this base application class, use default location
	User::LeaveIfError(iSearchSession.DefineVolume( _L(NOTES_QBASEAPPCLASS), KNullDesC ));

	// Open the database
	iIndexer = CCPixIndexer::NewL(iSearchSession);
	iIndexer->OpenDatabaseL( _L(NOTES_QBASEAPPCLASS) ); 

	// Start harvester for this plugin
	iObserver->AddHarvestingQueue(this, iIndexer->GetBaseAppClass());
	
	// Start monitoring Notes events
	//Have taken start time and end time reference from calender plugin
	TTime startTime , endTime;
	InitTimeValuesL ( startTime , endTime );
	TCalTime startTimeCal , endTimeCal;
	startTimeCal.SetTimeUtcL( startTime );
	endTimeCal.SetTimeUtcL( endTime );
	
	//Created the filter for monitoring. As of now there is no TChangeEntryType for monitoring Notes,
	//Monitoring done for all the calendar entry types and a check included while updating CPix database for Notes item.
	CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL( MCalChangeCallBack2::EChangeEntryAll, ETrue, 
                                                                               CalCommon::TCalTimeRange( startTimeCal, endTimeCal ) );
	//Start the Monitoring
	iSession->StartChangeNotification( *this, *filter );
	delete filter;
	CPIXLOGSTRING("CNotesPlugin::StartPluginL: Exit");
	OstTraceFunctionExit0( CNOTESPLUGIN_STARTPLUGINL_EXIT );
	}	

// ---------------------------------------------------------------------------
// CNotesPlugin::StartHarvestingL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::StartHarvestingL(const TDesC& /*aQualifiedBaseAppClass*/)
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_STARTHARVESTINGL_ENTRY );
    CPIXLOGSTRING("CNotesPlugin::StartHarvestingL: Enter");
    iIndexer->ResetL();
    iHarvestState = EHarvesterStartHarvest;
    //Have taken start time and end time reference from calender plugin
    TTime startTime , endTime;
    InitTimeValuesL( startTime , endTime );
    TCalTime startTimeCal , endTimeCal ;
    startTimeCal.SetTimeUtcL( startTime );
    endTimeCal.SetTimeUtcL( endTime );
    //Create an instance for list of Notes items in the system
    iNotesInstanceView->FindInstanceL( iNotesInstanceArray , CalCommon::EIncludeNotes , CalCommon::TCalTimeRange( startTimeCal, endTimeCal ));    
    iNoteCount = iNotesInstanceArray.Count();
    OstTrace1( TRACE_NORMAL, CNOTESPLUGIN_STARTHARVESTINGL, "CNotesPlugin::StartHarvestingL;Notes Count=%d", iNoteCount );
    CPIXLOGSTRING2("CNotesPlugin::StartHarvestingL(): Notes count =%d.", iNoteCount);
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
    iAsynchronizer->Start( 0, this, KHarvestingDelay );
    CPIXLOGSTRING("CNotesPlugin::StartHarvestingL: Exit");
    OstTraceFunctionExit0( CNOTESPLUGIN_STARTHARVESTINGL_EXIT );
    }

// -----------------------------------------------------------------------------
// CNotesPlugin::DelayedCallbackL
// -----------------------------------------------------------------------------
//
void CNotesPlugin::DelayedCallbackL( TInt /*aCode*/ )
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_DELAYEDCALLBACKL_ENTRY );
    // Harvest items on each call
    CPIXLOGSTRING("CNotesPlugin::DelayedCallbackL: Enter");
    //If paused state
    if(!iIndexState)
       return;
    
    if( iNoteCount )
        {
        OstTrace1( TRACE_NORMAL, CNOTESPLUGIN_DELAYEDCALLBACKL, "CNotesPlugin::DelayedCallbackL;Remaining Notes Count=%d", iNoteCount );
        CPIXLOGSTRING2("CNotesPlugin::DelayedCallbackL(): remaining Note count=%d.", iNoteCount);
        // Retrieve the calendar entry for the calinstance and update the CPix database.
        CCalEntry& noteentry =  iNotesInstanceArray[iNoteCount - 1]->Entry();        
        CreateNoteEntryL( noteentry.LocalUidL(), ECPixAddAction );        
        iNoteCount--;    
        // Request next entry.
        iAsynchronizer->Start( 0, this, KHarvestingDelay );           
        }
	else
		{
        OstTrace0( TRACE_NORMAL, DUP1_CNOTESPLUGIN_DELAYEDCALLBACKL, "CNotesPlugin::DelayedCallbackL: Completed Harvesting" );
        CPIXLOGSTRING("CNotesPlugin::DelayedCallbackL: Completed Harvesting");
		// Harvesting was successfully completed		
		Flush(*iIndexer);
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif
        iHarvestState = EHarvesterIdleState;
        iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), KErrNone);		
		}
	OstTraceFunctionExit0( CNOTESPLUGIN_DELAYEDCALLBACKL_EXIT );
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::DelayedError
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::DelayedError(TInt aError)
	{
	// Harvesting was completed
	Flush(*iIndexer);
	iHarvestState = EHarvesterIdleState;
	iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), aError);
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::CalChangeNotification
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::CalChangeNotification( RArray< TCalChangeEntry >& aChangeItems )
	{
    OstTraceFunctionEntry0( CNOTESPLUGIN_CALCHANGENOTIFICATION_ENTRY );
    CPIXLOGSTRING("CNotesPlugin::CalChangeNotification: Enter");
	const TInt count(aChangeItems.Count());
	OstTrace1( TRACE_NORMAL, CNOTESPLUGIN_CALCHANGENOTIFICATION, "CNotesPlugin::CalChangeNotification;Changed Item Count=%d", count );
	CPIXLOGSTRING2("CNotesPlugin::CalChangeNotification(): changed item count =%d.", count);
	for( TInt i = 0; i < count; ++i )
		{
        TRAP_IGNORE(HandleNoteChangedEntryL( aChangeItems[ i ] ));
		}
	CPIXLOGSTRING("CNotesPlugin::CalChangeNotification: Exit");
	OstTraceFunctionExit0( CNOTESPLUGIN_CALCHANGENOTIFICATION_EXIT );
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::HandleChangedEntryL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::HandleNoteChangedEntryL(const TCalChangeEntry& changedEntry)
	{
	switch( changedEntry.iChangeType )
		{		
		case EChangeAdd:
			{
			OstTrace1( TRACE_NORMAL, CNOTESPLUGIN_HANDLENOTECHANGEDENTRYL, "CNotesPlugin::HandleNoteChangedEntryL;Monitored add id=%d", changedEntry.iEntryId );
			CPIXLOGSTRING2("CNotesPlugin::HandleNoteChangedEntryL(): Monitored add id=%d.", changedEntry.iEntryId);
			if( iIndexState )
			    CreateNoteEntryL( changedEntry.iEntryId, ECPixAddAction );
			else
			    OverWriteOrAddToQueueL(changedEntry.iEntryId, ECPixAddAction);
			break;
			}

		case EChangeDelete:
			{	
			OstTrace1( TRACE_NORMAL, DUP1_CNOTESPLUGIN_HANDLENOTECHANGEDENTRYL, "CNotesPlugin::HandleNoteChangedEntryL;Monitored delete id=%d", changedEntry.iEntryId );
			CPIXLOGSTRING2("CNotesPlugin::HandleNoteChangedEntryL(): Monitored delete id=%d.", changedEntry.iEntryId);
			if( iIndexState )
			    CreateNoteEntryL( changedEntry.iEntryId, ECPixRemoveAction );
			else
			    OverWriteOrAddToQueueL(changedEntry.iEntryId, ECPixRemoveAction);
			break;
			}

		case EChangeModify:
			{
			OstTrace1( TRACE_NORMAL, DUP2_CNOTESPLUGIN_HANDLENOTECHANGEDENTRYL, "CNotesPlugin::HandleNoteChangedEntryL;Monitored update id=%d", changedEntry.iEntryId );
			CPIXLOGSTRING2("CNotesPlugin::HandleNoteChangedEntryL(): Monitored update id=%d.", changedEntry.iEntryId);
			if( iIndexState )
			    CreateNoteEntryL( changedEntry.iEntryId, ECPixUpdateAction );
			else
			    OverWriteOrAddToQueueL(changedEntry.iEntryId, ECPixUpdateAction);
			break;
			}

		case EChangeUndefined:
			{
			OstTrace0( TRACE_NORMAL, DUP3_CNOTESPLUGIN_HANDLENOTECHANGEDENTRYL, "CNotesPlugin::HandleNoteChangedEntryL(): EChangeUndefined." );
			CPIXLOGSTRING("CNotesPlugin::HandleNoteChangedEntryL(): EChangeUndefined.");
			// This event could be related to synchronization.
			// Mark harvesting as cancelled.
			// Remove it from the harvesting queue to cause it to enter 
			// EHarvesterStatusHibernate state.
			// Now add it to the harvesting queue and force a reharvest.

			Flush(*iIndexer);
			iJobQueue.Reset();
			iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), KErrCancel);
			iObserver->RemoveHarvestingQueue(this, iIndexer->GetBaseAppClass());
			iObserver->AddHarvestingQueue(this, iIndexer->GetBaseAppClass(), ETrue);
			break;
			}
		default:
			// Ignore other events
			break;
		}	
	}
	
// ---------------------------------------------------------------------------
// CNotesPlugin::CreateEntryL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::CreateNoteEntryL( const TCalLocalUid& aLocalUid, TCPixActionType aActionType )
	{
	if (!iIndexer)
    	return;

	OstTrace1( TRACE_NORMAL, CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL;uid=%d", aLocalUid );
	CPIXLOGSTRING2("CNotesPlugin::CreateNoteEntryL():  Uid = %d.", aLocalUid);
	
	
	// create CSearchDocument object with unique ID for this application
	TBuf<20> docid_str;
	docid_str.AppendNum(aLocalUid);
	
	if (aActionType == ECPixAddAction || aActionType == ECPixUpdateAction)
		{
        // Return the entry that has been fetched, this will be NULL if there are 
	    // no entries with the cal unique id
	    CCalEntry* entry = iEntryView->FetchL(aLocalUid);
	    if ( entry == NULL)  return;
	    CleanupStack::PushL(entry);
	    
	    if( CCalEntry::ENote != entry->EntryTypeL() )
	        {
            OstTrace0( TRACE_NORMAL, DUP1_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL(): return as not a Note entry." );
            CPIXLOGSTRING("CNotesPlugin::CreateNoteEntryL(): return as not a Note entry.");
	        CleanupStack::PopAndDestroy(entry);
	        return;
	        }
	    OstTrace0( TRACE_NORMAL, DUP4_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::Indexing Notes" );
	    
	    OstTrace0( TRACE_NORMAL, DUP2_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL(): Creating document." );
	    CPIXLOGSTRING("CNotesPlugin::CreateNoteEntryL(): Creating document.");
		CSearchDocument* index_item = CSearchDocument::NewLC(docid_str, _L(NOTESAPPCLASS));
		// Add Description fields		
		index_item->AddFieldL(KNpdMemo, entry->DescriptionL());
		// Add Date fields
		TBuf<30> dateString;
		TDateTime datetime = entry->EndTimeL().TimeLocalL().DateTime();		
		dateString.Format( KNotesTimeFormat, datetime.Year(),
                                     TInt(datetime.Month()+ 1),
                                     datetime.Day() + 1,
                                     datetime.Hour(),
                                     datetime.Minute());
		 
		index_item->AddFieldL(KNpdUpdateTime, dateString, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);

		index_item->AddFieldL(KMimeTypeField, KMimeTypeNotes, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);

		//For notes, no content is expected in excerpt for now.
		//See appclass-hierarchy.txt for details.
//#ifdef USE_HIGHLIGHTER
		_LIT(KExcerptTimeFormat,"%04d/%02d/%02d %02d:%02d");
		index_item->AddHLDisplayFieldL(entry->DescriptionL());
		
		dateString.Format( KExcerptTimeFormat, datetime.Year(),
		                                     TInt(datetime.Month()+ 1),
		                                     datetime.Day() + 1,
		                                     datetime.Hour(),
		                                     datetime.Minute());
        index_item->AddExcerptL(dateString);
    
//#endif      
		
		// Send for indexing
		if (aActionType == ECPixAddAction)
			{
#ifndef _DEBUG
            TRAP_IGNORE( iIndexer->AddL(*index_item) );
#else
			TRAPD( err, iIndexer->AddL(*index_item) );
			OstTrace1( TRACE_NORMAL, DUP9_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL : iIndexer->AddL;err=%d", err );
#endif
			}
		else
			{
#ifndef _DEBUG
		TRAP_IGNORE( iIndexer->UpdateL(*index_item) );
#else
			TRAPD( err, iIndexer->UpdateL(*index_item) );
			OstTrace1( TRACE_NORMAL, DUP10_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL: iIndexer->UpdateL;err=%d", err );
#endif
			}
		CleanupStack::PopAndDestroy(index_item);
		CleanupStack::PopAndDestroy(entry);
		}
	else if (aActionType == ECPixRemoveAction)
		{
#ifndef _DEBUG
        TRAP_IGNORE( iIndexer->DeleteL(docid_str) );
#else
		TRAPD( err, iIndexer->DeleteL(docid_str) );
		OstTrace1( TRACE_NORMAL, DUP3_CNOTESPLUGIN_CREATENOTEENTRYL, "CNotesPlugin::CreateNoteEntryL: iIndexer->RemoveL;err=%d", err );
#endif
		}	
	}

// ---------------------------------------------------------------------------
// CNotesPlugin::InitTimeValuesL
// ---------------------------------------------------------------------------
//  
void CNotesPlugin::InitTimeValuesL( TTime& aStartTime, TTime& aEndTime )
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_INITTIMEVALUESL_ENTRY );
    CPIXLOGSTRING("CNotesPlugin::InitTimeValuesL: Enter");
    //Open the cpix common repository
    CRepository* cpixrepo = CRepository::NewL( KCPIXrepoUidMenu );    
    TBuf<KDateStringLength> temp;
    //Read the Start date from the cenrep
    TInt error = cpixrepo->Get( KStartDateKey , temp);
    //Parse the date and update sStartTime
    TInt day,month,year;
    TLex svalue(temp);
    svalue.Val(day);
    svalue.Inc(1);
    svalue.Val(month);
    svalue.Inc(1);
    svalue.Val(year);
    TDateTime startdate(year,(TMonth)(month-1),day , 0 , 0 , 0 , 0);
    aStartTime = startdate;
    //Read the end date from the cenrep
    error = cpixrepo->Get( KEndDateKey , temp);
    //Parse the date and update aEndTime
    TLex evalue(temp);
    evalue.Val(day);
    evalue.Inc(1);
    evalue.Val(month);
    evalue.Inc(1);
    evalue.Val(year);
    TDateTime enddate(year,(TMonth)(month-1),day , 0 , 0 , 0 , 0);
    aEndTime = enddate;
    CPIXLOGSTRING2("CNotesPlugin::InitTimeValuesL: Exit with Error = %d", error);     
    OstTraceFunctionExit0( CNOTESPLUGIN_INITTIMEVALUESL_EXIT );
    }
// ---------------------------------------------------------------------------
// CNotesPlugin::PausePluginL
// ---------------------------------------------------------------------------
//
void CNotesPlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_PAUSEPLUGINL_ENTRY );
    iIndexState = EFalse;
    OstTraceFunctionExit0( CNOTESPLUGIN_PAUSEPLUGINL_EXIT );
    }
// ---------------------------------------------------------------------------
// CNotesPlugin::ResumePluginL
// ---------------------------------------------------------------------------
//
void CNotesPlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_RESUMEPLUGINL_ENTRY );
    iIndexState = ETrue;
            
    if(iHarvestState == EHarvesterStartHarvest)
        {
        if(iAsynchronizer->CallbackPending())
            iAsynchronizer->CancelCallback();
        iAsynchronizer->Start( 0, this, KHarvestingDelay );
        }
    else
        {
        IndexQueuedItems();
        }
    OstTraceFunctionExit0( CNOTESPLUGIN_RESUMEPLUGINL_EXIT );
    }
// ---------------------------------------------------------------------------
// CNotesPlugin::OverWriteOrAddToQueueL
// ---------------------------------------------------------------------------
//
void CNotesPlugin::OverWriteOrAddToQueueL(const TCalLocalUid& aLocalUid, TCPixActionType aActionType)
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_OVERWRITEORADDTOQUEUEL_ENTRY );
    TRecord entry;    
    for (TInt i=0; i<iJobQueue.Count(); i++)
        {
            if (iJobQueue[i].iLocalUid == aLocalUid)
            {
                // Older version found
                iJobQueue[i].iLocalUid = aLocalUid;
                iJobQueue[i].iActionType = aActionType;
                OstTraceFunctionExit0( CNOTESPLUGIN_OVERWRITEORADDTOQUEUEL_EXIT );
                return;
            }
        }        
    // older not found, append
    entry.iActionType = aActionType;
    entry.iLocalUid = aLocalUid;
    iJobQueue.AppendL(entry);
    OstTraceFunctionExit0( DUP1_CNOTESPLUGIN_OVERWRITEORADDTOQUEUEL_EXIT );
    }
// ---------------------------------------------------------------------------
// CNotesPlugin::IndexQueuedItems
// ---------------------------------------------------------------------------
//
void CNotesPlugin::IndexQueuedItems()
    {
    OstTraceFunctionEntry0( CNOTESPLUGIN_INDEXQUEUEDITEMS_ENTRY );
    while (iJobQueue.Count()>0)
        {
        TRecord entry = iJobQueue[0];        
        //Let the indexer handle this object TRAP it as it can leave
        TRAPD(err,CreateNoteEntryL( entry.iLocalUid, entry.iActionType ));        
        if(KErrNone == err)
            {
            iJobQueue.Remove(0);
            }
        }
    OstTraceFunctionExit0( CNOTESPLUGIN_INDEXQUEUEDITEMS_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CNotesPlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\NotesPerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\NotesPerf.txt"), EFileWrite));
    
    HBufC8 *heap = HBufC8::NewL(100);
    TPtr8 ptr = heap->Des();
    now.HomeTime();
    TBuf<50> timeString;             
                
    _LIT(KOwnTimeFormat,"%:0%H%:1%T%:2%S");
    now.FormatL(timeString,KOwnTimeFormat);
    ptr.AppendNum(now.DateTime().Day());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Month());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Year());
    ptr.Append(_L(":"));
    ptr.Append(timeString);
    ptr.Append( _L(":Ani: Time took for Harvesting Notes is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }
#endif
// End of file
