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
* Description:   The source file for the CClockTimeZoneResolver class.
*
*/

// User includes
#include "clocktimezoneresolver.h"
#include "clocktimezoneresolverimpl.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CClockTimeZoneResolver::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockTimeZoneResolver* CClockTimeZoneResolver::NewL()
	{
	__PRINTS( "CClockTimeZoneResolver::NewL - Entry" );
	
	CClockTimeZoneResolver* self = new ( ELeave ) CClockTimeZoneResolver;
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	CleanupStack::Pop();
	
	__PRINTS( "CClockTimeZoneResolver::NewL - Exit" );
	
	return self;
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolver::~CClockTimeZoneResolver
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockTimeZoneResolver::~CClockTimeZoneResolver()
	{
	__PRINTS( "CClockTimeZoneResolver::~CClockTimeZoneResolver - Entry" );
	
	if( iTzResolverImpl )
		{
		delete iTzResolverImpl;
		iTzResolverImpl = NULL;
		}
	
	__PRINTS( "CClockTimeZoneResolver::~CClockTimeZoneResolver - Exit" );
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolver::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolver::ConstructL()
	{
	__PRINTS( "CClockTimeZoneResolver::ConstructL - Entry" );
	
	// Create the resolver implementation.
	if( !iTzResolverImpl )
		{
		iTzResolverImpl = CClockTimeZoneResolverImpl::NewL();
		}
	
	__PRINTS( "CClockTimeZoneResolver::ConstructL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolver::GetTimeZoneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClockTimeZoneResolver::GetTimeZoneL( const STimeAttributes& aTimeInfo, 
		  									   		const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
		  									   		TInt& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolver::GetTimeZoneL - Entry" );
	
	// Get the timezone information from the implementation.
	TInt returnVal( KErrNotFound );
	
	if( iTzResolverImpl )
		{
		returnVal = iTzResolverImpl->GetTimeZoneL( aTimeInfo, aMcc, aTzId );
		}

	__PRINTS( "CClockTimeZoneResolver::GetTimeZoneL - Exit" );

	return returnVal;
	}

// ---------------------------------------------------------
// CClockTimeZoneResolver::TzIdFromMccL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClockTimeZoneResolver::TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
											   		RArray< CTzId >& aTzIdArray,
											   		const TTimeIntervalMinutes& aStdTimeOffset )
	{
	__PRINTS( "CClockTimeZoneResolver::TzIdFromMccL - Entry" );
	
	if( iTzResolverImpl )
	    {
	    iTzResolverImpl->TzIdFromMccL( aMCC, aTzIdArray, aStdTimeOffset );
	    }
	
	__PRINTS( "CClockTimeZoneResolver::TzIdFromMccL - Exit" );
	}

// ---------------------------------------------------------
// CClockTimeZoneResolver::MCCFromTzIdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClockTimeZoneResolver::MCCFromTzIdL( const CTzId& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolver::MCCFromTzIdL - Entry" );
	
	TInt returnVal( KErrNotFound );
	
	if( iTzResolverImpl )
		{
		returnVal = iTzResolverImpl->MCCFromTzIdL( aTzId );
		}
	
	__PRINTS( "CClockTimeZoneResolver::MCCFromTzIdL - Exit" );

	return returnVal;
	}

// End of file
