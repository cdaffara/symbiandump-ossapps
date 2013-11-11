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
* Description:  Media Image harvester plugin implementation
*
*/


#include "imageplugin.h"
#include <e32base.h> 
#include "harvesterserverlogger.h"
#include "common.h"
#include "csearchdocument.h"
#include "ccpixindexer.h"
#include "mdeharvester.h"
#include "mdeobjectqueuemanager.h"
#include "mdsmediamonitor.h"
#include "mdssessionobjectutils.h"
#include "cpixmediaimagedoc.h"
#include "cpixindexerutils.h"
#include "mmcmonitor.h"
#include "cpixmdedbmanager.h"
#include "mdsitementity.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "imagepluginTraces.h"
#endif


//Constants
_LIT(KPathTrailer, "\\root\\media\\image");

//***** MEDAI AUDIO*****
#define MEDIA_QBASEAPPCLASS   "@0:root media image"
#define LMEDIA_QBASEAPPCLASS  L"@0:root media image"
#define MEDIAAPPCLASS   "root media image"
#define LMEDIAAPPCLASS  L"root media image"

#define MEDIAGENERICAPPCLASS   ":root media image"
#define LMEDIAGENERICAPPCLASS  L":root media image"
#define DATABASEPATH           "\\root\\media\\image"
#define LDATABASEPATH           "\\root\\media\\image"

// -----------------------------------------------------------------------------
CImagePlugin* CImagePlugin::NewL()
	{
	CImagePlugin* instance = CImagePlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
	}

// -----------------------------------------------------------------------------
CImagePlugin* CImagePlugin::NewLC()
	{
	CImagePlugin* instance = new (ELeave) CImagePlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
	}

// -----------------------------------------------------------------------------
CImagePlugin::CImagePlugin()
	{
	//No implementation required. 
	//Necessary things done in ConstructL()
#ifdef __PERFORMANCE_DATA
    count = 0;
# endif	
	}

// -----------------------------------------------------------------------------
CImagePlugin::~CImagePlugin()
	{
	delete iMdeHarvester;
	delete iMdsMonitor;
	delete iObjectJobQueueManager;
	delete iMdsUtils;
	delete iImageDocument;
	if ( iIndexerUtil )
	   TRAP_IGNORE(iIndexerUtil->UnMountAllDrivesL(_L(MEDIAGENERICAPPCLASS)));
	delete iIndexerUtil;
	delete iMMcMonitor;
	delete iDBManager;
	delete iMdsItem;
	}
	
// -----------------------------------------------------------------------------
void CImagePlugin::ConstructL()
	{
    iObjectJobQueueManager = CMdeObjectQueueManager::NewL(this);
	}

// -----------------------------------------------------------------------------
void CImagePlugin::StartPluginL()
	{
    OstTraceFunctionEntry0( CIMAGEPLUGIN_STARTPLUGINL_ENTRY );
    CPIXLOGSTRING("CImagePlugin::StartPluginL");
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
	iMdsMonitor->StartMonitoringL(MdeConstants::Image::KImageObject);//Start monitoring Image objects
	iImageDocument = CCPIXMediaImageDoc::NewL(); //Doc fetcher
	// Start harvester for this plugin
	iObserver->AddHarvestingQueue(this, _L(MEDIA_QBASEAPPCLASS) );
	TUid uidOfPlugin = {0x20029ABB};
	iDBManager = CCPIXMDEDbManager::NewL(uidOfPlugin);
	iMdsItem = CMDSEntity::NewL();
	OstTraceFunctionExit0( CIMAGEPLUGIN_STARTPLUGINL_EXIT );
	}

void CImagePlugin::HarvestingCompletedL()
    {
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif 
    iObserver->HarvestingCompleted(this, _L(MEDIA_QBASEAPPCLASS), KErrNone);
    }
// -----------------------------------------------------------------------------
void CImagePlugin::StartHarvestingL( const TDesC& /*aMedia*/ )
    {
    OstTraceFunctionEntry0( CIMAGEPLUGIN_STARTHARVESTINGL_ENTRY );
    //iIndexer->ResetL();//reset any indexes if exist already
    CPIXLOGSTRING("CImagePlugin::StartHarvestingL");
    iDBManager->ResetL();
    iIndexerUtil->ResetAllL();
    //Start Media harvesting here
#ifdef __PERFORMANCE_DATA  
    iStartTime.UniversalTime();
#endif  
    iMdeHarvester->DoHarvesetL(MdeConstants::Image::KImageObject);//Start Harvesting
    OstTraceFunctionExit0( CIMAGEPLUGIN_STARTHARVESTINGL_EXIT );
    }

//handle MMC event for the Drive
void CImagePlugin::HandleMMCEventL(const TDriveNumber aDrive,const TBool aMMCInsertRemoval)
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

void CImagePlugin::HandleMdeItemL( TItemId aObjId, TCPixActionType aActionType)
    {
    //Get the object from object utils and get the CPIX document and add delete or update
    OstTrace1( TRACE_NORMAL, CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL;aObjId=%d", aObjId );
    CPIXLOGSTRING2("CImagePlugin::HandleMdeItemL(): aObjId = %d ", aObjId );
    // creating CSearchDocument object with unique ID for this application
    TBuf<KMaxFileName> docid_str;
    if (aActionType == ECPixAddAction || aActionType == ECPixUpdateAction )
        {
        //The caller should always handle leave of HandleMdeItemL
        if(iDBManager->IsAlreadyExistL(aObjId) && aActionType == ECPixAddAction)//Avoid reharvesting
            {
            OstTrace0( TRACE_NORMAL, DUP1_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Already harvested returning" );
            CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Already harvested returning");
            return;
            }
        CMdEObject& object= iMdsUtils->GetObjectL(aObjId,MdeConstants::Image::KImageObject);
        CMdENamespaceDef& defaultNamespaceDef = iMdsUtils->GetSession()->GetDefaultNamespaceDefL();
        CMdEObjectDef& audioObjDef = defaultNamespaceDef.GetObjectDefL(MdeConstants::Image::KImageObject);
        CSearchDocument* index_item = iImageDocument->GetCpixDocumentL(object,_L(MEDIAAPPCLASS),audioObjDef);
        iIndexer = iIndexerUtil->GetIndexerFromMediaId(object.MediaId());
        if(!iIndexer)
            {
            delete index_item;
            OstTrace0( TRACE_NORMAL, DUP2_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Indexer not found" );
            CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Indexer not found");
            return;
            }
        OstTrace0( TRACE_NORMAL, DUP12_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::Indexing Image" );
        
        // Send for indexing
        if (aActionType == ECPixAddAction)
            {
#ifdef __PERFORMANCE_DATA
            ++count;
# endif
            OstTrace0( TRACE_NORMAL, DUP3_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Indexer found calling AddL" );
            CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Indexer found calling AddL");
            //First check if it already exist in database
            TRAPD(err, iIndexer->UpdateL(*index_item)); //Always Update to avoid reharvesting
            if (err == KErrNone)
                {
                    //Add entity to database for later use
                    iMdsItem->Reset();//reset existing values
                    iMdsItem->Setkey(aObjId);
                    iMdsItem->SetUri(iImageDocument->GetUri());
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->SetDrive(drive);
                    iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                OstTrace0( TRACE_NORMAL, DUP4_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Added." );
                CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Added.");
                }
            else
                {
                OstTrace1( TRACE_NORMAL, DUP5_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL;Error %d in adding", err );
                CPIXLOGSTRING2("CImagePlugin::HandleMdeItemL(): Error %d in adding.", err);
                }
            }
        else
            {
            //If DocID for e.g \image\something.jpeg itself changes the we have to compare
            //existing URI with new one and Remove that first and Update to avoid 
            //Duplicate entries
            iMdsItem->Reset();//reset existing values
            TRAPD(err, iDBManager->GetItemL(aObjId,*iMdsItem));
            if (err == KErrNone)
                {
                //Get the drivenumber from the syncDB to delete the appropriate docId 
                //in case an item is moved to different drive, delete from the old drive first and later update.
                //comparing the absolute paths along with the root drives
                if(iMdsItem->GetPath().Compare(object.Uri()) != 0)
                    {
                    CCPixIndexer *tempIndexer;            
                    tempIndexer = iIndexerUtil->GetIndexerFromDrive(iMdsItem->DriveNumber());
                    tempIndexer->DeleteL(iMdsItem->Uri());
                    OstTrace0( TRACE_NORMAL, DUP6_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Deleted existing URI for update" );
                    CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Deleted existing URI for update");
                    }
                TRAPD(error, iIndexer->UpdateL(*index_item));
                if (error == KErrNone)
                    {
                    //Add as new
                    iMdsItem->Reset();//reset existing values
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->Set(aObjId,iImageDocument->GetUri(),drive);
                    iDBManager->UpdateL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP7_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Updated." );
                    CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Updated.");
                    }
                else
                    {
                    OstTrace1( TRACE_NORMAL, DUP8_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL;Error %d in updating", err );
                    CPIXLOGSTRING2("CImagePlugin::HandleMdeItemL(): Error %d in updating.", error);
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
                    iMdsItem->SetUri(iImageDocument->GetUri());//Use the Absolute URI
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->SetDrive(drive);
                    iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP9_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Added Item before Update." );
                    CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Added Item before Update.");
                    }
                }                       
            }
        delete index_item;
        iIndexer = NULL;//Assign to null not pointing to any memory
        }
    else if (aActionType == ECPixRemoveAction)
        {
        //We can not get media id of removed doc from MDS as it is deleed so we have to remove from 
        //all available Indexers
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
                    OstTrace0( TRACE_NORMAL, DUP10_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL(): Deleted." );
                    CPIXLOGSTRING("CImagePlugin::HandleMdeItemL(): Deleted.");
                   }
               else
                   {
                    OstTrace1( TRACE_NORMAL, DUP11_CIMAGEPLUGIN_HANDLEMDEITEML, "CImagePlugin::HandleMdeItemL;Error %d in deleting", err );
                    CPIXLOGSTRING2("CImagePlugin::HandleMdeItemL(): Error %d in deleting.", err);
                   }
               }
            //delete entry from database
            iDBManager->RemoveL(iMdsItem->Key());
            }
        iIndexer = NULL;//Assign to null not pointing to any memory
        }    
    }

void CImagePlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CIMAGEPLUGIN_PAUSEPLUGINL_ENTRY );
    iObjectJobQueueManager->PauseL();
    OstTraceFunctionExit0( CIMAGEPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CImagePlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CIMAGEPLUGIN_RESUMEPLUGINL_ENTRY );
    iObjectJobQueueManager->ResumeL();    
    OstTraceFunctionExit0( CIMAGEPLUGIN_RESUMEPLUGINL_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CImagePlugin::UpdateLogL()
    {
    UpdatePerformaceDataL();
    }

void CImagePlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError(fileSession.Connect ());
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
    if(perfFile.Open(fileSession, _L("c:\\data\\ImagePerf.txt"), EFileWrite))
    User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\ImagePerf.txt"), EFileWrite));
    
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
    ptr.Append( _L(" :Time took for Harvesting Image is : "));
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
