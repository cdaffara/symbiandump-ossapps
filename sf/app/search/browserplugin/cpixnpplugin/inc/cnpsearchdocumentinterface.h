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
#ifndef CNPSEARCHDOCUMENTINTERFACE_H_
#define CNPSEARCHDOCUMENTINTERFACE_H_

//  INCLUDES
#include "CObjectInterface.h"
#include "INPSearchDocument.h"

// CLASS DECLARATION

/**
 * Provides the interface between browser and the actual functionality implemented 
 * in CNPSearchDocument. Calls MNPSearchDocument methods of the implementing class
 * as a response to method invokation requests from the browser.
 */
class CNPSearchDocumentInterface : public CObjectInterface, public MNPSearchDocument
    {
 public:
    virtual ~CNPSearchDocumentInterface();

 public:
    bool InvokeL( NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result );
    bool GetProperty( NPIdentifier name, NPVariant *variant );
    bool SetPropertyL( NPIdentifier name, NPVariant *variant );
    
 protected:
	void InitInterfaceL();
    };

    
/**
 ********************************************************************************
     NPSearchDocumentObject Plugin API methods
 *********************************************************************************
 */
 struct NPSearchDocumentObject
     {
     NPObject object;
     CNPSearchDocumentInterface *plugin;
     };

 NPObject* NPSearchDocumentAllocate();

 void NPSearchDocumentInvalidate();

 void NPSearchDocumentDeallocate( NPSearchDocumentObject *obj );

 bool NPSearchDocumentHasMethod( NPSearchDocumentObject *obj, NPIdentifier name );

 bool NPSearchDocumentInvokeFunctionL( NPSearchDocumentObject* obj, NPIdentifier name,
                                      NPVariant *args, uint32_t argCount, NPVariant *result );

 bool NPSearchDocumentHasProperty( NPSearchDocumentObject *obj, NPIdentifier name );

 bool NPSearchDocumentGetProperty ( NPSearchDocumentObject *obj, NPIdentifier name,
                                    NPVariant *variant);

 void NPSearchDocumentSetProperty( NPSearchDocumentObject *obj, NPIdentifier name,
                                   NPVariant *variant);

 static NPClass _NPSearchDocumentClass =
     {
     0,
     (NPAllocateFunctionPtr) NPSearchDocumentAllocate,
     (NPDeallocateFunctionPtr) NPSearchDocumentDeallocate,
     (NPInvalidateFunctionPtr) NPSearchDocumentInvalidate,
     (NPHasMethodFunctionPtr) NPSearchDocumentHasMethod,
     (NPInvokeFunctionPtr) NPSearchDocumentInvokeFunctionL,
     (NPInvokeDefaultFunctionPtr) 0,
     (NPHasPropertyFunctionPtr) NPSearchDocumentHasProperty,
     (NPGetPropertyFunctionPtr) NPSearchDocumentGetProperty,
     (NPSetPropertyFunctionPtr) NPSearchDocumentSetProperty,
     (NPRemovePropertyFunctionPtr) 0
     };

 /**
  * static instance of the function pointer table
  */
 static NPClass* NPSearchDocumentClass = &_NPSearchDocumentClass;
 
#endif /*CNPSEARCHDOCUMENTINTERFACE_H_*/
