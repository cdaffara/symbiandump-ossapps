/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Represents one media group (e.g. EMsgMediaImage), and
*                contains a list of the MIME types that are of this media type
*                (read from the resource file). Wildcards may be used in the
*                resource, e.g. "image*".
*
*/




// INCLUDE FILES
#include <barsread.h>
#include <bautils.h>
#include <apgcli.h>
#include "MsgMediaTypeGroup.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMediaTypeGroup::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMsgMediaTypeGroup::ConstructL( TResourceReader& aReader )
    {
    // Read this media group's id
    iMediaType = TMsgMediaType( aReader.ReadInt32() );
    
    CDesCArrayFlat* tempArray = aReader.ReadDesCArrayL();
    CleanupStack::PushL( tempArray );
    TInt count = tempArray->MdcaCount();

    iMimeTypeList = new( ELeave ) CDesC8ArrayFlat( Max( count, 1 ) );
    for ( TInt i = 0; i < count; i++ )
        {   
        TPtrC16 mime16 = tempArray->MdcaPoint(i);
        // Convert to 8 bit
        HBufC8* mime8 = HBufC8::NewLC( mime16.Length() );
        mime8->Des().Copy( mime16 );
        iMimeTypeList->AppendL( *mime8 );
        CleanupStack::PopAndDestroy( mime8 );
        }

    CleanupStack::PopAndDestroy( tempArray );
    }

// -----------------------------------------------------------------------------
// CMGXExtensionRecognizer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMediaTypeGroup* CMsgMediaTypeGroup::NewLC( TResourceReader& aReader )
    {
    CMsgMediaTypeGroup* self = new( ELeave ) CMsgMediaTypeGroup;

    CleanupStack::PushL( self );
    self->ConstructL( aReader );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMsgMediaTypeGroup::~CMsgMediaTypeGroup()
    {
    delete iMimeTypeList;
    }

// -----------------------------------------------------------------------------
// CMsgMediaTypeGroup::MediaType
// -----------------------------------------------------------------------------
//
TMsgMediaType CMsgMediaTypeGroup::MediaType() const
    {
    return iMediaType;
    }

// -----------------------------------------------------------------------------
// CMsgMediaTypeGroup::HasMimeType
// -----------------------------------------------------------------------------
//
TBool CMsgMediaTypeGroup::HasMimeType( const TDesC8& aMimeType ) const
    {
    TInt mimeTypeCount( iMimeTypeList->Count() );

    // Comparing is done with MatchF to support wildcards (e.g. "image*")
    for ( TInt i( 0 ); i < mimeTypeCount; i++ )
        {
        if ( aMimeType.MatchF( iMimeTypeList->MdcaPoint( i ) ) != KErrNotFound )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//  End of File
