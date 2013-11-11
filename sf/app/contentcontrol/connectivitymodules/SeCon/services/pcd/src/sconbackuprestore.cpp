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
* Description:  CSConBackupRestore implementation
*
*/


// INCLUDE FILES
#include <pathinfo.h>

#include "sconbackuprestore.h"
#include "sconsbeclient.h"
#include "sconbrqueue.h"
#include "sconpcdconsts.h"

#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSConBackupRestore* CSConBackupRestore::NewL( CSConBackupRestoreQueue* aQueue,
//                                          cost TInt& aMaxObjectSize )
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConBackupRestore* CSConBackupRestore::NewL( CSConBackupRestoreQueue* aQueue,
                                              RFs& aFs )
    {
    CSConBackupRestore* self = new (ELeave) CSConBackupRestore( aQueue, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSConBackupRestore::CSConBackupRestore( CSConBackupRestoreQueue* aQueue )
// Constructor
// -----------------------------------------------------------------------------
//
CSConBackupRestore::CSConBackupRestore( CSConBackupRestoreQueue* aQueue, RFs& aFs ) : 
            CActive( EPriorityStandard ), iQueue( aQueue ), iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CSConBackupRestore::ConstructL( const TInt aMaxObjectSize )
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConBackupRestore::ConstructL()
    {
    iSBEClient = CSConSBEClient::NewL( iFs );
    }   
    
// -----------------------------------------------------------------------------
// CSConBackupRestore::~CSConBackupRestore()
// Destructor
// -----------------------------------------------------------------------------
//
CSConBackupRestore::~CSConBackupRestore()
    {
    if( iSBEClient )
        {
        iSBEClient->Cancel();
        delete iSBEClient;
        iSBEClient = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CSConBackupRestore::StartBackupRestore( TInt aTaskId )
// Starts backup-restore task
// -----------------------------------------------------------------------------
//
void CSConBackupRestore::StartBackupRestore( TInt aTaskId )
    {
    iBackupRestoreActive = ETrue;
    CSConTask* task = NULL;
    
    TInt ret = iQueue->GetTask( aTaskId, task );

    if( aTaskId > 0 && ret != KErrNotFound )
        {
        iCurrentTask = aTaskId;
        iQueue->SetTaskProgress( aTaskId, KSConCodeProcessingStarted );
        }
    
    switch( task->GetServiceId() )
        {
        case ESetBURMode :
            iQueue->ChangeQueueProcessStatus();
            iSBEClient->SetBURMode( iStatus, task );
            break;
        case EListPublicFiles :
            iSBEClient->ListPublicFiles( iStatus, task );
            break;
        case EListDataOwners :
            iSBEClient->ListDataOwners( iStatus, task );
            break;
        case EGetDataSize :
            iSBEClient->GetDataSize( iStatus, task );
            break;
        case ERequestData :
            iSBEClient->RequestData( iStatus, task );
            break;
        case EGetDataOwnerStatus :
            iSBEClient->GetDataOwnerStatus( iStatus, task );
            break;
        case ESupplyData :
            iSBEClient->SupplyData( iStatus, task );
            break;      
        default :
            break;
        }
    
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CSConBackupRestore::StopBackupRestore( TInt aTaskId )
// Stops executing backup-restore task
// -----------------------------------------------------------------------------
//
void CSConBackupRestore::StopBackupRestore( TInt aTaskId )
    {
    TRACE_FUNC_ENTRY;
     //if the task is active, do cancel
     if( iCurrentTask == aTaskId )
        {
        iBackupRestoreActive = EFalse;
        iSBEClient->Cancel();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConBackupRestore::Reset()
// Resets backup
// -----------------------------------------------------------------------------
//  
void CSConBackupRestore::Reset()
    {
    TRACE_FUNC_ENTRY;
    if( iSBEClient )
        {
        iSBEClient->Cancel();
        delete iSBEClient;
        iSBEClient = NULL;
        }
        
    TRAP_IGNORE( iSBEClient = CSConSBEClient::NewL( iFs ) );
    
    iBackupRestoreActive = EFalse;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConBackupRestore::BackupRestoreActive()
// returns backup-restore activity status
// -----------------------------------------------------------------------------
//
TBool CSConBackupRestore::BackupRestoreActive() const
	{
	return iBackupRestoreActive;
	}
// -----------------------------------------------------------------------------
// CSConBackupRestore::DoCancel()
// Implementation of CActive::DoCancel()
// -----------------------------------------------------------------------------
//  
void CSConBackupRestore::DoCancel()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConBackupRestore::RunL()
// Implementation of CActive::RunL()
// -----------------------------------------------------------------------------
//          
void CSConBackupRestore::RunL()
    {
    TRACE_FUNC;
    iQueue->CompleteTask( iCurrentTask, iStatus.Int() );
    iBackupRestoreActive = EFalse;
    }
    
// End of file
