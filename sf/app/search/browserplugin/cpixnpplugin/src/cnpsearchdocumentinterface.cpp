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
#include "CNPSearchDocumentInterface.h"

#include <utf.h>
#include "CNPDocumentFieldInterface.h"

#include "NPUtils.h"
using namespace nputils; 

// ============================ MEMBER FUNCTIONS ===============================

CNPSearchDocumentInterface::~CNPSearchDocumentInterface()
    {
    }


void CNPSearchDocumentInterface::InitInterfaceL()
    {
    const NPUTF8 *NPSearchDocumentMethodNames[] =
    	{ 
        "getFieldByName",
        "getId",
        "getAppClass",
        "getExcerpt",
        "getFieldCount"
        "getFieldByIndex",
    	};

    SetIdentifiersL( NULL, 0,
    				 NPSearchDocumentMethodNames, sizeof(NPSearchDocumentMethodNames)/sizeof(NPSearchDocumentMethodNames[0]));
    }

// -----------------------------------------------------------------------------

bool CNPSearchDocumentInterface::InvokeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result)
    {
    VOID_TO_NPVARIANT(*result);
    if ( name == iMethodIdentifiers[0] ) // getFieldByName
    	{
        if ( argCount >= 1 ) 
    		{
    		TRAP_IGNORE( 
				HBufC16* buf = VariantToHBufC16LC( args[0] );
				if ( buf ) 
					{
					NPDocumentFieldObject* ret = FieldL( *buf );
					CleanupStack::PopAndDestroy( buf ); 
					if ( ret != NULL )
						{
						OBJECT_TO_NPVARIANT( ( NPObject* ) &ret->object, *result ); 
						}
					else 
						{
						NULL_TO_NPVARIANT( *result );
						}
					}
				);
    		// TODO: report err
			return true; 
			}
    	}
    else if ( name == iMethodIdentifiers[1] ) // getId
    	{
    	TRAP_IGNORE(
    		DescriptorToVariantL( Id(), *result);
    		);
		// TODO: report err
    	return true;
    	}
    else if ( name == iMethodIdentifiers[2] ) // getAppClass
    	{
    	TRAP_IGNORE(
    		DescriptorToVariantL( AppClass(), *result);
			);
		// TODO: report err
    	return true;
    	}
    else if ( name == iMethodIdentifiers[3] ) // getExcerpt
    	{
    	TRAP_IGNORE(
			DescriptorToVariantL( Excerpt(), *result);
			);
		// TODO: report err
    	return true;
    	}
    else if ( name == iMethodIdentifiers[4] ) // getFieldCount
    	{
    	INT32_TO_NPVARIANT( FieldCount(), *result);
    	return true;
    	}
    else if ( name == iMethodIdentifiers[5] ) // getFieldByIndex
    	{
    	if (argCount >= 1) 
    		{
    		TRAP_IGNORE( 
				TInt index = VariantToInt( args[0] );
				NPDocumentFieldObject* ret = FieldL( index );
				if ( ret != NULL ) 
					{
					OBJECT_TO_NPVARIANT( ( NPObject* ) &ret->object, *result ); 
					}
				else 
					{
					NULL_TO_NPVARIANT( *result ); 
					}
				); 
			// TODO: report err
    		}
    	return true;
    	}
    
    return false;
    }

// -----------------------------------------------------------------------------

bool CNPSearchDocumentInterface::GetProperty (NPIdentifier name, NPVariant *variant)
    {
    // default variant value maps to javascript undefined
    VOID_TO_NPVARIANT(*variant);
    return false;
    }

// -----------------------------------------------------------------------------

bool CNPSearchDocumentInterface::SetPropertyL( NPIdentifier name, NPVariant *variant )
    {
    return false;
    }

// -----------------------------------------------------------------------------
// NPClass Function

NPObject *NPSearchDocumentAllocate ()
    {
    NPSearchDocumentObject *newInstance = (NPSearchDocumentObject *)User::Alloc (sizeof(NPSearchDocumentObject));       
    return (NPObject *)newInstance;
    }

void NPSearchDocumentInvalidate ()
    {
    }

void NPSearchDocumentDeallocate (NPSearchDocumentObject* obj) 
    {
    obj->plugin->Deallocate();
    User::Free ((void *)obj);
    }    
 
bool NPSearchDocumentHasMethod(NPSearchDocumentObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasMethod(name);
    }

bool NPSearchDocumentInvokeFunctionL(NPSearchDocumentObject* obj, NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result)
    {
    return obj->plugin->InvokeL(name, args, argCount, result);
    }    

bool NPSearchDocumentHasProperty(NPSearchDocumentObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasProperty(name);
    }    
    
bool NPSearchDocumentGetProperty (NPSearchDocumentObject* obj, NPIdentifier name, NPVariant *variant)
    {
    return obj->plugin->GetProperty(name,variant);
    }

void NPSearchDocumentSetProperty (NPSearchDocumentObject* obj, NPIdentifier name, NPVariant *variant)
    {
    TRAP_IGNORE(obj->plugin->SetPropertyL(name,variant));
    }

//  End of File
