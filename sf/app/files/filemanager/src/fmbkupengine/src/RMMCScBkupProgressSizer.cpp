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
* Description: RMMCScBkupProgressSizer implementation
*
*
*/

#include "RMMCScBkupProgressSizer.h"

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMMCScBkupProgressObserver.h"
#include "CMMCScBkupDriveAndOperationTypeManager.h"

// Constants
const TInt KMMCScBkupWeightingFactor = 10000;



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::RMMCScBkupProgressSizer()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
RMMCScBkupProgressSizer::RMMCScBkupProgressSizer( const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperationTypes )
:   iDriveAndOperationTypes( aDriveAndOperationTypes ), iDriveFilter( aDriveAndOperationTypes.DriveList() )
    {
    }

   
// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::BackupTotalProgressValueL()
// 
// Calculate the total amount of progress steps that a particular data owner 
// requires. This method can only be called after all of the sizing info
// has been obtained from the SBE
// ---------------------------------------------------------------------------
TInt64 RMMCScBkupProgressSizer::BackupTotalProgressValueL( const CMMCScBkupDataOwnerInfo& aDataOwner )
    {
    __LOG(" ");
    __LOG1("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - START - DO: 0x%08x", aDataOwner.SecureId().iId);
    TInt64 totalStepCount = 0;

    //////////////////////////////////////////////////////////////////////
    // THESE ELEMENTS HAVE PSEUDO-PROGRESS - that is, the report a fixed
    // number of progress nibbles per operation. For example:
    // 
    // TYPE     DRIVE        PROGRESS
    // =======================================
    // ACTIVE   (C:)          1 x 2000
    // PASSIVE  (C:)          1 x 2000
    // SYSTEM   (C:, E:)      2 x 2000
    // JAVA     (C:)          1 x 2000
    // PUBLIC   (C: = 12k)   12 x 1024
    // ----------------------------------------
    // total                 10000 + (12 x 1024)
    //
    //////////////////////////////////////////////////////////////////////

    const CDataOwnerInfo& sbeDataOwner = aDataOwner.Owner();
    
    // Passive data
    if  ( MMCScBkupSBEUtils::HasPassiveDataL( sbeDataOwner ) )
        {
        const TInt opsCount = NumberOfDriveOpsRequiredL( aDataOwner, EMMCScBkupOwnerDataTypePassiveData );
        __LOG2("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - passive - %d of progress (%d ops)", opsCount * KMMCScBkupWeightingFactor, opsCount);
        totalStepCount += ( opsCount * KMMCScBkupWeightingFactor );
        }

    // Active data
    if  ( MMCScBkupSBEUtils::HasActiveDataL( sbeDataOwner ) )
        {
        const TInt opsCount = NumberOfDriveOpsRequiredL( aDataOwner, EMMCScBkupOwnerDataTypeActiveData );
        __LOG2("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - active  - %d of progress (%d ops)", opsCount * KMMCScBkupWeightingFactor, opsCount);
        totalStepCount += ( opsCount * KMMCScBkupWeightingFactor );
        }

    // System data
    if  ( MMCScBkupSBEUtils::HasSystemDataL( sbeDataOwner ) )
        {
        const TInt opsCount = NumberOfDriveOpsRequiredL( aDataOwner, EMMCScBkupOwnerDataTypeSystemData );
        __LOG2("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - system  - %d of progress (%d ops)", opsCount * KMMCScBkupWeightingFactor, opsCount);
        totalStepCount += ( opsCount * KMMCScBkupWeightingFactor );
        }

    // Java data
    if  ( MMCScBkupSBEUtils::HasJavaDataL( sbeDataOwner ) )
        {
        const TInt opsCount = NumberOfDriveOpsRequiredL( aDataOwner, EMMCScBkupOwnerDataTypeJavaData );
        __LOG2("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - java    - %d of progress (%d ops)", opsCount * KMMCScBkupWeightingFactor, opsCount);
        totalStepCount += ( opsCount * KMMCScBkupWeightingFactor );
        }


    ///////////////////////////////////
    // PUBLIC REPORTS ACTUAL SIZING
    ///////////////////////////////////

    // Public data
    if  ( MMCScBkupSBEUtils::HasPublicDataL( sbeDataOwner ) )
        {
        const TInt64 size = AmountOfPublicDataToBeRestoredL( aDataOwner );
        __LOG1("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - public  - %Ld of progress", size);
        totalStepCount += size;
        }

    //
    __LOG2("RMMCScBkupProgressSizer::BackupTotalProgressValueL() - END - DO: 0x%08x, totalStepCount: %8Ld", aDataOwner.SecureId().iId, totalStepCount);
    return totalStepCount;
    }


// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::BackupReportFixedProgressForOpL()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupProgressSizer::BackupReportFixedProgressForOpL( MMMCScBkupProgressObserver& aProgressManager, TMMCScBkupOwnerDataType aType )
    {
    switch( aType )
        {
    case EMMCScBkupOwnerDataTypeSystemData:
    case EMMCScBkupOwnerDataTypeActiveData:
    case EMMCScBkupOwnerDataTypePassiveData:
    case EMMCScBkupOwnerDataTypeJavaData:
        __LOG2("RMMCScBkupProgressSizer::BackupReportFixedProgressForOpL() - aType: %d, amount: %d ", aType, KMMCScBkupWeightingFactor );
        aProgressManager.MMCScBkupHandleProgress( KMMCScBkupWeightingFactor );
        break;
    default:
    case EMMCScBkupOwnerDataTypeDataOwner:
    case EMMCScBkupOwnerDataTypePublicData:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::RestoreCombinedDataSizeL()
// 
// 
// ---------------------------------------------------------------------------
TInt64 RMMCScBkupProgressSizer::RestoreCombinedDataSizeL( const CMMCScBkupDataOwnerInfo& aOwner )
    {
    TInt64 size = 0;
    //
    iDriveFilter.Reset();
    iDriveFilter.SetSecondaryDriveFilter( aOwner.Owner().DriveList() );
    //
    TDriveNumber drive = EDriveA;
    while ( iDriveFilter.NextValidDrive( drive ) )
        {
        for( TInt i=0; i<EMMCScBkupOwnerDataTypeCount; i++ )
            {
            const TMMCScBkupOwnerDataType dataType = static_cast< TMMCScBkupOwnerDataType > ( i );
            const TBool allowedForDrive = iDriveAndOperationTypes.IsDataTypeAllowedToAccessDrive( drive, dataType );
            //
            if  ( allowedForDrive )
                {
                // Get the amount of data for this drive
                size += aOwner.OperationalSize( dataType, drive );
                }
            }
        }
    //
    return size;
    }











// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::NumberOfDriveOpsRequiredL()
// 
// 
// ---------------------------------------------------------------------------
TInt RMMCScBkupProgressSizer::NumberOfDriveOpsRequiredL( const CMMCScBkupDataOwnerInfo& aOwner, TMMCScBkupOwnerDataType aType )
    {
    TInt count = 0;
    //
    iDriveFilter.Reset();
    iDriveFilter.SetSecondaryDriveFilter( aOwner.Owner().DriveList() );
    //
    TDriveNumber drive = EDriveA;
    while ( iDriveFilter.NextValidDrive( drive ) )
        {
        const TBool allowedForDrive = iDriveAndOperationTypes.IsDataTypeAllowedToAccessDrive( drive, aType );
        //
        if  ( allowedForDrive )
            {
            ++count;
            }
        }
    //
    return count;
    }


// ---------------------------------------------------------------------------
// RMMCScBkupProgressSizer::AmountOfPublicDataToBeRestoredL()
// 
// 
// ---------------------------------------------------------------------------
TInt64 RMMCScBkupProgressSizer::AmountOfPublicDataToBeRestoredL( const CMMCScBkupDataOwnerInfo& aOwner )
    {
    TInt64 size = 0;
    //
    iDriveFilter.Reset();
    iDriveFilter.SetSecondaryDriveFilter( aOwner.Owner().DriveList() );
    //
    TDriveNumber drive = EDriveA;
    while ( iDriveFilter.NextValidDrive( drive ) )
        {
        const TBool allowedForDrive = iDriveAndOperationTypes.IsDataTypeAllowedToAccessDrive( drive, EMMCScBkupOwnerDataTypePublicData );
        //
        if  ( allowedForDrive )
            {
            // Get the amount of data for this drive
            size += aOwner.OperationalSize( EMMCScBkupOwnerDataTypePublicData, drive );
            }
        }
    //
    return size;
    }




