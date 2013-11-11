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
* Description: TMMCScBkupArchiveVector implementation
*
*
*/

#include "TMMCScBkupArchiveVector.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// TMMCScBkupArchiveVector::ExternalizedSize()
// 
// 
// ---------------------------------------------------------------------------

TInt TMMCScBkupArchiveVector::ExternalizedSize()
    {
    return 20; // 4 bytes for offset, 4 bytes for length, 4 bytes stream version, 4 bytes spare1, 4 bytes spare2
    }


// ---------------------------------------------------------------------------
// TMMCScBkupArchiveVector::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------

void TMMCScBkupArchiveVector::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( Offset() );
    aStream.WriteInt32L( Length() );
    aStream.WriteInt32L( 0 ); // Spare1
    aStream.WriteInt32L( 0 ); // Spare2
    }

// ---------------------------------------------------------------------------
// TMMCScBkupArchiveVector::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------

void TMMCScBkupArchiveVector::InternalizeL(RReadStream& aStream)
    {
    aStream.ReadInt32L(); // stream format version
    iOffset = aStream.ReadInt32L();
    iLength = aStream.ReadInt32L();
    aStream.ReadInt32L(); // Spare1
    aStream.ReadInt32L(); // Spare2
    //
    if  (iOffset < 0 || iLength < 0)
        {
        User::Leave(KErrCorrupt);
        }
    }



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// TMMCScBkupArchiveDriveAndVector::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupArchiveDriveAndVector::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    aStream.ReadInt32L(); // spare3
    //
    iDrive = static_cast<TDriveNumber>( aStream.ReadUint8L() );
    aStream >> iVector;
    }


// ---------------------------------------------------------------------------
// TMMCScBkupArchiveDriveAndVector::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupArchiveDriveAndVector::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    aStream.WriteInt32L( 0 ); // spare3
    //
    aStream.WriteUint8L( iDrive );
    aStream << iVector;
    }


