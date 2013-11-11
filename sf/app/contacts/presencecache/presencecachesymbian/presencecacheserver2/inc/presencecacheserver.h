/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for Presence Cache Server
*
*/


#ifndef PRESENCECACHESERVER_H
#define PRESENCECACHESERVER_H

// INCLUDE FILES
#include <e32base.h>
#include "presencecacheexpiry.h"

class CPresenceCacheServiceStore;
class CPresenceCacheSession;


/**
 * Shutdown timer class, to help us in delayed shutdown.
 * @since S60 v3.2
 */
class CShutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x200000};	// approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
    //Initiate server exit when the timer expires
	void RunL();
	};    


/**
 * Presence cache server class.
 * @since S60 v3.2
 */
class CPresenceCacheServer : public CPolicyServer
    {
    public : // Constructors and destructors

        /**
        * NewLC.
        * Two-phased constructor.
        * @return Pointer to created CPresenceCacheServer object.
        */
        static CPresenceCacheServer* NewLC();

        /**
        * ~CPresenceCacheServer.
        * Execution and Destruction.
        */
        virtual ~CPresenceCacheServer();
        static void ExecuteL();

    public: // New functions

        /**
        * IncrementSessions.
        * Increments the count of the active sessions for this server.
        */
        void IncrementSessions();

        /**
        * DecrementSessions.
        * Decrements the count of the active sessions for this server.
        * If no more sessions are in use the server terminates.
        */
        void DecrementSessions();
        
        /**
        * Finds whether given session is alive or not.
        *
        * @param aSession to look for.
        * @return ETrue if session is still alive
        */
        TBool SessionExists(CPresenceCacheSession* aSession);

        /**
         * Start expiry check mechanism
         * All buddy infos are read and expired buddy infos are removed
         * non-blocking function
         */
        inline void StartExpiryCheck();
        
    protected: // Functions from base classes

        // From CActive
        TInt RunError( TInt aError );

    private: // Constructors and destructors

        /**
        * CPresenceCacheServer.
        * C++ default constructor.
        * @param aPriority priority for this thread.
        */
        CPresenceCacheServer( TInt aPriority );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CServer, NewSessionL.
        * Creates a cache server session.
        * @param aVersion The client version.
        * @param aMessage Message from client.
        * @return Pointer to new session.
        */
        CSession2* NewSessionL( const TVersion& aVersion, 
                                const RMessage2& aMessage ) const;

    private: // Data

        /**
        * iSessionCount, the number of session owned by this server.
        */
        TInt iSessionCount;
        
        
        CShutdown iShutdown;
        
        /// Own
        CPresenceCacheExpiry* iExpiryCheck;
        
    public: // testing only    
        
        /**
        * OWN: Main Presence cache on server side. Used by all sessions. 
        */
        RPointerArray<CPresenceCacheServiceStore> iPresenceCache;
        
    };
    
inline void CPresenceCacheServer::StartExpiryCheck()
    {
    iExpiryCheck->Start();
    }


#endif // PRESENCECACHESERVER_H

// End of File

