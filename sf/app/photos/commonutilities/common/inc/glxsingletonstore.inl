/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   TLS store for singleton objects
*
*/



#include <glxlog.h>
#include <glxpanic.h>

// -----------------------------------------------------------------------------
// Return new or existing instance
// -----------------------------------------------------------------------------
//
template <class T>
T* CGlxSingletonStore::InstanceL( T* (*aFactoryFuncL)() )
	{
	// Get new or existing pointer to store.
	// InstanceL stores the pointer in TLS
    CGlxSingletonStore* store = InstanceLC();
    	
    // Try to find existing object of type T
    TInt count = store->iSingletons.Count();
    TInt i = 0;
    T* obj = NULL;
    while (i < count && !obj) 
        {
        obj = dynamic_cast<T*>(store->iSingletons[i].iObject);
        if (obj)
            {
            // Found an existing object of type T
            GLX_LOG_INFO1("CGlxSingletonStore::InstanceL, Found existing object %x", obj);
            // Add another reference
            store->iSingletons[i].iReferenceCount++;
            }
        i++;
        }
    
    // Create new object if one did not exist
    if ( !obj ) 
        {
        // If these calls leave, Cleanup stack will make sure tls 
        // pointer is cleared, if there are no other clients for CGlxSingletonStore
        // class
        
        // Reserve space in the array so that can safely append
        store->iSingletons.ReserveL( count + 1 );
        
        // Create a new object via provided factory function
        obj = aFactoryFuncL();
        
        // Give ownership of the new object to store
        TSingleton singleton;
        singleton.iObject = obj;
        singleton.iReferenceCount = 1;
        store->iSingletons.Append(singleton);

        GLX_LOG_INFO1 ("CGlxSingletonStore::InstanceL, Created new object %x", obj);
        }

    // Remove singleton store from cleanup stack
    CleanupStack::Pop(store);

	return obj;
	}    

