/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "cimageprintserver.h"
#include "imageprintclientserver.h"
#include "cimageprintsession.h"
#include "clog.h"
#include "cimageprintbody.h"
 
namespace
	{
	// panic
	_LIT( KImagePrintServerPanic, "CIPSer" );
	enum TImagePrintServerPanic
		{
		EBadDescriptor
		};
	
	void PanicClient( const RMessage2& aMessage, TImagePrintServerPanic aCategory )
		{
		aMessage.Panic( KImagePrintServerPanic, aCategory );
		}
	}


CImagePrintServer* CImagePrintServer::NewLC()
    {
	CImagePrintServer* server = new (ELeave) CImagePrintServer();
    CleanupStack::PushL( server ) ;
    server->ConstructL();
    return server;
    }

        
CImagePrintServer::CImagePrintServer()
	: CPolicyServer( 0, KImagePrintPolicy, ESharableSessions )
	{
	}    

CImagePrintServer::~CImagePrintServer()
    {
    LOG("CImagePrintServer::~CImagePrintServer begin");
    delete iEngine;
	iFbs.Disconnect();
	LOG("CImagePrintServer::~CImagePrintServer end");
    }

void CImagePrintServer::ConstructL()
    {  
    LOG("CImagePrintServer::ConstructL begin");          
    StartL( KImagePrintServerName );    
    User::LeaveIfError( iFbs.Connect() );        
    iEngine = CImagePrintBody::NewL();
    LOG("CImagePrintServer::ConstructL end");
    }

void CImagePrintServer::AddSession()
	{
	LOG("CImagePrintServer::AddSession begin");
	++iConnsCount;
	LOG("CImagePrintServer::AddSession end");
	}

void CImagePrintServer::RemoveSession()
	{
	--iConnsCount;
	LOG1("CImagePrintServer::RemoveSession iConnsCount: %d", iConnsCount);
	if( iConnsCount == 0 )
		{
		delete iEngine;
		iEngine = NULL;
		iFbs.Disconnect();
		CActiveScheduler::Stop();
		}
	LOG("CImagePrintServer::RemoveSession end");
	}

TInt CImagePrintServer::SessionCount() const
	{
	LOG("CImagePrintServer::SessionCount begin");
	LOG1("CImagePrintServer::SessionCount return: %d", iConnsCount);
	return iConnsCount;
	}

TInt CImagePrintServer::RunError( TInt aError )
    {
    LOG1("CImagePrintServer::RunError aError: %d", aError);
	if( aError == KErrBadDescriptor )
        {        
        PanicClient( Message(), EBadDescriptor );
        }
	else
        {
		Message().Complete( aError );
        }	
	ReStart();
	LOG("CImagePrintServer::RunError end");
	return KErrNone;
    }

    
CSession2* CImagePrintServer::NewSessionL( const TVersion& aVersion, const RMessage2& ) const
	{
	LOG("CImagePrintServer::NewSessionL begin");
	TBool supported = User::QueryVersionSupported( TVersion( KImagePrintServerMajor,
													KImagePrintServerMinor,
													KImagePrintServerBuild ),
													aVersion );	
	if( !supported )
        {
		User::Leave( KErrNotSupported );
        }
        
    LOG("CImagePrintServer::NewSessionL end");	
	return new (ELeave) CImagePrintSession();
	}
	
TInt CImagePrintServer::ReserveEngine( const CSession2* aSession )
	{
	LOG("CImagePrintServer::ReserveEngine begin");
	TInt err( KErrInUse );
	if( iEngineHolder == aSession || !iEngineHolder )
		{
		iEngineHolder = aSession;
		err = KErrNone;
		}
	LOG1("CImagePrintServer::ReserveEngine return: %d", err);
	return err;	
	}

TInt CImagePrintServer::ReleaseEngine( const CSession2* aSession )
	{
	LOG("CImagePrintServer::ReleaseEngine begin");
	TInt err( KErrInUse );
	if( iEngineHolder == aSession || !iEngineHolder )
		{
		iEngineHolder = NULL;
		err = KErrNone;
		}
	LOG1("CImagePrintServer::ReleaseEngine return: %d", err);
	return err;		
	}

CImagePrintBody& CImagePrintServer::Engine() const
	{
	LOG("CImagePrintServer::Engine begin");
	LOG("CImagePrintServer::Engine end");
	return *iEngine;
	}

//  End of File
