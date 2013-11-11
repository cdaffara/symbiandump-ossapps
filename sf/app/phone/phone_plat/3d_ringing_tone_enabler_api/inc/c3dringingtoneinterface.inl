/*
* Copyright (c) 2005,2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline functions for C3DRingingToneInterface class
*
*/



// INCLUDE FILES
#include    <ecom/ecom.h>

// CONSTANTS
_LIT8( KCoUtlResolverData, "default" );


// ============================ MEMBER FUNCTIONS ===============================
 
// -----------------------------------------------------------------------------
// C3DRingingToneInterface::NewL
// ECom is used to create instance.
// -----------------------------------------------------------------------------
//
inline C3DRingingToneInterface* C3DRingingToneInterface::NewL( TUid /*aImplementationUid*/ )
    {
    
    const TUid K3DRingingToneInterfaceDefinitionUid = 
        { K3DRingingToneInterfaceUid };

    TEComResolverParams params;
    params.SetDataType( KCoUtlResolverData );
    params.SetWildcardMatch( EFalse );
 
    RImplInfoPtrArray pluginArray;
    //List all plugins which implement C3DRingingToneInterface
    REComSession::ListImplementationsL( K3DRingingToneInterfaceDefinitionUid, pluginArray );

    if( pluginArray.Count() )
        {
        TAny* ptr = 
            REComSession::CreateImplementationL(
                K3DRingingToneInterfaceDefinitionUid,
                _FOFF( C3DRingingToneInterface, iDtor_ID_Key ), 
                NULL,
                params );
 
        pluginArray.ResetAndDestroy(); 
        return reinterpret_cast< C3DRingingToneInterface* >( ptr );
        }
    else
        {
        return NULL;
        }   
    }


// -----------------------------------------------------------------------------
// C3DRingingToneInterface::~C3DRingingToneInterface
// ECom is used to delete instance..
// REComSession::FinalClose() in the application which loads this plugin
// -----------------------------------------------------------------------------

inline C3DRingingToneInterface::~C3DRingingToneInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

        
//  End of File
