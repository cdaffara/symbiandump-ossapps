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
* Description: 
*
*/
#ifndef OBJECTINTERFACE_H
#define OBJECTINTERFACE_H

#include <e32base.h>

#ifdef __S60_50__
#include <npscript.h>
#include <npapi.h>
#else
#include <stdint.h>
#include <JavaScriptCore/npruntime.h>
#endif

/**
 * Provides utility functionality to make the commmunication between 
 * NP Runtime interface and S60 functionality more convinient. 
 */
class CObjectInterface : public CBase
{
	// FIXME: This is an antipattern! Move required methods into 
    // 

public:
	virtual ~CObjectInterface();

public:
	void SetInstance( NPP instance, NPObject* aNpObject ) { iInstanceHandle = instance; iNpObject = aNpObject; }
	virtual void Deallocate();
	
	bool HasMethod( NPIdentifier name );
	virtual bool HasProperty( NPIdentifier name );
	virtual bool InvokeL( NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result ) = 0;
	virtual bool GetProperty( NPIdentifier name, NPVariant *variant ) = 0;
	virtual bool SetPropertyL( NPIdentifier name, NPVariant *variant ) = 0;
	
protected:
	CObjectInterface();
	void SetIdentifiersL( const NPUTF8** aPropertyNames, TInt aPropertyCount, const NPUTF8** aMethodNames, TInt aMethodCount );
	
protected:
	NPP iInstanceHandle;
	NPObject* iNpObject; // For raising exceptions and reference counting
	
    NPIdentifier* iPropertyIdentifiers;
    NPIdentifier* iMethodIdentifiers;
    
    TInt          iPropertyCount;
    TInt          iMethodCount;
};

#endif /*OBJECTINTERFACE_H*/
