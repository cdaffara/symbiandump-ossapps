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
* Description: This file defines class CIpsPlgSmtpOperation.
*
*/


#ifndef IPSPLGSENDOPERATION_H
#define IPSPLGSENDOPERATION_H

#include <smtcmtm.h>
#include "ipsplgcommon.h"
// <qmail>
#include "ipsplgbaseoperation.h"

class CClientMtmRegistry;
class MFSMailRequestObserver;
// </qmail>

/**
 *  Class for smtp related operations
 *
 *  @lib ipssosplugin.lib
 *  @since FS 1.0
 */
// <qmail> base class changed: CMsvOperation -> CIpsPlgBaseOperation, MIpsPlgConnectOpCallback removed
NONSHARABLE_CLASS( CIpsPlgSmtpOperation ) : public CIpsPlgBaseOperation
    {

// <qmail> CredientialsSetL removed 

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    // <qmail> aPriority, aUsePublishSubscribe parameters removed, aFSOperationObserver, aFSRequestId added
    IMPORT_C static CIpsPlgSmtpOperation* NewL( 
        CMsvSession& aMsvSession, 
        TRequestStatus& aObserverRequestStatus,
        MFSMailRequestObserver* aFSOperationObserver = NULL,
        TInt aFSRequestId = KErrNotFound );

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    // <qmail> aPriority, aUsePublishSubscribe parameters removed, aFSOperationObserver, aFSRequestId added
    IMPORT_C static CIpsPlgSmtpOperation* NewLC(
        CMsvSession& aMsvSession, 
        TRequestStatus& aObserverRequestStatus,
        MFSMailRequestObserver* aOperationObserver = NULL,
        TInt aFSRequestId = KErrNotFound );

    /**
     * Class destructor
     *
     * @since FS 1.0
     * @return None
     */ 
    virtual ~CIpsPlgSmtpOperation( );
    
    /**
     * From CMsvOperation
     */ 
    virtual const TDesC8& ProgressL();
    
// <qmail>
    virtual const TDesC8& GetErrorProgressL( TInt aError );

    virtual TFSProgress GetFSProgressL() const;
// </qmail>

// <qmail> change ret val type
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
// </qmail>

    /**
     * Stard sending operation
     *
     * @param aSelection message selection for send messages
     * @since FS 1.0
     * @return None
     */     
    IMPORT_C void StartSendL( TMsvId aMessageId );
    
    IMPORT_C TInt EmptyOutboxFromPendingMessagesL( TMsvId aMailboxId );

    IMPORT_C CMsvEntrySelection* GetOutboxChildrensL( );

	/**
	 *  Sets CIpsPlgEventHandler
	 */
	void SetEventHandler( TAny* aEventHandler );
    
protected:

    /**
     * Class constructor
     *
     * @since FS 1.0
     * @return None
     */
    // <qmail> aPriority parameter removed, aFSOperationObserver, aFSRequestId added
    CIpsPlgSmtpOperation( 
        CMsvSession& aMsvSession, 
        TRequestStatus& aObserverRequestStatus,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );
    
    /**
     * Constructor for leaving methods
     *
     * @since FS 1.0
     * @return None
     */    
    void ConstructL( );

    /**
     * Completes observer with status aStatus
     * @param aStatus: Status of the operation.
     */
    void CompleteObserver( TInt aStatus = KErrNone );

    /**
     * Completes itself with KErrNone status
     */
    void CompleteThis();

private: // From CActive

    enum TIpsSendState
        {
        EIdle,
        EMovingOutbox,          // moving mail to OutBox folder
        ESending                // sending mail
		// <qmail> EQueryingDetails, EQueryingDetailsBusy removed
        };

    /**
     * From CActive
     *
     * @since FS 1.0
     */
    void RunL( );

// <qmail>
    TInt RunError( TInt aError );
// </qmail>

    void DoCancel( );
    
    /**
     *
     * @since FS 1.0
     */
    void CallSendL( );
    
    /**
     *
     * @since FS 1.0
     */
    void ConstructSmtpMtmL();
    
    /**
     * Validates all recipient addresses (to, cc, bcc)
     * Either returns silently if everything id ok, or leaves with KErrBadName
     * @param aEntry refers to message to be validated
     */
    void ValidateRecipientsL( CMsvEntry& aEntry );
    
    /**
     * Validates an array of address
     * Either returns silently if everything id ok, or leaves with KErrBadName
     * @param aRecipients array of addresses
     */
    void ValidateAddressArrayL( const CDesCArray& aRecipients );

	// <qmail> QueryUserPassL() function removed
private:

    CSmtpClientMtm*         iSmtpMtm;
    CMsvOperation*          iOperation;
    CMsvEntrySelection*     iSelection;
    CClientMtmRegistry*     iMtmRegistry;
    TInt                    iState;
    TMsvId                  iSmtpService;
// <qmail>
    MFSMailRequestObserver* iFSOperationObserver;
    TFSProgress             iFSProgress;
// </qmail>
	// not owned
    TAny*               	iEventHandler; // pointer to CIpsPlgEventHandler
    };

#endif /* IPSPLGSENDOPERATION_H */
