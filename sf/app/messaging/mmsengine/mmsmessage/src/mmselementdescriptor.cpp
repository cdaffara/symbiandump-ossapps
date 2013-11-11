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
* Description:   M-Mms-Element-Descriptor header
*
*/




// INCLUDE FILES
#include <msvstore.h>

#include    "mmselementdescriptor.h"

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
// CMmsElementDescriptor::CMmsElementDescriptor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsElementDescriptor::CMmsElementDescriptor()
    {
    // everything is initailized to 0 by CBase
    }
    
// Destructor
CMmsElementDescriptor::~CMmsElementDescriptor()
    {
    delete iContentReference;
    delete iContentType;
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::Externalize
// -----------------------------------------------------------------------------
//
void CMmsElementDescriptor::ExternalizeL( RMsvWriteStream& aStream  ) const
    {
    TInt32 length = 0;
    length = ContentReference().Length(); 
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream << ContentReference();
        }

    length = 0;
    length = ContentType().Length();
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream << ContentType();
        }
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::InternalizeL
// -----------------------------------------------------------------------------
//
void CMmsElementDescriptor::InternalizeL( RMsvReadStream& aStream  )
    {
    delete iContentReference;
    iContentReference = NULL;
    delete iContentType;
    iContentType = NULL;
    TInt32 length = 0;
    length = aStream.ReadInt32L();

    if ( length > 0 )
        {
        iContentReference = HBufC::NewL( aStream, length );
        }

    length = aStream.ReadInt32L();
    if ( length > 0 )
        {
        iContentType = HBufC8::NewL( aStream, length );
        }
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::Size
// -----------------------------------------------------------------------------
//
TInt CMmsElementDescriptor::Size() const
    {
    TInt size = 0;
    size += sizeof( TInt32 );
    // use size to net number of bytes, not characters
    size += ContentReference().Size();
    size += sizeof( TInt32 );
    size += ContentType().Size();
    return size;
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::ContentReference
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMmsElementDescriptor::ContentReference() const
    {
    return iContentReference ? TPtrC( *iContentReference ) : TPtrC();     
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::SetContentReferenceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsElementDescriptor::SetContentReferenceL( const TDesC& aContentReference )
    {
    delete iContentReference;
    iContentReference = NULL;
    iContentReference = aContentReference.AllocL();
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::ContentType
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CMmsElementDescriptor::ContentType() const
    {
    return iContentType ? TPtrC8( *iContentType ) : TPtrC8();     
    }

// -----------------------------------------------------------------------------
// CMmsElementDescriptor::SetContentTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsElementDescriptor::SetContentTypeL( const TDesC8& aContentType )
    {
    delete iContentType;
    iContentType = NULL;
    iContentType = aContentType.AllocL();
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
