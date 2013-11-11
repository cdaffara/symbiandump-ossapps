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
* Description: Inline methods for class CFSMailPlugin. 
*
*/


// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::CFSMailPlugin()
    {

    }
// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::~CFSMailPlugin()
    {
		REComSession::DestroyedImplementation(iDtor_ID_Key);
    }
    
// -----------------------------------------------------------------------------
// CFSMailPlugin::ListImplementationsL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::ListImplementationsL(RPointerArray<CImplementationInformation>& aInfo)
    {
    
    REComSession::ListImplementationsL(KFSMailPluginInterface,aInfo);

    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin* CFSMailPlugin::NewL(TUid aUid)
	{

		return static_cast<CFSMailPlugin*>(REComSession::CreateImplementationL(
							aUid,_FOFF( CFSMailPlugin, iDtor_ID_Key)));
    }
    
// -----------------------------------------------------------------------------
// CFSMailPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin* CFSMailPlugin::NewL()
	{
	
    //Set up the interface find for the default resolver.
    TEComResolverParams ResolverParams;
    ResolverParams.SetDataType( KFSPluginInterfaceImplementationType());
    ResolverParams.SetWildcardMatch( ETrue );        // Allow wildcard matching
        
    return REINTERPRET_CAST( CFSMailPlugin*, 
				REComSession::CreateImplementationL( 
						KFSMailPluginInterface, 
						_FOFF( CFSMailPlugin, iDtor_ID_Key ),
	          			ResolverParams ) );

    }

            
// End of File
