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
* Description: CMMCScBkupStateArchiveOpPublicDataFiles implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpPublicDataFiles.h"

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupConfig.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupIndexPublicDataFiles.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "CMMCScBkupFileInfo.h"
#include "CMMCScBkupFileListCollection.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "MMCScBkupOperations.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::CMMCScBkupStateArchiveOpPublicDataFiles()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpPublicDataFiles::CMMCScBkupStateArchiveOpPublicDataFiles( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpPublicDataFiles::CMMCScBkupStateArchiveOpPublicDataFiles() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpPublicDataFiles* CMMCScBkupStateArchiveOpPublicDataFiles::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpPublicDataFiles* self = new(ELeave) CMMCScBkupStateArchiveOpPublicDataFiles( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPublicDataFiles::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpPublicDataFiles;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPublicDataFiles::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpSystemData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPublicDataFiles::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdOperationComplete;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_BACKUP_PUB_DATA;

    CompleteSelf();
    SetState( EStateCommonKickOff );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_RESTORE_PUB_DATA;

    CompleteSelf();
    SetState( EStateCommonKickOff );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    const TState justFinishedExecutingState = State();
    //
    switch( justFinishedExecutingState )
        {
    case EStateCommonKickOff:
        CompressingAndWritingFileL();
        break;
    case EStateCommonProcessingFile:
        AddIndexRecordL();
        break;
    case EStateBackupAddIndexRecord:
        SetState( EStateCommonKickOff );
        CompleteSelf();
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    const TState justFinishedExecutingState = State();
    //
    switch( justFinishedExecutingState )
        {
    case EStateCommonKickOff:
        DecompressingAndWritingFileL();
        break;
    case EStateCommonProcessingFile:
        RestoreFileAttributesL();
        break;
    case EStateRestoreResetAttributes:
        SetState( EStateCommonKickOff );
        CompleteSelf();
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupBackup() - **** - error: %d", aError );
    TBool continueProcessing = ETrue;

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    const CMMCScBkupFileListCollection& fileList = Driver().DrvFileList();
    if  ( iIndexValueCurrent < fileList.Count() )
        {
        const CMMCScBkupFileInfo& fileInfo = fileList.Entry(iIndexValueCurrent);
        (void) fileInfo;

        __LOGFILE2("CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupBackup() - **** - error: %d, file: %S - public data skipped", aError, &fileInfo.FileName());

        // Ensures that asynchronous state step is called again..
        CompleteSelf();
        SetState( EStateCommonKickOff );
        }

    return continueProcessing;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );
    TBool continueProcessing = ETrue;

    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexPublicDataFiles& index = static_cast<CMMCScBkupIndexPublicDataFiles&>( footer.IndexByType( EMMCScBkupOwnerDataTypePublicData ) );
    const TInt count = index.Count();
    //
    if  ( iIndexValueCurrent < count )
        {
        // Get the entry to restore
        TMMCScBkupArchiveVector entryInfo;
        const CMMCScBkupFileInfo& fileInfo = index.At( iIndexValueCurrent, entryInfo );
        (void) fileInfo;

        __LOGFILE2("CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousErrorCleanupRestore() - **** - error: %d, file: %S - public data skipped", aError, &fileInfo.FileName());

        // Try to restore the next public file
        //SetState( EStateCommonKickOff );
        SetState( EStateCommonProcessingFile );
        CompleteSelf();
        }

    return continueProcessing;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformAsynchronousCancellation()
    {
    switch( State() )
        {
    case EStateCommonProcessingFile:
        // Asynchronous request issued to ADI. 
        Driver().DrvADI().ADIAsynchronousCancel();
        break;

    case EStateCommonKickOff:
    case EStateBackupAddIndexRecord:
    case EStateRestoreResetAttributes:
        // Other states complete this objects own request status => no action
        // needed.
        break;

    default:
    case EStateCommonIdle:
        ASSERT( EFalse );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypePublicData );
    index.StoreL( Driver() );
    
    Driver().DrvADI().ADIResetResources(EMMCScBkupOperationTypeFullBackup);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::PerformLastRightsRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::PerformLastRightsRestoreL( TBool /*aPartial*/ )
    {
    Driver().DrvADI().ADIResetResources(EMMCScBkupOperationTypeFullRestore);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::CompressingAndWritingFileL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::CompressingAndWritingFileL()
    {
    const CMMCScBkupFileListCollection& fileList = Driver().DrvFileList();
    const TInt count = fileList.Count();
    //
    if  ( ++iIndexValueCurrent < count )
        {
        const CMMCScBkupFileInfo& fileInfo = fileList.Entry(iIndexValueCurrent);

        __LOG1("CMMCScBkupStateArchiveOpPublicDataFiles::CompressingAndWritingFileL() - BACKUP - compressing file: %S", &fileInfo.FileName());

        // Compress the file & externalise it to the archive
        Driver().DrvADI().ADIWriteFileL( fileInfo.FileName(), iStatus );
        
        // Indicate the state we are in
        SetActive();
        SetState( EStateCommonProcessingFile );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::DecompressingAndWritingFileL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::DecompressingAndWritingFileL()
    {
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexPublicDataFiles& index = static_cast<CMMCScBkupIndexPublicDataFiles&>( footer.IndexByType( EMMCScBkupOwnerDataTypePublicData ) );
    const TInt count = index.Count();
    //
    if  ( ++iIndexValueCurrent < count )
        {
        // Get the entry to restore
        TMMCScBkupArchiveVector entryInfo;
        const CMMCScBkupFileInfo& fileInfo = index.At( iIndexValueCurrent, entryInfo );

        __LOG3("CMMCScBkupStateArchiveOpPublicDataFiles::DecompressingAndWritingFileL() - RESTORE - decompressing file: %S, offset: %8d, length: %8d", &fileInfo.FileName(), entryInfo.Offset(), entryInfo.Length());

        // Restore it!
        Driver().DrvADI().ADIReadFileL( fileInfo.FileName(), entryInfo, iStatus );
        
        // Indicate the state we are in
        SetActive();
        SetState( EStateCommonProcessingFile );
        }
    }
    

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::AddIndexRecordL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexPublicDataFiles& index = static_cast<CMMCScBkupIndexPublicDataFiles&>(footer.IndexByType(EMMCScBkupOwnerDataTypePublicData));
    const CMMCScBkupFileListCollection& fileList = Driver().DrvFileList();
    const CMMCScBkupFileInfo& fileInfo = fileList.Entry(iIndexValueCurrent);

    // Save the offset of the data owner
    const TMMCScBkupArchiveVector& info = Driver().DrvADI().ADICurrentArchiveVectorInfo();
    index.AddIndexRecordL( info, fileInfo );
    
    // And update the file info so that we can know how much compressed
    // data must be read during restore. This allows us to report accurate 
    // progress info. See CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL()
    // for the point when the public data size is zeroed.

    
    
    __LOG3("CMMCScBkupStateArchiveOpPublicDataFiles::AddIndexRecordL() - offset: %6d, length: %6d, file: %S", info.Offset(), info.Length(), &fileInfo.FileName());

    // Indicate the state we are in
    SetState( EStateBackupAddIndexRecord );
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPublicDataFiles::RestoreFileAttributesL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPublicDataFiles::RestoreFileAttributesL()
    {
    RFs& fsSession = Driver().DrvADI().ADIFsSession();
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexPublicDataFiles& index = static_cast<CMMCScBkupIndexPublicDataFiles&>( footer.IndexByType( EMMCScBkupOwnerDataTypePublicData ) );
   
    // Get the entry who's attributes need restoring
    TMMCScBkupArchiveVector entryInfo;
    const CMMCScBkupFileInfo& fileInfo = index.At( iIndexValueCurrent, entryInfo );

    // If file is in Photo Album Thumbnail folder
    // or file in videocenter folder and its subfolders
    if  ( fileInfo.FileName().MatchF( KMMCScBkupMediaGalleryThumbnailFolder ) != KErrNotFound ) 
        {
        // Get folder path length
        const TChar backslash('\\');
        const TInt pathlength = fileInfo.FileName().LocateReverseF( backslash ) + 1;

        // Set hidden attribute for Thumbnail folder.
        // If folder does not exist, create it
        fsSession.SetAtt( fileInfo.FileName().Left( pathlength ), KEntryAttHidden, KEntryAttNormal );
        }

    if  ( fileInfo.FileName().MatchF( KMMCScBkupVideoCenterFolder ) != KErrNotFound) 
        {
        // Get folder path length
        const TChar backslash('\\');
        const TInt pathlength = fileInfo.FileName().LocateReverseF( backslash ) + 1;

        // Set hidden attribute for the video center's sub folder.
        // If foler does not exist, create it
        fsSession.SetAtt( fileInfo.FileName().Left( pathlength ), KEntryAttHidden, KEntryAttNormal );
        
        //Manually set the videocenter main folder to be hidden
        fsSession.SetAtt( KMMCScBkupVideoCenterFolderPath, KEntryAttHidden, KEntryAttNormal );
        }
    // Set the original attributes, clear the others
    const TInt error = fsSession.SetEntry( fileInfo.FileName(), 
                                           fileInfo.DateTime(), 
                                           fileInfo.Attributes(), 
                                          !fileInfo.Attributes() );

    (void) error;

        
    // Indicate the state we are in
    SetState( EStateRestoreResetAttributes );
    CompleteSelf();
    }



