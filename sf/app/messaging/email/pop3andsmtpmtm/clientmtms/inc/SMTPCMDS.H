// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined (__SMTPCMDS_H__)
#define __SMTPCMDS_H__

#if !defined (__MSVSTD_HRH__)
#include <msvstd.hrh>
#endif

/** SMTP-specific commands that can be issued through CSmtpClientMtm::InvokeAsyncFunctionL(). 
@publishedAll
@released
*/
enum TSmtpCmds 
	{
	/** Not supported. */
	KSMTPMTMIsConnected = KMtmFirstFreeMtmFunctionId,
	/** Send messages on next connection.
	
	The first entry in aSelection specifies the SMTP service entry that is to 
	be used to send the email messages. aSelection may contain only one entry 
	ID. The SMTP MTM will append the Ids of all "ready" email messages which exist 
	in the outbox that match the IAP of the specified SMTP service. Whether an 
	email message is "ready" is determined from its TMsvEntry::SendingState() 
	flag.
	
	If an Internet connection has already been established by another client, 
	then this function may fail with KSmtpNoMsgsToSendWithActiveSettings if none 
	of the messages specified in aSelection match the IAP currently in use. */
	KSMTPMTMSendOnNextConnection,
	/** Creates a new message. */
	KSMTPMTMCreateNewEmailMessage,
	/** Creates a new reply message. */
	KSMTPMTMCreateReplyEmailMessage,
	/** Creates a new forwarded message. */
	KSMTPMTMCreateForwardEmailMessage,
	/** Creates a new forwarded message as an attachment. */
	KSMTPMTMCreateForwardAsAttachmentEmailMessage,
	/** Creates a new message-receipt message. */
	KSMTPMTMCreateReceiptEmailMessage
	};

#endif
