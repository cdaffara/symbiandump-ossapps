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


#include <e32base.h>

#include "directprintclientserver.h"
#include "directprintserver.h"
#include "directprintscheduler.h"
#include "clog.h"

// init and run the server
static void RunTheServerL()
	{
#ifndef __SECURE_API__
	User::LeaveIfError( RThread().Rename( KDirectPrintServerName ) );  //Deprecated! Replace this with the line below as soon as possible
#else
	User::LeaveIfError( RThread().RenameMe( KDirectPrintServerName ) );
#endif

	CActiveScheduler* scheduler = new (ELeave) CDirectPrintScheduler;
	CleanupStack::PushL( scheduler );
	CActiveScheduler::Install( scheduler );

	CDirectPrintServer::NewLC();
	
	RProcess::Rendezvous( KErrNone );

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( 2 );	// CDirectPrintServer, scheduler
	}

// Server process entry-point
TInt E32Main()
	{	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r( KErrNoMemory );
	if( cleanup )
		{
		TRAP( r, RunTheServerL() );
		delete cleanup;
		}
	return r;
	}

//  End of File
