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

#include "irbrowsechannelitems.h"
#include "irdebug.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//    
EXPORT_C CIRBrowseChannelItems* CIRBrowseChannelItems::NewLC()
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::NewLC - Entering" );
    CIRBrowseChannelItems* self = new( ELeave )CIRBrowseChannelItems;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRBrowseChannelItems::NewLC - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRBrowseChannelItems* CIRBrowseChannelItems::NewL()
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::NewL - Entering" );
    CIRBrowseChannelItems* self = CIRBrowseChannelItems::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRBrowseChannelItems::NewL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::ConstructL()
// standard two phased constructor
// ---------------------------------------------------------------------------
//
 void CIRBrowseChannelItems::ConstructL() const
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::~CIRBrowseChannelItems()
// standard C++ destructor
// ---------------------------------------------------------------------------
//    
 CIRBrowseChannelItems::~CIRBrowseChannelItems()
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::~CIRBrowseChannelItems - Entering" );
    delete iChannelName;
    iChannelName = NULL;
    delete iChannelGetOperation;
    iChannelGetOperation = NULL;
    delete iShortDescription;
    iShortDescription = NULL;
    iImgUrl.Close();
    
    delete iBannerUrl;
    iBannerUrl = NULL;
    delete iClickThroughUrl;
    iClickThroughUrl = NULL;

    IRLOG_DEBUG( "CIRBrowseChannelItems::~CIRBrowseChannelItems - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetTextL()
// Set data namely channelname,channelid,description
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetTextL( const TDesC& aChannelName, 
    const TDesC& aChannelId, const TDesC& aDescription )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetTextL - Entering" );
    iChannelName = aChannelName.Alloc();
    iShortDescription = aDescription.Alloc();    
    TLex var( aChannelId );
    var.Val( iChannelID );
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetTextL - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetGetOperationL()
// Set data get methord
// ---------------------------------------------------------------------------
//    
EXPORT_C void CIRBrowseChannelItems::SetGetOperationL( 
    const TDesC& aChannelGetOperation )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetGetOperationL - Entering" );
    iChannelGetOperation = aChannelGetOperation.Alloc();
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetGetOperationL - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetSize()
// Set data size of data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetSize( const TDesC& aSize )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetSize - Entering" );
    TLex var( aSize );
    var.Val( iSize );
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetSize - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetImageUrlL()
// Set the url for the iImgUrl
// ---------------------------------------------------------------------------
//    
EXPORT_C void CIRBrowseChannelItems::SetImageUrlL( const TDesC& aImgUrl )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetImageUrlL - Entering" );
    iImgUrl.CreateL( aImgUrl.Length() );
    iImgUrl.Copy( aImgUrl );
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetImageUrlL - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::ExternalizeL()
// Externalizes data into persistent storage
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::ExternalizeL( RWriteStream& aStream ) const
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::ExternalizeL - Entering" );
    if ( iChannelName )
        {
        aStream.WriteInt32L( iChannelName->Des().MaxLength() );
        aStream << *iChannelName;
        }
    else 
        {
        aStream.WriteInt32L (0 );
        }
    
    if ( ( iShortDescription ) && ( iShortDescription->Des().MaxLength() != 0 ) )
        {    
        TInt a = iShortDescription->Des().MaxLength();
        aStream.WriteInt32L( iShortDescription->Des().MaxLength() );
        aStream << *iShortDescription;
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    
    if ( iChannelGetOperation )
        {
        aStream.WriteInt32L( iChannelGetOperation->Des().MaxLength() );
        aStream << *iChannelGetOperation;
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    
    aStream.WriteInt32L( iBitrate1 );
    aStream.WriteInt32L( iBitrate2 );
    aStream.WriteInt32L( iBitrate3 );
    aStream.WriteUint32L( iChannelID );
    if ( iBannerUrl )
        {
        aStream.WriteInt32L( iBannerUrl->Des().MaxLength() );
        aStream << *iBannerUrl;
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
    
    if ( iClickThroughUrl )    
        {
        aStream.WriteInt32L( iClickThroughUrl->Des().MaxLength() );
        aStream << *iClickThroughUrl;
        }
    else
        {
        aStream.WriteInt32L( 0 );
        }
        
    if ( iImgUrl.Length() != 0 )
        {
        aStream.WriteInt32L( iImgUrl.MaxLength() );
        aStream << iImgUrl;
        }
    else
        {
        aStream.WriteInt32L( 0 );    
        }

    IRLOG_DEBUG( "CIRBrowseChannelItems::ExternalizeL - Exiting." );
    }  


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::InternalizeL()
// internalizes data from persistent storage
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::InternalizeL( RReadStream& aStream )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::InternalizeL - Entering" );
    TInt maxlen;
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iChannelName = HBufC::NewL( aStream,maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iShortDescription = HBufC::NewL( aStream,maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iChannelGetOperation = HBufC::NewL( aStream,maxlen );
        }
    iBitrate1 = aStream.ReadInt32L();
    iBitrate2 = aStream.ReadInt32L();
    iBitrate3 = aStream.ReadInt32L();
    iChannelID = aStream.ReadUint32L();
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iBannerUrl = HBufC::NewL( aStream, maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iClickThroughUrl = HBufC::NewL( aStream, maxlen );
        }
    
    maxlen = aStream.ReadInt32L();
    if ( maxlen != 0 )
        {
        iImgUrl.Close();
        iImgUrl.CreateL( aStream, maxlen );            
        }
    
    IRLOG_DEBUG( "CIRBrowseChannelItems::InternalizeL - Exiting." );
    }  
    


// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetBitrate()
// Set data for 3bitrates
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetBitrate( const TDesC& aBitrate1, 
    const TDesC& aBitrate2, const TDesC& aBitrate3 )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate3 - Entering" );
    TLex var1( aBitrate1 );
    var1.Val( iBitrate1 );
    TLex var2( aBitrate2 );
    var2.Val( iBitrate2 );
    TLex var3( aBitrate3 );
    var3.Val( iBitrate3 );    
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate3 - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetBitrate()
// Set data for 2 bitrates
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetBitrate( const TDesC& aBitrate1, 
    const TDesC& aBitrate2)
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate2 - Entering" );
    TLex var1( aBitrate1 );
    var1.Val( iBitrate1 );
    TLex var2( aBitrate2 );
    var2.Val( iBitrate2 );
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate2 - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetBitrate()
// Set data for 1 bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetBitrate( const TDesC& aBitrate1 )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate1 - Entering" );
    TLex var1( aBitrate1 );
    var1.Val( iBitrate1 );    
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBitrate1 - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetBannerUrl()
// Set data namely BannerUrl
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetBannerUrl( const TDesC& aBannerUrl )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBannerUrl - Entering" );
    iBannerUrl = aBannerUrl.Alloc();
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetBannerUrl - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRBrowseChannelItems::SetBannerUrl()
// Set data namely BannerUrl
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRBrowseChannelItems::SetClickThroughUrl( const TDesC& aClickThroughUrl )
    {
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetClickThroughUrl - Entering" );
    iClickThroughUrl = aClickThroughUrl.Alloc();
    IRLOG_DEBUG( "CIRBrowseChannelItems::SetClickThroughUrl - Exiting." );
    }
