/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Inline functions of CAgnExternalInterface
*
*/



/**
Finds and loads a plugin for the mime type provided.
@param aMimeType An 8 bit descriptor used for plugin recognition.
@return a pointer to a new instance of the appropriate plugin.
*/
inline CAgnExternalInterface* CAgnExternalInterface::NewL(const TDesC8& aMimeType)
    {
	const TUid KAgnExternalInterfaceUid = {0x10206C81};

	TEComResolverParams resolverParams; 
	resolverParams.SetDataType(aMimeType);
	TAny* ptr = REComSession::CreateImplementationL(KAgnExternalInterfaceUid, _FOFF(CAgnExternalInterface, iDtorIdKey), resolverParams);

	return reinterpret_cast<CAgnExternalInterface*>(ptr);
	}

/**
This destructor handles the destruction of the plugin loaded by a call to NewL.
@internalTechnology
*/
inline CAgnExternalInterface::~CAgnExternalInterface()
	{
	REComSession::DestroyedImplementation(iDtorIdKey);
	}
	
//End of file
