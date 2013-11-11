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

#include "CNPDocumentFieldInterface.h"

#include <utf.h>
#include "CNPDocumentFieldInterface.h"

#include "NPUtils.h"
using namespace nputils; 

// ============================ MEMBER FUNCTIONS ===============================

CNPDocumentFieldInterface::~CNPDocumentFieldInterface()
    {
    }


void CNPDocumentFieldInterface::InitInterfaceL()
    {
    const NPUTF8 *NPDocumentFieldMethodNames[] =
    	{ 
        "getName",
        "getValue"
    	};

    SetIdentifiersL( NULL, 0,
    				 NPDocumentFieldMethodNames, sizeof(NPDocumentFieldMethodNames)/sizeof(NPDocumentFieldMethodNames[0]));
    }

// -----------------------------------------------------------------------------

bool CNPDocumentFieldInterface::InvokeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result)
    {
    VOID_TO_NPVARIANT(*result);
    if ( name == iMethodIdentifiers[0] ) // getName
    	{
    	TRAP_IGNORE(
    		DescriptorToVariantL( Name(), *result);
    		);
    	// TODO: Report error
    	return true;
    	}
    else if ( name == iMethodIdentifiers[1] ) // getValue
    	{
    	TRAP_IGNORE(
    		DescriptorToVariantL( Value(), *result);
    		); 
    	// TODO: Report error
    	return true;
    	}
    return false;
    }

// -----------------------------------------------------------------------------

bool CNPDocumentFieldInterface::GetProperty (NPIdentifier name, NPVariant *variant)
    {
    // default variant value maps to javascript undefined
    VOID_TO_NPVARIANT(*variant);
    return false;
    }

// -----------------------------------------------------------------------------

bool CNPDocumentFieldInterface::SetPropertyL( NPIdentifier name, NPVariant *variant )
    {
    return false;
    }

// -----------------------------------------------------------------------------
// NPClass Function

NPObject *NPDocumentFieldAllocate ()
    {
    NPDocumentFieldObject *newInstance = (NPDocumentFieldObject *)User::Alloc (sizeof(NPDocumentFieldObject));       
    return (NPObject *)newInstance;
    }

void NPDocumentFieldInvalidate ()
    {
    }

void NPDocumentFieldDeallocate (NPDocumentFieldObject* obj) 
    {
    obj->plugin->Deallocate();
    User::Free ((void *)obj);
    }    
 
bool NPDocumentFieldHasMethod(NPDocumentFieldObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasMethod(name);
    }

bool NPDocumentFieldInvokeFunctionL(NPDocumentFieldObject* obj, NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result)
    {
    return obj->plugin->InvokeL(name, args, argCount, result);
    }    

bool NPDocumentFieldHasProperty(NPDocumentFieldObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasProperty(name);
    }    
    
bool NPDocumentFieldGetProperty (NPDocumentFieldObject* obj, NPIdentifier name, NPVariant *variant)
    {
    return obj->plugin->GetProperty(name,variant);
    }

void NPDocumentFieldSetProperty (NPDocumentFieldObject* obj, NPIdentifier name, NPVariant *variant)
    {
    TRAP_IGNORE(obj->plugin->SetPropertyL(name,variant));
    }

//  End of File
