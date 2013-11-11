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
* Description: 
*     Logic for scheduled IMAP / POP sync
*
*/


#ifndef IPSSOSAOIMAPPOPLOGIC_H
#define IPSSOSAOIMAPPOPLOGIC_H

#include <e32base.h> 
#include <msvapi.h>

class CIpsSosAOMBoxLogic;
class CIpsSosAOEMNResolver;
class CIpsSetDataApi;

_LIT( KIpsSosAOPanicLit, "AO");


/**
* class CIpsSosAOImapPopLogic;
*
* Creates and destroys mailbox spesific MBoxLogic instances according to mailbox
* Always online settings. Handles Always online server commands and sends
* TMboxLogicCommands to mailboxes.
* 
* lib ipssosaoplugin.lib
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOImapPopLogic) : public CBase
//</Qmail>
    {

public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOImapPopLogic* NewL( CMsvSession& aSession );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOImapPopLogic();
    
public:
    
    /**
     * Handles Always Online server commands
    * @param TInt, command Id
    * @param TDesC8*, parameter pack
    * @return TInt, return value
    */
    TInt HandleAOServerCommandL( 
        TInt    aCommand, 
        TDesC8* aParameters );
    
    /**
     * Handles msv session events
     * NOTE: this is called by CIpsSosAOEComInterface class, not directly
     * by msv session
     */
    void HandleMsvSessionEventL(
         MMsvSessionObserver::TMsvSessionEvent aEvent, 
         TAny* aArg1, TAny* aArg2, TAny* aArg3 );
    
private:
    
    /**
    * Constructor
    */
    CIpsSosAOImapPopLogic( CMsvSession& aSession );

    /**
    * Second phase constructor
    */
    void ConstructL();
    
    /*
     * sends TMboxLogicCommand to all mailboxes
     */
    void SendCommandToMailboxesL(TInt aCommand );
    
    /*
     * Sends Stop command to CIpsSosAOMBoxLogic object and removes it 
     */
    void StopAndRemoveMailboxL( TMsvId aMailboxId );
    
    /*
     * Sends TMboxLogicCommand to spesific mailbox
     */
    void SendCommandToSpecificMailboxL( 
            TMsvId aMailboxId, 
            TInt aCommand );
    
    /*
     * Handles emn message whitch is sent by always online server
     */
    void HandleEMNMessageL( const TDesC8& aParameters );
    
    /*
     * Checks all fs mailboxes, if timed sync is set on in settings, function
     * creates and starts mailbox logic object
     */
    void RefreshMailboxListL( );
    
    /*
     * Used by RefreshMailboxListL function.
     */
    void UpdateLogicArrayL( const CMsvEntrySelection& aChilds );
    
    /*
     * Checks is sync ongoing in mailbox and set value to descriptor, this is called by
     * always online server. Descriptor is send as a response of client - server async call 
     */
    TInt QueryMailboxStatus( TDesC8& aParameter );
    
    /*
     * get index of mailbox logic array according to mailbox id
     */
    inline TInt GetMailboxLogicIndex( TMsvId aMailboxId );

    /**
     * Removes orphaned logics. (mailbox has been deleted but logic has not been )
     */
    void RemoveOrphanLogicsL();
    
    /**
     * Takes care of reacting to entries created type of events
     */
    void handleEntriesCreatedL(const TAny* aArg1);

    /**
     * Takes the first entryId from the given selection and 
     * instantiates a TMsvEntry for that entry.
     * @param aSelection array of entry ids
     * @param aEntry On completion, contains the TMsvEntry data
     */
    void getFirstEntryFromSelectionL(const CMsvEntrySelection* aSelection, TMsvEntry& aEntry);

private:
    
    CMsvSession&                        iSession;
    
    // container of mailbox logic objects, each mailbox has own logic if always online is
    // set on in mailbox settings
    RPointerArray<CIpsSosAOMBoxLogic>   iMailboxLogics;
    
    CIpsSosAOEMNResolver*               iEmnResolver;
	//<QMail>

	//</QMail>
    // flag is set true when NoNWOpsAllowed (NW = network)
    // event is received, false when NWOpsAllowed is received
    TBool                               iNoNWOpsAllowed;
    
    };

#endif /* IPSSOSAOIMAPPOPLOGIC_H*/
