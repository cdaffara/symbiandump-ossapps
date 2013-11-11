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


#include <e32base.h> 
#include <msvids.h>
#include <msvuids.h>
#include <mmsconst.h>
#include <mmsclient.h>
#include <smsclnt.h>
#include <txtrich.h>
#include <smuthdr.h>
#include <smut.h>
#include <miutset.h>
#include <SendUiConsts.h>
#include <mtclreg.h>
#include <gsmuelem.h>
#include <msventry.h>
#include <ccpixindexer.h>

#include "cmessageplugin.h"
#include "cmessagemonitor.h"
#include "cmessageharvester.h"
#include "cmessagedatahandler.h"
#include "harvesterserverlogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmessagepluginTraces.h"
#endif


// maximum length that the fully qualified msg Plugin base app class descriptor can be
// e.g. "@c:root msg"
const TInt KMsgPluginBaseAppClassMaxLen = 64;


// local declarations and functions
namespace {

_LIT(KCPixSearchServerPrivateDirectory, "\\Private\\2001f6f7\\");
_LIT(KPathIndexDbPath, CPIX_INDEVICE_INDEXDB);

_LIT(KPathTrailer, "\\root\\msg\\smsmms");
_LIT(KMsgBaseAppClassGeneric, ":root msg smsmms");
_LIT(KAtSign, "@");
_LIT(KColon, ":");

/**
* MapFileToDrive - gets the TDriveNumber associated with the aBaseAppClass.
* @aBaseAppClass e.g. "@c:root file"
* @aDrive returns the TDriveNumber for the aBaseAppClass
* returns KErrNone on success or a standard error code
*/
TInt MapBaseAppClassToDrive(const TDesC& aBaseAppClass, TDriveNumber& aDrive)
    {
    if (KErrNone != aBaseAppClass.Left(1).Compare(KAtSign))
        {
        return KErrGeneral;
        }

    TPtrC drvChar = aBaseAppClass.Left(2).Right(1);
    TChar d(drvChar[0]);
    TInt drive;
    const TInt ret = RFs::CharToDrive(d, drive);
    if (!ret)
        {
        aDrive = TDriveNumber(drive);
        }

    return ret;
    }

}// anonymous namespace


// ---------------------------------------------------------------------------
// CMessagePlugin::NewL
// ---------------------------------------------------------------------------
//  
CMessagePlugin* CMessagePlugin::NewL()
{
	CMessagePlugin* instance = CMessagePlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
}

// ---------------------------------------------------------------------------
// CMessagePlugin::NewLC
// ---------------------------------------------------------------------------
//  
CMessagePlugin* CMessagePlugin::NewLC()
{
	CMessagePlugin* instance = new (ELeave)CMessagePlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
}

// ---------------------------------------------------------------------------
// CMessagePlugin::CMessagePlugin
// ---------------------------------------------------------------------------
//  
CMessagePlugin::CMessagePlugin()
{
	iMessageDataHandler = NULL;
	iMessageHarvester = NULL;
	iMessageMonitor = NULL;
	iMsvSession = NULL;
}

// ---------------------------------------------------------------------------
// CMessagePlugin::~CMessagePlugin
// ---------------------------------------------------------------------------
//  
CMessagePlugin::~CMessagePlugin()
{
    // remove notification paths before destroying iMonitor
    for (TInt i=EDriveA; i<=EDriveZ; i++)
        UnMount(TDriveNumber(i), EFalse);
    iMountDrives.Close();    
	delete iMessageDataHandler;
	delete iMessageHarvester;	
	delete iMessageMonitor;	
	delete iMsvSession;
	iFs.Close();
}

// ---------------------------------------------------------------------------
// CMessagePlugin::ConstructL
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::ConstructL()
{
    iIndexState = ETrue;
    TInt err = iFs.Connect();
    OstTrace1( TRACE_NORMAL, CMESSAGEPLUGIN_CONSTRUCTL, "CMessagePlugin::ConstructL;iFs Connect Error=%d", err );
    CPIXLOGSTRING2("CMessagePlugin::ConstructL, iFs.Connect: %i", err);
    User::LeaveIfError(err);
    iCurrentDrive = EDriveC; //Default drive is C drive
    iMsvSession = CMsvSession::OpenSyncL(*this);
    iMessageMonitor = CMessageMonitor::NewL( *this, *iMsvSession );    
    iMessageHarvester = CMessageHarvester::NewL( *this, *iMsvSession );    
    iMessageDataHandler = CMessageDataHandler::NewL( *this, *iMsvSession );    
    for (TInt i=EDriveA; i<=EDriveZ; i++)
        {
        iIndexer[i] = NULL; //Initialize to NULL
        }
}

// ---------------------------------------------------------------------------
// CMessagePlugin::StartPluginL
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::StartPluginL()
	{
	//Get the current Drive for storage
	iCurrentDrive = iMsvSession->CurrentDriveL();
	OstTrace1( TRACE_NORMAL, CMESSAGEPLUGIN_STARTPLUGINL, "CMessagePlugin::StartPluginL;Current Drive for Messaging =%d", iCurrentDrive );
	CPIXLOGSTRING2("currentDrive from messaging app : %d", iCurrentDrive );
	MountL(TDriveNumber(iCurrentDrive)); //Mount current drive
	// Define this base application class, use default location
	}

// ---------------------------------------------------------------------------
// CMessagePlugin::StartHarvestingL
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::StartHarvestingL(const TDesC& aQualifiedBaseAppClass)
	{
	OstTraceFunctionEntry0( CMESSAGEPLUGIN_STARTHARVESTINGL_ENTRY );
	CPIXLOGSTRING("START CMessagePlugin::StartHarvestingL");
	// Map base app class to a drive number
	TDriveNumber drive ( EDriveA );//Initialize to silence compiler warnings.
	if (KErrNone != MapBaseAppClassToDrive(aQualifiedBaseAppClass, drive))
	   User::Leave(KErrGeneral);

    // Leave if no indexer for this drive
	if (!iIndexer[drive])
	   User::Leave(KErrGeneral);
	        
    // Reset the database
	iIndexer[drive]->ResetL();
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
    if( iIndexState )
        {
        iMessageHarvester->StartHarvestingL();
        }
    else
        {
        iMountDrives.Append(TDriveNumber(drive));
        }
	OstTraceFunctionExit0( CMESSAGEPLUGIN_STARTHARVESTINGL_EXIT );
	}

// ---------------------------------------------------------------------------
// CMessagePlugin::MountL
// ---------------------------------------------------------------------------
//
void CMessagePlugin::MountL(TDriveNumber aMedia,TBool aForceReharvesting)
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_MOUNTL_ENTRY );
    CPIXLOGSTRING("START CMessagePlugin::MountL");
    // Check if already exists
    if (iIndexer[aMedia])
        return;
    
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(aMedia, baseAppClass);

    // Define this volume
    HBufC* path = DatabasePathLC(aMedia);
    User::LeaveIfError(iSearchSession.DefineVolume(baseAppClass, *path));
    CleanupStack::PopAndDestroy(path);
    
    // construct and open the database
    TRAPD(err,iIndexer[aMedia] = CCPixIndexer::NewL(iSearchSession));
    OstTrace1( TRACE_NORMAL, CMESSAGEPLUGIN_MOUNTL, "CMessagePlugin::MountL;Indexer NewL return error=%d", err );
    CPIXLOGSTRING2("CCPixIndexer::NewL returned : %d", err );
    TRAP(err,iIndexer[aMedia]->OpenDatabaseL(baseAppClass));

    // Add to harvesting queue
    iObserver->AddHarvestingQueue(this, baseAppClass,aForceReharvesting);  
    CPIXLOGSTRING("END CMessagePlugin::MountL");
    OstTraceFunctionExit0( CMESSAGEPLUGIN_MOUNTL_EXIT );
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::UnMount
// ---------------------------------------------------------------------------
//
void CMessagePlugin::UnMount(TDriveNumber aMedia, TBool aUndefineAsWell)
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_UNMOUNT_ENTRY );
    CPIXLOGSTRING("START CMessagePlugin::UnMount");
    // Check if already exists
    if (!iIndexer[aMedia])
        return;
    
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(aMedia, baseAppClass);             
        
    // Remove from harvesting queue
    iObserver->RemoveHarvestingQueue(this, baseAppClass);   
    
    // Delete the index object
    delete iIndexer[aMedia];
    iIndexer[aMedia] = NULL;
    
    // if the aActionType is EFFMmcDismount, then the
    // parameter aFilename is the baseAppClass of the Index database
    // to be dropped.
    if (aUndefineAsWell)
        iSearchSession.UnDefineVolume(baseAppClass);
    OstTraceFunctionExit0( CMESSAGEPLUGIN_UNMOUNT_EXIT );
    }


// ---------------------------------------------------------------------------
// CMessagePlugin::FormBaseAppClass
// ---------------------------------------------------------------------------
//  
TInt CMessagePlugin::FormBaseAppClass(TDriveNumber aMedia, TDes& aBaseAppClass)
    {
    TChar chr;
    const TInt ret = RFs::DriveToChar(aMedia, chr);
    if (KErrNone == ret)
        {
        aBaseAppClass.Copy(KAtSign);
        aBaseAppClass.Append(chr);
        aBaseAppClass.LowerCase();
        aBaseAppClass.Append(KMsgBaseAppClassGeneric);
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::DatabasePathLC
// ---------------------------------------------------------------------------
//
HBufC* CMessagePlugin::DatabasePathLC(TDriveNumber aMedia)
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_DATABASEPATHLC_ENTRY );
    CPIXLOGSTRING("START CMessagePlugin::DatabasePathLC");
    // Allocate extra space for root path e.g. "C:\\Private\\2001f6f7\\"
    const TInt KRootPathMaxLength = 30;
    HBufC* indexDbPath = HBufC::NewLC(KRootPathMaxLength + KPathIndexDbPath().Length() + KPathTrailer().Length());
    TPtr indexDbPathPtr = indexDbPath->Des();

#if 1 // Data caging implementation
    iFs.CreatePrivatePath(aMedia);

    TChar chr;
    RFs::DriveToChar(aMedia, chr);
    indexDbPathPtr.Append(chr);
    indexDbPathPtr.Append(KColon);

    TFileName pathWithoutDrive;
    iFs.PrivatePath(pathWithoutDrive);
    indexDbPathPtr.Append(KCPixSearchServerPrivateDirectory);
#else // here is the way to calculate the path if data caging is not being used.
    TFileName rootPath;
    PathInfo::GetRootPath(rootPath, aMedia);
    indexDbPathPtr.Append(rootPath);
#endif 

    indexDbPathPtr.Append(KPathIndexDbPath);
    indexDbPathPtr.Append(KPathTrailer);

    OstTraceFunctionExit0( CMESSAGEPLUGIN_DATABASEPATHLC_EXIT );
    return indexDbPath;
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::GetIndexer
// ---------------------------------------------------------------------------
//
CCPixIndexer* CMessagePlugin::GetIndexer()
    {
    return iIndexer[iCurrentDrive];
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::IsMediaRemovableL
// ---------------------------------------------------------------------------
// 
TBool CMessagePlugin::IsMediaRemovableL(TDriveNumber& aDrive)
    {
    TDriveInfo driveInfo; 
    User::LeaveIfError(iFs.Drive(driveInfo,aDrive));
    TBool mediaRemovable(EFalse);
    driveInfo.iDriveAtt & KDriveAttRemovable ? mediaRemovable = ETrue : mediaRemovable = EFalse;
    return mediaRemovable;
    }
// ---------------------------------------------------------------------------
// CMessagePlugin::HandleSessionEventL
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::HandleSessionEventL( TMsvSessionEvent aEvent, 
										  		 TAny* aArg1, 
										  		 TAny* aArg2, 
										  		 TAny* aArg3 )
	{		
	OstTraceFunctionEntry0( CMESSAGEPLUGIN_HANDLESESSIONEVENTL_ENTRY );
	CPIXLOGSTRING("ENTER CMessagePlugin::HandleSessionEventL");
	OstTrace1( TRACE_NORMAL, CMESSAGEPLUGIN_HANDLESESSIONEVENTL, "CMessagePlugin::HandleSessionEventL;aEvent=%d", aEvent );
	CPIXLOGSTRING2("TMsvSessionEvent aEvent: %d", aEvent );			
		
	switch( aEvent )
		{
		case EMsvEntriesCreated:
		case EMsvEntriesChanged:
			{
			// Handle changed / created event
            CMsvEntrySelection* selection = reinterpret_cast<CMsvEntrySelection*>(aArg1);
            TMsvId folderId = *( reinterpret_cast<TMsvId*>(aArg2));
#ifdef __PERFORMANCE_DATA
            iStartTime.UniversalTime();
		    iMessageMonitor->HandleMsgCreatedChangedL(*selection, folderId, aEvent);
		    UpdatePerformaceDataL(EMsvEntriesChanged);
#else
		    iMessageMonitor->HandleMsgCreatedChangedL(*selection, folderId, aEvent);
#endif		    
			break;
			}		
		case EMsvEntriesDeleted:
			{
			// Handle deletion event
		    CMsvEntrySelection* selection = reinterpret_cast<CMsvEntrySelection*> ( aArg1 );
#ifdef __PERFORMANCE_DATA
            iStartTime.UniversalTime();
		    iMessageMonitor->HandleMsgDeletedL( *selection );
		    UpdatePerformaceDataL(EMsvEntriesDeleted);
#else
		    iMessageMonitor->HandleMsgDeletedL( *selection );
#endif
			break;
			}
		case EMsvEntriesMoved:
			{
			// track move operations
			// or sent folder
			// ... extract the useful parameters
            if( aArg1 && aArg2 && aArg3 )
                {
                CMsvEntrySelection* selection = reinterpret_cast<CMsvEntrySelection*> ( aArg1 );
                // aArg2 is the TMsvId of the new parent. aArg3 is the TMsvId of 
                // the old parent entry.
                TMsvId newFolderId = *(reinterpret_cast<TMsvId*>(aArg2));
                TMsvId oldFolderId = *(reinterpret_cast<TMsvId*>(aArg3));
#ifdef __PERFORMANCE_DATA
                iStartTime.UniversalTime();
                iMessageMonitor->HandleMsgMovedL(newFolderId, oldFolderId, *selection);
                UpdatePerformaceDataL(EMsvEntriesMoved);
#else
                iMessageMonitor->HandleMsgMovedL(newFolderId, oldFolderId, *selection);
#endif
                }
			break;
			}			
		case EMsvMediaChanged:
		    {
		    //Unmount old media drive
		    OstTrace0( TRACE_NORMAL, DUP1_CMESSAGEPLUGIN_HANDLESESSIONEVENTL, "ENTER CMessagePlugin::HandleSessionEventL EMsvMediaChanged" );
		    CPIXLOGSTRING("ENTER CMessagePlugin::HandleSessionEventL EMsvMediaChanged");
		    TDriveNumber drive = *(reinterpret_cast<TDriveNumber*>(aArg1));
		    UnMount(drive);
		    drive = *(reinterpret_cast<TDriveNumber*>(aArg2));
		    //Check if Media is removable then force reharvesting else keep indexes same
		    MountL(drive,IsMediaRemovableL(drive)); //Mount new media drive for harvesting
		    iCurrentDrive = drive;//Index of current drive
		    }
		    break;
		case EMsvMediaUnavailable:
		    {
		    //No need to handle as when drive will be removed EMsvMediaChanged event
		    //will be received for default media drive can be handled there
		    //Unmount old media drive
		    OstTrace0( TRACE_NORMAL, DUP2_CMESSAGEPLUGIN_HANDLESESSIONEVENTL, "ENTER CMessagePlugin::HandleSessionEventL EMsvMediaUnavailable" );
		    CPIXLOGSTRING("ENTER CMessagePlugin::HandleSessionEventL EMsvMediaUnavailable");
		    TDriveNumber drive = *(reinterpret_cast<TDriveNumber*>(aArg1));
		    UnMount(drive);
		    }
		    break;
		case EMsvMediaAvailable:
			{
			OstTrace0( TRACE_NORMAL, DUP3_CMESSAGEPLUGIN_HANDLESESSIONEVENTL, "ENTER CMessagePlugin::HandleSessionEventL EMsvMediaAvailable" );
			CPIXLOGSTRING("ENTER CMessagePlugin::HandleSessionEventL EMsvMediaAvailable");
			//Removed media drive available again mount media drive and unmount default
			UnMount(TDriveNumber(iCurrentDrive)); //Unmount current drive
			TDriveNumber drive = *(reinterpret_cast<TDriveNumber*>(aArg1));
			MountL(drive,IsMediaRemovableL(drive)); //Mount avilable media drive for harvesting
			}
			break;
		case EMsvServerFailedToStart:			
		case EMsvCloseSession:
		case EMsvServerTerminated:
			{
			User::Panic(_L("CMessagePlugin PANIC"), KErrServerTerminated);
			break;				
			}	
			// Following commented only for decision coverage
//		case EMsvMtmGroupInstalled:
//		case EMsvMtmGroupDeInstalled:
//		case EMsvGeneralError: // not used after v5
//		case EMsvServerReady:
//		case EMsvCorruptedIndexRebuilt:
//		case EMsvMediaIncorrect:
//		case EMsvCorruptedIndexRebuilding:
		default:
			{
			break;
			}							
		}	
	CPIXLOGSTRING("END CMessagePlugin::HandleSessionEventL");	
		
	OstTraceFunctionExit0( CMESSAGEPLUGIN_HANDLESESSIONEVENTL_EXIT );
	}	

// ---------------------------------------------------------------------------
// CMessageDataHandler::CalculateMessageType
// ---------------------------------------------------------------------------
//	    
TMsgType CMessagePlugin::CalculateMessageType (const TMsvEntry& aEntry )
{
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_CALCULATEMESSAGETYPE_ENTRY );
    CPIXLOGSTRING("ENTER CMessagePlugin::CalculateMessageType");
	TMsgType ret = EMsgTypeInvalid;
	OstTrace1( TRACE_NORMAL, CMESSAGEPLUGIN_CALCULATEMESSAGETYPE, "CMessagePlugin::CalculateMessageType;uid=%x", aEntry.iMtm.iUid );
	CPIXLOGSTRING2("CMessagePlugin::CalculateMessageType uid: %x", aEntry.iMtm.iUid );
    if( aEntry.iMtm.iUid  == KUidMsgTypeMultimedia.iUid ) 
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMESSAGEPLUGIN_CALCULATEMESSAGETYPE, "CMessagePlugin:CalculateMessageType  ###  Mms Message ###" );
        CPIXLOGSTRING("CMessagePlugin:CalculateMessageType  ###  Mms Message ###");
        ret = EMsgTypeMms;
        }
	// Add J.J	
#ifndef __SERIES60_30__ // Not supported before S60 3d FP1
   	else if ( aEntry.iMtm.iUid == KSenduiMtmUniMessageUid.iUid )
	    {
	    OstTrace0( TRACE_NORMAL, DUP5_CMESSAGEPLUGIN_CALCULATEMESSAGETYPE, "CMessagePlugin:CalculateMessageType  ### EDraftMessage ###" );
	    CPIXLOGSTRING("CMessagePlugin:CalculateMessageType  ### EDraftMessage ###");
        ret = EMsgTypeDraft;
	    }
#endif
    // else if we have a normal text based SMS it is valid    
 	else if ( aEntry.iMtm.iUid == KUidMsgTypeSMS.iUid )
 		{
 		OstTrace0( TRACE_NORMAL, DUP6_CMESSAGEPLUGIN_CALCULATEMESSAGETYPE, "CMessagePlugin:CalculateMessageType  ###  SMS Message ###" );
 		CPIXLOGSTRING("CMessagePlugin:CalculateMessageType  ###  SMS Message ###");
        ret = EMsgTypeSms;
 		}
   	else if ( aEntry.iMtm.iUid == KSenduiMtmSmsUid.iUid )
		{
		OstTrace0( TRACE_NORMAL, DUP7_CMESSAGEPLUGIN_CALCULATEMESSAGETYPE, "CMessagePlugin:CalculateMessageType  ### Sms Message ###" );
		CPIXLOGSTRING("CMessagePlugin:CalculateMessageType  ### Sms Message ###");
        ret = EMsgTypeSms;
		}
    CPIXLOGSTRING("END CMessagePlugin::CalculateMessageType");
	OstTraceFunctionExit0( CMESSAGEPLUGIN_CALCULATEMESSAGETYPE_EXIT );
	return ret;
}

// ---------------------------------------------------------------------------
// CMessagePlugin::MessageItemL
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::MessageItemL( TMsvId aMsgId, TCPixActionType aActionType, TMsvId aFolderId  )	
    {
    //New item found gather data from that.
     iMessageDataHandler->GatherDataL( aMsgId, aActionType, aFolderId ); 
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::HarvestingCompleted
// ---------------------------------------------------------------------------
//  
void CMessagePlugin::HarvestingCompleted(TInt aError)
{
	// Notifies the indexing manager of completed harvesting, called by CMessageHarvester
    Flush( *GetIndexer() );
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    FormBaseAppClass(TDriveNumber(iCurrentDrive), baseAppClass);
#ifdef __PERFORMANCE_DATA
    TRAP_IGNORE( UpdatePerformaceDataL() );
#endif
    iObserver->HarvestingCompleted(this, baseAppClass, aError);
}

void CMessagePlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_PAUSEPLUGINL_ENTRY );
    iIndexState = EFalse;
    OstTraceFunctionExit0( CMESSAGEPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CMessagePlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_RESUMEPLUGINL_ENTRY );
    iIndexState = ETrue;
    //IndexQueuedItems();
    iMessageDataHandler->ResumeL();
    iMessageMonitor->ResumeL();
    iMessageHarvester->ResumeRequest();
    MountAvailableDrivesInQueue();
    OstTraceFunctionExit0( CMESSAGEPLUGIN_RESUMEPLUGINL_EXIT );
    }

void CMessagePlugin::MountAvailableDrivesInQueue()
    {
    OstTraceFunctionEntry0( CMESSAGEPLUGIN_MOUNTAVAILABLEDRIVESINQUEUE_ENTRY );
    for(TInt i=0;i<iMountDrives.Count();i++)
        {
        TDriveNumber drive = iMountDrives[i];
        iMountDrives.Remove(i);
        iMessageHarvester->StartHarvestingL();        
        }
    OstTraceFunctionExit0( CMESSAGEPLUGIN_MOUNTAVAILABLEDRIVESINQUEUE_EXIT );
    }

TBool CMessagePlugin::GetHarvesterState()
    {
    return iIndexState;
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::UpdatePerformaceDataL
// ---------------------------------------------------------------------------
//
#ifdef __PERFORMANCE_DATA
void CMessagePlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite));
    
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
    ptr.Append( _L("Time taken for Harvesting Message is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }

// ---------------------------------------------------------------------------
// CMessagePlugin::UpdatePerformaceDataL
// ---------------------------------------------------------------------------
//
void CMessagePlugin::UpdatePerformaceDataL(TMsvSessionEvent action) {
    
        iCompleteTime.UniversalTime();
        TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
        
        RFs fileSession;
        RFile perfFile;
        User::LeaveIfError( fileSession.Connect () );
        
        
        /* Open file if it exists, otherwise create it and write content in it */
        
            if(perfFile.Open(fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite))
                       User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\MessagePerf.txt"), EFileWrite));
        
        HBufC8 *heap = HBufC8::NewL(100);
        TPtr8 ptr = heap->Des();

        switch (action) {
            case EMsvEntriesDeleted: ptr.Append( _L("del "));break;
            case EMsvEntriesChanged: ptr.Append( _L("upd "));break;
            case EMsvEntriesMoved: ptr.Append( _L("mov "));break;
        }
        ptr.AppendNum(timeDiff.Int64()/1000) ;
        ptr.Append(_L("\n"));
        TInt myInt = 0;
        perfFile.Seek(ESeekEnd,myInt);
        perfFile.Write (ptr);
        perfFile.Close ();
        fileSession.Close ();
        delete heap;
    }

#endif
