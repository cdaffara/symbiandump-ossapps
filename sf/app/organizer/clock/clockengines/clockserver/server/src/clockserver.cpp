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
* Description:   The source file of the CClkSrvMain class.
*
*/

// System includes
#include <e32err.h>
#include <centralrepository.h>

// User includes
#include "clockserver.h"
#include "clockserversession.h"
#include "clockservercommon.h"
#include "clockserverimpl.h"
#include "clockprivatecrkeys.h"
#include "clock_debug.h"

// Constants
const TInt KClockServerMajorVN( 1 );
const TInt KClockServerMinorVN( 0 );
const TInt KClockServerBuildVN( 1 );

// ---------------------------------------------------------
// CClkSrvMain::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvMain* CClkSrvMain::NewL()
    {
    __PRINTS( "CClkSrvMain::NewL - Entry" );
    
    CClkSrvMain* clockServer = new( ELeave ) CClkSrvMain( CActive::EPriorityStandard );
    CleanupStack::PushL( clockServer );
    
    clockServer->ConstructL();
    clockServer->StartL( KNameClockServer );
    
    CleanupStack::Pop( clockServer );
    
    __PRINTS( "CClkSrvMain::NewL - Exit" );
    
    return clockServer;
    }
    
// ---------------------------------------------------------
// CClkSrvMain::~CClkSrvMain
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvMain::~CClkSrvMain()
	{
    __PRINTS( "CClkSrvMain::~CClkSrvMain - Entry" );
    
	// Destroy the server implementation.
	if( iClkSrvImpl )
		{
		delete iClkSrvImpl;
		iClkSrvImpl = NULL;
		}
	
	__PRINTS( "CClkSrvMain::~CClkSrvMain - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvMain::CClkSrvMain
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvMain::CClkSrvMain( TInt aPriority ) 
			: CServer2( aPriority ), iSessionQue( CClkSrvSession::LinkOffset() )
	{
    __PRINTS( "CClkSrvMain::CClkSrvMain - Entry" );

    // No implementation yet
    
    __PRINTS( "CClkSrvMain::CClkSrvMain - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvMain::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkSrvMain::ConstructL()
	{
    __PRINTS( "CClkSrvMain::ConstructL - Entry" );
    
	iClkSrvImpl = CClkSrvImpl::NewL( this );
	
	// Restore the state of the clockserver system.
	RestoreStateL();
	
	__PRINTS( "CClkSrvMain::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvMain::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkSrvMain::RestoreStateL()
	{
	__PRINTS( "CClkSrvMain::RestoreState - Entry" );
	
	// Here we read from the cenrep the id of the plugin which was active.
	// If the id is not NULL, we activate the plugin.
	
	TInt pluginId( NULL );
	// Get the active plugin id.
	TInt errVal( GetActivePluginL( pluginId ) );
	
	if( ( KErrNone == errVal ) && pluginId )
	    {
	    // Activate the plugin.
	    iClkSrvImpl->ActivateAllProtocolsL();
	    }
	
	__PRINTS( "CClkSrvMain::RestoreState - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvMain::NewSessionL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CSession2* CClkSrvMain::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
	{
    __PRINTS( "CClkSrvMain::NewSessionL - Entry" );
    
	TVersion versionObj( KClockServerMajorVN, KClockServerMinorVN, KClockServerBuildVN );
	
	// Check if the version is supported.
	if( !User::QueryVersionSupported( versionObj, aVersion ) )
		{
		// Version not supported.
		User::Leave( KErrNotSupported );
        }
	
	// Create a session and add it to the server's session queue.
	CClkSrvSession* clockSrvSes = new( ELeave ) CClkSrvSession( iClkSrvImpl );
    
    CClkSrvMain* clockServer = ( CClkSrvMain* )this;
    
    // Add the session to the queue.
    clockServer->SessionQue().AddLast( *clockSrvSes );
    
    __PRINTS( "CClkSrvMain::NewSessionL - Exit" );
    
    // return the session
    return clockSrvSes;
	}
	
// ---------------------------------------------------------
// CClkSrvMain::NotifyAboutChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkSrvMain::NotifyAboutChange( TChangedComponent aWhatChanged, TInt aWhoChanged, TInt aNotification )
	{
    __PRINTS( "CClkSrvMain::NotifyAboutChange - Entry" );

    __PRINT( "%d", aNotification );
    
	// Get the session que iterator.
	TDblQueIter< CClkSrvSession > sessionQueIter( SessionQue() );
	
	// Get each session object and notify about the change.
    CClkSrvSession* clkSrvSession = NULL;
    
    while( NULL != ( clkSrvSession = sessionQueIter++ ) )
        {
        __PRINTS( "Notify for Session" );

        clkSrvSession->NotifyAboutChange( aWhatChanged, aWhoChanged, aNotification );
        }
    
    __PRINTS( "CClkSrvMain::NotifyAboutChange - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvMain::SessionQue
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TDblQue< CClkSrvSession >& CClkSrvMain::SessionQue()
	{
    __PRINTS( "CClkSrvMain::SessionQue - Entry" );
    
    __PRINTS( "CClkSrvMain::SessionQue - Exit" );
    
	return iSessionQue;
	}

// ---------------------------------------------------------
// CClkSrvMain::GetActivePluginL
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
TInt CClkSrvMain::GetActivePluginL( TInt& aPluginId ) const
    { 
     __PRINTS( "CClkSrvMain::GetActivePluginL - Entry" );
    
	TInt returnVal( KErrNone );	
	CRepository* cenRep( NULL );
	
	// Open the repository.
	TRAPD( errVal, cenRep = CRepository::NewL( TUid::Uid(KCRUidNitz ) ) );
	if( KErrNone == errVal ) 
		{
		// Read the key.
		errVal = cenRep->Get( KActiveProtocol, aPluginId );
		
		if( KErrNone != errVal )
			{
			returnVal = errVal;
			}
		
		delete cenRep;
		}
	
	__PRINTS( "CClkSrvMain::GetActivePluginL - Exit" );
	
	return returnVal;	
    }

// ---------------------------------------------------------
// CClkSrvMain::SetActivePluginL
// rest of the details are commented in the header.
// ---------------------------------------------------------
//
TInt CClkSrvMain::SetActivePluginL( TInt aPluginId )
    {
    __PRINTS( "CClkSrvMain::SetActivePluginL - Entry" );
    
	TInt returnVal( KErrNone );   
    CRepository* cenRep( NULL );
    
    // Open the repository.
    TRAPD( errVal, cenRep = CRepository::NewL( TUid::Uid(KCRUidNitz ) ) );
    if( KErrNone == errVal ) 
        {
        // Update the key.
        errVal = cenRep->Set( KActiveProtocol, aPluginId );
        
        if( KErrNone != errVal )
            {
            returnVal = errVal;
            }
        
        delete cenRep;
        }
	
	__PRINTS( "CClkSrvMain::SetActivePluginL - Exit" );
	
    return returnVal;
    }

// End of file
