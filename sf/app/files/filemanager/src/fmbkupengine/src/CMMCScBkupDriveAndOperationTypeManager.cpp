/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMMCScBkupDriveAndOperationTypeManager implementation
*     
*
*/

#include "CMMCScBkupDriveAndOperationTypeManager.h"

// User includes
#include "MMCScBkupLogger.h"
#ifdef RD_MULTIPLE_DRIVE
#include "BkupEngine.hrh"
#include <driveinfo.h>
#include <coemain.h>
#endif // RD_MULTIPLE_DRIVE


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::CMMCScBkupDriveAndOperationTypeManager()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDriveAndOperationTypeManager::CMMCScBkupDriveAndOperationTypeManager()
    {
    iCalculatedDriveList.SetMax();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::CMMCScBkupDriveAndOperationTypeManager()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
EXPORT_C CMMCScBkupDriveAndOperationTypeManager::~CMMCScBkupDriveAndOperationTypeManager()
    {
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDriveAndOperationTypeManager::ConstructL( BkupDrivesAndOperationList *aDriveList )
    {
#ifdef RD_MULTIPLE_DRIVE
    RFs& fs( CCoeEnv::Static()->FsSession() );

    // Get all user visible drives
    TInt numDrives( 0 );
    TDriveList drvList;
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
        fs, drvList, numDrives ) );
    TInt drvListLen( drvList.Length() );
    TMMCScBkupDriveAndOperationType op;
    TInt count( aDriveList->Count() );

    // Get default system drive
    TInt sysDrive( 0 );
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
        DriveInfo::EDefaultSystem, sysDrive ) );

    for( TInt i( 0 ); i < count; ++i )
        {
        TBkupDrivesAndOperation *drvAndOp = &((*aDriveList)[i]);
        TUint drvCategories( drvAndOp->drvCategories() );
        TMMCScBkupOwnerDataType dataType(
            static_cast< TMMCScBkupOwnerDataType >( drvAndOp->ownerDataType() ) );

        if ( drvCategories & EBkupDeviceMemories )
            {
            // Default system drive is always backed up when
            // device memories are defined
            op.SetDrive( static_cast< TDriveNumber >( sysDrive ) );
            op.SetDataType( dataType );
            iEntries.AppendL( op );
            iCalculatedDriveList[ op.Drive() ] = ETrue;
            }

        for ( TInt j( 0 ); j < drvListLen; ++j )
            {
            if ( sysDrive != j && drvList[ j ] )
                {
                // Check for other drives
                TBool append( EFalse );
                TUint drvStatus( 0 );
                User::LeaveIfError( DriveInfo::GetDriveStatus(
                    fs, j, drvStatus ) );
                if ( ( drvCategories & EBkupDeviceMemories ) &&
                    ( drvStatus & DriveInfo::EDriveInternal ) &&
                    !( drvStatus & DriveInfo::EDriveExternallyMountable ) &&
                    !( drvStatus & DriveInfo::EDriveReadOnly ) )
                    {
                    append = ETrue; // Allow additional device memory
                    }
                else if ( ( drvCategories & EBkupInternalMassStorages ) &&
                    ( drvStatus & DriveInfo::EDriveInternal ) &&
                    ( drvStatus & DriveInfo::EDriveExternallyMountable ) )
                    {
                    append = ETrue; // Allow internal mass storage
                    }
                else if ( ( drvCategories & EBkupExternalMassStorages ) &&
                    ( drvStatus & DriveInfo::EDriveRemovable ) )
                    {
                    append = ETrue; // Allow external mass storage
                    }
                if ( append )
                    {
                    op.SetDrive(  static_cast< TDriveNumber >( j ) );
                    op.SetDataType( dataType );
                    iEntries.AppendL( op );
                    iCalculatedDriveList[ op.Drive() ] = ETrue;
                    }
                }
            }
        }
#else // RD_MULTIPLE_DRIVE
    TMMCScBkupDriveAndOperationType op;
    const TInt count = aReader.ReadInt8();
    //
    for( TInt i=0; i<count; i++ )
        {
        op.SetDrive( static_cast< TDriveNumber >( aReader.ReadInt8() ) );
        op.SetDataType( static_cast< TMMCScBkupOwnerDataType >( aReader.ReadInt8() ) );
        //
        iEntries.AppendL( op );
        //
        iCalculatedDriveList[ op.Drive() ] = ETrue;
        }
#endif // RD_MULTIPLE_DRIVE
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::NewL()
// 
//
// ---------------------------------------------------------------------------
EXPORT_C CMMCScBkupDriveAndOperationTypeManager* CMMCScBkupDriveAndOperationTypeManager::NewL(\
		BkupDrivesAndOperationList *aDriveList )
    {
    CMMCScBkupDriveAndOperationTypeManager* self = new(ELeave) CMMCScBkupDriveAndOperationTypeManager();
    CleanupStack::PushL( self );
    self->ConstructL( aDriveList );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::Count()
// 
//
// ---------------------------------------------------------------------------
TInt CMMCScBkupDriveAndOperationTypeManager::Count() const
    {
    return iEntries.Count();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::At()
// 
//
// ---------------------------------------------------------------------------
const TMMCScBkupDriveAndOperationType& CMMCScBkupDriveAndOperationTypeManager::At( TInt aIndex ) const
    {
    return iEntries[ aIndex ];
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDriveAndOperationTypeManager::IsDataTypeAllowedToAccessDrive()
// 
//
// ---------------------------------------------------------------------------
TBool CMMCScBkupDriveAndOperationTypeManager::IsDataTypeAllowedToAccessDrive( TDriveNumber aDrive, TMMCScBkupOwnerDataType aDataType ) const
    {
//    __LOG2("CMMCScBkupDriveAndOperationTypeManager::IsDataTypeAllowedToAccessDrive() - START - aDrive: %c, aDataType: %S", aDrive + 'A', &MMCScBkupLogger::DataType( aDataType ));

    TBool allowed = EFalse;
    //
    const TInt count = iEntries.Count();
    //
    for( TInt i=0; i<count; i++ )
        {
        const TMMCScBkupDriveAndOperationType& entry = iEntries[ i ];
        //
        if ( entry.DataType() == aDataType )
            {
            // Check drive...
            if ( entry.Drive() == aDrive )
                {
                allowed = ETrue;
                break;
                }
            }
        }
    //
//    __LOG1("CMMCScBkupDriveAndOperationTypeManager::IsDataTypeAllowedToAccessDrive() - END - allowed: %d", allowed);
    return allowed;
    }

