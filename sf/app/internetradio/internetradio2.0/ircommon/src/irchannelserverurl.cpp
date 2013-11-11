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

#include "irchannelserverurl.h"
#include "irdebug.h"
    
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRChannelServerUrl::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRChannelServerUrl* CIRChannelServerUrl::NewL()
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::NewL - Entering" );
    CIRChannelServerUrl* self = CIRChannelServerUrl::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRChannelServerUrl::NewL - Exiting." );
    return self;
    }
    

// ---------------------------------------------------------------------------
// CIRChannelServerUrl::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRChannelServerUrl* CIRChannelServerUrl::NewLC()
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::NewLC - Entering" );
    CIRChannelServerUrl* self = new( ELeave )CIRChannelServerUrl;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRChannelServerUrl::NewLC - Exiting." );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::SetServerName()
// sets server name 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRChannelServerUrl::SetServerName( const TDesC& aServerName )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::SetServerName - Entering" );
    iServerName = aServerName.Alloc();
    IRLOG_DEBUG( "CIRChannelServerUrl::SetServerName - Exiting." );
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::SetServerUrl()
// sets server url 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRChannelServerUrl::SetServerUrl( const TDesC& aServerUrl )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::SetServerUrl - Entering" );
    iURL = aServerUrl.Alloc();
    IRLOG_DEBUG( "CIRChannelServerUrl::SetServerUrl - Exiting." );
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::SetBitRate()
// sets server bitrate 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRChannelServerUrl::SetBitRate( TInt aBitrate )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::SetBitRate - Entering" );
    iBitrate = aBitrate;
    //if a negative value comes,set the bitrate to 0,so that it is accessable 
    //to all the setting combination
    if ( iBitrate < 0 )
        {
        iBitrate = 0;
        }
    IRLOG_DEBUG( "CIRChannelServerUrl::SetBitRate - Exiting." );
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::GetServerName()
// gets server name 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRChannelServerUrl::GetServerName() const
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::GetServerName" );
    if ( NULL == iServerName )
        {
        CIRChannelServerUrl *tempUrl = const_cast<CIRChannelServerUrl*>( this );
	    tempUrl->iServerName = HBufC::New(0);
        }
    return *iServerName;
    }
    

// ---------------------------------------------------------------------------
// CIRChannelServerUrl::GetServerUrl()
// gets server url 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRChannelServerUrl::GetServerUrl() const
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::GetServerUrl" );
    if ( NULL == iURL )
        {
        CIRChannelServerUrl *tempUrl = const_cast<CIRChannelServerUrl*>( this );
	    tempUrl->iURL = HBufC::New(0);
        }    
    return *iURL;
    }
    
    
// ---------------------------------------------------------------------------
//CIRChannelServerUrl::GetBitRate()
//gets server bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRChannelServerUrl::GetBitRate() const
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::GetBitRate" );
    return iBitrate;
    }
    
    
// ---------------------------------------------------------------------------
//CIRChannelServerUrl::~CIRChannelServerUrl()
//standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRChannelServerUrl::~CIRChannelServerUrl()
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::~CIRChannelServerUrl - Entering" );
    delete iServerName;
	iServerName=NULL;
    delete iURL;
	iURL=NULL;
    IRLOG_DEBUG( "CIRChannelServerUrl::~CIRChannelServerUrl - Exiting." );
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::=()
// standard C++ copy constructor( deep copy ) 
// ---------------------------------------------------------------------------
//    
EXPORT_C CIRChannelServerUrl& CIRChannelServerUrl::operator = ( 
    const CIRChannelServerUrl& churl )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::operator= - Entering" );
    if ( &churl == this )
        {
        return *this;
        }
    
    if ( iServerName )
        {
        delete iServerName;
        iServerName = NULL;
        }
    
    if ( churl.iServerName )
        {
        iServerName = churl.iServerName->Alloc();
        }
    
    if ( iURL )
        {
        delete iURL;
        iURL = NULL;
        }
    
    if ( churl.iBitrate )
        {
        iURL = churl.iURL->Alloc();
        }
    
    iBitrate = churl.iBitrate;
    IRLOG_DEBUG( "CIRChannelServerUrl::operator= - Exiting." );
    return *this;
    }
    
    
// ---------------------------------------------------------------------------
// CIRChannelServerUrl::ExternalizeL()
// function to externalize url data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRChannelServerUrl::ExternalizeL( RWriteStream& aWriteStream )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::ExternalizeL - Entering" );
    TInt len;
    if ( !iServerName )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iServerName->Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << *iServerName;
        }
        
    if ( !iURL )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iURL->Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << *iURL;
        }
        
    aWriteStream.WriteInt32L( iBitrate );
    IRLOG_DEBUG( "CIRChannelServerUrl::ExternalizeL - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRChannelServerUrl::InternalizeL()
// function to internalize url data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRChannelServerUrl::InternalizeL( RReadStream& aReadStream )
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::InternalizeL - Entering" );
    TInt len;
    len = aReadStream.ReadInt32L();
    if ( len != 0 )
        {
        iServerName=HBufC::NewL( aReadStream, len );    
        }

    len = aReadStream.ReadInt32L();
    if ( len != 0 )
        {
        iURL = HBufC::NewL( aReadStream, len );    
        }
    iBitrate = aReadStream.ReadInt32L();
    IRLOG_DEBUG( "CIRChannelServerUrl::InternalizeL - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRChannelServerUrl::ConstructL()
// standard second phase construction
// ---------------------------------------------------------------------------
//    
 void CIRChannelServerUrl::ConstructL() const
    {
    IRLOG_DEBUG( "CIRChannelServerUrl::ConstructL" );
    return;
    }
