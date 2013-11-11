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
* Description:  class handles mailbox sync state queries
*
*/


#ifndef IPSPLGSYNCSTATEHANDLER_H_
#define IPSPLGSYNCSTATEHANDLER_H_


#include "ipsplgeventhandler.h"

class CMsvSession;
class CIpsPlgSosBasePlugin;

/**
 *  
 *
 *  @lib
 *  @since FS 1.0
 */
NONSHARABLE_CLASS( CIpsPlgSyncStateHandler ) : 
    public CBase,
    public MIpsPlgPropertyObserver
    {
public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 2.0
     * @return Created object
     */
    static CIpsPlgSyncStateHandler* NewL( 
        CMsvSession& aSession, 
        CIpsPlgSosBasePlugin& aPlugin,
        RPointerArray<CIpsPlgSingleOpWatcher>& aOperationsRef );

    /**
     * Class destructor
     *
     * @since FS 2.0
     */
    virtual ~CIpsPlgSyncStateHandler();
    
    
public: // from MIpsPlgPropertyObserver
    
    virtual void HandlePropertyEventL( 
            TInt aEvent, TInt aMailbox, TInt aPluginId ,TInt aError );
    
public:    
    
    /**
     * Static utility function for saving 
     * sync time to extended mailbox settings
     * @param aSession, reference to msv session
     * @param aService, mailbox service id
     * @since FS 2.0
     */
    static void SaveSuccessfulSyncTimeL( 
            CMsvSession& aSession, TMsvId aService );

    /**
     * 
     *
     * @since FS 2.0
     */
    TSSMailSyncState GetCurrentSyncState(
        const TFSMailMsgId& aMailboxId );
    
    
    /**
    * Returns index of syncing mailbox array if found
    * @param aMailbox  id of mailbox to be found
    * @return index of syncing mailbox array or KErrNotFound if not found
    */
    TInt FindSyncingMailbox( TMsvId aMailbox );
    
    /**
    * Updates mailbox state to syncing mailboxes array, append box if not
    * found and append can be done (no errors occur)
    * @param aMailbox  id of mailbox to be append
    * @param aState state to be updated or added to mailbox
    */
    void AppendMailboxToSyncingMailboxL( TMsvId aMailbox, TInt aState );
    
    TInt GetMailboxIpsState( TMsvId aMailbox );
    
    void NotifyMailboxRemove( TMsvId aMailbox );
    
    TInt FindMailbox( TMsvId aMailbox );
    
    /**
    * Checks is there any connect operations running, meaning
    * that the mailbox is in "connected" state.
    * @param aMailBoxId Id of the mailbox which state is checked
    */
    TBool ConnOpRunning( const TFSMailMsgId& aMailBoxId  );
    
private:
    
    /**
     *
     */
    CIpsPlgSyncStateHandler( 
        CMsvSession& aSession, 
        CIpsPlgSosBasePlugin& aPlugin,
        RPointerArray<CIpsPlgSingleOpWatcher>& aOperationsRef );

    /**
     * 
     */
    void ConstructL();
    
    
    TInt SolveOpProtocolType( );
    
private:

    CMsvSession&            iSession;
    CIpsPlgSosBasePlugin&   iPlugin;
    
    // used because of efficency, stores ids of mailboxes that is 
    // syncing
    RArray<TIpsMailboxState>        iSyncingMailboxes;
    
    RPointerArray<CIpsPlgSingleOpWatcher>& iOperationsRef;
    
    };

#endif /*IPSPLGSYNCSTATEHANDLER_H_*/

// End of file

