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
 * Description:  CS Server main class. Co-ordinates server startup,
 *                shutdown and receives client requests.
 *
 */

// INCLUDE FILES
#include <e32base.h>
#include <ccsdefs.h>
#include <ccsconversationentry.h>

// USER INCLUDES
#include "ccsdebug.h"
#include "ccsserver.h"
#include "ccssession.h"
#include "ccsplugininterface.h"
#include "ccsconversationevent.h"
#include "ccsconversationcache.h"
#include "ccscontactsmanager.h"
#include "ccsbackuphandler.h"
//Costant Declaration

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsServer::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsServer* CCsServer::NewL()
{
    //TODO check the print macro for warning
    //TODO handle all error conditions

    PRINT ( _L("Enter CCsServer::NewL") );

    CCsServer* self = new (ELeave) CCsServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    self->StartL(KCsServerName);
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsServer::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsServer::CCsServer
// Constructor
// ----------------------------------------------------------------------------
CCsServer::CCsServer() :
    CServer2(EPriorityLow)
{
}

// ----------------------------------------------------------------------------
// CCsServer::DeletePlugins
// Unload the plugins for backup/restore
// ----------------------------------------------------------------------------
void CCsServer::DeletePlugins()
{
    if (iConversationPlugin) {
        delete iConversationPlugin;
        iConversationPlugin = NULL;
    }
}

// ----------------------------------------------------------------------------
// CCsServer::LoadPlugins
// Load the plugins after a backup/restore
// ----------------------------------------------------------------------------

void CCsServer::LoadPlugins()
{
    iConversationPlugin = CCsPluginInterface::NewL();
    // Create the plugin for the required entries
    iConversationPlugin->InstantiatePluginL(this);
    //fetch all initial set of messages
    iConversationPlugin->GetConversationsL();
    RefreshConversations();
}

// ----------------------------------------------------------------------------
// CCsServer::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CCsServer::ConstructL()
{
    PRINT ( _L("Enter CCsServer::ConstructL") );

    // Create contacts manager and resgiter for events
    iContactsManager = new CCsContactsManager();
    
    iConversationPlugin = CCsPluginInterface::NewL();

    // Create the plugin for the required entries
    iConversationPlugin->InstantiatePluginL(this);
    
    //fetch all initial set of messages
    iConversationPlugin->GetConversationsL();
    
    // create cache
    iConversationCache = CCsConversationCache::NewL(iContactsManager, this);

    iCsCachingStatus = KCachingStatusUnknown;
	
    iBackUpHandler = CCsBackUpHandler::NewL(*this);
    
    PRINT ( _L("End CCsServer::ConstructL") );
}

// ----------------------------------------------------------------------------
// CCsServer::~CCsServer
// Destructor
// ----------------------------------------------------------------------------
CCsServer::~CCsServer()
{
    PRINT ( _L("Enter CCsServer::~CCsServer") );

    // delete cache
    if (iConversationCache)
    {
        delete iConversationCache;
        iConversationCache = NULL;
    }

    // delete the plugin interface object
    if (iConversationPlugin)
    {
        delete iConversationPlugin;
        iConversationPlugin = NULL;
    }

    // Delete the contact manager
    if (iContactsManager)
    {
        delete iContactsManager;
        iContactsManager = NULL;
    }

	if(iBackUpHandler)
	{
		delete iBackUpHandler;
		iBackUpHandler = NULL;
	}
    REComSession::FinalClose();

    PRINT ( _L("End CCsServer::~CCsServer") );
}

// ----------------------------------------------------------------------------
// CCsServer::NewSessionL
// Constructor
// ----------------------------------------------------------------------------
CSession2* CCsServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
{
    TVersion serverVersion(1, 0, 0);
    if (!User::QueryVersionSupported(serverVersion, aVersion))
        User::Leave(KErrNotSupported);

    CCsSession* session = CCsSession::NewL(const_cast<CCsServer*> (this));

    PRINT ( _L("CCsServer::NewSessionL - New Session Created") );

    return session;
}

// ----------------------------------------------------------------------------
// CCsServer::ConversationCacheInterface
// Get the cache interface
// ----------------------------------------------------------------------------
CCsConversationCache* CCsServer::ConversationCacheInterface()
{
    return iConversationCache;
}

// ----------------------------------------------------------------------------
// CCsServer::AddConversations
// This is to handle the new conversation event callback from plugin dll
// The idea is to add entries to the cache and at the same time
// update the client UI
// ----------------------------------------------------------------------------
void CCsServer::AddConversations(
                                 const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
{
    PRINT ( _L("Enter CCsServer::AddConversations") );

    iConversationCache->HandleConversations(aConversationEntryLists,
                                            KConversationEventNew);

    PRINT ( _L("End CCsServer::AddConversations") );
}

// ----------------------------------------------------------------------------
// CCsServer::ModifyConversations
// This is to handle the modify conversation event callback from plugin dll
// The idea is to update entries to the cache and at the same time update
// the client UI
// ----------------------------------------------------------------------------
void CCsServer::ModifyConversations(
                                    const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
{
    PRINT ( _L("Enter CCsServer::ModifyConversations") );

    iConversationCache->HandleConversations(aConversationEntryLists,
                                            KConversationEventUpdate);

    PRINT ( _L("End CCsServer::ModifyConversations") );
}

// ----------------------------------------------------------------------------
// CCsServer::DeleteConversations
// This is to handle the delete event callback from plugin dll
// The idea is to delete entries to the cache and at the same time update the client UI
// ----------------------------------------------------------------------------
void CCsServer::DeleteConversations(
                                    const RPointerArray<CCsConversationEntry>& aConversationEntryLists)
{
    PRINT ( _L("Enter CCsServer::DeleteConversations") );

    iConversationCache->HandleConversations(aConversationEntryLists,
                                            KConversationEventDelete);

    PRINT ( _L("End CCsServer::DeleteConversations") );
}

// ----------------------------------------------------------------------------
// CCsServer::NotifySessions
// Callback for on-the-fly data changes
// ----------------------------------------------------------------------------
void CCsServer::NotifySessions(CCsClientConversation* aConversation,
                               TUint32 aEvent)
{
    iSessionIter.SetToFirst();
    CCsSession* session;
    while ( (session = (CCsSession*) iSessionIter++) != NULL)
    {
        TRAPD(error, session->HandleChangeEventL( aConversation, aEvent ));
        if (error != KErrNone)
        {
            PRINT1 ( _L("CCsServer::NotifySessions - Error:%d"), error );
        }
    }
}

// ----------------------------------------------------------------------------
// CCsServer::CachingCompleted
// This is to get caching complete event from plugins
// ----------------------------------------------------------------------------
void CCsServer::CachingCompleted()
{
    // NOTE:- NO CHECK DONE NOW ON NUMBER OF PLUGINS TO KEEP IT SIMPLE.
    // NOT EXPECTED TO BE MORE THAN 1 PLUGIN FOR TB 9.2.
    iCsCachingStatus = KCachingStatusCompleted;
}

// ----------------------------------------------------------------------------
// CCsServer::RefreshConversations
// Refresh conversations.
// ----------------------------------------------------------------------------
void CCsServer::RefreshConversations()
{
    // NOTE:- NO CHECK DONE NOW ON NUMBER OF PLUGINS TO KEEP IT SIMPLE.
    // NOT EXPECTED TO BE MORE THAN 1 PLUGIN FOR TB 9.2.

    // Delete and recreate cache.
    if (iConversationCache)
    {
        delete iConversationCache;
        iConversationCache = NULL;

        iCsCachingStatus = KCachingStatusUnknown;
        TRAPD(error, iConversationCache = CCsConversationCache::
                NewL(iContactsManager, this));
        if (error != KErrNone)
        {
            PRINT1 ( _L("CCsServer::RefreshConversations - Error:%d"), error );
        }
    }

    // Notify client to refresh
    NotifySessions(NULL, KConversationEventRefresh);
    NotifySessions(NULL, KConversationEventListRefresh);
    // Read again.
    TRAPD(error, iConversationPlugin->GetConversationsL());
    if (error != KErrNone)
    {
        PRINT1 ( _L("CCsServer::RefreshConversations - Error:%d"), error );
    }
}

// end of file
