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
* Description: Pre Install helper class.
*
*/
#ifndef __EMAILSTOREPREINSTALL_H__
#define __EMAILSTOREPREINSTALL_H__

// ========
// INCLUDES
// ========
#include <e32base.h>
#include <w32std.h>
#include <apgtask.h>
#include <e32property.h>
#include <DebugLogMacros.h>
#include "ProcessTimer.h"

// ========
// LOCALS
// ========
LOCAL_C void DoPreInstallL( );

/**
 * Pre Install helper class.
 */
NONSHARABLE_CLASS ( CEmailStorePreInstall ) : public CTimer , public MObserverPreInstall
    {
    public:
        /**
         * Factory methods.
         */
        static CEmailStorePreInstall* NewL();
        static CEmailStorePreInstall* NewLC();
        
        /**
         * Destructor.
         */
        virtual ~CEmailStorePreInstall();
        
    public:
        // FROM: MObserverPreInstall
        void Timeout();
        
    protected:
        //from CTimer
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );
                
    private:
        /**
         * Constructor.
         */
        CEmailStorePreInstall( );
        
        /**
         * Second-phase constructor.
         */
        void ConstructL();

        /**
         * Stop servers.
         * Iterates through servers and signals them to stop.
         * If the stop doesn't occur in a certain time, then the server is killed.
         */
        void StopNextServerL();
        
        // Internal state
        enum TState
            {
            EInit,
            EStopServers,
            EWaitForProcess,
            EDone
            };

    private:
        /** Current index in the server list. */
        TInt    iServerIndex;        
        /** Internal state. */
        TState  iState;
        /** Process finish timeout. */
        CProcessTimer* iTimeoutTimer;
        /** Current server to be stopped handle. */
        RProcess iProcess;
        
        /** Logging. */
        __LOG_DECLARATION        
    };

#endif  //__EMAILSTOREPREINSTALL_H__
