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
* Description:   The source file of the CClockNitzListener class.
*
*/

// System includes
#include <e32std.h>
#include <etelmm.h>
#include <e32property.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include <mmtsy_names.h>

// User includes
#include "clocknitzlistener.h"
#include "clocknitzpluginimpl.h"
#include "clock_debug.h"

// Constants
const TInt KZerothItem( 0 );

// ---------------------------------------------------------
// CClockNitzListener::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzListener* CClockNitzListener::NewL( CClockNitzPluginImpl* aObserver )
    {
    __PRINTS( "CClockNitzListener::NewL - Entry" );
	
    CClockNitzListener* nitzListener = new( ELeave ) CClockNitzListener( aObserver );
    CleanupStack::PushL( nitzListener );
    
    nitzListener->ConstructL();
    
    CleanupStack::Pop( nitzListener );
    
    __PRINTS( "CClockNitzListener::NewL - Exit" );
    
    return nitzListener;
    }

// ---------------------------------------------------------
// CClockNitzListener::CClockNitzListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzListener::~CClockNitzListener()
    {
    __PRINTS( "CClockNitzListener::~CClockNitzListener - Entry" );
    
    // Cancel any outstanding requests.
    Cancel();
    // Close the ETel Server and RMobilePhone sessions.
    iPhone.Close();
    iEtelServer.Close();
    
    __PRINTS( "CClockNitzListener::~CClockNitzListener - Exit" );
    }

// ---------------------------------------------------------
// CClockNitzListener::Start
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::Start()
	{
	__PRINTS( "CClockNitzListener::Start - Entry" );
	
	// Set the object active and request for NITZ notifications.
	if( !IsActive() )
        {
        iPhone.NotifyNITZInfoChange( iStatus, iNitzInfo );
        SetActive();
		}
	
	__PRINTS( "CClockNitzListener::Start - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzListener::Stop
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::Stop()
	{
	__PRINTS( "CClockNitzListener::Stop - Entry" );
	
	Cancel();
	
	__PRINTS( "CClockNitzListener::Stop - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzListener::GetCurrentNITZInfoL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockNitzListener::GetCurrentNITZInfoL( RMobilePhone::TMobilePhoneNITZ& aNitzInfo )
	{
	__PRINTS( "CClockNitzListener::GetCurrentNITZInfoL - Entry" );
	
	// Check network connection because we do not trust the NitzInfo if Offline.
	TInt errorVal( KErrNotFound );
  	TInt networkStatus;

	CRepository* cenRep( NULL );
	TRAP( errorVal, cenRep = CRepository::NewL( KCRUidCoreApplicationUIs ) );
	
	if( KErrNone == errorVal )
		{
		// Check if the phone is online
		errorVal = cenRep->Get( KCoreAppUIsNetworkConnectionAllowed, networkStatus );
		
		if( ( KErrNone == errorVal ) && networkStatus )
			{
			errorVal = iPhone.GetNITZInfo( aNitzInfo );
			}
		}
	
	// Cleanup.
	delete cenRep;
	
	__PRINTS( "CClockNitzListener::GetCurrentNITZInfoL - Exit" );
	
    return errorVal;
	}

// ---------------------------------------------------------
// CClockNitzListener::RunL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::RunL()
	{
	__PRINTS( "CClockNitzListener::RunL - Entry" );
	
	// If error, don't use this notification.
	if( iStatus == KErrNone ) 
		{
		// Check the network status before reading the NitzInfo
		TInt networkStatus;
		TInt errorVal( KErrNone );
		
		CRepository* cenRep = CRepository::NewL( KCRUidCoreApplicationUIs );
		errorVal = cenRep->Get( KCoreAppUIsNetworkConnectionAllowed, networkStatus );
		// Cleanup.
		delete cenRep;

		if( ( KErrNone == errorVal ) && networkStatus ) 
			{
			// We read the network status correctly and are not in Offline or BT_SAP mode.
			errorVal = iPhone.GetNITZInfo( iNitzInfo );
			
			if( KErrNone == errorVal ) 
				{
				// Check if the contents of the NITZ packet are valid.
				// i.e if you can construct a TDateTime object using NITZ info.
				TDateTime dateTime;
				dateTime.Set( iNitzInfo.Year(),
							  TMonth( iNitzInfo.Month() ),
							  iNitzInfo.Day(),
							  iNitzInfo.Hour(),
							  iNitzInfo.Minute(),
							  iNitzInfo.Second(),
							  iNitzInfo.MicroSecond() );
				
				if( KErrNone == CheckDateTimeVal( dateTime ) )
					{
					// We have valid NitzInfo. Notify the observer
					NotifyObserverL();
					}
				}
			}
		}
	// Always re-register for Nitz notifications under all circumstances
	Start();
	 	
	__PRINTS( "CClockNitzListener::RunL - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzListener::DoCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::DoCancel()
    {
    __PRINTS( "CClockNitzListener::DoCancel - Entry" );
    
    // Cancel any outstanding requests.
    iPhone.CancelAsyncRequest( EMobilePhoneNotifyNITZInfoChange );
    
    __PRINTS( "CClockNitzListener::DoCancel - Exit" );
    }

// ---------------------------------------------------------
// CClockNitzListener::CClockNitzListener
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzListener::CClockNitzListener( CClockNitzPluginImpl* aObserver )
    : CActive( CActive::EPriorityHigh ),
      iObserver( aObserver )
    {
    __PRINTS( "CClockNitzListener::CClockNitzListener - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CClockNitzListener::CClockNitzListener - Exit" );
    }

// ---------------------------------------------------------
// CClockNitzListener::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::ConstructL()
    {
    __PRINTS( "CClockNitzListener::ConstructL - Entry" );
    
	// Initialize variables.
	iNitzUtcOffsetMin = CClockNitzListener::ENitzInvalidUtcOffsetMin;
   
    // Queue the active object in the scheduler.
    CActiveScheduler::Add( this );
     
    // Connect to the Etel server.
    const TInt KTriesToConnectServer( 10 );
    const TInt KTimeBeforeRetryingServerConnection( 100000 );
    
    TInt thisTry( NULL );
    TInt errVal( KErrNone );
    
    while( KErrNone != ( errVal = iEtelServer.Connect() ) &&
           KTriesToConnectServer >= ( thisTry++ ) )
        {
        User::After( KTimeBeforeRetryingServerConnection );
        }
    User::LeaveIfError( errVal );
    User::LeaveIfError( iEtelServer.LoadPhoneModule( KMmTsyModuleName ) );
    
    RTelServer::TPhoneInfo phoneInfo;
    
    TInt numPhone;
    User::LeaveIfError( iEtelServer.EnumeratePhones( numPhone ) );
    
    TName tsyName;
    for( TInt index( KZerothItem ); index < numPhone; index++ )
        {
        User::LeaveIfError( iEtelServer.GetPhoneInfo( index, phoneInfo ) );
        User::LeaveIfError( iEtelServer.GetTsyName( index, tsyName ) );

        if( KErrNone == tsyName.CompareF( KMmTsyModuleName ) )
            {
            break;
            }
        }
    User::LeaveIfError( iPhone.Open( iEtelServer, phoneInfo.iName ) );
    
    __PRINTS( "CClockNitzListener::ConstructL - Exit" );
    }


// ---------------------------------------------------------
// CClockNitzListener::NotifyObserverL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzListener::NotifyObserverL()
    {
    __PRINTS( "CClockNitzListener::NotifyObserverL - Entry" );
    
    if( iObserver )
        {
        iObserver->NotifyNITZInfoChangeL();
        }
    
    __PRINTS( "CClockNitzListener::NotifyObserverL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzListener::CheckDateTimeVal
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockNitzListener::CheckDateTimeVal( const TDateTime& aDateTime )
    {
    __PRINTS( "CClockNitzListener::CheckDateTimeVal - Entry" );
    
    TDateTime tempDateTime;
    
    __PRINTS( "CClockNitzListener::CheckDateTimeVal - Exit" );
    
    return tempDateTime.Set( aDateTime.Year(),
    						 aDateTime.Month(),
    						 aDateTime.Day(),
    						 aDateTime.Hour(),
    						 aDateTime.Minute(),
    						 aDateTime.Second(),
    						 aDateTime.MicroSecond() );
    }

// End of file
