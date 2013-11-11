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
* Description: CMMCScBkupStateValidateDiskSpace implementation
*
*
*/

#include "CMMCScBkupStateValidateDiskSpace.h"

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupFileInfo.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupArchiveHeader.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "RMMCScBkupProgressSizer.h"
#include "MMMCScBkupProgressObserver.h"
#include "CMMCScBkupFileListCollection.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupIndexPublicDataFiles.h"
#include "CMMCScBkupDataStrategies.h"
#include "MMCScBkupSBEUtils.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::~CMMCScBkupStateValidateDiskSpace()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateValidateDiskSpace::~CMMCScBkupStateValidateDiskSpace()
    {
    iDriveSizes.Close();
    iDriveMaxFileSizes.Close();
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::CMMCScBkupStateValidateDiskSpace()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateValidateDiskSpace::CMMCScBkupStateValidateDiskSpace( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver ), iCumulatedSize(0)
    {
    __LOG1("CMMCScBkupStateValidateDiskSpace::CMMCScBkupStateValidateDiskSpace() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateValidateDiskSpace* CMMCScBkupStateValidateDiskSpace::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateValidateDiskSpace* self = new(ELeave) CMMCScBkupStateValidateDiskSpace( aDriver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateValidateDiskSpace::ConstructL( )
    {
    for( TInt i = EDriveA; i<=EDriveZ; i++ )
        {
        const TDriveNumber drive = static_cast< TDriveNumber >( i );
        
        // Zero-initialize max. file sizing info arrays
        TMMCScBkupDriveAndSize newEntry( drive, 0 );
        iDriveSizes.AppendL( newEntry );
        TMMCScBkupDriveAndSize maxEntry( drive, 0 );
        iDriveMaxFileSizes.AppendL( maxEntry );
        }
    }

// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateValidateDiskSpace::StateId() const
    {
    return KMMCScBkupStateIdValidateDiskSpace;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateValidateDiskSpace::NextStateId() const
    {
    TMMCScBkupStateId nextState = KMMCScBkupStateIdOperationComplete;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        nextState = KMMCScBkupStateIdArchiveOpPublicDataFiles;
        break;
    case EMMCScBkupOperationTypeFullRestore:
        nextState = KMMCScBkupStateIdSetPhoneMode;
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateValidateDiskSpace::PerformStateInitL()
    {
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateValidateDiskSpace::PerformAsynchronousStateStepL()
    {
    const TMMCScBkupOperationType type = Driver().DrvOperation();

    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        {
        ValidateFreeSpaceBeforeBackupL();
        break;
        }
    case EMMCScBkupOperationTypeFullRestore:
        {
        ValidateFreeSpaceBeforeRestoreL();
        break;
        }
    default:
        User::Leave(KErrNotSupported);
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    TInt64 uncompressedSize;

    __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - START");
    
    // Calculate the total progress required for the entire backup operation
    uncompressedSize = dataOwners.TotalOperationalSizeL();
    __LOG1("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - Total estimated uncompressed size for archive %Ld", uncompressedSize);
	//const TInt ownerCount = dataOwners.Count();
    
    //for( TInt i=0; i<ownerCount; i++ )
    //    {
    //    CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( i );
        // Reset the size of public data to 0. We update this value
        // with the compressed size during the compression stage. This
        // is needed in order to create accurate restore information (since
        // all of the data owner sizing info is written to disk).
        // owner.ResetOperationalSize( EMMCScBkupOwnerDataTypePublicData );
    //    }
              
    // Let's check do public files fit in target drive
    TInt driveNumber;
    TDriveInfo driveInfo;
    TVolumeInfo volInfo;
    TInt8 percentFree(100);
    
    // First get the drive number, where archive resides and then get drive's free space.
    TInt err = ADI().ADIRawArchiveFile().Drive( driveNumber, driveInfo );
    if( err == KErrNone)
        {
        err = ADI().ADIFsSession().Volume( volInfo, driveNumber );
        if( err == KErrNone)
            {
            iCumulatedSize += uncompressedSize;
            
            if(Driver().DrvLastCategory())
                {
                __LOG3("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - Final cumulated size %Ld, free size on drive %c is %Ld", 
                    iCumulatedSize, driveNumber + 'A', volInfo.iFree);
                
                if( iCumulatedSize > volInfo.iFree)
                    {
                    percentFree = TInt( (volInfo.iFree  * 100 ) / iCumulatedSize ); // accurate enough, no rounding
                    }
                    
                err = Driver().DrvProgressHandler().MMCScBkupHandleFreeSpace( percentFree );
                if( err != KErrNone )
                    {
                    // Error indicates backup creation no more allowed
                    Driver().DrvProgressHandler().MMCScBkupStartBackuping( EFalse );
                    User::Leave( err );
                    }
                else
                    {
                    Driver().DrvProgressHandler().MMCScBkupStartBackuping( ETrue );
                    }
                }
            else
                {
                __LOG3("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - Cumulated size now %Ld, free size on drive %c is %Ld", 
                    iCumulatedSize, driveNumber + 'A', volInfo.iFree);
                }
            }
        else
            {
            __LOG1("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - Volume info error %d", err);
            }
        }
    
    __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeBackupL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL()
    {
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexPublicDataFiles& index = static_cast<CMMCScBkupIndexPublicDataFiles&>( footer.IndexByType( EMMCScBkupOwnerDataTypePublicData ) );
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    CMMCScBkupFileListCollection& fileList = Driver().DrvFileList();
    TInt indexValueCurrent(0);
    
    __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - START");
    
    const TInt ownerCount = dataOwners.Count();
    
    // We can only make a direct comparison for public data
    for( TInt i=0; i<ownerCount; i++ )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( i );
        const CDataOwnerInfo& sbeDataOwner = owner.Owner();

        if  ( MMCScBkupSBEUtils::HasPublicDataL( sbeDataOwner ) )
            {
            for( TInt i = EDriveA; i<=EDriveZ; i++ )
                {
                const TDriveNumber drive = static_cast< TDriveNumber >( i );
                
                const TInt64 size = owner.OperationalSize( EMMCScBkupOwnerDataTypePublicData, drive );
                TMMCScBkupDriveAndSize& entry = iDriveSizes[ drive ];
                entry.SetSize(entry.Size() + size);
                __LOG4("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - DO: 0x%08x has %Ld bytes for drive %c (total %Ld)", 
                    owner.SecureId().iId, size, entry.Drive() + 'A', entry.Size());
                }
            }
        }
    
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    for( TInt i = EDriveA; i<=EDriveZ; i++ )
        {
        TMMCScBkupDriveAndSize& entry = iDriveSizes[ i ];
        __LOG2("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Initially %Ld bytes required on drive %c", 
            entry.Size(), entry.Drive() + 'A');
        }
#endif
    
    const TInt count = index.Count();

    __LOG1("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Public files to compare %d", count);
    
    // Check needed size by comparing archived public files to files in disk - decrease size if matching file found
    while( ++indexValueCurrent < count )
        {
        // Get the entry to restore
        TMMCScBkupArchiveVector entryInfo;
        //const CMMCScBkupFileInfo& fileInfo = index.At( indexValueCurrent, entryInfo );
        const CMMCScBkupFileInfo& fileInfo = fileList.Entry(indexValueCurrent);
        const TDriveNumber drive = fileInfo.Drive();

        __LOG2("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Archived file: %S, of size %d", &fileInfo.FileName(), fileInfo.Size());

        // Check file (if any) size in disk
        RFile64 restoreFile;
        TInt64 fileSize = 0;
        TInt error = restoreFile.Open(ADI().ADIFsSession(), fileInfo.FileName(), EFileShareReadersOnly | EFileRead);
        if(error == KErrNone)
            {
            error = restoreFile.Size(fileSize);
            if(error == KErrNone)
                {
                __LOG1("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Found matching file of size %d", fileInfo.Size());
                
                // Take into account biggest file already on disk, because reserves that much space temporarily
                if(iDriveMaxFileSizes[ drive ].Size() < fileSize && KMMCScBkupUseTempFile)
                    {
                    TMMCScBkupDriveAndSize& bigFile = iDriveMaxFileSizes[ drive ];
                    bigFile.SetSize(fileSize);
                    }
                }
            else
                {
                fileSize = 0;
                __LOG1("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Error % requesting file size", error);
                }
            
            restoreFile.Close();
            }
            
        TMMCScBkupDriveAndSize& entry = iDriveSizes[ drive ];
        entry.SetSize(entry.Size() - fileSize);
        
        __LOG2("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - %Ld bytes required now on drive %c", entry.Size(), entry.Drive() + 'A');
        }
    
    __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - No estimation for private and system files");
    
    if(Driver().DrvLastCategory())
        {
        TVolumeInfo volInfo;
        TInt spaceCheck(KErrNone);

        // Let user interrupt restore if necessary in future. Currently if any of drives cannot be restored, we interrupt restore.
        for( TInt i = EDriveA; i<=EDriveZ; i++ )
            {
            // Check whether data fits into this drive.
            const TDriveNumber drive = static_cast< TDriveNumber >( i );
                    
            TInt err = ADI().ADIFsSession().Volume( volInfo, drive );
            if( err == KErrNone)
                {
                // Total compressed size. If this exceeds total size of drive, then restore does not succeed for sure.
                const TInt64 size = dataOwners.DiskSpaceRequiredForRestore( drive ); 
                __LOG3("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - drive %c requires %Ld + %Ld for temporary file", 
                    iDriveSizes[ drive ].Drive() + 'A', iDriveSizes[ drive ].Size(), iDriveMaxFileSizes[ drive ].Size());
                
                if  ( (( iDriveSizes[ drive ].Size() + iDriveMaxFileSizes[ drive ].Size() ) > volInfo.iFree ) ||
                       ( size > volInfo.iSize ) )
                    {
                    __LOG2("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Not enough space (%Ld) on drive %c", 
                        TInt(volInfo.iFree), iDriveSizes[ drive ].Drive() + 'A');
                    spaceCheck = KErrDiskFull;
                    break;
                    }
                }
            }
        
        User::LeaveIfError(spaceCheck);
        }
    else
        {
        __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - Checking next category...");
        }
    
    __LOG("CMMCScBkupStateValidateDiskSpace::ValidateFreeSpaceBeforeRestoreL() - END");
    }


