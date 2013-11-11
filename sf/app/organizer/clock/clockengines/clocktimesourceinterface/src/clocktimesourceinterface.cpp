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
* Description:   The source file of the CClockTimeSourceInterface class.
*
*/

// System includes
#include <ecom.h>

// User includes
#include "clocktimesourceinterface.h"
#include "clockpluginobserver.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CClockTimeSourceInterface::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockTimeSourceInterface* CClockTimeSourceInterface::NewL( const TUid& aImpUid, 
														   			 MClockPluginObserver* aObserver )
	{
	__PRINTS( "CClockTimeSourceInterface::NewL - Entry" );
    
    // Create implementation based on UID.
	CClockTimeSourceInterface* self( NULL );
	
	TAny* implementation( NULL );
	TRAPD( errVal, implementation = REComSession::CreateImplementationL( aImpUid, 
																		 _FOFF( CClockTimeSourceInterface, iDtor_ID_key ) ) );
	if ( KErrNone == errVal )
		{
		self = REINTERPRET_CAST( CClockTimeSourceInterface*, implementation );
		CleanupStack::PushL( self );
		
		self->ConstructL( aObserver );
		
		CleanupStack::Pop( self );
		}
	
	__PRINTS( "CClockTimeSourceInterface::NewL - Exit" );
	
	return self;
	}

// ---------------------------------------------------------
// CClockTimeSourceInterface::~CClockTimeSourceInterface
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClockTimeSourceInterface::~CClockTimeSourceInterface()
	{
	__PRINTS( "CClockTimeSourceInterface::~CClockTimeSourceInterface - Entry" );
	
	// Destroy the implementation.
	REComSession::DestroyedImplementation( iDtor_ID_key );
	
	__PRINTS( "CClockTimeSourceInterface::~CClockTimeSourceInterface - Exit" );
	}

// ---------------------------------------------------------
// CClockTimeSourceInterface::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockTimeSourceInterface::ConstructL( MClockPluginObserver* aObserver )
	{
	__PRINTS( "CClockTimeSourceInterface::ConstructL - Entry" );
	
	// Keep track of observer to notify.
	iObserver = aObserver;
	
	__PRINTS( "CClockTimeSourceInterface::ConstructL - Exit" );
	}
	
// End of file
