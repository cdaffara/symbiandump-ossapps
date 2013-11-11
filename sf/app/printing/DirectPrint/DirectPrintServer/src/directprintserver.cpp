/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#include "directprintserver.h"
#include "directprintclientserver.h"
#include "directprintsession.h"
#include "clog.h"
#include "directprintbody.h"
 
namespace
	{
	// panic
	_LIT( KDirectPrintServerPanic, "CIPSer" );
	enum TDirectPrintServerPanic
		{
		EBadDescriptor
		};
	
	void PanicClient( const RMessage2& aMessage, TDirectPrintServerPanic aCategory )
		{
		aMessage.Panic( KDirectPrintServerPanic, aCategory );
		}
	}


CDirectPrintServer* CDirectPrintServer::NewLC()
    {
	CDirectPrintServer* server = new (ELeave) CDirectPrintServer();
    CleanupStack::PushL( server ) ;
    server->ConstructL();
    return server;
    }

        
CDirectPrintServer::CDirectPrintServer()
	: CPolicyServer( 0, KDirectPrintPolicy, ESharableSessions )
	{
	}    

CDirectPrintServer::~CDirectPrintServer()
    {
    LOG("CDirectPrintServer::~CDirectPrintServer begin");
    delete iEngine;
	iFbs.Disconnect();
	LOG("CDirectPrintServer::~CDirectPrintServer end");
    }

void CDirectPrintServer::ConstructL()
    {  
    LOG("CDirectPrintServer::ConstructL begin");          
    StartL( KDirectPrintServerName );    
    User::LeaveIfError( iFbs.Connect() );        
    iEngine = CDirectPrintBody::NewL();
    LOG("CDirectPrintServer::ConstructL end");
    }

void CDirectPrintServer::AddSession()
	{
	LOG("CDirectPrintServer::AddSession begin");
	++iConnsCount;
	LOG("CDirectPrintServer::AddSession end");
	}

void CDirectPrintServer::RemoveSession()
	{
	--iConnsCount;
	LOG1("CDirectPrintServer::RemoveSession iConnsCount: %d", iConnsCount);
	if( iConnsCount == 0 )
		{
		delete iEngine;
		iEngine = NULL;
		iFbs.Disconnect();
		CActiveScheduler::Stop();
		}
	LOG("CDirectPrintServer::RemoveSession end");
	}

TInt CDirectPrintServer::SessionCount() const
	{
	LOG("CDirectPrintServer::SessionCount begin");
	LOG1("CDirectPrintServer::SessionCount return: %d", iConnsCount);
	return iConnsCount;
	}

TInt CDirectPrintServer::RunError( TInt aError )
    {
    LOG1("CDirectPrintServer::RunError aError: %d", aError);
	if( aError == KErrBadDescriptor )
        {        
        PanicClient( Message(), EBadDescriptor );
        }
	else
        {
		Message().Complete( aError );
        }	
	ReStart();
	LOG("CDirectPrintServer::RunError end");
	return KErrNone;
    }

    
CSession2* CDirectPrintServer::NewSessionL( const TVersion& aVersion, const RMessage2& ) const
	{
	LOG("CDirectPrintServer::NewSessionL begin");
	TBool supported = User::QueryVersionSupported( TVersion( KDirectPrintServerMajor,
													KDirectPrintServerMinor,
													KDirectPrintServerBuild ),
													aVersion );	
	if( !supported )
        {
		User::Leave( KErrNotSupported );
        }
        
    LOG("CDirectPrintServer::NewSessionL end");	
	return new (ELeave) CDirectPrintSession();
	}
	
TInt CDirectPrintServer::ReserveEngine( const CSession2* aSession )
	{
	LOG("CDirectPrintServer::ReserveEngine begin");
	TInt err( KErrInUse );
	if( iEngineHolder == aSession || !iEngineHolder )
		{
		iEngineHolder = aSession;
		err = KErrNone;
		}
	LOG1("CDirectPrintServer::ReserveEngine return: %d", err);
	return err;	
	}

TInt CDirectPrintServer::ReleaseEngine( const CSession2* aSession )
	{
	LOG("CDirectPrintServer::ReleaseEngine begin");
	TInt err( KErrInUse );
	if( iEngineHolder == aSession || !iEngineHolder )
		{
		iEngineHolder = NULL;
		err = KErrNone;
		}
	LOG1("CDirectPrintServer::ReleaseEngine return: %d", err);
	return err;		
	}

CDirectPrintBody& CDirectPrintServer::Engine() const
	{
	LOG("CDirectPrintServer::Engine begin");
	LOG("CDirectPrintServer::Engine end");
	return *iEngine;
	}

//  End of File
