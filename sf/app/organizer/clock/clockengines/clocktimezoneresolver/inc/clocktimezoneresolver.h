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
* Description:   The header file for the CClockTimeZoneResolver class.
*
*/

#ifndef __CLOCK_TIMEZONE_RESOLVER_H__ 
#define __CLOCK_TIMEZONE_RESOLVER_H__ 

//System includes
#include <e32base.h>
#include <etelmm.h>

// User includes
#include "clocktimesourceinterface.hrh"

// Forward declarations
class CClockTimeZoneResolverImpl;
class CTzId;

// Constants
const TInt KInvalidTimeZoneId( -1 );

// Literals
_LIT( KInvalidMCC, "9999" );

// Class declaration
/** 
* @class CClockTimeZoneResolver
* @brief An instance of class CClockTimeZoneResolver which provides interfaces to get timezone related data.
* @dll clocktimezoneresolver.dll
*/
class CClockTimeZoneResolver : public CBase
	{
	public:				// New functions
	
		/**
		* @brief Creates a CClockTimeZoneResolver object
		* @return CClockTimeZoneResolver*
		*/
	    IMPORT_C static CClockTimeZoneResolver* NewL();        								   
		
		/**
		* @brief Destroy the object and release all memory objects
		*/
		IMPORT_C ~CClockTimeZoneResolver();
	    
	public:				// From base classes
	
		/**
		* @brief Gets the timezone based on the MCC and NITZ info.
		* @param aTimeInfo Time information got from the plugin.
		* @param aMcc Mobile country code.
		* @param aTzId The matching timezone ID.
		* @return KErrNone if a matching timezone ID is found, KErrNotFound otherwise.
		*/
		IMPORT_C TInt GetTimeZoneL( const STimeAttributes& aTimeInfo, 
				  		   			const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
				  		   			TInt& aTzId );
		
		/**
		* @brief Find all TzIds for a given MCC with optional matching StdTimeOffset
		* @param aMCC Mobile Country Code to search for
		* @param aTzIdArray Array of CTzID objects to add matching zones to.
		* @param aStdTimeOffset Limit returned zones to zones with a matching offset
		*/
		IMPORT_C void TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
						   			RArray< CTzId >& aTzIdArray,
						   			const TTimeIntervalMinutes& aStdTimeOffset = -1 );
		
		/**
		* @brief Find the MCC for a given TzId
		* @param aTzId CTzId to search for
		* @return the MCC of the DST zone, or KErrNotFound if not found.
		*/
		IMPORT_C TInt MCCFromTzIdL( const CTzId& aTzId );
	
	private:			// New functions
		
		/** 
		* @brief Symbian OS 2nd phase constructor is private.
		*/
	    void ConstructL();
		
	private:			// Data
	
		/**
		* @var iTzResolverImpl 
		* @brief An object of type CClockTimeZoneResolverImpl
		*/
		CClockTimeZoneResolverImpl*				iTzResolverImpl;	
	};


#endif	// __CLOCK_TIMEZONE_RESOLVER_H__

// End of file
 
 