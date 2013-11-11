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
* Description: CMMCScBkupDataStrategy implementation
*
*
*/

#include "CMMCScBkupDataStrategies.h"

// User includes
#include "MMCScBkupConfig.h"
#include "MMCScBkupLogger.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::CMMCScBkupDataStrategy()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataStrategy::CMMCScBkupDataStrategy()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::ReadStrategyLC()
// 
// Factory function
// ---------------------------------------------------------------------------
CMMCScBkupDataStrategy* CMMCScBkupDataStrategy::ReadStrategyLC(const TDesC& aName, RFs& aFsSession)
    {
    // Fetch the size
    TEntry entry;
    const TInt error = aFsSession.Entry(aName, entry);
    User::LeaveIfError(error);

    // Construct
    return CMMCScBkupDataStrategy::ReadStrategyLC(aName, aFsSession, 0, entry.iSize);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::ReadStrategyLC()
// 
// Factory function
// ---------------------------------------------------------------------------
CMMCScBkupDataStrategy* CMMCScBkupDataStrategy::ReadStrategyLC(const TDesC& aName, RFs& aFsSession, TInt aOffset, TInt aLength)
    {
    CMMCScBkupDataStrategy* ret = NULL;

    // First, try a normal read strategy
    TRAPD(err, ret = CMMCScBkupDataFileStrategy::NewForReadingL(aName, aFsSession));
    if  (err != KErrNone)
        {
        // Try the file section read strategy - don't trap this - if it fails
        // then this file cannot be read...
        ret = CMMCScBkupDataFileSectionReadStrategy::NewL(aName, aFsSession);
        }
    //
    if  (ret)
        {
        ret->SetOffsetAndLength(aOffset, aLength);
        }
    //
    CleanupStack::PushL( ret );
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::WriteStrategyLC()
// 
// Factory function
// ---------------------------------------------------------------------------
CMMCScBkupDataStrategy* CMMCScBkupDataStrategy::WriteStrategyLC( const TDesC& aName, RFs& aFsSession, TBool aUseTempFile )
    {
    CMMCScBkupDataStrategy* ret = CMMCScBkupDataFileStrategy::NewForWritingL( aName, aFsSession, aUseTempFile );
    CleanupStack::PushL(ret);
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::Write()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataStrategy::Write(const TDesC8& /*aSource*/)
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::Finalize()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataStrategy::Finalize()
    {
    return KErrNone;
    }
    

// ---------------------------------------------------------------------------
// CMMCScBkupDataStrategy::SetOffsetAndLength()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataStrategy::SetOffsetAndLength(TInt aStartOffset, TInt aLengthToRead)
    {
    iOffset = aStartOffset; 
    iLengthToRead = aLengthToRead;
    }




































// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::CMMCScBkupDataFileStrategy()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataFileStrategy::CMMCScBkupDataFileStrategy( RFs& aFsSession, TBool aUseTempFile )
:   iFsSession( aFsSession ), iUseTempFile( aUseTempFile )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::~CMMCScBkupDataFileStrategy()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataFileStrategy::~CMMCScBkupDataFileStrategy()
    {
    iFile.Close();
    delete iFileName;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataFileStrategy::ConstructL( const TDesC& aName, TUint aMode )
    {
    iFileName = aName.AllocL();
    
    // The previous version of this engine would attempt to use the old
    // backup & restore API to close this file. 
    //
    // With the new Symbian Secure Backup Architecture, data owners are expected
    // to release file locks on public files as soon as they receive
    // notification that a backup is beginning.
    TInt error = KErrNone;
    //
    if  ( iUseTempFile )
        {
        // Ensure path for temporary files exists on same drive where file will eventually
        // be stored. This is important, because we estimate available size on target drive
        // and so also temporary file size affects whether restore can be carried out.
        HBufC* tempPath = HBufC::NewLC( KMaxFileName );
        TPtr pTempPath( tempPath->Des() );
        pTempPath.Zero();
        if(aName.Length())
            {
            pTempPath.Append( aName[0] );
            }
        else
            {
            const TDriveUnit driveUnit( KMMCScBkupTempFileDrive );
            const TDriveName drive( driveUnit.Name() );
            pTempPath.Append( drive );
            }
        pTempPath.Append( KMMCScBkupTempDir );

        error = iFsSession.MkDirAll( pTempPath );
        if  ( ! (error == KErrNone || error == KErrAlreadyExists) )
            {
            User::LeaveIfError( error );
            }
            
        // Make a temporary file
        error = iFile.Temp( iFsSession, pTempPath, iTempFileName, aMode );
        __LOG2("CMMCScBkupDataFileStrategy::ConstructL() - Created temporary file: %S, error code: %d", &iTempFileName, error);
        
        // Clean up
        CleanupStack::PopAndDestroy( tempPath );
        }
    else
        {
        if( aMode & EFileWrite )
            {
            error = PrepareToOverwriteFile( iFsSession, *iFileName );

            if  ( error == KErrNone )
                {
                error = iFile.Create( iFsSession, *iFileName, aMode );
                __LOG2("CMMCScBkupDataFileStrategy::ConstructL() - Created file %S, error code: %d", iFileName, error);
                }
            }
        else
            {
            error = iFile.Open( iFsSession, aName, aMode );
            }
        }
    //
    User::LeaveIfError(error);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::NewForReadingL()
// 
// Factory function
// ---------------------------------------------------------------------------
CMMCScBkupDataFileStrategy* CMMCScBkupDataFileStrategy::NewForReadingL( const TDesC& aName, RFs& aFsSession )
    {
    const TUint mode = EFileRead | EFileShareReadersOnly | EFileStream;
    //
    CMMCScBkupDataFileStrategy* self = new(ELeave) CMMCScBkupDataFileStrategy( aFsSession );
    CleanupStack::PushL(self);
    self->ConstructL( aName, mode );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::NewForWritingL()
// 
// Factory function
// ---------------------------------------------------------------------------
CMMCScBkupDataFileStrategy* CMMCScBkupDataFileStrategy::NewForWritingL( const TDesC& aName, RFs& aFsSession, TBool aUseTempFile )
    {
    const TUint mode = EFileWrite | EFileStream;
    //
    CMMCScBkupDataFileStrategy* self = new(ELeave) CMMCScBkupDataFileStrategy( aFsSession, aUseTempFile );
    CleanupStack::PushL(self);
    self->ConstructL( aName, mode );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::Read()
// 
//
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataFileStrategy::Read(TDes8& aSink)
    {
    TInt error = KErrNone;
    //
    aSink.Zero();
    TInt amountToRead = (LengthToRead() - Offset());
    if  (amountToRead > aSink.MaxLength())
        {
        amountToRead = aSink.MaxLength();
        }
    //
    if  (amountToRead > 0)
        {
        // Do the read
        error = iFile.Read( static_cast<TInt64>(Offset()), aSink, amountToRead );
        if  (error == KErrNone)
            {
            SetOffset( Offset() + aSink.Length() );
            }
        }
    //
    return error;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::Write()
// 
//
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataFileStrategy::Write(const TDesC8& aSource)
    {
    // Offset writing not supported (no requirement)
    const TInt error = iFile.Write( aSource );
    return error;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::Finalize()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataFileStrategy::Finalize()
    {
    TInt error = KErrNone;
    //
    if  ( iUseTempFile )
        {
        // Prepare for over-write
        error = PrepareToOverwriteFile( iFsSession, *iFileName );
        //
        if  ( error == KErrNone )
            {
            // Finally, do the rename
            error = iFile.Rename( *iFileName );
            __LOG2("CMMCScBkupDataFileStrategy::Finalize() - Renamed temporary file as: %S, error code: %d", iFileName, error);
            }
        }

    // Whatever the situation, we close the file now
    iFile.Close();
    return error;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileStrategy::PrepareToOverwriteFile()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataFileStrategy::PrepareToOverwriteFile( RFs& aFsSession, const TDesC& aFileName )
    {
    TInt err = KErrNone;

    // Create the full path, if not exists
    err = aFsSession.MkDirAll( aFileName );

    if( err == KErrAlreadyExists || !err )
        {
        // Reset file flags
        err = aFsSession.SetAtt( aFileName, KEntryAttNormal, !KEntryAttNormal );
        }

    if( !err )
        {
        // Delete the file
        err = aFsSession.Delete( aFileName );
        }

    if( err == KErrNotFound )
        {
        // These errors are ignored
        err = KErrNone;
        }

    return err;
    }

















































// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupDataFileSectionReadStrategy::CMMCScBkupDataFileSectionReadStrategy()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupDataFileSectionReadStrategy::CMMCScBkupDataFileSectionReadStrategy(RFs& aFsSession)
:   iFsSession(aFsSession)
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileSectionReadStrategy::~CMMCScBkupDataFileSectionReadStrategy()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataFileSectionReadStrategy::~CMMCScBkupDataFileSectionReadStrategy()
    {
    delete iFileName;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileSectionReadStrategy::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupDataFileSectionReadStrategy::ConstructL(const TDesC& aName)
    {
    iFileName = aName.AllocL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupDataFileSectionReadStrategy::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupDataFileSectionReadStrategy* CMMCScBkupDataFileSectionReadStrategy::NewL(const TDesC& aName, RFs& aFsSession)
    {
    CMMCScBkupDataFileSectionReadStrategy* self = new(ELeave) CMMCScBkupDataFileSectionReadStrategy(aFsSession);
    CleanupStack::PushL(self);
    self->ConstructL(aName);
    CleanupStack::Pop(self);
    return self;
    }



// ---------------------------------------------------------------------------
// CMMCScBkupDataFileSectionReadStrategy::Read()
// 
//
// ---------------------------------------------------------------------------
TInt CMMCScBkupDataFileSectionReadStrategy::Read(TDes8& aSink)
    {
    TInt error = KErrNone;
    //
    aSink.Zero();
    TInt amountToRead = (LengthToRead() - Offset());
    if  (amountToRead > aSink.MaxLength())
        {
        amountToRead = aSink.MaxLength();
        }
    //
    if  (amountToRead > 0)
        {
        // Do the read
        error = iFsSession.ReadFileSection( *iFileName, Offset(), aSink, amountToRead );
        if  (error == KErrNone)
            {
            SetOffset( Offset() + aSink.Length() );
            }
        }
    return error;
    }
















