/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserServer implementation.
*
*/


#include "clientserver.h"
#include "cntparserserver.h"
#include "cntparserserversession.h"
#include "irmcconsts.h"

#include "debug.h"

#include <cntdb.h>

//------------------------------------------------------------
// RunServer
//------------------------------------------------------------
TInt CCntParserServer::RunServer()
	{
	LOGGER_ENTERFN( "CCntParserServer::RunServer" );
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt ret( KErrNoMemory );
	if( cleanup )
		{
		TRAP( ret, CCntParserServer::RunServerL() );
		delete cleanup;
		}
	if( ret != KErrNone )
		{
		// Signal the client that server creation failed
		RProcess::Rendezvous( ret );
		}
	LOGGER_LEAVEFN( "CCntParserServer::RunServer" );
	return ret;
	}
//------------------------------------------------------------
// RunServerL
//------------------------------------------------------------
void CCntParserServer::RunServerL()
	{
	// Create and install the active scheduler we need
	LOGGER_ENTERFN( "CCntParserServer::RunServerL" );
	CActiveScheduler *as=new (ELeave)CActiveScheduler;
	CleanupStack::PushL( as );
	CActiveScheduler::Install( as );

	// Create server
	CCntParserServer::NewL();

	// Initialisation complete, now signal the client
	User::LeaveIfError( RThread().RenameMe( KTxtServer ) );
	RProcess::Rendezvous( KErrNone );

	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( as );
	LOGGER_LEAVEFN( "CCntParserServer::RunServerL" );
	}
//------------------------------------------------------------
// E32Main()
//------------------------------------------------------------
TInt E32Main()
	{
	return CCntParserServer::RunServer();
	}
//------------------------------------------------------------
// PanicServer(TCntParserServerPanic aPanic)
//------------------------------------------------------------
GLDEF_C void PanicServer(TCntParserServerPanic aPanic)
	{
	LOGGER_WRITE( "PANIC SERVER!" );
	_LIT(KTxtServerPanic,"Parser server panic");
	User::Panic( KTxtServerPanic, aPanic );
	}


//**********************************
//CCntParserServer
//**********************************

//------------------------------------------------------------
// CCntParserServer::CCntParserServer()
//------------------------------------------------------------
CCntParserServer::CCntParserServer() : CServer2( EPriorityStandard )
	{
	}
//------------------------------------------------------------
// CCntParserServer::~CCntParserServer()
//------------------------------------------------------------
EXPORT_C CCntParserServer::~CCntParserServer()
	{
	if( iCollector )
	    {
	    iCollector->Cancel();
	    }
	delete iCollector;
	delete iCurrentDatabase;
	LOGGER_WRITE( "CntParserServer: DELETED!" );
	}
//------------------------------------------------------------
// CCntParserServer* CCntParserServer::NewL()
//------------------------------------------------------------
EXPORT_C CCntParserServer* CCntParserServer::NewL()
	{
	LOGGER_WRITE( "CntParserServer: NEW" );
	CCntParserServer* pS=new (ELeave) CCntParserServer();
	CleanupStack::PushL(pS);
	pS->StartL( KCntParserName );
	CleanupStack::Pop( pS );
	LOGGER_WRITE( "CntParserServer: Created" );
	return pS;
	}
//------------------------------------------------------------
// CCntParserServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMsg*/) const
//------------------------------------------------------------
CSession2* CCntParserServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMsg*/) const
	{
	LOGGER_WRITE( "CntParserServer: NEW SessionL" );
	// check we're the right version
	TVersion v( KCntParserMajorVersionNumber,KCntParserMinorVersionNumber,KCntParserBuildVersionNumber );
	if ( !User::QueryVersionSupported( v,aVersion ) )
		User::Leave( KErrNotSupported );
	CCntParserServerSession* self=CCntParserServerSession::NewL( (CCntParserServer*)this );
	return self;
	}
//------------------------------------------------------------
// CCntParserServer::IncreaseSessionCountL()
//------------------------------------------------------------
void CCntParserServer::IncreaseSessionCountL()
	{
	LOGGER_WRITE( "CntParserServer: Increase Session Count" );
	iSessionCount++;
	if( iSessionCount == 1 )	//If first session, prepare collector
		{
		PrepareCollectorL();
		}
	}
//------------------------------------------------------------
// CCntParserServer::DecreaseSessionCount()
//------------------------------------------------------------
void CCntParserServer::DecreaseSessionCount()
	{
	LOGGER_WRITE( "CntParserServer: Decrease Session Count" );
	iSessionCount--;
	if( iSessionCount==0 )	//if last session is disconnected, delete server
		{
		Stop();
		}
	}
//------------------------------------------------------------
// CCntParserServer::ContactsParsed()
//------------------------------------------------------------
TInt CCntParserServer::ContactsParsed()
	{
	return iCollector->CurrentItem();
	}
//------------------------------------------------------------
// CCntParserServer::TotalNumOfContacts()
//------------------------------------------------------------
TInt CCntParserServer::TotalNumOfContacts()	
	{
	//After the PrepareCollector is called, the number of contacts can be returned
	return iCollector->Count();
	}
//------------------------------------------------------------
// CCntParserServer::ContactsSaved()
//------------------------------------------------------------
TInt CCntParserServer::ContactsSaved()
	{
	return iCollector->ContactsSaved();
	}
//------------------------------------------------------------
// CCntParserServer::PrepareCollectorL()
//------------------------------------------------------------
void CCntParserServer::PrepareCollectorL()
	{
	LOGGER_WRITE( "CntParserServer: Prepare collectorL: Open Contact Database" );

	TFileName dbFile;
	TFileName defaultDb;
  
	CContactDatabase::GetDefaultNameL( defaultDb );
	
	LOGGER_WRITE( "CntParserServer: Prepare collectorL: GetDefault" );

	iCurrentDatabase=CContactDatabase::OpenL();
	
	LOGGER_WRITE( "CntParserServer: Prepare collectorL: OpenDefault" );

	iCurrentDatabase->GetCurrentDatabase( dbFile );	//Get selected database

	LOGGER_WRITE( "CntParserServer: Prepare collectorL: CompareDefault" );

	if( dbFile.CompareF(defaultDb)!=0 && dbFile!=KNullDesC16 /*&& dbFile.CompareF(KSimDummyDatabase)!=0*/)	//If the selected database is not the default and something is selected
		{
		LOGGER_WRITE( "CntParserServer: Prepare collectorL: Close Default" );
		delete iCurrentDatabase;
		iCurrentDatabase = NULL;

		LOGGER_WRITE( "CntParserServer: Prepare collectorL: OpenSelected" );
		iCurrentDatabase=CContactDatabase::OpenL( dbFile );	//open the selected one
		}

	LOGGER_WRITE( "CntParserServer: Prepare collectorL: Is everything ok?" );
	if( !iCurrentDatabase )
		{
		User::Leave( KErrNotFound );	
		}

	LOGGER_WRITE( "CntParserServer: Prepare collectorL: Create Collector" );
	iCollector = CCntParserCollector::NewL( this,iCurrentDatabase );

	iRequests = 0;
	}
//------------------------------------------------------------
// CCntParserServer::CreateIrMCL2PhoneBookL()
//------------------------------------------------------------
TInt CCntParserServer::CreateIrMCL2PhoneBookL(const TDesC& path, TInt aMaxNumberOfContacts,TBool aForced)
	{
	LOGGER_WRITE( "CntParserServer: CreateIrMCL2PhoneBookL" );

	TInt err( KErrNone );

	iRequests++;
	if( iRequests == 1 )
		{
		LOGGER_WRITE( "CntParserServer: CreateIrMCL2PhoneBookL: Ready to call collector" );

		err = iCollector->CreateIrMCL2FileL( path,aMaxNumberOfContacts,aForced );
		if( err==KPBNotUpdated ) 
			{
			iRequests = 0;	//If the file exists (no parsing), then more requests can be made	
			}
		}
	else
		{
		return KErrNotReady;	
		}
	return err;
	}
//------------------------------------------------------------
// CCntParserServer::CompletePhoneBookRequests()
//------------------------------------------------------------
void CCntParserServer::CompletePhoneBookRequests(TInt aErr)	//Goes through all the sessions and completes the request
	{
	LOGGER_WRITE( "CntParserServer: Complete phonebook request" );
	iSessionIter.SetToFirst();
	while( iSessionIter )	//Go through all sessions
		{
		( (CCntParserServerSession*) (iSessionIter++) )->CompletePhoneBookRequest( aErr );
		}
	iRequests = 0;	//Collector is ready to start again..
	}
//------------------------------------------------------------
// CCntParserServer::CancelPhoneBookRequest()
//------------------------------------------------------------
void CCntParserServer::CancelPhoneBookRequest()
	{
	if( iRequests == 1 )
		{
		iCollector->Cancel();
		}
	iRequests--;
	}
//------------------------------------------------------------
// CCntParserServer::CancelAllPhoneBookRequests()
//------------------------------------------------------------
void CCntParserServer::CancelAllPhoneBookRequests()
	{
	iCollector->Cancel();
	iRequests = 0;
	}
//------------------------------------------------------------
// CCntParserServer::Disconnect()
//------------------------------------------------------------
void CCntParserServer::Disconnect()
	{
	LOGGER_WRITE( "CntParserServer: Disconnect" );
	}
//------------------------------------------------------------
// CCntParserServer::Stop()
//------------------------------------------------------------
void CCntParserServer::Stop()
	{
	LOGGER_ENTERFN( "CntParserServer::Stop()" );
	if( iCollector )
	    {
	    iCollector->Cancel();
	    }
	delete iCollector;
	iCollector = NULL;
	delete iCurrentDatabase;
	iCurrentDatabase = NULL;
	CActiveScheduler::Stop();
	LOGGER_LEAVEFN( "CntParserServer::Stop()" );
	}
//------------------------------------------------------------
// CCntParserServer::SetRequests()
//------------------------------------------------------------
void CCntParserServer::SetRequests(TInt aRequests)
	{
	iRequests=aRequests;
	}



