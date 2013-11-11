/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  mmsmmboxviewheaders implementation
*
*/




// INCLUDE FILES
#include    <msvstore.h>

#include    "mmsmmboxviewheaders.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsMMBoxViewHeaders::CMmsMMBoxViewHeaders
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMMBoxViewHeaders::CMmsMMBoxViewHeaders()
    {
    }

// Destructor
CMmsMMBoxViewHeaders::~CMmsMMBoxViewHeaders()
    {
    iAttributes.Reset();
    iMmsMMStates.Reset();
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxViewHeaders::Internalize
// -----------------------------------------------------------------------------
//
void CMmsMMBoxViewHeaders::InternalizeL( RMsvReadStream& aStream )
    {
    iAttributes.Reset();
    iMmsMMStates.Reset();

    iMmsStart = aStream.ReadInt32L();
    iMmsLimit = aStream.ReadInt32L();
    iMmsTotals = aStream.ReadInt32L();
    iMmsQuotas = aStream.ReadInt32L();
    iMmsMessageCount = aStream.ReadInt32L();
    iMMBoxTotalNumber = aStream.ReadInt32L();
    iMMBoxTotalSize = aStream.ReadInt32L();
    iMMBoxQuotaNumber = aStream.ReadInt32L();
    iMMBoxQuotaSize = aStream.ReadInt32L();

    TInt32 length = 0;
    TInt i;
    TInt value;
    length = aStream.ReadInt32L();
    for ( i = 0; i < length; i++ )
        {
        value = aStream.ReadInt32L();
        User::LeaveIfError( iAttributes.Append( value ) );
        }
    length = aStream.ReadInt32L();
    for ( i = 0; i < length; i++ )
        {
        value = aStream.ReadInt32L();
        User::LeaveIfError( iMmsMMStates.Append( value ) );
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxViewHeaders::ExternalizeL
// -----------------------------------------------------------------------------
//
void CMmsMMBoxViewHeaders::ExternalizeL( RMsvWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iMmsStart );
    aStream.WriteInt32L( iMmsLimit );
    aStream.WriteInt32L( iMmsTotals );
    aStream.WriteInt32L( iMmsQuotas );
    aStream.WriteInt32L( iMmsMessageCount );
    aStream.WriteInt32L( iMMBoxTotalNumber );
    aStream.WriteInt32L( iMMBoxTotalSize );
    aStream.WriteInt32L( iMMBoxQuotaNumber );
    aStream.WriteInt32L( iMMBoxQuotaSize );

    // Write Attibutes array only if present
    TInt32 length = iAttributes.Count();
    TInt i;
    aStream.WriteInt32L( length );
    for ( i = 0; i < length; i++ )
        {
        aStream.WriteInt32L( iAttributes[i] );
        }

    // Write state filter array
    length = iMmsMMStates.Count();
    aStream.WriteInt32L( length );
    for ( i = 0; i < length; i++ )
        {
        aStream.WriteInt32L( iMmsMMStates[i] );
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxViewHeaders::Size
// -----------------------------------------------------------------------------
//
TInt CMmsMMBoxViewHeaders::Size() const
    {
    TInt size = 0;
    size += sizeof( iMmsStart );
    size += sizeof( iMmsLimit );
    size += sizeof( iMmsTotals );
    size += sizeof( iMmsQuotas );
    size += sizeof( iMmsMessageCount );
    size += sizeof( iMMBoxTotalNumber );
    size += sizeof( iMMBoxTotalSize );
    size += sizeof( iMMBoxQuotaNumber );
    size += sizeof( iMMBoxQuotaSize );

    size += sizeof( TInt32 );
    TInt i;
    for ( i = 0; i < iAttributes.Count(); i++)
        {
        size += sizeof( TUint );
        }

    size += sizeof( TInt32 );
    for ( i = 0; i < iMmsMMStates.Count(); i++)
        {
        size += sizeof( TInt );
        }

    return size;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
