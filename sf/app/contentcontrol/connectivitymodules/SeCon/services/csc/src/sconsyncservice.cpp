/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConSyncService implementation.
*
*/


// Includes
#include <centralrepository.h>
#include <seconsdkcrkeys.h>

#include <swi/sisregistryentry.h>   // for searching app name from installer
#include <swi/sisregistrypackage.h>
#include <swi/sisregistrysession.h>
#include <javaregistryincludes.h>
using namespace Java;

#include "sconsyncservice.h"
#include "capability.h"
#include "debug.h"

// Constants
const TInt KDsAppUi = 0x101F6DE4; // NSmlDSSync.exe
const TSmlDataProviderId KCalendarId = 0x101f6dde;
const TSmlDataProviderId KContactsId = 0x101f6ddd;
const TSmlDataProviderId KSmsId = 0x10206b5c;
const TSmlDataProviderId KBookmarkId = 0x102751ba;

const TInt KDateFormatLength(16);

// -----------------------------------------------------------------------------
// CSConSyncService::NewLC()
// -----------------------------------------------------------------------------
//
CSConSyncService* CSConSyncService::NewLC()
	{
	TRACE_FUNC_ENTRY;
	CSConSyncService* self = new (ELeave) CSConSyncService();
	CleanupStack::PushL( self );
	self->ConstructL();
	TRACE_FUNC_EXIT;
	return self;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::CSConSyncService()
// -----------------------------------------------------------------------------
//
CSConSyncService::CSConSyncService()
	{
	}

// -----------------------------------------------------------------------------
// CSConSyncService::~CSConSyncService()
// -----------------------------------------------------------------------------
//
CSConSyncService::~CSConSyncService()
	{
	TRACE_FUNC_ENTRY;
	if ( iApaSessionConnected )
		{
		iApaSession.Close();
		}
	if ( iSyncSessionOpened )
		{
		iSyncSession.Close();
		}
	delete iRepository;
	TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::ConstructL()
// -----------------------------------------------------------------------------
//
void CSConSyncService::ConstructL()
	{
	iRepository = CRepository::NewL( KCRUidSecon );
	}

// -----------------------------------------------------------------------------
// CSConSyncService::GetSolutionL()
// Populate SolutionInfo according to content type.
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetSolutionL( const TUint aSeconContentType,
		TSConSolutionInfo& aSolutionInfo )
	{
	TRACE_FUNC_ENTRY;
	TInt appUid;
	User::LeaveIfError( iRepository->Get( aSeconContentType, appUid ));
    LOGGER_WRITE_2( "repository readed ok, uid: %d, hex:%x", appUid, appUid );
    
    aSolutionInfo.iUid = appUid;
    if ( aSolutionInfo.iUid == KDsAppUi )
        {
        // read profile name
        LOGGER_WRITE( "Search correct profile" );
        GetLastUsedProfileNameL( aSolutionInfo.iSolutionName,
        	aSolutionInfo.iTime, ProviderId( aSeconContentType ) );
        }
    else if ( aSolutionInfo.iUid != 0 )
        {
        LOGGER_WRITE( "Search correct application name" );
        TUid appUid = {aSolutionInfo.iUid};
        GetApplicationCaptionL( aSolutionInfo.iSolutionName, appUid );
        
        // get solution timestamp
        TBuf<KDateFormatLength> timeBuffer;
        // Timestamp is stored always in next to Uid value on cenrep
        TInt err = iRepository->Get( aSeconContentType+1, timeBuffer );
        
        LOGGER_WRITE_1( "iRepository->Get(time) err: %d", err );
        if ( err == KErrNone )
        	{
	        err = ParseTimestamp( aSolutionInfo.iTime, timeBuffer );
	        LOGGER_WRITE_1( "ParseTimestamp err: %d", err );
        	}
        }
    TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::ProviderId()
// Return correct providerId according to content type.
// -----------------------------------------------------------------------------
//
TSmlDataProviderId CSConSyncService::ProviderId( const TUint32 aSeconContentType ) const
	{
	TSmlDataProviderId providerId(KErrNotFound);
	switch (aSeconContentType)
		{
		case KSeconCalendarUid:
		providerId = KCalendarId;
		break;
		case KSeconContactsUid:
		providerId = KContactsId;
		break;
		case KSeconSmsUid:
		providerId = KSmsId;
		break;
		case KSeconBookmarkUid:
		providerId = KBookmarkId;
		break;
		default:
		LOGGER_WRITE_1( "Invalid content type: %d", aSeconContentType );
		}
	return providerId;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::ParseTimestamp()
// Parses timestamp to TTime format
// -----------------------------------------------------------------------------
//
TInt CSConSyncService::ParseTimestamp( TTime& aTime, const TDesC& aTimestamp ) const
	{
	// timestamp must be in following format:
	// YYYYMMDDTHHMMSSZ
	// MM=1..12, DD=1..number of days in month, HH=0..23, MM;SS=0..59
	// Example 20071224T162215Z = 24.12.2007, 16:22:15 UTC time
	
	if ( aTimestamp.Length() != KDateFormatLength )
		{
		LOGGER_WRITE_1( "aTimestamp.Length(): %d", aTimestamp.Length() );
		return KErrArgument;
		}
	
	TPtrC temp;
	temp.Set(aTimestamp.Ptr());
	
	TLex lex(temp.Mid(0,4));
	TInt year;
	TInt month;
	TInt day;
	TInt hour;
	TInt minute;
	TInt second;
	TInt err = lex.Val( year );
	if (err != KErrNone) return err;
	
	lex = temp.Mid(4,2);
	err = lex.Val( month );
	if ( err != KErrNone) return err;
	month--;
	
	lex = temp.Mid(6,2);
	err = lex.Val( day );
	if (err != KErrNone) return err;
	day--;
	
	lex = temp.Mid(9,2);
	err = lex.Val( hour );
	if (err != KErrNone) return err;
	
	lex = temp.Mid(11,2);
	err = lex.Val( minute );
	if (err != KErrNone) return err;
	
	lex = temp.Mid(13,2);
	err = lex.Val( second );
	if (err != KErrNone) return err;
	
	TDateTime parsedTime;
	err = parsedTime.Set(year, (TMonth)month, day, hour, minute, second, 0);
	if (err != KErrNone) return err;
	
	aTime = parsedTime;
	return err;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::GetApplicationCaptionL()
// Get application caption.
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetApplicationCaptionL( TDes& aCaption, const TUid aAppUid )
    {
    TRACE_FUNC_ENTRY;
    
    TInt err(KErrNotReady);
    // connect to RApaLsSession, if not yet ready
    if ( !iApaSessionConnected )
    	{
    	err = iApaSession.Connect();
    	if ( err == KErrNone )
    		{
    		iApaSessionConnected = ETrue;
    		}
    	}
    
    // Get application caption using RApaLsSession::GetAppInfo
	TApaAppInfo appInfo;
	if ( iApaSessionConnected )
		{
		err = iApaSession.GetAppInfo( appInfo ,aAppUid );
		}
    
    LOGGER_WRITE_1( "GetAppInfo err: %d", err );
    if ( err == KErrNone )
    	{
    	aCaption = appInfo.iCaption;
    	}
    else
        {
        // Caption does not exist, search application name from installer
        GetInstPackageCaptionL( aCaption, aAppUid );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSyncService::GetInstPackageCaptionL()
// Search UID from installed applications, return app caption
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetInstPackageCaptionL( TDes& aCaption, const TUid aPackageUid )
	{
	TRACE_FUNC_ENTRY;
    LOGGER_WRITE("Name not found, search from installer");
    // Search application name from Installer.
    Swi::RSisRegistrySession sisRegistry;
    User::LeaveIfError( sisRegistry.Connect() );
    CleanupClosePushL( sisRegistry );
    
    //Check if uid belongs to SIS package
    if( sisRegistry.IsInstalledL( aPackageUid ) )
        {
        LOGGER_WRITE("SIS package found");
        Swi::RSisRegistryEntry entry;
        CleanupClosePushL(entry);
        User::LeaveIfError( entry.Open( sisRegistry, aPackageUid ) );
        aCaption.Copy( *entry.PackageNameL() );
        CleanupStack::PopAndDestroy( &entry );
        }
    else
        {
        // not sis app, might be java app.
        LOGGER_WRITE("SIS package not found");
        CJavaRegistry* javaRegistry = CJavaRegistry::NewLC( );
        TBool entryExist = javaRegistry->RegistryEntryExistsL( aPackageUid );
        if ( entryExist )
            {
            CJavaRegistryEntry* entry = javaRegistry->RegistryEntryL( aPackageUid );
            if ( entry )
                {
                aCaption.Copy( entry->Name() );
                delete entry;
                }
            }
        
        CleanupStack::PopAndDestroy( javaRegistry ); 
        }
    
    CleanupStack::PopAndDestroy( &sisRegistry );
    TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::GetLastUsedProfileNameL()
// Gets last used profile for selected provider
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetLastUsedProfileNameL( TDes& aProfileName, TTime& aTime,
		const TSmlDataProviderId& aDataProviderId )
    {
    TRACE_FUNC_ENTRY;
    if ( !iSyncSessionOpened )
		{
		iSyncSession.OpenL();
		iSyncSessionOpened = ETrue;
		}
    RArray<TSmlProfileId> profiles;
    CleanupClosePushL( profiles );
    iSyncSession.ListProfilesL(profiles, ESmlDataSync );
    TSmlProfileId lastSyncProfile(KErrNotFound);
    TTime lastSyncTime(0);
    LOGGER_WRITE_1( "Profiles count: %d", profiles.Count() );
    for (TInt i=0; i<profiles.Count(); i++)
    	{
    	TTime syncTime(0);
    	GetLastSyncTimeL( syncTime, profiles[i], aDataProviderId );
    	if ( syncTime.Int64() != 0 && syncTime > lastSyncTime )
    		{
    		LOGGER_WRITE_1( "New record, profile: %d",profiles[i]);
    		lastSyncTime = syncTime;
    		lastSyncProfile = profiles[i];
    		}
    	}
    CleanupStack::PopAndDestroy( &profiles );
    LOGGER_WRITE_1( "lastSyncProfile: %d", lastSyncProfile );
    if ( lastSyncProfile != KErrNotFound )
    	{
    	aTime = lastSyncTime;
    	RSyncMLDataSyncProfile syncProfile;
    	syncProfile.OpenL( iSyncSession, lastSyncProfile, ESmlOpenRead );
    	TPtrC displayName = syncProfile.DisplayName();
    	aProfileName.Copy( displayName );
    	syncProfile.Close();
    	}
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSyncService::GetLastSyncTimeL()
// Gets last sync time for certain profile and provider.
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetLastSyncTimeL( TTime& aTime, const TSmlProfileId aSyncProfileId,
		const TSmlDataProviderId aDataProviderId  )
	{
	TRACE_FUNC_ENTRY;
	LOGGER_WRITE_1( "aSyncProfileId: %d", aSyncProfileId );
	LOGGER_WRITE_1( "aDataProviderId: 0x%08x", aDataProviderId );
	if ( !iSyncSessionOpened )
		{
		LOGGER_WRITE("iSyncSession is not opened, Leave!");
		User::Leave( KErrNotReady );
		}
	
	RSyncMLDataSyncProfile syncProfile;
	syncProfile.OpenL( iSyncSession, aSyncProfileId, ESmlOpenRead );
    CleanupClosePushL( syncProfile );
    LOGGER_WRITE( "syncProfile opened OK" );
    
    TSmlTaskId providerTask;
    GetProviderTaskL( providerTask, syncProfile, aDataProviderId  );
    if ( providerTask != KErrNone )
    	{
        RSyncMLHistoryLog historyLog;
        historyLog.OpenL( iSyncSession, aSyncProfileId );
        CleanupClosePushL( historyLog );
        historyLog.SortEntries(CSyncMLHistoryEntry::ESortByTime);
        LOGGER_WRITE_1( "historyLog.Count: %d", historyLog.Count() );
        const CSyncMLHistoryJob* historyJob = LatestHistoryJob( historyLog, providerTask );
        if ( historyJob )
            {
            aTime = historyJob->TimeStamp();
#ifdef _DEBUG
            TDateTime timeStamp = historyJob->TimeStamp().DateTime();
            _LIT(KDateFormat, " (%04d%02d%02dT%02d%02d%02d)");
            const TInt KDateFormatLength(20);
            TBuf<KDateFormatLength> tempdate;
            tempdate.Format(KDateFormat, timeStamp.Year(), timeStamp.Month()+1, timeStamp.Day()+1,
                    timeStamp.Hour(), timeStamp.Minute(), timeStamp.Second());
            LOGGER_WRITE_1("history found, TimeStamp: %S", &tempdate);
#endif
            }
    	
        CleanupStack::PopAndDestroy( &historyLog );
    	}
    CleanupStack::PopAndDestroy( &syncProfile );
    TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::GetProviderTaskL()
// Gets provider task id from profile
// -----------------------------------------------------------------------------
//
void CSConSyncService::GetProviderTaskL( TSmlTaskId& aProviderTask,
		RSyncMLDataSyncProfile& aSyncProfile,
		const TSmlDataProviderId aDataProviderId  ) const
	{
	TRACE_FUNC_ENTRY;
	aProviderTask = KErrNotFound;
	
	RArray<TSmlTaskId> tasks;
	CleanupClosePushL( tasks );
	
	aSyncProfile.ListTasksL( tasks );
	LOGGER_WRITE_1( "tasks.Count: %d", tasks.Count() );
	
	for ( TInt i=0; i<tasks.Count() && aProviderTask==KErrNotFound; i++ )
		{
		RSyncMLTask syncTask;
	    syncTask.OpenL( aSyncProfile, tasks[i] );
	    if ( syncTask.DataProvider() == aDataProviderId )
	        {
	        LOGGER_WRITE_1( "dataprovider task found, taskId: %d", tasks[i] );
	        aProviderTask = tasks[i];
	        }
	    syncTask.Close();
		}
	
	CleanupStack::PopAndDestroy( &tasks );
	TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// CSConSyncService::LatestHistoryJob()
// Gets latest historyJob where task is correct.
// -----------------------------------------------------------------------------
//
 const CSyncMLHistoryJob* CSConSyncService::LatestHistoryJob(
		 RSyncMLHistoryLog& aHistoryLog, TInt aTaskId ) const
    {
    TRACE_FUNC_ENTRY;
    
    TInt count = aHistoryLog.Count();
    if ( count == 0 )
        {
        LOGGER_WRITE( "profile has no history job" );
        TRACE_FUNC_EXIT;
        return NULL;  // profile has no history job
        }

    aHistoryLog.SortEntries(CSyncMLHistoryEntry::ESortByTime);    

    // try to find latest sync job (start from last array entry)

    for ( TInt i=count-1; i>=0; i-- )
        {
        const CSyncMLHistoryEntry& entry = aHistoryLog[i];
        const CSyncMLHistoryJob* jobEntry = CSyncMLHistoryJob::DynamicCast(&entry);
        if ( jobEntry )
            {
            if ( TaskExist(jobEntry, aTaskId) )
                {
                LOGGER_WRITE( "history job found" );
                TRACE_FUNC_EXIT;
                return jobEntry;
                }
            }
        }
    LOGGER_WRITE( "history job not found" );
    TRACE_FUNC_EXIT;
    return NULL;
    }

 // -----------------------------------------------------------------------------
 // CSConSyncService::TaskExist()
 // Returns ETrue if task exists in HistoryJob.
 // -----------------------------------------------------------------------------
 //
TBool CSConSyncService::TaskExist( const CSyncMLHistoryJob* aHistoryJob, TInt aTaskId ) const
    {
    TRACE_FUNC_ENTRY;
    TInt taskCount = aHistoryJob->TaskCount();
    for ( TInt i=0; i<taskCount; i++ )
        {
        const CSyncMLHistoryJob::TTaskInfo& taskInfo = aHistoryJob->TaskAt(i);

        if ( taskInfo.iTaskId == aTaskId )
            {
            LOGGER_WRITE( "task exists." );
            TRACE_FUNC_EXIT;
            return ETrue;
            }
        }
    TRACE_FUNC_EXIT;
    return EFalse;
    }

