/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSMailRequestObserver.
*
*/

#include "emailtrace.h"

// <qmail>
#include <nmcommonheaders.h>
// </qmail>

#include "CFSMailRequestObserver.h"
#include "CFSMailRequestHandler.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailRequestObserver::CFSMailRequestObserver
// -----------------------------------------------------------------------------
CFSMailRequestObserver::CFSMailRequestObserver()
{
    NM_FUNCTION;
}

// -----------------------------------------------------------------------------
// CFSMailRequestObserver::~CFSMailRequestObserver
// -----------------------------------------------------------------------------
CFSMailRequestObserver::~CFSMailRequestObserver()
{
    NM_FUNCTION;
}
// -----------------------------------------------------------------------------
// CFSMailRequestObserver::NewLC
// -----------------------------------------------------------------------------
CFSMailRequestObserver* CFSMailRequestObserver::NewLC( CFSMailRequestHandler& aPluginManager,
											 MFSMailRequestObserver& aObserver)
{
    NM_FUNCTION;
    
	CFSMailRequestObserver* obs = new (ELeave) CFSMailRequestObserver();
  	CleanupStack:: PushL(obs);
  	obs->ConstructL(aPluginManager,aObserver);
  	return obs;
} 

// -----------------------------------------------------------------------------
// CFSMailRequestObserver::NewL
// -----------------------------------------------------------------------------
CFSMailRequestObserver* CFSMailRequestObserver::NewL( CFSMailRequestHandler& aPluginRequestHandler,
											MFSMailRequestObserver& aObserver )
{
    NM_FUNCTION;
    
  	CFSMailRequestObserver* obs =  CFSMailRequestObserver::NewLC(aPluginRequestHandler,aObserver);
  	CleanupStack::Pop(obs);
  	return obs;
}

// -----------------------------------------------------------------------------
// CFSMailRequestObserver::ConstructL
// -----------------------------------------------------------------------------
void CFSMailRequestObserver::ConstructL( CFSMailRequestHandler& aPluginRequestHandler,
									MFSMailRequestObserver& aObserver )
{
    NM_FUNCTION;
    
	iObserver = &aObserver;
	iRequestHandler = &aPluginRequestHandler;
}

// -----------------------------------------------------------------------------
// CFSMailRequestObserver::RequestResponseL
// -----------------------------------------------------------------------------
void CFSMailRequestObserver::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
{
    NM_FUNCTION;

	if(iObserver)
		{
		iObserver->RequestResponseL(aEvent,aRequestId);	
		}
		
	if(aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete ||
		aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled)
		{
		iRequestHandler->CompleteRequest(aRequestId);
		}

}

// -----------------------------------------------------------------------------
// CFSMailRequestObserver::SetUserObserver
// -----------------------------------------------------------------------------
void CFSMailRequestObserver::SetUserObserver(MFSMailRequestObserver& aObserver)
	{
    NM_FUNCTION;
    
    iObserver = &aObserver;
	}
	
