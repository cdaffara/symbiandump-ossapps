/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupEngineImpl implementation
*
*
*/

#include "CMMCScBkupEngineImpl.h"

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupState.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupArchive.h"
#include "MMCScBkupOperations.h"
#include "MMCScBkupArchiveUtils.h"
#include "CMMCScBkupStateFactory.h"
#include "MMCScBkupPhoneModelUtils.h"
#include "MMMCScBkupEngineObserver.h"
#include "CMMCScBkupFileListCollection.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupOperationParameters.h"
#include "CMMCScBkupArchiveInfo.h"
#include "BkupEngine.hrh"
#include <pathinfo.h>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CMMCScBkupEngineImpl()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupEngineImpl::CMMCScBkupEngineImpl( RFs& aFsSession )
:   CActive( CActive::EPriorityIdle ), 
    iFsSession(aFsSession), 
    iCurrentArchive(0),
    iTotalProgress(0),
    iActiveDataProcessingOngoing(EFalse)
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::~CMMCScBkupEngineImpl()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupEngineImpl::~CMMCScBkupEngineImpl()
    {
    Cancel();
    //
    TRAP_IGNORE(CleanupL());

    iArchives.Close();
    iDataOwners.Close();
    iFileLists.Close();
    delete iSBEClient;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::ConstructL()
// 
// Second phase constructor
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::ConstructL()
    {
    __LOG("CMMCScBkupEngineImpl::ConstructL() - START");

#ifdef MAKE_SBE_LOGGING_DIRECTORY
    _LIT( KSBELoggingDir, "C:\\Logs\\connect\\" );
    iFsSession.MkDirAll( KSBELoggingDir );
#endif
#ifdef MAKE_MMCSCBKUP_LOGGING_DIRECTORY
    iFsSession.MkDirAll( KMMCScBkupLoggingFullPath );
#endif
    
    __LOG("CMMCScBkupEngineImpl::ConstructL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::NewL()
// 
// Static constructor
// ---------------------------------------------------------------------------
CMMCScBkupEngineImpl* CMMCScBkupEngineImpl::NewL( RFs& aFsSession )
    {
    CMMCScBkupEngineImpl* self = new(ELeave) CMMCScBkupEngineImpl( aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::StartOperationL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::StartOperationL(TMMCScBkupOperationType aOperation, MMMCScBkupEngineObserver& aObserver, CMMCScBkupOpParamsBase* aParams)
    {
    __ASSERT_DEBUG(DrvOperation() == EMMCScBkupOperationTypeIdle, User::Invariant()); // 

    // Setup our observer - we will report progress via this interface
    iObserver = &aObserver;

    // Take ownership of the parameters immediately
    SetParameters(aParams);
    
    if(!iSBEClient)
        {
        __LOG("CMMCScBkupEngineImpl::ConstructL() - creating SBE client...");
        iSBEClient = CSBEClient::NewL();
        }

#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    // Create a logging directory for logging purposes and empty contents
    // in order to have the log only from last backup or restore operation
    CFileMan* fileMan = CFileMan::NewL( iFsSession );
    CleanupStack::PushL( fileMan );
    TParse* path = new(ELeave) TParse();
    
    CleanupStack::PushL(path);
    path->Set(KMMCScBkupLoggingFullPathAndName, NULL, NULL);
    fileMan->Delete( path->Path(), CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( path->Path() );

    CleanupStack::PopAndDestroy(2, fileMan );
#endif
    
    // Prepare supporting objects
    PrepareObjectsL();

    // Work out what to do...
    CreateFactoryL( aOperation );
    //
    switch(aOperation)
        {
    case EMMCScBkupOperationTypeIdle:
        __ASSERT_ALWAYS(EFalse, User::Invariant());
        break;
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        PrepareForBackupL( DrvOperation() == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        PrepareForRestoreL( DrvOperation() == EMMCScBkupOperationTypePartialRestore );
        break;
        }

    // Do this last, after everything that can leave has been executed
    iOperationType = aOperation;

    // Notify the observer we're starting up
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationStarting);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::ValidArchiveForRestoreL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupEngineImpl::ValidArchiveForRestoreL( const TDesC& aFileName )
    {
    const TBool valid = CMMCScBkupArchive::ValidArchiveForRestoreL( iFsSession, aFileName );
    return valid;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::TotalOperationSizeL()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupEngineImpl::TotalOperationSizeL() const
    {
    const TInt64 size = iDataOwners[iCurrentArchive]->TotalOperationalSizeL();
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::RebootRequired()
// 
// NOTE: This method is not used in S60 3.x - Sysap handles the reboot
// ---------------------------------------------------------------------------
TBool CMMCScBkupEngineImpl::RebootRequired() const
    {
    // Criteria for a reboot are:
    //
    // 1) One or more data owners indicate that a reboot is necessary
    //    via their backup_registration.xml files.
    //
    // 2) The current operation is some form of restore (partial or full).
    
    const TMMCScBkupOperationType currentOp = DrvOperation();
    __LOG1("CMMCScBkupEngineImpl::RebootRequired() - operation type is: %d", currentOp);

    TBool singleDataOwnerNeedsReboot = EFalse;
    
    for(TInt i = 0; i < iDataOwners.Count(); i++)
        {
        singleDataOwnerNeedsReboot |= iDataOwners[i]->RebootRequired();
        }
        
    __LOG1("CMMCScBkupEngineImpl::RebootRequired() - D.O requires reboot?: %d", singleDataOwnerNeedsReboot);
    
    const TBool rebootRequired = singleDataOwnerNeedsReboot && 
        ( currentOp == EMMCScBkupOperationTypeFullRestore || 
          currentOp == EMMCScBkupOperationTypePartialRestore );
    __LOG1("CMMCScBkupEngineImpl::RebootRequired() - final rebootRequired value: %d", rebootRequired);

    // We return the value to the caller - the UI will display appropriate
    // UI dialogs and reset the machine as appropriate.
    return rebootRequired;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvADI()
// 
// 
// ---------------------------------------------------------------------------
MMMCScBkupArchiveDataInterface& CMMCScBkupEngineImpl::DrvADI() const
    {
    return iArchives[iCurrentArchive]->ADI();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvArchive()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchive& CMMCScBkupEngineImpl::DrvArchive() const
    {
    __ASSERT_ALWAYS(iCurrentArchive < iArchives.Count(), User::Invariant());
    
    return *iArchives[iCurrentArchive];
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvSecureBackupClient()
// 
// 
// ---------------------------------------------------------------------------
CSBEClient& CMMCScBkupEngineImpl::DrvSecureBackupClient() const
    {
    return *iSBEClient;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvOperation()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupOperationType CMMCScBkupEngineImpl::DrvOperation() const
    {
    return iOperationType;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvParamsBase()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupOpParamsBase& CMMCScBkupEngineImpl::DrvParamsBase() const
    {
    return *iParameters;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvDataOwners()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataOwnerCollection& CMMCScBkupEngineImpl::DrvDataOwners() const
    {
    __ASSERT_ALWAYS(iCurrentArchive < iDataOwners.Count(), User::Invariant());
    
    return *iDataOwners[iCurrentArchive];
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvDataOwnersAll()
// 
// 
// ---------------------------------------------------------------------------
RPointerArray<CMMCScBkupDataOwnerCollection>& CMMCScBkupEngineImpl::DrvDataOwnersAll()
    {
    return iDataOwners;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvFileList()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupFileListCollection& CMMCScBkupEngineImpl::DrvFileList() const
    {
    __ASSERT_ALWAYS(iCurrentArchive < iFileLists.Count(), User::Invariant());
    
    return *iFileLists[iCurrentArchive];
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvLastCategory()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupEngineImpl::DrvLastCategory() const
    {
    return ( (iCurrentArchive + 1) == iArchives.Count() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvStoreTotalProgress()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::DrvStoreTotalProgress(TInt64 aProgress)
    {
    iTotalProgress += aProgress;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvTotalProgress()
// 
// 
// ---------------------------------------------------------------------------
TInt64 CMMCScBkupEngineImpl::DrvTotalProgress() const
    {
    return iTotalProgress;
    }

// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DrvProgressHandler()
// 
// 
// ---------------------------------------------------------------------------
MMMCScBkupProgressObserver& CMMCScBkupEngineImpl::DrvProgressHandler() const
    {
    CMMCScBkupEngineImpl* self = const_cast< CMMCScBkupEngineImpl* > (this);
    return *self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::MMCScBkupHandleProgress()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::MMCScBkupHandleProgress( TInt aAmountCompleted )
    {
    __ASSERT_ALWAYS( aAmountCompleted >= 0, User::Invariant() );
    if  (aAmountCompleted > 0)
        {
        __LOG1("CMMCScBkupEngineImpl::MMCScBkupHandleProgress() - amount: %d", aAmountCompleted);
        iCumulativeProgress += aAmountCompleted;
        
        // Notify the observer that we've calculated the operational size
        NotifyObserver( MMMCScBkupEngineObserver::ECommonProgress, iCumulativeProgress );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::MMCScBkupHandleProgressDomainUnderstood()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::MMCScBkupHandleProgressDomainUnderstood( TInt aTotalProgressAmount )
    {
    __LOG1("CMMCScBkupEngineImpl::MMCScBkupHandleProgressDomainUnderstood() - ##### TOTAL PROGRESS AMOUNT IS: %d", aTotalProgressAmount);

    // Notify the observer that we've calculated the operational size
    NotifyObserver( MMMCScBkupEngineObserver::ECommonSizeOfTaskUnderstood, aTotalProgressAmount );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::MMCScBkupHandleFreeSpace()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupEngineImpl::MMCScBkupHandleFreeSpace( TInt aPercentualFree )
    {
    TInt error(KErrNone);
    
    TRAP_IGNORE( error = iObserver->HandleBkupEngineEventL( MMMCScBkupEngineObserver::EBackupAnalysingData, aPercentualFree ) );
    
    return error;
    }

// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::MMCScBkupStartBackuping()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::MMCScBkupStartBackuping( TBool aProceed )
    {
    __LOG1("CMMCScBkupEngineImpl::MMCScBkupStartBackuping() - Disk space validation done, proceed %d", aProceed);
    
    if( !aProceed )
        {
        // If there is no space to carry out backup, "restore" existing backup file,
        // so that it can still be restored.
        for(TInt i = 0; i < iArchives.Count(); i++)
            {
            iArchives[i]->RestoreOldArchive( );
            }
        }
    else
        {
        // Clean up existing backup files.
        for(TInt i = 0; i < iArchives.Count(); i++)
            {
            iArchives[i]->DeleteOldArchive( );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::RunL()
    {
    __LOG(" ");
    __LOG(" ");
    if  ( !CurrentStateAvailable() )
        {
        __LOG2("CMMCScBkupEngineImpl::RunL() - START - iStatus: %d, iCurrentArchive %d", 
            iStatus.Int(), iCurrentArchive);
        }
    else
        {
        __LOG3("CMMCScBkupEngineImpl::RunL() - START - iStatus: %d, state: 0x%08x, iCurrentArchive %d", 
            iStatus.Int(), CurrentState().StateId().iUid, iCurrentArchive);
        }

    User::LeaveIfError(iStatus.Int());
    
    // Get the state that just finished - its always the head item
    const TBool stateStepAvailable = CurrentStateAvailable();
    __ASSERT_ALWAYS(stateStepAvailable, User::Invariant());
    CMMCScBkupState& currentState = CurrentState();
  
    // Store completed state id as we're about to delete the object
    // so it won't be available afterwards...
    const TMMCScBkupStateId completedState = currentState.StateId();

    // Identify which state should run next. In effect each state
    // defines the overall state machine. This is required since
    // some states need to dynamically change which state is executed
    // next. A good example is the archive op for active data - when
    // one or more active data clients are not ready, then the next
    // state should be to requery their readyness. After their
    // readyness has been ascertained, then any DO's that have now
    // become ready should be archived. This process continues
    // until all Active Data owners have provided all their data.
    TMMCScBkupStateId nextState;
    
    __LOG3("CMMCScBkupEngineImpl::RunL() - iActiveDataProcessingOngoing %d, completedState 0x%08x, nextState 0x%08x", 
            iActiveDataProcessingOngoing, completedState.iUid, currentState.NextStateId().iUid);

    // Identify should we run current state again, i.e. run that state 
    // for next category. If state does not require processing of same
    // state several times, then move on to next state.
    if( currentState.CategorySpecific() != CMMCScBkupState::EStateOnce )
        {
        // For simplicity let's collect active data for each category in row.
        // This means checking data owner status before proceeding to next category.
        if( iActiveDataProcessingOngoing )
            {
            iActiveDataProcessingOngoing = EFalse;
            nextState = currentState.NextStateId();
            }
        else 
            {
            if( completedState == KMMCScBkupStateIdArchiveOpActiveData )
                {
                if( currentState.NextStateId() == KMMCScBkupStateIdGetDataOwnerStatuses )
                    {
                    // There is still some active data to be processed
                    iActiveDataProcessingOngoing = ETrue;
                    nextState = KMMCScBkupStateIdGetDataOwnerStatuses;
                    }
                }
            
            if( !iActiveDataProcessingOngoing )
                {
                if( ++iCurrentArchive < iArchives.Count() )
                    {
                    nextState = completedState;
                    }
                else
                    {
                    iCurrentArchive = 0;
                    nextState = currentState.NextStateId();
                    }
                }
            }
        }
    else
        {
        iCurrentArchive = 0;
        nextState = currentState.NextStateId();
        }

    if(nextState == completedState && currentState.CategorySpecific() == CMMCScBkupState::EStatePerCategoryCommon)
        {
        __LOG2("CMMCScBkupEngineImpl::RunL() - object not recreated for state: 0x%08x, iCurrentArchive %d", 
            nextState.iUid, iCurrentArchive);
        }
    else        
        {
        // Remove the completed state (destroys 'currentState')
        DestroyCurrentState();
        // Identify the next state. Causes RunL to be called when the state
        // has finished executing
        __LOG2("CMMCScBkupEngineImpl::RunL() - preparing state: 0x%08x, iCurrentArchive %d", 
            nextState.iUid, iCurrentArchive);
        
        PrepareNextStateL( nextState );
        }

    if  (CurrentStateAvailable())
        {
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        CMMCScBkupState& newState = CurrentState();
        if( newState.CategorySpecific() != CMMCScBkupState::EStateOnce )
            {
            __LOG2("CMMCScBkupEngineImpl::RunL() - executing category specific state: 0x%08x for category 0x%x", 
                nextState.iUid, iArchives[iCurrentArchive]->Category().iFlags);
            }
        else
            {
            __LOG1("CMMCScBkupEngineImpl::RunL() - executing non category specific state: 0x%08x", nextState.iUid);
            }
#endif
        ExecuteStateL();
        }
    else
        {
        if ( iSBEClient )
        	{
        	MMCScBkupSBEUtils::EndBackupOrRestoreL( *iSBEClient );
        	}
        // Signal we are about to finish, so that bkupchecker can be notified.
        NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationPrepareEnded);
        // We've finished
        CleanupL();
        NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationEnded);
        // Remove observer
        iObserver = NULL;
        }

    __LOG3("CMMCScBkupEngineImpl::RunL() - END - nextState: 0x%08x, isActive: %d, iStatus: %d", nextState.iUid, IsActive(), iStatus.Int());
    __LOG(" ");
    __LOG(" ");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::DoCancel()
    {
    __LOG("CMMCScBkupEngineImpl::DoCancel() - START");

    if  (CurrentStateAvailable())
        {
        __LOG1("CMMCScBkupEngineImpl::DoCancel() - current state is: 0x%08x", CurrentState().StateId().iUid);

        // Our request should be completed by the state
        CurrentState().Cancel();
        }
        
    // Signal we are about to finish, so that bkupchecker can be notified.
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationPrepareEnded);
    // Cleanup
    TRAP_IGNORE( CleanupL( KErrCancel ) );

    // Ensure observer is informed
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationError, KErrCancel);
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationEnded, KErrCancel);
    // Remove observer
    iObserver = NULL;

    __LOG("CMMCScBkupEngineImpl::DoCancel() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupEngineImpl::RunError(TInt aError)
    {
    __LOG1("CMMCScBkupEngineImpl::RunError() - START - aError: %d", aError);

    // Inform observer of error
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationError, aError);
    // Signal we are about to finish, so that bkupchecker can be notified.
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationPrepareEnded);
    // Cleanup
    TRAP_IGNORE( CleanupL( aError ) );

    // Finalize observer
    NotifyObserver(MMMCScBkupEngineObserver::ECommonOperationEnded, aError);
    // Remove observer
    iObserver = NULL;

    // Return KErrNone to stop the scheduler from panicking our thread

    __LOG("CMMCScBkupEngineImpl::RunError() - END");
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CompleteOwnRequest()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::CompleteOwnRequest(TInt aCompletionCode, TBool aSetActive)
    {
    if  (aSetActive)
        {
        SetActive();
        }
    //
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aCompletionCode);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::NotifyObserver()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::NotifyObserver( MMMCScBkupEngineObserver::TEvent aEvent, TInt aAssociatedData )
    {
    if  ( iObserver )
        {
        TRAP_IGNORE( iObserver->HandleBkupEngineEventL( aEvent, aAssociatedData ) );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::PrepareForBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::PrepareForBackupL( TBool aPartial )
    {
#ifdef DEBUGGING_DATA_TRANSFER
    CleanBackupFilesL();
#endif
    
    // Open the archives for writing
    for(TInt i = 0; i < iArchives.Count(); i++)
        {
        const TPtrC pArchiveName( DrvParamsBase().FileName(i) );
        iArchives[i]->OpenForWritingL( pArchiveName );
        }

    // Initialise our state model
    if  ( !aPartial )
        {
        // Starting a FULL BACKUP
        PrepareNextStateL( KMMCScBkupStateArchiveOpArchiveHeader );
        ExecuteStateL();
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::PrepareForRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::PrepareForRestoreL( TBool aPartial )
    {
#ifdef DEBUGGING_DATA_TRANSFER
    ClearRestoreFilesL();
#endif

    // Open the archives for reading
    for(TInt i = 0; i < iArchives.Count(); i++)
        {
        const TPtrC pArchiveName( DrvParamsBase().FileName(i) );
        iArchives[i]->OpenForReadingL( pArchiveName );

#ifdef RD_FILE_MANAGER_BACKUP
        // Validate archive content is not altered
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        TUint32 startTime = User::NTickCount();
#endif
        TInt validArchiveForRestore = MMCScBkupArchiveUtils::ValidateArchiveCrcsL( iFsSession, pArchiveName );
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        __LOG2("CMMCScBkupEngineImpl::PrepareForRestoreL - crcs validation result %d, ticks in ms %u", 
            validArchiveForRestore, User::NTickCount() - startTime);
#endif
        if(validArchiveForRestore == EFalse)
            {
            User::Leave(KErrCorrupt);
            }
#endif
        }

    // Initialise our state model
    if  ( !aPartial )
        {
        // Starting a FULL RESTORE
        PrepareNextStateL( KMMCScBkupStateArchiveOpArchiveHeader );
        ExecuteStateL();
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::SetParameters()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::SetParameters(CMMCScBkupOpParamsBase* aParameters)
    {
    __ASSERT_ALWAYS(aParameters != NULL, User::Invariant());
    //
    delete iParameters;
    iParameters = aParameters;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CreateFactoryL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::CreateFactoryL(TMMCScBkupOperationType aOperation)
    {
    __ASSERT_DEBUG(iFactory == NULL, User::Invariant());
    //
    CMMCScBkupStateFactory* factory = CMMCScBkupStateFactory::FactoryByOperationTypeLC(aOperation);
    delete iFactory;
    iFactory = factory;
    CleanupStack::Pop( factory );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CurrentStateAvailable()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupEngineImpl::CurrentStateAvailable() const
    {
    return (iCurrentState != NULL);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CurrentStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupEngineImpl::CurrentStateId() const
    {
    __ASSERT_ALWAYS(CurrentStateAvailable(), User::Invariant());
    return CurrentState().StateId();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CurrentState()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupState& CMMCScBkupEngineImpl::CurrentState()
    {
    return *iCurrentState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CurrentState()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupState& CMMCScBkupEngineImpl::CurrentState() const
    {
    return *iCurrentState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::PrepareNextStateL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::PrepareNextStateL( TMMCScBkupStateId aNextState )
    {
    if  ( aNextState != KMMCScBkupStateIdOperationComplete )
        {
        CMMCScBkupState* nextState = iFactory->GetStateLC( aNextState, *this );
        delete iCurrentState;
        iCurrentState = nextState;
        CleanupStack::Pop( nextState );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::DestroyCurrentState()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::DestroyCurrentState()
    {
    delete iCurrentState;
    iCurrentState = NULL;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::ExecuteStateL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::ExecuteStateL()
    {
    const TBool stateStepAvailable = CurrentStateAvailable();
    __ASSERT_ALWAYS(stateStepAvailable, User::Invariant());
    //
    CMMCScBkupState& currentState = CurrentState();
    //
    currentState.ExecuteL(iStatus);
    SetActive();

    // RunL will now be called when the state has completed execution
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CleanupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::CleanupL( TInt aError )
    {
    __LOG1("CMMCScBkupEngineImpl::CleanupL() - START - aError: %d", aError);

    // If SBE died, then recreate
    if  ( aError == KErrServerTerminated )
        {
        __LOG("CMMCScBkupEngineImpl::CleanupL() - assuming SBE panicked - re-creating SBE session...");
        CSBEClient* newSBEClient = CSBEClient::NewL();
        delete iSBEClient;
        iSBEClient = newSBEClient;
        }


    __LOG("CMMCScBkupEngineImpl::CleanupL() - destroying current state...");
    DestroyCurrentState();

    const TBool backupOrRestoreUnderway = MMCScBkupSBEUtils::PhoneIsInBackupOrRestoreModeL();
    __LOG1("CMMCScBkupEngineImpl::CleanupL() - backupOrRestoreUnderway: %d", backupOrRestoreUnderway);
    if  ( backupOrRestoreUnderway && iSBEClient )
        {
        // Must end backup or restore
        __LOG("CMMCScBkupEngineImpl::CleanupL() - ending backup or restore...");
        MMCScBkupSBEUtils::EndBackupOrRestoreL( *iSBEClient );
        }

    // Ensure the archive is closed
    if  ( iArchives.Count() )
        {
        __LOG("CMMCScBkupEngineImpl::CleanupL() - closing archives...");
        for(TInt i = 0; i < iArchives.Count(); i++)
            {
            iArchives[i]->Close( aError );
            }
        
        __LOG("CMMCScBkupEngineImpl::CleanupL() - reseting archive array...");
        iArchives.ResetAndDestroy();
        }

    // Destroy old parameters
    __LOG("CMMCScBkupEngineImpl::CleanupL() - destroying parameters...");
    delete iParameters;
    iParameters = NULL;
        
    // Set back to idle
    __LOG("CMMCScBkupEngineImpl::CleanupL() - setting operation status to idle...");
    iOperationType = EMMCScBkupOperationTypeIdle;
        
    // Dispose of our collections
    __LOG("CMMCScBkupEngineImpl::CleanupL() - destroying data owners and file lists...");
    iDataOwners.ResetAndDestroy();
    iFileLists.ResetAndDestroy();

    // Dispose of state factory
    __LOG("CMMCScBkupEngineImpl::CleanupL() - destroying factory...");
    delete iFactory;
    iFactory = NULL;
    
    // Reset progress
    iCumulativeProgress = 0;

    // Reduce memory consumption by deleting SBE client. Session will be closed
    // and SBE frees reserved (e.g. 128 kB...) memory pool.
    if(iSBEClient)
        {
        __LOG("CMMCScBkupEngineImpl::CleanupL() - deleting SBE client...");
        delete iSBEClient;
        iSBEClient = NULL;
        }
    
    __LOG("CMMCScBkupEngineImpl::CleanupL() - END");
    }
    

// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::PrepareObjectsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::PrepareObjectsL()
    {
    __LOG("CMMCScBkupEngineImpl::PrepareObjectsL() - creating objects...");

    iTotalProgress = 0;
    iCurrentArchive = 0;
    
    const RMMCScBkupPointerArray<CMMCScBkupArchiveInfo>& archives = DrvParamsBase().ArchiveInfos();
    TInt count = archives.Count();

    if(count == 0)
        {
        __LOG("CMMCScBkupEngineImpl::PrepareObjectsL() - nothing to be done, leaving...");
        User::Leave(KErrCancel);
        }

    for(TInt i = 0; i < count; i++)
        {
        CMMCScBkupArchive* archive;
        archive = CMMCScBkupArchive::NewL( iFsSession, DrvProgressHandler(), *this, archives[i]->Category() );
        CleanupStack::PushL( archive );
        iArchives.AppendL( archive );
        CleanupStack::Pop( archive );
    
        CMMCScBkupDataOwnerCollection* dataOwners;
        dataOwners = CMMCScBkupDataOwnerCollection::NewL( *this, archives[i]->Category() );
        CleanupStack::PushL( dataOwners );
        iDataOwners.AppendL( dataOwners );
        CleanupStack::Pop( dataOwners );
        
        CMMCScBkupFileListCollection* fileList;
        fileList = CMMCScBkupFileListCollection::NewL( archives[i]->Category(), iFsSession );
        CleanupStack::PushL( fileList );
        iFileLists.AppendL( fileList );
        CleanupStack::Pop( fileList );
        }
    }
    
TBool CMMCScBkupEngineImpl::DeleteArchivesL( RPointerArray< CMMCScBkupArchiveInfo >& aArchives ) const
    {
    TBool ret( ETrue );
    TInt i( 0 );
    TInt err;
    while ( i < aArchives.Count() )
        {
        CMMCScBkupArchiveInfo* archiveInfo = aArchives[ i ];
        err = iFsSession.SetAtt( archiveInfo->FileName(), KEntryAttNormal, !KEntryAttNormal );
        err = iFsSession.Delete( archiveInfo->FileName() );
        if( err!=KErrNone )
            {
            ret = EFalse ;
            }
        aArchives.Remove( i );
        delete archiveInfo;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::ListArchivesL()
// 
// Provide a list of valid (both content and category do match) archives 
// available on all drives' backup folder
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::ListArchivesL(
        RPointerArray< CMMCScBkupArchiveInfo >& aArchives,
        CMMCScBkupOpParamsBase* aParams,
        const TUint32 aDriveAttMatch,
        const TInt aDriveMatch )
    {
    __LOG("CMMCScBkupEngineImpl::ListArchivesL() - START");

    for(TInt i = 0; i < KMaxDrives; i++)
        {
        // Check is drive number allowed
        if ( aDriveMatch != KErrNotFound && aDriveMatch != i )
            {
            continue;
            }
        // Check are drive attributes allowed
        TBool supported( EFalse );
        TDriveInfo driveInfo;
        if ( iFsSession.Drive( driveInfo, i ) == KErrNone )
            {
            if ( driveInfo.iDriveAtt & aDriveAttMatch )
                {
                supported = ETrue;
                }
            }
        if ( !supported )
            {
            continue;
            }

        const TDriveUnit driveUnit(i);
        const TDriveName driveName(driveUnit.Name());
        CDir* pFiles = NULL;
        TFileName path;
        path.Append(driveName);
        path.Append(KBackUpFolder());
        path.Append(KBackUpFiles());
        
        // Scan for all archives 
        iFsSession.GetDir(path, KEntryAttMatchMask, ESortNone, pFiles);
        CleanupStack::PushL(pFiles);
        
        if(pFiles)
            {
    		TInt count = pFiles->Count();
    		for (TInt x = 0; x < count; x++)
    			{
    			const TEntry& entryPtr = (*pFiles)[x];
    			TEntry entry(entryPtr);
                TFileName pathAndFile;
                pathAndFile.Append(driveName);
                pathAndFile.Append(KBackUpFolder());
                pathAndFile.Append(entry.iName);
                
                entry.iName = pathAndFile;

                // Read category information from archive
                TBitFlags category;
                TRAPD(err, category = MMCScBkupArchiveUtils::ReadBkUpCategoryInformationL( iFsSession, pathAndFile ));
                
                if(err == KErrNone)
                    {
                    // Append archive in list when valid
                    if((category.Value() & aParams->Categories().Value()) && ValidArchiveForRestoreL( pathAndFile ))
                        {
                        __LOG2("CMMCScBkupEngineImpl::ListArchivesL() - adding archive %S of category 0x%x", 
                            &pathAndFile, category.Value());
                        CMMCScBkupArchiveInfo* archiveInfo = CMMCScBkupArchiveInfo::NewLC( entry );
                        // Set category in archive info - can be used for filtering 
                        archiveInfo->SetCategory( category );
                        // Move ownership to array
                        aArchives.AppendL(archiveInfo);
                        CleanupStack::Pop(archiveInfo);
                        }
                    else
                        {
                        __LOG3("CMMCScBkupEngineImpl::ListArchivesL() - archive %S of category 0x%x (vs. 0x%x) not included", 
                            &pathAndFile, category.Value(), aParams->Categories().Value());
                        }
                    }
                }
            }
            
        CleanupStack::PopAndDestroy(pFiles);
        }

    __LOG("CMMCScBkupEngineImpl::ListArchivesL() - END");
    }


#ifdef DEBUGGING_DATA_TRANSFER
// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::CleanBackupFilesL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::CleanBackupFilesL()
    {
    CFileMan* fileMan = CFileMan::NewL( iFsSession );
    CleanupStack::PushL( fileMan );
    
    // Make directory, ignore error, ugly, functionise later on...
    _LIT(KMMCScBkupFormatDes, "%S%S%S%S");
    TFileName cleanPath;
    const TDesC& path = PathInfo::MemoryCardRootPath();

    //
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataJava, &KMMCScBkupDataTransferDebuggingPathDataBackup);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataSystem, &KMMCScBkupDataTransferDebuggingPathDataBackup);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataPassive, &KMMCScBkupDataTransferDebuggingPathDataBackup);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataActive, &KMMCScBkupDataTransferDebuggingPathDataBackup);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    //
    CleanupStack::PopAndDestroy( fileMan );
    }
    

// ---------------------------------------------------------------------------
// CMMCScBkupEngineImpl::ClearRestoreFilesL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupEngineImpl::ClearRestoreFilesL()
    {
    CFileMan* fileMan = CFileMan::NewL( iFsSession );
    CleanupStack::PushL( fileMan );
    
    // Make directory, ignore error, ugly, functionise later on...
    _LIT(KMMCScBkupFormatDes, "%S%S%S%S");
    TFileName cleanPath;
    const TDesC& path = PathInfo::MemoryCardRootPath();
    
    //
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataJava, &KMMCScBkupDataTransferDebuggingPathDataRestore);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataSystem, &KMMCScBkupDataTransferDebuggingPathDataRestore);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataPassive, &KMMCScBkupDataTransferDebuggingPathDataRestore);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    cleanPath.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot, &KMMCScBkupDataTransferDebuggingPathDataActive, &KMMCScBkupDataTransferDebuggingPathDataRestore);
    fileMan->Delete( cleanPath, CFileMan::ERecurse ); // Ignore error
    iFsSession.MkDirAll( cleanPath );
    //
    CleanupStack::PopAndDestroy( fileMan );
    }

#endif


















