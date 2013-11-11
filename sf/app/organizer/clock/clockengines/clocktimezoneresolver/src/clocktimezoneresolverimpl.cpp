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
* Description:   The source file for the CClockTimeZoneResolverImpl class.
*
*/

// System includes
#include <tzlocalizer.h>

// User includes
#include "clocktimezoneresolverimpl.h"
#include "clockmcctzmapper.h"
#include "clocktimezoneresolver.h"
#include "clock_debug.h"

// Constants
const TInt KMinute( 60 );     // Seconds
const TInt KZerothRule( 0 );
const TInt KOne( 1 );

// Literals
_LIT( KDefault, "default" );

// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------------------------
//
CClockTimeZoneResolverImpl* CClockTimeZoneResolverImpl::NewL()
	{
	__PRINTS( "CClockTimeZoneResolverImpl::NewL - Entry" );
	
	CClockTimeZoneResolverImpl* self = new ( ELeave ) CClockTimeZoneResolverImpl();
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	CleanupStack::Pop();
	
	__PRINTS( "CClockTimeZoneResolverImpl::NewL - Exit" );
	
	return self;
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::~CClockTimeZoneResolverImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockTimeZoneResolverImpl::~CClockTimeZoneResolverImpl()
	{
	__PRINTS( "CClockTimeZoneResolverImpl::~CClockTimeZoneResolverImpl - Entry" );
	
	if( iMccTzIdMapper )
		{
		delete iMccTzIdMapper;
		iMccTzIdMapper = NULL;
		}
		
	iRtz.Close();
	
	iTempZoneIds.ResetAndDestroy();
		
	__PRINTS( "CClockTimeZoneResolverImpl::~CClockTimeZoneResolverImpl - Exit" );
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::CClockTimeZoneResolverImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
CClockTimeZoneResolverImpl::CClockTimeZoneResolverImpl()
	{
	__PRINTS( "CClockTimeZoneResolverImpl::CClockTimeZoneResolverImpl - Entry" );
	
	// No implementation yet
	
	__PRINTS( "CClockTimeZoneResolverImpl::CClockTimeZoneResolverImpl - Exit" );
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::ConstructL()
	{
	__PRINTS( "CClockTimeZoneResolverImpl::ConstructL - Entry" );
	
	iMccTzIdMapper = CClockMCCTzIdMapper::NewL();
	
	User::LeaveIfError( iRtz.Connect() );
	iRtz.SetAutoUpdateBehaviorL( RTz::ETZAutoDSTUpdateOn );
	
	__PRINTS( "CClockTimeZoneResolverImpl::ConstructL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::GetTimeZoneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::GetTimeZoneL( const STimeAttributes& aTimeInfo, 
										  	   const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
										  	   TInt& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneL - Entry" );
	
	__PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneL - Exit" );
	
	// Try and find a DST zone Id that has current MCC and this UTC offset. 
    return FindDstZoneIdFromMccAndUtcOffsetL( aTimeInfo, aMcc, aTzId );
	}

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::TzIdFromMccL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
									  		   RArray< CTzId >& aTzIdArray,
									  		   const TTimeIntervalMinutes& aStdTimeOffset )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::TzIdFromMccL - Entry" );
	
	iMccTzIdMapper->TzIdFromMccL( aMCC, aTzIdArray, aStdTimeOffset );
	
	__PRINTS( "CClockTimeZoneResolverImpl::TzIdFromMccL - Exit" );
	}

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::MCCFromTzIdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::MCCFromTzIdL( const CTzId& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::MCCFromTzIdL - Entry" );
	
	__PRINTS( "CClockTimeZoneResolverImpl::MCCFromTzIdL - Exit" );
	
	return iMccTzIdMapper->MCCFromTzIdL( aTzId );
	}

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL( const STimeAttributes& aTimeInfo, 
																	const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
										  							TInt& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Entry" );
	
	// Check if MCC is not available, in which case aMcc will be sent as 9999. 
	// Try to narrow down to the timezone ID using the offset that we have got in aTimeInfo.
	const TBuf< 4 > invalidMCC( KInvalidMCC );
	
	if( invalidMCC == aMcc &&  aTimeInfo.iTimeZoneOffset != TTimeIntervalMinutes( KErrNotFound ) )
		{
		__PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Exit" );
		
		// Try to find if we can narrow down to a timezone ID with just the offset.
		return GetTimeZoneIdWithOffsetL( aTimeInfo.iTimeZoneOffset, aTzId );
		}
	
	// Check if we can find any zones for the given MCC even though we dont have a valid
	// timezone offset. If so, set the timezone in case of countries with single zone.
	if( aTimeInfo.iTimeZoneOffset == TTimeIntervalMinutes( KErrNotFound ) )
		{		
		__PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Exit" );
		
		return GetTimeZoneIdWithMccL( aMcc, aTimeInfo.iTimeZoneOffset, aTzId );
		}

    // Calculate timezone offset.
    TTimeIntervalSeconds tzOffsetSec(0);
    
    if( KErrNotFound != aTimeInfo.iTimeZoneOffset.Int() )
    	{
    	tzOffsetSec=(aTimeInfo.iTimeZoneOffset.Int() * KMinute );
    	}
    
    // Calculate dst offset.
    TTimeIntervalSeconds dstOffsetSec(0);
    
    if( KErrNotFound != aTimeInfo.iDstOffset.Int() )
    	{
    	dstOffsetSec=( aTimeInfo.iDstOffset.Int() * KMinute );
    	}
    
    // Time sent by nw is UTC
    TTime nwUtcTime( aTimeInfo.iUtcDateTime );

    // Calculate local time => UTC time + timzone offset + DST offset
    TTime nwLocalTime( nwUtcTime + tzOffsetSec + dstOffsetSec );
	

	// Create array we we store all localized zones
    CTzLocalizedTimeZoneArray* locTimeZones = CTzLocalizedTimeZoneArray::NewL();
    CleanupStack::PushL( locTimeZones );
    
    // Create array of CTzID to store all unlocalized zones
    RArray<CTzId> tzArray;
    CleanupClosePushL(tzArray);

    // Get the unlocalized zones based on the utc offset and mcc we have.
    GetAllUnLocalizedZonesL( aMcc, aTimeInfo.iTimeZoneOffset, tzArray );
    
    // Make sure old stuff is deleted.
    iTempZoneIds.ResetAndDestroy();

    // Try to find a truly matching zone.
    FindMatchingUnLocalizedZoneL( tzArray, iTempZoneIds,nwLocalTime, nwUtcTime );
       
    CleanupStack::PopAndDestroy(1, &tzArray);   
    
    if( KErrNone == iTempZoneIds.Count() )
    	{
    	// Get the localized zones based on the utc offset and mcc we have
    	GetLocalizedZonesL( aMcc, aTimeInfo.iTimeZoneOffset, *locTimeZones );

    	// If we have no localised zones and network supports DST, we have done everything right, 
    	// OR if UTC offset is invalid and we have a multizone country, we can do nothing.
    
    	TInt returnVal( GetCorrectZone( *locTimeZones, aTimeInfo.iTimeZoneOffset, aTimeInfo.iDstOffset, aTzId ) );
    		
    	if( KErrNotFound == aTzId || KErrNone == returnVal )
    		{
    		__PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Exit" );
		
    		return returnVal;
    		}
    		    	
    	// Make sure old stuff is deleted.
    	iTempZoneIds.ResetAndDestroy();

    	// Try to find a truly matching zone.
    	FindMatchingLocalisedZoneL( *locTimeZones, iTempZoneIds, nwLocalTime, nwUtcTime );

    	// nitz improvement for spreading UTC offset start/

    	// If no matches found and DST indication is not supported by the NW,
    	// we need to subtract one DST offset amount from the UTC offset
    	// and see if that gives us any zones. 
    	// This is for cases when operators dont send the DST section of a nitz
    	// even when it is included in the utc offset.

    	if ( KErrNone == iTempZoneIds.Count() && ( KErrNotFound == aTimeInfo.iDstOffset.Int() ) ) 
    		{
    		TTimeIntervalMinutes tempUtcOffset( 0 );
    		RArray< CTzId > aTzIdArray;
    		CleanupClosePushL( aTzIdArray );

    		// Convert the TMobilePhoneNetworkCountryCode to a TInt
    		TInt mcc;
    		TLex lex( aMcc );
    		User::LeaveIfError( lex.Val( mcc ) );

    		// Get the DST zones within this country code
    		iMccTzIdMapper->DoGetTzIdFromMCCL( mcc, aTzIdArray );

    		CTzLocalizedTimeZoneArray* spreadLocTimeZones = CTzLocalizedTimeZoneArray::NewL();
    		CleanupStack::PushL( spreadLocTimeZones );
    		TInt32 dstOffsetInt = 0;
    		TInt32 tmpUtcOffset = 0;
    		TInt32 dstOffsetOld = -1;

    		for( TInt i( 0 ) ; i < aTzIdArray.Count(); i++ )
    			{
    			// Get the StdOffset and DstOffset values for every time zone.
    			// Use each new value of StdOffset to find matching LocalTimeZones
    			GetStdOffSetAndDstOffSetL( dstOffsetInt, tmpUtcOffset, aTzIdArray[ i ] );
    			if( dstOffsetOld != dstOffsetInt )
    				{
    				dstOffsetOld = dstOffsetInt;
    				}
    			else
    				{
    				continue;
    				}

    			tmpUtcOffset = aTimeInfo.iTimeZoneOffset.Int() - dstOffsetInt;
    			GetLocalizedZonesL( aMcc, tmpUtcOffset, *spreadLocTimeZones );
    			FindMatchingLocalisedZoneL( *spreadLocTimeZones, iTempZoneIds, nwLocalTime, nwUtcTime );  
    			}

    		CleanupStack::PopAndDestroy( spreadLocTimeZones );
    		CleanupStack::PopAndDestroy( &aTzIdArray );
    		}    
    	// nitz improvement for spreading UTC offset end
    	}
      
   
    TInt returnValue ( GetMatchingZonesL( locTimeZones, aTzId ) );
    
    if ( KErrNone == returnValue )
    {
    __PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Exit" );
    
    return returnValue;
    }

    // If there are more than one matching unlocalized cities and zero localized cities
    // set to any one of the unlocalized cities.
    __PRINTS( "CClockTimeZoneResolverImpl::FindDstZoneIdFromMccAndUtcOffsetL - Exit" );
    
    return ProcessMatchingZones( *locTimeZones, aTzId );
    }
    
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::GetLocalizedZonesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::GetLocalizedZonesL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                                            		 const TTimeIntervalMinutes& aTimezoneOffset,                                            
                                            		 CTzLocalizedTimeZoneArray& aTzLocalisedArray )
    {
    __PRINTS( "CClockTimeZoneResolverImpl::GetLocalizedZonesL - Entry" );
    
    RArray< CTzId > tzArray;
    CleanupClosePushL( tzArray );
  
    // Get the DST zones within this country code
    iMccTzIdMapper->TzIdFromMccL( aMcc, tzArray, aTimezoneOffset );
        
    // If there is no DST zones for this MCC in our database, we can do nothing.
    if( tzArray.Count() == 0 ) 
		{
	    CleanupStack::PopAndDestroy( &tzArray );
	    
	    __PRINTS( "CClockTimeZoneResolverImpl::GetLocalizedZonesL - Exit" );
	    
        return;
		}

    CTzLocalizedTimeZone* timezone( NULL );
    
    // Create the localizer component
    CTzLocalizer* localizer = CTzLocalizer::NewL(); 
    CleanupStack::PushL( localizer );
    
    // Loop through the returned DST zone Ids picking the ones that have 
    // a corresponding localized timezone
    for( TInt index = 0 ; index < tzArray.Count() ; index++) 		
		{
        TRAPD( error, timezone = localizer->GetLocalizedTimeZoneL( tzArray[ index ].TimeZoneNumericID() ) );
        if( !error )
            {
            if( timezone ) 
                {
                aTzLocalisedArray.AppendL( timezone );
                }			
            }
		}
    
    CleanupStack::PopAndDestroy( 2, &tzArray );
    
    __PRINTS( "CClockTimeZoneResolverImpl::GetLocalizedZonesL - Exit" );
    }

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::FindMatchingLocalisedZoneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::FindMatchingLocalisedZoneL( CTzLocalizedTimeZoneArray& aLocTimeZones, 
                                           	            	 RPointerArray< CTzId >& aZoneIdArray, 
                                                        	 TTime& aNwLocalTime, 
                                                        	 TTime& aNwUtcTime )
    {
    __PRINTS( "CClockTimeZoneResolverImpl::FindMatchingLocalisedZoneL - Entry" );
    
    CTzConverter* tempCTzConv = CTzConverter::NewL( iRtz );
    CleanupStack::PushL( tempCTzConv );
 
    // Try to find a matching time from the list of localised timezones
    for( TInt index = 0 ; index < aLocTimeZones.Count() ; index++ )
        {            
        CTzId* tempTzId = CTzId::NewL( aLocTimeZones.At( index ).TimeZoneId() );
        CleanupStack::PushL( tempTzId );
        
        TTime tempTime = aNwUtcTime;            
        User::LeaveIfError( tempCTzConv->ConvertToLocalTime( tempTime,*tempTzId ) );
        if( tempTime == aNwLocalTime )
            {            
            aZoneIdArray.AppendL( tempTzId );
            CleanupStack::Pop( tempTzId );
            }
        else
            {
            CleanupStack::PopAndDestroy( tempTzId );
            }           
        }
    
    CleanupStack::PopAndDestroy( tempCTzConv );
    
    __PRINTS( "CClockTimeZoneResolverImpl::FindMatchingLocalisedZoneL - Exit" );
    }
    
// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::GetAllUnLocalizedZonesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::GetAllUnLocalizedZonesL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                                            			  const TTimeIntervalMinutes& aTimezoneOffset,                                                                                        
                                           				  RArray< CTzId >& aTzUnLocalizedArray )
    {      
    __PRINTS( "CClockTimeZoneResolverImpl::GetAllUnLocalizedZonesL - Entry" );
        
    // Get the DST zones within this country code
    iMccTzIdMapper->TzIdFromMccL( aMcc,aTzUnLocalizedArray,aTimezoneOffset);
    
    __PRINTS( "CClockTimeZoneResolverImpl::GetAllUnLocalizedZonesL - Exit" );
    }

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::FindMatchingUnLocalizedZoneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::FindMatchingUnLocalizedZoneL( RArray< CTzId >& aUnLocalizedTzArray, 
                                           					   RPointerArray< CTzId >& aZoneIdArray, 
                                           					   TTime& aNwLocalTime, 
                                           					   TTime& aNwUtcTime)
    {
	__PRINTS( "CClockTimeZoneResolverImpl::FindMatchingUnLocalizedZoneL - Entry" );

	CTzConverter* tempCTzConv = CTzConverter::NewL(iRtz);
	CleanupStack::PushL(tempCTzConv);

	// Try to find a matching time from the list of timezones
	for (TInt index( 0 ); index < aUnLocalizedTzArray.Count(); index++)
		{                    

		CTzId* tempTzId = CTzId::NewL( aUnLocalizedTzArray[ index ].TimeZoneNumericID() );
		CleanupStack::PushL( tempTzId );
		TTime tempTime = aNwUtcTime;   

		//Convert the zone time of the city to local time in microseconds                  
		User::LeaveIfError( tempCTzConv->ConvertToLocalTime(tempTime,*tempTzId) );

		//If the networklocal time and unlocalised city time from TZ rules 
		//are same, add it to list
		if( tempTime == aNwLocalTime )
			{            
			aZoneIdArray.AppendL( tempTzId );
			CleanupStack::Pop( tempTzId );
			}
		else
			{
			CleanupStack::PopAndDestroy( tempTzId );
			}           
		}

	CleanupStack::PopAndDestroy( tempCTzConv );
    
    __PRINTS( "CClockTimeZoneResolverImpl::FindMatchingUnLocalizedZoneL - Exit" );
	}

// ---------------------------------------------------------
// CClockTimeZoneResolverImpl::GetStdOffSetAndDstOffSetL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeZoneResolverImpl::GetStdOffSetAndDstOffSetL( TInt32& aDstOffset,
													   		TInt32& aStdOffset,
													   		const CTzId& aTzId )
    {
	__PRINTS( "CClockTimeZoneResolverImpl::GetStdOffSetAndDstOffSetL - Entry" );   
	
	// Local time
    TTime homeTime;
    homeTime.HomeTime();    
    
    TDateTime dateTime;
    dateTime = homeTime.DateTime();
    
	// Get the CTzRules for the current year and for the given time zone id.
    CTzRules* tzRules = iRtz.GetTimeZoneRulesL( aTzId, 
    										    dateTime.Year(), 
    										    dateTime.Year(), 
    										    ETzWallTimeReference );
	CleanupStack::PushL( tzRules );
    
	// Get the Actualised rules for the same year. These are the DST rules from which we get the iNewOffset.
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
													homeTime.DateTime().Year(), 
													homeTime.DateTime().Year() );
	CleanupStack::PushL( vActualisedRules );
	tzRules->GetActualisedRulesL( *vActualisedRules );
	
	// The initial offset or the standard offset (w/o DST)
	TInt32 initialTimeZoneOffset = tzRules->InitialStdTimeOffset();
	
	// The number of actualised rules
	TInt ruleCount = vActualisedRules->Count();
	
	for( TInt ruleIndex( KZerothRule ); ruleIndex < ruleCount; ruleIndex++ )
		{
		const TVTzActualisedRule& tVTzactRule = ( *vActualisedRules )[ ruleIndex ];
		
		// If the standard offset and the new offset do not match then we have a dst offset.
		// Technically if a timezone has DST then it can have a max of two offsets. One is the standard which doesn't show the 
		// DST usage, and the other is the DST offset which is standard offset + the amount of DST
		if( initialTimeZoneOffset != tVTzactRule.iNewOffset )
			{
			aDstOffset = tVTzactRule.iNewOffset - initialTimeZoneOffset;
			aStdOffset = initialTimeZoneOffset;
			CleanupStack::PopAndDestroy( vActualisedRules );
			CleanupStack::PopAndDestroy( tzRules );
			
			__PRINTS( "CClockTimeZoneResolverImpl::GetStdOffSetAndDstOffSetL - Exit" );
	
			return;
			}
		}
	__PRINTS( "CClockTimeZoneResolverImpl::GetStdOffSetAndDstOffSetL - Exit" );	

    CleanupStack::PopAndDestroy( vActualisedRules );
	CleanupStack::PopAndDestroy( tzRules );
	}

// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::GetTimeZoneIdWithOffsetL
// rest of the details commented in the header
// ---------------------------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::GetTimeZoneIdWithOffsetL( const TTimeIntervalMinutes& aTzOffset, TInt& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneIdWithOffsetL() - Entry" );
	
	TInt retVal( KErrNotFound );
	
	RArray< CTzId > tzIdArray;
	CleanupClosePushL( tzIdArray );
	
	iMccTzIdMapper->TzIdFromOffsetL( aTzOffset, tzIdArray );
	
	if( KOne < tzIdArray.Count() )	
		{
		// For a multizone country without NITZ, it is not possible to 
		// calculate the TZ.
		aTzId = KErrNotFound;					
		}
	else if( KOne == tzIdArray.Count() )
		{
		// For a single zone country, without NITZ, set the default TZ
		aTzId = tzIdArray[ KErrNone ].TimeZoneNumericID();		 	
		retVal = KErrNone;
		}
	
	CleanupStack::PopAndDestroy();
	
	__PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneIdWithOffsetL() - Exit" );
	
	return retVal;
	}
    
// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::GetTimeZoneIdWithMccL
// rest of the details commented in the header
// ---------------------------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::GetTimeZoneIdWithMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc,
                                                   const TTimeIntervalMinutes& aTzOffset,
                                                   TInt& aTzId )
    {
    __PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneIdWithMccL() - Entry" );
    
    TInt retVal( KErrNotFound );
    
    RArray< CTzId > tzIdArray;
    CleanupClosePushL( tzIdArray );
            
    // Use the MCC to get all unlocalised zones.
    iMccTzIdMapper->TzIdFromMccL( aMcc, tzIdArray, aTzOffset );
    
    if( KOne < tzIdArray.Count() ) 
        {
        // For a multizone country without NITZ, it is not possible to 
        // calculate the TZ.
        aTzId = KErrNotFound;                 
        }
    else if( KOne == tzIdArray.Count() )
        {
        // For a single zone country, without NITZ, set the default TZ
        aTzId = tzIdArray[ KErrNone ].TimeZoneNumericID();         
        retVal = KErrNone;
        }
    
    CleanupStack::PopAndDestroy();
    
    __PRINTS( "CClockTimeZoneResolverImpl::GetTimeZoneIdWithMccL() - Exit" );
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::ProcessMatchingZonesL
// rest of the details commented in the header
// ---------------------------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::ProcessMatchingZones( CTzLocalizedTimeZoneArray& aLocTimeZones, TInt& aTzId )
    {
    __PRINTS( "CClockTimeZoneResolverImpl::ProcessMatchingZonesL() - Entry" );
    
    if( ( KOne < iTempZoneIds.Count() ) && ( KErrNone == aLocTimeZones.Count() ) )
        {
        aTzId = iTempZoneIds[ FALSE ]->TimeZoneNumericID();
        iTempZoneIds.ResetAndDestroy();
        CleanupStack::PopAndDestroy( &aLocTimeZones );

        __PRINT( "aTzId: %d", aTzId );
        __PRINTS( "CClockTimeZoneResolverImpl::ProcessMatchingZonesL() - Exit" );

        return KErrNone;
        }

    iTempZoneIds.ResetAndDestroy();

    // If we don't have any localized zones at this point, we can do nothing
    if( KErrNone == aLocTimeZones.Count() )
        {        
        aTzId = KErrNotFound;
        CleanupStack::PopAndDestroy( &aLocTimeZones );

        __PRINT( "aTzId: %d", aTzId );
        __PRINTS( "CClockTimeZoneResolverImpl::ProcessMatchingZonesL() - Exit" );

        return KErrNotFound;
        }

    // If we have more than one localized zone at this point, we need to find the default one.
    TInt matchIndex( FALSE );
    for( TInt index( FALSE ); index < aLocTimeZones.Count(); index++ )
        {
        if( FALSE == aLocTimeZones.At( index ).StandardName().Compare( KDefault ) )
            {
            matchIndex = index;
            break; // We will take the first one we find.
            }
        }

    // This is the DST zone we are in.            
    aTzId = aLocTimeZones.At( matchIndex ).TimeZoneId();

    CleanupStack::PopAndDestroy( &aLocTimeZones );

    __PRINT( "aTzId: %d", aTzId );

    __PRINTS( "CClockTimeZoneResolverImpl::ProcessMatchingZonesL() - Exit" );

    return KErrNone;
    }
 
    
// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::GetCorrectZone
// rest of the details commented in the header
// ---------------------------------------------------------------------------
// 
  TInt CClockTimeZoneResolverImpl::GetCorrectZone( CTzLocalizedTimeZoneArray& aLocTimeZones, 
                       						  const TTimeIntervalMinutes& aTimeZoneOffset,
                       						  const TTimeIntervalMinutes& aDstOffset,
                       						  TInt& aTzId )
	{
	__PRINTS( "CClockTimeZoneResolverImpl::GetCorrectZone - Entry" );
     
	TInt returnVal( KErrNotFound );
	
   	// If we have no localised zones and network supports DST, we have done everything right, 
	// OR if UTC offset is invalid and we have a multizone country, we can do nothing.
	if( ( KErrNone == aLocTimeZones.Count() && KErrNotFound != aDstOffset.Int() ) ||
		( KOne < aLocTimeZones.Count() && KErrNotFound == aTimeZoneOffset.Int() ) )
		{
		aTzId = KErrNotFound;
		
		CleanupStack::PopAndDestroy( &aLocTimeZones );
		}    
	// Only one zone and DST indication supported by network. This is the dream case.
	else if( KOne == aLocTimeZones.Count() && KErrNotFound != aDstOffset.Int() )
		{
		// This is the easiest case, get the zone id and return correct zone
		aTzId = aLocTimeZones.At( FALSE ).TimeZoneId();
		returnVal = KErrNone;
		
		CleanupStack::PopAndDestroy( &aLocTimeZones );
		}

	
    __PRINTS( "CClockTimeZoneResolverImpl::GetCorrectZone - Exit" );
    
   	return returnVal;
   	}
   	
   	
// ---------------------------------------------------------------------------
// CClockTimeZoneResolverImpl::GetMatchingZonesL
// rest of the details commented in the header
// ---------------------------------------------------------------------------
//
TInt CClockTimeZoneResolverImpl::GetMatchingZonesL( CTzLocalizedTimeZoneArray* aLocTimeZones, TInt& aTzId )
	{
    // If we have only one matching zone, pick that up.
    if( KOne == iTempZoneIds.Count() )
        {
        aTzId = iTempZoneIds[ FALSE ]->TimeZoneNumericID();
        
        iTempZoneIds.ResetAndDestroy();
        CleanupStack::PopAndDestroy( aLocTimeZones );
        
       	__PRINT( "aTzId: %d", aTzId);
        
        __PRINTS( "CClockTimeZoneResolverImpl::GetMatchingZonesL - Exit" );
        
        return KErrNone;
        }        

    // If we have more zones, we will process them further.
    else if( KOne < iTempZoneIds.Count() )
        {
        
        for( TInt index( NULL ); index < aLocTimeZones->Count(); index++ )
        	{
        	aLocTimeZones->Remove( index );
        	}
        
        CTzLocalizedTimeZone* timezone( NULL );
        CTzLocalizer* localizer = CTzLocalizer::NewL();
        CleanupStack::PushL( localizer );

		// Get the localized timezones for each zone that matches.
        for( TInt zoneIndex( FALSE ); zoneIndex < iTempZoneIds.Count(); zoneIndex++ )
           {
            TRAPD( errVal,
            	timezone = localizer->GetLocalizedTimeZoneL( iTempZoneIds[ zoneIndex ]->TimeZoneNumericID() ) );
            if( KErrNone == errVal && timezone )
                {
                aLocTimeZones->AppendL( timezone );
                }                
            }
        CleanupStack::PopAndDestroy( localizer );
        } 
        
    __PRINTS( "CClockTimeZoneResolverImpl::GetMatchingZonesL - Exit" );
        
    return KErrNotFound;
	}
	
// End of file
