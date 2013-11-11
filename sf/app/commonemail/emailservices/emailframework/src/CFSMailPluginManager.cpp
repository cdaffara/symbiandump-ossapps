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
* Description:  common mailbox object
*
*/

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>
#include "CFSMailPluginManager.h"
#include "CFSMailRequestObserver.h"
#include "CFSMailRequestHandler.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailPluginManager::CFSMailPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager::CFSMailPluginManager()
{
    NM_FUNCTION;
}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::~CFSMailPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager::~CFSMailPluginManager()
{
    NM_FUNCTION;

	iPluginInfo.ResetAndDestroy();
}


// -----------------------------------------------------------------------------
// CFSMailPluginManager::NewLC
// -----------------------------------------------------------------------------
CFSMailPluginManager* CFSMailPluginManager::NewLC(TInt aConfiguration)
{
    NM_FUNCTION;

    CFSMailPluginManager* pluginManager = new (ELeave) CFSMailPluginManager();
    CleanupStack:: PushL(pluginManager);
    pluginManager->ConstructL(aConfiguration);
    return pluginManager;
}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::NewL
// -----------------------------------------------------------------------------
CFSMailPluginManager* CFSMailPluginManager::NewL(TInt aConfiguration)
{
    NM_FUNCTION;

    CFSMailPluginManager* pluginManager =  CFSMailPluginManager::NewLC(aConfiguration);
    CleanupStack:: Pop(pluginManager);
    return pluginManager;
}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::ConstructL
// -----------------------------------------------------------------------------
void CFSMailPluginManager::ConstructL(TInt aConfiguration)
{
    NM_FUNCTION;

	// list plugin implementations

	iPluginInfo.Reset();
	CFSMailPlugin::ListImplementationsL(iPluginInfo);
	CFSMailPlugin::Close();

	// load plugins
	CFSMailPlugin* plugin = NULL;
    // <cmail>
	TInt tempCount = iPluginInfo.Count();
	for(TInt i=0;i<tempCount;i++)
	// </cmail>
	{
		TUid id = iPluginInfo[i]->ImplementationUid();
		if(aConfiguration == EFSLoadPlugins)
		{
			TRAPD(err,plugin = CFSMailPlugin::NewL( id ));
			CFSMailPlugin::Close();		
			if(err == KErrNone && plugin)
			{
				AddPluginL(id,plugin);
			}
		}
// <cmail>
		else
		    {
		    }
// </cmail>
	}

}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::GetPluginByIndex
// -----------------------------------------------------------------------------
CFSMailPlugin* CFSMailPluginManager::GetPluginByIndex(TUint aIndex)
	{
    NM_FUNCTION;

	return iPluginList[aIndex]->iPlugin;
	}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::GetPluginCount
// -----------------------------------------------------------------------------
TUint CFSMailPluginManager::GetPluginCount( )
	{
    NM_FUNCTION;

	return iPluginList.Count();
	}



