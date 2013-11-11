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

#ifndef MSG_STORE_HANDLER_H
#define MSG_STORE_HANDLER_H

// INCLUDE FILES
#include <msvapi.h>
#include <ccsdefs.h>
#include <QString>
#include <platform/ssm/ssmstateawaresession.h>

//USER INCLUDES
#include "msginfodefs.h"

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;
class OutboxIndicatorData;
class MsgNotifierPrivate;
class CCSRequestHandler;
class CMsvEntrySelection;

// CONSTANTS

/**
 * @class MsgStoreHandler
 * handles the Messaging store for outbox events.
 */
class MsgStoreHandler: public CActive, public MMsvSessionObserver, public MMsvEntryObserver
{
public:

    /**
     * Default constructor.
     */
    explicit MsgStoreHandler(MsgNotifierPrivate* notifier, CCSRequestHandler* aHandler);

    /**
     * Destructor
     */
    virtual ~MsgStoreHandler();
    
    /**
     * Get the number of entries in the outbox that are in the failed or resend state.
     * @param indicatorData, MsgInfo Return parameter.
     */
    TInt GetOutboxEntriesL(MsgInfo& aFailedIndicatorData, 
                           MsgInfo& aPendingIndicatorData);

    /*
     * Get unread message count.
     */
    int GetUnreadMessageCountL();

public:
    /**
     * From MMsvSessionObserver. HandleSessionEventL.
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    /**
     * from MMsvEntryObserver, HandleEntryEventL
     */
    virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);

private:

    /**
     * UpdateOutboxIndications()
     * @
     */
     void UpdateOutboxIndications();

     /**
      * Initializes the MsgStoreHandler class.     
      */
     void InitL();
     
    /**
     * Process the indicator data to retrieve relevant details of the conversation
     * @param msgId, message id.
     * @param indicatorData data retrieved about the outbox entry
     * 
     */
    void ProcessIndicatorDataL(TMsvId msgId, MsgInfo& indicatorData);
    
    /**
     * Get MMS notification status.
     * @param aMsvId MMS notification status.
     */
    TCsMmsNotificationMsgState MmsNotificationStatus(TMsvEntry entry);

    /**
     * verifys the current operation status
     *@param aEntry, TMsvEntry
     */
    TBool OperationFinished(const TMsvEntry& aEntry) const;

    /**
     * verifys the current operation status
     * @param aEntry, TMsvEntry
     */
    TBool OperationOngoing(const TMsvEntry& aEntry) const;

    /**
     * Process the class0 SMS and extract all the relevant information
     * @param aHeader, Class0 SMS header
     */
    void HandleClass0SmsL(CMsvEntry* aMsgEntry, TMsvId aMsgId);

	/**
	 * From CActive
	 */
    void DoCancel();
    void RunL();

private:
    /**
     * Own. Msv Session
     */
    CMsvSession* iMsvSession;

    /**
     * Own. Msv Entry
     */
    CMsvEntry* iMsvEntry;

    /**
     * Not Owned. Message notifier.
     */
    MsgNotifierPrivate* iNotifier;

    /**
     * Not Owned. Request handler
     */
    CCSRequestHandler* iRequestHandler;
    
    /**
     * List of messages in Fialed states
     */
    CMsvEntrySelection* iFailedMessages;   
    
    /**
     * Interface to receive notifications when the
     * System State changes.
     */
    RSsmStateAwareSession iStateAwareSession;   
};

#endif // MSG_STORE_HANDLER_H
// EOF
