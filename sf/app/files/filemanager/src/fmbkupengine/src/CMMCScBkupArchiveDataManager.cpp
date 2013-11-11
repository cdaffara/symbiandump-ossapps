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
* Description: CMMCScBkupArchiveDataManager Implementation
*
*
*/

#include "CMMCScBkupArchiveDataManager.h"

// System includes
#include <ezcompressor.h>
#include <ezdecompressor.h>

// User includes
#include "MMCScBkupLogger.h"
#include "RMMCScBkupArchiveStreams.h"
#include "MMCScBkupOperations.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CMMCScBkupArchiveDataManager()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveDataManager::CMMCScBkupArchiveDataManager( RFs& aFsSession, RFile64& aFile, MMMCScBkupProgressObserver& aProgressManager )
:   CActive( CActive::EPriorityIdle ), iFsSession( aFsSession ), iFile( aFile ), iProgressManager( aProgressManager )
    {
    __LOG1("CMMCScBkupArchiveDataManager::CMMCScBkupArchiveDataManager() - START - aFile: 0x%08x", aFile.SubSessionHandle() );
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::~CMMCScBkupArchiveDataManager()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveDataManager::~CMMCScBkupArchiveDataManager()
    {
    Cancel();
    //
    iWriteStream.Close();
    //
    delete iCompressor;
    delete iDecompressor;
    delete iBufferManager;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ConstructL()
    {
    TInt64 size = 0;
    User::LeaveIfError( iFile.Size( size ) );
    //
    iOverallArchiveVectorInfo.SetOffset( 0 );
    iOverallArchiveVectorInfo.SetLength( size );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveDataManager* CMMCScBkupArchiveDataManager::NewL( RFs& aFsSession, RFile64& aFile, MMMCScBkupProgressObserver& aProgressManager )
    {
    CMMCScBkupArchiveDataManager* self = new(ELeave) CMMCScBkupArchiveDataManager( aFsSession, aFile, aProgressManager );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CurrentOffsetL()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupArchiveDataManager::CurrentOffsetL() const
    {
    TInt offset = CurrentOffset();
    User::LeaveIfError(offset);
    return offset;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CurrentOffset()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupArchiveDataManager::CurrentOffset() const
    {
    TInt64 offsetOrError = 0;
    TInt error = iFile.Seek( ESeekCurrent, offsetOrError );
    //
    if  (error != KErrNone)
        {
        offsetOrError = error;
        }
    //
    return offsetOrError;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::SetCurrentVector()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::SetCurrentVector( const TMMCScBkupArchiveVector& aVector )
    {
    iCurrentVectorInfo = aVector;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIFsSession()
// 
// 
// ---------------------------------------------------------------------------
RFs& CMMCScBkupArchiveDataManager::ADIFsSession() const
    {
    return iFsSession;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIRawArchiveFile()
// 
// 
// ---------------------------------------------------------------------------
RFile64& CMMCScBkupArchiveDataManager::ADIRawArchiveFile() const
    {
    return iFile;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADICurrentArchiveVectorInfo()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& CMMCScBkupArchiveDataManager::ADICurrentArchiveVectorInfo() const
    {
    return iCurrentVectorInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIOverallArchiveVectorInfo()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& CMMCScBkupArchiveDataManager::ADIOverallArchiveVectorInfo() const
    {
    return iOverallArchiveVectorInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIWriteStreamUncompressedLC()
// 
// 
// ---------------------------------------------------------------------------
RWriteStream& CMMCScBkupArchiveDataManager::ADIWriteStreamUncompressedLC( TInt aPos )
    {
    __LOG1("CMMCScBkupArchiveDataManager::ADIWriteStreamUncompressedLC() - START - aPos: %d", aPos );
    //
    __ASSERT_ALWAYS( !iWriteStream.IsOpen(), User::Invariant() );
    TInt offset = aPos;
    if  (offset == KMMCScBkupArchiveWriteStreamCurrentPos)
        {
        offset = CurrentOffsetL();
        __LOG1("CMMCScBkupArchiveDataManager::ADIWriteStreamUncompressedLC() - current offset is: %d", offset );
        }
    //
    iWriteStream.OpenLC(*this, iFile, offset);

    __LOG("CMMCScBkupArchiveDataManager::ADIWriteStreamUncompressedLC() - END - stream opened and pushed");

    iCurrentVectorInfo.Reset();
    return iWriteStream;
    }
  

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIReadStreamUncompressedLC()
// 
// 
// ---------------------------------------------------------------------------
RReadStream& CMMCScBkupArchiveDataManager::ADIReadStreamUncompressedLC( TInt aPos )
    {
    __ASSERT_ALWAYS( !iReadStream.IsOpen(), User::Invariant() );
    TInt offset = aPos;
    if  (offset == KMMCScBkupArchiveReadStreamCurrentPos)
        {
        offset = CurrentOffsetL();
        }
    //
    iReadStream.OpenLC(*this, iFile, offset);
    iCurrentVectorInfo.Reset();
    return iReadStream;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIWriteL()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& CMMCScBkupArchiveDataManager::ADIWriteL( const TDesC8& aData )
    {
    const TInt offset = CurrentOffsetL();
    const TInt error = iFile.Write( aData );
    User::LeaveIfError(error);
    
#ifdef RD_FILE_MANAGER_BACKUP
    CalculateCrc(aData.Ptr(), aData.Length());
#endif
    //
    iCurrentVectorInfo.SetOffset( offset );
    iCurrentVectorInfo.SetLength( aData.Length() );
    //
    return iCurrentVectorInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIReadL()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& CMMCScBkupArchiveDataManager::ADIReadL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo )
    {
    TMMCScBkupArchiveVector readInfo(aInfo);

    // Read straight into aSink.
    const TInt maxLength = aSink.MaxLength();
    if  (aInfo.Length() > maxLength)
        {
        readInfo.SetLength( maxLength );
        }

    aSink.Zero();
    const TInt error = iFile.Read( static_cast<TInt64>(readInfo.Offset()), aSink, readInfo.Length() );
    User::LeaveIfError( error );
    //
    iCurrentVectorInfo.SetOffset( readInfo.Offset() );
    iCurrentVectorInfo.SetLength( aSink.Length() );
    //
    return iCurrentVectorInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIWriteFileL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIWriteFileL( const TDesC& aSourceFileName, TRequestStatus& aStatus )
    {
    __LOG1("CMMCScBkupArchiveDataManager::ADIWriteFileL() - START - file: %S", &aSourceFileName);

    if  (!iBufferManager || iBufferManager->Type() != CMMCScBkupBufferManagerBase::ETypeFileToArchive)
        {
        CMMCScBkupBufferManagerBase* bm = 
            CMMCScBkupBufferManagerBase::NewByTypeL(
                    CMMCScBkupBufferManagerBase::ETypeFileToArchive, 
                    iProgressManager,
                    *this, 
                    (TAny*) &aSourceFileName);
        //
        delete iBufferManager;
        iBufferManager = bm;
        }
    else
        {
        iBufferManager->InitializeL( (TAny*) &aSourceFileName );
        }

    // Setup our observer & op
    SetOperation(EOperationCompressing);
    SetObserver(aStatus);

    // Compressing aData to the archive file
    EnsureCompressorExistsL(*iBufferManager);

    // Do the compression asynchronously
    CompleteSelf();

    __LOG("CMMCScBkupArchiveDataManager::ADIWriteFileL() - END");
    }
 

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIReadFileL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIReadFileL( const TDesC& aDestinationFileName, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus )
    {
    __LOG3("CMMCScBkupArchiveDataManager::ADIReadFileL() - START - file: %S, offset: %8d, length: %8d", &aDestinationFileName, aInfo.Offset(), aInfo.Length());

    if  (!iBufferManager || iBufferManager->Type() != CMMCScBkupBufferManagerBase::ETypeArchiveToFile)
        {
        CMMCScBkupBufferManagerBase* bm = 
            CMMCScBkupBufferManagerBase::NewByTypeL(
                    CMMCScBkupBufferManagerBase::ETypeArchiveToFile, 
                    iProgressManager,
                    *this, 
                    (TAny*) &aDestinationFileName,
                    (TAny*) &aInfo );
        //
        delete iBufferManager;
        iBufferManager = bm;
        }
    else
        {
        iBufferManager->InitializeL( (TAny*) &aDestinationFileName, (TAny*) &aInfo );
        }

    // Setup our observer & op
    SetOperation(EOperationDecompressing);
    SetObserver(aStatus);

    // Compressing aData to the archive file
    EnsureDecompressorExistsL(*iBufferManager);

    // Do the compression asynchronously
    CompleteSelf();

    __LOG("CMMCScBkupArchiveDataManager::ADIReadFileL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIWriteCompressedL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIWriteCompressedL( const TDesC8& aData, TRequestStatus& aStatus )
    {
    if  (!iBufferManager || iBufferManager->Type() != CMMCScBkupBufferManagerBase::ETypeDescriptorToArchive)
        {
        CMMCScBkupBufferManagerBase* bm = 
            CMMCScBkupBufferManagerBase::NewByTypeL(
                    CMMCScBkupBufferManagerBase::ETypeDescriptorToArchive, 
                    iProgressManager,
                    *this, 
                    (TAny*) &aData );
        //
        delete iBufferManager;
        iBufferManager = bm;
        }
    else
        {
        iBufferManager->InitializeL( (TAny*) &aData );
        }

    // Setup our observer & op
    SetOperation(EOperationCompressing);
    SetObserver(aStatus);

    // Compressing aData to the archive file
    EnsureCompressorExistsL(*iBufferManager);

    // Do the compression asynchronously
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIReadDecompressedL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIReadDecompressedL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus )
    {
    if  (!iBufferManager || iBufferManager->Type() != CMMCScBkupBufferManagerBase::ETypeArchiveToDescriptor)
        {
        CMMCScBkupBufferManagerBase* bm = 
            CMMCScBkupBufferManagerBase::NewByTypeL(
                    CMMCScBkupBufferManagerBase::ETypeArchiveToDescriptor, 
                    iProgressManager,
                    *this, 
                    (TAny*) &aSink,
                    (TAny*) &aInfo  );
        //
        delete iBufferManager;
        iBufferManager = bm;
        }
    else
        {
        iBufferManager->InitializeL( (TAny*) &aSink, (TAny*) &aInfo );
        }

    // Setup our observer & op
    SetOperation(EOperationDecompressing);
    SetObserver(aStatus);

    // Compressing aData to the archive file
    EnsureDecompressorExistsL(*iBufferManager);

    // Do the compression asynchronously
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIAsynchronousCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIAsynchronousCancel()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::ADIResetResources()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::ADIResetResources(TMMCScBkupOperationType /* aType */)
    {
    __LOG("CMMCScBkupArchiveDataManager::ADIResetResources()");
    
    delete iCompressor;
    iCompressor = NULL;
    delete iDecompressor;
    iDecompressor = NULL;
    delete iBufferManager;
    iBufferManager = NULL;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::EnsureCompressorExistsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::EnsureCompressorExistsL( MEZBufferManager& aBufferManager )
    {
    delete iDecompressor;
    iDecompressor = NULL;
    //
    if  (iCompressor == NULL)
        {
        iCompressor = CEZCompressor::NewL( aBufferManager );
        }
    else
        {
        iCompressor->ResetL(aBufferManager);
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::EnsureDecompressorExistsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::EnsureDecompressorExistsL( MEZBufferManager& aBufferManager )
    {
    delete iCompressor;
    iCompressor = NULL;
    //
    if  (iDecompressor == NULL)
        {
        iDecompressor = CEZDecompressor::NewL( aBufferManager );
        }
    else
        {
        iDecompressor->ResetL(aBufferManager);
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::SetObserver()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::SetObserver( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG(iObserverStatus == NULL, User::Invariant());
    iObserverStatus = &aStatus;
    *iObserverStatus = KRequestPending;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CompleteSelf()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::CompleteSelf( TInt aCompletionCode )
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aCompletionCode);
    }



// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CompleteObserver(()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::CompleteObserver( TInt aCompletionCode )
    {
    __ASSERT_ALWAYS(iObserverStatus != NULL, User::Invariant());
    User::RequestComplete(iObserverStatus, aCompletionCode);
    
    // Implicitly, we must be finished
    SetOperation(EOperationIdle);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::SetOperation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::SetOperation( TOperation aOperation )
    {
    iOperation = aOperation;
    }


#ifdef RD_FILE_MANAGER_BACKUP
// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::CalculateCrc()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::CalculateCrc( const TAny* aPtr,TInt aLength )
    {
    if(iCrcCalcActivated)
        {
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        TUint32 startTime = User::NTickCount();
#endif
        Mem::Crc32(iCrc, aPtr, aLength);
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
        iTotalTickCount += (User::NTickCount() - startTime);
        //__LOG5("CMMCScBkupArchiveDataManager::CalculateCrc() - crc %u, addr 0x%08x, len %d, offset %d, ticks in ms %u", 
        //    iCrc, aPtr, aLength, CurrentOffset(), iTotalTickCount);
#endif
        }
    }
#endif
    
    
// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::RunL()
    {
    User::LeaveIfError(iStatus.Int());
    TBool moreToDo = EFalse;
    //
    switch(iOperation)
        {
    case EOperationIdle:
        ASSERT( EFalse );
        break;
    case EOperationCompressing:
        __LOG("CMMCScBkupArchiveDataManager::RunL() - START - EOperationCompressing");
        moreToDo = iCompressor->DeflateL();
        break;
    case EOperationDecompressing:
        __LOG("CMMCScBkupArchiveDataManager::RunL() - START - EOperationDecompressing");
        moreToDo = iDecompressor->InflateL();
        break;
        }
    //
    if  (moreToDo)
        {
        CompleteSelf();
        }
    else
        {
        // Update current offset & length info based upon buffer manager indications
        const TMMCScBkupArchiveVector& finalInfo = iBufferManager->Info();
        iCurrentVectorInfo = finalInfo;

        // Ensure we set the state back to idle, ready for next operation...
        iOperation = EOperationIdle;
        
        __LOG2("CMMCScBkupArchiveDataManager::RunL() - operation complete - offset: %8d, length: %8d", iCurrentVectorInfo.Offset(), iCurrentVectorInfo.Length());
        CompleteObserver();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveDataManager::DoCancel()
    {
    __ASSERT_DEBUG(iOperation != EOperationIdle, User::Invariant());
    //
    CompleteObserver(KErrCancel);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveDataManager::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupArchiveDataManager::RunError( TInt aError )
    {
    __LOG1("CMMCScBkupArchiveDataManager::RunError() - START - aError: %d", aError);
    CompleteObserver(aError);
    //
    return KErrNone;
    }

