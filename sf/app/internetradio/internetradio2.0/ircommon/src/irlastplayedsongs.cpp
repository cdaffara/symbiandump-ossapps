/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "irlastplayedsongs.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIRLastPlayedSongs
// Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRLastPlayedSongs* CIRLastPlayedSongs::NewL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::NewL - Entering" );
	CIRLastPlayedSongs* self = CIRLastPlayedSongs::NewLC();
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRLastPlayedSongs::NewL - Exiting." );
	return self;
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRLastPlayedSongs* CIRLastPlayedSongs::NewLC()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::NewLC - Entering" );
	CIRLastPlayedSongs* self = new( ELeave )CIRLastPlayedSongs;
	CleanupStack::PushL( self );
	self->ConstructL();
	IRLOG_DEBUG( "CIRLastPlayedSongs::NewLC - Exiting." );
	return self;
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::~CIRLastPlayedSongs()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRLastPlayedSongs::~CIRLastPlayedSongs()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::~CIRLastPlayedSongs - Entering" );
	delete iSongName;
	iSongName = NULL;
	delete iArtistName;
	iArtistName = NULL;
	IRLOG_DEBUG( "CIRLastPlayedSongs::~CIRLastPlayedSongs - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::SetSongName()
// Function to set song name from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLastPlayedSongs::SetSongName( const TDesC& aSongName )
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::SetSongName - Entering" );
	iSongName = aSongName.Alloc();
	IRLOG_DEBUG( "CIRLastPlayedSongs::SetSongName - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::GetSongName()
// Function to get song name from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CIRLastPlayedSongs::GetSongName()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::GetSongName" );
	return *iSongName;
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::SetArtistName()
// Function to set artist name from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLastPlayedSongs::SetArtistName( const TDesC& aArtistName )
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::SetArtistName - Entering" );
	iArtistName = aArtistName.Alloc();
	IRLOG_DEBUG( "CIRLastPlayedSongs::SetArtistName - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::GetArtistName()
// Function to get artist name from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& CIRLastPlayedSongs::GetArtistName()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::GetArtistName" );
	return *iArtistName;
	}
	

// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::ExternalizeL()
// externalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLastPlayedSongs::ExternalizeL( RWriteStream& aWriteStream )
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::ExternalizeL - Entering" );
	TInt32 len = iSongName->Length();
	TRAP_IGNORE( aWriteStream.WriteInt32L( len ) )
	aWriteStream << *iSongName;
	len = iArtistName->Length();
	TRAP_IGNORE( aWriteStream.WriteInt32L( len ) )
	aWriteStream << *iArtistName;	
	IRLOG_DEBUG( "CIRLastPlayedSongs::ExternalizeL - Exiting." );	
	}
	

// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::InternalizeL()
// internalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRLastPlayedSongs::InternalizeL( RReadStream& aReadStream )
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::InternalizeL - Entering" );
	TInt8 len;
	len = aReadStream.ReadInt8L();
	iSongName = HBufC::NewL( aReadStream, len );
	len=aReadStream.ReadInt8L();
	iArtistName = HBufC::NewL( aReadStream, len) ;
	IRLOG_DEBUG( "CIRLastPlayedSongs::InternalizeL - Exiting." );
	}
		

// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::=()
// standard C++ copy constructor(deep copy) 
// ---------------------------------------------------------------------------
//	
EXPORT_C CIRLastPlayedSongs& CIRLastPlayedSongs::operator = ( const 
	CIRLastPlayedSongs& aSng )
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::operator= - Entering" );
	 if ( &aSng == this )
		{
		return *this;
		}
	 
	iArtistName = aSng.iArtistName;
	iSongName = aSng.iSongName;
	IRLOG_DEBUG( "CIRLastPlayedSongs::operator= - Exiting." );
	return *this;
	}


// ---------------------------------------------------------------------------
// CIRLastPlayedSongs::ConstructL()
// standard second phase constructor
// ---------------------------------------------------------------------------
//
void CIRLastPlayedSongs::ConstructL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongs::ConstructL" );
	return;
	}

