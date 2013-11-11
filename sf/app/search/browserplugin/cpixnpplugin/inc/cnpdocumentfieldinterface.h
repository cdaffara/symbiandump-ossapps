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
#ifndef CNPDOCUMENTFIELDINTERFACE_H_
#define CNPDOCUMENTFIELDINTERFACE_H_

//  INCLUDES
#include "CObjectInterface.h"
#include "INPDocumentField.h"

// CLASS DECLARATION

/**
 * Provides the interface between browser and the actual functionality implemented 
 * in CCPixNPSearcher. Calls MCPixNPPlugin methods of the implementing class
 * as a response to method invokation requests from the browser.
 */
class CNPDocumentFieldInterface : public CObjectInterface, public MNPDocumentField
    {
 public:
    virtual ~CNPDocumentFieldInterface();

 public:
    bool InvokeL( NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result );
    bool GetProperty( NPIdentifier name, NPVariant *variant );
    bool SetPropertyL( NPIdentifier name, NPVariant *variant );
    
 protected:
	void InitInterfaceL();
    };

    
/**
 ********************************************************************************
     NPDocumentFieldObject Plugin API methods
 *********************************************************************************
 */
struct NPDocumentFieldObject
     {
     NPObject object;
     CNPDocumentFieldInterface *plugin;
     };

 NPObject* NPDocumentFieldAllocate();

 void NPDocumentFieldInvalidate();

 void NPDocumentFieldDeallocate( NPDocumentFieldObject *obj );

 bool NPDocumentFieldHasMethod( NPDocumentFieldObject *obj, NPIdentifier name );

 bool NPDocumentFieldInvokeFunctionL( NPDocumentFieldObject* obj, NPIdentifier name,
                                      NPVariant *args, uint32_t argCount, NPVariant *result );

 bool NPDocumentFieldHasProperty( NPDocumentFieldObject *obj, NPIdentifier name );

 bool NPDocumentFieldGetProperty ( NPDocumentFieldObject *obj, NPIdentifier name,
                                    NPVariant *variant);

 void NPDocumentFieldSetProperty( NPDocumentFieldObject *obj, NPIdentifier name,
                                   NPVariant *variant);

 static NPClass _NPDocumentFieldClass =
     {
     0,
     (NPAllocateFunctionPtr) NPDocumentFieldAllocate,
     (NPDeallocateFunctionPtr) NPDocumentFieldDeallocate,
     (NPInvalidateFunctionPtr) NPDocumentFieldInvalidate,
     (NPHasMethodFunctionPtr) NPDocumentFieldHasMethod,
     (NPInvokeFunctionPtr) NPDocumentFieldInvokeFunctionL,
     (NPInvokeDefaultFunctionPtr) 0,
     (NPHasPropertyFunctionPtr) NPDocumentFieldHasProperty,
     (NPGetPropertyFunctionPtr) NPDocumentFieldGetProperty,
     (NPSetPropertyFunctionPtr) NPDocumentFieldSetProperty,
     (NPRemovePropertyFunctionPtr) 0
     };

 /**
  * static instance of the function pointer table
  */
 static NPClass* NPDocumentFieldClass = &_NPDocumentFieldClass;
 

#endif /*CNPDOCUMENTFIELDINTERFACE_H_*/
