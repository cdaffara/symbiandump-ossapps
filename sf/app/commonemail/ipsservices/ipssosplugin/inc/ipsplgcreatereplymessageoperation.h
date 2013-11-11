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
* Description:  New reply message creation operation
*
*/

// <qmail>
#ifndef IPSPLGCREATEREPLYMESSAGEOPERATION_H
#define IPSPLGCREATEREPLYMESSAGEOPERATION_H

#include "ipsplgcreatemessageoperation.h"

/**
* class 
*
* Email operation to create a new reply message asynchronously.
*/
NONSHARABLE_CLASS ( CIpsPlgCreateReplyMessageOperation ) : public CIpsPlgCreateMessageOperation
    {
public:
    //<qmail> comments + aSmtpService -> reference
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
    static CIpsPlgCreateReplyMessageOperation* NewL(
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
    virtual ~CIpsPlgCreateReplyMessageOperation();
    
private:

    /**
    * Constructor.
    */
//<qmail>	
    CIpsPlgCreateReplyMessageOperation(
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
    //<qmail>
    /**
    * Copy recipients from original message to new one 
    * 
    * @param aHeader            IM header of original message
    * @param aNewMessage        Reply message where recipients added
    */ 
    void RecipientsCallToOrderL(
            CImHeader* aHeader,
            CFSMailMessage* aNewMessage );
//</qmail>
private:
    TMsvId iOriginalMessageId;
    };


#endif//IPSPLGCREATEREPLYMESSAGEOPERATION_H

// </qmail>
