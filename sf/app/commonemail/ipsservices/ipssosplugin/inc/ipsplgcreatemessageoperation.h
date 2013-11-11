/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  New message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEMESSAGEOPERATION_H
#define IPSPLGCREATEMESSAGEOPERATION_H


class TFSMailMsgId;
class MFSMailRequestObserver;

/**
* class 
*
* Email operation to create a new message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateMessageOperation ) : public CIpsPlgBaseOperation
    {
public:
    //<qmail> comments added
	//<qmail> para to refe
	//<qmail> aSmtpServiceId removed
    /**
    * Two-phased constructor
    * 
    * @param aSmtpService               contains SMTP message services
    * @param aMsvSession                client/server session to MsvServer
    * @param aObserverRequestStatus     operations observer status
    * @param aPartList                  specifies one or more parts of a message
    * @param aMailBoxId                 id's for mailbox and plugin
    * @param aOperationObserver         observes the progress of this operation
    * @param aRequestId                 identifier for this instance of operation, 
    *                                   assigned by the client
    * @return CIpsPlgCreateMessageOperation* self pointer                                  
    */
    static CIpsPlgCreateMessageOperation* NewL(
        CIpsPlgSmtpService& aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
//</qmail>        
    /**
    * Destructor.
    */
    virtual ~CIpsPlgCreateMessageOperation();
    
    /**
    * From CMsvoperation
    */
    virtual const TDesC8& ProgressL();

    /**
    * From CIpsPlgBaseOperation
    */
    virtual const TDesC8& GetErrorProgressL(TInt aError);

    /**
    * From CIpsPlgBaseOperation
    */
    virtual TFSProgress GetFSProgressL() const;

    /**
     * From CIpsPlgBaseOperation
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;    

protected:

    /**
    * Constructor.
    */
		//<qmail> para to refe
	//<qmail> aSmtpServiceId removed
    CIpsPlgCreateMessageOperation(
        CIpsPlgSmtpService& aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
    //</qmail>
    /**
    * Constructor.
    */
    void ConstructL();
    
    void SignalFSObserver( TInt aStatus, CFSMailMessage* aMessage );
    
    TMsvId GetIdFromProgressL( const TDesC8& aProg );

    /**
    * From CActive
    */
    void DoCancel();
    
    /**
    * From CActive
    */
    void RunL();
    
    /**
    * From CActive
    */
    TInt RunError( TInt aError );

    void StartMessageCreation();

    virtual void StartMessageCreationL();

protected:
	//<qmail> para to refe
    CIpsPlgSmtpService& iSmtpService;   // not owned
    CMsvOperation* iOperation;   // owned 
	//<qmail> iSmtpServiceId removed
    TMsvPartList iPartList;
// <qmail> iBlank member removed
    MFSMailRequestObserver& iOperationObserver;  // not owned
    TFSProgress iFSProgress;
    };


#endif//IPSPLGCREATEMESSAGEOPERATION_H

// </qmail>
