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
* Description:  Media Video harvester plugin implementation
*
*/

//  Include Files
#include "videoplugin.h" //CVideoPlugin
#include <e32base.h>
#include "harvesterserverlogger.h"
#include "common.h"
#include "csearchdocument.h"
#include "ccpixindexer.h"
#include "mdeharvester.h"
#include "mdeobjectqueuemanager.h"
#include "mdsmediamonitor.h"
#include "mdssessionobjectutils.h"
#include "cpixmediavideodoc.h"
#include "cpixindexerutils.h"
#include "mmcmonitor.h"
#include "cpixmdedbmanager.h"
#include "mdsitementity.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "videopluginTraces.h"
#endif


//Constants
_LIT(KPathTrailer, "\\root\\media\\video");
#define MEDIA_QBASEAPPCLASS   "@0:root media video"
#define LMEDIA_QBASEAPPCLASS  L"@0:root media video"
#define MEDIAAPPCLASS   "root media video"
#define LMEDIAAPPCLASS  L"root media video"

#define MEDIAGENERICAPPCLASS   ":root media video"
#define LMEDIAGENERICAPPCLASS  L":root media video"
#define DATABASEPATH           "\\root\\media\\video"
#define LDATABASEPATH           "\\root\\media\\video"

//Member Functions

CVideoPlugin* CVideoPlugin::NewL()
    {
    CVideoPlugin* self = CVideoPlugin::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CVideoPlugin* CVideoPlugin::NewLC()
    {
    CVideoPlugin* self = new (ELeave) CVideoPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CVideoPlugin::CVideoPlugin()
    {
#ifdef __PERFORMANCE_DATA
    count = 0;
#endif
    //No implementation required.    
    }

CVideoPlugin::~CVideoPlugin()
    {    
    delete iMdeHarvester;
    delete iMdsMonitor;
    delete iObjectJobQueueManager;
    delete iMdsUtils;
    delete iVideoDocument;
    if ( iIndexerUtil )
      TRAP_IGNORE(iIndexerUtil->UnMountAllDrivesL(_L(MEDIAGENERICAPPCLASS)));
    delete iIndexerUtil;
    delete iMMcMonitor;
    delete iDBManager;
    delete iMdsItem;
    }

void CVideoPlugin::ConstructL()
    {
    iObjectJobQueueManager = CMdeObjectQueueManager::NewL(this);
    }

void CVideoPlugin::StartPluginL()
    {
    OstTraceFunctionEntry0( CVIDEOPLUGIN_STARTPLUGINL_ENTRY );
    //Define the basic application class 
    CPIXLOGSTRING("CVideoPlugin::StartPluginL");
    iIndexerUtil = CCPixIndexerUtils::NewL( iSearchSession );
    TBuf<KMaxFileName> databasePathTemp;
    databasePathTemp.Append( KPathTrailer );
    iIndexerUtil->MountAllAvailableDriveL( _L(MEDIAGENERICAPPCLASS), databasePathTemp );
    // Instansiate harvesting and monitering
    iMdsUtils = CMdsSessionObjectUtils::NewL();
    iMdsUtils->InitializeL();
    //iObjectJobQueueManager = CMdeObjectQueueManager::NewL( this );
    iMdeHarvester = CMdeHarvester::NewL( iMdsUtils->GetSession(), this,
                                         iObjectJobQueueManager );   
    iMdsMonitor = CMdsMediaMonitor::NewL( iMdsUtils->GetSession(), iObjectJobQueueManager );
    iMMcMonitor = CMMCMonitorUtil::NewL( this );//Observer is self
    iMMcMonitor->StartMonitoring();
    //start monitoring video items
    iMdsMonitor->StartMonitoringL( MdeConstants::Video::KVideoObject );
    iVideoDocument = CCPIXMediaVideoDoc::NewL(); //Doc fetcher
    // Start harvester for this plugin
    iObserver->AddHarvestingQueue(this, _L(MEDIA_QBASEAPPCLASS) );
    TUid uidOfPlugin = {0x20029ABA};
    iDBManager = CCPIXMDEDbManager::NewL(uidOfPlugin);
    iMdsItem = CMDSEntity::NewL();
    OstTraceFunctionExit0( CVIDEOPLUGIN_STARTPLUGINL_EXIT );
    }

void CVideoPlugin::StartHarvestingL( const TDesC& /*aMedia*/ )
    {
    OstTraceFunctionEntry0( CVIDEOPLUGIN_STARTHARVESTINGL_ENTRY );
    CPIXLOGSTRING("CVideoPlugin::StartHarvestingL");
    iDBManager->ResetL();
    iIndexerUtil->ResetAllL();
    //start video harvesting
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
    iMdeHarvester->DoHarvesetL( MdeConstants::Video::KVideoObject );      
    OstTraceFunctionExit0( CVIDEOPLUGIN_STARTHARVESTINGL_EXIT );
    }

void CVideoPlugin::HarvestingCompletedL()
    {
    OstTraceFunctionEntry0( CVIDEOPLUGIN_HARVESTINGCOMPLETEDL_ENTRY );
    CPIXLOGSTRING("CVideoPlugin::HarvestingCompletedL");
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif
    iObserver->HarvestingCompleted(this, _L(MEDIA_QBASEAPPCLASS), KErrNone);    
    OstTraceFunctionExit0( CVIDEOPLUGIN_HARVESTINGCOMPLETEDL_EXIT );
    }

//handle MMC event for the Drive
void CVideoPlugin::HandleMMCEventL(const TDriveNumber aDrive,const TBool aMMCInsertRemoval)
    {
    TBuf<KMaxFileName> databasePathTemp;
    databasePathTemp.Append(KPathTrailer);
    if(aMMCInsertRemoval) //MMC inserted
        {        
        iIndexerUtil->MountDriveL(aDrive,_L(MEDIAGENERICAPPCLASS),databasePathTemp);        
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

void CVideoPlugin::HandleMdeItemL( TItemId aObjId, TCPixActionType aActionType)
    {
    //Get the object from object utils and get the CPIX document and add delete or update
    OstTrace1( TRACE_NORMAL, CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL;aObjId=%d", aObjId );
    CPIXLOGSTRING2("CVideoPlugin::HandleMdeItemL(): aObjId = %d ", aObjId );
    // creating CSearchDocument object with unique ID for this application
    TBuf<KMaxFileName> docid_str;
    if (aActionType == ECPixAddAction || aActionType == ECPixUpdateAction )
        {
        //To Improve performance check here only before Getobject if it is Add event
        //and we already have in our database just return in case of update move forward
        if(iDBManager->IsAlreadyExistL(aObjId) && aActionType == ECPixAddAction)//Avoid reharvesting
           {
           OstTrace0( TRACE_NORMAL, DUP1_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Already harvested returning" );
           CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Already harvested returning");
           return;
           }
        CMdEObject& object= iMdsUtils->GetObjectL( aObjId,MdeConstants::Video::KVideoObject );        
        CMdENamespaceDef& defaultNamespaceDef = iMdsUtils->GetSession()->GetDefaultNamespaceDefL();        
        CMdEObjectDef& videoObjDef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Video::KVideoObject );        
        CSearchDocument* index_item = iVideoDocument->GetCpixDocumentL(object,_L(MEDIAAPPCLASS),videoObjDef);
        iIndexer = iIndexerUtil->GetIndexerFromMediaId(object.MediaId());
        
        if (!iIndexer)
            {
            delete index_item;
            OstTrace0( TRACE_NORMAL, DUP2_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Indexer not found" );
            CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Indexer not found");
            return;
            }
        // Send for indexing
        OstTrace0( TRACE_NORMAL, DUP12_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::Indexing Video" );
        
        if (aActionType == ECPixAddAction)
            {
#ifdef __PERFORMANCE_DATA
            ++count;
#endif
            OstTrace0( TRACE_NORMAL, DUP3_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Indexer found calling AddL" );
            CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Indexer found calling AddL");
            //First check if it already exist in database
            TRAPD(err, iIndexer->UpdateL(*index_item)); //Always Update to avoid reharvesting
            if (err == KErrNone)
                {
                //Add entity to database for later use
                iMdsItem->Reset();//reset existing values
                iMdsItem->Setkey(aObjId);
                iMdsItem->SetUri(iVideoDocument->GetUri());//Use the Absolute URI
                TDriveNumber drive;
                iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                iMdsItem->SetDrive(drive);
                iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                OstTrace0( TRACE_NORMAL, DUP4_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Added." );
                CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Added.");
                }
            else
                {
                OstTrace1( TRACE_NORMAL, DUP5_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL;Error %d in adding", err );
                CPIXLOGSTRING2("CVideoPlugin::HandleMdeItemL(): Error %d in adding.", err);
                }           
            }
        else
            {
            //If DocID for e.g \video\something.3gpp itself changes the we have to compare
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
                    OstTrace0( TRACE_NORMAL, DUP6_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Deleted existing URI for update" );
                    CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Deleted existing URI for update");
                    }
                TRAPD(error, iIndexer->UpdateL(*index_item));
                if (error == KErrNone)
                    {
                    //Add as new
                    iMdsItem->Reset();//reset existing values
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->Set(aObjId,iVideoDocument->GetUri(),drive);
                    iDBManager->UpdateL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP7_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Updated." );
                    CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Updated.");
                    }
                else
                    {
                    OstTrace1( TRACE_NORMAL, DUP8_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL;Error %d in updating", err );
                    CPIXLOGSTRING2("CVideoPlugin::HandleMdeItemL(): Error %d in updating.", error);
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
                    iMdsItem->SetUri(iVideoDocument->GetUri());//Use the Absolute URI
                    TDriveNumber drive;
                    iIndexerUtil->GetDriveFromMediaId(object.MediaId(),drive);
                    iMdsItem->SetDrive(drive);
                    iDBManager->AddL(aObjId,*iMdsItem);//Add to SyncDB
                    OstTrace0( TRACE_NORMAL, DUP9_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Added Item before Update." );
                    CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Added Item before Update.");
                    }
                }                    
            }
        delete index_item;
        iIndexer = NULL;        
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
                        OstTrace0( TRACE_NORMAL, DUP10_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL(): Deleted." );
                        CPIXLOGSTRING("CVideoPlugin::HandleMdeItemL(): Deleted.");
                        }
                    else
                        {
                        OstTrace1( TRACE_NORMAL, DUP11_CVIDEOPLUGIN_HANDLEMDEITEML, "CVideoPlugin::HandleMdeItemL;Error %d in deleting", err );
                        CPIXLOGSTRING2("CVideoPlugin::HandleMdeItemL(): Error %d in deleting.", err);
                        }
                    }
                //delete entry from database
                iDBManager->RemoveL(iMdsItem->Key());
                }
            iIndexer = NULL;//Assign to null not pointing to any memory
            }    
    }

void CVideoPlugin::PausePluginL()
    {
    OstTraceFunctionEntry0( CVIDEOPLUGIN_PAUSEPLUGINL_ENTRY );
    iObjectJobQueueManager->PauseL();
    OstTraceFunctionExit0( CVIDEOPLUGIN_PAUSEPLUGINL_EXIT );
    }

void CVideoPlugin::ResumePluginL()
    {
    OstTraceFunctionEntry0( CVIDEOPLUGIN_RESUMEPLUGINL_ENTRY );
    iObjectJobQueueManager->ResumeL();
    OstTraceFunctionExit0( CVIDEOPLUGIN_RESUMEPLUGINL_EXIT );
    }

#ifdef __PERFORMANCE_DATA
void CVideoPlugin::UpdateLogL()
    {
    UpdatePerformaceDataL();
    }

void CVideoPlugin::UpdatePerformaceDataL()
    {
    TTime now;
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\VideoPerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\VideoPerf.txt"), EFileWrite));
    
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
    ptr.Append(_L("--"));
    ptr.AppendNum(count);
    ptr.Append( _L(" :Time took for Harvesting Video is : "));
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
