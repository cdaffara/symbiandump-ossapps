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
* Description:   Structure to hold MMS encapsulation 1.1 version
*                previoulsy-sent-by and previously-sent-date information  
*
*/




// INCLUDE FILES
#include <msvstore.h>
#include "mmsmmboxflags.h"

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
// CMmsMMBoxFlags::CMmsMMBoxFlags
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMMBoxFlags::CMmsMMBoxFlags()
    {
    // everything is initailized to 0 by CBase
    }

    
// Destructor
CMmsMMBoxFlags::~CMmsMMBoxFlags()
    {
    delete iKeyword;
    }


// -----------------------------------------------------------------------------
// CMmsMMBoxFlags::Internalize
// -----------------------------------------------------------------------------
//
void CMmsMMBoxFlags::InternalizeL( RMsvReadStream& aStream )
    {
    iOperationToken = aStream.ReadInt32L();
    TInt32 length = 0;
    length = aStream.ReadInt32L();
    delete iKeyword;
    iKeyword = NULL;
    if ( length > 0 )
        {
        iKeyword = HBufC::NewL( aStream, length );
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxFlags::ExternalizeL
// -----------------------------------------------------------------------------
//
void CMmsMMBoxFlags::ExternalizeL( RMsvWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iOperationToken );
    TInt32 length = Keyword().Length();
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream << Keyword();
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxFlags::SetKeywordL
// -----------------------------------------------------------------------------
//

void CMmsMMBoxFlags::SetKeywordL( const TDesC& aKeyword )
    {
    delete iKeyword;
    iKeyword = NULL;
    iKeyword = aKeyword.AllocL();
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxFlags::Size
// -----------------------------------------------------------------------------
//
TInt CMmsMMBoxFlags::Size() const
    {
    TInt size = 0;
    size += sizeof( iOperationToken );
    size += sizeof( TInt32 );
    size += Keyword().Size();
    return size;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
