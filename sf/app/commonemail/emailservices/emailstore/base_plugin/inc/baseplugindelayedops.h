/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Support for executing asynchronously certain plugin
* operations.
*/


#ifndef __BASEPLUGINDELAYEDOPS_H__
#define __BASEPLUGINDELAYEDOPS_H__


#include <e32base.h>
#include "CFSMailCommon.h"
#include "BasePlugin.h"
#include "DebugLogMacros.h"

//<qmail>
#include "baseplugindef.h"
//</qmail>

class CBasePlugin;
class CDelayedOp;


/**
 * The interface to the delayed operations manager implementation. Plugins
 * must talk to the manager only through this interface.
 */
class MDelayedOpsManager
    {

public:

    /**
     * 
     */
    virtual ~MDelayedOpsManager() {};
    
    /**
     * @param aOp ownership is transferred 
     */
    virtual void EnqueueOpL( CDelayedOp* aOp ) = 0;
            
    /**
     * @param aOp the caller must delete the operation
     */
    virtual void DequeueOp( const CDelayedOp& aOp ) = 0;

    /**
     * API for future extensions.
     */
    virtual TInt Extension1(
        TUint aExtensionId, TAny*& a0, TAny* a1 ) = 0;
    
    };



/**
 * The base class for the delayed operations. All of the delayed operations must
 * derive from it.
 */
class BASEPLUGIN_EXPORT CDelayedOp : public CActive
    {

public:
    
     virtual ~CDelayedOp();
    
     void SetContext(
        CBasePlugin& aPlugin,
        MDelayedOpsManager& aManager );
    
	 void StartOp();
    /**
     * Derived classes must implement their asynchronous processing in this
     * method. The GetPlugin method can only be used from within this method.
     */
    virtual TBool ExecuteOpL() = 0;

protected:
    
     CDelayedOp();
    
    /**
     * Returns the plugin instance associated with this operation. Available
     * only from within the ExecuteOpL method.
     */
     CBasePlugin& GetPlugin();
    
    
private:
    
     virtual void RunL();
     virtual void DoCancel();

private:
    MDelayedOpsManager* iManager;   //not owned
    CBasePlugin* iPlugin;           //not owned
    
public:
    TInt iRequestId;
    MFSMailRequestObserver* iOperationObserver;
    
    __LOG_DECLARATION
    };

#endif // __BASEPLUGINDELAYEDOPS_H__
