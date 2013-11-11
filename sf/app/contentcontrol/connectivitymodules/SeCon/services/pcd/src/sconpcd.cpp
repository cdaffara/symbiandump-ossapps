/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConPCD implementation
*
*/


// INCLUDE FILES

#include "sconpcd.h"
#include "sconpcdconsts.h"
#include "sconinstqueue.h"
#include "sconbrqueue.h"
#include "sconmetadata.h"
#include "sconsynchandler.h"
#include "debug.h"

const TInt KMaxObjectSize(65536); // informed to PC

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CreateCSConPCDL()
// Entry to CSConPCD
// -----------------------------------------------------------------------------
//
EXPORT_C CSConPCD* CreateCSConPCDL()
    {
    TRACE_FUNC;
    return CSConPCD::NewL();
    }
    
// -----------------------------------------------------------------------------
// CSConPCD::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConPCD* CSConPCD::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConPCD* self = new (ELeave) CSConPCD();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSConPCD::CSConPCD()
// Constructor
// -----------------------------------------------------------------------------
//
CSConPCD::CSConPCD()
    {
    }
    
// -----------------------------------------------------------------------------
// CSConPCD::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConPCD::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iFs.Connect() );
    if ( !CActiveScheduler::Current() )
        {
        CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
        CActiveScheduler::Install( scheduler );
        }
    
    iInstallerQueue = CSConInstallerQueue::NewL( iFs );
    iBackupRestoreQueue = CSConBackupRestoreQueue::NewL( iFs );
    iInstallerQueue->QueueAddress( iBackupRestoreQueue );
    iBackupRestoreQueue->QueueAddress( iInstallerQueue );
    
    iSyncHandler = CSconSyncHandler::NewL( iFs );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCD::~CSConPCD()
// Destructor
// -----------------------------------------------------------------------------
//
CSConPCD::~CSConPCD()
    {
    TRACE_FUNC_ENTRY;
    delete iInstallerQueue;
    delete iBackupRestoreQueue;
    delete iLatestReply;
    delete iSyncHandler;
    iFs.Close();
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConPCD::PutTaskL( CSConTask* aTask )
// Receives a new task
// -----------------------------------------------------------------------------
//
TInt CSConPCD::PutTaskL( CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TInt maxObjectSize( 0 );
    
    if ( iLastOperation == EPutTask )
        {
        LOGGER_WRITE( "CSConPCD::PutTaskL() : Two Puts in a row -> Reset()" );
        ResetPCD();
        }
    else
        {
        LOGGER_WRITE( "CSConPCD::PutTaskL() : Protocol procedure ok" );
        iLastOperation = EPutTask;
        }
    
    switch( aTask->GetServiceId() )
        {
        case ECancel :
            ret = CancelL( aTask->iCancelTaskId, aTask->iCancelTaskAll );
            delete aTask;
            break;
        case EGetDataOwnerStatus :
            ret = SupportedBRTaskL( aTask );
            break;
        case EGetDataSize :
            ret = SupportedBRTaskL( aTask );
            break;
        case EGetStatus :
            ret = GetStatusL( aTask->iGetStatusParams->iTaskId, 
            		aTask->iGetStatusParams->iAll );
            delete aTask;
            break;
        case EInstall :
            ret = SupportedInstTaskL( aTask );
            break;
        case EListDataOwners :
            ret = SupportedBRTaskL( aTask );
            break;
        case EListInstalledApps :
            ret = SupportedInstTaskL( aTask );
            break;
        case EListPublicFiles :
            ret = SupportedBRTaskL( aTask );          
            break;
        case ERequestData :
            ret = SupportedBRTaskL( aTask );
            break;
        case ESetBURMode :
            ret = SupportedBRTaskL( aTask );
            break;
        case ESetInstParams :
            ret = SupportedInstTaskL( aTask );
            break;
        case ESupplyData :
            ret = SupportedBRTaskL( aTask );
            break;
        case EUninstall :
            ret = SupportedInstTaskL( aTask );
            break;
        case EUpdateDeviceInfo :
            maxObjectSize = aTask->iDevInfoParams->iMaxObjectSize;
            ret = UpdateDeviceInfoL( maxObjectSize );
            iTaskNumber++;
            delete aTask;
            break;
        case EReboot :
            RebootTaskL( aTask );
            delete aTask;
            break;
        case EGetMetadata :
        	ret = KErrNone;
            GetMetadataTaskL( *aTask );
            delete aTask;
            break;
        default :
            break;
        }
    LOGGER_WRITE_1( "CSConPCD::PutTaskL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCD::GetReply()
// Returns a reply to previous PutTaskL operation
// -----------------------------------------------------------------------------
//
CSConStatusReply* CSConPCD::GetReply()
    {
    TRACE_FUNC_ENTRY;
    
    if ( iLastOperation == EGetReply )
        {
        LOGGER_WRITE( "CSConPCD::GetReply() : Two Gets in a row -> Reset()" );
        ResetPCD();
        }
    else
        {
        LOGGER_WRITE( "CSConPCD::GetReply() : Protocol procedure ok" );
        iLastOperation = EGetReply;
        }
    CSConStatusReply* reply = iLatestReply;
    iLatestReply = NULL;
    TRACE_FUNC_EXIT;
    return reply;
    }

// -----------------------------------------------------------------------------
// CSConPCD::ResetPCD()
// Resets the module to the initial state
// -----------------------------------------------------------------------------
//
void CSConPCD::ResetPCD()
    {
    TRACE_FUNC_ENTRY;
    if ( iInstallerQueue )
        {
        LOGGER_WRITE( "CSConPCD::ResetPCD() : Reseting installer queue" );
        iInstallerQueue->Reset();
        }
        
    if ( iBackupRestoreQueue )
        {
        LOGGER_WRITE( "CSConPCD::ResetPCD() : Reseting backup queue" );
        iBackupRestoreQueue->Reset();
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCD::HandleGetSyncRequestL()
// 
// -----------------------------------------------------------------------------
//
void CSConPCD::HandleGetSyncRequestL( const TDesC8& aRequest, RWriteStream& aResult, TInt aMaxObjectSize )
    {
    TRACE_FUNC_ENTRY;
    iSyncHandler->HandleGetSyncRequestL( aRequest, aResult, aMaxObjectSize );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCD::HandlePutSyncRequestL()
// 
// -----------------------------------------------------------------------------
//
void CSConPCD::HandlePutSyncRequestL( const TDesC8& aRequest, RReadStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    iSyncHandler->HandlePutSyncRequestL( aRequest, aResult );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCD::SupportedInstTaskL( CSConTask* aTask )
// Fills a reply with predefined supported task -information
// -----------------------------------------------------------------------------
//  
TInt CSConPCD::SupportedInstTaskL( CSConTask* aTask )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    iTaskNumber++;
    ret = iInstallerQueue->AddNewTask( aTask, iTaskNumber );
    
    //Get status for this task
    TBool all( EFalse );
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( reply );
    iInstallerQueue->GetQueueStatusL( iTaskNumber, all, reply );
    CleanupStack::Pop( reply );
    
    StoreReply( reply );    
        
    LOGGER_WRITE_1( "CSConPCD::SupportedInstTaskL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCD::SupportedBRTask( CSConTask* aTask )
// Fills a reply with predefined supported task -information
// -----------------------------------------------------------------------------
//
TInt CSConPCD::SupportedBRTaskL( CSConTask* aTask )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    TInt taskId;
    
    taskId = ++iTaskNumber;
        
    ret = iBackupRestoreQueue->AddNewTask( aTask, taskId );
    LOGGER_WRITE_1( "iBackupRestoreQueue->AddNewTask : ret %d", ret );
    //Get status for this task
    TBool all( EFalse );
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( reply );
    iBackupRestoreQueue->GetQueueStatusL( iTaskNumber, all, reply );
    CleanupStack::Pop( reply );
    
    StoreReply( reply );    
    
    TRACE_FUNC_EXIT;
    return ret;
    }


// -----------------------------------------------------------------------------
// CSConPCD::RebootTask( const CSConTask* aTask )
// Fills a reply with predefined unsupported task -information
// -----------------------------------------------------------------------------
//  
TInt CSConPCD::RebootTaskL( const CSConTask* aTask )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNotSupported );
    iTaskNumber++;
    TSConMethodName method( aTask->GetServiceId() );
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( reply );
    CSConTaskReply* taskReply = new (ELeave) CSConTaskReply();
    CleanupStack::PushL( taskReply );
    
    TBool complete( ETrue );
    taskReply->InitializeL( method, KSConCodeNotSupported, complete );
    taskReply->iTaskId = iTaskNumber;
    
    reply->iNoTasks = EFalse;
    User::LeaveIfError( reply->iTasks.Append( taskReply ) );
    CleanupStack::Pop( taskReply );
    
    StoreReply( reply );
    CleanupStack::Pop( reply );
        
    LOGGER_WRITE_1( "CSConPCD::RebootTask() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCD::GetMetadataTask( const CSConTask* aTask )
// Fills a reply with predefined GetMetadata task -information
// -----------------------------------------------------------------------------
//
void CSConPCD::GetMetadataTaskL( CSConTask& aTask )
    {
    TRACE_FUNC_ENTRY;
    
    aTask.iTaskId = ++iTaskNumber;
    
    //Set progress value "task accepted for execution"
    aTask.SetProgressValue( KSConCodeTaskCreated );
    aTask.SetCompleteValue( EFalse );
    
    SConMetadata::ProcessTask( aTask, iFs );
    
    //Get status for this task
 
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( reply );
    CSConTaskReply* taskReply = new (ELeave) CSConTaskReply();
    CleanupStack::PushL( taskReply );
    taskReply->InitializeL( aTask );
    User::LeaveIfError( reply->iTasks.Append( taskReply ) );
    CleanupStack::Pop( taskReply );
    
    if ( !aTask.GetComplete() )
        {
        // clean all unneccessary data from the reply packet
        taskReply->CleanTaskData();
        }
    
    
    StoreReply( reply );
    CleanupStack::Pop( reply ); 
 
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCD::GetStatus( TInt aTask, TBool aAll )
// Collects a status of a specified task / all tasks
// -----------------------------------------------------------------------------
//
TInt CSConPCD::GetStatusL( TInt aTask, TBool aAll )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    // create mergeReply object. Store installer and br replys here.
    CSConStatusReply* mergeReply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( mergeReply );
            
    
    CSConStatusReply* brReply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( brReply );
    iBackupRestoreQueue->GetQueueStatusL( aTask, aAll, brReply );
    
    
    // Create installerReply and populate it
    CSConStatusReply* installerReply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( installerReply );
    iInstallerQueue->GetQueueStatusL( aTask, aAll, installerReply );
    
    if ( !installerReply->iNoTasks || !brReply->iNoTasks )
        {
        mergeReply->iNoTasks = EFalse;
        }
    else
        {
        mergeReply->iNoTasks = ETrue;
        }
        
    for ( TInt i = 0; i < installerReply->iTasks.Count(); i++ )
        {
        CSConTaskReply* temp = installerReply->iTasks[i]->CopyAndFreeL();
        CleanupStack::PushL( temp );
        mergeReply->iTasks.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    
    // installer replys are copied to mergereply, delete installerReply
    CleanupStack::PopAndDestroy( installerReply );
    
    // do same for br reply
    for ( TInt i = 0; i < brReply->iTasks.Count(); i++ )
        {
        CSConTaskReply* temp = brReply->iTasks[i]->CopyAndFreeL();
        CleanupStack::PushL( temp );
        mergeReply->iTasks.AppendL( temp );
        CleanupStack::Pop( temp );
        }
    
    CleanupStack::PopAndDestroy( brReply );
    
    CleanupStack::Pop( mergeReply );
    // store reply
    StoreReply( mergeReply );
    
    LOGGER_WRITE_1( "CSConPCD::GetStatus() : returned %d", ret );
    return ret;
    }
        
// -----------------------------------------------------------------------------
// CSConPCD::Cancel( TInt aTask, TBool aAll )
// Cancels a specified task / all tasks
// -----------------------------------------------------------------------------
//
TInt CSConPCD::CancelL( TInt aTask, TBool aAll )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    iInstallerQueue->CancelTask( aTask, aAll );
    iBackupRestoreQueue->CancelTask( aTask, aAll );
    
    reply->iNoTasks = ETrue;
    
    StoreReply( reply );
    
    LOGGER_WRITE_1( "CSConPCD::CancelL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConPCD::StoreReply( CSConStatusReply*& aReply )
// Stores a reply
// -----------------------------------------------------------------------------
//
void CSConPCD::StoreReply( CSConStatusReply*& aReply )  
    {
    TRACE_FUNC_ENTRY;
    if ( iLatestReply )
        {
        delete iLatestReply;
        iLatestReply = NULL;
        }
        
    iLatestReply = aReply;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConPCD::UpdateDeviceInfo( TInt aMaxObjectSize )
// Creates a reply for UpdateDeviceInfo task
// -----------------------------------------------------------------------------
//
TInt CSConPCD::UpdateDeviceInfoL( TInt aMaxObjectSize )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TSConMethodName method( EUpdateDeviceInfo );
    
    CSConStatusReply* reply = new (ELeave) CSConStatusReply();
    CleanupStack::PushL( reply );
    CSConTaskReply* taskReply = new (ELeave) CSConTaskReply();
    CleanupStack::PushL( taskReply );
    
    
    TBool complete( ETrue );
    TInt progress( KSConCodeTaskCompleted );
    
    taskReply->InitializeL( method, progress, complete );
    taskReply->iTaskId = iTaskNumber;
        
    TInt arrayLength = sizeof KSupportedMethods / sizeof KSupportedMethods[0];
    
    for ( TInt i = 0; i < arrayLength; i++ )
        {
        switch( KSupportedMethods[i].method )
            {
            case EGetDataSize :
                taskReply->iDevInfoParams->iGetSizeSupp = 
                KSupportedMethods[i].support;
                break;
            case EInstall :
                taskReply->iDevInfoParams->iInstallSupp = 
                KSupportedMethods[i].support;
                break;
            case EListDataOwners :
                taskReply->iDevInfoParams->iDataOwnersSupp = 
                KSupportedMethods[i].support;
                break;
            case EListInstalledApps :
                taskReply->iDevInfoParams->iInstAppsSupp = 
                KSupportedMethods[i].support;
                break;
            case ERequestData :
                taskReply->iDevInfoParams->iReqDataSupp = 
                KSupportedMethods[i].support;
                break;
            case ESetBURMode :
                taskReply->iDevInfoParams->iSetBURModeSupp = 
                KSupportedMethods[i].support;
                break;
            case ESetInstParams :
                taskReply->iDevInfoParams->iInstParamsSupp = 
                KSupportedMethods[i].support;
                break;
            case ESupplyData :
                taskReply->iDevInfoParams->iSupplyDataSupp = 
                KSupportedMethods[i].support;
                break;
            case EUninstall :
                taskReply->iDevInfoParams->iUninstallSupp = 
                KSupportedMethods[i].support;
                break;
            case EReboot :
                taskReply->iDevInfoParams->iRebootSupp = 
                KSupportedMethods[i].support;
                break;
            default :
                break;
            }
        }
    
    taskReply->iDevInfoParams->iVersion.Copy( KConMLVersion );
    taskReply->iDevInfoParams->iMaxObjectSize = KMaxObjectSize;
    
    reply->iNoTasks = EFalse;

    User::LeaveIfError( reply->iTasks.Append( taskReply ) );
    StoreReply( reply );
    
    CleanupStack::Pop( taskReply );
    CleanupStack::Pop( reply );
    LOGGER_WRITE_1( "CSConPCD::UpdateDeviceInfo() : returned %d", ret );
    return ret;
    }

// End of file
