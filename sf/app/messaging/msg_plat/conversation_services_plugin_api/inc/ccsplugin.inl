/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom Conversation Server interface definition
 *
*/


// INCLUDE FILES
#include <ecom/ecom.h>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCsPlugin::NewL
// Two phase construction
// ----------------------------------------------------------------------------
inline CCsPlugin* CCsPlugin::NewL( TUid aImpUid,
        MCsPluginEventObserver* aMCsPluginEventObserver )
    {
    CCsPlugin* self = CCsPlugin::NewLC( aImpUid, aMCsPluginEventObserver );	
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// CCsPlugin::NewLC
// Two phase construction
// ----------------------------------------------------------------------------
inline CCsPlugin* CCsPlugin::NewLC( TUid aImpUid,
        MCsPluginEventObserver* aMCsPluginEventObserver )
    {
    TAny* implementation = REComSession::CreateImplementationL ( aImpUid,
            _FOFF ( CCsPlugin, iDtor_ID_Key ),
            aMCsPluginEventObserver );
    CCsPlugin* self = REINTERPRET_CAST( CCsPlugin*, implementation );
    self->iPluginId = aImpUid;
    CleanupStack::PushL( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CCsPlugin::ConstructL
// Destructor
// ----------------------------------------------------------------------------
inline CCsPlugin::~CCsPlugin()
    {
    REComSession::DestroyedImplementation ( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CCsPlugin::PluginId
// Returns the plugin id
// ----------------------------------------------------------------------------
inline TUid CCsPlugin::PluginId() const
    {  
    return iPluginId;
    }

// End of file

