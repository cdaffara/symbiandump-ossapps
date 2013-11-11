/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for Presence Cache Server
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <e32base.h>
#include <e32std.h>
#include <ecom.h>

#include "presencecacheservicestore.h"
#include "presencecacheserver.h"
#include "presencecachesession.h"
#include "presencecachedefs2.h"
#include "prestrace.h"
#include "presencecachesecuritypolicy.h"

//Include Cache server namespace
using namespace NCacheSrv2;


// -----------------------------------------------------------------------------
// CPresenceCacheServer::NewLC()
// -----------------------------------------------------------------------------
//
CPresenceCacheServer* CPresenceCacheServer::NewLC()
    {
    CPresenceCacheServer* self = new( ELeave ) CPresenceCacheServer(EPriorityNormal);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::~CPresenceCacheServer()
// -----------------------------------------------------------------------------
//
CPresenceCacheServer::~CPresenceCacheServer()
    {
    delete iExpiryCheck;
    DoCancel();
    TInt count = iPresenceCache.Count();
    for (TInt i=0;i<count;i++)
        {
        delete iPresenceCache[i];
        }
    iPresenceCache.Close();
    REComSession::FinalClose();
    TRACE( _L("CPresenceCacheServer::~CPresenceCacheServer - end") );
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::IncrementSessions()
// -----------------------------------------------------------------------------
//
void CPresenceCacheServer::IncrementSessions()
    {
    TRACE( _L("CPresenceCacheServer::IncrementSessions - start") );
    iSessionCount++;        
    iShutdown.Cancel();      
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::DecrementSessions()
// -----------------------------------------------------------------------------
//
void CPresenceCacheServer::DecrementSessions()
    {
    TRACE( _L("CPresenceCacheServer::DecrementSessions - start") );
    iSessionCount--;
    if( iSessionCount == 0 )
        {
        iShutdown.Start();
        }
    }
    
// -----------------------------------------------------------------------------
// CPresenceCacheServer::SessionExists()
// -----------------------------------------------------------------------------
//
TBool CPresenceCacheServer::SessionExists(CPresenceCacheSession* aSession)
    {
    iSessionIter.SetToFirst();

    // Check TID match
    while ( iSessionIter )
        {
        CSession2* session = iSessionIter;
        iSessionIter++;
        if (((CPresenceCacheSession*)session)==aSession)
            return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::RunError()
// -----------------------------------------------------------------------------
//
TInt CPresenceCacheServer::RunError( TInt aError )
    {
    TRACE_1( _L("CPresenceCacheServer::RunError [%d] - enter"), aError );
    Message().Complete( aError );

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;    // Handled the error fully    
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::CPresenceCacheServer()
// -----------------------------------------------------------------------------
//
CPresenceCacheServer::CPresenceCacheServer( TInt aPriority )
: CPolicyServer( aPriority, KCacheServerPolicy )
    //                                 : CServer2( aPriority )
    {
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::ConstructL()
// -----------------------------------------------------------------------------
//
void CPresenceCacheServer::ConstructL()
    {
    iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	
	iExpiryCheck = CPresenceCacheExpiry::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CPresenceCacheServer::NewSessionL()
// -----------------------------------------------------------------------------
//
CSession2* CPresenceCacheServer::NewSessionL( const TVersion& aVersion, 
                        const RMessage2& /*aMessage */) const
    {
    TRACE( _L("CPresenceCacheServer::NewSessionL - enter"));
    TVersion srvVersion( NVersion::KMajor,
                         NVersion::KMinor,
                         NVersion::KBuild );

    if( !User::QueryVersionSupported( aVersion, srvVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    CPresenceCacheServer* self = const_cast< CPresenceCacheServer* >( this );
    CSession2* session = NULL;
   
    session = CPresenceCacheSession::NewL( *self );
    TRACE( _L("CPresenceCacheServer::NewSessionL - end"));
    return session;
    }
    
// -----------------------------------------------------------------------------
// CPresenceCacheServer::ExecuteL()
// -----------------------------------------------------------------------------
//
void CPresenceCacheServer::ExecuteL()
    {
    TRACE( _L("CPresenceCacheServer::ExecuteL - enter"));
    // start scheduler
    CActiveScheduler* pA = new( ELeave )CActiveScheduler;
    CleanupStack::PushL( pA );
    CActiveScheduler::Install( pA );

    // create server
    CPresenceCacheServer* server = CPresenceCacheServer::NewLC();
    server->StartL( NName::KSymbianServer );

    //Signal client that we are started
    RProcess().Rendezvous( KErrNone );

    //Execute the server
    CActiveScheduler::Start(); // CSI: 3 #

    //Cleanup
    CleanupStack::PopAndDestroy( server );//server
    CleanupStack::PopAndDestroy( pA );
    CActiveScheduler::Install( NULL );
    TRACE( _L("CPresenceCacheServer::ExecuteL - end"));
    }
    
// -----------------------------------------------------------------------------
// E32Main()
// Provides the API for the operating system to start the executable.
// Returns the address of the function to be called.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TRACE( _L("Cache E32Main - enter") );

    __UHEAP_MARK;

    User::RenameThread( NName::KMainThread );

    CTrapCleanup* tc = CTrapCleanup::New();
    if( !tc )
        {
        return KErrNoMemory;
        }

    TRAPD( err, CPresenceCacheServer::ExecuteL() );
    delete tc;

    __UHEAP_MARKEND;

    TRACE_1( _L("Cache E32Main - exit: %d"), err );
    return err;
    }

// -----------------------------------------------------------------------------
// CShutdown::RunL()
// -----------------------------------------------------------------------------
//
void CShutdown::RunL()
    {
    CActiveScheduler::Stop();
	}    
    
inline CShutdown::CShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CShutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CShutdown::Start()
	{After(KMyShutdownDelay);}    

// End of File

