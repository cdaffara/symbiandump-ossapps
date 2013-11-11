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
* Description:  This is the client side server interface. This class makes 
*               request to the server and is an interface b/w UI Engine and server.
*
*/

#ifndef __C_CS_REQUEST_HANDLER_H__
#define __C_CS_REQUEST_HANDLER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <mcsplugineventobserver.h>

// USER INCLUDES
#include <rcssession.h>

// FORWARD DECLARATIONS
class CCsConversationEntry;
class CCsClientConversation;
class MCsConversationListChangeObserver;
class MCsConversationChangeObserver;
class CCsConversationChangeObserver;
class MCsResultsObserver;
class MCsCachingStatusObserver;
class CCsConversationCache;
class CCsNotificationHandler;

// CLASS DECLARATION
/**
 * CCSRequestHandler
 * This class handles all request and response to server and 
 * act as a interface b/w UI engine and Conversation server
 *
 * @remarks Lib: CsServerClientAPI.lib
 * @since S60 v5.0
 */
class CCSRequestHandler : public CActive
    {
    
public: // Constructors and destructors

    /**
     * NewL.
     * Two-phased constructor.
     * Creates a CCSRequestHandler object using two phase construction,
     * and return a pointer to the created object
     *
     * @return A pointer to the created instance of CCSRequestHandler.
     */
    IMPORT_C static CCSRequestHandler* NewL ();

    /**
     * NewLC.
     * Two-phased constructor.
     * Creates a CCSRequestHandler object using two phase construction,
     * and return a pointer to the created object.
     *  
     * @return A pointer to the created instance of CCSRequestHandler.
     */
    IMPORT_C static CCSRequestHandler* NewLC ();

    /**
     * ~CCSRequestHandler.
     * Destructor.
     * Destroys the object and release all memory objects.
     */
    virtual ~CCSRequestHandler ();

public: // New functions

    /**
     * Version.
     * Returns the conversation server version.
     * Synchronous.
     * 
     * @return The current version of conversation server.
     */
    IMPORT_C TVersion Version () const;     

    /**
     * RequestResultsEventL.
     * Adds observer to handle async results from server
     * Synchronous.
     * 
     * @param aObserver The object to be used to handle updates 
     * from the server.
     */
    IMPORT_C void RequestResultsEventL ( MCsResultsObserver* aObserver );

    /**
     * RemoveResultsEventL.
     * Removes observer that handle async results from server
     * 
     * @param aObserver The object used to handle updates from the server.
     */
    IMPORT_C void RemoveResultsEventL ( MCsResultsObserver* aObserver );

    /**
     * RequestConversationListChangeEventL
     * Adds observer to handle real time conversation list 
     * change events from server
     * Synchronous.
     * 
     * @param aObserver The object The object to be notified for updates 
     * from the server.
     */
    IMPORT_C void RequestConversationListChangeEventL ( 
            MCsConversationListChangeObserver* aObserver );

    /**
     * RemoveConversationListChangeEventL
     * Removes observer that handles real time conversation list 
     * change events from server
     * Synchronous.
     * 
     * @param aObserver The object used to handle updates from the server.
     */
    IMPORT_C void RemoveConversationListChangeEventL ( 
            MCsConversationListChangeObserver* aObserver );

    /**
     * RequestConversationChangeEventL
     * Adds observer to handle real time updates for conversation 
     * change events from server. As of now its for Meco second view
     * Synchronous.
     * 
     * @param aObserver The object to be notified for updates 
     *                  from the server.
     * @param aClientConversation Client conversation object
     */
    IMPORT_C void RequestConversationChangeEventL ( 
            MCsConversationChangeObserver* aObserver ,
            CCsClientConversation* aClientConversation);

    /**
     * RemoveConversationChangeEventL
     * Removes observer that handle async conversation 
     * change events from server
     * Synchronous.
     * 
     * @param aObserver The object used to handle updates from the server.
     * @param aClientConversation Client conversation object
     */
    IMPORT_C void RemoveConversationChangeEventL ( 
            MCsConversationChangeObserver* aObserver ,
            CCsClientConversation* aClientConversation);

    /**
     * RequestCachingStatusEventL
     * Adds observer to handle async caching status events from server
     * Synchronous.
     * 
     * @param aObserver The object to be used to handle updates 
     *                  from the server.
     */
    IMPORT_C void RequestCachingStatusEventL ( 
            MCsCachingStatusObserver* aObserver );

    /**
     * RemoveCachingStatusEventL
     * Removes observer that handle async caching status events from server
     * Synchronous.
     * 
     * @param aObserver The object used to handle updates from the server.
     */
    IMPORT_C void RemoveCachingStatusEventL ( 
            MCsCachingStatusObserver* aObserver);

    /**
     * ShutdownServerL.
     * Shuts down the conversation server.
     * Synchronous.
     * 
     * CAUTION: Shutdown of conversation server 
     * shouldn't be done for each session.
     * It should be done only when the conversation server
     * needs to be completely shutdown.
     */
    IMPORT_C void ShutdownServerL();

    /**  
     * Get Recent Conversation Entry list with display name
     * for all stored conversation entry IDs.
     * This is a Async API. 
     */
    IMPORT_C void GetConversationListL();

    /**
     * Get Recent unread Conversation Entry list with display name
     * for all stored conversation entry IDs.
     * Synchronous.
     *
     * @param aClientConversationList Pointer to list of CCsClientConversation
     * having only unread conversation entries.
     * @attention Caller is owner of aClientConversationList. ResetAndDestroy(),
     * Close() & delete needs to be called to avoid memory leaks.
     */
    IMPORT_C void GetConversationUnreadListL(RPointerArray<
            CCsClientConversation>* aClientConversationList);

    /**
     * Get Conversation Entry list for given Conversation Entry ID.
     * This is a Async API. 
     * 
     * @param aClientConversation - class encapsulating Conversation Entry ID 
     * @param aKnownIndex - known index that is fetched
     * @aPageSize - page size that has to be fetched.
     */
    IMPORT_C void GetConversationsL(
            CCsClientConversation*  aClientConversation,
            TInt aKnownIndex, 
            TInt aPageSize);

    /**
     *  Delete a conversation.
     *  Synchronous.
     *  
     *  @param aConversationId The conversation id.
     */
    IMPORT_C void DeleteConversationL(TInt aConversationId);            
    
    /**
     * GetCachingStatusL
     * Get caching status
     * Synchronous. 
     * 
     * @return  Caching Status 
     * KCacheStatusUnknwon / KCacheStatusStarted /
     * KCacheStatusCompleted / KCacheStatusError
     */
    IMPORT_C TUint8 GetCachingStatusL();

    /**
     * GetTotalUnreadCountL
     * Get total unread conversation entries.
     * Synchronous.
     *
     * @return  Total unread conversation entries.
     */
    IMPORT_C TUint32 GetTotalUnreadCountL();
     
    /**
     * Get the conversation identifier for a contact.
     * @param Contact Id.
     * @return Conversation Id.
     */
    IMPORT_C TInt GetConversationIdL ( TInt aContactId );
	
	/**
     * Get the conversation identifier for a contact.
     * @param Contact address.
     * @return Conversation Id.
     */
    IMPORT_C TInt GetConversationIdFromAddressL ( TDesC& aContactAddress );
    
    /**
     * MarkConversationReadL
     * Mark conversation as read.
     * This is Sync API.
     * Read status is set in the background.
     * 
     * @param aConversationId Conversation Id.
     */
    IMPORT_C void MarkConversationReadL ( TInt aConversationId );
    
    /**
     * GetMessagingHistoryL.
     * Get the messaging history for a contact.
     * This is Async API.
     * Data is returned via MCsResultsObserver.
     * 
     * @param aContactId Contact Id.
     * @param aKnownIndex known index fetched
     * @param aPageSize page size to be fetched
     */
    IMPORT_C void GetMessagingHistoryL(TInt aContactId,
        TInt aKnownIndex, 
        TInt aPageSize);

    /**
     * ClearMessagingHistoryL
     * Clear the messaging history for a contact.
     * This is Sync API.
     * Delete is done in the background.
     * 
     * @param aContactId Contact Id.
     */
    IMPORT_C void ClearMessagingHistoryL ( TInt aContactId );
    
    /**
     * MarkMessagingHistoryReadL
     * Set the messaging history as read.
     * This is Sync API.
     * Read status is set in the background.
     * 
     * @param aContactId Contact Id.
     */
    IMPORT_C void MarkMessagingHistoryReadL ( TInt aContactId );
	
	  /**
     * Get the conversation for a message.
     * @param Message Id.
     * @return CCsClientConversation
     */
    IMPORT_C CCsClientConversation* GetConversationFromMessageIdL ( TInt aMessageId );
    
    IMPORT_C CCsClientConversation* GetConversationFromConversationIdL( TInt aConversationId );

protected: // Functions from base classes

    /**
     * RunL
     * From CActive, RunL.
     * Callback function.
     * Invoked to handle responses from the server.
     */
    void RunL();

    /**
     * DoCancel
     * From CActive, DoCancel.
     * Cancels any outstanding operation.
     */
    void DoCancel ();

private: // Constructors and destructors

    /**
     * CCSRequestHandler.
     * Performs the first phase of two phase construction.
     */
    CCSRequestHandler ();

    /**
     * ConstructL.
     * Performs the second phase construction of a
     * CCSRequestHandler object.
     */
    void ConstructL ();

    /**  
     * HandleGetConversationListResults
     * This is for handling GetEntryList results asynchronusly
     * from the server.
     */
    void HandleGetConversationListResults();

    /**  
     * HandleGetConversationResults
     * This is for handling GetConversation results  
     * async callback from server.
     */
    void HandleGetConversationResults();

    /**  
     * HandleGetConversationListOverflow.
     * This is for handling GetEntryList results buffer 
     * overflow error.
     */
    void HandleGetConversationListOverflow();

    /**  
     * HandleGetConversationOverflow.
     * This is for handling GetConversation results buffer 
     * overflow error.
     */
    void HandleGetConversationOverflow();

    /**
     * HandleErrorL.
     * Handles error events from server.
     * 
     * @param aErrorCode Error code .
     */
    void HandleErrorL ( TInt aErrorCode );   

    /**
     * HandleAddConversationList.
     * Handles new conversation list event.
     */
    void HandleAddConversationList (HBufC8* aResultsBuffer); 

    /**
     * HandleDeleteConversationList.
     * Handles delete conversation list event.
     */
    void HandleDeleteConversationList (HBufC8* aResultsBuffer); 
    
	/**
	 * HandlePartialDeleteConversationList
	 * Handles partial delete of conversation event.
	 */
    void HandlePartialDeleteConversationList(HBufC8* aResultsBuffer);

    /**
     * HandleModifyConversationList.
     * Handles modify conversation list event.
     */
    void HandleModifyConversationList (HBufC8* aResultsBuffer); 

    /**
     * HandleAddConversation.
     * Handles add conversation event.
     */
    void HandleAddConversation (HBufC8* aResultsBuffer); 

    /**
     * HandleDeleteConversationEvent.
     * Handles delete conversation event.
     */
    void HandleDeleteConversation (HBufC8* aResultsBuffer); 

    /**
     * HandleModifyConversationEvent.
     * Handles modify conversation event.
     */
    void HandleModifyConversation (HBufC8* aResultsBuffer); 

    /**
     * HandleCachingStarted.
     * Handles caching started event.
     */
    void HandleCachingStarted (HBufC8* aResultsBuffer); 

    /**
     * HandleCachingCompleted.
     * Handles caching Completed event.
     */
    void HandleCachingCompleted (HBufC8* aResultsBuffer);

    /**
     * HandleCachingError
     * Handles caching error event.
     */
    void HandleCachingError (HBufC8* aResultsBuffer);
    
    /**
     * HandleRefreshConversationList
     * Handles refresh list event.
     */
    void HandleRefreshConversationList (HBufC8* aResultsBuffer);
    
    /**
     * HandleRefreshConversation
     * Handles refresh event.
     */
    void HandleRefreshConversation (HBufC8* aResultsBuffer);

private: // Data

    friend class CCsNotificationHandler;
    
    /**
     * iSession
     * The conversation server csession.
     * Own.  
     */
    RCsSession iSession;

    /**
     * iResultsObserver
     * Observer which handles conversation event from server.
     * Own.
     */
    MCsResultsObserver* iResultsObserver;

    /**
     * iConversationListChangeObserver
     * Observer which handle conversation list changes.
     * Own.
     */
    MCsConversationListChangeObserver* iConversationListChangeObserver;

    /**
     * iConversationChangeObserver
     * Observer which handles conversation changes.
     * Own.
     */
    CCsConversationChangeObserver* iConversationChangeObserver;

    /**
     * iCachingChangeObserver
     * Observers which handle conversation change events from server.
     * Own.
     */
    MCsCachingStatusObserver* iCachingStatusObserver;

    /**
     * iBuffer
     * The buffer that holds the conversation data like contact 
     * or conversation entry id. Required for IPC with server.
     * Own.
     */
    HBufC8* iBuffer;

    /**
     * iResultsBuffer.
     * The buffer hold the response data for IPC communication.
	 * Use this buffer sync calls to server.
     * Own.
     */
    HBufC8* iResultsBuffer;
    
    /**
     * iListResultsBuffer.
     * The buffer is used for async call to get conversation list 
	 * from server.
     * Own.
     */	 
    HBufC8* iListResultsBuffer;
	
    /**
     * iConvResultsBuffer.
 	 * The buffer is used for async call to get conversations 
	 * from server.
     * Own.
     */
    HBufC8* iConvResultsBuffer;
	
    /**
     * iNotificationHandler
     * Own.
     */
    CCsNotificationHandler* iNotificationHandler;
    
    };
#endif //__C_CS_REQUEST_HANDLER_H__

// End of File
