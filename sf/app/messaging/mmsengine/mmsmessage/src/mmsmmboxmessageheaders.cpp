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
* Description:   MMBox related headers present in message PDUs
*
*/




// INCLUDE FILES
#include    <msvstore.h>
#include    <badesca.h>

#include    "mmsmmboxmessageheaders.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KMmsContentLocationNumber = 3;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::CMmsMMBoxMessageHeaders
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMMBoxMessageHeaders::CMmsMMBoxMessageHeaders()
    {
    // Everything is initialized to 0 by CBase
    }

// Symbian OS default constructor can leave.
void CMmsMMBoxMessageHeaders::ConstructL()
    {
    iContentLocationArray = new ( ELeave )CDesC8ArrayFlat( KMmsContentLocationNumber );
    }
    
// Two-phased constructor.
CMmsMMBoxMessageHeaders* CMmsMMBoxMessageHeaders::NewL()
    {
    CMmsMMBoxMessageHeaders* self = new ( ELeave ) CMmsMMBoxMessageHeaders;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CMmsMMBoxMessageHeaders::~CMmsMMBoxMessageHeaders()
    {
    delete iContentLocationArray;
    delete iMmsStoreStatusText;
    iKeywordArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::Reset
// -----------------------------------------------------------------------------
//
void CMmsMMBoxMessageHeaders::Reset()
    {
    iContentLocationArray->Reset();
    delete iMmsStoreStatusText;
    iMmsStoreStatusText = NULL;
    iKeywordArray.ResetAndDestroy();
    iMmsStore = 0;
    iMmsMMState = 0;
    iMmsStored = 0;
    iMmsStoreStatus = 0;
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::Internalize
// -----------------------------------------------------------------------------
//
void CMmsMMBoxMessageHeaders::InternalizeL( RMsvReadStream& aStream )
    {
    Reset();
    iMmsStore = aStream.ReadInt32L();
    iMmsMMState = aStream.ReadInt32L();
    iMmsStored = aStream.ReadInt32L();
    iMmsStoreStatus = aStream.ReadInt32L();

    TInt32 length = 0;
    length = aStream.ReadInt32L();
    if ( length > 0 )
        {
        iMmsStoreStatusText = HBufC::NewL( aStream, length );
        }

    TInt i;
    // Read content location array if present
    length = aStream.ReadInt32L();
    TInt stringLength = 0;
    for ( i = 0; i < length; i++ )
        {
        stringLength = aStream.ReadInt32L();
        HBufC8* buf=HBufC8::NewLC( aStream, stringLength );
        iContentLocationArray->AppendL( buf->Des() );
        CleanupStack::PopAndDestroy( buf );
        }

    // Read Keyword array only if present
    length = aStream.ReadInt32L();
    for ( i = 0; i < length; i++ )
        {
        CMmsMMBoxFlags* item = new( ELeave )CMmsMMBoxFlags;
        CleanupStack::PushL( item );
        item->InternalizeL( aStream );
        User::LeaveIfError( iKeywordArray.Append( item ) ); 
        CleanupStack::Pop( item ); // item is in member array now
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::ExternalizeL
// -----------------------------------------------------------------------------
//
void CMmsMMBoxMessageHeaders::ExternalizeL( RMsvWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iMmsStore );
    aStream.WriteInt32L( iMmsMMState );
    aStream.WriteInt32L( iMmsStored );
    aStream.WriteInt32L( iMmsStoreStatus );

    TInt32 length = MmsStoreStatusText().Length();
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream << MmsStoreStatusText();
        }

    // Write the content location array
    TInt i;
    TInt count = iContentLocationArray->Count();
    aStream.WriteInt32L( count );
    for ( i = 0; i < count; i++ )
        {
        length = iContentLocationArray->MdcaPoint(i).Length();
        aStream.WriteInt32L( length );
        aStream << iContentLocationArray->MdcaPoint(i);
        }

    // Write Keyword array only if present
    length = iKeywordArray.Count();
    aStream.WriteInt32L( length );
    for ( i = 0; i < length; i++ )
        {
        iKeywordArray[i]->ExternalizeL( aStream );
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::Size
// -----------------------------------------------------------------------------
//
TInt CMmsMMBoxMessageHeaders::Size() const
    {
    TInt size = 0;
    size += sizeof( iMmsStore );
    size += sizeof( iMmsMMState );
    size += sizeof( iMmsStored );
    size += sizeof( iMmsStoreStatus );
    size += sizeof( TInt32 );
    size += MmsStoreStatusText().Size(); // size, not length to get number of bytes
    size += sizeof( TInt32 );
    TInt i;

    for ( i = 0; i < iContentLocationArray->Count(); i++ )
        {
        size += iContentLocationArray->MdcaPoint(i).Size();
        }

    size += sizeof( TInt32 );
    for ( i = 0; i < iKeywordArray.Count(); i++)
        {
        size += iKeywordArray[i]->Size();
        }

    return size;
    }

// ---------------------------------------------------------
// CMmsMMBoxMessageHeaders::MmsStoreStatusText
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMmsMMBoxMessageHeaders::MmsStoreStatusText() const
    {
    return iMmsStoreStatusText ? TPtrC( *iMmsStoreStatusText ) : TPtrC(); 
    }

// ---------------------------------------------------------
// CMmsMMBoxMessageHeaders::SetMmsStoreStatusTextL
// ---------------------------------------------------------
//
EXPORT_C void CMmsMMBoxMessageHeaders::SetMmsStoreStatusTextL( const TDesC& aText )
    {
    // If the length of the new text is 0, the text is cleared
    delete iMmsStoreStatusText;
    iMmsStoreStatusText = NULL;
    if ( aText.Length() > 0 )
        {
        iMmsStoreStatusText = aText.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::AppendKeywordItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsMMBoxMessageHeaders::AppendKeywordItemL( TInt aToken, const TDesC& aKeyword )
    {
    TInt index = FindItemWithKeyword( aKeyword );
    if ( index == KErrNotFound )
        {
        CMmsMMBoxFlags* item = new( ELeave ) CMmsMMBoxFlags;
        CleanupStack::PushL( item );
        item->SetToken( aToken );
        item->SetKeywordL( aKeyword );
        TInt error = iKeywordArray.Append( item );
        User::LeaveIfError( error ); // should only leave if out of memory
        CleanupStack::Pop( item ); // Item is now in array
        }
    else
        {
        // index is valid as it is what we just found
        iKeywordArray[index]->SetToken( aToken );
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::DeleteKeywordItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsMMBoxMessageHeaders::DeleteKeywordItem( const TDesC& aKeyword )
    {
    TInt index = FindItemWithKeyword( aKeyword );
    if ( index != KErrNotFound )
        {
        CMmsMMBoxFlags* deadItem = iKeywordArray[index];
        iKeywordArray.Remove( index );
        delete deadItem;
        }
    }

// -----------------------------------------------------------------------------
// CMmsMMBoxMessageHeaders::FindItemWithKeyword
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsMMBoxMessageHeaders::FindItemWithKeyword( const TDesC& aKeyword )
    {
    TInt i;
    TInt index = KErrNotFound;
    for ( i = 0; i < iKeywordArray.Count() && index == KErrNotFound; i++ )
        {
        if ( iKeywordArray[i]->Keyword().CompareF( aKeyword ) == 0 )
            {
            index = i;
            }
        }
    return index;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
