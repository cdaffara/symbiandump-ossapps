/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This is partial interface implementation which is 
*              used by plugins. 
* 
*/



// INCLUDE FILES

#include <ecom/ecom.h>       // declares E-com framework classes


// -----------------------------------------------------------------------------
// CMaptileGeocoderPlugin::~CMaptileGeocoderPlugin()
// Destroys the plugin implementation
// -----------------------------------------------------------------------------
//
inline CMaptileGeocoderPlugin::~CMaptileGeocoderPlugin()
{
    REComSession::DestroyedImplementation( iDtorKey );
}


// -----------------------------------------------------------------------------
// CMaptileGeocoderPlugin::NewL()
// Creates the instance of CMaptileGeocoderPlugin concrete implementation.
// -----------------------------------------------------------------------------
//
inline CMaptileGeocoderPlugin* CMaptileGeocoderPlugin::NewL()
{
    CMaptileGeocoderPlugin* self = reinterpret_cast<CMaptileGeocoderPlugin*>(
        REComSession::CreateImplementationL(
            KMaptileGeoCoderPluginUid, _FOFF( CMaptileGeocoderPlugin,iDtorKey ) ) );

    return self;
}

// End of file
