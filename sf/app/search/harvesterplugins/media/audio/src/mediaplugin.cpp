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
* Description:  Media Audio harvester plugin implementation
*
*/


#include "mediaplugin.h"

#include <e32base.h> 

#include "harvesterserverlogger.h"
#include "common.h"
#include "csearchdocument.h"
#include "ccpixindexer.h"
#include "mdeharvester.h"
#include "mdeobjectqueuemanager.h"
#include "mdsmediamonitor.h"
#include "mdssessionobjectutils.h"
#include "cpixmediaaudiodoc.h"
#include "cpixindexerutils.h"
#include "mmcmonitor.h"
#include "cpixmdedbmanager.h"
#include "mdsitementity.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mediapluginTraces.h"
#endif


//Constants
_LIT(KPathTrailer, "\\root\\media\\audio");

//***** MEDAI AUDIO*****
#define MEDIA_QBASEAPPCLASS   "@0:root media audio"
#define LMEDIA_QBASEAPPCLASS  L"@0:root media audio"
#define MEDIAAPPCLASS   "root media audio"
#define LMEDIAAPPCLASS  L"root media audio"

#define MEDIAGENERICAPPCLASS   ":root media audio"
#define LMEDIAGENERICAPPCLASS  L":root media audio"
#define DATABASEPATH           "\\root\\media\\audio"
#define LDATABASEPATH           "\\root\\media\\audio"

// -----------------------------------------------------------------------------
CAudioPlugin* CAudioPlugin::NewL()
	{
	CAudioPlugin* instance = CAudioPlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
	}

// -----------------------------------------------------------------------------
CAudioPlugin* CAudioPlugin::NewLC()
	{
	CAudioPlugin* instance = new (ELeave) CAudioPlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
	}

// -----------------------------------------------------------------------------
CAudioPlugin::CAudioPlugin()
	{
	//No implementation required. 
	//Necessary things done in ConstructL()
#ifdef __PERFORMANCE_DATA
    count = 0;
#endif
	}

// -----------------------------------------------------------------------------
CAudioPlugin::~CAudioPlugin()
	{
	delete iMdeHarvester;
	delete iMdsMonitor;
	delete iObjectJobQueueManager;
	delete iMdsUtils;
	delete iAudioDocument;
	if (iIndexerUtil)
	  TRAP_IGNORE(iIndexerUtil->UnMountAllDrivesL(_L(MEDIAGENERICAPPCLASS)));
	delete iIndexerUtil;
	delete iMMcMonitor;
	delete iDBManager;
	delete iMdsItem;
	}
	
// -----------------------------------------------------------------------------
void CAudioPlugin::ConstructL()
	{
    iObjectJobQueueManager = CMdeObjectQueueManager::NewL(this);
	}

// -----------------------------------------------------------------------------
void CAudioPlugin::StartPluginL()
	{
    OstTraceFunctionEntry0( CAUDIOPLUGIN_STARTPLUGINL_ENTRY );
    CPIXLOGSTRING("CAudioPlugin::StartPluginL");
	// Define this base application class, use default location
	iIndexerUtil = CCPixIndexerUtils::NewL(iSearchSession);
	//Test well before use
	TBuf<KMaxFileName> databasePathTemp;
	databasePathTemp.Append(KPathTrailer);
	iIndexerUtil->MountAllAvailableDriveL(_L(MEDIAGENERICAPPCLASS),databasePathTemp);
	//Instantiate all monitoring and harvesting class here
	iMdsUtils = CMdsSessionObjectUtils::NewL();
	iMdsUtils->InitializeL(); //Create valid session in it
	//iObjectJobQueueManager = CMdeObjectQueueManager::NewL(this);
	iMdeHarvester = CMdeHarvester::NewL(iMdsUtils->GetSession(),
	                                    this,iObjectJobQueueManager);
	iMdsMonitor = CMdsMediaMonitor::NewL(iMdsUtils->GetSession(),iObjectJobQueueManager);
	iMMcMonitor = CMMCMonitorUtil::NewL(this);//Observer is self
	iMMcMonitor->StartMonitoring();
	//Start Monitoring now
	iMdsMonitor->StartMonitoringL(MdeConstants::Audio::KAudioObject);//Start monitoring Audio objects
	iAudioDocument = CCPIXMediaAudioDoc::NewL(); //Doc fetcher
	// Start harvester for this plugin
	iObserver->AddHarvestingQueue(this, _L(MEDIA_QBASEAPPCLASS) );
	TUid uidOfPlugin = {0x20029AB9};
	iDBManager = CCPIXMDEDbManager::NewL(uidOfPlugin);
	iMdsItem = CMDSEntity::NewL();
	OstTraceFunctionExit0( CAUDIOPLUGIN_STARTPLUGINL_EXIT );
	}

void CAudioPlugin::HarvestingCompletedL()
    {
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif 
    iObserver->HarvestingCompleted(this, _L(MEDIA_QBASEAPPCLASS), KErrNone);
    }
// -----------------------------------------------------------------------------
void CAudioPlugin::StartHarvestingL( const TDesC& /*aMedia*/ )
    {
    OstTraceFunctionEntry0( CAUDIOPLUGIN_STARTHARVESTINGL_ENTRY );
    //iIndexer->ResetL();//reset any indexes if exist already
    CPIXLOGSTRING("CAudioPlugin::StartHarvestingL");
    iDBManager->ResetL();
    iIndexerUtil->ResetAllL();
    //Start Media harvesting here
#ifdef __PERFORMANCE_DATA  
    iStartTime.UniversalTime();
#endif  
    iMdeHarvester->DoHarvesetL(MdeConstants::Audio::KAudioObject);//Start Harvesting
    OstTraceFunctionExit0( CAUDIOPLUGIN_STARTHARVESTINGL_EXIT );
    }

//handle MMC event for the Drive
void CAudioPlugin::HandleMMCEventL(const TDriveNumber aDrive,const TBool aMMCInsertRemoval)
    {
    TBuf<KMaxFileName> databasePathTemp;
    databasePathTemp.Append(KPathTrailer);
    if(aMMCInsertRemoval) //MMC inserted
        {
        //Test well before use
        iIndexerUtil->MountDriveL(aDrive,_L(MEDIAGENERICAPPCLASS),databasePathTemp);
        //reset indexes existing first
        //With new Implementation no need to reset
        //iIndexerUtil->GetIndexerFromDrive(aDrive)->ResetL();
        }
    else
        {
        //MMC is removed but we can not reset index here as Media physically not avilable so
        // we have to do at the time of insertion only
        //Unmount drive and also reset the index as we anyhow has to reindex on insertion back 
        iIndexerUtil->UnMountDriveL(aDrive,_L(MEDIAGENERICAPPCLASS),ETrue);
        }
    }

void CAudioPlugin::HandleMdeItemL( TItemId aObjId, TCPixActionType aActionType)
    {
    //Get the object from object utils and get the CPIX document and add delete or update
    OstTrace1( TRACE_NORMAL, CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL;aObjId=%d", aObjId );
    CPIXLOGSTRING2("CAudioPlugin::HandleMdeItemL(): aObjId = %d ", aObjId );
    // creating CSearchDocument object with unique ID for this application
    TBuf<KMaxFileName> docid_str;
    if (aActionType == ECPixAddAction || aActionType == ECPixUpdateAction )
        {
        //To Improve performance check here only before Getobject if it is Add event
        //and we already have in our database just return in case of update move forward
        if(iDBManager->IsAlreadyExistL(aObjId) && aActionType == ECPixAddAction)//Avoid reharvesting
            {
            OstTrace0( TRACE_NORMAL, DUP1_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Already harvested returning" );
            CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Already harvested returning");
            return;
            }
        //The caller should always handle leave of HandleMdeItemL
        CMdEObject& object= iMdsUtils->GetObjectL(aObjId,MdeConstants::Audio::KAudioObject);
        CMdENamespaceDef& defaultNamespaceDef = iMdsUtils->GetSession()->GetDefaultNamespaceDefL();
        CMdEObjectDef& audioObjDef = defaultNamespaceDef.GetObjectDefL(MdeConstants::Audio::KAudioObject );
        CSearchDocument* index_item = iAudioDocument->GetCpixDocumentL(object,_L(MEDIAAPPCLASS),audioObjDef);
        iIndexer = iIndexerUtil->GetIndexerFromMediaId(object.MediaId());
        if(!iIndexer)
            {
            delete index_item;//Remove
            OstTrace0( TRACE_NORMAL, DUP2_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Indexer not found" );
            CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Indexer not found");
            return;
            }
        OstTrace0( TRACE_NORMAL, DUP12_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL Indexing Audio" );
        
        // Send for indexing
        if (aActionType == ECPixAddAction)
            {
#ifdef __PERFORMANCE_DATA
            ++count;
#endif
            OstTrace0( TRACE_NORMAL, DUP3_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Indexer found calling AddL" );
            CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Indexer found calling AddL");
            //First check if it already exist in database
            TRAPD(err, iIndexer->UpdateL(*index_item)); //Always Update to avoid reharvesting
            if (err == KErrNone)
                {
                //Add entity to database for later use
                iMdsItem->Reset();//reset existing values
                iMdsItem->Setkey(aObjId);
                iMdsItem->SetUri(iAudioDocument->GetUri());//Use the Absolute URI
                TDriveNumber drive;
                iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                iMdsItem->SetDrive(drive);
                iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                OstTrace0( TRACE_NORMAL, DUP4_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Added." );
                CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Added.");
                }
            else
                {
                OstTrace1( TRACE_NORMAL, DUP5_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL;Error %d in adding", err );
                CPIXLOGSTRING2("CAudioPlugin::HandleMdeItemL(): Error %d in adding.", err);
                }
            }
        else
            {
            //If DocID for e.g \music\something.mp3 itself changes the we have to compare
            //existing URI with new one and Remove that first and Update to avoid 
            //Duplicate entries
            iMdsItem->Reset();//reset existing values
            TRAPD(err, iDBManager->GetItemL(aObjId,*iMdsItem));
            if(err == KErrNone)
               {
               //Get the drivenumber from the syncDB to delete the appropriate docId 
               //in case an item is moved to different drive, delete from the old drive first and later update.
               //comparing the absolute paths along with the root drives
                if(iMdsItem->GetPath().Compare(object.Uri()) != 0)
                   {                        
                   CCPixIndexer *tempIndexer;            
                   tempIndexer = iIndexerUtil->GetIndexerFromDrive(iMdsItem->DriveNumber());
                   tempIndexer->DeleteL(iMdsItem->Uri());
                   OstTrace0( TRACE_NORMAL, DUP6_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Deleted existing URI for update" );
                   CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Deleted existing URI for update");
                   }
                TRAPD(error, iIndexer->UpdateL(*index_item));
                if (error == KErrNone)
                    {
                    //Add as new
                    iMdsItem->Reset();//reset existing values
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->Set(aObjId,iAudioDocument->GetUri(),drive);
                    iDBManager->UpdateL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP7_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Updated." );
                    CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Updated.");
                    }
                else
                    {
                    OstTrace1( TRACE_NORMAL, DUP8_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL;Error %d in Updating", err );
                    CPIXLOGSTRING2("CAudioPlugin::HandleMdeItemL(): Error %d in updating.", error);
                    }
               }
            //If a media item is added when downloading or streaming begins, add event is surpassed
            //by update or delete event because of delay introduced on monitered object by objectqueue manager
            //So add the Item first, when GetItemL() leaves with item not found
            else if(err == KErrNotFound)
                {
                TRAPD(error, iIndexer->UpdateL(*index_item));
                if (error == KErrNone)
                    {
                    //Add entity to database for later use
                    iMdsItem->Reset();//reset existing values
                    iMdsItem->Setkey(aObjId);
                    iMdsItem->SetUri(iAudioDocument->GetUri());//Use the Absolute URI
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->SetDrive(drive);
                    iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP9_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Added Item before Update." );
                    CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Added Item before Update.");
                    }
                }            
            }
        delete index_item;
        iIndexer = NULL;//Assign to null not pointing to any memory
        }
    else if (aActionType == ECPixRemoveAction)
        {
        // Remove the document
        //Get the Item from SyncDB for removed Item
        iMdsItem->Reset();//reset existing values
        TRAPD(error, iDBManager->GetItemL(aObjId,*iMdsItem));
        if (error == KErrNone)
            {
            iIndexer = iIndexerUtil->GetIndexerFromDrive(iMdsItem->DriveNumber());//Get Indexer from drive
            if(iIndexer) //If indexer found delete
                {
                docid_str.Copy(iMdsItem->Uri());
                TRAPD(err, iIndexer->DeleteL(docid_str));//Delete it
                if (err == KErrNone)
                    {
                    OstTrace0( TRACE_NORMAL, DUP10_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL(): Deleted." );
                    CPIXLOGSTRING("CAudioPlugin::HandleMdeItemL(): Deleted.");
                    }
                else
                    {
                    OstTrace1( TRACE_NORMAL, DUP11_CAUDIOPLUGIN_HANDLEMDEITEML, "CAudioPlugin::HandleMdeItemL;Error %d in deleting", err );
                    CPIXLOGSTRING2("CAudioPlugin::HandleMdeItemL(): Error %d in deleting.", err);
                    }
                }
            //delete entry from database
            iDBManager->RemoveL(iMdsItem->Key());
            }
        iIndexer = NULL;//Assign to null not pointing to any memory
        }    
    }

void CAudioPlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CAUDIOPLUGIN_PAUSEPLUGINL_ENTRY );
    iObjectJobQueueManager->PauseL();
    OstTraceFunctionExit0( CAUDIOPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CAudioPlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CAUDIOPLUGIN_RESUMEPLUGINL_ENTRY );
    iObjectJobQueueManager->ResumeL();
    OstTraceFunctionExit0( CAUDIOPLUGIN_RESUMEPLUGINL_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CAudioPlugin::UpdateLogL()
    {
    UpdatePerformaceDataL();
    }

void CAudioPlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError(fileSession.Connect ());
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
    if(perfFile.Open(fileSession, _L("c:\\data\\AudioPerf.txt"), EFileWrite))
    User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\AudioPerf.txt"), EFileWrite));
    
    HBufC8 *heap = HBufC8::NewL(100);
    TPtr8 ptr = heap->Des();
    now.HomeTime();
    TBuf<100> timeString;             
                
    _LIT(KOwnTimeFormat,"%:0%H%:1%T%:2%S");
    now.FormatL(timeString,KOwnTimeFormat);
    ptr.AppendNum(now.DateTime().Day());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Month());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Year());
    ptr.Append(_L(":"));
    ptr.Append(timeString);
    ptr.Append(_L("--"));
    ptr.AppendNum(count);
    ptr.Append( _L(" :Time took for Harvesting Audio is : "));
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
