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
* Description:   The source file of the CClkMccListener class.
*
*/

// System includes
#include <mmtsy_names.h>
#include <e32property.h>

// User includes
#include "clockservermcclistener.h"
#include "clockservermccobserver.h"
#include "clock_debug.h"

// Constants
const TInt KZeroIndex( 0 );
const TInt KMinMccValue( 199 );
const TInt KMaxMccValue( 800 );

// Literals
_LIT( KNoName, "" );

// ---------------------------------------------------------
// CClkMccListener::NewL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkMccListener* CClkMccListener::NewL( MClkSrvMccObserver* aObserver )
    {
    __PRINTS( "CClkMccListener::NewL - Entry" );
    
    CClkMccListener* mccListener = new( ELeave ) CClkMccListener( aObserver );
    CleanupStack::PushL( mccListener );
    
    mccListener->ConstructL();
        
    CleanupStack::Pop( mccListener );
    
    __PRINTS( "CClkMccListener::NewL - Exit" );
    
    return mccListener;
    }

// ---------------------------------------------------------
// CClkMccListener::~CClkMccListener()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkMccListener::~CClkMccListener()
	{
	__PRINTS( "CClkMccListener::~CClkMccListener - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClkMccListener::~CClkMccListener - Exit" );
	}

// ---------------------------------------------------------
// CClkMccListener::CClkMccListener()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkMccListener::CClkMccListener( MClkSrvMccObserver* aObserver ) 
						: CActive( EPriorityStandard ),
						  iIsInitialized( EFalse ),
						  iNetworkInfoPckg( iNetworkInfo ),
						  iCurrentMcc( KNoName ),
						  iObserver( aObserver )
	{
	__PRINTS( "CClkMccListener::CClkMccListener - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClkMccListener::CClkMccListener - Exit" );
	}

// ---------------------------------------------------------
// CClkMccListener::ConstructL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkMccListener::ConstructL()
	{
	__PRINTS( "CClkMccListener::ConstructL - Entry" );
	
	// Add the active object to the scheduler.
	CActiveScheduler::Add( this );
	
	// Constants
	const TInt KTriesToConnectServer( 10 );
	const TInt KTimeBeforeRetryingServerConnection( 100000 );
	
	TInt thisTry( 0 );
	TInt returnVal( KErrNone );
	
	// Connect to the ETelServer.
	while ( ( returnVal = iEtelServer.Connect() ) != KErrNone &&
			( thisTry++ ) <= KTriesToConnectServer )
		{
		User::After( KTimeBeforeRetryingServerConnection );
		}
	User::LeaveIfError( returnVal );
	
	// Load the phone module.
	User::LeaveIfError( iEtelServer.LoadPhoneModule( KMmTsyModuleName ) );
	
	RTelServer::TPhoneInfo phoneInfo;
	TInt numPhone;
	User::LeaveIfError( iEtelServer.EnumeratePhones( numPhone ) );
	TName tsyName;
	
	// 
	for( TInt index( KZeroIndex ); index < numPhone; index++ )
		{
		// Get phone info.
		User::LeaveIfError( iEtelServer.GetPhoneInfo( index, phoneInfo ) );
		// Get Tsy name.
		User::LeaveIfError( iEtelServer.GetTsyName( index, tsyName ) );
		
		if( tsyName.CompareF( KMmTsyModuleName ) == 0 )
			{
			// We have the required Tsy.
			break;
			}
		}
	
	// Open a connection to the tsy.		
	User::LeaveIfError( iMobilePhone.Open( iEtelServer, phoneInfo.iName ) );
	
	__PRINTS( "CClkMccListener::ConstructL - Exit" );
	}
	
// ---------------------------------------------------------
// CClkMccListener::Start()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkMccListener::Start()
	{
	__PRINTS( "CClkMccListener::Start - Entry" );
	
	// Send a request and set the object active.
	if( !IsActive() )
		{
		// Object not active yet.
		if ( !iIsInitialized )
			{
			// This is for the first registration. 
			iMobilePhone.GetCurrentNetwork( iStatus, iNetworkInfoPckg );
			}
		else
			{
			// For subsequent ones, subscribe only for changes.
			iMobilePhone.NotifyCurrentNetworkChange( iStatus, iNetworkInfoPckg );
			}
		
		SetActive();
		}
	else
		{
		// Object already active.
		}
	
	__PRINTS( "CClkMccListener::Start - Exit" );
	}

// ---------------------------------------------------------
// CClkMccListener::Start()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkMccListener::RunL()
	{
	__PRINTS( "CClkMccListener::RunL - Entry" );
	
	if( iStatus == KErrNone )
		{
		// If iStatus is KErrNone or iStatus is not KRequestPending
		iIsInitialized = ETrue;
		
		// Get the Mcc.	
		TLex lexNetInfo( iNetworkInfo.iCountryCode );
				
		TInt networkError;
		TUint32 networkResult;				
		
		// Get the int of Mcc.
		networkError = lexNetInfo.Val( networkResult, EDecimal );		
		
		// If no error		
		if( KErrNone == networkError )
			{
			// Here we check if the iNetworkInfo.iCountryCode is a valid number 
			// and if it is a valid country code. 
			// According to http://en.wikipedia.org/wiki/Mobile_Country_Code, country codes
			// ranging from 200 to 799 are valid and used commercially.
			// MCC 1  is an exception for 3GPP conformance test.
			if( ( ( KMinMccValue < networkResult ) && ( KMaxMccValue > networkResult ) )
															 || ( 1 == networkResult ) )
				{						        
				// We have received a new Country Code
				iCurrentMcc = iNetworkInfo.iCountryCode;

				// Notify the observers about the new mcc.
				if( iObserver )
				    {
				    iObserver->NotifyMccChangeL();
				    }
				}
			}
		}
	
	// Re-subscribe for notfication on change of Mcc.	
	Start();
	
	__PRINTS( "CClkMccListener::RunL - Exit" );
	}

// ---------------------------------------------------------
// CClkMccListener::Stop()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkMccListener::Stop()	
	{
	__PRINTS( "CClkMccListener::Stop - Entry" );
	
	// Stop the active object.
	Cancel();
	
	__PRINTS( "CClkMccListener::Stop - Exit" );
	}
	
// ---------------------------------------------------------
// CClkMccListener::DoCancel()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkMccListener::DoCancel()	
	{
	__PRINTS( "CClkMccListener::DoCancel - Entry" );
	
	// Cancel for notification upon change.
	iMobilePhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
	
	__PRINTS( "CClkMccListener::DoCancel - Exit" );
	}

// ---------------------------------------------------------
// CClkMccListener::GetCurrentMcc()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkMccListener::GetCurrentMcc( RMobilePhone::TMobilePhoneNetworkCountryCode& aMcc )
	{
	__PRINTS( "CClkMccListener::GetCurrentMcc - Entry" );
	
	TInt returnVal( KErrNone );
	
	if( 0 < iIsInitialized && iCurrentMcc.Length() ) 
		{
		// If initialized and a valid MCC is available.
		aMcc = iCurrentMcc;
		returnVal = KErrNone;
		}
	else
		{
		// Else no MCC yet.
		returnVal = KErrNotFound;
		}
	
	__PRINTS( "CClkMccListener::GetCurrentMcc - Exit" );
	
	return returnVal;
	}	
	
// End of file
