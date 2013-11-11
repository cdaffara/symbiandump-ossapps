// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "smcmserver.h"

#include "smcmstepinitialise.h"
#include "smcmstepfind.h"
#include "smcmstepfindalias.h"
#include "smcmstepforward.h"
#include "smcmstepforwardemail.h"
#include "smcmstepreply.h"
#include "smcmstepreplyemail.h"
#include "smcmstepquerycapabilities.h"
#include "smcmstepsettings.h"


_LIT(KServerName,"smcmsuite_gsm");

CSmsServer::CSmsServer( )
	{

	}


// @return - Instance of the test server
// Called inside the MainL() function to create and start the
// CTestServer derived server.
CSmsServer* CSmsServer::NewL()
	{
	CSmsServer* server = new (ELeave) CSmsServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}

void CSmsServer::ConstructL( const TDesC& aName )
	{
	CTestServer::ConstructL( aName );
	}
	
LOCAL_C void MainL()
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
 	RProcess().SecureApi(RProcess::ESecureApiOn);
		
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmsServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	server = NULL;
	delete sched;
	}


// @return - Standard Epoc error code on exit
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


// @return - A CTestStep derived instance
// Implementation of CTestServer pure virtual
CTestStep* CSmsServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	//StaticSystemInformation
	if( aStepName == KSMCMStepInitialise )
		{
		testStep = new CSMCMStepInitialise( );
		}
	else if ( aStepName == KSMCMStepFind )
		{
		testStep = new CSMCMStepFind( );
		}
	else if ( aStepName == KSMCMStepFindAlias )
		{
		testStep = new CSMCMStepFindAlias( );
		}
	else if ( aStepName == KSMCMStepForward )
		{
		testStep = new CSMCMStepForward( );
		}
	else if ( aStepName == KSMCMStepForwardEmail )
		{
		testStep = new CSMCMStepForwardEmail( );
		}
	else if ( aStepName == KSMCMStepQueryCapabilities )
		{
		testStep = new CSMCMStepQueryCapabilities( );
		}
	else if ( aStepName == KSMCMStepReply )
		{
		testStep = new CSMCMStepReply( );
		}
	else if ( aStepName == KSMCMStepReplyEmail )
		{
		testStep = new CSMCMStepReplyEmail( );
		}
	else if ( aStepName == KSMCMStepSettings )
		{
		testStep = new CSMCMStepSettings( );
		}

	return testStep;
	
	}
