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
* Description: CMMCScBkupBufferManagerBase implementation
*
*
*/

#include "CMMCScBkupBufferManagers.h"

// System includes
#include <ezstream.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataStrategies.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "MMMCScBkupProgressObserver.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerBase::CMMCScBkupBufferManagerBase()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerBase::CMMCScBkupBufferManagerBase(MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI, TType aType )
:   iType( aType ), iBufferPointer( NULL, 0, 0 ), iProgressManager( aProgressManager ), iADI( aADI )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerBase::~CMMCScBkupBufferManagerBase()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerBase::~CMMCScBkupBufferManagerBase()
    {
    delete iBuffer;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerBase::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerBase::ConstructL(TInt aBufferSize)
    {
    HBufC8* buffer = HBufC8::NewL(aBufferSize);
    delete iBuffer;
    iBuffer = buffer;
    iBufferPointer.Set(iBuffer->Des());
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerBase::NewByTypeL()
// 
// Factory
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerBase* CMMCScBkupBufferManagerBase::NewByTypeL(TType aType, MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI, TAny* aParam1, TAny* aParam2)
    {
    CMMCScBkupBufferManagerBase* ret = NULL;
    //
    switch(aType)
        {
    case ETypeDescriptorToArchive:
        ret = CMMCScBkupBufferManagerDescriptorToArchive::NewL( aProgressManager, aADI );
        break;
    case ETypeFileToArchive:
        ret = CMMCScBkupBufferManagerFileToArchive::NewL( aProgressManager, aADI );
        break;
    case ETypeArchiveToDescriptor:
        ret = CMMCScBkupBufferManagerArchiveToDescriptor::NewL( aProgressManager, aADI );
        break;
    case ETypeArchiveToFile:
        ret = CMMCScBkupBufferManagerArchiveToFile::NewL( aProgressManager, aADI );
        break;
    default:
        User::Leave(KErrNotSupported);
        break;
        }
    //
    CleanupStack::PushL(ret);
    ret->InitializeL(aParam1, aParam2);
    CleanupStack::Pop(ret);
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerBase::WriteToArchiveAndUpdateStatsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerBase::WriteToArchiveAndUpdateStatsL( const TDesC8& aData )
    {
    // Write the data we currently have to file. This reset
    const TMMCScBkupArchiveVector& writtenInfo = ADI().ADIWriteL( aData );

    // Update our stats. We combined what we've already written with
    // the amount of data we just wrote.
    if  ( Info().Offset() == 0 )
        {
        // Possibly never updated the offset before...
        Info().SetOffset( writtenInfo.Offset() );
        }
    //
    const TInt existingLength = Info().Length();
    Info().SetLength( existingLength + writtenInfo.Length() );
    }


















// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::CMMCScBkupBufferManagerDescriptorToArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerDescriptorToArchive::CMMCScBkupBufferManagerDescriptorToArchive( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
:   CMMCScBkupBufferManagerBase( aProgressManager, aADI, ETypeDescriptorToArchive ), iSource( KNullDesC8 )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::CMMCScBkupBufferManagerDescriptorToArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerDescriptorToArchive* CMMCScBkupBufferManagerDescriptorToArchive::NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
    {
    CMMCScBkupBufferManagerDescriptorToArchive* self = new(ELeave) CMMCScBkupBufferManagerDescriptorToArchive( aProgressManager, aADI );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerDescriptorToArchive::InitializeL(TAny* aParam1, TAny* /*aParam2*/)
    {
    const TDesC8* pData = reinterpret_cast<const TDesC8*>(aParam1);
    iSource.Set(*pData);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerDescriptorToArchive::InitializeL( CEZZStream& aZStream )
    {
    // We use the source object as the input buffer. Therefore the
    // dynamic buffer is used as an intermediate storage depot for compressed
    // data before it is flushed to disk
    aZStream.SetInput( iSource );
    aZStream.SetOutput( WriteBuffer() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::NeedInputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerDescriptorToArchive::NeedInputL(CEZZStream& /*aZStream*/)
    {
    // Nothing to do here - we have provided all our input in one go
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::NeedOutputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerDescriptorToArchive::NeedOutputL( CEZZStream& aZStream )
    {
    const TPtrC8 output(aZStream.OutputDescriptor());
    WriteToArchiveAndUpdateStatsL( output );

    // Reset ready for more writing
    WriteBuffer().Zero();
    aZStream.SetOutput( WriteBuffer() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerDescriptorToArchive::FinalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerDescriptorToArchive::FinalizeL( CEZZStream& aZStream )
    {
    // Write any remaining data...
    const TPtrC8 output(aZStream.OutputDescriptor());
    WriteToArchiveAndUpdateStatsL( output );
    }





























// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::CMMCScBkupBufferManagerFileToArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerFileToArchive::CMMCScBkupBufferManagerFileToArchive( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
:   CMMCScBkupBufferManagerBase( aProgressManager, aADI, ETypeDescriptorToArchive ), iOutputPointer( NULL, 0, 0 )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::~CMMCScBkupBufferManagerFileToArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerFileToArchive::~CMMCScBkupBufferManagerFileToArchive()
    {
    delete iReadStrategy;
    delete iOutput;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::ConstructL( const TDesC& aFileName )
    {
    CMMCScBkupBufferManagerBase::ConstructL();
    //
    CMMCScBkupDataStrategy* strategy = CMMCScBkupDataStrategy::ReadStrategyLC(aFileName, ADI().ADIFsSession());
    delete iReadStrategy;
    iReadStrategy = strategy;
    CleanupStack::Pop(strategy);
    //
    HBufC8* output = HBufC8::NewL(KScBkupDefaultBufferSize);
    delete iOutput;
    iOutput = output;
    iOutputPointer.Set(iOutput->Des());
    //
    iFileName.Set(aFileName);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::CMMCScBkupBufferManagerFileToArchive()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerFileToArchive* CMMCScBkupBufferManagerFileToArchive::NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
    {
    CMMCScBkupBufferManagerFileToArchive* self = new(ELeave) CMMCScBkupBufferManagerFileToArchive( aProgressManager, aADI );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::InitializeL(TAny* aParam1, TAny* /*aParam2*/)
    {
    const TDesC* pFileName = reinterpret_cast<const TDesC*>(aParam1);
    const TPtrC fileName(*pFileName);
    ConstructL(fileName);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::InitializeL( CEZZStream& aZStream )
    {
    // Seed input buffer
    TPtr8 pWriteBuffer( WriteBuffer() );
    iReadStrategy->Read( pWriteBuffer );
    aZStream.SetInput( pWriteBuffer );

    // Progress during a backup is calculated based upon the amount of the
    // source file that has been read.
    ProgressManager().MMCScBkupHandleProgress( pWriteBuffer.Length() );

    // Make ready for output
    aZStream.SetOutput( iOutputPointer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::NeedInputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::NeedInputL( CEZZStream& aZStream )
    {
    // Read some data from the file
    TPtr8 pWriteBuffer( WriteBuffer() );
    iReadStrategy->Read( pWriteBuffer );

    // Progress during a backup is calculated based upon the amount of the
    // source file that has been read.
    ProgressManager().MMCScBkupHandleProgress( pWriteBuffer.Length() );

    aZStream.SetInput( pWriteBuffer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::NeedOutputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::NeedOutputL( CEZZStream& aZStream )
    {
    const TPtrC8 output(aZStream.OutputDescriptor());
    WriteToArchiveAndUpdateStatsL( output );

    // Reset ready for more writing
    iOutputPointer.Zero();
    aZStream.SetOutput( iOutputPointer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerFileToArchive::FinalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerFileToArchive::FinalizeL( CEZZStream& aZStream )
    {
    const TPtrC8 output(aZStream.OutputDescriptor());
    
#if defined(__MMCSCBKUPLOGGING_ENABLED__)
    const TInt compressedSize = output.Size() + Info().Length();
    TEntry entry;
    const TInt error = ADI().ADIFsSession().Entry(iFileName, entry);
    if  (error == KErrNone)
        {
        const TReal ratio = (TReal) entry.iSize / (TReal) compressedSize;
        __LOG4("CMMCScBkupBufferManagerFileToArchive::FinalizeL() - compressing %S - original: %d, compressed: %d, ratio: %f", &iFileName, entry.iSize, compressedSize, ratio);
        if  (compressedSize > entry.iSize)
            {
            // We've made this file bigger
            __LOG3("CMMCScBkupBufferManagerFileToArchive::FinalizeL() - compressing %S had negative effect - originalSize: %d vs compressed: %d", &iFileName, entry.iSize, compressedSize);
            }
        }
#endif
    
    WriteToArchiveAndUpdateStatsL( output );
    }

























































// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::CMMCScBkupBufferManagerArchiveToDescriptor()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToDescriptor::CMMCScBkupBufferManagerArchiveToDescriptor( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
:   CMMCScBkupBufferManagerBase( aProgressManager, aADI, ETypeArchiveToDescriptor ), iOutputPointer( NULL, 0, 0 )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::~CMMCScBkupBufferManagerArchiveToDescriptor()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToDescriptor::~CMMCScBkupBufferManagerArchiveToDescriptor()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::CMMCScBkupBufferManagerArchiveToDescriptor()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToDescriptor* CMMCScBkupBufferManagerArchiveToDescriptor::NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
    {
    CMMCScBkupBufferManagerArchiveToDescriptor* self = new(ELeave) CMMCScBkupBufferManagerArchiveToDescriptor( aProgressManager, aADI );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::InitializeL( TAny* aParam1, TAny* aParam2 )
    {
    // Setup sink
    TDes8* pDataSink = (TDes8*) aParam1;
    iOutputPointer.Set( const_cast<TUint8*>( pDataSink->Ptr() ), pDataSink->Length(), pDataSink->MaxLength() );

    // Setup max read info
    const TMMCScBkupArchiveVector* readInfo = (const TMMCScBkupArchiveVector*) aParam2;
    Info() = *readInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::InitializeL( CEZZStream& aZStream )
    {
    // Try to read some data
    TryToReadMoreSourceDataL();

    // We use the source object as the input buffer. Therefore the
    // dynamic buffer is used as an intermediate storage depot for compressed
    // data before it is flushed to disk
    aZStream.SetInput( DataView() );
    aZStream.SetOutput( iOutputPointer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::NeedInputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::NeedInputL(CEZZStream& aZStream)
    {
    // Try to read some more data
    TryToReadMoreSourceDataL();
    aZStream.SetInput( DataView() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::NeedOutputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::NeedOutputL( CEZZStream& /*aZStream*/ )
    {
    // Nothing to do here, we've got no more space if the sink is full
    ASSERT( EFalse );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::FinalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::FinalizeL( CEZZStream& /*aZStream*/ )
    {
    // Nothing to do here, we've writing straight to the client's descriptor
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToDescriptor::TryToReadMoreSourceDataL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToDescriptor::TryToReadMoreSourceDataL()
    {
    if  ( Info().Length() > 0 )
        {
        // Try to read some source data, but don't read beyond the limits
        const TMMCScBkupArchiveVector& readInfoResult = ADI().ADIReadL( WriteBuffer(), Info() );

        // Update remaining read info so that we know where from and how much data
        // to read next time around
        const TInt newStartingOffset = readInfoResult.Offset() + readInfoResult.Length();
        const TInt remainingToBeRead = Info().Length() - readInfoResult.Length();
        //
        Info().SetOffset( newStartingOffset );
        Info().SetLength( remainingToBeRead );
        //
        __LOG1("CMMCScBkupBufferManagerArchiveToDescriptor::TryToReadMoreSourceDataL() - read: %d bytes from archive", readInfoResult.Length());
        //
        ProgressManager().MMCScBkupHandleProgress( readInfoResult.Length() );
        }
    }

















































// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::CMMCScBkupBufferManagerArchiveToFile()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToFile::CMMCScBkupBufferManagerArchiveToFile( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
:   CMMCScBkupBufferManagerBase( aProgressManager, aADI, ETypeArchiveToFile ), iOutputPointer( NULL, 0, 0 )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::~CMMCScBkupBufferManagerArchiveToFile()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToFile::~CMMCScBkupBufferManagerArchiveToFile()
    {
    delete iWriteStrategy;
    delete iOutput;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::ConstructL( const TDesC& aFileName )
    {
    CMMCScBkupBufferManagerBase::ConstructL();

    // Create a write strategy that ultimately writes to a temp file in our
    // private data cage. When the file has been successfully extracted,
    // we then replace the original. -> see FinalizeL()
    CMMCScBkupDataStrategy* strategy = CMMCScBkupDataStrategy::WriteStrategyLC( aFileName, ADI().ADIFsSession(), KMMCScBkupUseTempFile );
    delete iWriteStrategy;
    iWriteStrategy = strategy;
    CleanupStack::Pop( strategy );
    //
    HBufC8* output = HBufC8::NewL(KScBkupDefaultBufferSize);
    delete iOutput;
    iOutput = output;
    iOutputPointer.Set(iOutput->Des());
    //
    iFileName.Set(aFileName);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::CMMCScBkupBufferManagerArchiveToFile()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupBufferManagerArchiveToFile* CMMCScBkupBufferManagerArchiveToFile::NewL( MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupArchiveDataInterface& aADI )
    {
    CMMCScBkupBufferManagerArchiveToFile* self = new(ELeave) CMMCScBkupBufferManagerArchiveToFile( aProgressManager, aADI );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::InitializeL(TAny* aParam1, TAny* aParam2)
    {
    // Param 1 is the destination file name
    const TDesC* pFileName = reinterpret_cast<const TDesC*>(aParam1);
    const TPtrC fileName(*pFileName);
    ConstructL(fileName);

    // Param 2 defines where we should read archive data from (and how much we should read)
    const TMMCScBkupArchiveVector* readInfo = (const TMMCScBkupArchiveVector*) aParam2;
    Info() = *readInfo;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::InitializeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::InitializeL( CEZZStream& aZStream )
    {
    // Try to read some data
    TryToReadMoreSourceDataL();

    // Input comes from the base class buffer. Its effectively the 
    // data from the archive.
    aZStream.SetInput( WriteBuffer() );

    // Output goes to our secondary buffer - we'll eventually
    // write this to the destination file.
    aZStream.SetOutput( iOutputPointer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::NeedInputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::NeedInputL( CEZZStream& aZStream )
    {
    // Try to read some more data
    TryToReadMoreSourceDataL();
    aZStream.SetInput( DataView() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::NeedOutputL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::NeedOutputL( CEZZStream& aZStream )
    {
    // Flush output to oure write strategy
    const TPtrC8 output(aZStream.OutputDescriptor());
    const TInt error = iWriteStrategy->Write( output );
    User::LeaveIfError( error );

    // Reset ready for more writing
    iOutputPointer.Zero();
    aZStream.SetOutput( iOutputPointer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::FinalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::FinalizeL( CEZZStream& aZStream )
    {
    const TPtrC8 output(aZStream.OutputDescriptor());
    TInt error = iWriteStrategy->Write( output );
    User::LeaveIfError( error );
    
    // No errors writing to temp file, try to finalise write strategy - 
    // essentially this replaces the original file.
    error = iWriteStrategy->Finalize();
    User::LeaveIfError( error );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupBufferManagerArchiveToFile::TryToReadMoreSourceDataL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupBufferManagerArchiveToFile::TryToReadMoreSourceDataL()
    {
    if  ( Info().Length() > 0 )
        {
        // Try to read some source data, but don't read beyond the limits
        const TMMCScBkupArchiveVector& readInfoResult = ADI().ADIReadL( WriteBuffer(), Info() );

        // Update remaining read info so that we know where from and how much data
        // to read next time around
        const TInt newStartingOffset = readInfoResult.Offset() + readInfoResult.Length();
        const TInt remainingToBeRead = Info().Length() - readInfoResult.Length();
        //
        Info().SetOffset( newStartingOffset );
        Info().SetLength( remainingToBeRead );
        //
        __LOG2("CMMCScBkupBufferManagerArchiveToFile::TryToReadMoreSourceDataL() - read: %d bytes from archive when reconstructing: %S", readInfoResult.Length(), &iFileName );
        //
        ProgressManager().MMCScBkupHandleProgress( readInfoResult.Length() );
        }
    }

