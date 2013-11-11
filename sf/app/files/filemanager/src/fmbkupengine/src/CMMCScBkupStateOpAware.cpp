/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMMCScBkupStateOpAware implementation
*
*
*/

#include "CMMCScBkupStateOpAware.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::CMMCScBkupStateOpAware()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateOpAware::CMMCScBkupStateOpAware( MMMCScBkupDriver& aDriver, TInt aPriority )
:   CMMCScBkupState( aDriver, aPriority )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateOpAware::NextStateId() const
    {
    TMMCScBkupStateId state = KMMCScBkupStateIdOperationComplete;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        state = NextStateBackupId( type == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        state = NextStateRestoreId( type == EMMCScBkupOperationTypePartialRestore );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return state;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousCancellationBackup()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousCancellationBackup( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousCancellationRestore()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousCancellationRestore( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformLastRightsRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformLastRightsRestoreL( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt /*aError*/ )
    {
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt /*aError*/ )
    {
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformStateInitL()
    {
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        PerformStateInitBackupL( type == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        PerformStateInitRestoreL( type == EMMCScBkupOperationTypePartialRestore );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousStateStepL()
    {
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        PerformAsynchronousStateStepBackupL( type == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        PerformAsynchronousStateStepRestoreL( type == EMMCScBkupOperationTypePartialRestore );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformAsynchronousCancellation()
    {
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        PerformAsynchronousCancellationBackup( type == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        PerformAsynchronousCancellationRestore( type == EMMCScBkupOperationTypePartialRestore );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformLastRightsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateOpAware::PerformLastRightsL()
    {
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        PerformLastRightsBackupL( type == EMMCScBkupOperationTypePartialBackup );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        PerformLastRightsRestoreL( type == EMMCScBkupOperationTypePartialRestore );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateOpAware::PerformAsynchronousErrorCleanup( TInt aError )
    {
    TBool handled = EFalse;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
    case EMMCScBkupOperationTypePartialBackup:
        handled = PerformAsynchronousErrorCleanupBackup( type == EMMCScBkupOperationTypePartialBackup, aError );
        break;
    case EMMCScBkupOperationTypeFullRestore:
    case EMMCScBkupOperationTypePartialRestore:
        handled = PerformAsynchronousErrorCleanupRestore( type == EMMCScBkupOperationTypePartialRestore, aError );
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return handled;
    }

