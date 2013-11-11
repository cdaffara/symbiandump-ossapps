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
* Description:  CS Server Plugin Interface
*
*/


// INCLUDE FILES
#include "ccsplugin.h"
#include "ccsserver.h"
#include "ccsplugininterface.h"
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsPluginInterface::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsPluginInterface* CCsPluginInterface::NewL()
{
    PRINT ( _L("Enter CCsPluginInterface::NewL") );

    CCsPluginInterface* self = CCsPluginInterface::NewLC();
    CleanupStack::Pop( self );

    PRINT ( _L("End CCsPluginInterface::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::NewLC
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsPluginInterface* CCsPluginInterface::NewLC()
{
    PRINT ( _L("Enter CCsPluginInterface::NewLC") );

    CCsPluginInterface* self = new( ELeave ) CCsPluginInterface();
    CleanupStack::PushL( self );
    self->ConstructL();

    PRINT ( _L("End CCsPluginInterface::NewLC") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::CCsPluginInterface
// Constructor
// ----------------------------------------------------------------------------
CCsPluginInterface::CCsPluginInterface()
{
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::ConstructL
// 2nd phase constructor
// ----------------------------------------------------------------------------
void CCsPluginInterface::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::~CCsPluginInterface
// Constructor
// ----------------------------------------------------------------------------
CCsPluginInterface::~CCsPluginInterface( )
{
    PRINT ( _L("Enter CCsPluginInterface::~CCsPluginInterface") );

    UnloadPlugIns();
    iCsPluginInstances.ResetAndDestroy();

    PRINT ( _L("End CCsPluginInterface::~CCsPluginInterface") );
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::InstantiateAllPlugInsL
// Instantiates all plugins
// ----------------------------------------------------------------------------
void CCsPluginInterface::InstantiatePluginL(CCsServer* aCsServer )
    {
    RImplInfoPtrArray infoArray;

    // Get list of all implementations
    ListAllImplementationsL( infoArray );

    // Instantiate plugins for all impUIds by calling
    // InstantiatePlugInFromImpUidL
    for ( TInt iloop=0; iloop<infoArray.Count(); iloop++ )
        {
        // Get imp info
        CImplementationInformation& info( *infoArray[iloop] );

        // Get imp UID
        TUid impUid ( info.ImplementationUid() );

        PRINT ( _L("------------- CS Plugin Details -----------"));
        PRINT1 ( _L("Name: %S"), &(info.DisplayName()) );
        PRINT1 ( _L("UID: %X"), impUid.iUid );
        iPluginInUse.iUid = impUid.iUid;
        PRINT ( _L("-----------------------------------------------"));

        CCsPlugin* plugin = NULL;

        //instantiate plugin for impUid
	        plugin = InstantiatePlugInFromImpUidL( impUid, aCsServer );
        if ( plugin )
            {
            iCsPluginInstances.AppendL( plugin );
            }
        }
    infoArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCsPluginInterface::UnloadPlugIns
// Unloads plugins
// -----------------------------------------------------------------------------
void CCsPluginInterface::UnloadPlugIns()
{
    REComSession::FinalClose();
    PRINT ( _L("CCsPluginInterface::UnloadPlugIns - Plugins Unloaded") );
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::InstantiatePlugInFromImpUidL
// Instantiates plugin
// ----------------------------------------------------------------------------
CCsPlugin* CCsPluginInterface::InstantiatePlugInFromImpUidL( const TUid& aImpUid,
        CCsServer* aCsServer )
{
    CCsPlugin* plugin = CCsPlugin::NewL(aImpUid, aCsServer );

    PRINT( _L("CCsPluginInterface::InstantiatePlugInFromImpUidL - Plugin created") );

    return plugin;
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::GetConversationsL
// Forwards the request to the plugin instance ,
// to get the all conversation entry list from List of plugins
// ----------------------------------------------------------------------------
void CCsPluginInterface::GetConversationsL()
{
    for(TInt iloop=0 ; iloop < iCsPluginInstances.Count(); iloop++)
        iCsPluginInstances[iloop]->GetConversationsL();
}

// ----------------------------------------------------------------------------
// CCsPluginInterface::GetPlugin
// Get Plugins count
// ----------------------------------------------------------------------------
TInt CCsPluginInterface:: GetPluginsCount()
{
    return iCsPluginInstances.Count();
}

//EOF
