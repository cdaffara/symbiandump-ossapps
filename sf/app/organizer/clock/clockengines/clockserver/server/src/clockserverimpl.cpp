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
* Description:   The source file of the CClkSrvImpl class. 
*
*/

// System includes
#include <tz.h>
#include <centralrepository.h>
#include <starterdomaincrkeys.h>
#include <ecom.h>

// User includes
#include "clockserverimpl.h"
#include "clockserver.h"
#include "clocktimesourceinterface.h"
#include "clocktimesourceinterface.hrh"
#include "clockservermcclistener.h"
#include "clocktimezoneresolver.h"
#include "clockservermccobserver.h"
#include "clockecomlistener.h"
#include "clock_debug.h"

// Constants
const TInt KZeroIndex( 0 );
const TInt KSingleZone( 1 );
const TInt KSinglePlugin( 1 );
const TUid KTimeSourceInterfaceUID = { 0x200159A7 };
const TInt KNitzPlugin( 0x200159A5 );

// ---------------------------------------------------------
// CClkSrvImpl::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvImpl* CClkSrvImpl::NewL( CClkSrvMain* aClkSrv )
    {
    __PRINTS( "CClkSrvImpl::NewL - Entry" );
    
    CClkSrvImpl* clkSrvImpl = new( ELeave ) CClkSrvImpl( aClkSrv );
    CleanupStack::PushL( clkSrvImpl );
    
    clkSrvImpl->ConstructL();
        
    CleanupStack::Pop( clkSrvImpl );
    
    __PRINTS( "CClkSrvImpl::NewL - Exit" );
    
    return clkSrvImpl;
    }

// ---------------------------------------------------------
// CClkSrvImpl::~CClkSrvImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvImpl::~CClkSrvImpl()
	{
	__PRINTS( "CClkSrvImpl::~CClkSrvImpl - Entry" );
	
	// Cleanup
	TInt objCount( iTimeSourceObjArray.Count() );
	
	// First iterate and destroy all the objects. 
	// Only removing from the array won't free the memory.
	for( TInt index ( KZeroIndex ); index < objCount; index++ )
		 {
		 CClockTimeSourceInterface* ifObject = iTimeSourceObjArray[ index ];

		 // Remove the object from the array.
		 iTimeSourceObjArray.Remove( index );
		
		 delete ifObject;
		 ifObject = NULL;
		 }
	
	// Close the handle to the array.
	iTimeSourceObjArray.Close();
	
	if( iMccListener )
		{
		iMccListener->Stop();
		delete iMccListener;
		iMccListener = NULL;
		}
	if( iTimeAttributes )
		{
		delete iTimeAttributes;
		iTimeAttributes = NULL;
		}
	if( iTzResolver )
		{
		delete iTzResolver;
		iTzResolver = NULL;
		}
	if( iClockEComListener )
	    {
	    delete iClockEComListener;
	    iClockEComListener = NULL;
	    }
	
	__PRINTS( "CClkSrvImpl::~CClkSrvImpl - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvImpl::CClkSrvImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvImpl::CClkSrvImpl( CClkSrvMain* aClkSrv ) : iClkSrv( aClkSrv ),
												   iMccReceived( EFalse ),
												   iPluginData( EFalse )
	{
	__PRINTS( "CClkSrvImpl::CClkSrvImpl - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClkSrvImpl::CClkSrvImpl - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvImpl::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvImpl::ConstructL()
	{
	__PRINTS( "CClkSrvImpl::ConstructL - Entry" );
	
	// Construct and start the mcc listener.
	iMccListener = CClkMccListener::NewL( this );
	
	if( iMccListener )
	    {
	    iMccListener->Start();
	    }
	
	// Create the resolver.
	iTzResolver = CClockTimeZoneResolver::NewL();
	
	// Create the ECOM listener.
	TRAP_IGNORE( iClockEComListener = CClockEComListener::NewL( *this ) );
	
	__PRINTS( "CClkSrvImpl::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvImpl::ActivateAllProtocols
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::ActivateAllProtocolsL()
	{
	__PRINTS( "CClkSrvImpl::ActivateAllProtocolsL All- Entry" );	
	TInt returnVal( KErrNone );
	// Array to return all implementations in an interface.
    RImplInfoPtrArray plugInArray;
    // Get the list of all implementations.
    REComSession::ListImplementationsL( KTimeSourceInterfaceUID, plugInArray );
    
    TInt count( plugInArray.Count() );
    // Activate all the given protocols.
    for( TInt index( KZeroIndex ); index < count; index++ )
		 {	
		 returnVal = ActivateProtocolL( plugInArray[ index ]->ImplementationUid().iUid );		 			
		 }
    // Cleanup.
    plugInArray.ResetAndDestroy();
    plugInArray.Close();
    
    __PRINTS( "CClkSrvImpl::ActivateAllProtocolsL All- Exit" );
    return returnVal;	
	}
// ---------------------------------------------------------
// CClkSrvImpl::ActivateProtocol
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::ActivateProtocolL( TInt aClkSrvProtocol )
	{
	__PRINTS( "CClkSrvImpl::ActivateProtocolL - Entry" );
	
	__PRINT( "The uid of the protocol being activated: %x", aClkSrvProtocol );
	
	TInt returnVal( KErrNone );
	
	// First we need to check if the plugin is already active. So we iterate through the 
	// array of plugins to see if the protocol implementation id matches with any of the 
	// plugin loaded. If not we load it.
	TBool alreadyActive( EFalse );
	
	// Number of plugins loaded.
	TInt pluginCount = iTimeSourceObjArray.Count();
	
	for( TInt index ( KZeroIndex ); index < pluginCount; index++ )
		{
		if( aClkSrvProtocol == iTimeSourceObjArray[ index ]->GetPluginUid() )
			  {
			  // We have a match, plugin is already loaded.
			  alreadyActive = ETrue;
			  break;
			  }
		}
	
	if( alreadyActive )
	    {
    	    // Plugin already loaded. So return.
    	    __PRINTS( "Plugin is already loaded." );    	    
    	    __PRINTS( "CClkSrvImpl::ActivateProtocolL - Exit" );
    	    
    	    return returnVal;
	    }
	
	if( !alreadyActive )
		  {
		  // Construct and append the object to the array.
		  CClockTimeSourceInterface* timeSourceObj( NULL );
		  timeSourceObj = CClockTimeSourceInterface::NewL( TUid::Uid( aClkSrvProtocol ),this );
		
		  if( !timeSourceObj )
			    {
					// The plugin could not be loaded.
		      __PRINTS( "The plugin could not be loaded." );    
		      __PRINTS( "CClkSrvImpl::ActivateProtocolL - Exit" );
		      
		      returnVal = KErrNotFound;
		      
		      return returnVal;
			    }
		  else
			   {
				 TInt automaticTimeUpdateState( KZeroIndex );
					
				 iClkSrv->GetActivePluginL( automaticTimeUpdateState );
				 // If the protocol is the default protocol for the device ,set it to On in cenrep.
				 if( !automaticTimeUpdateState || KNitzPlugin == aClkSrvProtocol )
					   {
					   iClkSrv->SetActivePluginL( ETrue );						
					   }
							
				 // The plugin was constructed.
				 CleanupStack::PushL( timeSourceObj );
					
				 // Append the constructed object to the array.
				 iTimeSourceObjArray.Append( timeSourceObj );
					
				 CleanupStack::Pop( timeSourceObj );
				 timeSourceObj = NULL;
				 }
		  }
	__PRINTS( "CClkSrvImpl::ActivateProtocolL - Exit" );
	
	return returnVal;
	}
	
// ---------------------------------------------------------
// CClkSrvImpl::IsProtocolActive
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::IsProtocolActive( TBool& aIsActive, TInt aClkSrvProtocol )
	{
	__PRINTS( "CClkSrvImpl::IsProtocolActive - Entry" );
	__PRINT( "%x", aClkSrvProtocol );
	
	TInt returnVal( KErrNone );
	
	aIsActive = EFalse;
	
	// Get the count of objects.
	TInt arrayCount = iTimeSourceObjArray.Count();
			
	// Browse the array of active plugins to check if the input plugin exists 
	for(TInt index ( KZeroIndex ); index < arrayCount; index++ )
		{
		if( aClkSrvProtocol == iTimeSourceObjArray[ index ]->GetPluginUid() )
			{
			// We have a match, plugin is loaded.
			aIsActive = ETrue;
						
			break;
			}
		}	
	
	__PRINTS( "CClkSrvImpl::IsProtocolActive - Exit" );
	
	return returnVal;
	}
// ---------------------------------------------------------
// CClkSrvImpl::DeActivateAllProtocols
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::DeActivateAllProtocolsL()
	{
	__PRINTS( "CClkSrvImpl::DeActivateAllProtocolsL All- Entry" );	
	
	TInt returnVal( KErrNone );
	// Array to return all implementations in an interface.
  RImplInfoPtrArray plugInArray;
  
  // Get the list of all implementations.
  REComSession::ListImplementationsL( KTimeSourceInterfaceUID, plugInArray ); 
    
  // Set the cendrep key for Automatic time update status to off.
  TInt automaticTimeUpdateState( KZeroIndex );
			
	iClkSrv->GetActivePluginL( automaticTimeUpdateState );
			
	if( automaticTimeUpdateState )
			{				
			iClkSrv->SetActivePluginL( EFalse );				
			}			
    
  // Deactivate all the given protocols.
  for( TInt index( KZeroIndex ); index < plugInArray.Count(); index++ )
	     {	
		   returnVal = DeActivateProtocolL( plugInArray[ index ]->ImplementationUid().iUid );		 			
		   }
  // Cleanup.
  plugInArray.ResetAndDestroy();
  plugInArray.Close();
    
  __PRINTS( "CClkSrvImpl::DeActivateAllProtocolsL All- Exit" );
  return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvImpl::DeActivateProtocolL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkSrvImpl::DeActivateProtocolL( TInt aClkSrvProtocol )
	{
	__PRINTS( "CClkSrvImpl::DeActivateProtocolL - Entry" );
	__PRINT( "The uid of the protocol being deactivated :%x", aClkSrvProtocol );
	
	TInt returnVal( KErrNotFound );
	
	// Get the count of objects.
	TInt arrayCount = iTimeSourceObjArray.Count();
	
	
		
	// Browse every active plugin object to find the match.
	for( TInt index( KZeroIndex ) ; index < arrayCount; index++ )
		 {
		 if( aClkSrvProtocol == iTimeSourceObjArray[ index ]->GetPluginUid() )
			 {
			 // set the state it to Off in cenrep
			 TInt automaticTimeUpdateState( KZeroIndex );						
			 iClkSrv->GetActivePluginL( automaticTimeUpdateState );
				
			 if( ( automaticTimeUpdateState ) && ( KSinglePlugin == arrayCount  ) )
				   {				
				   iClkSrv->SetActivePluginL( EFalse );				
				   }			
			 // We have a match, plugin is loaded. Now destroy it.
			 CClockTimeSourceInterface* ifObject = iTimeSourceObjArray[ index ];
			 CleanupStack::PushL( ifObject );
				
			 // Remove from the array.
			 iTimeSourceObjArray.Remove( index );
				
			 // Free the memory.
			 CleanupStack::PopAndDestroy( ifObject );
				
			 returnVal = KErrNone;
			 break;
			 }
		 }
	__PRINTS( "CClkSrvImpl::DeActivateProtocolL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvImpl::GetProtocolInformationL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::GetProtocolInformationL( STimeAttributes& aTimeInformation )
	{
	__PRINTS( "CClkSrvImpl::GetProtocolInformationL - Entry" );
	
	TInt returnValue( KErrNotFound );
	
	if( iTimeAttributes )
		{
		returnValue = KErrNone;
		
		// Assign the time information.
		aTimeInformation = *iTimeAttributes;
		}
	
	__PRINTS( "CClkSrvImpl::GetProtocolInformationL - Exit" );
	
	return returnValue;
	}

// ---------------------------------------------------------
// CClkSrvImpl::GetCurrentMccL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::GetCurrentMccL( TInt& aCurrentMcc )
	{
	__PRINTS( "CClkSrvImpl::GetCurrentMccL - Entry" );
	
	TInt returnValue( KErrNotFound );
	
	if( iMccReceived )
		{
		TLex currentMcc;
		currentMcc.Assign( iMcc );
		
		// Return the value
		currentMcc.Val( aCurrentMcc );
		
		returnValue = KErrNone;
		
		__PRINT( "Current MCC - %d", aCurrentMcc );
		}
	
	__PRINTS( "CClkSrvImpl::GetCurrentMccL - Exit" );
	
	return returnValue;
	}

// ---------------------------------------------------------
// CClkSrvImpl::GetCurrentTimeZoneIdL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::GetCurrentTimeZoneIdL( TInt& aCurrentTimeZoneId )
	{
	__PRINTS( "CClkSrvImpl::GetCurrentTimeZoneIdL - Entry" );
		
	TInt returnValue( KErrNotFound );

	if( 0 != iTimeZoneId )
		{
		// Assign the value
		aCurrentTimeZoneId = iTimeZoneId;
		
		returnValue = KErrNone;
		}

	__PRINT( "Current timezone ID: %d", iTimeZoneId );

	__PRINTS( "CClkSrvImpl::GetCurrentTimeZoneIdL - Exit" );

	return returnValue;
	}

// ---------------------------------------------------------
// CClkSrvImpl::IsAutoTimeUpdateOn
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvImpl::IsAutoTimeUpdateOn( TBool& aEnabled )
    {
    __PRINTS( "CClkSrvImpl::IsAutoTimeUpdateOn - Entry" );
    
    TInt returnValue( KErrNone );
    
    // If the array has any items, it means atleast one of the protocols is active.
    aEnabled = iTimeSourceObjArray.Count() ? ETrue : EFalse;
    
    __PRINTS( "CClkSrvImpl::IsAutoTimeUpdateOn - Exit" );
    
    return returnValue;
    }

// ---------------------------------------------------------
// CClkSrvImpl::NotifyTimeChangeL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvImpl::NotifyTimeChangeL( CClockTimeSourceInterface& aPluginImpl )
	{
	__PRINTS( "CClkSrvImpl::NotifyTimeChangeL - Entry" );
	
	// Construct the time attributes holder struct object.
	if( !iTimeAttributes )
		{
		TRAPD( errVal, iTimeAttributes = new( ELeave ) STimeAttributes );
		if( KErrNone != errVal )
		    {
		    __PRINTS( "Could not construct time attributes holder" );
		    
		    __PRINTS( "CClkSrvImpl::NotifyTimeChangeL - Exit" );
		    
		    return;
		    }
		}
	
	// Initialise the attributes to null.
	TTime invalidTime( 0 );
	iTimeAttributes->iDstOffset = TTimeIntervalMinutes( 0 );
	iTimeAttributes->iTimeZoneOffset =  TTimeIntervalMinutes( 0 );
	iTimeAttributes->iUtcDateTime = invalidTime.DateTime();
	// Get all the information related to the plugin.
	// The UTC time.
	TRAP_IGNORE( aPluginImpl.GetTimeInformationL( EUTCTime, iTimeAttributes ) );	
	// The default offset.
	TRAP_IGNORE( aPluginImpl.GetTimeInformationL( ETZOffset, iTimeAttributes ) );
	// The dst offset.
	TRAP_IGNORE( aPluginImpl.GetTimeInformationL( EDSTOffset, iTimeAttributes ) );
	
	// Set a flag saying we have plugin data.
	iPluginData = ETrue;
	
	if( iMccReceived )
		{
		__PRINTS( "We have already recieved the MCC" );
		
		// Try to resolve the timezone id with the data that we have recieved.
		TInt timezoneId;
		TInt errorVal = 0;
		TRAP_IGNORE( errorVal = iTzResolver->GetTimeZoneL( *iTimeAttributes, iMcc, timezoneId ) );
		
		__PRINT( "CClkSrvImpl::NotifyTimeChangeL - timeZoneId: %d", timezoneId );
		
		__PRINT( "NotifyTimeChangeL::timezoneId : %d", timezoneId);
		
		// If a match is found, set the timezone and the time.
		if( KErrNone == errorVal )
			{
			// Save the timezone Id.
			iTimeZoneId = timezoneId;
			
			RTz tz;
			User::LeaveIfError( tz.Connect() );
			CleanupClosePushL( tz );

			// Get current DST zone Id
			CTzId* currentCTzId = tz.GetTimeZoneIdL();
			CleanupStack::PushL( currentCTzId );

			if( KInvalidTimeZoneId != timezoneId ) 
				{
                // A valid new zone was found successfully
				CTzId* newTzId = CTzId::NewL( timezoneId );
				CleanupStack::PushL( newTzId );

				__PRINT( "NotifyTimeChangeL::currentCTzId : %d", currentCTzId->TimeZoneNumericID() );

				__PRINT( "NotifyTimeChangeL::newTzId : %d", newTzId->TimeZoneNumericID() );

				if( *newTzId != *currentCTzId )
					{
					// The new zone is different than the current one
					// GOAL 3: Set the DST zone of the device
					__PRINTS( "Timezone ID is different. Setting the zone." );
						
					TRAP_IGNORE( tz.SetTimeZoneL( *newTzId ) );
					}

				CleanupStack::PopAndDestroy( newTzId );
				}
			
			CleanupStack::PopAndDestroy( currentCTzId );
			CleanupStack::PopAndDestroy( &tz );

			// Time sent by nw is UTC
			TTime nwUtcTime( iTimeAttributes->iUtcDateTime );

			// Set the UTC time only. This is being done because with the UTC time,
			// before the time is being set, the dst properties for the timezone are being checked.
			// If its not the first boot, then set the time.
			__PRINTS( "Setting the UTC time." );
				
			TRAP_IGNORE( User::SetUTCTime( nwUtcTime ) );
			}
		
		// Reset the flags.
		iPluginData = EFalse;
		}
	else
		{
		__PRINTS( "MCC has not yet been recieved. Trying to resolve with offset only." );
		
		// Handle the case.
		// 1. Use the plugin data and set the time.
		// 2. Use the offset in the plugindata or any available to deduce the timezone id.
		
		// Try to resolve the timezone id with the data that we have recieved.
		TInt timezoneId;
		TInt errorVal = 0;
		const TBuf< 4 > invalidMCC( KInvalidMCC );
		TRAP_IGNORE( errorVal = iTzResolver->GetTimeZoneL( *iTimeAttributes, invalidMCC, timezoneId ) );
		
		if( KErrNone == errorVal )
			{
			RTz tz;
			User::LeaveIfError( tz.Connect() );
			CleanupClosePushL( tz );

			// Get current DST zone Id
			CTzId* currentCTzId = tz.GetTimeZoneIdL();
			CleanupStack::PushL( currentCTzId );
			
			if( KInvalidTimeZoneId != timezoneId ) 
				{
                // A valid new zone was found successfully
				CTzId* newTzId = CTzId::NewL( timezoneId );
				CleanupStack::PushL( newTzId );

				if( *newTzId != *currentCTzId )
					{
					// The new zone is different than the current one
					// GOAL 3: Set the DST zone of the device
					// If firstboot then don't set the time.
					__PRINTS( "The timezone ID is different. Setting the zone." );
						
					TRAP_IGNORE( tz.SetTimeZoneL( *newTzId ) );
					}
				CleanupStack::PopAndDestroy( newTzId );
				}
			CleanupStack::PopAndDestroy( currentCTzId );
			CleanupStack::PopAndDestroy( &tz );
			
		
			// Set the home time only and leave the zone as it is.
			TTime nwUtcTime( iTimeAttributes->iUtcDateTime );

			// Set the UTC time only. This is being done because with the UTC time,
			// before the time is being set, the dst properties for the timezone are being checked.
			// Set the time only if its not the first boot.
			__PRINTS( "Setting the UTC time." );

			TRAP_IGNORE( User::SetUTCTime( nwUtcTime ) );
			}
		}
	__PRINTS( "Notifying the timechange to client" );
	
	// Notify the sessions about the change.
	iClkSrv->NotifyAboutChange( EComponentTime, aPluginImpl.GetPluginUid(), KErrNone );
		
	__PRINTS( "CClkSrvImpl::NotifyTimeChangeL - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvImpl::NotifyMccChange()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvImpl::NotifyMccChangeL()
	{
	__PRINTS( "CClkSrvImpl::NotifyMccChangeL - Entry" );
	
	// We have the mobile country code. Get it from the listener.
	iMccListener->GetCurrentMcc( iMcc );
	
	// Set mcc received flag.
	iMccReceived = ETrue;

	// We should change the timezone only if user has chosen to update time automatically.
	if( iTimeSourceObjArray.Count() )
		{
		__PRINTS( "Automatic time update is ON. Trying to resolve the timezone." );
		
		// We have atleast one protocol active.
		// Lets see if we can narrow down to a single timezone with the MCC recieved.
		RArray< CTzId > tzIdArray;
		
		// This parte of code introduced due to error ID EASH-82DPPC(3G Tests for NITZ and GPRS Interaction)
		// Fix is bascially to narrowing down to single timezone id if NITZ packet is received prior to MCC.
		// In first IF state we are trying to narrowdown to single timezone ID, if not possible try to use MCC to get timezone
		// In second IF , we are using MCC to narrow down to single timezone ID directly as dont have NITZ info.
		if( iTimeAttributes )
		{
            __PRINTS("timeatrrtibutes has value");
		
			TTime invalidTime(0);
			if( iTimeAttributes->iDstOffset != TTimeIntervalMinutes( 0 ) &&
			iTimeAttributes->iTimeZoneOffset != TTimeIntervalMinutes( 0 ) &&
			iTimeAttributes->iUtcDateTime.Year() != invalidTime.DateTime().Year() )
			{
				// Try to resolve the timezone id with the data that we have recieved.
				TInt timezoneId;
				TRAP_IGNORE( TInt errorVal = iTzResolver->GetTimeZoneL( *iTimeAttributes, iMcc, timezoneId ) );
				
				__PRINT("TIMEZONE ID %d", timezoneId );

				// Append the timezone id to array.

				CTzId* matchingDSTZoneId = CTzId::NewL( timezoneId );
				CleanupStack::PushL( matchingDSTZoneId );

				tzIdArray.AppendL( *matchingDSTZoneId );

				CleanupStack::PopAndDestroy( matchingDSTZoneId );

				matchingDSTZoneId = NULL;
			}
			else
			{
				__PRINTS("NOT ABLE TO NARROW DOWN TO TIMEZONE ID WITH RECEIVED NITZ HENCE TRY WITH MCC");
				// Not able to narrow down to single timezone id with received NITZ packet hence try with MCC.
				TRAP_IGNORE( iTzResolver->TzIdFromMccL( iMcc, tzIdArray, KInvalidTimeZoneId ) );	
			}
		}
		else
		{
			__PRINTS("NO NITZ INFO HENCE TRY WITH MCC");
			// No NITZ info hecne try with MCC to get the time zone Id.
			TRAP_IGNORE( iTzResolver->TzIdFromMccL( iMcc, tzIdArray, KInvalidTimeZoneId ) );
		}
		
		// A single matching timezone was found. Set it as the default one.
		if( KSingleZone == tzIdArray.Count() )
			{
			RTz tz;
			User::LeaveIfError( tz.Connect() );
			CleanupClosePushL( tz );
	
			// Get current DST zone Id
			CTzId* currentCTzId = tz.GetTimeZoneIdL();
			CleanupStack::PushL( currentCTzId );
	
			// Fetch the first timezone
			TInt timezoneID = tzIdArray[ KZeroIndex ].TimeZoneNumericID();
	
			if( KInvalidTimeZoneId != timezoneID ) 
				{
				// A valid new zone was found successfully
				CTzId* newTzId = CTzId::NewL( timezoneID );
				CleanupStack::PushL( newTzId );
				
				iTimeZoneId = timezoneID ;
	
				if( *newTzId != *currentCTzId ) 
					{
					// The new zone is different than the current one
					// Set the DST zone of the device
					// Set the zone only for subsequent boots.
					__PRINTS( "The timezone ID is different. Setting the zone." );
						
					TRAP_IGNORE( tz.SetTimeZoneL( *newTzId ) );
					}
				CleanupStack::PopAndDestroy( newTzId );
				}
			CleanupStack::PopAndDestroy( currentCTzId );
			CleanupStack::PopAndDestroy( &tz );		
	
		}
		
		// Set the time on device and set the attributes to null so that we avoid setting old value.		
		if( iTimeAttributes )
		    {
            // Time sent by nw is UTC
            TTime nwUtcTime( iTimeAttributes->iUtcDateTime);
    
            // Set the UTC time only. This is being done because with the UTC time,
            // before the time is being set, the dst properties for the timezone are being checked.
            // If its not the first boot, then set the time.
            __PRINTS( "Setting the UTC time." );
    
            TRAP_IGNORE( User::SetUTCTime( nwUtcTime ) );
    
            // Setting the attributes to null again as we dont trust on
            // previous NITZ data recevied by device.If device receive
            // NITZ data again attributes would get filled with actual value.
            // Has been kept outside because this statement is valid for
            // first boot also.
    
            iTimeAttributes->iDstOffset = TTimeIntervalMinutes( 0 );
            iTimeAttributes->iTimeZoneOffset =  TTimeIntervalMinutes( 0 );
            TTime invalidTime( 0 );
            iTimeAttributes->iUtcDateTime = invalidTime.DateTime(); 
            }
		}
	
	// Notify the sessions about the change in mcc.
	iClkSrv->NotifyAboutChange( EComponentMcc, 0, KErrNone );
	
	__PRINTS( "CClkSrvImpl::NotifyMccChangeL - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvImpl::NotifyPluginInstallUninstallL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvImpl::NotifyPluginInstallUninstallL( const CImplementationInformation& aImplInfo, TBool /*aInstalled*/ )
    {
    __PRINTS( "CClkSrvImpl::NotifyPluginInstallUninstallL - Entry" );
    
    // Get the plugin UID which was modified.
    TUid pluginUid( aImplInfo.ImplementationUid() );
    TBool pluginActive( EFalse );
    
    // Check if the protocol was already active.
    IsProtocolActive( pluginActive, pluginUid.iUid );
    
    // If the plugin was not already active, then we dont have to bother,
    // since the clockserver state is not affected by the change.
    if( !pluginActive )
        {
        __PRINTS( "The plugin is not active. Check if Automatic time update is on,if True activate the plugin." );
        
        TBool timeUpdateOn( EFalse );
        IsAutoTimeUpdateOn( timeUpdateOn );
        if( timeUpdateOn )
            {
            ActivateProtocolL( pluginUid.iUid );	
            }
        
        __PRINTS( "CClkSrvImpl::NotifyPluginInstallUninstallL - Exit" );
        
        return;
        }
    
    // If the plugin was active, then deactivate it so that we dont have any older references.
    TInt returnVal( DeActivateProtocolL( pluginUid.iUid ) );
    
    if( KErrNone == returnVal )
    	{
    	__PRINTS( "Closing the ECom session" );
    	
    	// Close the REComSession. This is added as a fix for the CClockEcomResolver component where
    	// call to this ensures that the plugin and its associated DLL is removed from the memory.
    	REComSession::FinalClose();
    	}
    
    // Activate the protocol again. This is required because - 
    // 1. If the plugin was a upgrade and it was uninstalled, we need to activate the older version.
    // 2. If a new plugin was installed, we need to activate the newer version of the plugin.
    ActivateProtocolL( pluginUid.iUid );

    __PRINTS( "CClkSrvImpl::NotifyPluginInstallUninstallL - Exit" );
    }

// End of file
