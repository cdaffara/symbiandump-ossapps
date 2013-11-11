/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is the client side RSession interface declaration 
 *                for the conversation server.
 *
*/


#ifndef __R_CS_SESSION_H__
#define __R_CS_SESSION_H__

// INCLUDE FILES

// SYSTEM INCLUDES

// USER INCLUDES
#include <ccsdefs.h>

// CONSTANTS

/**
 * CS Server Version details
 */
const TInt KCsServerMajorVersionNumber = 1;
const TInt KCsServerMinorVersionNumber = 0;
const TInt KCsServerBuildVersionNumber = 0;

// CLASS DECLARATION

/**
 * RCsSession
 * Client side RSession interface for conversation server.
 *
 * @remarks Lib: CsServerClientAPI.lib
 * @since S60 v5.0
 */
class RCsSession : public RSessionBase
    {
public:

    /**
     * RCsSession.
     * Constructor
     */
    RCsSession ();  

    /**
     * Connect.
     * Connects to the conversation server
     * @return KErrNone if connected.
     *         otherwise one of the other system-wide error codes. 
     * 
     */
    TInt Connect ();

    /**
     * Version.
     * Returns the version number of conversation server
     *
     * @return Version information
     */
    TVersion Version () const;

    /**
     * ShutdownServerL.
     * Shuts down the conversation server
     */                                
    void ShutdownServerL ();

    /**
     * GetConversationListL.
     * This function sends the request to conversation server
     * to get Recent Conversation list with display name, contact link
     * for all stored conversation entry IDs.
     * 
     * @param aResultsBuffer Pointer to the results buffer. 
     * @param aStatus Active thread request status
     */
    void GetConversationListL (
            TPtr8 aResultsBuffer,
            TRequestStatus& aStatus);

    /**
     * GetConversationUnreadListL.
     * This function sends the request to conversation server
     * to get Recent Conversation list with display name, contact link
     * for all stored unread conversation entry IDs.
     *
     * @param aResultsBuffer Pointer to the results buffer.
     */
    void GetConversationUnreadListL (TPtr8 aOverflow, TPtr8 aResultsBuffer);

    /**
     * GetConversationsL.
     * This function sends the request to conversation server
     * to get Conversation Entry list for given Conversation Entry ID.
     * 
     * @param aClientConversation The client conversation.
     * @param aResultsBuffer Pointer to the results buffer. 
     * @param aStatus Active thread request status
     */
    void GetConversationsL( 
            const TDes8&  aClientConversation,
            TPtr8 aResultsBuffer,
            TRequestStatus& aStatus);  
    
    /**
     * SendNewBufferGetConversationL
     * This function sends the request to conversation server
     * to get Conversation for the new buffer size
     * 
     * @param aResultsBuffer Pointer to the results buffer. 
     * @param aStatus Active thread request status
     */
    void SendNewBufferGetConversationL (
            TPtr8 aResultsBuffer,
            TRequestStatus& aStatus);

    /**
     * GetCachingStatusL.
     * This function sends the request to conversation server
     * to get caching status.
     * 
     * @param aResultsBuffer Pointer to the results buffer. 
     */
    void GetCachingStatusL(TPtr8 aResultsBuffer);

    /**
     * GetTotalUnreadCountL.
     * Get total unread conversation entries.
     *
     * @param aResultsBuffer Pointer to the results buffer.
     */
    void GetTotalUnreadCountL(TPtr8 aResultsBuffer);
    
    /**
     * DeleteConversationL.
     * This function sends the request to conversation server
     * to delete a conversation. 
     * 
     * @param aConversationId The conversation id.
     */
    void DeleteConversationL(TInt aConversationId);  
    
    /**
      * SetConversationListChangeObserverL.
      * This function sends the request to conversation server
      * to set conversation list change observer flag.
      */
     void SetConversationListChangeObserverL();  
     
     /**
      * ResetConversationListChangeObserverL.
      * This function sends the request to conversation server
      * to reset conversation list change observer flag. 
      */
     void ResetConversationListChangeObserverL();  
     
     /**
      * SetConversationChangeObserverL.
      * This function sends the request to conversation server
      * to set conversation change observer flag for given
      * client conversation
      * 
      * @param aClientConversation Cliet Conversation object
      */
     void SetConversationChangeObserverL(
             const TDes8&  aClientConversation);  

     /**
      * SetConversationChangeObserverL.
      * This function sends the request to conversation server
      * to reset conversation change observer flag for given
      * client conversation 
      * 
      * @param aClientConversation Cliet Conversation object
      */
     void ResetConversationChangeObserverL(
             const TDes8&  aClientConversation);  
     
     /**
      * SetCachingStatusObserverL.
      * This function sends the request to conversation server
      * to set caching status observer flag.
      */
     void SetCachingStatusObserverL();  

     /**
      * ResetCachingStatusObserverL.
      * This function sends the request to conversation server
      * to reset caching status observer flag. 
      */
     void ResetCachingStatusObserverL();  
     
    /**
     * RequestChangeEventL
     * This function sends the request to conversation server
     * to request for any cache change events
     * 
     * @param aLastReqID request Count
  	 * @param aNextReqIDBuffer request buffer pointer
     * @param aResultsBuffer Pointer to the results buffer. 
     * @param aStatus Active thread request status
     */
    void RequestChangeEventL(TInt aLastReqID, 
            TPtr8 aNextReqIDBuffer,
            TPtr8 aResultsBuffer,
            TRequestStatus& aStatus);
    
    /**
     * RemoveChangeEventL
     * This function sends the request to conversation server
     * to remove itself for any cache change events
     */
    void RemoveChangeEventL ();
       
    /**
     * GetConversationId
     * @param aContactId Contact Id.
     * @return aResultsBuffer Pointer to the results buffer.
     */
    void GetConversationIdL(TInt aContactId, TPtr8 aResultsBuffer);    
    
    /**
     * GetConversationIdFromAddressL
     * @param aContactAddress Contact Address.
     * @return aResultsBuffer Pointer to the results buffer.
     */
    void GetConversationIdFromAddressL(
            TDesC& aContactAddress, 
            TPtr8 aResultsBuffer);
    
    /**
     * GetConversation
     * @param aMessageId message Id.
     * @return aResultsBuffer Pointer to the results buffer.
     */
    void GetConversationFromMessageIdL(TInt aMessageId, TPtr8 aResultsBuffer);
    
    void GetConversationFromConversationIdL(TInt aConversationId, TPtr8 aResultsBuffer);

    /**
     * MarkConversationReadL.
     * This function sends the request to conversation server
     * to mark a conversation as read.
     * 
     * @param aConversationId The conversation id.
     */
    void MarkConversationReadL(TInt aConversationId);  
    
private:

    /**
     * iListResultsBufferPtr.
     * Pointer to the buffer holding response data.
     */
    TPtr8  iListResultsBufferPtr;
	
    /**
     * iConvResultsBufferPtr.
     * Pointer to the buffer holding response data.
     */
    TPtr8  iConvResultsBufferPtr;
    
	/**
     * iRequestBufferPtr.
     * Pointer to the buffer holding request data.
     */
    TPtrC8   iRequestBufferPtr;

    /**
     * iNotifyResultsBufferPtr.
     * Pointer to the notification buffer holding response data.
     */
    TPtr8   iNotifyResultsBufferPtr;
    
    /**
     * iNotifyRequestBufferPtr.
     * Pointer to the notification buffer holding request data.
     */
    TPtr8   iNotifyRequestBufferPtr;
    };

#endif // __R_CS_SESSION_H__

// End of file
