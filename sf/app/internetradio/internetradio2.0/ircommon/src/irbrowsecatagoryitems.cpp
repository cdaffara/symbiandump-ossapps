/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
 *
 */

#include <s32strm.h>

#include "irdebug.h"
#include "irbrowsecatagoryitems.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRBrowseCatagoryItems* CIRBrowseCatagoryItems::NewL()
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::NewL - Entering" );
    CIRBrowseCatagoryItems* self = CIRBrowseCatagoryItems::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::NewL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRBrowseCatagoryItems* CIRBrowseCatagoryItems::NewLC()
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::NewLC - Entering" );
    CIRBrowseCatagoryItems* self = new( ELeave )CIRBrowseCatagoryItems;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::NewLC - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::ConstructL()
// standard second phase constructor
// ---------------------------------------------------------------------------
//
 void CIRBrowseCatagoryItems::ConstructL() const
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::~CIRBrowseCatagoryItems()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
 CIRBrowseCatagoryItems::~CIRBrowseCatagoryItems()
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::~CIRBrowseCatagoryItems - Entering" );
    delete iCatName;
    iCatName = NULL;
    delete iCatId;
    iCatId = NULL;
    delete iCatGetOperation;
    iCatGetOperation = NULL;
    
    delete iCatBannerUrl;
    iCatBannerUrl = NULL;
    
    delete iCatClickThroughUrl;
    iCatClickThroughUrl = NULL;
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::~CIRBrowseCatagoryItems - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::SetTextL()
// sets the cache status informa
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseCatagoryItems::SetTextL( const TDesC& aCatName,
                                               const TDesC& aCatId )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetTextL - Entering" );
    iCatName = aCatName.Alloc();
    iCatId = aCatId.Alloc();    
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetTextL - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::SetSize()
// sets the size of the object iSize,after converting it from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseCatagoryItems::SetSize( const TDesC& aCatSize )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetSize - Entering" );
    TLex var( aCatSize );
    var.Val( iSize );
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetSize - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::SetGetOperation()
// sets params for retriving data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseCatagoryItems::SetGetOperation( 
                                                  const TDesC& aCatGetOperation )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetGetOperation - Entering" );
    iCatGetOperation = aCatGetOperation.Alloc();
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetGetOperation - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::ExternalizeL()
// externalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseCatagoryItems::ExternalizeL( RWriteStream& aStream ) const
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::ExternalizeL - Entering" );
    if ( !iCatGetOperation )
        {
        aStream.WriteInt32L( 0 );
        }
    else
        {
        aStream.WriteInt32L( iCatGetOperation->Des().MaxLength() );
        aStream << *iCatGetOperation;
        }
    
    if ( !iCatName )
        {
        aStream.WriteInt32L( 0 );
        }
    else
        {
        aStream.WriteInt32L( iCatName->Des().MaxLength() );
        aStream << *iCatName;
        }
    
    if ( !iCatId )
        {
        aStream.WriteInt32L( 0 );            
        }
    else
        {
        aStream.WriteInt32L( iCatId->Des().MaxLength() );
        aStream << *iCatId;
        }
    
    if ( iSize>0 )
        {
        aStream.WriteInt32L( iSize );
        }
    else
        {
        aStream.WriteInt32L( 0 );            
        }
    
    if ( !iCatBannerUrl )
        {
        aStream.WriteInt32L( 0 );            
        }
    else
        {
        aStream.WriteInt32L( iCatBannerUrl->Des().MaxLength() );
        aStream << *iCatBannerUrl;
        }
    
    if ( !iCatClickThroughUrl )
        {
        aStream.WriteInt32L( 0 );    
        }
    else
        {
        aStream.WriteInt32L( iCatClickThroughUrl->Des().MaxLength() );
        aStream << *iCatClickThroughUrl;
        }
    
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::ExternalizeL - Exiting." );    
    }  


// ---------------------------------------------------------------------------
// CIRBrowseCatagoryItems::InternalizeL()
// internalizes the data.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseCatagoryItems::InternalizeL( RReadStream& aStream )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::InternalizeL - Entering" );
    TInt maxlen;
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iCatGetOperation = HBufC::NewL( aStream,maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iCatName = HBufC::NewL( aStream,maxlen );            
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iCatId = HBufC::NewL( aStream,maxlen );    
        }
    
    iSize  = aStream.ReadInt32L();
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iCatBannerUrl = HBufC::NewL( aStream, maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iCatClickThroughUrl = HBufC::NewL( aStream, maxlen );    
        }
    
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::InternalizeL - Exiting." );
    }  
    

EXPORT_C void CIRBrowseCatagoryItems::SetBannerUrl( const TDesC& aBannerUrl )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetBannerUrl() - Entering" );
    iCatBannerUrl = aBannerUrl.Alloc();
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetBannerUrl - Exiting" );
    }
    
    
EXPORT_C void CIRBrowseCatagoryItems::SetClickThroughUrl( const TDesC& aClickThroughUrl )
    {
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetClickThroughUrl() - Entering" );
    iCatClickThroughUrl = aClickThroughUrl.Alloc();
    IRLOG_DEBUG( "CIRBrowseCatagoryItems::SetClickThroughUrl() - Exiting" );
    }
