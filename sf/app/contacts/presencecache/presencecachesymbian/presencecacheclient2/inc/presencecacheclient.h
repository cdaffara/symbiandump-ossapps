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
* Description:  Implementation for presence cache reader and writer.
*
*/

#ifndef PRESENCECACHECLIENT_H
#define PRESENCECACHECLIENT_H

#include <presencecachereader2.h>
#include <presencecachewriter2.h>

#include "presencecachedefs2.h"

class MPresenceBuddyInfo2;
class MPresenceCacheWriteHandler2;
class MPresenceCacheReadHandler2;
class CPresenceCacheClientNotification;

//Include Cache server namespace
using namespace NCacheSrv2;

/**
 * Implementation for presence cache reader and writer. This object is 
 * instantiated either by MPresenceCacheReader2 or MPresenceCacheWriter2 using ecom framework.
 * Apart from implementing these two objects, this is also a Symbian OS client and
 * it communicates with presence cache server. Its also an active object.
 *
 * @since S60 v3.2
 */
class CPresenceCacheClient : public CActive,
							 public RSessionBase,
							 public MPresenceCacheReader2,
                             public MPresenceCacheWriter2

    {
public:

    /**
     * Factory method to instantiate CPresenceCacheClient through the ECom. 
     *
     * @return The new CPresenceCacheClient object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static CPresenceCacheClient* NewL();
    
    /**
     * Factory method to instantiate CPresenceCacheClient through the ECom.
     * It's returns MPresenceCacheReader2
     *
     * @return The new CPresenceCacheClient object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceCacheReader2* CreateReaderL();
    
    /**
     * Factory method to instantiate CPresenceCacheClient through the ECom.
     * It's returns MPresenceCacheWriter2
     *
     * @return The new CPresenceCacheClient object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceCacheWriter2* CreateWriterL();
    
  
    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */
    virtual ~CPresenceCacheClient();
    
    /**
    * Start the server
    * @return TInt Error code
    */
    TInt StartServer();
    
    /**
    * Version.
    * Gets the version number.
    * @return The version.
    */
    TVersion Version() const;
        
        
private:

    /**
    * C++ constructor
    */
    CPresenceCacheClient();
    
    /**
    * Symbian second phase constructor
    */
    void ConstructL();
    
    /**
    * Connect.
    * Connects to the server and create a session.
    * @return Error code.
    */
    TInt Connect();
    
    /**
    * Packs MPresenceBuddyInfo2 object into a buffer
    * 
    * @param aBuddyPresInfo, MPresenceBuddyInfo2
    * @return HBufC8* buffer
    */
    HBufC8* PackBuddyPresenceInfoLC(const MPresenceBuddyInfo2* aBuddyPresInfo);
    
    /**
    * Packs MPresenceBuddyInfo2List object into a buffer
    * 
    * @param aList, MPresenceBuddyInfo2List
    * @return HBufC8* buffer
    */
    HBufC8* PackPresenceBuddyListLC(const RPointerArray<MPresenceBuddyInfo2>& aList);  
      
    
    /**
    * Handle asynchronous call from server that buddies presence have been prepared.
    * This function does further processing and calls the client.
    * 
    * @param aError, Error came from server.
    */
    void HandlePreparedAllBuddiesPresenceL(TInt aError);
    
    /**
    * Return true if given identity is valid xsp identity
    * 
    * @param ETrue if valid xsp identity
    */
    TBool ValidateXspId(const TDesC& aXspId);
    
       
      
public: //From MPresenceCacheReader2

    MPresenceBuddyInfo2* PresenceInfoLC(const TDesC& aIdentity);    
    
    TInt BuddyCountInAllServices();
    
    TInt BuddyCountInService(const TDesC& aServiceName);
    
    TInt ServicesCount();
    
    TInt CancelRead();                                        
                                                
    TInt SubscribePresenceBuddyChangeL(const TDesC& aIdentity);
                                                
    void UnSubscribePresenceBuddyChangeL(const TDesC& aIdentity);                             
                                                

public: //From MPresenceCacheReader2, Asynchronous methods

    TInt AllBuddiesPresenceInService(const TDesC& aServiceName, 
                                        MPresenceCacheReadHandler2* aHandler);

    TInt SetObserverForSubscribedNotifications(
                        MPresenceCacheReadHandler2* aHandler);                             

public: //From MPresenceCacheWriter2

    TInt WritePresenceL(const MPresenceBuddyInfo2* aPresenceBuddyInfo);
    
    TInt DeleteService(const TDesC& aServiceName);
    
    TInt DeletePresenceL(const TDesC& aIdentity);
        
    MPresenceBuddyInfo2* NewBuddyPresenceInfoLC();
    
    // MPresenceBuddyInfoList* NewPresenceBuddyInfoListLC(const TDesC& aServiceName);
    
    TInt CancelWrite();
    

public: //From MPresenceCacheWriter2, Asynchronous methods

    TInt WriteMultiplePresenceL(const RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfos,
                        MPresenceCacheWriteHandler2* aHandler);
                                    
public:  //From CActive

    void RunL();

    void DoCancel();
    
private: // Data
    
    //Not Owned
    MPresenceCacheWriteHandler2* iWriteHandler;
    
    //Not Owned
    MPresenceCacheReadHandler2* iReadHandler;
    
    //To store async request status
    NRequest::TOpAsyncCodes iAsyncReq;
          
    /**
     * Client who subscribed cache notifications
     * NOT OWN
     */
    MPresenceCacheReadHandler2* iNotifyClient;   
    
    /**
     * Handler for subscribed cache notifications
     * OWN
     */    
    CPresenceCacheClientNotification* iNotifyHandler;
    
    /**
     * Asynchronous data packet
     * OWN
     */
    HBufC8* iPresInfoPack;    
    
    /**
     * Asynchronous data packet
     * OWN
     */
    HBufC16* iPresInfoPack16;      
    
private: // friend classes

	friend class CPresenceCacheClientNotification;

    };

#endif // PRESENCECACHECLIENT_H



