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
* Description: TMMCScBkupDriveAndSize implementation
*
*
*/

#include "TMMCScBkupDriveAndSize.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// TMMCScBkupDriveAndSize::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupDriveAndSize::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    //
    iDrive = static_cast<TDriveNumber>( aStream.ReadUint8L() );

    iSize = aStream.ReadInt32L();
    // If 64-bit handling will be supported in the future.
    // That will mean in practice a break in archive file format.
    /*
    if ( format == EStreamFormatVersion1 )
        {
        iSize = aStream.ReadInt32L();
        }
    else
        {
        iSize = aStream.ReadReal64L();
        }
    */
    }


// ---------------------------------------------------------------------------
// TMMCScBkupDriveAndSize::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void TMMCScBkupDriveAndSize::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    //
    aStream.WriteUint8L( iDrive );
    aStream.WriteInt32L( iSize );
    //aStream.WriteReal64L( iSize ); // If 64-bit handling will be supported in the future.
                                     // That will mean in practice a break in archive file format.
    }


