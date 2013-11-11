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
* Description:   This is the header file of the CClockTimeZoneResolverImpl class.
*
*/

#ifndef __CLOCK_TIMEZONE_RESOLVER_IMPL_H__ 
#define __CLOCK_TIMEZONE_RESOLVER_IMPL_H__

// System includes
#include <e32base.h>
#include <etelmm.h>
#include <tz.h>
#include <tzconverter.h>
#include <tzlocalizationdatatypes.h>
#include <vtzrules.h>

// User includes
#include "clocktimesourceinterface.hrh"

// Forward declarations
class CClockMCCTzIdMapper;

// Class declaration
/** 
* @class CClockTimeZoneResolverImpl
* @brief An instance of class CClockTimeZoneResolverImpl which is the actual implementation of timezone mapper.
* @dll clocktimezoneresolver.dll
*/
class CClockTimeZoneResolverImpl : public CBase
	{
	public:				// Constructor and destructor
	
		/**
		* @brief Creates a CClockTimeZoneResolverImpl object
		* @return CClockTimeZoneResolverImpl*
		*/
    	static CClockTimeZoneResolverImpl* NewL();
		
		/**
		* @brief Destroy the object and release all memory objects
		*/
	  	~CClockTimeZoneResolverImpl();
	    
	public:				// New functions
	
		/**
		* @brief Gets the timezone based on the MCC and NITZ info.
		* @param aTimeInfo Time information got from the plugin.
		* @param aMcc Mobile country code.
		* @param aTzId The matching timezone ID.
		* @return KErrNone if a matching timezone ID is found, KErrNotFound otherwise.
		*/
		TInt GetTimeZoneL( const STimeAttributes& aTimeInfo, 
						   const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
						   TInt& aTzId );
		
		/**
		* @brief Find all TzIds for a given MCC with optional matching StdTimeOffset
		* @param aMCC Mobile Country Code to search for
		* @param aTzIdArray Array of CTzID objects to add matching zones to.
		* @param aStdTimeOffset Limit returned zones to zones with a matching offset
		*/
		void TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
						   RArray< CTzId >& aTzIdArray,
						   const TTimeIntervalMinutes& aStdTimeOffset = -1 );
		
		/**
		* @brief Find the MCC for a given TzId
		* @param aTzId CTzId to search for
		* @return the MCC of the DST zone, or KErrNotFound if not found.
		*/
		TInt MCCFromTzIdL( const CTzId& aTzId );
		
	private:			// Constructor
	
		/**
		* @brief C++ default constructor
		*/
	  	CClockTimeZoneResolverImpl();
		
		/** 
		* @brief Symbian OS 2nd phase constructor is private.
		*/
		void ConstructL();
	  
	private:			// New functions
		
		/** 
		* @brief Uses the received Nitz UTC offset in conjunction with the
		* 		 Mobile Country Code to calculate which DST zone we are in.
		* @param aTimeInfo Time information got from any of the plugins.
		* @param aMcc Mobile Country code got from MCC Listener. 
		* @param aTzId Contains the valid timezone ID, -1 otherwise.
		* @return TInt KErrNone if successful, KErrNotFound otherwise.
		*/
		TInt FindDstZoneIdFromMccAndUtcOffsetL( const STimeAttributes& aTimeInfo, 
										  	    const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
										  	    TInt& aTzId );
										  	   
		/** 
		* @brief Gets all the localized timezones for the corresponding mcc and timezone offset.
		* @param aMcc Mobile Country code got from MCC Listener.
		* @param aTimezoneOffset The standard offset got from the plugin.
		* @param aTzLocalisedArray Contains all the matching localized timezones.
		*/
		void GetLocalizedZonesL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                               	 const TTimeIntervalMinutes& aTimezoneOffset,                                            
                                 CTzLocalizedTimeZoneArray& aTzLocalisedArray );
        
        /** 
		* @brief Gets the list of timezone ID's given an array of localized timezones.
		* @param aLocTimeZones List of localized timezones.
		* @param aZoneIdArray Array containing all matching timezone ID's.
		* @param aNwLocalTime Local time.
		* @param aNwUtcTime UTC time.
		*/                       
        void FindMatchingLocalisedZoneL( CTzLocalizedTimeZoneArray& aLocTimeZones, 
                           	    		 RPointerArray<CTzId>& aZoneIdArray, 
                                		 TTime& aNwLocalTime, 
                                		 TTime& aNwUtcTime );
	    
       	/** 
		* @brief Gets all the timezones for the corresponding mcc and timezone offset.
		* @param aMcc Mobile Country code got from MCC Listener.
		* @param aTimezoneOffset The standard offset got from the plugin.
		* @param aTzUnLocalizedArray Contains all the matching timezones.
		*/

        void GetAllUnLocalizedZonesL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                                      const TTimeIntervalMinutes& aTimezoneOffset,                                                                                        
                                      RArray< CTzId >& aTzUnLocalizedArray);


        /** 
		* @brief Gets the list of timezone ID's given an array of unlocalized timezones.
		* @param aUnLocalizedTzArray List of unlocalized timezones.
		* @param aZoneIdArray Array containing all matching timezone ID's.
		* @param aNwLocalTime Local time.
		* @param aNwUtcTime UTC time.
		*/                       
 
        void FindMatchingUnLocalizedZoneL( RArray< CTzId >& aUnLocalizedTzArray, 
                                           RPointerArray<CTzId>& aZoneIdArray, 
                                           TTime& aNwLocalTime, 
                                           TTime& aNwUtcTime );
                                           
        /**
        * @brief Find DstOffset and standard Offset for the time zone aTzId using CTzRules
        * @param aDstOffset referance to DstOffset
        * @param aStdOffset referance to Standard Offset
        * @param aTzId time zone Id for which DstOffset and StdOffset is to be found
        */
        void GetStdOffSetAndDstOffSetL( TInt32& aDstOffset, 
                                      	TInt32& aStdOffset,
                                      	const CTzId& aTzId );                                    
        
        /**
        * @brief Finds the matching timezone ID with the specified offset.
        * @param aTzOffset The offset for which we are trying to find the timezone ID.
        * @param aTzId Contains the timezone ID matching the offset.
        * @return KErrNotFound if no matches were found, KErrNone otherwise.
        */
        TInt GetTimeZoneIdWithOffsetL( const TTimeIntervalMinutes& aTzOffset,
                                       TInt& aTzId );
        /**
        * @brief Finds the matching timezone ID with the specified MCC.
        * @param aMcc The MCC for which we are trying to find the timezone ID.
        * @param aTzOffset The offset for which we are trying to find the timezone ID.
        * @param aTzId Contains the timezone ID matching the MCC.
        * @return KErrNotFound if no matches were found, KErrNone otherwise.
        */
        TInt GetTimeZoneIdWithMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                                    const TTimeIntervalMinutes& aTzOffset,
                                    TInt& aTzId );
        
        /**
        * @brief Tries to narrow down to a single timezone ID from the timezone array
        * @param aLocTimeZones The array which contains all matching timezones.
        * @param aTzId Contains the timezone ID, if found.
        * @return KErrNotFound if no matches were found, KErrNone otherwise.
        */
        TInt ProcessMatchingZones( CTzLocalizedTimeZoneArray& aLocTimeZones, TInt& aTzId );
        
        
        /**
        * @brief Tries to get timezone ID based on the dstoffset.
        * @param aLocTimeZones The array which contains all matching timezones.
        * @param aTzOffset The offset for which we are trying to find the timezone ID.
        * @param aDstOffset The DST offset for which we are trying to find the timezone ID.
        * @return KErrNotFound if no matches were found, KErrNone otherwise.
        */
        TInt GetCorrectZone( CTzLocalizedTimeZoneArray& aLocTimeZones, 
                             const TTimeIntervalMinutes& aTzOffset,
                             const TTimeIntervalMinutes& aDstOffset, TInt& aTzId );
        
        /**
        * @brief Gets the localized timezones for each zone that matches.
        * @param aLocTimeZones The array which contains all matching timezones.
        * @param aTzId Contains the timezone ID, if found.
        * @return KErrNone if we have only one matching zone.
        */
        TInt GetMatchingZonesL( CTzLocalizedTimeZoneArray* aLocTimeZones, TInt& aTzId );
 
	private:			// Data
		
		/**
		* @var iMccTzIdMapper 
		* @brief Interface to the MCC->TzIDMapper of type CMCCTzIdMapper*
		*/
		CClockMCCTzIdMapper*		iMccTzIdMapper;
		
		/**
		* @var iTempZoneIds 
		* @brief Array of timezone id's.
		*/
		RPointerArray< CTzId >		iTempZoneIds;
		
		/**
		* @var iRtz 
		* @brief Object of type RTz.
		*/
		RTz 						iRtz;

	};

#endif		// __CLOCK_TIMEZONE_RESOLVER_IMPL_H__

// End of file
