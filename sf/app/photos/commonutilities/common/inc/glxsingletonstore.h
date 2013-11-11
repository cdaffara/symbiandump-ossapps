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



#ifndef __C_GLXSINGLETONSTORE_H__
#define __C_GLXSINGLETONSTORE_H__

#include <e32base.h>

/**
 *  CGlxSingletonStore
 *
 *  Store for singleton objects
 * 
 *  Usage:
 *
 *  Singleton class must be derived from CBase (either
 *  directly or via base class). Otherwise, singleton's destructor
 *  is not called correctly.
 *
 *  class CMyClass : public CBase ...
 *      {
 *  public:
 *      static CMyClass* InstanceL() 
 *          {
 *          // Pass in the factory function
 *          return CGlxSingletonStore::InstanceL(&NewL);
 *          }
 *
 *      void Close() 
 *          {
 *          CGlxSingletonStore::Close(this);
 *          }
 *
 *  private:
 *      static CMyClass* NewL() 
 *          {
 *          ...
 *          }
 *      };
 *
 */
NONSHARABLE_CLASS(CGlxSingletonStore) : public CBase
    {
public:
    /** 
     * Return a singleton instance and add to reference count
     */ 
    template <class T>
    static T* InstanceL(T* (*aFactoryFuncL)());

    /** 
     * Decrement reference count of singleton instance 
     * and delete if last reference removed
     */ 
    IMPORT_C static void Close(CBase* aInstance);
    
    /** Destructor */ 
    IMPORT_C ~CGlxSingletonStore();

private:
    /** Constructor */ 
    CGlxSingletonStore();
    
    /** 
     * Two-phase constructor
     * Creates an instance if one does not already exist
     * Exported for accessing from inline function
     */
    IMPORT_C static CGlxSingletonStore* InstanceLC();

    /** Cleanup function for Cleanup Stack */
    static void Cleanup(TAny* aSingletonStore);        
    
private:
    /** Struct to store a singleton an its ref count */
	class TSingleton
	    {
	public:
	    CBase* iObject;
	    TInt iReferenceCount;	    
	    };
	
	/** Array of singletons */
	RArray<TSingleton> iSingletons;
    };
    
#include "glxsingletonstore.inl"

#endif // __C_GLXSINGLETONSTORE_H__
