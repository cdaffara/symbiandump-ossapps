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


#include <e32base.h>

#include "imageprintclientserver.h"
#include "cimageprintserver.h"
#include "cimageprintscheduler.h"
#include "clog.h"

// init and run the server
static void RunTheServerL()
	{
#ifndef __SECURE_API__
	User::LeaveIfError( RThread().Rename( KImagePrintServerName ) );  //Deprecated! Replace this with the line below as soon as possible
#else
	User::LeaveIfError( RThread().RenameMe( KImagePrintServerName ) );
#endif

	CActiveScheduler* scheduler = new (ELeave) CImagePrintScheduler;
	CleanupStack::PushL( scheduler );
	CActiveScheduler::Install( scheduler );

	CImagePrintServer::NewLC();
	
	RProcess::Rendezvous( KErrNone );

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( 2 );	// CImagePrintServer, scheduler
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
