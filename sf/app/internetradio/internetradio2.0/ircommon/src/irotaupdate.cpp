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
#include "irotaupdate.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// Function : NewLC
// Function returns an instance of CIROTAUpdate
// Two phase constructor
//----------------------------------------------------------------------------
//
EXPORT_C CIROTAUpdate* CIROTAUpdate::NewLC()
	{
	IRLOG_DEBUG( "CIROTAUpdate::NewLC - Entering" );
	CIROTAUpdate* self = new( ELeave )CIROTAUpdate;
	CleanupStack::PushL( self );
	self->ConstructL();
	IRLOG_DEBUG( "CIROTAUpdate::NewLC - Exiting." );
	return self;
	}


//----------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIROTAUpdate
// Two phase constructor
//----------------------------------------------------------------------------
//
EXPORT_C CIROTAUpdate* CIROTAUpdate::NewL()
	{
	IRLOG_DEBUG( "CIROTAUpdate::NewL - Entering" );
	CIROTAUpdate* self = CIROTAUpdate::NewLC();
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIROTAUpdate::NewL - Exiting." );
	return self;
	}


//----------------------------------------------------------------------------
//CIROTAUpdate::ConstructL()
//standard second phase constructor
//----------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::ConstructL()
	{
    IRLOG_DEBUG( "CIROTAUpdate::ConstructL" );
	return;
	}


//----------------------------------------------------------------------------
// CIROTAUpdate::~CIROTAUpdate()
// standard C++ destructor
//----------------------------------------------------------------------------
//	
CIROTAUpdate::~CIROTAUpdate()
	{
    IRLOG_DEBUG( "CIROTAUpdate::~CIROTAUpdate - Entering" ); 
    delete iUpdateMethod;
    delete iVersion;
    delete iOTAUrl;
    IRLOG_DEBUG( "CIROTAUpdate::~CIROTAUpdate - Exiting." );
	}


//----------------------------------------------------------------------------
//CIROTAUpdate::SetUpdateMethod()
//Function to set update method
//----------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::SetUpdateMethod( const TDesC& aUpdateMethod )
	{
	IRLOG_DEBUG( "CIROTAUpdate::SetUpdateMethod - Entering" );
	iUpdateMethod = aUpdateMethod.Alloc();
	IRLOG_DEBUG( "CIROTAUpdate::SetUpdateMethod - Exiting." );
	}
	

//----------------------------------------------------------------------------
// CIROTAUpdate::SetVersion()
// Function to set version
//----------------------------------------------------------------------------
//	
EXPORT_C  void CIROTAUpdate::SetVersion( const TDesC& aVersion )
	{          
	IRLOG_DEBUG( "CIROTAUpdate::SetVersion - Entering" );
	iVersion = aVersion.Alloc();
	IRLOG_DEBUG( "CIROTAUpdate::SetVersion - Exiting." );
	}


//----------------------------------------------------------------------------
// CIROTAUpdate::SetUpdateInterval()
// Function to set update interval
//----------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::SetUpdateInterval( const TDesC& aUpdateInterval )
	{
	IRLOG_DEBUG( "CIROTAUpdate::SetUpdateInterval - Entering" );
	TLex var( aUpdateInterval );
	var.Val( iUpdateInterval );
	IRLOG_DEBUG( "CIROTAUpdate::SetUpdateInterval - Exiting." );
	}
	

//----------------------------------------------------------------------------
// CIROTAUpdate::SetOTAUrl()
// Function to set OTA Url
//----------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::SetOTAUrl( const TDesC& aOTAUrl )
  {
    IRLOG_DEBUG( "CIROTAUpdate::SetOTAUrl - Entering" );
	iOTAUrl = aOTAUrl.Alloc();
	IRLOG_DEBUG( "CIROTAUpdate::SetOTAUrl - Exiting." );
  }


// ---------------------------------------------------------------------------
// CIROTAUpdate::ExternalizeL()
// externalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::ExternalizeL( RWriteStream& aStream ) const
	{
	IRLOG_DEBUG( "CIROTAUpdate::ExternalizeL - Entering" );
	aStream.WriteInt32L( iUpdateInterval );
	TInt length = 0;
	if( iVersion )
		{
		length = iVersion->Length();
		aStream.WriteInt32L( length );
		aStream << *iVersion;
		}
	else
		{
		length = 0;
		aStream.WriteInt32L( length );
		}

	if ( iOTAUrl )
		{
		length = iOTAUrl->Length();
		aStream.WriteInt32L( length );
		aStream << *iOTAUrl;
		}
	else
		{
		length = 0;
		aStream.WriteInt32L( length );
		}
		
	if ( iUpdateMethod )
		{
		length = iUpdateMethod->Length();
		aStream.WriteInt32L( length );
		aStream << *iUpdateMethod;
		}
	else
		{
		length = 0;
		aStream.WriteInt32L( length );
		}
	
    IRLOG_DEBUG( "CIROTAUpdate::ExternalizeL - Exiting." );
	}  


// ---------------------------------------------------------------------------
// CIROTAUpdate::InternalizeL()
// internalizes the data.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIROTAUpdate::InternalizeL( RReadStream& aStream )
	{
	IRLOG_DEBUG( "CIROTAUpdate::InternalizeL - Entering" );
	iUpdateInterval = aStream.ReadInt32L();
	TInt length = aStream.ReadInt32L();
	if ( length )
		{
		iVersion = HBufC::NewL( aStream, length );
		}
		
	length = aStream.ReadInt32L();
	if ( length )
		{
		iOTAUrl = HBufC::NewL( aStream, length );
		}
		
 	length = aStream.ReadInt32L();
	if ( length )
		{
		iUpdateMethod = HBufC::NewL( aStream, length );
		}
	
	IRLOG_DEBUG( "CIRBrowseCatagoryItems::InternalizeL - Exiting." );
	}  
	
	
