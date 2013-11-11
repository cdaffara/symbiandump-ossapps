/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "mmssendingchain.h"

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
// CMmsSendingChain::CMmsSendingChain
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsSendingChain::CMmsSendingChain()
    {
    // everything is initailized to 0 by CBase
    }

    
// Destructor
CMmsSendingChain::~CMmsSendingChain()
    {
    delete iSender;
    }


// -----------------------------------------------------------------------------
// CMmsSendingChain::Compare
// -----------------------------------------------------------------------------
//
TInt CMmsSendingChain::Compare(
    const CMmsSendingChain& aFirst,
    const CMmsSendingChain& aSecond )
    {
    if ( aFirst.iOrder == aSecond.iOrder )
        {
        return 0;
        }
    if ( aFirst.iOrder > aSecond.iOrder )
        {
        return 1;
        }
    else
        {
        return -1;
        }
    }

// -----------------------------------------------------------------------------
// CMmsSendingChain::InternalizeL
// -----------------------------------------------------------------------------
//
void CMmsSendingChain::InternalizeL( RMsvReadStream& aStream )
    {
    aStream >> iOrder;
    aStream >> iDate;
    TInt32 length;
    aStream >> length;
    delete iSender;
    iSender = NULL;
    if ( length > 0 )
        {
        iSender = HBufC::NewL( aStream, length );
        }
    }

// -----------------------------------------------------------------------------
// CMmsSendingChain::ExternalizeL
// -----------------------------------------------------------------------------
//
void CMmsSendingChain::ExternalizeL( RMsvWriteStream& aStream ) const
    {
    aStream << iOrder;
    aStream << iDate;
    TInt32 length = Sender().Length();
    aStream << length;
    if ( length > 0 )
        {
        aStream << Sender();
        }
    }

// -----------------------------------------------------------------------------
// CMmsSendingChain::Sender
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMmsSendingChain::Sender() const
    {
    return iSender ? TPtrC( *iSender ) : TPtrC(); 
    }

// -----------------------------------------------------------------------------
// CMmsSendingChain::SetSenderL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsSendingChain::SetSenderL( const TDesC& aSender )
    {
    HBufC* newAttrib = aSender.AllocL();
    delete iSender;
    iSender = newAttrib;
    }

// -----------------------------------------------------------------------------
// CMmsSendingChain::Size
// -----------------------------------------------------------------------------
//
TInt CMmsSendingChain::Size() const
    {
    TInt size = 0;
    size += sizeof( iOrder );
    size += sizeof( iDate );
    size += sizeof( TInt32 );
    size += Sender().Size();
    return size;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
