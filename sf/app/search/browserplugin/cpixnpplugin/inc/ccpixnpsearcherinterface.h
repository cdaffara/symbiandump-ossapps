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
#ifndef CCPIXNPSEARCHERINTERFACE_H_
#define CCPIXNPSEARCHERINTERFACE_H_

//  INCLUDES
#include "CObjectInterface.h"
#include "ICPixNPSearcher.h"

// CLASS DECLARATION

/**
 * CCPixNPSearcherInterface
 * Provides the interface between browser and the actual functionality implemented 
 * in CCPixNPSearcher. Calls MCPixNPSearcher methods of the implementing class
 * as a response to method invokation requests from the browser.
 */
class CCPixNPSearcherInterface : public CObjectInterface, public MCPixNPSearcher
    {
 public:
    virtual ~CCPixNPSearcherInterface();

 public:
    bool InvokeL( NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result );
    bool GetProperty( NPIdentifier name, NPVariant *variant );
    bool SetPropertyL( NPIdentifier name, NPVariant *variant );
    
    virtual void Invalidate() = 0; 
    
 protected:
	void InitInterfaceL();
    };

    
/**
 ********************************************************************************
     CPixNPSearcherObject Plugin API methods
 *********************************************************************************
 */
 typedef struct
     {
     NPObject object;
     CCPixNPSearcherInterface *plugin;
     } CPixNPSearcherObject;

 NPObject* CPixNPSearcherAllocate();

 void CPixNPSearcherInvalidate( CPixNPSearcherObject *obj );

 void CPixNPSearcherDeallocate( CPixNPSearcherObject *obj );

 bool CPixNPSearcherHasMethod( CPixNPSearcherObject *obj, NPIdentifier name );

 bool CPixNPSearcherInvokeFunctionL( CPixNPSearcherObject* obj, NPIdentifier name,
                                      NPVariant *args, uint32_t argCount, NPVariant *result );

 bool CPixNPSearcherHasProperty( CPixNPSearcherObject *obj, NPIdentifier name );

 bool CPixNPSearcherGetProperty ( CPixNPSearcherObject *obj, NPIdentifier name,
                                    NPVariant *variant);

 void CPixNPSearcherSetProperty( CPixNPSearcherObject *obj, NPIdentifier name,
                                   NPVariant *variant);

 static NPClass _CPixNPSearcherClass =
     {
     0,
     (NPAllocateFunctionPtr) CPixNPSearcherAllocate,
     (NPDeallocateFunctionPtr) CPixNPSearcherDeallocate,
     (NPInvalidateFunctionPtr) CPixNPSearcherInvalidate,
     (NPHasMethodFunctionPtr) CPixNPSearcherHasMethod,
     (NPInvokeFunctionPtr) CPixNPSearcherInvokeFunctionL,
     (NPInvokeDefaultFunctionPtr) 0,
     (NPHasPropertyFunctionPtr) CPixNPSearcherHasProperty,
     (NPGetPropertyFunctionPtr) CPixNPSearcherGetProperty,
     (NPSetPropertyFunctionPtr) CPixNPSearcherSetProperty,
     (NPRemovePropertyFunctionPtr) 0
     };

 /**
  * static instance of the function pointer table
  */
 static NPClass* CPixNPSearcherClass = &_CPixNPSearcherClass;

#endif /*CCPIXNPSEARCHERINTERFACE_H_*/
