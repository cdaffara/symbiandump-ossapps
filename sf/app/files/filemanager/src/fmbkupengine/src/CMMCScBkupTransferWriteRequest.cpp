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
* Description: CMMCScBkupWriteDataTransferRequestBase implementation
*
*
*/

#include "CMMCScBkupTransferWriteRequest.h"

// User includes
#include "MMCScBkupLogger.h"
#include "MMMCScBkupDriver.h"
#include "CMMCScBkupArchive.h"
#include "MMCScBkupSBEUtils.h"
#include "RMMCScBkupProgressSizer.h"
#include "MMMCScBkupProgressObserver.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "CMMCScBkupDriveAndOperationTypeManager.h"
#include <pathinfo.h>



#ifdef __MMCSCBKUP_DATA_LOGGING_ENABLED__

static void __DebugDump( const TDesC& aFmt, const TUint8* aAddress, const TUint8* aDisplayStartAddress, TInt aLength, TInt aMaxLength)
    {
    const TInt maxLen = aMaxLength;
    TInt len = aLength;
    const TUint8* pDataAddr = aAddress;

    TBuf<81> out;
    TBuf<20> ascii;
    TInt offset = 0;
    const TUint8* a = pDataAddr;
    const TUint8* displayAddress = aDisplayStartAddress;
    //
    while(len>0)
        {
        out.Zero();
        ascii.Zero();
        out.AppendNumFixedWidth((TUint) displayAddress, EHex, 8);
        out.Append(_L(": "));

        TUint b;
        for (b=0; b<16; b++)
            {
            TUint8 c = ' ';
            if  ((pDataAddr + offset + b) < pDataAddr + maxLen)
                {
                c = *(pDataAddr + offset + b);
                out.AppendNumFixedWidth(c, EHex, 2);
                }
            else
                {
                out.Append(_L("  "));
                }
            out.Append(' ');
            if (c<=0x20 || c>=0x7f || c=='%')
                c=0x2e;
            ascii.Append(TChar(c));
            }
        out.Append(ascii);
        out.ZeroTerminate();

        RDebug::Print(aFmt, &out);

        displayAddress += 16;
        a += 16;
        offset += 16;
        len -= 16;
        }
    }

#endif






// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::CMMCScBkupWriteDataTransferRequestBase()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupWriteDataTransferRequestBase::CMMCScBkupWriteDataTransferRequestBase( 
                                                MMMCScBkupDriver& aDriver, 
                                                MMMCScBkupIndexHandler& aIndexHandler,
                                                TMMCScBkupOwnerDataType aElementType,
                                                TSBDerivedType aExpectedTransferType,
                                                TInt aPriority )

:   CMMCScBkupTransferRequestBase( aDriver, aElementType, aPriority ), 
    iIndexHandler( aIndexHandler ),
    iExpectedTransferType( aExpectedTransferType )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::~CMMCScBkupWriteDataTransferRequestBase()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupWriteDataTransferRequestBase::~CMMCScBkupWriteDataTransferRequestBase()
    {
    delete iTransferType;
    if  ( iStreamIsOpen )
        {
        iStream.Close();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupWriteDataTransferRequestBase::RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver, const CMMCScBkupDriveAndOperationTypeManager& aDriveAndOperations )
    {
#ifdef __MMCSCBKUPLOGGING_ENABLED__
    if ( ElementType() == EMMCScBkupOwnerDataTypeJavaData )
        {
        HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( aOwner.Owner().Identifier() );
        __LOG2("CMMCScBkupWriteDataTransferRequestBase::RequestL() - START - reading data from SBE for JavaHash: %S, ElementType: %S", hash, &MMCScBkupLogger::DataType( ElementType() ));
        CleanupStack::PopAndDestroy( hash );
        }
    else
        {
        __LOG2("CMMCScBkupWriteDataTransferRequestBase::RequestL() - START - reading data from SBE for DO: 0x%08x, ElementType: %S", aOwner.SecureId().iId, &MMCScBkupLogger::DataType( ElementType() ));
        }
#endif

    iDriveAndOperations = &aDriveAndOperations;
    CMMCScBkupTransferRequestBase::RequestL( aOwner, aObserver );
    //
    iDriveFilter.Reset();
    iDriveFilter.SetPrimaryDriveFilter( iDriveAndOperations->DriveList() );
    iDriveFilter.SetSecondaryDriveFilter( aOwner.Owner().DriveList() );
    
    // Set us going
    SetState( ETransferData );
    CompleteSelf();
    __LOG("CMMCScBkupWriteDataTransferRequestBase::RequestL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL()
    {
    MMMCScBkupDriver& driver = Driver();
    MMMCScBkupArchiveDataInterface& archiveDataInterface = driver.DrvADI();

    // Get a handle to the received data
    TBool finished = EFalse;
    CSBGenericTransferType* receivedTransferInfo = NULL;
    const TPtrC8& pData = driver.DrvSecureBackupClient().TransferDataInfoL( receivedTransferInfo, finished );
    iAdditionalTransferRequired = !finished;
    
    __LOG4("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - received: %d bytes from SBE for SID: 0x%08x, Drive: %c:, ElementType: %S", pData.Length(), DataOwner().SecureId().iId, 'A' + CurrentDrive(), &MMCScBkupLogger::DataType( ElementType() ) );

    // Validate the data
    CleanupStack::PushL( receivedTransferInfo );
    ValidateTransferInfoL( receivedTransferInfo );
    CleanupStack::PopAndDestroy( receivedTransferInfo );

    // If we've never written any registration data before, then we must
    // request a write stream at the current archive write pos...
    if  ( pData.Length() && !iStreamIsOpen )
        {
        __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - stream isnt open - opening now..." );

        iStream = archiveDataInterface.ADIWriteStreamUncompressedLC();
        CleanupStack::Pop(); // iWriteStream - gets closed in dtor in case of leave
        iStreamIsOpen = ETrue;
        }

    // Write the data - assumes stream already open if this is not the
    // first time that we've received data for this drive...
    //
    // We don't use the externalisation operators, as we want to write
    // only the data (no leading byte count etc).
    if  ( pData.Length() )
        {
        __LOG1("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - got %d bytes of data...", pData.Length() );
        
#ifdef __MMCSCBKUP_DATA_LOGGING_ENABLED__
        _LIT(KDumpFormat, "SBEData - %S - ");
        __DebugDump( KDumpFormat, pData.Ptr(), pData.Ptr(), pData.Length(), pData.Length() );
#endif

        // Write the data to the stream
        __LOG1("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - writing %d bytes to stream...", pData.Length() );
        iStream.WriteL(pData);
        __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - data written OK");

        // Update our stats. We overwrite any earlier estimate from the SBE
        // with the actual values that we ultimately end up writing to the
        // archive. This allows us to provide a more accurate restore progress
        // experience.
        DataTransferred( pData.Length() );

#ifdef DEBUGGING_DATA_TRANSFER
        TRAP_IGNORE( DumpTransferDataL( archiveDataInterface.ADIFsSession(), pData ) );
#endif
        }

    // If we've received everything we can commit & close the stream
    if  ( finished )
        {
        __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - finished...");

        // Implicitly, if the stream is open then we have written some
        // data to the archive. Therefore we only add an index record
        // when the stream is open.
        if  ( iStreamIsOpen )
            {
            __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - Committing stream...");
            iStream.CommitL();

            __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - Closing stream...");
            iStream.Close();
            iStreamIsOpen = EFalse;

            // Must store the position, as we're going to also write an index at
            // the end of this whole process.
            __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - Adding index record...");
            CMMCScBkupArchiveFooter& footer = driver.DrvArchive().Footer();
            const TMMCScBkupArchiveVector& info = archiveDataInterface.ADICurrentArchiveVectorInfo();
            
            // If we actually wrote something to the archive, then add an index record.
            if  (info.Length())
                {
                iIndexHandler.AddIndexRecordL( footer, DataOwner(), info, CurrentDrive() );
                }
            }
        }

    // Not yet supporting chunked transfer
    __LOG("CMMCScBkupWriteDataTransferRequestBase::ProcessChunkOfDataL() - END");
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL()
    {
    // If we must make a repeat transfer request, then we don't yet
    // move to the next drive.
    if  ( iAdditionalTransferRequired )
        {
        // Must ask for more of the same data, so no need
        // to rebuild transfer info - we just reuse it
        RequestDataTransferL();
        }
    else
        {
        // Can move to the next drive
        TDriveNumber drive = EDriveA;
        const TBool driveAvailable = iDriveFilter.NextValidDrive( drive );
        __LOG1("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - current drive: %c", drive + 'A');
        __LOG1("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - driveAvailable: %d", driveAvailable);

        if  ( driveAvailable )
            {
            const TBool dataTransferAllowableForDrive = iDriveAndOperations->IsDataTypeAllowedToAccessDrive( drive, ElementType() );
            __LOG1("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - dataTransferAllowableForDrive: %d", dataTransferAllowableForDrive);

            if  ( dataTransferAllowableForDrive )
                {
                __LOG1("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - making transfer request for drive: %c", drive + 'A');

                // Update the current drive
                SetCurrentDrive( drive );

                // Make virtual function call to make transfer object
                const CSBGenericDataType& generic = DataOwner().Owner().Identifier();
                CSBGenericTransferType* transferType = PrepareTransferTypeL( generic, CurrentDrive(), DataOwner().Version() );
                delete iTransferType;
                iTransferType = transferType;

                // We have a transfer packet, so now request data
                RequestDataTransferL();
                }
            else
                {
                // Couldn't process this drive, let's try to find another drive.
                // This means we don't change state this time.
                __LOG("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - not allowed to back up this drive... trying next one...");
                CompleteSelf( KErrNone );
                }
            }
        else
            {
            // No more drives left, we've finished!
            __LOG("CMMCScBkupWriteDataTransferRequestBase::PrepareDataTransferL() - no more drives to process - we are finished!");
            SetState( EFinished );
            CompleteSelf();
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::CurrentDrive()
// 
// 
// ---------------------------------------------------------------------------
TDriveNumber CMMCScBkupWriteDataTransferRequestBase::CurrentDrive() const
    {
    return iCurrentDrive;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::ValidateTransferInfoL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupWriteDataTransferRequestBase::ValidateTransferInfoL( CSBGenericTransferType* aInfo )
    {
    if  ( !aInfo )
        {
        User::Leave( KErrCorrupt );
        }

    // Check the type is as expected
    const TSBDerivedType type = aInfo->DerivedTypeL();
    if  ( type != iExpectedTransferType )
        {
        __LOG2("CMMCScBkupWriteDataTransferRequestBase::ValidateTransferInfoL() - ERROR - types don not match! type: %d vs expected: %d", type, iExpectedTransferType );
        User::Leave( KErrNotSupported );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::RequestDataTransferL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupWriteDataTransferRequestBase::RequestDataTransferL()
    {
    __LOG3("CMMCScBkupWriteDataTransferRequestBase::RequestDataTransferL() - requesting data from SBE for SID: 0x%08x, Drive: %c:, ElementType: %S", DataOwner().SecureId().iId, 'A' + CurrentDrive(), &MMCScBkupLogger::DataType( ElementType() ) );

    CSBEClient& sbeClient = Driver().DrvSecureBackupClient();
    sbeClient.RequestDataL( *iTransferType, iStatus );
    SetActive();
    SetState( EProcessData );

    // Report the progress
    RMMCScBkupProgressSizer progressSizer( Driver().DrvParamsBase().DriveAndOperations() );
    progressSizer.BackupReportFixedProgressForOpL( Driver().DrvProgressHandler(), ElementType() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupWriteDataTransferRequestBase::RunError( TInt aError )
    {
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    const TSecureId sid = DataOwner().SecureId();
    __LOGFILE3("CMMCScBkupWriteDataTransferRequestBase::RunError() - **** - aError: %d, sid: 0x%08x, drive: %c", aError, sid.iId, 'A' + CurrentDrive() );
#endif

    TInt ret = KErrNone;
    
    // Make sure we close the stream (if we had it open)
    if  ( iStreamIsOpen )
        {
        iStream.Close();
        iStreamIsOpen = EFalse;
        }

    // Now notify the observer or silently try again (depends on type of error)
    if  ( aError == KErrNotFound )
        {
        __LOGFILE1("CMMCScBkupWriteDataTransferRequestBase::RunError() - **** - non fatal error (%d) so trying the next drive...", aError);

        // Try the next drive - if none is available, it will complete the
        // observer as if nothing went wrong.
        iAdditionalTransferRequired = EFalse;
        SetState( ETransferData );
        CompleteSelf();
        }
    else
        {
        __LOGFILE1("CMMCScBkupWriteDataTransferRequestBase::RunError() - **** - FATAL ERROR (%d)", aError);
        ret = CMMCScBkupTransferRequestBase::RunError( aError );
        }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupWriteDataTransferRequestBase::DumpTransferDataL()
// 
// 
// ---------------------------------------------------------------------------
#ifdef DEBUGGING_DATA_TRANSFER
void CMMCScBkupWriteDataTransferRequestBase::DumpTransferDataL( RFs& aFsSession, const TDesC8& aData ) const
    {
    TPtrC subDirectory( KNullDesC );
    //
    switch( ElementType() )
        {
    case EMMCScBkupOwnerDataTypeJavaData:
        subDirectory.Set(KMMCScBkupDataTransferDebuggingPathDataJava);
        break;
    case EMMCScBkupOwnerDataTypeSystemData:
        subDirectory.Set(KMMCScBkupDataTransferDebuggingPathDataSystem);
        break;
    case EMMCScBkupOwnerDataTypePassiveData:
        subDirectory.Set(KMMCScBkupDataTransferDebuggingPathDataPassive);
        break;
    case EMMCScBkupOwnerDataTypeActiveData:
        subDirectory.Set(KMMCScBkupDataTransferDebuggingPathDataActive);
        break;
    default:
        User::Leave( KErrNotSupported );
        break;
        }
    //
    const TSecureId secureId = DataOwner().SecureId();
    _LIT(KMMCScBkupFormatDes, "%S%S");
    TFileName transferDumpFileName;
    const TDesC& path = PathInfo::MemoryCardRootPath();
    transferDumpFileName.Format(KMMCScBkupFormatDes, &path, &KMMCScBkupDataTransferDebuggingPathRoot);

    transferDumpFileName.Append( subDirectory );
    transferDumpFileName.Append( KMMCScBkupDataTransferDebuggingPathDataBackup );
    transferDumpFileName.AppendFormat( KMMCScBkupDataTransferDebuggingFileName, secureId.iId, 'a' + CurrentDrive() );
    //
    RFile64 file;
    TInt error = KErrNone;
    TEntry entry;
    if  ( aFsSession.Entry( transferDumpFileName, entry ) == KErrNone )
        {
        // Already exists - append data
        error = file.Open( aFsSession, transferDumpFileName, EFileWrite | EFileStream | EFileShareExclusive );
        }
    else
        {
        entry.iSize = 0;
        error = file.Create( aFsSession, transferDumpFileName, EFileWrite | EFileStream | EFileShareExclusive );
        }
    //
    User::LeaveIfError( error );
    CleanupClosePushL( file );
    error = file.Write( entry.iSize, aData );
    CleanupStack::PopAndDestroy( &file );
    }
#endif












