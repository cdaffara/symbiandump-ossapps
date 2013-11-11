/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of MPEPhoneModelInternal class.
*
*/


#ifndef M_MPEPHONEMODELINTERNAL
#define M_MPEPHONEMODELINTERNAL

#include <pevirtualengine.h>

class MPEDataStore;
class MPEExternalDataHandler;
class CPERemotePartyInfoMediator;


/**
 *  Phone Model internal interface.
 *
 *  
 *
 *  @lib phoneengine
 *  @since S60 v4.0 
 */
NONSHARABLE_CLASS( MPEPhoneModelInternal ) 
    {

public:

    /**
     * Returns pointer to information object (CPEEngineInfo) 
     *
     * @since S60 v4.0
     * @return pointer to CPEEngineInfo object as MPEDataStore
     */
     virtual MPEDataStore* DataStore() = 0;    

    /**
     * Handle internal message
     *
     * @since S60 v4.0
     * @param aMessage specifies the event that has taken place
     * @param aCallId is the call id number.
     */
     virtual void HandleInternalMessage( const TInt aMessage ) = 0;

    /**
     * Reroutes messages from the susbsystems to the message handler
     *
     * @since S60 v4.0
     * @param aMessage is the message id.
     */
     virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage ) = 0;

    /**
     * Reroutes messages from the susbsystems to the message handler
     *
     * @since S60 v4.0
     * @param aMessage is the message id.
     * @param aCallId is the call id number.
     */
     virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
            const TInt aCallId ) = 0;
     
    /**
     * Returns pointer to external data handler
     *
     * @since S60 v4.0
     * @return pointer to MPEExternalDataHandler object
     */
     virtual MPEExternalDataHandler* DataStoreExt() = 0;
     
     
     /**
      * Returns pointer to Mediator Command handler
      *
      * @since S60 v5.2
      * @return pointer to CPERemotePartyInfoMediator object
      */
     virtual CPERemotePartyInfoMediator* MediatorCommunicationHandler() = 0;
   
    };


#endif // M_MPEPHONEMODELINTERNAL
