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
* Description:   Store for singletons
*
*/



// INCLUDE FILES
#include "glxsingletonstore.h"

#include <glxlog.h>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxSingletonStore::CGlxSingletonStore()
	{
    GLX_LOG_INFO("CGlxSingletonStore::CGlxSingletonStore");
	}

// -----------------------------------------------------------------------------
// return new instance
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxSingletonStore* CGlxSingletonStore::InstanceLC()
	{
    CGlxSingletonStore* store = reinterpret_cast<CGlxSingletonStore*>(Dll::Tls());
    
    // Create new object, if one has not been created previously
    if ( !store )
        {
        store = new (ELeave) CGlxSingletonStore;
        
        // Store in tls pointer
        Dll::SetTls( reinterpret_cast<TAny*>(store));
        }
    
    // Push to cleanup stack - will make sure tls pointer is cleared if 
    // something leaves
    CleanupStack::PushL( TCleanupItem(&Cleanup, store) );
    
    return store;
	}

// -----------------------------------------------------------------------------
// Cleanup function
// -----------------------------------------------------------------------------
//
void CGlxSingletonStore::Cleanup( TAny* /*aStore*/ )
    {   
    // In case of one singleton owning another there can be nested calls to
    // CGlxSingletonStore::InstanceL, and so multiple Cleanup items on the
    // stack to delete the store.
    // Get the store pointer from TLS to ensure it hasn't already been deleted
    CGlxSingletonStore* obj
                    = reinterpret_cast<CGlxSingletonStore*>( Dll::Tls() );
    
    // Delete the object if instance was just created, and singleton
    // was _not_ successfully added
    if ( obj && obj->iSingletons.Count() == 0 ) 
        {
        delete obj;
        }   

    GLX_LOG_WARNING("Singleton factory function has (probably) left");
    }
 		
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxSingletonStore::~CGlxSingletonStore()
	{
    GLX_LOG_INFO("CGlxSingletonStore::~CGlxSingletonStore");
    
    // Delete remaining objects, if any
    TInt count = iSingletons.Count();
    for (TInt i = 0; i < count; i++) 
        {
        delete iSingletons[i].iObject;
        }
        
    // Close the singletons array
    iSingletons.Close();
    
    // Clear tls pointer
    Dll::SetTls( NULL );
	}
	
// -----------------------------------------------------------------------------
// Decrement reference count, and potentially delete object(s)
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxSingletonStore::Close(CBase* aObj)
    {
    CGlxSingletonStore* store = reinterpret_cast<CGlxSingletonStore*>(Dll::Tls());
    if ( store ) 
        {
        // Find an object by type
        TInt count = store->iSingletons.Count();
        TInt i = 0;
        while ( i < count ) 
            {
            TSingleton& singleton = store->iSingletons[i];
            if ( aObj == singleton.iObject )
                {
                // Found an existing object of type T
                // Remove reference
                singleton.iReferenceCount--;
                GLX_LOG_INFO2("CGlxSingletonStore::Close, Ref count %d, %x", singleton.iReferenceCount, singleton.iObject);
                
                // Delete object if no more references
                if ( !singleton.iReferenceCount )
                    {
                    store->iSingletons.Remove(i);

                    // Delete store object if last singleton
                    if ( store->iSingletons.Count() == 0 )
                    	{
                    	// CGlxSingletonStore destructor will clean up DLL's TLS pointer
                	    delete store;
                    	}

                    delete aObj;
                    }
                break;
                }
            i++;
            }
        }    
    else 
        {
        GLX_LOG_ERROR("CGlxSingletonStore::Close - Too many calls to Close()");
        }
    }
