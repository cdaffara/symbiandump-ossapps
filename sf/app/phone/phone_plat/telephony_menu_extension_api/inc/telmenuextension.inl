/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline functions for CTelMenuExtension.
*
*/

#include <ecom/ecom.h>

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
inline CTelMenuExtension::~CTelMenuExtension()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline CTelMenuExtension* CTelMenuExtension::NewL( 
    TUid aImplementationUid ) 
    {
    TAny* ptr( NULL );
    TInt32 keyOffset = _FOFF( CTelMenuExtension, iDtor_ID_Key );
    ptr = REComSession::CreateImplementationL(
        aImplementationUid,
        keyOffset );
    
    CTelMenuExtension* plugin = 
        reinterpret_cast <CTelMenuExtension*> (ptr);
    return plugin;
    }    

//  End of File  
