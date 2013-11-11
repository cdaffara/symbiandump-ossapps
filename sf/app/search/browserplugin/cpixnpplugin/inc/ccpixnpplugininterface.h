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


#ifndef CPIXNPPLUGININTERFACE_H
#define CPIXNPPLUGININTERFACE_H

#include "e32def.h"

//  INCLUDES
#include "CObjectInterface.h"
#include "idl/ICPixNPPlugin.h"

// CLASS DECLARATION

/**
 * Provides the interface between browser and the actual functionality implemented
 * in CCPixNPPlugin. Calls MCPixNPPlugin methods of the implementing class
 * as a response to method invokation requests from the browser.
 */
class CCPixNPPluginInterface : public CObjectInterface, public MCPixNPPlugin
    {
public:
    virtual ~CCPixNPPluginInterface();

public:
    bool InvokeL( NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result );
    bool GetProperty( NPIdentifier name, NPVariant *variant );
    bool SetPropertyL( NPIdentifier name, NPVariant *variant );

protected:
    void InitInterfaceL();

private:
    // calls RSearchServerSession::DefineVolume()
    void InvokeDefineVolumeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result);
    // calls RSearchServerSession::UnDefineVolume()
    void InvokeUnDefineVolumeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result);
    };
    

/**
 ********************************************************************************
     CPixPlugin API methods
 *********************************************************************************
 */
 typedef struct
     {
     NPObject object;
     CCPixNPPluginInterface *plugin;
     } CPixPluginObject;

 NPObject *CPixPluginAllocate();

 void CPixPluginInvalidate();

 void CPixPluginDeallocate( CPixPluginObject *obj );

 bool CPixPluginHasMethod( CPixPluginObject *obj, NPIdentifier name );

 bool CPixPluginInvokeFunctionL( CPixPluginObject* obj, NPIdentifier name,
                                      NPVariant *args, uint32_t argCount, NPVariant *result );

 bool CPixPluginHasProperty( CPixPluginObject *obj, NPIdentifier name );

 bool CPixPluginGetProperty ( CPixPluginObject *obj, NPIdentifier name,
                                    NPVariant *variant);

 void CPixPluginSetProperty( CPixPluginObject *obj, NPIdentifier name,
                                   NPVariant *variant);

 static NPClass _CPixPluginClass =
     {
     0,
     (NPAllocateFunctionPtr) CPixPluginAllocate,
     (NPDeallocateFunctionPtr) CPixPluginDeallocate,
     (NPInvalidateFunctionPtr) CPixPluginInvalidate,
     (NPHasMethodFunctionPtr) CPixPluginHasMethod,
     (NPInvokeFunctionPtr) CPixPluginInvokeFunctionL,
     (NPInvokeDefaultFunctionPtr) 0,
     (NPHasPropertyFunctionPtr) CPixPluginHasProperty,
     (NPGetPropertyFunctionPtr) CPixPluginGetProperty,
     (NPSetPropertyFunctionPtr) CPixPluginSetProperty,
     (NPRemovePropertyFunctionPtr) 0
     };

 /**
  * static instance of the function pointer table
  */
 static NPClass* CPixPluginClass = &_CPixPluginClass;
    
#endif // CPIXBROWSERPLUGININTERFACE_H
