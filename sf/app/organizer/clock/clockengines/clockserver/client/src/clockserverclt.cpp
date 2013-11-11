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
* Description:   The source file of the RClkSrvInterface class
*
*/

// System includes
#include <f32file.h>

// User includes
#include "clockserverclt.h"
#include "clockservercommon.h"
#include "clockservercmds.h"
#include "clocktimesourceinterface.hrh"
#include "clock_debug.h"

// Constants
const TInt KClockServerMajorVN( 1 );
const TInt KClockServerMinorVN( 0 );
const TInt KClockServerBuildVN( 1 );
const TInt KOneSecond = 1000000;
const TInt KMaxRetryTimes = 3;

// Literals
_LIT( KPanicLabel, "RClkSrvSession:Panic" );
_LIT( KCommand, "");
_LIT(KDC_PROGRAMS_DIR,"\\sys\\bin\\");
_LIT( KClockServer, "Z:clockserver.exe");

// ---------------------------------------------------------
// RClkSrvInterface::RClkSrvInterface
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C RClkSrvInterface::RClkSrvInterface() : iWhatChanged( NULL, NULL, NULL ),
										 		iWhoChanged( NULL, NULL, NULL )
	{
	__PRINTS( "RClkSrvInterface::RClkSrvInterface - Entry" );
	
    __PRINTS( "RClkSrvInterface::RClkSrvInterface - Exit" );
	}

// ---------------------------------------------------------
// RClkSrvInterface::Connect
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::Connect()
	{
	__PRINTS( "RClkSrvInterface::Connect - Entry" );

	TInt errVal( KErrNone );

	// Create the session to the mentioned server.
	errVal = CreateSession( KNameClockServer, Version(), KClockServerMessageSlots );

	// Try to start the server and then try to create session.
	if( KErrNone != errVal )
		{
		__PRINT( "RClkSrvInterface::Connect - Server has not been started - error code : %d", errVal );
		
		TRAP_IGNORE( StartClockSrvL() );

		// Wait till server gets started.
		TInt seconds(0);
		while ( KErrNone != errVal )
			{
			errVal = CreateSession( KNameClockServer, Version(), KClockServerMessageSlots );
			User::After( KOneSecond );
			seconds++;
			if ( KMaxRetryTimes == seconds )
				{
				errVal = KErrServerTerminated;
				break;
				}
			}

		}

    __PRINTS( "RClkSrvInterface::Connect - Exit" );
	
	return errVal;
}

// ---------------------------------------------------------
// RClkSrvInterface::Version
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TVersion RClkSrvInterface::Version() const
	{
	__PRINTS( "RClkSrvInterface::Version - Entry" );
	
    __PRINTS( "RClkSrvInterface::Version - Exit" );

	return TVersion( KClockServerMajorVN, KClockServerMinorVN, KClockServerBuildVN );
	}
	
// ---------------------------------------------------------
// RClkSrvInterface::ActivateProtocol
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::ActivateProtocol( TInt aClkSrvProtocol )
	{
	__PRINTS( "RClkSrvInterface::ActivateProtocol - Entry" );
	__PRINT( "%x", aClkSrvProtocol );
	
    __PRINTS( "RClkSrvInterface::ActivateProtocol - Exit" );

	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvActivateProtocol, TIpcArgs( aClkSrvProtocol ) );
	}

// ---------------------------------------------------------
// RClkSrvInterface::ActivateAllProtocols
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::ActivateAllProtocols()
	{
	__PRINTS( "RClkSrvInterface::ActivateAllProtocols - Entry" );
	
    __PRINTS( "RClkSrvInterface::ActivateAllProtocols - Exit" );

	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvActivateProtocol, TIpcArgs() );
	}
	
// ---------------------------------------------------------
// RClkSrvInterface::IsProtocolActive
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::IsProtocolActive( TInt aClkSrvProtocol, TBool& aActive )
	{
	__PRINTS( "RClkSrvInterface::IsProtocolActive - Entry" );
	__PRINT( "%x", aClkSrvProtocol );
	
	TPckg< TBool > boolArg( aActive );

    __PRINTS( "RClkSrvInterface::IsProtocolActive - Exit" );

	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvIsProtocolActive, TIpcArgs( aClkSrvProtocol, &boolArg ) );
	}	

// ---------------------------------------------------------
// RClkSrvInterface::DeActivateProtocol()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::DeActivateProtocol( TInt aClkSrvProtocol )
	{
	__PRINTS( "RClkSrvInterface::DeActivateProtocol() - Entry" );
	__PRINT( "%x", aClkSrvProtocol );
	
    __PRINTS( "RClkSrvInterface::DeActivateProtocol() - Exit" );
    
	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvDeactivateProtocol, TIpcArgs( aClkSrvProtocol ) );
	}

// ---------------------------------------------------------
// RClkSrvInterface::DeActivateAllProtocols()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::DeActivateAllProtocols()
	{
	__PRINTS( "RClkSrvInterface::DeActivateAllProtocols() - Entry" );
	
    __PRINTS( "RClkSrvInterface::DeActivateAllProtocols() - Exit" );
    
	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvDeactivateProtocol, TIpcArgs() );
	}

// ---------------------------------------------------------
// RClkSrvInterface::GetProtocolInfo()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::GetProtocolInfo( TInt aClkSrvProtocol, STimeAttributes& aTimeAttribute )
	{
	__PRINTS( "RClkSrvInterface::GetProtocolInfo() - Entry" );
	
	TPckg< STimeAttributes > timeAttributeArg( aTimeAttribute );
		
	__PRINTS( "RClkSrvInterface::GetProtocolInfo() - Exit" );
	
	// Send the message and receive completion status (synchronous)
	return SendReceive( EClkSrvGetProtocolInfo, TIpcArgs( aClkSrvProtocol, &timeAttributeArg ) );
	}

// ---------------------------------------------------------
// RClkSrvInterface::GetCurrentMcc()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::GetCurrentMcc( TInt& aCurrentMcc )
	{
	__PRINTS( "RClkSrvInterface::GetCurrentMcc() - Entry" );
	
	TPckg< TInt > aCurrentMccBuf( aCurrentMcc );
	
	TInt returnVal( SendReceive( EClkSrvGetCurrentMcc, TIpcArgs( &aCurrentMccBuf ) ) );
	
	__PRINT( "Current MCC - %d", aCurrentMcc );
	
	__PRINTS( "RClkSrvInterface::GetCurrentMcc() - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// RClkSrvInterface::GetCurrentTimeZondId()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::GetCurrentTimeZondId( TInt& aCurrentTimeZoneId )
	{
	__PRINTS( "RClkSrvInterface::GetCurrentTimeZondId() - Entry" );
	
	TPckg< TInt > aCurrentTzIdBuf( aCurrentTimeZoneId );
	
	TInt returnVal( SendReceive( EClkSrvGetCurrentTimeZoneId, TIpcArgs( &aCurrentTzIdBuf ) ) );
	
	__PRINT( "Current timezone ID - %d", aCurrentTimeZoneId );
	
	__PRINTS( "RClkSrvInterface::GetCurrentTimeZondId() - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// RClkSrvInterface::NotifyOnChange()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C void RClkSrvInterface::NotifyOnChange( TInt32& aWhatChanged, TInt32& aWhoChanged, TRequestStatus& aStatus )
	{
	__PRINTS( "RClkSrvInterface::NotifyOnChange() - Entry" );

	iWhatChanged.Set( ( TUint8* )&aWhatChanged, sizeof( aWhatChanged ), sizeof( aWhatChanged ) );
	iWhoChanged.Set( ( TUint8* )&aWhoChanged, sizeof( aWhoChanged ), sizeof( aWhoChanged ) );
	
	// Send the message and receive completion status (asynchronous)
	SendReceive( EClkSrvNotifyOnChange, TIpcArgs( &iWhatChanged, &iWhoChanged ), aStatus );
	
	__PRINTS( "RClkSrvInterface::NotifyOnChange() - Exit" );
	}

// ---------------------------------------------------------
// RClkSrvInterface::NotifyOnChange()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C void RClkSrvInterface::NotifyOnChangeCancel()
	{
	__PRINTS( "RClkSrvInterface::NotifyOnChangeCancel() - Entry" );
	
	// Send the message and receive completion status (synchronous)
	TInt returnVal = SendReceive( EClkSrvCancelNotifyOnChange, TIpcArgs() );
	
	__ASSERT_ALWAYS( KErrNone == returnVal || KErrServerTerminated,
                                    Panic( EClkSrvCltNotifyChangeCancel ) );
	
	__PRINTS( "RClkSrvInterface::NotifyOnChangeCancel - Exit" );
	}

// ---------------------------------------------------------
// RClkSrvInterface::IsAutoTimeUpdateOn
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
EXPORT_C TInt RClkSrvInterface::IsAutoTimeUpdateOn( TBool& aEnabled )
    {
    __PRINTS( "RClkSrvInterface::IsAutoTimeUpdateOn - Entry" );

    TPckg< TBool > boolArg( aEnabled );

    __PRINTS( "RClkSrvInterface::IsAutoTimeUpdateOn - Exit" );
    
    // Send the message and receive completion status (synchronous)
    return SendReceive( EClkSrvIsAutoTimeUpdadeOn, TIpcArgs( &boolArg ) );
    }

// ---------------------------------------------------------
// RClkSrvInterface::IsClockSrvRunning() const
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
TInt RClkSrvInterface::IsClockSrvRunning() const
	{
	__PRINTS( "RClkSrvInterface::IsClockSrvRunning - Entry" );
	
	TInt retVal( KErrNone );
	TFindServer findServer( KNameClockServer );
	TFullName name;
	if ( KErrNone != findServer.Next( name ) )
		{
		__PRINTS( "RClkSrvInterface::Server is not running - Entry" );
		
		RSemaphore semaphore;
		// Waites for Starter to start the nitz server for max. 3 seconds.
		for ( TInt i( 0 ); i < KMaxRetryTimes; i++ )
			{
			retVal = semaphore.OpenGlobal( KNameClockServerStartSemaphore );
			if ( KErrNone == retVal )
				{
				__PRINTS( "RClkSrvInterface::Server is now running - Entry" );
				
				semaphore.Close();
				break;
				}
			User::After( KOneSecond );
			}
		}
		
	__PRINTS( "RClkSrvInterface::IsClockSrvRunning - Exit" );
	
	return retVal;
	}

// ---------------------------------------------------------
// RClkSrvInterface::StartClockSrvL()
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
void RClkSrvInterface::StartClockSrvL()
	{
	__PRINTS( "RClkSrvInterface::StartClockSrvL - Entry" );
	
	if ( KErrNone != IsClockSrvRunning( ) )
        {
		__PRINTS( "RClkSrvInterface::StartClockSrvL:IsClockSrvRunning - Entry" );
		
		TParse fileparser;
		fileparser.Set( KClockServer, &KDC_PROGRAMS_DIR, NULL );

		RProcess ClockSrvProc;
		CleanupClosePushL( ClockSrvProc );        
		User::LeaveIfError( ClockSrvProc.Create( fileparser.FullName( ), KCommand,
												 TUidType( KNullUid, KNullUid, KClockSrvUid ) ) ); 


		ClockSrvProc.Resume();
		CleanupStack::PopAndDestroy( &ClockSrvProc );
	}
		
	__PRINTS( "RClkSrvInterface::StartClockSrvL - Exit" );
	}

// ---------------------------------------------------------
// Panic
// rest of the details are commented in the header.
// ---------------------------------------------------------
//	
GLDEF_C void Panic( TClkSrvSessionPanic aPanic )
    {
    User::Panic( KPanicLabel, aPanic );
    }
    
// End of file
