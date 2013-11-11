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

// INCLUDE FILES
#include <common.h>

#include "CCPixNPSearcherInterface.h"
#include "utf.h"

#include "e32base.h"

#include "NPUtils.h"
using namespace nputils; 

// ============================ MEMBER FUNCTIONS ===============================

CCPixNPSearcherInterface::~CCPixNPSearcherInterface()
    {
    }

// -----------------------------------------------------------------------------

void CCPixNPSearcherInterface::InitInterfaceL()
    {
    const NPUTF8 *CPixNPSearcherMethodNames[] =
    	{
        "setObserver",
        "search",
        "getDocument",
        "setAnalyzer",
        "cancel",
        "isActive"
    	};
    SetIdentifiersL( NULL, 0, 
    				 CPixNPSearcherMethodNames, sizeof(CPixNPSearcherMethodNames)/sizeof(CPixNPSearcherMethodNames[0]));
    }

// -----------------------------------------------------------------------------

bool CCPixNPSearcherInterface::InvokeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result)
    {
    
    VOID_TO_NPVARIANT( *result );
    if ( name == iMethodIdentifiers[0] ) // setObserver
        {
        if ( argCount >= 1 && NPVARIANT_IS_OBJECT( args[0] ) ) 
        	{
			TRAPD(err, 
				NPObject* observer = NPVARIANT_TO_OBJECT( args[0] ); 
				SetObserverL( observer );
				); 
			// TODO: Report err
        	}
        else 
        	{
        	// TODO: Exception handling: NotEnoughtArgumentsException & IllegalArgumentExceptions
        	}
        return true; 
        }
    else if ( name == iMethodIdentifiers[1] ) // search
        {
        PERFORMANCE_LOG_START("CCPixNPSearcherInterface::InvokeL");
        PERFORMANCE_LOG_MESSAGE(_L("----------New search----------"));

        if ( argCount >= 1 ) 
        	{
			TRAPD(err, 
				HBufC16* query = VariantToHBufC16LC( args[0] );
				SearchL( *query);
				CleanupStack::PopAndDestroy( query ); 
				); 
			// TODO: Report err
        	}
        else 
        	{
        	// TODO: Exception handling: NotEnoughtArgumentsException & IllegalArgumentExceptions
        	}

        return true;
        }
    else if ( name == iMethodIdentifiers[2] ) // getDocument
        {
    	if (argCount >= 1) 
    		{
			TInt index = VariantToInt( args[0] );
			TRAPD(err, GetDocumentL(index));
			// TODO: report err
    		}
		// TODO: Report err
        return true;
        }
    else if ( name == iMethodIdentifiers[3] ) // setAnalyzer
        {
        if ( argCount >= 1 ) 
         	{
 			TRAPD(err, 
 				HBufC16* query = VariantToHBufC16LC( args[0] );
 				SetAnalyzerL( *query );
 				CleanupStack::PopAndDestroy( query ); 
 				); 
 			// TODO: Report err
         	}
         else 
         	{
         	// TODO: Exception handling: NotEnoughtArgumentsException & IllegalArgumentExceptions
         	}
        return true;
        }
    else if ( name == iMethodIdentifiers[4] ) // cancel
        {
		TRAPD(err,
			Cancel(); 
			); 
		// TODO: Report err
        return true;
        }
    else if ( name == iMethodIdentifiers[5] ) // isActive
        {
		TRAPD(err,
			BOOLEAN_TO_NPVARIANT(IsActive(),  *result); 
			); 
		// TODO: Report err
        return true;
        }
    
    return false;
    }

// -----------------------------------------------------------------------------

bool CCPixNPSearcherInterface::GetProperty (NPIdentifier name, NPVariant *variant)
    {
    // default variant value maps to javascript undefined
    VOID_TO_NPVARIANT(*variant);
    return false;
    }

// -----------------------------------------------------------------------------

bool CCPixNPSearcherInterface::SetPropertyL( NPIdentifier name, NPVariant *variant )
    {
    return false;
    }

// -----------------------------------------------------------------------------
// NPClass Function

NPObject *CPixNPSearcherAllocate ()
    {
    CPixNPSearcherObject *newInstance = (CPixNPSearcherObject *)User::Alloc (sizeof(CPixNPSearcherObject));       
    return (NPObject *)newInstance;
    }

void CPixNPSearcherInvalidate (CPixNPSearcherObject *obj)
    {
    obj->plugin->Invalidate();
    }

void CPixNPSearcherDeallocate (CPixNPSearcherObject* obj) 
    {    
    obj->plugin->Deallocate();
    User::Free ((void *)obj);
    }    
 
bool CPixNPSearcherHasMethod(CPixNPSearcherObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasMethod(name);
    }

bool CPixNPSearcherInvokeFunctionL(CPixNPSearcherObject* obj, NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result)
    {
    return obj->plugin->InvokeL(name, args, argCount, result);
    }    

bool CPixNPSearcherHasProperty(CPixNPSearcherObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasProperty(name);
    }    
    
bool CPixNPSearcherGetProperty (CPixNPSearcherObject* obj, NPIdentifier name, NPVariant *variant)
    {
    return obj->plugin->GetProperty(name,variant);
    }

void CPixNPSearcherSetProperty (CPixNPSearcherObject* obj, NPIdentifier name, NPVariant *variant)
    {
    TRAP_IGNORE(obj->plugin->SetPropertyL(name,variant));
    }

//  End of File
