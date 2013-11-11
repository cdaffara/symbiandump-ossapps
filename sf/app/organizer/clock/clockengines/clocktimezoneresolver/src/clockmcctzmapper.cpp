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
* Description:   The source file for the CClockMCCTzIdMapper class.
*
*/

// System includes
#include <tz.h>
#include <tzdefines.h>
#include <vtzrules.h>
#include <bautils.h>

// User includes
#include "clockmcctzmapper.h"
#include "clock_debug.h"

// Constants

// Literals
_LIT( KMCCResourceFileDir, "\\resource\\mcc\\" );
_LIT( KMCCResourceFileName, "mcc.rsc" );

// ---------------------------------------------------------
// CClockMCCTzIdMapper::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMCCTzIdMapper* CClockMCCTzIdMapper::NewL()
	{
	__PRINTS( "CClockMCCTzIdMapper::NewL - Entry" );
    
    CClockMCCTzIdMapper* self = CClockMCCTzIdMapper::NewLC();
	CleanupStack::Pop( self );
	
	__PRINTS( "CClockMCCTzIdMapper::NewL - Exit" );
	
	return self;
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::NewLC
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMCCTzIdMapper* CClockMCCTzIdMapper::NewLC()
	{
	__PRINTS( "CClockMCCTzIdMapper::NewLC - Entry" );
	
	CClockMCCTzIdMapper* self = new ( ELeave ) CClockMCCTzIdMapper();
	CleanupStack::PushL( self );

	self->ConstructL();
	
	__PRINTS( "CClockMCCTzIdMapper::NewLC - Exit" );
	
	return self;
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMCCTzIdMapper::ConstructL()
	{
	__PRINTS( "CClockMCCTzIdMapper::ConstructL - Entry" );
	
	// Connect to the file server
	User::LeaveIfError( iFs.Connect() );	
    
    // Get the full filename with drive letter
    TFileName rscFileName; 
    TFindFile fileFinder( iFs );

    // TFindFile applies search order that is from drive Y to A, then Z
    TInt err = fileFinder.FindByDir( KMCCResourceFileName, KMCCResourceFileDir );

    if ( err == KErrNone )
        {
        // Found file.
        rscFileName = fileFinder.File();
        }
    else
        {
        __PRINTS( "Unable to open mcc.rsc. Leaving now!!" );
        
        // Leave with reason, file not found.
        User::Leave( KErrPathNotFound );
        }
    
	// Open the resource file.  The resource file will not be localized as it
	// only contains integers, so there is no need to find a language specific
	// resource file name.
	iMCCResourceFile.OpenL( iFs, rscFileName );
	iMCCResourceFile.ConfirmSignatureL();

	// Assign buffer for holding resource
	TInt resourceId = iMCCResourceFile.Offset() + EMCCFirstResource;
	iResourceBuffer = iMCCResourceFile.AllocReadL( resourceId );
	
	__PRINTS( "CClockMCCTzIdMapper::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::~CClockMCCTzIdMapper
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMCCTzIdMapper::~CClockMCCTzIdMapper()
	{
	__PRINTS( "CClockMCCTzIdMapper::~CClockMCCTzIdMapper - Entry" );
	
	// Close the resource file.
	iMCCResourceFile.Close();
	// Free up the resource buffer.
	if( iResourceBuffer )
	    {
	    delete iResourceBuffer;
	    iResourceBuffer = NULL;
	    }

	// Close the file server session.
	iFs.Close();
	
	__PRINTS( "CClockMCCTzIdMapper::~CClockMCCTzIdMapper - Exit" );
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::UTCOffSetWithoutDSTChangesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockMCCTzIdMapper::UTCOffSetWithoutDSTChangesL( RTz& aTz, const CTzId& aTzId ) const
	{
	__PRINTS( "CClockMCCTzIdMapper::UTCOffSetWithoutDSTChangesL - Entry" );
	
	TInt timeOffset( KErrNotFound );

	TTime universalTime;
	universalTime.UniversalTime();

	// Get the current rules for the timezone rules
	CTzRules* currentRules = aTz.GetTimeZoneRulesL( aTzId, universalTime, universalTime, ETzUtcTimeReference );
	if( currentRules )
	    {
	    timeOffset = currentRules->InitialStdTimeOffset();
	    delete currentRules;
	    }
	
	__PRINTS( "CClockMCCTzIdMapper::UTCOffSetWithoutDSTChangesL - Exit" );

	return timeOffset;
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::TzIdFromMccL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMCCTzIdMapper::TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
								        RArray< CTzId >& aTzIdArray,
								        const TTimeIntervalMinutes& aStdTimeOffset )
	{
	__PRINTS( "CClockMCCTzIdMapper::TzIdFromMccL - Entry" );
	
	// Convert the TMobilePhoneNetworkCountryCode to a TInt.
	TInt currentMcc;
	TLex lexUtil( aMCC );
	User::LeaveIfError( lexUtil.Val( currentMcc ) );
	
	__PRINT( "MCC: %d", currentMcc );

	// Get all the DST zones that match mcc
	DoGetTzIdFromMCCL( currentMcc, aTzIdArray );

	// Limit the returned entries to those with a specific StdTimeOffset
	TInt offset = aStdTimeOffset.Int();
	if( offset != -1 )
		{
		// Connect to Tz
		RTz tzHandle;
		CleanupClosePushL( tzHandle );
		User::LeaveIfError( tzHandle.Connect() );

		TInt currentZoneOffset;
		TInt count = aTzIdArray.Count();
		// Loop through the array in reverse order, removing all entries
		// with non matching standard time offsets.
		for( TInt index( count - 1 ); index >= KErrNone; --index )
			{
			currentZoneOffset = UTCOffSetWithoutDSTChangesL( tzHandle, aTzIdArray[ index ] );
			if( currentZoneOffset != offset )
				{
				aTzIdArray.Remove( index );
				}
			}
		// Cleanup.
		CleanupStack::PopAndDestroy( &tzHandle );
		}
	
	__PRINTS( "CClockMCCTzIdMapper::TzIdFromMccL - Exit" );
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::TzIdFromOffsetL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMCCTzIdMapper::TzIdFromOffsetL( const TTimeIntervalMinutes& aStdTimeOffset, RArray< CTzId >& aTzIdArray )
	{
	__PRINTS( "CClockMCCTzIdMapper::TzIdFromOffsetL - Entry" );
	
	// Reset the position in the resource buffer
	iResourceReader.SetBuffer( iResourceBuffer );
	// First TInt is the number of array items
	TInt numberOfItems( iResourceReader.ReadInt16() );

	// Loop through the resource array reading the TzId
	for( TInt index( KErrNone ); index < numberOfItems; ++index )
		{
		TInt timeZoneId( iResourceReader.ReadInt16() );
		TInt mobileCC( iResourceReader.ReadInt16() );
		
		CTzId* timeZone = CTzId::NewL( timeZoneId );
		CleanupStack::PushL( timeZone );
		
		aTzIdArray.AppendL( *timeZone );
		
		CleanupStack::PopAndDestroy( timeZone );
		timeZone = NULL;
		}
	
	// Limit the returned entries to those with a specific StdTimeOffset
	TInt timeOffset( aStdTimeOffset.Int() );

	// Connect to Tz
	RTz tzHandle;
	CleanupClosePushL( tzHandle );
	User::LeaveIfError( tzHandle.Connect() );

	TInt currentZoneOffset;
	TInt idCount( aTzIdArray.Count() );

	// Loop through the array in reverse order, removing all entries
	// with non matching standard time offsets.
	for( TInt index( idCount - 1 ); index >= KErrNone; --index )
		{
		currentZoneOffset = UTCOffSetWithoutDSTChangesL( tzHandle, aTzIdArray[ index ] );
		if( currentZoneOffset != timeOffset )
			{
			aTzIdArray.Remove( index );
			}
		}
	CleanupStack::PopAndDestroy( &tzHandle );
	
	// This is a check to see if the timezone array has multiple entries with same timezone ID.
	// This happens because many countries have multiple MCCs for a single timezone ID.
	// For ex: India has MCCs 404 and 405 for the same timezone ID 1624 and offset GMT+5:30.
	TInt zoneCount( aTzIdArray.Count() );
	if( 1 < zoneCount )
		{
		CTzId& prevTzId = aTzIdArray[ KErrNone ];
		for( TInt zoneIndex( zoneCount - 1 ); zoneIndex > KErrNone; --zoneIndex )
			{
			CTzId& curTzId = aTzIdArray[ zoneIndex ];
			if( prevTzId != curTzId )
				{
				// There are multiple timezones. So skip.
				break;
				}
			else
				{
				// Keep removing duplicate zone IDs.
				aTzIdArray.Remove( zoneIndex );
				}
			}
		}

	__PRINTS( "CClockMCCTzIdMapper::TzIdFromOffsetL - Exit" );
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::DoGetTzIdFromMCCL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMCCTzIdMapper::DoGetTzIdFromMCCL( TInt aMcc, RArray< CTzId >& aTzIdArray )
	{
	__PRINTS( "CClockMCCTzIdMapper::DoGetTzIdFromMCCL - Entry" );
	
	__PRINT( "MCC:%d", aMcc );
	
	// Reset the position in the resource buffer.
	iResourceReader.SetBuffer( iResourceBuffer );
	// First TInt is the number of array items.
	TInt numberOfItems( iResourceReader.ReadInt16() );

	TInt timeZoneId;
	TInt mobileCC;
	CTzId* matchingDSTZone( NULL );

	// Loop through the resource array reading the TzId / MCC pairs.
	for( TInt index( KErrNone ); index < numberOfItems; ++index )
		{
		timeZoneId = iResourceReader.ReadInt16();
		mobileCC = iResourceReader.ReadInt16();

		// Found a match - create a new CTzId and append to the TzId array.
		if( mobileCC == aMcc )
			{
			matchingDSTZone = CTzId::NewL( timeZoneId );
			CleanupStack::PushL( matchingDSTZone );
			
			aTzIdArray.AppendL( *matchingDSTZone );
			
			CleanupStack::PopAndDestroy( matchingDSTZone );
			
			matchingDSTZone = NULL;
			}
		}
	
	__PRINTS( "CClockMCCTzIdMapper::DoGetTzIdFromMCCL - Exit" );
	}

// ---------------------------------------------------------
// CClockMCCTzIdMapper::MCCFromTzIdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockMCCTzIdMapper::MCCFromTzIdL( const CTzId& aTzId )
	{
	__PRINTS( "CClockMCCTzIdMapper::MCCFromTzIdL - Entry" );
	
	// Reset the position in the resource buffer
	iResourceReader.SetBuffer( iResourceBuffer );
	// First TInt is the number of array items
	TInt numberOfItems( iResourceReader.ReadInt16() );
	TInt timeZoneId;
	TInt mobileCC;

	// Loop through the resource array reading the TzId / MCC pairs
	for( TInt index( KErrNone ); index < numberOfItems; ++index )
		{
		timeZoneId = iResourceReader.ReadInt16();
		mobileCC = iResourceReader.ReadInt16();

		// Found a match return the mcc
		if( timeZoneId == aTzId.TimeZoneNumericID() )
			{
			__PRINT( "Found a match, MCC - %d", mobileCC );
			
			return mobileCC;
			}
		}
	
	__PRINTS( "CClockMCCTzIdMapper::MCCFromTzIdL - Exit" );
	
	return KErrNotFound;
	}

// End of file
