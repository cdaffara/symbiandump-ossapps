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
* Description: CMMCScBkupReadDataTransferRequestBase implementation
*
*
*/

#include "CMMCScBkupTransferReadRequest.h"

// User includes
#include "MMCScBkupLogger.h"
#include "MMMCScBkupDriver.h"
#include "CMMCScBkupArchive.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "MMMCScBkupProgressObserver.h"
#include <pathinfo.h>

// Constants
const TBool KMMCScBkupDontUpdateOpSizes = EFalse;



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::CMMCScBkupReadDataTransferRequestBase()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupReadDataTransferRequestBase::CMMCScBkupReadDataTransferRequestBase( 
                        MMMCScBkupDriver& aDriver,
                        TMMCScBkupOwnerDataType aElementType,
                        TInt aChunkSize,
                        TInt aPriority )
:   CMMCScBkupTransferRequestBase( aDriver, aElementType, KMMCScBkupDontUpdateOpSizes, aPriority ), 
    iReadChunkSize( aChunkSize ),
    iCurrentIndex( -1 )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::~CMMCScBkupReadDataTransferRequestBase()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupReadDataTransferRequestBase::~CMMCScBkupReadDataTransferRequestBase()
    {
    delete iTransferType;
    delete iTemporaryTransferSink;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupReadDataTransferRequestBase::RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver, const RArray<TMMCScBkupArchiveDriveAndVector>& aEntries )
    {
#ifdef __MMCSCBKUPLOGGING_ENABLED__
    if ( ElementType() == EMMCScBkupOwnerDataTypeJavaData )
        {
        HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( aOwner.Owner().Identifier() );
        __LOG2("CMMCScBkupReadDataTransferRequestBase::RequestL() - START - reading data from SBE for JavaHash: %S, ElementType: %S", hash, &MMCScBkupLogger::DataType( ElementType() ));
        CleanupStack::PopAndDestroy( hash );
        }
    else
        {
        __LOG2("CMMCScBkupReadDataTransferRequestBase::RequestL() - START - reading data from SBE for DO: 0x%08x, ElementType: %S", aOwner.SecureId().iId, &MMCScBkupLogger::DataType( ElementType() ));
        }
#endif

    CMMCScBkupTransferRequestBase::RequestL( aOwner, aObserver );
    //
    if  ( !iTemporaryTransferSink )
        {
        iTemporaryTransferSink = HBufC8::NewL( iReadChunkSize );        
        }
    //
    iEntries = &aEntries;
    iCurrentIndex = -1; // Increased by one in PrepareForNextEntry
    //
    TState nextState = EProcessData;
    const TBool entryAvailable = PrepareForNextEntry();
    if  ( !entryAvailable )
        {
        nextState = EFinished;
        }
    //
    SetState( nextState );
    CompleteSelf();
    __LOG1("CMMCScBkupReadDataTransferRequestBase::RequestL() - END - nextState: %d", nextState);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::ProcessChunkOfDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupReadDataTransferRequestBase::ProcessChunkOfDataL()
    {
    __ASSERT_ALWAYS( iCurrentIndex >= 0, User::Invariant() );

    TBool callAgain = EFalse;

    // Do we need to move to the next entry?
    TBool entryAvailable = ( iCurrentIndex < iEntries->Count() );
    if  ( entryAvailable )
        {
        // At least one more entry left to process.. but have we finished
        // the current entry? If we've read all the data, then the
        // answer is yes...
        const TMMCScBkupArchiveDriveAndVector& entry = (*iEntries)[ iCurrentIndex ];
        const TInt endOffset = entry.iVector.EndOffset();

#ifdef __MMCSCBKUPLOGGING_ENABLED__
		if ( ElementType() == EMMCScBkupOwnerDataTypeJavaData )
			{
			HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( DataOwner().Owner().Identifier() );
			__LOG6("CMMCScBkupReadDataTransferRequestBase::ProcessChunkOfDataL() - START - JavaHash: %S, ElementType: %S, offset: %8d, length: %8d, endOffset: %8d, drive: %c:", hash, &MMCScBkupLogger::DataType( ElementType() ), entry.iVector.Offset(), entry.iVector.Length(), endOffset, entry.iDrive + 'A' );
			CleanupStack::PopAndDestroy( hash );
			}
		else
			{
			__LOG6("CMMCScBkupReadDataTransferRequestBase::ProcessChunkOfDataL() - START - DO: 0x%08x, ElementType: %S, offset: %8d, length: %8d, endOffset: %8d, drive: %c:", DataOwner().SecureId().iId, &MMCScBkupLogger::DataType( ElementType() ), entry.iVector.Offset(), entry.iVector.Length(), endOffset, entry.iDrive + 'A' );
			}
#endif

        // Check bounds
        if  ( CurrentOffset() < 0 || CurrentOffset() > endOffset )
            {
            ASSERT( EFalse );
            User::Leave( KErrCorrupt );
            }
        //
        if  ( iCurrentOffset == endOffset )
            {
            // We're done with this entry - move to the next
            entryAvailable = PrepareForNextEntry();
            }
        }
    //
    if  ( entryAvailable )
        {
        // We need to read more archive data in order to transfer this
        // entry to the SBE
        callAgain = DoProcessChunkOfDataL();
        }
    else
        {
        // No more left to process - we're finished!
        SetState( EFinished );
        CompleteSelf();
        }
    //
    return callAgain;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::PrepareDataTransferL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupReadDataTransferRequestBase::PrepareDataTransferL()
    {
    __ASSERT_ALWAYS(iTransferChunkPointer != NULL, User::Invariant());
    const CSBGenericDataType& generic = DataOwner().Owner().Identifier();

    // Prepare the transfer type with a virtual function call...
    CSBGenericTransferType* transferType = PrepareTransferTypeL( generic, CurrentDrive(),
         DataOwner().Version());
    delete iTransferType;
    iTransferType = transferType;

#ifdef __MMCSCBKUPLOGGING_ENABLED__
    TInt lengthOfDataReadyForTransfer = 0;
    if  ( iTransferChunkPointer != NULL )
        {
        lengthOfDataReadyForTransfer = iTransferChunkPointer->Length();
        }

    if ( iTransferType->DerivedTypeL() == EJavaTransferDerivedType )
        {
        HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( DataOwner().Owner().Identifier() );
        __LOG5("CMMCScBkupReadDataTransferRequestBase::PrepareDataTransferL() - supplying %d bytes of data for has: %S, drive: %c, iTransferChunkPointer addr: 0x%08x, iTransferChunkPointer length: %d", lengthOfDataReadyForTransfer, hash, 'A' + CurrentDrive(), iTransferChunkPointer->Ptr(), iTransferChunkPointer->Length() );
        CleanupStack::PopAndDestroy( hash );
        }
    else
        {
        const TSecureId sid = DataOwner().SecureId();
        __LOG5("CMMCScBkupReadDataTransferRequestBase::PrepareDataTransferL() - supplying %d bytes of data for sid: 0x%08x, drive: %c, iTransferChunkPointer addr: 0x%08x, iTransferChunkPointer length: %d", lengthOfDataReadyForTransfer, sid.iId, 'A' + CurrentDrive(), iTransferChunkPointer->Ptr(), iTransferChunkPointer->Length() );
        }
#endif

    //
    Driver().DrvSecureBackupClient().SupplyDataL( *iTransferType, iFinishedSupplyingData, iStatus );
    SetActive();
    
    // Reset
    ResetDataTransferCounter();
    iTransferChunkPointer = NULL; // Address is retrieved from global chunk in DoProcessChunkOfDataL
    
    __LOG("CMMCScBkupReadDataTransferRequestBase::PrepareDataTransferL() - data transmitted, waiting for request completion...");

    // When the SBE informs us that it has read the data, we'll need to send it the next 
    // chunk.
    SetState( EProcessData );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::ReadChunkL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupReadDataTransferRequestBase::ReadChunkL( TDes8& aSink, TInt aLength )
    {
    const TInt endOffset = CurrentReadInfo().EndOffset();
    __LOG4("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - START - aLength: %d, endOffset: %d, iCurrentOffset: %d, sinkLength: %d", aLength, endOffset, iCurrentOffset, aSink.Length());
    //
    if  ( iCurrentOffset >= 0 && iCurrentOffset <= endOffset )
        {
        if  ( aLength > 0 )
            {
            MMMCScBkupArchiveDataInterface& archiveDataInterface = Driver().DrvADI();

            // We read based upon the current offset and the specified length
            TPtr8 sink( iTemporaryTransferSink->Des() );
            const TMMCScBkupArchiveVector readRequest( iCurrentOffset, aLength );
            const TMMCScBkupArchiveVector& readResult = archiveDataInterface.ADIReadL( sink, readRequest );
            __LOG2("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - read result - offset: %d, length: %d", readResult.Offset(), readResult.Length());
            
            // Update offset
            iCurrentOffset += readResult.Length();
            aSink.Append( sink );
            __LOG2("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - new offset (iCurrentOffset): %d, sink Length: %d", iCurrentOffset, aSink.Length());

#ifdef DEBUGGING_DATA_TRANSFER
            TRAP_IGNORE( DumpTransferDataL( archiveDataInterface.ADIFsSession(), sink) );
#endif

            if  ( iCurrentOffset > CurrentReadInfo().EndOffset() )
                {
                __LOG("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - Fatal Error - read went beyond archive entry boundary!");
                ASSERT( EFalse );
                User::Leave( KErrCorrupt );
                }
            else
                {
                __LOG("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - read was okay - updating stats & reporting progress...");

                // We managed to read the data okay, so let's update our stats
                DataTransferred( readResult.Length() );

                // ... and also update progress
                Driver().DrvProgressHandler().MMCScBkupHandleProgress( readResult.Length() );
                }
            }
        }
    else
        {
        __LOG("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - Fatal Error - current offset out of bounds!");
        ASSERT( EFalse );
        User::Leave( KErrCorrupt );
        }

    __LOG("CMMCScBkupReadDataTransferRequestBase::ReadChunkL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL()
    {
    MMMCScBkupDriver& driver = Driver();
    const TInt startingOffset = CurrentOffset();
    const TInt finalOffset = CurrentReadInfo().EndOffset();

    // Calculate how much data is remaining
    const TInt amountOfDataStillToBeRead = finalOffset - startingOffset;

    __LOG4("CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL() - START - startingOffset: 0x%08x, finalOffset: 0x%08x, amountToBeRead: %8d, iFinishedSupplyingData: %d", 
        startingOffset, finalOffset, amountOfDataStillToBeRead, iFinishedSupplyingData);

    // Get a handle to the received data - but only if we don't already
    // have a handle to it! Calling this method resets whatever is already
    // in the transfer chunk - hence we must only call it once (at the beginning
    // of a multi-chunked transfer)
    if  ( iTransferChunkPointer == NULL )
        {
        iTransferChunkPointer = &driver.DrvSecureBackupClient().TransferDataAddressL();
        }

    // How much space is available?
    const TInt spaceAvailable = iTransferChunkPointer->MaxLength() - iTransferChunkPointer->Length();
    const TInt amountToRead = Min( spaceAvailable, Min( iReadChunkSize, amountOfDataStillToBeRead ) );
    __LOG2("CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL() - xfer space available: %8d, amountToRead: %8d", spaceAvailable, amountToRead);

    // Do the read
    ReadChunkL( *iTransferChunkPointer, amountToRead );

    // Was this the last time we needed to perform a read?
    const TBool lastReadFromArchive = ( CurrentOffset() == finalOffset );
    __LOG1("CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL() - read data okay - lastReadFromArchive: %d", lastReadFromArchive);

    // Check to see if the data sink chunk is full. If so then we need to let 
    // the SBE process what we've prepared so far, then we'll fill it with more 
    // data. 
    iFinishedSupplyingData = lastReadFromArchive;
    TBool moreToBeRead = !lastReadFromArchive;
    if  ( !iFinishedSupplyingData && iTransferChunkPointer->Length() == iTransferChunkPointer->MaxLength() )
        {
        // Also cope with the boundary condition that the sink is full
        // but we've also just given it the last byte anyway. In which case,
        // we don't need to request a repeat transfer. This is actually
        // all the data in one go.
        moreToBeRead = EFalse;
        }
    //
    __LOG1("CMMCScBkupReadDataTransferRequestBase::DoProcessChunkOfDataL() - END - more to read: %d", moreToBeRead);
    return moreToBeRead;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::PrepareForNextEntry()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupReadDataTransferRequestBase::PrepareForNextEntry()
    {
    TBool anotherEntryAvailable = EFalse;
    
    // Update our starting offset
    iFinishedSupplyingData = EFalse;
    iCurrentOffset = -1;
    //
    if  ( ++iCurrentIndex < iEntries->Count() )
        {
        const TMMCScBkupArchiveVector& currentReadInfo = CurrentReadInfo();
        iCurrentOffset = currentReadInfo.Offset();

        __LOG3("CMMCScBkupReadDataTransferRequestBase::PrepareForNextEntry() - offset: %6d, length: %6d, drive: %c:", currentReadInfo.Offset(), currentReadInfo.Length(), 'A' + CurrentDrive() );

        anotherEntryAvailable = ETrue;
        }
    //
    return anotherEntryAvailable;
    }
 

// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::CurrentDrive()
// 
// 
// ---------------------------------------------------------------------------
TDriveNumber CMMCScBkupReadDataTransferRequestBase::CurrentDrive() const
    {
    return CurrentEntry().iDrive;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::CurrentReadInfo()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& CMMCScBkupReadDataTransferRequestBase::CurrentReadInfo() const
    {
    return CurrentEntry().iVector;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::CurrentEntry()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveDriveAndVector& CMMCScBkupReadDataTransferRequestBase::CurrentEntry() const
    {
    __ASSERT_ALWAYS( iCurrentIndex >= 0 && iCurrentIndex < iEntries->Count(), User::Invariant() );

    // Get the next entry
    const TMMCScBkupArchiveDriveAndVector& entry = (*iEntries)[ iCurrentIndex ];
    return entry;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupReadDataTransferRequestBase::RunError( TInt aError )
    {
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    const TSecureId sid = DataOwner().SecureId();
    __LOGFILE3("CMMCScBkupReadDataTransferRequestBase::RunError() - **** - aError: %d, sid: 0x%08x, drive: %c", aError, sid.iId, 'A' + CurrentDrive() );
#endif

    TInt ret = KErrNone;

    if  ( aError == KErrNotFound )
        {
        // KErrNotFound is treated as non-fatal.
        const TBool entryAvailable = PrepareForNextEntry();

        // Try the next entry (if any)
        TState nextState = EProcessData;
        if  ( entryAvailable )
            {
            __LOGFILE("CMMCScBkupReadDataTransferRequestBase::RunError() - **** - another drive available for same DO - trying the next drive...");
            nextState = EProcessData;
            }
        else
            {
            // Done
            __LOGFILE("CMMCScBkupReadDataTransferRequestBase::RunError() - **** - no drives let for same DO...");
            nextState = EFinished;
            }

        SetState( nextState );
        CompleteSelf();
        }
    else
        {
        // Everything else is fatal
        __LOGFILE1( "CMMCScBkupReadDataTransferRequestBase::RunError() - **** - FATAL ERROR (%d)", aError );
        ret = CMMCScBkupTransferRequestBase::RunError( aError );
        }
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupReadDataTransferRequestBase::DumpTransferDataL()
// 
// 
// ---------------------------------------------------------------------------
#ifdef DEBUGGING_DATA_TRANSFER
void CMMCScBkupReadDataTransferRequestBase::DumpTransferDataL( RFs& aFsSession, const TDesC8& aData ) const
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
    transferDumpFileName.Append( KMMCScBkupDataTransferDebuggingPathDataRestore );
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






