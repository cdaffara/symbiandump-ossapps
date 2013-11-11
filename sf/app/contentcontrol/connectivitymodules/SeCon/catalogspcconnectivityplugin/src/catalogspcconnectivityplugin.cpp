/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This plugin enables the Catalogs PC client to communicate 
*                with the Catalogs engine
*
*/



#include "catalogspcconnectivityplugin.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCatalogsPCConnectivityPlugin::CCatalogsPCConnectivityPlugin()
    {
    }

// ---------------------------------------------------------------------------
// Empty
// ---------------------------------------------------------------------------
//
void CCatalogsPCConnectivityPlugin::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Standard symbian object constructing first phase
// ---------------------------------------------------------------------------
//
EXPORT_C CCatalogsPCConnectivityPlugin* CCatalogsPCConnectivityPlugin::NewL()
    {
    CCatalogsPCConnectivityPlugin* self = 
        CCatalogsPCConnectivityPlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Standard symbian object constructing first phase
// ---------------------------------------------------------------------------
//
EXPORT_C CCatalogsPCConnectivityPlugin* CCatalogsPCConnectivityPlugin::NewLC()
    {
    CCatalogsPCConnectivityPlugin* self = 
        new( ELeave ) CCatalogsPCConnectivityPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCatalogsPCConnectivityPlugin::~CCatalogsPCConnectivityPlugin()
    {
    iClient.Close();
    }


// ---------------------------------------------------------------------------
// Connect to Catalogs engine pc client front-end
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCatalogsPCConnectivityPlugin::Connect()
    {
    return iClient.Connect();
    }

// ---------------------------------------------------------------------------
// Close the connection to Catalogs engine pc client front-end
// ---------------------------------------------------------------------------
//
EXPORT_C void CCatalogsPCConnectivityPlugin::Close()
    {
    iClient.Close();
    }

// ---------------------------------------------------------------------------
// Give all MIME types supported by this component
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<TDataType> CCatalogsPCConnectivityPlugin::
    SupportedMimeTypesL()
    {
    return iClient.SupportedMimeTypesL();
    }

// ---------------------------------------------------------------------------
// Data package from PC Client
// ---------------------------------------------------------------------------
//
EXPORT_C void CCatalogsPCConnectivityPlugin::PutDataL( 
    const TDataType& aMimeType, const CBufFlat& aData )
    {
    iClient.PutDataL( aMimeType, aData );
    }

// ---------------------------------------------------------------------------
// Data request from PC Client
// ---------------------------------------------------------------------------
//
EXPORT_C void CCatalogsPCConnectivityPlugin::GetDataL( 
    TDataType& aMimeType, CBufFlat& aData )
    {
    iClient.GetDataL( aMimeType, aData );
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Dll entry point
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll()
    {
    return( KErrNone );
    }

