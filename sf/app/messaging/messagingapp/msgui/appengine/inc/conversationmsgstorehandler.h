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
* Description:  Wrapper classes which encapsulates Message store functions
 *
*/

#ifndef CONVERSATION_MSG_STORE_HANDLER_H
#define CONVERSATION_MSG_STORE_HANDLER_H

// INCLUDE FILES
#include <msvapi.h>
#include <ccsdefs.h>
#include <QString>

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;
class DraftsModel;
class CClientMtmRegistry;
class CMmsClientMtm;
class CMmsNotificationClientMtm;


class ConversationMsgStoreHandler: public CBase, public MMsvSessionObserver
    {
public:
    
    /*
     * Draft processing states
     */
    enum TDraftHandlerState
        {
        EReadDrafts,
        EProcessDrafts,
        EComplete
        };

    /**
     * Default constructor.
     */
    explicit ConversationMsgStoreHandler();

    /**
     * Destructor
     */
    virtual ~ConversationMsgStoreHandler();

private:
    
    /**
     * Initializes the ConversationMsgStoreHandler class.     
     */
    void InitL();

public:
    /**
     * From MMsvSessionObserver. HandleSessionEventL.
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
            TAny* aArg3);
public: 
    /**
     * Get MMS notification status.
     * @param aMsvId MMS notification status.
     */
    TCsMmsNotificationMsgState MmsNotificationStatus( TInt aMsvId );
    
    /**
     * Mark specified messages as unread.
     * @param aIdArray Array of message ids.
     */
    void MarkMessagesReadL(RArray<TInt>& aIdArray);

    /**
     * Delete specified messages from message store
     * @param aIdArray Array of message ids.
     */
    void DeleteMessages(RArray<TInt>& aIdArray);

    /**
     * Delete all draft messages.
     */
    void DeleteAllDraftMessagesL();

    /**
     * Resend a specified message that has failed.
     * @param aId TMsvId of the failed message 
	 * @return bool true if succeeded
     */
    bool ResendMessageL(TMsvId aId);
    
    /**
     * Returns the msv session object
     * @return CMsvSession object
     */
    CMsvSession& GetMsvSession();

    /**
     * Start fetching all draft messages
     */
    void FetchDraftMessages(DraftsModel* draftsModel);

    /**
     * Mark message as read and get the message type.
     * @param messageId  message Id 
     * @param msgType message type is written to this.
     * @param msgType message sub type is written to this.
     */
    void markAsReadAndGetType(int msgId,
                              int& msgType,
                              int& msgSubType);
    
    /**
     * Extract the message type.
     * @param entry  message entry.
     * @param msgType message type is written to this.
     * @param msgType message sub type is written to this.
     */
    void extractMsgType(const TMsvEntry& entry,
                        int& msgType,
                        int& msgSubType);
    /**
     * Query if the download operation be performed for
     * a message notification entry
     * @param aId, message notification id
     * @return true if download operation is possible
     */
    bool DownloadOperationSupported(const TMsvId& aId);
    
	
    /**
     * Sets content to messageId
     * @param messageId, Message Id content
     */
    void setNotificationMessageIdL(int messageId);

    /**
     * Seeker method for getting mms notification estimated msg size
     * @return formatted string for msg size in KB
     */
    QString NotificationMsgSize();
    
    /**
     * Seeker method for getting mms notification class type
     * @return Notification Class type string
     */
    QString NotificationClass();
    
    /**
     * Seeker method for message status and error string
     * mms notification's download state
     * @param [OUT] status, msg status
     * @param [OUT] statusStr, localized string of msg status
     */
    void NotificationStatus(int& status,
                     QString& statusStr);
    
    /**
     * Seeker method getting expiry info of a notification msg
     * @return formatted string of notification's expiry time
     */
    QString NotificationExpiryDate();
					 
    /**
     * Download a specified message
     * @param aId TMsvId of the message 
     * @return KErrNone if Download request is successful, 
     *         KErrNotFound if settings are not available,
     *         KErrGeneral for other cases
     */
    TInt DownloadMessageL(TMsvId aId);
    
    /**
     * Returns the Bio Type of a message
     */
    int getMsgSubType(int msgId);

private:

    /**
     * verifys the current operation status
     *@param aEntry, TMsvEntry
     */
    TBool OperationFinished( const TMsvEntry& aEntry ) const;

    /**
     * verifys the current operation status
     * @param aEntry, TMsvEntry
     */
    TBool OperationOngoing( const TMsvEntry& aEntry ) const;

    /**
     * CIdle callback
     */
    static TInt ProcessDraftMessages(TAny* arg);

    /**
     * State handler to read all draft messages 
     */
    TInt ProcessDraftMessagesL();

    /**
     * Cleanup
     */
    void CleanupL();

    /**
     * Checks if the MTM type supported
     */
    TBool IsMtmSupported(long uid);
    
    /**
     * Get max receive size limit (for MMS) from settings
     */
    TInt MaxReceiveSizeLimitL();

private: 
    /**
     * Msv Session
     * Own. 
     */
    CMsvSession* iMsvSession;

    /**
     * Draft entry
     * Own
     */
    CMsvEntry* iDraftEntry;

    /**
     * Active object
     */
    CIdle* iIdle;

    /**
     * Messages under root entry
     * Own
     */
    CMsvEntrySelection* iDraftMessages;

    /**
     * Drafts model
     * Not Own
     */
    DraftsModel *mDraftsModel;

    /**
     * Mesage count
     */
    TInt iDraftMessageCount;

    /**
     * State
     */
    TDraftHandlerState iState;
    /**
     * Own. Mtm Registry
     */
    CClientMtmRegistry* iMtmReg;

    /**
     * MMS client mtm
     */
    CMmsClientMtm* iMmsClient;

    /**
     * Notification client mtm
     */
    CMmsNotificationClientMtm* iNotificationClient;
	
    TBool iOfflineSupported;
    };

#endif // CONVERSATION_MSG_STORE_HANDLER_H

// EOF
