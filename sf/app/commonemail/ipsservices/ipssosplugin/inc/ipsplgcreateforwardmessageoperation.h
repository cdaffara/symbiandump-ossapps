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
* Description:  New forward message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEFORWARDMESSAGEOPERATION_H
#define IPSPLGCREATEFORWARDMESSAGEOPERATION_H

#include "ipsplgcreatemessageoperation.h"

/**
* class 
*
* Email operation to create a new forward message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateForwardMessageOperation ) : public CIpsPlgCreateMessageOperation
    {
public:
    //<qmail> comments added
	//<qmail> para to refe
    /**
    * Two-phased constructor
    * 
    * @param aSmtpService               contains SMTP message services
    * @param aMsvSession                client/server session to MsvServer
    * @param aObserverRequestStatus     operations observer status
    * @param aPartList                  specifies one or more parts of a message
    * @param aMailBoxId                 id's for mailbox and plugin
    * @param aOriginalMessageId         id of message to be forward
    * @param aOperationObserver         observes the progress of this operation
    * @param aRequestId                 identifier for this instance of operation, 
    *                                   assigned by the client
    * @return CIpsPlgCreateForwardMessageOperation* self pointer                                  
    */
    static CIpsPlgCreateForwardMessageOperation* NewL(
        CIpsPlgSmtpService& aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        TMsvId aOriginalMessageId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
        //</qmail>
    /**
    * Destructor.
    */
    virtual ~CIpsPlgCreateForwardMessageOperation();
    
private:

    /**
    * Constructor.
    */
		//<qmail> para to refe
    CIpsPlgCreateForwardMessageOperation(
        CIpsPlgSmtpService& aSmtpService,
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvPartList aPartList,
        TFSMailMsgId aMailBoxId,
        TMsvId aOriginalMessageId, 
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );
	//</qmail>
    /**
    * From CActive
    */
    void RunL();
    
    void StartMessageCreationL();

private:
    TMsvId iOriginalMessageId;
    };


#endif//IPSPLGCREATEFORWARDMESSAGEOPERATION_H

// </qmail>
