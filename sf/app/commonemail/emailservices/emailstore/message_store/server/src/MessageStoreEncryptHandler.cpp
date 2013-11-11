/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store encrypt handler implementation.
*
*/



#include "MessageStoreEncryptHandler.h"
#include "ContainerStoreGeneralTable.h"
#include "ContainerStoreUtils.h"

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMessageStoreEncryptHandler* CMessageStoreEncryptHandler::NewL( CContainerStoreGeneralTable& aGeneralTable,
                                                                  CContainerStoreUtils&        aUtils,
                                                                  TInt                         aPriority )
    {
    CMessageStoreEncryptHandler* self = new(ELeave) CMessageStoreEncryptHandler( aGeneralTable, aUtils, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: CMessageStoreEncryptHandler
// ==========================================================================
CMessageStoreEncryptHandler::CMessageStoreEncryptHandler( CContainerStoreGeneralTable&    aGeneralTable,
                                                            CContainerStoreUtils&           aUtils,
                                                            TInt                            aPriority )
: CActive( aPriority ),
  iGeneralTable( aGeneralTable ),
  iUtils( aUtils )
    {
    __LOG_CONSTRUCT( "msg", "CMessageStoreEncryptHandler" )
    }

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMessageStoreEncryptHandler::ConstructL()
    {
    InitState();
    CActiveScheduler::Add(this);    
    }
        
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMessageStoreEncryptHandler::~CMessageStoreEncryptHandler()
    {
    __LOG_DESTRUCT
    
    Cancel();
    iEncryptableTables.Close();
    }


// ==========================================================================
// FUNCTION: AddEncryptableTable
// ==========================================================================
void CMessageStoreEncryptHandler::AddEncryptableTableL( MEncryptableTable* aEncryptableTable )
    {
    iEncryptableTables.AppendL( aEncryptableTable );
    }
// ==========================================================================
// FUNCTION: ResumeIfNeededL
// ==========================================================================
void CMessageStoreEncryptHandler::StartL()
    {
    __LOG_ENTER_SUPPRESS( "StartL" )
    
    iEncryptionState = iGeneralTable.EncryptionStateL();
    if ( iEncryptionState != EMsgStoreESIdle )
        {
        __LOG_WRITE_INFO("Start/Resume Encrypt/Decrypt")
        InitState();
        ScheduleNext();
        }
    }

// ==========================================================================
// FUNCTION: EncryptL
// ==========================================================================
void CMessageStoreEncryptHandler::EncryptL()
    {
    if ( iEncryptionState != EMsgStoreESIdle  )
        {
        User::Leave( KErrInUse );
        }
    iGeneralTable.SetEncryptionStateL( EMsgStoreESEncrypting );
    StartL();
    }
        
// ==========================================================================
// FUNCTION: DecryptL
// ==========================================================================
void CMessageStoreEncryptHandler::DecryptL()
    {
    if ( iEncryptionState != EMsgStoreESIdle  )
        {
        User::Leave( KErrInUse );
        }
    iGeneralTable.SetEncryptionStateL( EMsgStoreESDecrypting );
    StartL();
    }

void CMessageStoreEncryptHandler::ScheduleNext()
    {
    // Schedule next operation (yield, in case higher priority active objects have something to do).
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );      
    }

// ==========================================================================
// FUNCTION: RunL
// ==========================================================================
void CMessageStoreEncryptHandler::RunL()
    {
    DoNextOperationL();
    }

// ==========================================================================
// FUNCTION: DoCancel
// ==========================================================================
void CMessageStoreEncryptHandler::DoCancel()
    {
    //Nothing to do
    }

// ==========================================================================
// FUNCTION: StartL
// ==========================================================================
void CMessageStoreEncryptHandler::DoNextOperationL()
    {
    __LOG_ENTER_SUPPRESS( "DoNextOperationL" )
    
    if ( iEncryptionState != EMsgStoreESIdle )
        {
        iUtils.SuspendCompactionLC();
        iUtils.BeginDatabaseTransactionLC();
        
        MEncryptableTable* curTable = iEncryptableTables[iCurTableIndex];
        TBool hasMore = EFalse;
        if ( iIsFirstInTable )
            {
            if ( iEncryptionState == EMsgStoreESEncrypting )
                {
                hasMore = curTable->EncryptFirstL( iBookmark );
                }
            else
                {
                hasMore = curTable->DecryptFirstL( iBookmark );
                }
            
            iIsFirstInTable = EFalse;
            }
        else
            {
            if ( iEncryptionState == EMsgStoreESEncrypting )
                {
                hasMore = curTable->EncryptNextL( iBookmark );
                }
            else
                {
                hasMore = curTable->DecryptNextL( iBookmark );
                }
            }
        
        if ( !hasMore )
            {
            iCurTableIndex++;
            if ( iCurTableIndex >= iEncryptableTables.Count() )
                {
                //we are done, update the table and do NOT schedule next
                iGeneralTable.SetEncryptionStateL( EMsgStoreESIdle );
                iEncryptionState = EMsgStoreESIdle;
                }
            else
                {
                //start encrypt/decrypt the next table in next RunL()
                iIsFirstInTable = ETrue;
                ScheduleNext();
                }
            }
        else
            {
            ScheduleNext();
            }
        
        iUtils.CommitDatabaseTransactionL();
        iUtils.ResumeCompaction();
        }
    }

void CMessageStoreEncryptHandler::InitState()
    {
    iCurTableIndex = 0;
    iIsFirstInTable = ETrue;
    }
