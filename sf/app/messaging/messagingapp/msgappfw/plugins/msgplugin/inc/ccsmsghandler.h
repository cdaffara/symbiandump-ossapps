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
* Description:  Request Handler plugin
 *
*/


#ifndef _C_CS_MSG_REQUEST_HANDLER_H_
#define _C_CS_MSG_REQUEST_HANDLER_H_

//SYSTEM INCLUDES
#include <msvapi.h>

//USER INCLUDES
#include "ccsmsg.h"
#include "ccsmsgpluginutility.h"


// FORWARD DECLARATION
class CCsPlugin;
class CMsvSession;
class MCsPluginEventObserver;
class MMsvSessionObserver;
class CCsConversationEntry;
class CCsMsg;
class CCsMsgPluginUtility;
class CClientMtmRegistry;
class CSmsClientMtm;
class CMmsClientMtm;

/**
 * Operation Type
 */  
enum TEventType
    {
    ERead,
    EUpdate,
    EDelete
    };

/**
 * States
 */
enum TMsgHandlerState
    {
    EReadInbox,
    EReadSent,
    EProcessDraft,
    EReadOutbox,
    ESortEntries,
    EProcessEntries,
    EComplete
    };

/**
 *  Message plugin Request Handler Class
 *  This class interacts with MsvServer to fetch message data
 */
class CCsMsgHandler : public CBase, public MMsvSessionObserver
    {

public: 

    /**
     * Two phase construction
     * @param aMsgObserver MCsMsgObserver interface
     */
    static CCsMsgHandler* NewL(MCsMsgObserver *aMsgObserver);

    /**
     * Destructor
     */
    virtual ~CCsMsgHandler();

    /**
     *  Start
     *  This function starts the state machine to fetch msg data from msvserver
     */        
    void StartL();

public://MMsvSessionObserver
    /**
     * Handles the MsvServer updates. Implemented for MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, 
            TAny* aArg1, 
            TAny* aArg2, 
            TAny* aArg3);

private:

    /**
     * CIdle callback
     */
    static TInt UploadMsg(TAny* arg);

    /**
     * State handler to read all messages 
     */
    TInt UploadMsgL();

    /**
     * Checks if the MTM type supported
     */
    TBool IsMtmSupported(long uid);
    
    /**
     * Read all message Id in iMessageArray
     * @aFolderId - Folder Id whose message need to be sync.
     */
    void UpdateMessageArrayL(const TMsvId& aFolderId);
    
private:

    /**
     * Constructor   
     */
    CCsMsgHandler();

    /**
     * 2nd phase construtor
     * @param aMsgPlugin The main message plugin object
     */
    void ConstructL(MCsMsgObserver *aMsgPlugin);

    /**
     * ProcessResultsL fetches results and updates the CCsServer
     * with the fetched data
     */    
    void ProcessResultsL(TMsvEntry aEntry);

    /**
     * Create CCsConversationEntry and adds to the list     
     * @param aContact, Contact details
     * @param aDescription, message body
     * @param aEntry, TMsvEntry
     */
    void CreateAndAddEntryL(const TDesC& aContact,
            const TDesC& aDescription,
            const TMsvEntry& aEntry);

    /**
     * Process each entry
     * @param aEvent Read, Update, Delete
     * @param aContact (From/To) Field
     * @param aDescription, message body
     * @param aEntry, Message entry
     */
    void ProcessEntryL(TEventType aEvent, 
            const TDesC& aContact,
            const TDesC& aDescription,
            const TMsvEntry& aEntry);

    /**
     * Process each entry
     *@param aSelection, CMsvEntrySelections
     *@param aParent, parent id if TMsvEntry
     */    
    void HandleEventL(CMsvEntrySelection* aSelection, TMsvId aParent);

    /**
     * Extracts the addresses in the to field and updates them to server
     *@param aEntry, TMsvEntry
     *@param aEvent Read, Update, Delete
     *@param aFromAddress, from address of the message
     */    
    void ExtractAddressesL( 
            TMsvEntry aEntry , 
            TEventType aEvent,
            RPointerArray<HBufC>& addressList);

    /**
     * Extracts the Message type based on the MTM value 
     * @param aEntry, TMsvEntry
     */
    TCsType ExtractCsType( const TMsvEntry& aEntry);
	
private: //Data

    /**
     * MsvServer session object
     */
    CMsvSession* iSession;

    /**
     * Utility object
     */
    CCsMsgPluginUtility* iMsgPluginUtility;

    /**
     * iConverstationEntryList
     * ConversationEntry list
     * Own.
     */    
    RPointerArray<CCsConversationEntry> *iConverstationEntryList;

    /**
     * iMsgObserver
     * The Observer interface
     * Not Own.
     */        
    MCsMsgObserver *iMsgObserver;

    /**
     * Mtm registry object
     */
    CClientMtmRegistry* iMtmRegistry;

    /**
     * SMS client mtm object
     */
    CSmsClientMtm* iSmsMtm;

    /**
     * Mms client mtm object
     */
    CMmsClientMtm* iMmsMtm;

    /**
     * Previous entry, used for comparing with new entry received    
     */
    TMsvEntry iPrevEntry;

    /**
     * Master array for holding all the messages 
     */
   RArray<TMsvId>* iMessageArray;

    /**
     * State
     */
    TMsgHandlerState iState;

    /**
     * Active object
     */
    CIdle* iIdle;
    };

#endif // _C_CS_MSG_REQUEST_HANDLER_H_
