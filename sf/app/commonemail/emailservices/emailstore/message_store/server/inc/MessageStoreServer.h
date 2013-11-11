/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store server.
*
*/



#ifndef __MESSAGE_STORE_SERVER_H__
#define __MESSAGE_STORE_SERVER_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
//<cmail>
#include "DebugLogMacros.h"
#include "ImsPointsecObserver.h"
#include "ImsPointsecMonitor.h"
#include "StoreDriveMonitor.h"
//</cmail>

#include "BackupRestoreObserver.h"
#include "ContainerStore.h"
#include "MessageStoreClientServer.h"
#include "psintpropertyobserver.h" // Support for on-the-fly upgrade

// ====================
// FORWARD DECLARATIONS
// ====================

class CMessageStoreSession;
class CShutdownServer;
class CImsPointsecMonitor;

// =============================
// PLATFORM SECURITY DEFINITIONS
// =============================

// Total number of ranges
const TUint KMessageStoreRangeCount = 5;

// Definition of the ranges of IPC numbers
const TInt KMessageStoreRanges[KMessageStoreRangeCount] = 
        {
        0,
        KMsgStoreCmdAlwaysPass,
        KMsgStoreCmdFirstReadUserData,
        KMsgStoreCmdFirstWriteUserData,
        KMsgStoreCmdLast+1 
        }; 

// Policy to implement for each of the above ranges        
const TUint8 KMessageStoreElementsIndex[KMessageStoreRangeCount] = 
        {
#ifdef _DEBUG
        CPolicyServer::EAlwaysPass,   // applies to 0th range
#else
        CPolicyServer::ENotSupported, // applies to 0th range
#endif
        CPolicyServer::EAlwaysPass,   // KMessageStoreElements index that applies to 1st range
        0,                            // KMessageStoreElements index that applies to 2nd range
        1,                            // KMessageStoreElements index that applies to 3rd range
        CPolicyServer::ENotSupported  // applies to 3nd range (out of range IPC)
        };

const CPolicyServer::TPolicyElement KMessageStoreElements[] = 
	{
    	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},
       	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient}
	};

// Package all the above together into a policy
const CPolicyServer::TPolicy KMessageStorePolicy =
        {
        CPolicyServer::EAlwaysPass,  // specifies all connect attempts should pass
        KMessageStoreRangeCount,     // number of ranges                                   
        KMessageStoreRanges,         // ranges array
        KMessageStoreElementsIndex,  // elements<->ranges index
        KMessageStoreElements
        };

// =====
// TYPES
// =====

enum TMessageStorePanic
  {
  EPanicGeneral,
  EPanicIllegalFunction
  };

void PanicClient(const RMessagePtr2& aMessage,TMessageStorePanic aPanic);

// =======
// CLASSES
// =======

// ==============================================================================================
// CLASS: CMessageStoreServer
// ==============================================================================================
class CMessageStoreServer : private CPolicyServer, 
							private MCustomBehaviorProvider, 
							private MBackupRestoreObserver,
                            private MImsPointsecObserver,
                            public  MPSPropertyWatcherObserver
    {
    public:

        // ==============
        // PUBLIC METHODS
        // ==============

        static CMessageStoreServer* NewLC();

        void AddSessionL( CMessageStoreSession* aSession );

        void DropSession( CMessageStoreSession* aSession );

        // will leave with KErrInUse if a backup or restore is in progress
        CContainerStore& MessageStoreL();

        void WipeEverythingL();

        #ifdef _DEBUG
        
            void InjectBackupRestoreEventL( TUint aEvent );
            void InjectPointSecEvent( TBool aLock );
            void Shutdown();
            
        #endif
            
        // FROM: MPSPropertyWatcherObserver
        void OnPSValueChangedToRequired();
                
    private:

        // ===============
        // PRIVATE METHODS
        // ===============

        CMessageStoreServer();    
        void ConstructL();

        ~CMessageStoreServer();

        void CreateContainerStoreL();
        void CreatePredefinedFoldersIfNeededL();

        // inherited from CPolicyServer    
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
        //virtual CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing );

        // inherited from MQuickPropertiesFinder
        virtual void QuickPropertiesAndCountsL( TContainerId  aType,
        							 		    const TDesC8& aProperties,
        									    RBuf8&        aQuickProperties,
        										TDes8&        aCounts ) const;
        
        virtual void SortableFieldsL( const TDesC8& aQuickProperties, const TDesC8& aProperties, RMsgStoreSortableFields& aSortableFields ) const;

        virtual void InitializeCounts( TDes8& aCounts ) const;
        virtual void LengthsL( TUint& aCountsLength,
                               TUint& aQuickPropertiesMaxLength ) const;
        virtual void IncrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const;
        virtual void DecrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const;	
        virtual void LogCounts( const TDesC& aDescription, const TDesC8& aCounts ) const;

        // inherited from MBackupRestoreObserver							  			
        virtual void BackupOrRestoreInProgress( TBool aIsARestore );
        virtual void BackupOrRestoreCompleted();	
        
        // inherited from MImsPointsecObserver
        virtual void PointSecLockStarted();
        virtual void PointSecLockEnded();
        
        
        void LockSystem();
        void UnlockSystemL();
        
    
        void SendSystemLockMessage( TInt aEvent );
        void TPtrC8ToRBuf16L( const TPtrC8& aPtr8, RBuf& aBuf ) const;
                
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================

        RPointerArray<CMessageStoreSession> iSessions;
        CShutdownServer*                    iShutdown;
        CContainerStore*                    iMessageStore;
        CBackupRestoreNotifier*             iBackupRestoreNotifier;
        CImsPointsecMonitor*                iPointsecMonitor;
        TBool                               iWipeAfterBackupRestore;
        


        TBool                               iLockedByBackupRestore;
        TBool                               iLockedByPointSec;
        
        CPSIntPropertyWatcher*              iUpgradePropertyWatcher;
        
        __LOG_DECLARATION

    }; // end class CMessageStoreServer

#endif //__MESSAGE_STORE_SERVER_H__
