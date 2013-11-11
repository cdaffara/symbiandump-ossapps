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
// MIUT_ERR.H
//

/**
 * @file 
 * @publishedAll
 * @released
 */

#ifndef __MIUT_ERR_H__
#define __MIUT_ERR_H__

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#endif

/**
@publishedAll
@released
*/
const TInt KMiutErrorBase						= (-170);

// POP3-specific error codes

/**
@publishedAll
@released
*/
const TInt KPop3ErrorBase						= KMiutErrorBase;

/**
@publishedAll
@released
*/
const TInt KPop3CannotConnect					= KPop3ErrorBase;

/**
@publishedAll
@released
*/
const TInt KPop3InvalidUser						= KPop3ErrorBase-1;

/**
@publishedAll
@released
*/
const TInt KPop3InvalidLogin					= KPop3ErrorBase-2;

/**
@publishedAll
@released
*/
const TInt KPop3CannotCreateApopLogonString		= KPop3ErrorBase-3;

/**
@publishedAll
@released
*/
const TInt KPop3ProblemWithRemotePopServer		= KPop3ErrorBase-4;

/**
@publishedAll
@released
*/
const TInt KPop3CannotOpenServiceEntry			= KPop3ErrorBase-5;

/**
@publishedAll
@released
*/
const TInt KPop3CannotSetRequiredFolderContext	= KPop3ErrorBase-6;

/**
@publishedAll
@released
*/
const TInt KPop3InvalidApopLogin				= KPop3ErrorBase-7;

/**
@publishedAll
@released
*/
// if Authentcation flag is ON, Fallback flag is OFF and email server doesn't support CRAM-MD5 
// authentication mechanism, then KPop3AuthenticationFailed error will be returned.
const TInt KPop3AuthenticationFailed			= KPop3ErrorBase-8;


/**
@publishedAll
@released
*/
const TInt KPopTopError							= (-179);

// IMSK-specific error code
/**
@publishedAll
@released
*/
const TInt KImskBaseError						= (-180);

/**
@publishedAll
@released
*/
const TInt KImskErrorDNSNotFound				= KImskBaseError;

/**
@publishedAll
@released
*/
const TInt KImskErrorControlPanelLocked			= KImskBaseError-1;

/**
@publishedAll
@released
*/
const TInt KImskErrorISPOrIAPRecordNotFound		= KImskBaseError-2;

/**
@publishedAll
@released
*/
const TInt KImskErrorActiveSettingIsDifferent	= KImskBaseError-3;

/**
@publishedAll
@released
*/
const TInt KImskSecuritySettingsFailed			= KImskBaseError-4;

/**
@publishedAll
@released
*/
const TInt KImskSSLTLSNegotiateFailed			= KImskBaseError-5;

/**
@publishedAll
@released
*/
const TInt KImskTopError						= (-190);

// error codes -190 to -199 are used by Sockets

// IMAP-specific error codes
/**
@publishedAll
@released
*/
const TInt KImapBaseError						= (-200);

/**
@publishedAll
@released
*/
const TInt KErrImapConnectFail					= KImapBaseError;

/**
@publishedAll
@released
*/
const TInt KErrImapServerFail					= KImapBaseError-1;

/**
@publishedAll
@released
*/
const TInt KErrImapServerParse					= KImapBaseError-2;

/**
@publishedAll
@released
*/
const TInt KErrImapServerBusy					= KImapBaseError-3;

/**
@publishedAll
@released
*/
const TInt KErrImapServerVersion				= KImapBaseError-4;

/**
@publishedAll
@released
*/
const TInt KErrImapSendFail						= KImapBaseError-5;

/**
@publishedAll
@released
*/
const TInt KErrImapBadLogon						= KImapBaseError-6;

/**
@publishedAll
@released
*/
const TInt KErrImapSelectFail					= KImapBaseError-7;

/**
@publishedAll
@released
*/
const TInt KErrImapWrongFolder					= KImapBaseError-8;

/**
@publishedAll
@released
*/
const TInt KErrImapServerNoSecurity				= KImapBaseError-9;

/**
@publishedAll
@released
*/
const TInt KErrImapServerLoginDisabled			= KImapBaseError-10;

/**
@publishedAll
@released
*/
const TInt KErrImapTLSNegotiateFailed  			= KImapBaseError-11;

/**
@publishedAll
@released
*/
const TInt KErrImapCantDeleteFolder  			= KImapBaseError-12;

/**
@publishedAll
@released
*/
const TInt KErrImapInvalidServerResponse  		= KImapBaseError-13;

/**
@publishedAll
@released
*/
//This POP error declared in IMAP error range, 
//since there is no scope to declare the error within pop range. 
const TInt KErrPop3ServerAlreadyConnected 		= KImapBaseError-14;


/**
@publishedAll
@released
*/
// if Authentcation flag is ON, Fallback flag is OFF and email server doesn't support CRAM-MD5 
// authentication mechanism, then KErrImapAuthenticationFailed error will be returned.
const TInt KErrImapAuthenticationFailed		= 	KImapBaseError-15;

/**
@publishedAll
@released
*/
const TInt KImapTopError						= (-219);

// DMSS-specific error codes
/**
@publishedAll
@released
*/
const TInt KDmssBaseError						= (-220);

/**
@publishedAll
@released
*/
const TInt KDmssUnknownErr						= (KDmssBaseError-1);

/**
@publishedAll
@released
*/
const TInt KDmssMailboxUnavailableErr			= (KDmssBaseError-2);

/**
@publishedAll
@released
*/
const TInt KDmssActionAbortedErr				= (KDmssBaseError-3);

/**
@publishedAll
@released
*/
const TInt KDmssActionNotTakenErr				= (KDmssBaseError-4);

/**
@publishedAll
@released
*/
const TInt KDmssCmdUnrecognisedErr				= (KDmssBaseError-5);

/**
@publishedAll
@released
*/
const TInt KDmssSyntaxErrorErr					= (KDmssBaseError-6);

/**
@publishedAll
@released
*/
const TInt KDmssCmdNotImplementedErr			= (KDmssBaseError-7);

/**
@publishedAll
@released
*/
const TInt KDmssBadSequenceErr					= (KDmssBaseError-8);

/**
@publishedAll
@released
*/
const TInt KDmssParamNotImplementedErr			= (KDmssBaseError-9);

/**
@publishedAll
@released
*/
const TInt KDmssMailboxNoAccessErr				= (KDmssBaseError-10);

/**
@publishedAll
@released
*/
const TInt KDmssExceededStorageErr				= (KDmssBaseError-11);

/**
@publishedAll
@released
*/
const TInt KDmssMailboxNameErr					= (KDmssBaseError-12);

/**
@publishedAll
@released
*/
const TInt KDmssTransactionFailedErr			= (KDmssBaseError-13);

/**
@publishedAll
@released
*/
const TInt KDmssTimeOutErr						= (KDmssBaseError-14);

/**
@publishedAll
@released
*/
const TInt KDmssTopError						= (-239);

// SMTP client-specific error codes
/**
@publishedAll
@released
*/
const TInt KSmtpBaseError						= (-240);

/**
@publishedAll
@released
*/
const TInt KSmtpNoMailFromErr					=  KSmtpBaseError;

/**
@publishedAll
@released
*/
const TInt KSmtpUnknownErr						= (KSmtpBaseError-1);

/**
@publishedAll
@released
*/
const TInt KSmtpBadMailFromAddress				= (KSmtpBaseError-2);

/**
@publishedAll
@released
*/
const TInt KSmtpBadRcptToAddress				= (KSmtpBaseError-3);

/**
@publishedAll
@released
*/
const TInt KSmtpLoginRefused					= (KSmtpBaseError-4);

/**
@publishedAll
@released
*/
const TInt KSmtpNoMsgsToSendWithActiveSettings	= (KSmtpBaseError-5);

/**
@publishedAll
@released
*/
const TInt KErrSmtpTLSNegotiateFailed 			= (KSmtpBaseError-6);

/**
@publishedAll
@released
*/
const TInt KErrSmtpBufferOverFlow 			    = (KSmtpBaseError-7);

/**
@publishedAll
@released
*/
const TInt KSmtpTopError					    = (-249);
 
// IMCM error codes
/**
@publishedAll
@released
*/
const TInt KImcmBaseError						= (-250);

/**
@publishedAll
@released
*/
const TInt KImcmHTMLPartNotPopulated			=  KImcmBaseError;

/**
@publishedAll
@released
*/
const TInt KImcmInvalidMessageStructure			= (KImcmBaseError-1);

// POPS error codes
/**
@publishedAll
@released
*/
const TInt KPop3BaseError						= (-260);

/**
@publishedAll
@released
*/
const TInt KErrPop3TLSNegotiateFailed			=  KPop3BaseError;


#endif // __MIUT_ERR_H__
