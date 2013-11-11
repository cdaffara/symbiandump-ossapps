// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CIMAPSESSIONCONSTS_H__
#define __CIMAPSESSIONCONSTS_H__

/**
Completion code indicating that the IMAP server returned a tagged NO response
*/
const TInt KErrImapNo = 1;
/**
Completion code indicating that the IMAP server returned a tagged BAD response
*/
const TInt KErrImapBad = 2;
/**
Completion code indicating that one of the streams have closed
*/
const TInt KErrImapClosed = 3;
/**
Completion code indicating that an attempt to flush a cancelled command has timed out
*/
const TInt KErrImapFlushTimeout = 4;
/**
Completion code indicating that corrupt data was enountered during parsing of IMAP server data
*/
const TInt KErrImapCorrupt = 5;
/**
Completion code returned by the session manager indicating that an error has occured while connecting a session.
*/
const TInt KErrImapConnectError = 6;

_LIT8(KImapTxtCrlf, "\r\n");
_LIT8(KImapTxtNil, "NIL");
_LIT8(KImapTxtOpenBracket, "(");
_LIT8(KImapTxtCloseBracket, ")");
_LIT8(KImapTxtDoubleQuote, "\"");
_LIT8(KImapTxtEscape, "\\");
_LIT8(KImapTxtEmptyStringAsDoubleQuotePair, "\"\""); // ""

// Tags and Response Codes
_LIT8(KImapTxtOk, "OK");
_LIT8(KImapTxtNo, "NO");
_LIT8(KImapTxtBad, "BAD");
_LIT8(KImapTxtContinuation, "+");
_LIT8(KImapTxtUntagged, "*");
_LIT8(KImapTxtBye, "BYE");
_LIT8(KImapTxtPreAuth, "PREAUTH");

// Commands (e.g. for checking tagged responses)
_LIT8(KImapTxtCheck, "CHECK");
_LIT8(KImapTxtExpunge, "EXPUNGE");
_LIT8(KImapTxtSearch, "SEARCH");
_LIT8(KImapTxtClose, "CLOSE");
_LIT8(KImapTxtAppend, "APPEND");
_LIT8(KImapTxtLSub, "LSUB");
_LIT8(KImapTxtList, "LIST");
_LIT8(KImapTxtRename, "RENAME");
_LIT8(KImapTxtDelete, "DELETE");
_LIT8(KImapTxtCreate, "CREATE");
_LIT8(KImapTxtNoop, "NOOP");
_LIT8(KTxtImapCapability, "CAPABILITY");
_LIT8(KImapTxtStatus, "STATUS");

// Unsolicited Responses
// Expunge already defined - see above
_LIT8(KImapTxtExists, "EXISTS");
_LIT8(KImapTxtRecent, "RECENT");

// Imap Capabilities
_LIT8(KImapTxtImapVersion, "IMAP4rev1");
_LIT8(KImapTxtStartTls, "STARTTLS");
_LIT8(KImapTxtLoginDisabled, "LOGINDISABLED");
_LIT8(KImapTxtIdle, "IDLE");

// Imap Capabilities: Authentication types that we're interested in
_LIT8(KImapTxtAuthPlain, "AUTH=PLAIN");

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
_LIT8(KImapTxtAuthCRamMd5, "AUTH=CRAM-MD5");
_LIT8(KImapTxtAuthLogin, "AUTH=LOGIN");
_LIT8(KImapTxtBinaryCap, "BINARY");
#endif

// Flags for FLAGS and FETCH FLAGS responses
_LIT8(KImapTxtFlagSeen, "\\Seen");
_LIT8(KImapTxtFlagAnswered, "\\Answered"); 
_LIT8(KImapTxtFlagDeleted, "\\Deleted");
_LIT8(KImapTxtFlagFlagged, "\\Flagged");
_LIT8(KImapTxtFlagDraft, "\\Draft");
_LIT8(KImapTxtFlagRecent, "\\Recent"); // This flag only appears in FETCH FLAGS responses, not FLAGS

// Select Flags
_LIT8(KImapTxtUidNext,"UIDNEXT");
_LIT8(KImapTxtUnseen,"UNSEEN");
_LIT8(KImapTxtUidValidity,"UIDVALIDITY");
_LIT8(KImapTxtMessages, "MESSAGES");

// List Item Flags
_LIT8(KImapTxtNoInferiors, "\\Noinferiors");
_LIT8(KImapTxtNoSelect, "\\Noselect");
_LIT8(KImapTxtMarked, "\\Marked");
_LIT8(KImapTxtUnmarked, "\\Unmarked");

// Bodystructure Types
_LIT8(KImapTxtMultipart, "MULTIPART");
_LIT8(KImapTxtText, "TEXT");
_LIT8(KImapTxtMessage, "MESSAGE");
_LIT8(KImapTxtRfc822, "RFC822");

// FETCH Data Items
_LIT8(KImapTxtFetch, "FETCH");
_LIT8(KImapTxtUid, "UID");
_LIT8(KImapTxtFlags, "FLAGS");
_LIT8(KImapTxtBodyStructure, "BODYSTRUCTURE");
_LIT8(KImapTxtBodyHeaderFields, "BODY[HEADER.FIELDS");
_LIT8(KImapTxtBody, "BODY[");
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
_LIT8(KImapTxtBinary, "BINARY[");
#endif
_LIT8(KImapTxtMime, ".MIME]");

// Header Fields
_LIT8(KImapTxtFrom, "From");
_LIT8(KImapTxtReplyTo, "Reply-to");
_LIT8(KImapTxtTo, "To");
_LIT8(KImapTxtCc, "Cc");
_LIT8(KImapTxtBcc, "Bcc");
_LIT8(KImapTxtSubject, "Subject");
_LIT8(KImapTxtDate, "Date");
_LIT8(KImapTxtReceived, "Received");
_LIT8(KImapTxtMessageId, "Message-ID");
_LIT8(KImapTxtXMailer, "X-Mailer");
_LIT8(KImapTxtPriority, "Priority");
_LIT8(KImapTxtXPriority, "X-Priority");
_LIT8(KImapTxtXMSMailPriority, "X-MSMail-Priority");
_LIT8(KImapTxtPrecedence, "Precedence");
_LIT8(KImapTxtImportance, "Importance");
_LIT8(KImapTxtReturnReceiptTo, "Return-Receipt-To");
_LIT8(KImapTxtXReturnReceiptTo, "X-Return-Receipt-To");
_LIT8(KImapTxtDispositionNotificationTo, "Disposition-Notification-To");
_LIT8(KImapTxtDispositionNotificationOptions, "Disposition-Notification-Options");

// MIME header fields
_LIT8(KImapTxtContentBase, "Content-Base");
_LIT8(KImapTxtContentLocation, "Content-Location");

#endif // __CIMAPSESSIONCONSTS_H__
