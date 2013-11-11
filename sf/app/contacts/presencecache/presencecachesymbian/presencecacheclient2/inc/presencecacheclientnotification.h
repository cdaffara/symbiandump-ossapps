/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache reader notification handling
*
*/

#ifndef PRESENCECACHECLIENTNOTIFICATION_H
#define PRESENCECACHECLIENTNOTIFICATION_H

#include "presencecacheclient.h"

// #include "presencecachedefs2.h"


/**
 * Implementation for presence cache reader notification. This object is 
 * instantiated by CPresenceCacheClient. This class handles client-server
 * messaging for notifications.
 *
 * @since S60 v5.0
 */
 NONSHARABLE_CLASS( CPresenceCacheClientNotification ) : public CActive
    {

public:

        /**
        * Two-phased constructor.
        * @param aClient presence cache client
        */
        static CPresenceCacheClientNotification* NewL( CPresenceCacheClient& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CPresenceCacheClientNotification();
        
        /**
         * Subscribe for given presence buddy's change in presence. 
         *
         * @param aIdentity buddy id, in xsp format
         * @return KErrNone if successful, KErrArgument if observer not set
         * @leave Only in system error cases e.g. if failed to allocate memory.
         */
        TInt SubscribePresenceBuddyChangeL(const TDesC& aIdentity);   
        
        /**
         * Unsubscribe for given presence buddy's change in presence. 
         *
         * @param aIdentity buddy id, in xsp format
         * @leave Only in system error cases e.g. if failed to allocate memory.
         */
        void UnsubscribePresenceBuddyChangeL(const TDesC& aIdentity);          
        
private:

    /**
    * C++ constructor
    */
    CPresenceCacheClientNotification( CPresenceCacheClient& aClient );
    
    /**
    * Symbian second phase constructor
    */
    void ConstructL();
    
    void StartNotifySubscription();
    
    /**
     * Client callback handler Accesor 
     */
    MPresenceCacheReadHandler2* ClientNotifyHandler(); 
    
    /**
     * Get presence from Cache server to a client observer
     * @param aClient client callback observer
     * @param aSize notification size
     */
    void PreseneceFromServerToClientL( MPresenceCacheReadHandler2& aClient, TInt aSize );
       
                                       
public:  //From CActive

    void RunL();

    void DoCancel();
    
    TInt RunError( TInt aError );
    
private: // Data
    
        /**
         * presence cache client handling client-server messages
         */
        CPresenceCacheClient& iClient;
                
        /**
         * Size of presence notification for client-sever messages
         */
        TPckgBuf<TInt> iSizePckg;  
        
        /**
         * Whether the first notification subscription is done
         */
        TBool iNotificationSubscribed;
        
    };

#endif // PRESENCECACHECLIENTNOTIFICATION_H



