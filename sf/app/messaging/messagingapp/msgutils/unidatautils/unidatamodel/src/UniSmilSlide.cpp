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
* Description: 
*       Provides MMS SMIL Slide methods.
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32def.h>
#include <eikenv.h>
#include <msvids.h>
#include <msvstd.h>

#include <MsgMedia.hrh>
#include <MsgMediaInfo.h>

#include "UniModelConst.h"
#include "UniObject.h"    
#include "UniSmilSlide.h"

// ========== LOCAL CONSTANTS AND MACROS ===================

const TUint KMaxObjectPerSlide( 3 );

// ========== MEMBER FUNCTIONS =============================

CUniSmilSlide* CUniSmilSlide::NewLC( TUniLayout aLayout )
    {
    CUniSmilSlide* self = new ( ELeave ) CUniSmilSlide( aLayout );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CUniSmilSlide::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniSmilSlide* CUniSmilSlide::NewL( TUniLayout aLayout )
    {
    CUniSmilSlide* self = NewLC( aLayout );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CUniSmilSlide::CUniSmilSlide
//
// Constructor.
// ---------------------------------------------------------
//
CUniSmilSlide::CUniSmilSlide( TUniLayout aLayout ) :
    iLayout( aLayout )
    {
    }

// ---------------------------------------------------------
// CUniSmilSlide::CUniSmilSlide
//
// Destructor.
// ---------------------------------------------------------
//
 CUniSmilSlide::~CUniSmilSlide()
    {
    //Objects not owned by slide!!
    //->ResetAndDestroy() not called.
    delete iObjectArray;
    }


// ---------------------------------------------------------
// CUniSmilSlide::AddObjectL
//
// Object should be valid for addition at this point ie 
// should be checked that it can be added to slide.
// Takes ownership of the aObject.
// ---------------------------------------------------------
//  
void CUniSmilSlide::AddObjectL( CUniObject* aObject )
    {
    TUniRegion region = EUniRegionUnresolved;
    switch ( aObject->MediaType() )
        {
        case EMsgMediaText:
        case EMsgMediaXhtml:
            {
            region = EUniRegionText;
            }
            break;
        case EMsgMediaAudio:
            {
            region = EUniRegionAudio;
            }
            break;
#ifdef RD_SVGT_IN_MESSAGING
        case EMsgMediaSvg:
#endif
        case EMsgMediaImage:
        case EMsgMediaVideo:
            {
            region = EUniRegionImage;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    AddObjectL( aObject, region );
    }


// ---------------------------------------------------------
// CUniSmilSlide::AddObjectL
//
// Object should be valid for addition at this point ie 
// should be checked that it can be added to slide.
// Takes ownership of the aObject.
// ---------------------------------------------------------
//  
void CUniSmilSlide::AddObjectL( CUniObject* aObject, TUniRegion aRegion )
    {
    for ( TInt i = 0; i < iObjectArray->Count(); ++i)
        {
        if ( iObjectArray->At( i )->Region() == aRegion )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    aObject->SetReferenceCount( aObject->ReferenceCount() + 1 );
    aObject->SetRegion( aRegion );
    if ( aRegion == EUniRegionAudio )
        {
        //always put audio region last...
        iObjectArray->AppendL( aObject );
        }
    else
        {
        //...and other regions first...
        iObjectArray->InsertL( 0, aObject );
        //...and update layout when needed
        UpdateLayoutL(); // does not leave, really.
        }
    }


// ---------------------------------------------------------
// CUniSmilSlide::RemoveObject
//
// 
// ---------------------------------------------------------
//
void CUniSmilSlide::RemoveObject( CUniObject* aObject )
    {
    TInt index( -1 );  // Initialize to impossible index.

    for ( TInt i = 0; i < iObjectArray->Count(); ++i )
        {
        if ( iObjectArray->At( i ) == aObject )
            {
            index = i;
            }
        }
    if ( index != -1 )
        {
        // Found
        iObjectArray->Delete( index );
        aObject->SetReferenceCount( aObject->ReferenceCount() - 1 );
        }
    }

// ---------------------------------------------------------
// CUniSmilSlide::GetObject
//
// Accessor. 
// ---------------------------------------------------------
//
CUniObject* CUniSmilSlide::GetObject( TUniRegion aRegion ) const 
    {
    TInt cnt = iObjectArray->Count();

    for ( TInt i = 0; i < cnt; ++i )
        {
        if ( iObjectArray->At( i )->Region() == aRegion )
            {
            return iObjectArray->At( i );
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CUniSmilSlide::GetObject
//
// Accessor. 
// ---------------------------------------------------------
//
CUniObject* CUniSmilSlide::GetObjectByIndex( TInt aObjNum ) const 
    {
    if ( aObjNum >= 0 &&
        aObjNum < iObjectArray->Count() )
        {
        return iObjectArray->At( aObjNum );
        }
    return NULL;
    }

// ---------------------------------------------------------
// CUniSmilSlide::IsObject
//
//  
// ---------------------------------------------------------
//
TBool CUniSmilSlide::IsObject( CUniObject* aObject ) const
    {
    TInt cnt = iObjectArray->Count();

    for ( TInt i = 0; i < cnt; ++i )
        {
        if ( iObjectArray->At( i ) == aObject )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CUniSmilSlide::ObjectCount
//
// Accessor
// ---------------------------------------------------------
//
TInt CUniSmilSlide::ObjectCount() const
    {
    return iObjectArray->Count();
    }


// ---------------------------------------------------------
// CUniSmilSlide::UpdateLayoutL
//
// 
// ---------------------------------------------------------
// 
void CUniSmilSlide::UpdateLayoutL( TUniLayout aNewLayout )
    {
    // Only need to do something if slide has text & image.
    // If only other or neither -> do nothing.
    iLayout = aNewLayout;
    UpdateLayoutL();
    }

// ---------------------------------------------------------
// CUniSmilSlide::UpdateLayoutL
//
// 
// ---------------------------------------------------------
// 
void CUniSmilSlide::UpdateLayoutL()
    {
    if ( iLayout == EUniImageFirst ||
        iLayout == EUniTextFirst )
        {
        CUniObject* image = GetObject( EUniRegionImage );
        CUniObject* text = GetObject( EUniRegionText );
        if ( image && text )
            {
            TInt imageRef = image->ReferenceCount();
            TInt textRef = text->ReferenceCount();
            RemoveObject( image );
            RemoveObject( text );
            if ( iLayout == EUniImageFirst )
                {
                iObjectArray->InsertL( 0, text );
                iObjectArray->InsertL( 0, image );
                }
            else
                {
                iObjectArray->InsertL( 0, image );
                iObjectArray->InsertL( 0, text );
                }
            image->SetReferenceCount( imageRef );
            text->SetReferenceCount( textRef );
            }
        }
    }

// ---------------------------------------------------------
// CUniSmilSlide::ConstructL
//
// 2nd phase Constructor.
// ---------------------------------------------------------
//
void CUniSmilSlide::ConstructL()
    {
    iObjectArray = new ( ELeave ) CSmilObjectArray( KMaxObjectPerSlide );
    }

// End of file
