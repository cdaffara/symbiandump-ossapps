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
* Description:   The source file for the CClockNitzPluginImpl class.
*
*/

// System includes
#include <e32const.h>
#include <e32base.h>

// User includes
#include "clocknitzplugin.h"
#include "clocknitzpluginimpl.h"
#include "clockpluginobserver.h"
#include "clock_debug.h"

// Constants
const TInt KQuaterHour( 15 );
const TInt KMinute( 60 );
const TInt KTimeZoneValidBit( 0x3F );
const TInt KTimeZoneSignBit( 0x80 );

// ---------------------------------------------------------
// CClockNitzPluginImpl::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzPluginImpl* CClockNitzPluginImpl::NewL( CClockNitzPlugin* aNitzPlugin )
	{
	__PRINTS( "CClockNitzPluginImpl::NewL - Entry" );
	
	CClockNitzPluginImpl* self = new ( ELeave ) CClockNitzPluginImpl( aNitzPlugin );
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	CleanupStack::Pop();
	
	__PRINTS( "CClockNitzPluginImpl::NewL - Exit" );
	
	return self;
	}
	
// ---------------------------------------------------------
// CClockNitzPluginImpl::~CClockNitzPluginImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzPluginImpl::~CClockNitzPluginImpl()
	{
	__PRINTS( "CClockNitzPluginImpl::~CClockNitzPluginImpl - Entry" );
	
	if( iNitzSrv )
		{
		iNitzSrv->Stop();
		delete iNitzSrv;
		}
	
	__PRINTS( "CClockNitzPluginImpl::~CClockNitzPluginImpl - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzPluginImpl::CClockNitzPluginImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzPluginImpl::CClockNitzPluginImpl( CClockNitzPlugin* aNitzPlugin )
						                	: iNitzPlugin( aNitzPlugin )
	{
	__PRINTS( "CClockNitzPluginImpl::CClockNitzPluginImpl - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClockNitzPluginImpl::CClockNitzPluginImpl - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzPluginImpl::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPluginImpl::ConstructL()
	{
	__PRINTS( "CClockNitzPluginImpl::ConstructL - Entry" );
	
	// Construct the listener.
	iNitzSrv = CClockNitzListener::NewL( this );
	// Start the listener.
	iNitzSrv->Start();
	
	__PRINTS( "CClockNitzPluginImpl::ConstructL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzPluginImpl::GetTimeInformationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPluginImpl::GetTimeInformationL( TInterfaceAttribute aInterfaceAttribute, 
											  	STimeAttributes* aTimeAttributes )
	{
	__PRINTS( "CClockNitzPluginImpl::GetTimeInformationL - Entry" );
	
	__PRINT( "TInterfaceAttribute: %d", aInterfaceAttribute );
	
	if( !aTimeAttributes )
	    {
	    __PRINTS( "Time attributes holder is NULL" );
	    
	    return;
	    }
	
	TInt timeZone( NULL );
	
	STimeAttributes* nitzData( aTimeAttributes );
	
	TInt retVal( KErrNone );
	if( iNitzSrv )
	    {
	    // Get the NITZ information from the listener.
	    retVal = iNitzSrv->GetCurrentNITZInfoL( iNitzInfo );
	    }
	
	if( KErrNotFound == retVal )
	    {
	    __PRINTS( "Unable to get NITZ information" );
	    
	    return;
	    }
	
	// Get the capabilities sent by the n/w.
	TInt32 nitzCaps( iNitzInfo.iNitzFieldsUsed );
	
	switch( aInterfaceAttribute )
		{
		case EUTCTime:
		    {
			// Fetch UTC time.
			if( nitzCaps & RMobilePhone::KCapsTimeAvailable )
				{
				// We have a valid time.
				nitzData->iUtcDateTime.Set( iNitzInfo.Year(),
		            	  					TMonth( iNitzInfo.Month() ),
						  					iNitzInfo.Day(),
							  				iNitzInfo.Hour(),
							  				iNitzInfo.Minute(),
							  				iNitzInfo.Second(),
							  				iNitzInfo.MicroSecond() );
				}
			else
				{
				TTime invalidTime( -1 );
				nitzData->iUtcDateTime = invalidTime.DateTime();
				}
		    }
			break;
			
		case EDSTOffset:
		    {
			// Check if DST is available in the NITZ packet.
			if( nitzCaps & RMobilePhone::KCapsDSTAvailable )
				{
				// DST is supported by the n/w.
				nitzData->iDstOffset = iNitzInfo.iDST * KMinute;
				}
			else
				{
				// Reset the DST data.
				nitzData->iDstOffset = TTimeIntervalMinutes( -1 );
				}
		    }
			break;
			
		case ETZOffset:
		    {
			// Calculate std offset
			// We only accept a NitzInfo with time and zone info. We are very picky
		    if( nitzCaps & RMobilePhone::KCapsTimezoneAvailable )
		    	{
				timeZone = iNitzInfo.iTimeZone & KTimeZoneValidBit;

	        	if( iNitzInfo.iTimeZone & KTimeZoneSignBit )
	            	{
	            	// Changes sign
	            	timeZone = -timeZone;
	            	}
			
				// Calculate standard UTC offset
				nitzData->iTimeZoneOffset = timeZone * KQuaterHour;
				
				// Check if dst is available. If so, the standard offset above will already have
				// DST offset included. We've to remove it. The following check tells if dst is
				// available.
				if( nitzCaps & RMobilePhone::KCapsDSTAvailable )
					{
					// Deduct the DST offset from the standard offset.
					nitzData->iTimeZoneOffset = timeZone * KQuaterHour
												- iNitzInfo.iDST * KMinute;
					}
		    	}
		    else
		    	{
		    	// Invalid timezone. Reset the offset.
				nitzData->iTimeZoneOffset = TTimeIntervalMinutes( -1 );
		    	}
		    }
			break;
			
		default:
		    {
		    // No implementation yet.
		    }
			break;			
		}
	
	__PRINTS( "CClockNitzPluginImpl::GetTimeInformationL - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzPluginImpl::NotifyOnNITZInfoChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//		
void CClockNitzPluginImpl::NotifyOnNITZInfoChange( TInterfaceAttribute /*aInterfaceAttribute*/ )
	{
	__PRINTS( "CClockNitzPluginImpl::NotifyOnNITZInfoChange - Entry" );
	
	// Start the listener to get NITZ notifications.
	if( iNitzSrv )
	    {
	    iNitzSrv->Start();
	    }
	
	__PRINTS( "CClockNitzPluginImpl::NotifyOnNITZInfoChange - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzPluginImpl::NotifyNITZInfoChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPluginImpl::NotifyNITZInfoChangeL()
	{
	__PRINTS( "CClockNitzPluginImpl::NotifyNITZInfoChange - Entry" );
	
	// Notify the plugin that NITZ info has changed.
	if( iNitzPlugin )
	    {
	    iNitzPlugin->NotifyNITZInfoChangeL();
	    }
	
	__PRINTS( "CClockNitzPluginImpl::NotifyNITZInfoChange - Exit" );
	}

// End of file
