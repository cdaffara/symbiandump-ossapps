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
* Description: CMMCScBkupArchive Implementation
*
*
*/

#include "CMMCScBkupArchive.h"

// System includes
#include <bautils.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupArchiveUtils.h"
#include "CMMCScBkupArchiveHeader.h"
#include "CMMCScBkupArchiveFooter.h"
#include "MMCScBkupPhoneModelUtils.h"
#include "CMMCScBkupArchiveDataManager.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupArchive::CMMCScBkupArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchive::CMMCScBkupArchive( RFs& aFsSession, MMMCScBkupProgressObserver& aProgressManager, 
    MMMCScBkupDriver& aDriver, TBitFlags aCategory )
:   iFsSession( aFsSession ), iProgressManager( aProgressManager ), iDriver( aDriver ), iCategory( aCategory )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::~CMMCScBkupArchive()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchive::~CMMCScBkupArchive()
    {
    Close( KErrNone );
    DeleteOldArchive();
    // Must do these last, as we may need to use the file name in order 
    // to delete any partially created or old archives.
    delete iArchiveFileName;
    iArchiveFileName = NULL;

    delete iOldArchiveFileName;
    iOldArchiveFileName = NULL;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchive* CMMCScBkupArchive::NewL( RFs& aFsSession, MMMCScBkupProgressObserver& aProgressManager, 
    MMMCScBkupDriver& aDriver, TBitFlags aCategory  )
    {
    __LOG("CMMCScBkupArchive::NewL() - START");
    CMMCScBkupArchive* self = new(ELeave) CMMCScBkupArchive( aFsSession, aProgressManager, aDriver, aCategory );
    __LOG("CMMCScBkupArchive::NewL() - END");
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::OpenForReadingL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::OpenForReadingL( const TDesC& aName )
    {
    __LOG1("CMMCScBkupArchive::OpenForReadingL() - START - aName: %S", &aName);

    Close( KErrNone );
    iArchiveFileName = aName.AllocL();
    //
    const TInt error = iArchiveFile.Open(iFsSession, *iArchiveFileName, EFileShareReadersOnly | EFileStream | EFileRead);
    __LOG1("CMMCScBkupArchive::OpenForReadingL() - open error: %d", error);
    User::LeaveIfError(error);
    //
    PrepareObjectsL();
    SetMode(EModeReading);
    __LOG1("CMMCScBkupArchive::OpenForReadingL() - END - file handle: 0x%08x", iArchiveFile.SubSessionHandle());
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::OpenForWritingL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::OpenForWritingL( const TDesC& aName )
    {
    __LOG1("CMMCScBkupArchive::OpenForWritingL() - START - aName: %S", &aName);

    Close( KErrNone );
    iArchiveFileName = aName.AllocL();

    // Name old archive as .old until disk space validation is carried out.
    _LIT( KOldSuffix, "old" );
    const TChar dot('.');
    iOldArchiveFileName = HBufC::NewL(aName.Length() + KOldSuffix().Length() + 1);
    TPtr temp( iOldArchiveFileName->Des() );
    temp.Copy( aName );
    const TInt location = temp.LocateReverseF( dot ) + 1;
    TInt length = temp.Length() - location;

    if( location != KErrNotFound && length <= KOldSuffix().Length() )
        {
        temp.Delete( location, length );
        temp.Append( KOldSuffix().Ptr(), length );
        }
    else
        {
        temp.Append( dot );
        temp.Append( KOldSuffix().Ptr(), length );
        }

    // Ensure archive directory and attributes permit writing of file
    TInt error = PrepareToOverwrite(aName);
    __LOG1("CMMCScBkupArchive::OpenForWritingL() - prepare to over-write error: %d", error);
    User::LeaveIfError(error);

    // Open file for writing
    error = iArchiveFile.Replace(iFsSession, *iArchiveFileName, EFileShareExclusive | EFileStream | EFileWrite);
    __LOG1("CMMCScBkupArchive::OpenForWritingL() - replace error: %d", error);
    User::LeaveIfError(error);
    //
    PrepareObjectsL();
    SetMode(EModeWriting);
    __LOG1("CMMCScBkupArchive::OpenForWritingL() - END - file handle: 0x%08x", iArchiveFile.SubSessionHandle());
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::RestoreOldArchive()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::RestoreOldArchive()
    {
    Close( KErrCancel );
    TInt err = iFsSession.Rename( *iOldArchiveFileName, *iArchiveFileName );
    
    __LOG2("CMMCScBkupArchive::RestoreOldArchive() - %S restore error: %d", iArchiveFileName, err);
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchive::DeleteOldArchive()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::DeleteOldArchive()
    {
    if( iOldArchiveFileName != NULL )
        {
        TInt err = iFsSession.Delete( *iOldArchiveFileName );
        
        __LOG2("CMMCScBkupArchive::DeleteOldArchive() - %S delete error: %d", iOldArchiveFileName, err);
        }
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchive::Close()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::Close( TInt aError )
    {
    __LOG1("CMMCScBkupArchive::Close() - aError: %d", aError);

    delete iHeader;
    iHeader = NULL;
    delete iFooter;
    iFooter = NULL;
    delete iDataManager;
    iDataManager = NULL;
    //
    if  ( aError != KErrNone && Mode() == EModeWriting )
        {
        // Delete the archive if there was an error with the backup operation
        __LOG1("CMMCScBkupArchive::Close() - ERROR CREATING BACKUP - aError: %d => archive will be deleted", aError);

        // Ignore error when attempting delete
        iArchiveFile.Close();
        (void) iFsSession.Delete( *iArchiveFileName );
        }

    // Close archive. It might have already been closed in an error situation
    // but that's okay - it doesn't cause any problems to close it twice.
    iArchiveFile.Close();
    
    // Ignore error if setting read-only attribute fails, because it is not fatal
    iFsSession.SetAtt( *iArchiveFileName, KEntryAttReadOnly, !KEntryAttReadOnly );
    
    // Reset mode back to default, ready for next operation
    iMode = EModeUninitialised;

    __LOG("CMMCScBkupArchive::Close() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::Header()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveHeader& CMMCScBkupArchive::Header() const
    {
    return *iHeader;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::Footer()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveFooter& CMMCScBkupArchive::Footer() const
    {
    return *iFooter;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::ADI()
// 
// 
// ---------------------------------------------------------------------------
MMMCScBkupArchiveDataInterface& CMMCScBkupArchive::ADI() const
    {
    return *iDataManager;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::PrepareToOverwrite()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupArchive::PrepareToOverwrite( const TDesC& aFile )
    {
    __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - START - aFile: %S", &aFile);
    TInt err = KErrNone;

    // Try deleting file with temporary name (one we have created earlier,
    // but which has potentially remained for some reason).
    err = iFsSession.Delete( *iOldArchiveFileName );
    __LOG2("CMMCScBkupArchive::PrepareToOverwrite() - delete %S result: %d", iOldArchiveFileName, err);
    // Rename the file for possible later "restoration".
    err = iFsSession.Rename( aFile, *iOldArchiveFileName );
    __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - rename existing file: %d", err);
    // Reset file flags in order to make sure file can be deleted
    err = iFsSession.SetAtt( *iOldArchiveFileName, KEntryAttNormal, !KEntryAttNormal );
    __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - setFileAttNormal: %d", err);

    // Create the full path, if not exists
    err = iFsSession.MkDirAll( aFile );
    __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - mkDirAll: %d", err);

    if  (err == KErrAlreadyExists || err == KErrNone )
        {
        // Set folder hidden, ignore error
        err = iFsSession.SetAtt( BaflUtils::DriveAndPathFromFullName( aFile ), KEntryAttHidden, !KEntryAttHidden );
        __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - setFolderHidden: %d", err);
        
        // Reset file flags in order to make sure file can be deleted
        err = iFsSession.SetAtt( aFile, KEntryAttNormal, !KEntryAttNormal );
        __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - setFileAttNormal: %d", err);

        }
    //
    if  ( err == KErrNotFound )
        {
        // These errors are ignored
        __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - mapped %d to KErrNone -> everything is okay", err);
        err = KErrNone;
        }
    //
    __LOG1("CMMCScBkupArchive::PrepareToOverwrite() - END - err: %d", err);
    return err;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::ValidArchiveForRestoreL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupArchive::ValidArchiveForRestoreL( RFs& aFsSession, const TDesC& aFileName )
    {
    HBufC8* modelInfo = NULL;
    TBitFlags archiveFlags( 0 );

    TVersion archiveVersion;

    // Read the header from the specified archive
    MMCScBkupArchiveUtils::ReadPhoneValidityInformationL( aFsSession, aFileName, modelInfo, archiveFlags, archiveVersion );
    CleanupStack::PushL( modelInfo );

    // Check whether its okay to restore the archive.
    const TBool validArchiveForRestore = MMCScBkupPhoneModelUtils::ArchiveRestorePermissableL( *modelInfo, archiveFlags, archiveVersion );
    CleanupStack::PopAndDestroy(modelInfo);
    //
    return validArchiveForRestore;
    }



// ---------------------------------------------------------------------------
// CMMCScBkupArchive::SetMode()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::SetMode(TMode aMode)
    {
    iMode = aMode;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchive::PrepareObjectsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchive::PrepareObjectsL()
    {
    __LOG("CMMCScBkupArchive::PrepareObjectsL() - START");

    __LOG("CMMCScBkupArchive::PrepareObjectsL() - creating data manager...");
    iDataManager = CMMCScBkupArchiveDataManager::NewL( iFsSession, iArchiveFile, iProgressManager );

    __LOG("CMMCScBkupArchive::PrepareObjectsL() - creating header...");
    iHeader = CMMCScBkupArchiveHeader::NewL( *iDataManager, iDriver );

    __LOG("CMMCScBkupArchive::PrepareObjectsL() - creating footer...");
    iFooter = CMMCScBkupArchiveFooter::NewL( *iDataManager, iDriver );

    __LOG("CMMCScBkupArchive::PrepareObjectsL() - END");
    }







