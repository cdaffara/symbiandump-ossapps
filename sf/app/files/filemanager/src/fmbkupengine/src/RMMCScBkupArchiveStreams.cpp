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
* Description: RMMCScBkupArchiveReadStream implementation
*
*
*/

#include "RMMCScBkupArchiveStreams.h"

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupArchiveDataManager.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::RMMCScBkupArchiveBuf()
// 
// 
// ---------------------------------------------------------------------------
RMMCScBkupArchiveBuf::RMMCScBkupArchiveBuf()
:   iADM(NULL)
    {
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::AttachL()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveBuf::AttachL( CMMCScBkupArchiveDataManager& aADM, RFile64& aFile, TInt aPos, TBool aForWrite )
    {
    //__LOG3("RMMCScBkupArchiveBuf::AttachL() - START - aFile: 0x%08x, aPos: %d, aForWrite: %d", aFile.SubSessionHandle(), aPos, aForWrite);

    // Prevent RFileBuf from resetting our file handle
    RFile64 file(aFile);
    RFileBuf::Attach(file, aPos);

    // We save the attach pos so that we can update the ADM with the final
    // length of data that was read or written...
    iInfo.Reset();
    iInfo.SetOffset(aPos);

    iWriteMode = aForWrite;
    iADM = &aADM;

    //__LOG("RMMCScBkupArchiveBuf::AttachL() - END");
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::Close()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C void RMMCScBkupArchiveBuf::Close()
    {
    //__LOG1("RMMCScBkupArchiveBuf::Close() - START - File: 0x%08x", File().SubSessionHandle());

    TRAP_IGNORE(DoSynchL());
    Detach();
    Reset();

    //__LOG1("RMMCScBkupArchiveBuf::Close() - END - File: 0x%08x", File().SubSessionHandle());
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::IsOpen()
// 
// 
// ---------------------------------------------------------------------------
TBool RMMCScBkupArchiveBuf::IsOpen() const
    {
    return (File().SubSessionHandle() != KNullHandle);
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::DoRelease()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveBuf::DoRelease()
    {
    //__LOG1("RMMCScBkupArchiveBuf::DoRelease() - START - File: 0x%08x", File().SubSessionHandle());

    TRAP_IGNORE( UpdateInfoL() );
    Detach();
    RFileBuf::DoRelease();

    //__LOG1("RMMCScBkupArchiveBuf::DoRelease() - END - File: 0x%08x", File().SubSessionHandle());
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::DoSynchL()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveBuf::DoSynchL()
    {
    //__LOG1("RMMCScBkupArchiveBuf::DoSynchL() - START - File: 0x%08x", File().SubSessionHandle());

    RFileBuf::DoSynchL();
    UpdateInfoL();

    //__LOG1("RMMCScBkupArchiveBuf::DoSynchL() - END - File: 0x%08x", File().SubSessionHandle());
    }


#ifdef RD_FILE_MANAGER_BACKUP
// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::DoWriteL()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveBuf::DoWriteL(const TAny* aPtr,TInt aLength)
    {
    iADM->CalculateCrc(aPtr, aLength);
    RFileBuf::DoWriteL(aPtr, aLength);
    }
#endif


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveBuf::UpdateInfoL()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveBuf::UpdateInfoL()
    {
    //__LOG1("RMMCScBkupArchiveBuf::UpdateInfoL() - START - File: 0x%08x", File().SubSessionHandle());

    // Update our stats
    if  ( iADM && File().SubSessionHandle() != KNullHandle )
        {
        TStreamPos pos;
        if  ( iWriteMode )
            {
            pos = TellL( MStreamBuf::EWrite );
            //__LOG("RMMCScBkupArchiveBuf::UpdateInfoL() - write mode...");
            }
        else
            {
            pos = TellL( MStreamBuf::ERead );
            //__LOG("RMMCScBkupArchiveBuf::UpdateInfoL() - read mode...");
            }
        //
        const TInt currentOffset = pos.Offset();
        const TInt length = currentOffset - iInfo.Offset();

        //__LOG1("RMMCScBkupArchiveBuf::UpdateInfoL() - currentOffset: %8d", currentOffset);
        //__LOG1("RMMCScBkupArchiveBuf::UpdateInfoL() - length: %8d", length);
        iInfo.SetLength( length );

        // Inform ADM of final info
        iADM->SetCurrentVector( iInfo );
        }

    //__LOG1("RMMCScBkupArchiveBuf::UpdateInfoL() - END - File: 0x%08x", File().SubSessionHandle());
    }











// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// RMMCScBkupArchiveReadStream::OpenLC()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveReadStream::OpenLC( CMMCScBkupArchiveDataManager& aADM, RFile64& aArchiveFile, TInt aPos )
    {
    CleanupClosePushL( *this );
    //
    iSource.AttachL( aADM, aArchiveFile, aPos, EFalse );
    RReadStream::Attach( &iSource );
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveReadStream::IsOpen()
// 
// 
// ---------------------------------------------------------------------------
TBool RMMCScBkupArchiveReadStream::IsOpen() const
    {
    return iSource.IsOpen();
    }












// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// RMMCScBkupArchiveWriteStream::OpenLC()
// 
// 
// ---------------------------------------------------------------------------
void RMMCScBkupArchiveWriteStream::OpenLC( CMMCScBkupArchiveDataManager& aADM, RFile64& aArchiveFile, TInt aPos )
    {
    CleanupClosePushL( *this );
    //
    iSink.AttachL( aADM, aArchiveFile, aPos, ETrue );
    RWriteStream::Attach( &iSink );
    }


// ---------------------------------------------------------------------------
// RMMCScBkupArchiveWriteStream::IsOpen()
// 
// 
// ---------------------------------------------------------------------------
TBool RMMCScBkupArchiveWriteStream::IsOpen() const
    {
    return iSink.IsOpen();
    }






