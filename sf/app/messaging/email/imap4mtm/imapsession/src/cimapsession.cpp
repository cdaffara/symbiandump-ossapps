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


#include "cimapsession.h"

#include "moutputstream.h"
#include "minputstream.h"

#include "cimapsessionconsts.h"

#include "cimapidle.h"
#include "cimapservereventwait.h"
#include "cimapservergreeting.h"
#include "cimapservergreetinginfo.h"
#include "cimapcapability.h"
#include "cimapcapabilityinfo.h"
#include "cimapnoop.h"
#include "cimaplogout.h"
#include "cimaplogin.h"
#include "cimapstarttls.h"
#include "cimapselect.h"
#include "cimapexamine.h"
#include "cimapcreate.h"
#include "cimapdelete.h"
#include "cimaprename.h"
#include "cimapsubscribe.h"
#include "cimapunsubscribe.h"
#include "cimaplist.h"
#include "cimaplsub.h"
#include "cimapstatus.h"
#include "cimapappend.h"
#include "cimapsearch.h"
#include "cimapclose.h"
#include "cimapstore.h"
#include "cimapcopy.h"
#include "cimapexpunge.h"
#include "cimapfetchflags.h"
#include "cimapfetchbody.h"
#include "cimapfetchsinglebodystructure.h"
#include "cimapfetchmultibodystructures.h"
#include "cimapfetchbodyresponse.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	#include "cimapauthhelpers.h"
#endif
/**
An RBuf buffer is used to create the sequence set string.
This specifies by how the buffer should grow each time it needs to be expanded.
*/
const TInt KSequenceSetGranularity = 32;
/**
A CBufFlat buffer is used to store incoming string data whenever incoming data needs to be spliced
together in order to form a contiguous line or literal block.
This specifies by how the buffer should grow each time it needs to be expanded.
*/
const TInt KInputCacheGranularity = 1024;
/**
During a flush (after cancelling a command),
time out if data is not received within 10 seconds of requesting it.
*/
const TInt KFlushTimeoutPeriod = 10*1000*1000;

#ifdef __IMAP_LOGGING
_LIT(KLogFileName, "imlog%d");
#endif //__IMAP_LOGGING

/**
The factory constructor. Part of two phased construction.
*/
EXPORT_C CImapSession* CImapSession::NewL(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, MInputStream& aInputStream, MOutputStream& aOutputStream)
// static method
	{
	CImapSession* self = new(ELeave) CImapSession(aImapSettings, aImapMailStore, aInputStream, aOutputStream);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImapSession::CImapSession(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, MInputStream& aInputStream, MOutputStream& aOutputStream)
	: iImapSettings(aImapSettings), iImapMailStore(aImapMailStore) 
	, iInputStream(&aInputStream), iOutputStream(&aOutputStream)
	{}

void CImapSession::ConstructL()
	{
	__ASSERT_DEBUG(iInputStream != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullInputStream));
	__ASSERT_DEBUG(iOutputStream != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullOutputStream));

#ifdef __IMAP_LOGGING	
	//Getting the server port number for creating the IMAP log file
	TInt portNum = 143;
	TBuf<10> fileName;
	fileName.AppendFormat(KLogFileName, portNum);
	__LOG_CREATE_STANDARD(fileName, iLogId);
#endif //__IMAP_LOGGING	

	iServerGreetingInfo = CImapServerGreetingInfo::NewL();
	iCapabilityInfo = CImapCapabilityInfo::NewL();
	iInputCache = CBufFlat::NewL(KInputCacheGranularity);

	TCallBack callBack(CImapSession::AsyncCallBack, this);
	iAsyncCallBack = new(ELeave)CAsyncCallBack(callBack, CActive::EPriorityStandard);
	
	iFlushCommandTimeout = CImapObservableTimer::NewL(*this);
	
	iInputStream->Bind(*this, iLogId);
	iOutputStream->Bind(*this, iLogId);
	}
	
CImapSession::~CImapSession()
	{
	Cancel();

	// Close the streams if they are not already closed. Just closing the
	// output stream will also close the input stream.
	if (iOutputStream != NULL)
		{
		iOutputStream->Close();
		}

	delete iFlushCommandTimeout;
	delete iAsyncCallBack;
	
	delete iCurrentCommand;
	
	delete iSelectedFolderInfo;
	delete iCapabilityInfo;	
	delete iServerGreetingInfo;
	
	delete iInputCache;

	__LOG_CLOSE(iLogId);
	}

/**
Returns the current server state.
EServerStateNone indicates that the server greeting has not yet been received.
CImapSession will panic if an attempt is made to call invoke an IMAP command when the IMAP server is 
not in the correct state for the command.
This method can be called to check the state before invoking such a command.
@return The current server state.
*/
EXPORT_C CImapSession::TImapServerState CImapSession::ServerState()
	{
	return iServerState;
	}

/**
Returns inforamtion about the currently selected folder.
If no folder is currently selected, then NULL will be returned.
@return information about the currently selected folder or NULL if no folder is currently selected.
*/
EXPORT_C CImapFolderInfo* CImapSession::SelectedFolderInfo()
	{
	return iSelectedFolderInfo;
	}

/** 
Returns the server greeting information object.
The data in the object will only be valid after a greeting has been received from the server.
This must be requested using the ReadServerGreetingL() method immediately after connection has been made.
@return The server greeting information object.
*/
EXPORT_C const CImapServerGreetingInfo& CImapSession::ServerGreetingInfo()
	{
	return *iServerGreetingInfo;
	}

/** 
Returns the server capability information object.
The data in the object will only be valid after a capability response has been received from the server.
This can be requested using the CapabilityL() method.
@return The server capability information object.
*/
EXPORT_C const CImapCapabilityInfo& CImapSession::CapabilityInfo()
	{
	return *iCapabilityInfo;
	}

/** 
@return ETrue if IMAP IDLE mode is supported by the server.
*/
EXPORT_C TBool CImapSession::ImapIdleSupported() const
	{
	return iCapabilityInfo->QueryFlag(CImapCapabilityInfo::EIdle);
	}

/** 
Request to enter IMAP IDLE mode.
This method will complete when confirmation of entering IDLE mode has been received 
from the server, via the '+' continuation response.
The server may send untagged status information BEFORE sending the continuation response.
This will be recorded and can be accessed via SelectedFolderInfo().
Upon completion of EnterIdleL(), 
WaitForIdleEvent() should be called in order to wait for the next status information event, 
or DoneIdle() should be called to stop receiving further status information.
@param aStatus Signals the receipt of the continuation response.
*/
EXPORT_C void CImapSession::EnterIdleL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::EnterIdleL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapIdle::NewL(iSelectedFolderInfo, iLogId));
	
	SetPendingOperation(EOpIdleEnter);
	}
	
/**
Request to be notified of an IMAP IDLE event.
This method will complete when the next line of status information is received.
The updated information can be accessed via SelectedFolderInfo().
However, the information might not be complete as further lines of status information may 
have been sent but not yet received.
Upon completion of WaitForIdleEvent(), 
WaitForIdleEvent() should be called again in order to receive the next event
or DoneIdle() should be called to receive complete status information 
and to stop receiving further information.
This method will panic if the session is not currently in IDLE mode.
Use EnterIdleL() to enter IDLE mode.
@param aStatus Signals the receipt of a line of status information.
*/
EXPORT_C void CImapSession::WaitForIdleEvent(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::WaitForIdleEvent()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleWaitWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpIdleEnter || iPendingOperation == EOpIdleWait, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleWaitWhenOperationPendingNotIdle));
	__ASSERT_DEBUG(iSessionState == ESessionNormal, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleWaitWhenSessionNonNormal));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpIdleWait);
		
	Queue(aStatus);
	
	// Restart the ProcessInputBufferL() loop asynchronously
	ProcessInputBufferAsync();
	}
	
/** 
Request to exit IMAP IDLE mode.
This will cause the session to send a DONE message to the server.
The method will complete when a tagged response is received.
Upon completion, up-to-date status information can be accessed via SelectedFolderInfo().
This method will panic if the session is not currently in IDLE mode.
Use EnterIdleL() to enter IDLE mode.
@param aStatus Signals completion of the request.
*/
EXPORT_C void CImapSession::DoneIdle(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::DoneIdle()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleDoneWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpIdleEnter || iPendingOperation == EOpIdleWait, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleDoneWhenWhenInvalidOperationPending));
	__ASSERT_DEBUG(iSessionState == ESessionNormal, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleDoneWhenSessionNonNormal));
	__ASSERT_DEBUG(iCurrentCommand != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullCurrentCommand));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	CImapIdle* idleCommand = static_cast<CImapIdle*>(iCurrentCommand);

	SetPendingOperation(EOpIdleDone); // Treat as a normal command.
	SetSessionState(ESessionNormal); // If called after cancel, then set the session back to normal state.
	
	// iOutputStream must be non-null because CompleteIfStreamsClosed() returned EFalse.
	idleCommand->SendDone(*iOutputStream);
	
	Queue(aStatus);
	
	// Restart the ProcessInputBufferL() loop asynchronously
	ProcessInputBufferAsync();
	}

/**
This method does not send a command to the server, 
but simply waits on the input stream for any unsolicited response message
that the server might decide to send.  See section 5.3 of RFC3501.
This method should be used in place of the IDLE methods when no command is
being processed.
To cancel this method, call Cancel() followed by FlushCancelledCommand() to 
ensure that partial responses are not left on the input stream before 
the issuing the next command
@param aStatus Signals that an unsolicited response has been received.
*/
EXPORT_C void CImapSession::WaitForServerEventL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::WaitForServerEventL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EWaitForServerEventWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EWaitForServerEventWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapServerEventWait::NewL(iSelectedFolderInfo, iLogId));
	}

/** 
Reads and parses the server greeting from the input stream.
This method must (and must only) be called immediately after making a connection to the server.
Once this method has completed succesfully, the server greeting information will be accessible 
from the ServerGreetingInfo() method of this object.
@return The server greeting information object.
*/
EXPORT_C void CImapSession::ReadServerGreetingL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::ReadServerGreetingL()");
	__ASSERT_DEBUG(iServerState == EServerStateNone, TImapServerPanic::ImapPanic(TImapServerPanic::EReadServerGreetingWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EReadServerGreetingWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpServerGreeting); // Next server state depends on ServerGreeting result: could be PreAuth
	StartCommandL(aStatus, CImapServerGreeting::NewL(iSelectedFolderInfo, iLogId, *iServerGreetingInfo));
	}

/** 
Issue the IMAP capability command to gather IMAP capability information from the remote server.
Once this method has completed successfully, the capability information will be accessible 
from the CapabilityInfo() method of this object.
@param aStatus Signals completion of the request
@see CapabilityInfo()
*/
EXPORT_C void CImapSession::CapabilityL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::CapabilityL()");
	__ASSERT_DEBUG(iServerState != EServerStateNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECapabilityWhenServerStateUnknown));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECapabilityWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapCapability::NewL(iSelectedFolderInfo, iLogId, *iCapabilityInfo));
	}
/** 
Issue the IMAP noop command.
When in selected state, and upon completions, the caller should 
check SelectedFolderInfo() for any updates.
@param aStatus 		Signals completion of the request
*/
EXPORT_C void CImapSession::NoopL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::NoopL()");
	__ASSERT_DEBUG(iServerState != EServerStateNone, TImapServerPanic::ImapPanic(TImapServerPanic::ENoopWhenServerStateUnknown));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ENoopWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapNoop::NewL(iSelectedFolderInfo, iLogId));
	}

/**
Issue the IMAP logout command.
@param aStatus Signals completion of the request
*/
EXPORT_C void CImapSession::LogoutL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::LogoutL()");
	__ASSERT_DEBUG(iServerState != EServerStateNone, TImapServerPanic::ImapPanic(TImapServerPanic::ELogOutWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ELogOutWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpLogout); // Server state becomes None (i.e. disconnected)
	StartCommandL(aStatus, CImapLogout::NewL(iSelectedFolderInfo, iLogId));
	}
	
/**
Issue the IMAP login command.
@param aStatus Signals completion of the request
@param aUserName A user name supplied by the user
@param aPassword A password supplied by the user
*/
EXPORT_C void CImapSession::LoginL(TRequestStatus& aStatus, const TDesC8& aUserName, const TDesC8& aPassword)
	{
	__LOG_TEXT(iLogId, "CImapSession::LoginL()");
	__ASSERT_DEBUG(iServerState == EServerStateNotAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ELoginWhenServerStateUnknown));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ELoginWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpLogin); // << Server state becomes Authenticated
	StartCommandL(aStatus, CImapLogin::NewL(iSelectedFolderInfo, iLogId, aUserName, aPassword, *iCapabilityInfo));
	}
	
/** 
Issue the IMAP StartTLS command
@param aStatus Signals completion of the request
*/
EXPORT_C void CImapSession::StartTlsL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::StartTlsL()");
	__ASSERT_DEBUG(iServerState == EServerStateNotAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EStartTLSWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EStartTLSWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapStartTls::NewL(iSelectedFolderInfo, iLogId));
	}

/**
Issue the IMAP select command.
Opens the folder identified by aFolderInfo.Name() for read-write operations.
@param aStatus Signals completion of the request
@param aFolderInfo The folder object that represents the mailbox.  CImapSession takes ownership of this object.
The folder object initially contains the name of the folder to select.
The folder object will be updated upon successful completion of the command.
The folder object will be updated when any subsequent command receives folder information from the server.
*/
EXPORT_C void CImapSession::SelectL(TRequestStatus& aStatus, CImapFolderInfo* aFolderInfo, TBool aSelectBox)
	{
	__LOG_TEXT(iLogId, "CImapSession::SelectL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ESelectWhenInvalidOperationPending));
	__ASSERT_DEBUG(aFolderInfo != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionImapFolderInfoIsNull));
	
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		// Attempt to select while the stream is closed.
		// This failed select effectively puts us into the authenticated server state, 
		// where iSelectedFolderInfo should be NULL.
		delete iSelectedFolderInfo;
		iSelectedFolderInfo = NULL;		

		delete aFolderInfo; // No need to set to NULL as we're returning.
		return;
		}
			// Folder is already SELECTed so dont select again.. Hope this saves users bandwidth 
	if(aSelectBox)		
	{
		if(iSelectedFolderInfo && iSelectedFolderInfo->MsvId() == aFolderInfo->MsvId())
			{
			Queue(aStatus);
			Complete(KErrNone);
			delete aFolderInfo;
			return;
			}	
	}
	CleanupStack::PushL(aFolderInfo); // Take ownership but don't assign as a data member yet
	
	SetPendingOperation(EOpSelect); // Server state becomes Selected if successful, otherwise becomes Authenticated - and selected folder info becomes NULL
	StartCommandL(aStatus, CImapSelect::NewL(aFolderInfo, iLogId));
	
	// Overwrite any existing folder info, prior to performing the select
	// If select succeeds, iSelectedFolderInfo will be correct.
	// If select fails then iSelectedFolderInfo will be deleted and set to NULL later, as the state will 
	//   revert from Selected to Authenticated.
	
	CleanupStack::Pop(aFolderInfo); // Now we can assign aFolderInfo as a data member
	
	delete iSelectedFolderInfo;
	iSelectedFolderInfo = aFolderInfo;
	}

/**
Issue the IMAP examine command.
Opens the folder identified by aFolderInfo.Name() for read-only operations.
@param aStatus Signals completion of the request
@param aFolderInfo The folder object that represents the mailbox.  CImapSession takes ownership of this object.
The folder object initially contains the name of the folder to examine.
The folder object will be updated upon successful completion of the command.
The folder object will be updated when any subsequent command receives folder information from the server.
*/
EXPORT_C void CImapSession::ExamineL(TRequestStatus& aStatus, CImapFolderInfo* aFolderInfo)
	{
	__LOG_TEXT(iLogId, "CImapSession::ExamineL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EExamineWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EExamineWhenInvalidOperationPending));
	__ASSERT_DEBUG(aFolderInfo != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionImapFolderInfoIsNull));

	if (CompleteIfStreamsClosed(aStatus)) 
		{
		// Attempt to select while the stream is closed.
		// This failed select effectively puts us into the authenticated server state, 
		// where iSelectedFolderInfo should be NULL.
		delete iSelectedFolderInfo;
		iSelectedFolderInfo = NULL;		
		
		delete aFolderInfo; // No need to set to NULL as we're returning.		
		return;
		}
	
	CleanupStack::PushL(aFolderInfo); // Take ownership but don't assign as a data member yet
	
	SetPendingOperation(EOpSelect); // Server state becomes Selected if successful, otherwise becomes Authenticated - and selected folder info becomes NULL
	StartCommandL(aStatus, CImapExamine::NewL(aFolderInfo, iLogId));
	
	// Overwrite any existing folder info, prior to performing the select
	// If select succeeds, iSelectedFolderInfo will be correct.
	// If select fails then iSelectedFolderInfo will be deleted and set to NULL later, as the state will 
	//   revert from Selected to Authenticated.
	
	CleanupStack::Pop(aFolderInfo); // Now we can assign aFolderInfo as a data member
	
	delete iSelectedFolderInfo;
	iSelectedFolderInfo = aFolderInfo;
	}

/**
Issue the IMAP create command
Creates a remote mailbox with the name given by aMailboxName
@param aStatus Signals completion of the request
@param aMailboxName The name to be given to the new mailbox.
					This is the FULL PATH to the mailbox, as understood by the remote server.
*/
EXPORT_C void CImapSession::CreateL(TRequestStatus& aStatus, const TDesC& aMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::CreateL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECreateWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapCreate::NewL(iSelectedFolderInfo, iLogId, aMailboxName));
	}

/** 
Issue the IMAP delete command
Deletes the folder identified by aMailboxName.
@param aStatus Signals completion of the request.
@param aMailboxName The name of the mailbox that is to be selected.
					This is the FULL PATH to the mailbox, as understood by the remote server.
*/
EXPORT_C void CImapSession::DeleteL(TRequestStatus& aStatus, const TDesC& aMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::DeleteL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapDelete::NewL(iSelectedFolderInfo, iLogId, aMailboxName));
	}

/**
Issue the IMAP rename command.
Renames the folder identified by aExistingMailboxName to aNewMailboxName.
@param aStatus Signals completion of the request.
@param aExistingMailboxName The current name of the mailbox that is to be renamed
@param aNewMailboxName The new name to be given to the mailbox
*/
EXPORT_C void CImapSession::RenameL(TRequestStatus& aStatus, const TDesC& aExistingMailboxName, const TDesC& aNewMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::RenameL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapRename::NewL(iSelectedFolderInfo, iLogId, aExistingMailboxName, aNewMailboxName));
	}

/** 
Issue the IMAP subscribe command.
Subscribes to the folder identified by aMailboxName.
@param aStatus Signals completion of the request.
@param aMailboxName The name of the mailbox that is to be subscribed to.
					This is the FULL PATH to the mailbox, as understood by the remote server.
*/
EXPORT_C void CImapSession::SubscribeL(TRequestStatus& aStatus, const TDesC& aMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::SubscribeL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ESubscribeWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ESubscribeWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapSubscribe::NewL(iSelectedFolderInfo, iLogId, aMailboxName));
	}

/**
Issue the IMAP unsubscribe command
Unsubscribes from the folder identified by aMailboxName.
@param aStatus Signals completion of the request.
@param aMailboxName The name of the mailbox that is to be unsubscribed from.
					This is the FULL PATH to the mailbox, as understood by the remote server.
*/
EXPORT_C void CImapSession::UnsubscribeL(TRequestStatus& aStatus, const TDesC& aMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::UnsubscribeL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EUnSubscribeWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EUnSubscribeWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapUnsubscribe::NewL(iSelectedFolderInfo, iLogId, aMailboxName));
	}

/**
Issue the IMAP list command
Populates aFolderList with information about the requested mailbox.
@param aStatus Signals completion of the request.
@param aReferenceName	The reference name can be used to qualify the path of the mailbox.  See section 6.3.8 of RFC 3501
@param aMailboxName		Specifies which mailbox to list.  Only subfolders of the specified mailbox will be listed.
						This is the FULL PATH to the mailbox, as understood by the remote server.
@param aFolderList An array which will be populated with the list of folders upon successful completion of the command.
*/
EXPORT_C void CImapSession::ListL(TRequestStatus& aStatus, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList)
	{
	__LOG_TEXT(iLogId, "CImapSession::ListL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EListWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EListWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapList::NewL(iSelectedFolderInfo, iLogId, aReferenceName, aMailboxName, aFolderList, iImapSettings));
	}

/**
Issue the IMAP lsub command
Populates aFolderList with information about the requested remotely subscribed mailbox.
@param aStatus Signals completion of the request.
@param aReferenceName	The reference name can be used to qualify the path of the mailbox.  See section 6.3.9 of RFC 3501
@param aMailboxName		Specifies which mailbox to list.  Only subscribed subfolders of the specified mailbox will be listed.
						This is the FULL PATH to the mailbox, as understood by the remote server.
@param aFolderList		An array which will be populated with the list of folders upon successful completion of the command.
*/
EXPORT_C void CImapSession::LsubL(TRequestStatus& aStatus, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList)
	{
	__LOG_TEXT(iLogId, "CImapSession::LsubL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ELsubWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ELsubWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapLsub::NewL(iSelectedFolderInfo, iLogId, aReferenceName, aMailboxName, aFolderList, iImapSettings));
	}

/**
Issue the IMAP status command
Populates aFolderInfo with information about the folder identified by aMailboxName
This method SHOULD NOT be used to access information on this session's currently selected mailbox.
@param aStatus Signals completion of the request.
@param aMailboxName The name of the mailbox whose status information is to be fetched.
					This is the FULL PATH to the mailbox, as understood by the remote server.
@param aDataItemNames Specifies what information about the folder to request.
@param aFolderInfo The folder object that represents the mailbox that whose status information is to be fetched.
The folder object will be updated upon successful completion of the command.
Ownership of aFolderInfo will NOT be taken by CImapSession.
*/
EXPORT_C void CImapSession::StatusL(TRequestStatus& aStatus, const TDesC& aMailboxName, const TDesC8& aDataItemNames, CImapFolderInfo& aFolderInfo)
	{
	__LOG_TEXT(iLogId, "CImapSession::StatusL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EStatusWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EStatusWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapStatus::NewL(iSelectedFolderInfo, iLogId, aMailboxName, aDataItemNames, aFolderInfo));
	}

/**
Issue the IMAP append command
@param aStatus Signals completion of the request.
@param aSource An object from which the message source can be obtained
@param aDestinationMailboxName The name of the mailbox to which the message is to be appended
*/
EXPORT_C void CImapSession::AppendL(TRequestStatus& aStatus, CImSendMessage& aSource, const TDesC& aDestinationMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::AppendL()");
	__ASSERT_DEBUG(iServerState >= EServerStateAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendWhenServerUnAuthenticated));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EAppendWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}

	StartCommandL(aStatus, CImapAppend::NewL(iSelectedFolderInfo, iLogId, aSource, aDestinationMailboxName, *this));
	}

/**
Issue the IMAP close command
@param aStatus Signals completion of the request.
*/
EXPORT_C void CImapSession::CloseL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::CloseL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::ECloseWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECloseWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	SetPendingOperation(EOpClose); // Server state becomes Authenticated, iSelectedFolderInfo becomes NULL
	StartCommandL(aStatus, CImapClose::NewL(iSelectedFolderInfo, iLogId));
	}

/**
Issue the IMAP search command
This returns a list of id's of messages that match the specified search critera.
@param aStatus Signals completion of the request.
@param aSearchCriteria Specifies the set of message id's to return.
@param aMatchingMessageIds An array of message id's that will receive the result of the search.
The array will be populated upon successful completion of the command.
*/
EXPORT_C void CImapSession::SearchL(TRequestStatus& aStatus, const TDesC8& aSearchCriteria, RArray<TUint>& aMatchingMessageIds)
	{
	__LOG_TEXT(iLogId, "CImapSession::SearchL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::ESearchWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ESearchInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapSearch::NewL(iSelectedFolderInfo, iLogId, aSearchCriteria, aMatchingMessageIds));
	}

/**
Issues the IMAP fetch command with FLAGS as the only message data item.
The flags for a range of messages may be asked for in a single request.
@param aStatus Signals completion of the request.
@param aSequenceSet Specifies the messages to fetch
@param aOutFlagInfo An array which will be populated with the list of messages and their flags upon successful completion of the command.
*/
EXPORT_C void CImapSession::FetchFlagsL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, RArrayMessageFlagInfo& aOutFlagInfo)
	{
	__LOG_TEXT(iLogId, "CImapSession::FetchFlagsL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchFlagsWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchFlagsInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
		
	StartCommandL(aStatus, CImapFetchFlags::NewL(iSelectedFolderInfo, iLogId, aSequenceSet, aOutFlagInfo));
	}
	
/**
Issues the IMAP fetch command with a BODY (or BODY.PEEK) as the only message data item.
Only one message body may be fetched at a time, although the implementaton may transparently pipeline the fetch.
The caller only provides the <section> information for the BODY.  The rest of the request is built up by the method.
The BODY response data will be stored in the mailstore by the session.
The sessison will use the supplied TMsvId to instruct the mailstore which entry to populate.
@param aStatus Signals completion of the request.
@param aMessageUid Specifies the message to fetch.  This is the IMAP message UID.
@param aPeek Whether the \\Seen flag of the message should be updated by the IMAP server.
@param aFetchBodyInfo container class with extra information about the part being fetched. 
*/	
EXPORT_C void CImapSession::FetchBodyL(TRequestStatus& aStatus, TUint aMessageUid, TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse)
	{
	__LOG_TEXT(iLogId, "CImapSession::FetchBodyL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EFetchBodyWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpWaitForBodyOperationComplete);
	StartCommandL(aStatus, CImapFetchBody::NewL(iSelectedFolderInfo, iLogId, aMessageUid, aPeek, aFetchBodyInfo, aFetchBodyResponse, iImapSettings, iImapMailStore, *this));
	}
	
/**
Issues the IMAP fetch command with the FLAGS, BODYSTRUCTURE and BODY.PEEK[HEADER.FIELDS(<fields>)] data items set.
Only one message may be fetched with this method.  An override exists for a range of messages.
@param aStatus Signals completion of the request.
@param aMessageUid Specifies the message to fetch.  This is the IMAP message UID.
@param aFieldNames A space separated list of header fields that are to be fetched by this method.
@param aOutFetchResponse An object that will be populated with all requested structural information upon successful completion of the command.
*/	
EXPORT_C void CImapSession::FetchBodyStructureAndHeadersL(TRequestStatus& aStatus, TUint aMessageUid, const TDesC8& aFieldNames, CImapFetchResponse& aOutFetchResponse)
	{
	__LOG_TEXT(iLogId, "CImapSession::FetchBodyStructureAndHeadersL()-single");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::FetchBodyStructureSingleWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::FetchBodyStructureSingleWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapFetchSingleBodyStructure::NewL(iSelectedFolderInfo, iLogId, aMessageUid, aFieldNames, aOutFetchResponse));
	}

/**
Issues the IMAP fetch command with the FLAGS, BODYSTRUCTURE and BODY.PEEK[HEADER.FIELDS(<fields>)] data items set.
A range of messages can be fetched with this method.  An override exists for single message fetches.
@param aStatus Signals completion of the request.
@param aMessageUid Specifies the message to fetch.  This is the IMAP message UID.
@param aFieldNames A space separated list of header fields that are to be fetched by this method.
@param aObserver The implementor of this interface will be called each time a fetch response has been received and parsed.
*/	
EXPORT_C void CImapSession::FetchBodyStructureAndHeadersL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC8& aFieldNames, MImapFetchStructureObserver& aObserver)
	{
	__LOG_TEXT(iLogId, "CImapSession::FetchBodyStructureAndHeadersL()-multi");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::FetchBodyStructureMultiWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::FetchBodyStructureMultiWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapFetchMultiBodyStructures::NewL(iSelectedFolderInfo, iLogId, aSequenceSet, aFieldNames, aObserver));
	}

/**
Issues the IMAP store command
aSequenceSet, aMessageDataItem and aValue are the STORE parameters defined in RFC3501 - see section 6.4.6
If aUseSilent is true, then the .SILENT is appended to end of the aMessageDataItem specifier.
@param aStatus Signals completion of the request.
@param aSequenceSet Specifies the range of messages to modify.
@param aMessageDataItem Specifies the name of the message data item to be stored.
@param aValue Provides the new value of the message item.
@param aUseSilent Whether the server should return the updated data value.  
If ETrue, then .SILENT is appended to end of the aMessageDataItem specifier.
*/
EXPORT_C void CImapSession::StoreL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC8& aMessageDataItem, const TDesC8& aValue, TBool aUseSilent, RArrayMessageFlagInfo& aOutMessageFlagInfo)
	{
	__LOG_TEXT(iLogId, "CImapSession::StoreL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::EStoreWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EStoreWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapStore::NewL(iSelectedFolderInfo, iLogId, aSequenceSet, aMessageDataItem, aValue, aUseSilent, aOutMessageFlagInfo));
	}

/**
Issue the IMAP copy command
This instructs the server to copy the messages identified by aSequence from the currently 
selected folder to the folder identified by aDestinationMailbox.
@param aStatus Signals completion of the request.
@param aSequenceSet Specifies the range of messages to copy.
@param aDestinationMailboxName The name of the mailbox to which the message(s) is(are) to be copied.
*/
EXPORT_C void CImapSession::CopyL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC& aDestinationMailboxName)
	{
	__LOG_TEXT(iLogId, "CImapSession::CopyL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapCopy::NewL(iSelectedFolderInfo, iLogId, aSequenceSet, aDestinationMailboxName));
	}

/**
Issues the IMAP expunge command.
@param aStatus Signals completion of the request.
*/
EXPORT_C void CImapSession::ExpungeL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::ExpungeL()");
	__ASSERT_DEBUG(iServerState == EServerStateSelected, TImapServerPanic::ImapPanic(TImapServerPanic::EExpungeWhenNotSelected));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::EExpungeWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	StartCommandL(aStatus, CImapExpunge::NewL(iSelectedFolderInfo, iLogId));
	}

/**
Produces an efficient sequence-set string for the given set of message uids.
For instance, the set (3 4 5 6 21) would produce the string 3:6,21
Methods that take the aSequenceSet parameter can also accept wildcard sequence sets such as "24:*"
which cannot be produced by this method (it would make no sense from finite set of uids).
@param aMessageUids the set of messages to include in the sequence set
@return the sequence set that represents the message uids.  The caller takes ownership of the returned object.
*/
EXPORT_C HBufC8* CImapSession::CreateSequenceSetLC(RArray<TUint>& aMessageUids)
// static method
	{
	_LIT8(KFormatFirstUid, "%d");
	_LIT8(KFormatEndRangeAndNextUid, ":%d,%d");
	_LIT8(KFormatNextUid, ",%d");
	_LIT8(KFormatFinalEndRangeUid, ":%d");
	
	aMessageUids.Sort();
	
	TBool inRange = EFalse;
	TUint prevUid = 0;
	
	TImapOverflowDetector overflowHandler;
	
	RBuf8 sequenceSet;
	sequenceSet.CleanupClosePushL();
	sequenceSet.CreateL(KSequenceSetGranularity);
	
	TInt countUids = aMessageUids.Count();
	for (TInt i=0; i<countUids; i++)
		{
		// First (and lowest) uid always gets written
		if (i==0)
			{
			prevUid = aMessageUids[0];
			AppendAndGrowFormatL(sequenceSet, KFormatFirstUid(), &overflowHandler, prevUid);
			}
		// Avoid duplicate uids
		else if (aMessageUids[i] > prevUid)
			{
			if (inRange && aMessageUids[i] > prevUid + 1 )
				{
				// Next uid is more than 1 greater than the last.  The range has ended.
				AppendAndGrowFormatL(sequenceSet, KFormatEndRangeAndNextUid(), &overflowHandler, prevUid, aMessageUids[i]);
				inRange = EFalse;
				}
			else if (!inRange)
				{
				if (aMessageUids[i] == prevUid + 1)
					{
					// One greater than the last uid means a range has started.  
					// Wait until the end of the range before outputing anything.
					inRange = ETrue;
					}
				else
					{
					// This one is on its own.
					AppendAndGrowFormatL(sequenceSet, KFormatNextUid(), &overflowHandler, aMessageUids[i]);
					}			
				}
			}
			
		prevUid = aMessageUids[i];
		}
	
	// Output the final (range ending) value as it was skipped earlier.
	if (inRange)
		{
		AppendAndGrowFormatL(sequenceSet, KFormatFinalEndRangeUid(), &overflowHandler, prevUid);
		}
	
	// Allocate on the heap.
	HBufC8* sequenceSetOnHeap = sequenceSet.AllocL();
	// Rbuf no longer needed...
	CleanupStack::PopAndDestroy(&sequenceSet);
	
	// But heap version needs to be put onto the cleanup stack.
	CleanupStack::PushL(sequenceSetOnHeap);
	return sequenceSetOnHeap;
	}

/**
@return A pointer to the input stream
@internalComponent
*/
MInputStream* CImapSession::InputStream()
	{
	return iInputStream;
	}

/**
@return A pointer to the output stream
@internalComponent
*/
MOutputStream* CImapSession::OutputStream()
	{
	return iOutputStream;
	}

void CImapSession::StartCommandL(TRequestStatus& aStatus, CImapCommand* aCommand)
	{
	__ASSERT_DEBUG(iCurrentCommand == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNotNullCurrentCommand));
	__ASSERT_DEBUG(iOutputStream != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullOutputStream));
	__ASSERT_DEBUG(iInputStream != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullInputStream));
	__ASSERT_DEBUG(!iInputStream->IsReading(), TImapServerPanic::ImapPanic(TImapServerPanic::ESessionBadInputStreamState));
	__ASSERT_DEBUG(iSessionState == ESessionNormal, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));
	
	iCurrentCommand = aCommand; // Take ownership immediately before there's a chance of leaving...
		
	iCurrentCommand->SendMessageL(++iNextTagId, *iOutputStream);
	
	// Start supplying Imap Server data to the command.
	if (iInputBuffer.Length() > 0)
		{
		// Use data that was previously delivered by the input stream
		// but which has not yet been consumed.
		ProcessInputBufferAsync();
		}
	else
		{
		RequestReadData();
		}
			
	// Queue should always be the last method, so as to avoid leaving!
	Queue(aStatus);
	}

/**
Requests data from the input stream
*/
void CImapSession::RequestReadData()
	{
	// Check that there is no asynchronous call to ProcessInputBuffer outstanding
	__ASSERT_DEBUG(iAsyncCallBackOpCode != EAsyncCallBackOpProcessInputBuffer, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionAsyncCallBackOpInvalid));
	__ASSERT_DEBUG(iInputStream!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullInputStream));
	
	__LOG_TEXT(iLogId, "CImapSession::RequestReadData()");
	
	if (iInputStream != NULL)
		{
		if( iPendingOperation == EOpIdleDone)
			{
			__LOG_TEXT(iLogId, "CImapSession iInputStream->ReadReq() for DONE");
			SetPendingOperation(EOpNone);
			iInputStream->ReadReq(10);
			}
		else 
			{
			__LOG_TEXT(iLogId, "CImapSession iInputStream->ReadReq()");
			iInputStream->ReadReq();
			}
		
		if (iSessionState == ESessionFlushing)
			{
			__LOG_TEXT(iLogId, "CImapSession Start FlushTimeout");
			iFlushCommandTimeout->After(KFlushTimeoutPeriod);
			}
		}
	}

/**
Stops reading server data and completes any Queued operation with KErrCancel.
Call FlushCancelledCommand() to read and discard any data associated with the command
Or call DoneIdle() to complete an idle command.
*/
EXPORT_C void CImapSession::Cancel()
	{
	__LOG_TEXT(iLogId, "CImapSession::Cancel() - START");
	DoCancel();
	__LOG_TEXT(iLogId, "CImapSession::Cancel() - done DoCancel");
	Complete(KErrCancel);
	__LOG_TEXT(iLogId, "CImapSession::Cancel() - END");
	}

/**
Stops reading server data, but does not complete any Queued operation.
This should either be called by Cancel() itself - which will complete the queued operation with KErrCancel
or from methods internal to CImapSession.  
Internal methods that call DoCancel() are reponsible for ensuring that any queued operation will get completed.
 - either by completing with some error code just before or after calling this method.
 - or by initiating a new async step that will complete the queued operation later
*/
void CImapSession::DoCancel()
	{
	__LOG_FORMAT((iLogId, "CImapSession::DoCancel() - START - iSessionState: %d, iPendingOperation: %d, iCurrentCommand: 0x%08x", iSessionState, iPendingOperation, iCurrentCommand ));
	// Stop processing server data
	if (iInputStream != NULL)
		{
		__LOG_TEXT(iLogId, "CImapSession::DoCancel() - iInputStream->CancelReadReq()");
		iInputStream->CancelReadReq();
		__LOG_TEXT(iLogId, "CImapSession::DoCancel() - iInputStream->CancelReadReq() cancelled");
		}
		
	iAsyncCallBack->Cancel();
	iAsyncCallBackOpCode = EAsyncCallBackOpNone;
	__LOG_TEXT(iLogId, "CImapSession::DoCancel() - iAsyncCallBackOpCode = EAsyncCallBackOpNone");
	
	iFlushCommandTimeout->Cancel();
	__LOG_TEXT(iLogId, "CImapSession::DoCancel() - iFlushCommandTimeout cancelled");
	
	if (iSessionState == ESessionNormal && iCurrentCommand != NULL)
		{
		__LOG_TEXT(iLogId, "CImapSession::DoCancel() - cancelling the current command");
		
		iCurrentCommand->Cancel();
		DoPendingOperationForCancel();
		
		// Most commands need to flush after cancel, but commands such as Idle may have different behaviours.
		if (iCurrentCommand->Flushing())
			{
			SetSessionState(ESessionFlushing);
			}
		}
	__LOG_FORMAT((iLogId, "CImapSession::DoCancel() - END - iSessionState: %d, iCurrentCommand: 0x%08x", iSessionState, iCurrentCommand ));
	}

/**
This method reads and discards incoming data from the IMAP server until 
no more data is expected.
It can be called after calling Cancel() on any asynchronous method other 
than EnterIdleL() or WaitForIdleEvent().
It usually will complete when all the data for the cancelled command has been received.
The method will also complete after a 10 second timeout - indicating that the 
expected data has not been received from the server.
@param aStatus Signals completion of the request or a timeout.
*/
EXPORT_C void CImapSession::FlushCancelledCommand(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iLogId, "CImapSession::FlushCancelledCommand()");
	
	// Complete immediately if the streams are closed.
	// The caller receives the KErrImapClosed status code.
	if (CompleteIfStreamsClosed(aStatus))
		{
		return;
		}
	
	Queue(aStatus);
		
	// Complete immediately if there is no command to flush.
	// The caller will receive the KErrNone status code - so will know that the streams are open.
	// This makes it possible to call Cancel() followed by FlushCancelledCommand() when there is no command operating.
	if (iCurrentCommand == NULL)
		{
		__ASSERT_DEBUG(iSessionState == ESessionNormal, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));
		__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidPendingOp));
		
		__LOG_TEXT(iLogId, "CImapSession No command to flush");
		Complete(KErrNone);
		return;
		}
		
	// If we got this far, then a command is in progress.
	// The following ASSERTS ensure that it is approriate to be Flushing the command at this time.
	__ASSERT_DEBUG(iCurrentCommand->Flushing(), TImapServerPanic::ImapPanic(TImapServerPanic::ESessionCommandNotFlushing));
	__ASSERT_DEBUG(iSessionState == ESessionFlushing, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));

	// Complete immediately if the command can be closed early
	if (iCurrentCommand->CanCompleteFlushNow())
		{
		CompleteAndDestroyCommand(KErrNone, EFalse);
		SetSessionState(ESessionNormal);
		return;
		}
	
	// Continue supplying Imap Server data to the command.
	if (iInputBuffer.Length() > 0)
		{
		// Use data that was previously delivered by the input stream
		// but which has not yet been consumed.
		ProcessInputBufferAsync();
		}
	else
		{
		RequestReadData();
		}
	}

/**
This is called when the FlushCancelledCommand() operation times out.
It cancels the operation and completes with the code KErrImapFlushTimeout.
@param aSourceTimer a referecne to the iFlushCommandTimeout object.
*/
#ifdef _DEBUG
void CImapSession::OnTimerL(const CImapObservableTimer& aSourceTimer)
#else // _DEBUG
void CImapSession::OnTimerL(const CImapObservableTimer& /*aSourceTimer*/)
#endif // _DEBUG
	{
	__LOG_TEXT(iLogId, "CImapSession::OnTimerL()");
	__ASSERT_DEBUG(&aSourceTimer == iFlushCommandTimeout, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionUnknownTimer));
	
	__ASSERT_DEBUG(iCurrentCommand->Flushing(), TImapServerPanic::ImapPanic(TImapServerPanic::ESessionCommandNotFlushing));
	__ASSERT_DEBUG(iSessionState == ESessionFlushing, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));
	
	DoCancel();	
	Complete(KErrImapFlushTimeout);
	}

/**
Commands (such as CImapAppend) that send their data asynchronously
should call this method if the send fails (e.g. due to lack of resources)
*/
void CImapSession::AsyncSendFailed(TInt aErr)
	{
	__LOG_FORMAT((iLogId, "CImapSession::AsyncSendFailed(%d)", aErr));
	// Report the error.  
	// Assert that there is a TRequestStatus to report to.
	// This should always be the case, a command's send/receive is always paired with a call to Queue.
	__ASSERT_DEBUG(iReport != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullReportStatus));
	CompleteAndDestroyCommand(aErr, ETrue);
	
	SetSessionState(ESessionUnrecoverable);
	}

/**
Call this last when an asynch operation has been requested.
@param aStatus	The request status of an active object that will be notified 
				when the async operation has completed.
*/
void CImapSession::Queue(TRequestStatus& aStatus)
    {
    if(iReport !=NULL)
    	return;
	__ASSERT_DEBUG(iReport==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionReportStatusAlreadyQueued));
    aStatus=KRequestPending;
    iReport=&aStatus;
    }

/**
Call this last when an asynch operation has been completed.
@param aStatus the return error/status code for the completed operation.
*/
void CImapSession::Complete(TInt aStatus)
	{
	__LOG_FORMAT((iLogId, "CImapSession::Complete() - aStatus: %d, iReport: 0x%08x", aStatus, iReport));
	if (iReport)
	    {
	    // only complete properly once.
	    // this allows a derived class to complete and then cancel, reporting the desired state instead of KErrCancel
	    DoComplete(aStatus);
	    User::RequestComplete(iReport,aStatus);
		}
	}

/**
Implements CMsgActive::DoComplete()
Does nothing except logging the call.
*/
#ifdef __IMAP_LOGGING
void CImapSession::DoComplete(TInt& aStatus)
#else //__IMAP_LOGGING
void CImapSession::DoComplete(TInt& /*aStatus*/)
#endif //__IMAP_LOGGING
	{
	__LOG_FORMAT((iLogId, "CImapSession::DoComplete() - completing observers status with error: %d", aStatus));
	}

TInt CImapSession::AsyncCallBack(TAny* aSelf)
// static method
	{
	CImapSession* self = static_cast<CImapSession*>(aSelf);
	self->AsyncCallBack();
	
	return EFalse; // Tell the OS not to call this method again without being requested.
	}

void CImapSession::AsyncCallBack()
	{
	TRAPD(err, DoAsyncCallBackL());
	if (err != KErrNone)
		{
		__LOG_FORMAT((iLogId, "CImapSession Leave in DoAsyncCallBackL(): %d", err));
		// Report the error.  
		// Assert that there is a TRequestStatus to report to.
		// This should always be the case, as async callbacks requests are paired with calls to Queue.
		__ASSERT_DEBUG(iReport != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullReportStatus));
		CompleteAndDestroyCommand(err, ETrue);
		
		SetSessionState(ESessionUnrecoverable);
		}
	}
	
void CImapSession::DoAsyncCallBackL()
	{
	__LOG_TEXT(iLogId, "CImapSession::DoAsyncCallBackL()");
	
	// Allow methods called below to check that no async operation is queued,
	// and even to queue one up, by caching the current op code and nulling it.
	TAsyncCallBackOpCode cachedOpCode = iAsyncCallBackOpCode;
	iAsyncCallBackOpCode = EAsyncCallBackOpNone;
	
	__LOG_TEXT(iLogId, "CImapSession::iAsyncCallBackOpCode = EAsyncCallBackOpNone");
	
	switch (cachedOpCode)
		{
		case EAsyncCallBackOpProcessInputBuffer:
			{
			__LOG_TEXT(iLogId, "CImapSession cachedOpCode = EAsyncCallBackOpNone");
			ProcessInputBufferL();
			}	
			break;
		case EAsyncCallBackOpNone:
		default:
			{
			// No valid op code was specified.
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionAsyncCallBackOpInvalid));
			break;
			}			
		}
	}

/**
Calls ProcessInputBufferL() at a later stage.
*/
void CImapSession::ProcessInputBufferAsync()
	{
	__LOG_TEXT(iLogId, "CImapSession::ProcessInputBufferAsync()");
	
	// if we have an input stream, assert that it is not reading.
	__ASSERT_DEBUG((iInputStream == NULL) ? !iInputStream->IsReading() : ETrue, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionBadInputStreamState));
	__ASSERT_DEBUG(iAsyncCallBackOpCode == EAsyncCallBackOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionAsyncCallBackOpInvalid));
		
	iAsyncCallBack->CallBack();	
	iAsyncCallBackOpCode = EAsyncCallBackOpProcessInputBuffer;
	__LOG_TEXT(iLogId, "CImapSession::iAsyncCallBackOpCode = EAsyncCallBackOpProcessInputBuffer");
	}

/**
Notifies the observer that data has been received from the 
connected host.The supplied buffer will remain valid until 
the input stream is notified that it is no longer needed.

@param	aBuffer	A buffer containing the received data.
@see MInputStreamObserver
*/
void CImapSession::ReceivedDataIndL(const TDesC8& aBuffer)
// Implements MInputStreamObserver
	{	
	__LOG_TEXT(iLogId, "CImapSession::ReceivedDataIndL()");
	
	// Should not be requesting an async callback and data from the transport handler at the same time.
	__ASSERT_DEBUG(iAsyncCallBackOpCode != EAsyncCallBackOpProcessInputBuffer, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionAsyncCallBackOpInvalid));

	// Should not have any data remaining in the input buffer.
	// If we have then the stream was restarted too soon.
	__ASSERT_DEBUG(iInputBuffer.Length()==0, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInputBufferNotEmpty));

	iInputBuffer.Set(aBuffer);
	
	iFlushCommandTimeout->Cancel();
	
	ProcessInputBufferL();
	}
	
/**
Processes data in iInputBuffer
*/
void CImapSession::ProcessInputBufferL()
	{
	// Should have a command ready to process the input buffer.
	__ASSERT_DEBUG(iCurrentCommand != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullCurrentCommand));
	__ASSERT_DEBUG(iSessionState == ESessionNormal || iSessionState == ESessionFlushing, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));

	TPtrC8 dataToDeliver(KNullDesC8());
	
	TBool continueReadingData = ETrue;	
	TBool commandComplete = EFalse;
	TBool deliveredSomeData = EFalse;
	CImapCommand::TParseState commandParseState = CImapCommand::EWaitStartResponse;
				
	while(iInputBuffer.Length() > 0 && continueReadingData)
		{
		// The following TryFindXxx methods will remove data from iInputBuffer,
		//   returning it to dataToDeliver
		//   and/or copying it the iInputCache
			
		TBool foundBlock = EFalse;	
		if (iCommandLiteralRequestSize == 0)
			{
			foundBlock = TryFindLineL(dataToDeliver);
			}
		else
			{
			foundBlock = TryFindLiteralBlockL(dataToDeliver);
			}
			
		if (foundBlock)
			{
			// Reset expected command literal size, now that we are delivering a whole block.
			iCommandLiteralRequestSize = 0;
			
			// Deliver the data to the command
			TRAPD(err, 
				commandParseState = iCurrentCommand->ParseBlockL(dataToDeliver);
				);
				
			// Record that at least one block of data was delivered
			deliveredSomeData = ETrue;
				
			// Cleanup the cache
			iInputCache->Reset();
			dataToDeliver.Set(KNullDesC8()); // defensive: dataToDeliver might be invalid if it was pointing at the cache
				
			if (err != KErrNone)
				{
				// As well as completing, destroy the command to help free up resources.
				CompleteAndDestroyCommand(err, ETrue);

				// Don't allow the session to be called again.
				SetSessionState(ESessionUnrecoverable);
				
				return;
				}
				
			// If a literal block is expected next, then find out how big it is expected to be.
			switch (commandParseState)
				{
				case CImapCommand::EWaitStartResponse:
					{
					// This state means that a response been completely parsed,
					// but another response is expected.for this command.
					// Usually this means the last response was untagged * or +,
					// but it can also happen during multi-fetch where more than one tagged OK is expected.

					continueReadingData = DoPendingOperationForIntermediateResponse();
					}
					break;
				case CImapCommand::EWaitLiteralParse:
				case CImapCommand::EWaitLiteralIngore:
					{
					iCommandLiteralRequestSize = iCurrentCommand->LiteralBytesExpected();
					__ASSERT_DEBUG(iCommandLiteralRequestSize > 0, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionLiteralSizeZero));
					}
					break;
				case CImapCommand::ECommandComplete:
					{
					commandComplete = ETrue;
					continueReadingData = EFalse;
					}
					break;
				default:
					break;
				}				
			}
		else // block not found
			{
			// All the data should have been added to the cache
			__ASSERT_DEBUG(iInputBuffer.Length() == 0, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInputBufferNotEmpty));
			}
			
		} // end of while loop
		
	// The current block of data has been parsed.  Do we need any more, or are we finished?
	if (commandComplete)
		{
		CommandComplete();
		}
	else if (continueReadingData)
		{
		RequestReadData();
		
		if (deliveredSomeData)
			{
			// Inform the command that no more data is available from the input buffer.
			// This allows the command to commit any bulk operations while waiting for the next set of data.
			__ASSERT_DEBUG(iCurrentCommand != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullCurrentCommand));
			iCurrentCommand->WaitingForMoreDataL();
			}
		}
	}

void CImapSession::CommandComplete()
	{
	switch (iCurrentCommand->ResponseCode())
		{
		case CImapCommand::EImapResponseOk:
			{
			if (iSessionState == ESessionNormal)
				{
				// Complete unless deferred by pending operation.
				if (DoPendingOperationForOk())
					{
					// Command has completed successfully
					CompleteAndDestroyCommand(KErrNone, EFalse);
					}
				}
			else
				{
				// We're flushing the command, so always complete straight away.
				__ASSERT_DEBUG(iSessionState == ESessionFlushing, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));
				CompleteAndDestroyCommand(KErrNone, EFalse);
				SetSessionState(ESessionNormal);
				}				
						
			// NOTE: No need to request more data here.  The next StartCommandL will do that for us.
			}
			break;
		case CImapCommand::EImapResponseNo:
			{
			DoPendingOperationForFail();			
			CompleteAndDestroyCommand(KErrImapNo, EFalse);
			SetSessionState(ESessionNormal);
			}
			break;
		case CImapCommand::EImapResponseBad:
			{
			DoPendingOperationForFail();			
			CompleteAndDestroyCommand(KErrImapBad, EFalse);
			SetSessionState(ESessionNormal);
			}
			break;
		default:
			{
			// No other result codes should be returned.
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidCommandResponseCode));
			}
			break;
		}
	}

/**
Destroys the current command object and completes any outstanding asynchronous request with the supplied completion code
The input and output streams are optionally closed too - this should be done when we are completing because an unexpected error has just occured.
@param aCompletionCode the return error/status code for the completed operation
@param aCloseStreams whether to close the input and output streams
*/
void CImapSession::CompleteAndDestroyCommand(TInt aCompletionCode, TBool aCloseStreams)
	{
	__LOG_FORMAT((iLogId, "CImapSession::CompleteAndDestroyCommand() - START - iSessionState: %d, iPendingOperation: %d, iCurrentCommand: 0x%08x, aCompletionCode: %d, aCloseStreams: %d", iSessionState, iPendingOperation, iCurrentCommand, aCompletionCode, aCloseStreams ));
	// Complete must happen BEFORE closing the stream so that the correct error code is returned to the async caller.
	// If we allowed iOutputStream->Close() to be called first, then KErrImapClosed would be returned - which is not what we want.
	Complete(aCompletionCode);
	
	//	Pipelining commands can increment the tag-id internally, so we need to find the 
	// latest value for the next command.And before closing the command object, fetch the current command tag Id.
	if( iCurrentCommand != NULL )	
		{
		iNextTagId = iCurrentCommand->GetTagId();
		}

	// iOutputStream->Close() must be called BEFORE iCurrentCommand is destroyed.
	// This is because iCurrentCommand might have initiated an async write operation on the stream 
	// which may not have completed yet, in which case the stream will still be referencing output buffer
	// the descriptor owned by the iCurrentCommand.
	// Calling Close() will cancel the async write, making it safe to destroy the command.
	// This helps avoid an ESOCK 14 panic (bad descriptor)
	if (aCloseStreams && iOutputStream != NULL)
		{
		iOutputStream->Close();
		}
		
	// Finished with this command object...
	delete iCurrentCommand;
	iCurrentCommand = NULL;
	
	SetPendingOperation(EOpNone);
	__LOG_FORMAT((iLogId, "CImapSession::CompleteAndDestroyCommand() - END - iSessionState: %d, iPendingOperation: %d, iCurrentCommand: 0x%08x, aCompletionCode: %d, aCloseStreams: %d", iSessionState, iPendingOperation, iCurrentCommand, aCompletionCode, aCloseStreams ));
	}

/**
Perform any extra operations upon reciept of the command's tagged OK response
@return Whether it is OK to CompleteAndDestroyCommand() straight away.  
		Some commands may continue to run asynchronusly after receiving the tagged OK response
		CompleteAndDestroyCommand() after receving further notification that the command has completed.
*/
TBool CImapSession::DoPendingOperationForOk()
	{
	TBool completeNow = ETrue;
	
	switch (iPendingOperation)
		{
		case EOpServerGreeting:
			{
			if (iServerGreetingInfo->ResponseTag() == CImapServerGreetingInfo::ETagOk)
				{
				SetServerState(EServerStateNotAuthenticated);
				}
			else if (iServerGreetingInfo->ResponseTag() == CImapServerGreetingInfo::ETagPreAuth)
				{
				SetServerState(EServerStateAuthenticated);
				}					
			}
			break;
			
		case EOpSelect:
			{
			SetServerState(EServerStateSelected);
			}
			break;
		case EOpClose:
			{
			SetServerState(EServerStateAuthenticated);
			
			delete iSelectedFolderInfo;
			iSelectedFolderInfo = NULL;
			}
			break;
		case EOpLogin:
			{
			SetServerState(EServerStateAuthenticated);
			
			__ASSERT_DEBUG(iSelectedFolderInfo == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullSelectedFolderInfo));
			}
			break;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		case EOpAuthenticate:
			{
			SetServerState(EServerStateAuthenticated);
			
			__ASSERT_DEBUG(iSelectedFolderInfo == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionNullSelectedFolderInfo));
			}
			break;
#endif
		case EOpLogout:
			{
			SetServerState(EServerStateNone);
			
			delete iSelectedFolderInfo;
			iSelectedFolderInfo = NULL;
			}
			break;
		case EOpWaitForBodyOperationComplete:
			{
			// Wait for FetchBodyOperationComplete() to complete.
			CImapFetchBody* fetchBodyCommand = static_cast<CImapFetchBody*>(iCurrentCommand);
			completeNow = fetchBodyCommand->IsStoreOperationComplete();
			}			
			break;
		default:
			break;
		}
		
	return completeNow;
	}
	
/**
Called when the IMAP server returns with a tagged NO or tagged BAD response.
*/
void CImapSession::DoPendingOperationForFail()
	{
	switch (iPendingOperation)
		{
		case EOpSelect:
			{
			SetServerState(EServerStateAuthenticated);
			
			delete iSelectedFolderInfo;
			iSelectedFolderInfo = NULL;
			}
			break;
		default:
			break;
		}
	}

/**
Called when cancelling a command.
If an operation can change the server state, assume that the server is in the lowest
state for that operation.
*/
void CImapSession::DoPendingOperationForCancel()
	{
	switch (iPendingOperation)
		{
		case EOpSelect:
		case EOpClose:
			{
			SetServerState(EServerStateAuthenticated);
			
			delete iSelectedFolderInfo;
			iSelectedFolderInfo = NULL;
			}
			break;
		case EOpLogout:
			{
			SetServerState(EServerStateNone);
			
			delete iSelectedFolderInfo;
			iSelectedFolderInfo = NULL;
			}
			break;
		default:
			break;
		}
	}

/**
This is called after any resopnse other than the final response has been parsed.
@return whether to continue reading data from the output stream
*/
TBool CImapSession::DoPendingOperationForIntermediateResponse()
	{
	TBool continueParsing = ETrue;
	
	switch (iPendingOperation)
		{
		case EOpIdleEnter:
		case EOpIdleWait:
			{
			CImapIdle* idleCommand = static_cast<CImapIdle*>(iCurrentCommand);
			if (idleCommand->IdleState() == CImapIdle::EIdleWaitResponse)
				{
				// EOpIdleEnter: The continuation response has been received.
				// EOpIdleWait:  The next response has been received.
				//	
				// Notify the caller and exit the parse loop.
				// We expect the caller to call either WaitForIdleEvent() or DoneIdle()
				
				Complete(KErrNone);
				continueParsing = EFalse;
				}
			}
			break;
		default:
			break;
		}
	
	return continueParsing;
	}
	
/**
Takes data from iInputBuffer in order to build a literal block.
If not enough data is available, then the whole of iInputBuffer is added to the cache.
iInputBuffer will be updated so that it only points at unconsumed data.
@param aDataToDeliver Output parameter that receives the literal block.
@return 			  If enough data is available, then this parameter is set to point to the whole literal block.
					  Otherwise it points at KNullDesC8.
@return Whether enough data was available to return a whole literal block in the output parameter.
*/
TBool CImapSession::TryFindLiteralBlockL(TPtrC8& aDataToDeliver)
	{
	TInt inputCacheLength = iInputCache->Size();
	TBool foundLiteralBlock = EFalse;
	aDataToDeliver.Set(KNullDesC8());
	
	// Is there enough data to send a complete block?
	if (iInputBuffer.Length() + inputCacheLength >= iCommandLiteralRequestSize)
		{
		foundLiteralBlock = ETrue;
		
		// Either send it in directly or use the cache
		TInt lengthToConsume = 0;
		if (inputCacheLength > 0)
			{			
			// Some of the block is in the cache.  So append the rest of the block to the cache and return the cache.
			lengthToConsume = iCommandLiteralRequestSize - inputCacheLength;
			iInputCache->InsertL(iInputCache->Size(), iInputBuffer.Left(lengthToConsume));
			
			aDataToDeliver.Set(iInputCache->Ptr(0));
			}
		else
			{
			// The whole of the block is in iInput buffer.  So just return that.
			lengthToConsume = iCommandLiteralRequestSize;
			aDataToDeliver.Set(iInputBuffer.Left(iCommandLiteralRequestSize));
			}		
			
		// Consume the input buffer
		if (iInputBuffer.Length() > lengthToConsume)
			{
			iInputBuffer.Set(iInputBuffer.Mid(lengthToConsume));
			}
		else
			{
			iInputBuffer.Set(KNullDesC8());
			}
		}
	else
		{
		// Not enough data is available yet.  So cache all of it and wait for more
		iInputCache->InsertL(iInputCache->Size(), iInputBuffer);
		iInputBuffer.Set(KNullDesC8());
		}
	
	return foundLiteralBlock;
	}
	
/**
Takes data from iInputBuffer in order to build a line.
If not enough data is available, then whole of iInputBuffer is added to the cache.
iInputBuffer will be updated so that it only points at unconsumed data.
@param aDataToDeliver Output parameter that receives the line of data.
	   				  If enough data is available, then this parameter is set to point to the whole line (without the CRLF).
					  Otherwise it points at KNullDesC8.
@return Whether enough data was available to return a whole line in the output parameter.
*/
TBool CImapSession::TryFindLineL(TPtrC8& aDataToDeliver)
	{
	TInt inputCacheLength = iInputCache->Size();
	TBool foundLine = EFalse;
	aDataToDeliver.Set(KNullDesC8());
	

	// Need to check for a Special Case first: 
	//   End of cache has the Cr, start of iInputBuffer has the Lf
	//
	// Check for Lf first
	if (iInputBuffer[0] == '\n')
		{
		// Check for Cr
		if (inputCacheLength > 0 && iInputCache->Ptr(inputCacheLength-1)[0] == '\r')
			{
			foundLine = ETrue;
			
			// Point at the line in the cache (but not the Cr)
			aDataToDeliver.Set(iInputCache->Ptr(0).Left(inputCacheLength-1));
			
			// Consume the Lf
			if (iInputBuffer.Length() > 1)
				{
				iInputBuffer.Set(iInputBuffer.Mid(1));
				}
			else
				{
				iInputBuffer.Set(KNullDesC8());
				}
			}
		}
			
	// Usually, the Special Case will fail, so aDataToDeliver will still be empty at this point.
	if (aDataToDeliver.Size() == 0)
		{
		// Try to find a whole CRLF
		
		// In some cases server sends the FETCH header data in the following manner(Eg: Lukku server)
		// *43 FETCH (FLAGS (\Seen $LuukkuClean $NotJunk JunkRecorded) UID 6285 BODYSTRUCTURE (("TEXT" "PLAIN" ("CHARSET" "ISO-8859-1") NIL NIL "7BIT" 4 0 NIL NIL NIL)("APPLICATION" "PDF" ("NAME" {25}
		// Kopio_paperikirjeestä.pdf) NIL NIL "BASE64" 187346 NIL ("ATTACHMENT" ("FILENAME" {25} 
		// Kopio_paperikirjeestä.pdf)) NIL) "MIXED" ("BOUNDARY" "----=_Part_7226_1401000605.1221158570821") NIL NIL) BODY[HEADER.FIELDS (Received Date Subject From Priority X-MSMail-Priority X-Priority Importance)] {977}
		
		TInt posCrlf =0;
		posCrlf = iInputBuffer.Find(KImapTxtCrlf());
		TInt pos1= iInputBuffer.LocateReverse('\n');
		TInt len=iInputBuffer.Length();
		
		if(pos1>0)  // => posCrlf>0 
			{	
			if((iInputBuffer[pos1-1] == '\r') )
				{
				 if(posCrlf != pos1-1)  // Checking if there are more than 1 CRLF(\r\n) in InputBuffer
				 	{
				 	 if((posCrlf> 0) && (iInputBuffer[posCrlf-1] == '}') )
				 	 	{
			 	 		_LIT8(KImapTxt1, "]");
			 	 		TInt pos2= iInputBuffer.Find(KImapTxt1());
						// Checking for out of boundary condition
			 	 		if(((pos2+2) <= (len-1)) && (iInputBuffer[pos2+2] == '{'))
				 	 		{
				 	 		TInt pos3=pos2+2;
				 	 		
				 	 		// Checking for out of boundary condition
				 	 		while(((pos3+1) <= (len-1)) && (iInputBuffer[pos3] != '}'))
				 	 			{
				 	 			pos3++;
			 	 				}
			 	 				
							if((pos3+2) <= (len-1)) // Checking for out of boundary condition
								{
								if((iInputBuffer[pos3+1] == '\r') && (iInputBuffer[pos3+2] == '\n'))
									{
									if((pos2>0) && (iInputBuffer[pos2-1] == ')'))
										{
										posCrlf = pos3+1;
										}
									}
								}
			 		 		}
				 	 	}
				 	}
				}
			}

		if (posCrlf == KErrNotFound)
			{
			// Append all the data to the cache
			iInputCache->InsertL(iInputCache->Size(), iInputBuffer);
			iInputBuffer.Set(KNullDesC8());
			}
		else if(posCrlf >= 0)
			{
			foundLine = ETrue;
			
			// Fetch the line and consume it from iInputBuffer
			aDataToDeliver.Set(iInputBuffer.Left(posCrlf));
			if (iInputBuffer.Length() > posCrlf + KImapTxtCrlf.iTypeLength)
				{
				iInputBuffer.Set(iInputBuffer.Mid(posCrlf + KImapTxtCrlf.iTypeLength));
				}
			else
				{
				iInputBuffer.Set(KNullDesC8());
				}
			
			// Prepend any cached data
			if (inputCacheLength > 0)
				{
				iInputCache->InsertL(iInputCache->Size(), aDataToDeliver);
				aDataToDeliver.Set(iInputCache->Ptr(0));
				}
			}
		else
			{
			// Unexpected error from Find()
			User::Leave(posCrlf);
			}
		}
		
	return foundLine;	
	}

/**
This method is called by the transport handler upon completion of a request to 
upgrade to a secure connection, using MInputStream::SecureClientReq().
But as CImapSession never makes such a request, the method should never be called here.
*/
void CImapSession::SecureServerCnf()
// Implements MInputStreamObserver
	{
	// CImapSessionManager uses MInputStream::SecureClientReq() and should have its own version of this method called.
	__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionReceivedSecureServerCnf));
	}
	
void CImapSession::InputStreamCloseInd(TInt /*aError*/)
// Implements MInputStreamObserver
	{
	__LOG_TEXT(iLogId, "CImapSession::InputStreamCloseInd() - START");
	// Input stream has been closed. Transport handler is about to delete it
	// and the output stream too.
	iInputStream = NULL;
	iOutputStream = NULL;

	// DoCancel() must be called after CompleteAndDestroyCommand(), to prevent attempt 
	// by DoCancel() to call Cancel on the command.
	CompleteAndDestroyCommand(KErrImapClosed, EFalse);
	DoCancel(); 
	__LOG_TEXT(iLogId, "CImapSession::InputStreamCloseInd() - END");
	}

void CImapSession::SendDataCnf()
// Implements MOutputStreamObserver
	{
	if (iCurrentCommand != NULL)
		{
		TRAPD(err, 
			iCurrentCommand->SendDataCnfL();
			);
			
		if (err != KErrNone)
			{
			CompleteAndDestroyCommand(err, ETrue);
			DoCancel();
			
			SetSessionState(ESessionUnrecoverable);
			}		
		}
	}
void CImapSession::OutputStreamCloseInd(TInt /*aError*/)
// Implements MOutputStreamObserver
	{
	__LOG_TEXT(iLogId, "CImapSession::OutputStreamCloseInd() - START");
	// Output stream has been closed. Transport handler is about to delete it
	// and the input stream too.
	iInputStream = NULL;
	iOutputStream = NULL;

	// DoCancel() must be called after CompleteAndDestroyCommand(), to prevent attempt 
	// by DoCancel() to call Cancel on the command.
	CompleteAndDestroyCommand(KErrImapClosed, EFalse);
	DoCancel();
    __LOG_TEXT(iLogId, "CImapSession::OutputStreamCloseInd() - END");
	}
	
/**
Checks whether the input and output streams are closed.
If they are closed, then aStatus is completed immediately with the completion code KErrImapClosed.
@param aStatus The request status that will may be completed immediately if the streams are closed.
@return ETrue if aStatus was completed immediately, otherwise EFalse.
*/
TBool CImapSession::CompleteIfStreamsClosed(TRequestStatus& aStatus)
	{
	TBool completed = EFalse;
	
	if (iInputStream == NULL || iOutputStream == NULL)
		{
		// If one stream is NULL then so should the other
		__ASSERT_DEBUG(iInputStream==NULL && iOutputStream==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionStreamsNotBothNull));
		
		Queue(aStatus);
		Complete(KErrImapClosed);
		
		completed = ETrue;
		}
		
	return completed;
	}

/**
Appends a formatted string to the supplied buffer.  If the buffer is too small to append 
the string, then the buffer is expanded and another attempt is made to perform the append operation.
@param aBuffer 			The buffer that will have formatted string appended to it.
@param aFormat 			Specifies the format of the string to be appended.
@param aOverflowHandler An object that helps detect when the buffer needs to be expanded.  
						Despite this object being a pointer, it may not be NULL, and no 
						transference of ownership occurs.
*/	
void CImapSession::AppendAndGrowFormatL(RBuf8& aBuffer, TRefByValue<const TDesC8> aFormat, TImapOverflowDetector* aOverflowHandler, ...)
// static 
	{
	__ASSERT_DEBUG(aOverflowHandler != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidOverFlowHandler));
	
	VA_LIST list;
	VA_START(list, aOverflowHandler); // Note that VA_START() does not accept reference parameters.
	
	TInt bufferLength = aBuffer.Length();
	
	while (ETrue)
		{
		aBuffer.AppendFormatList(aFormat, list, aOverflowHandler);
		
		if (aOverflowHandler->OverflowDetected())
			{
			// Invalidate any data that was added in the previous attempt to append, by reseting its length.
			aBuffer.SetLength(bufferLength);
		
			// Make some more space for the next attempt.	
		 	TInt newSize = aBuffer.MaxLength() + KSequenceSetGranularity;
			User::LeaveIfError(aBuffer.ReAlloc(newSize));
			}
		else
			{
			break;
			}
		}
	}
	
/**
Called by the CImapFetchBody command when a body part has been stored.
@param aErrorCode the result of the store operation.
*/
void CImapSession::FetchBodyOperationComplete(TInt aErrorCode)
	{
	__ASSERT_DEBUG(iPendingOperation == EOpWaitForBodyOperationComplete, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidPendingOp));
	__ASSERT_DEBUG(iSessionState == ESessionNormal, TImapServerPanic::ImapPanic(TImapServerPanic::ESessionInvalidSessionState));

	// Check for unrecoverable errors
	if (aErrorCode < 0 || aErrorCode == KErrImapCorrupt)
		{
		// Finished with this command object...	
		CompleteAndDestroyCommand(aErrorCode, ETrue);
		
		// This unexpected error cannot be recovered from as we no longer know our state
		SetSessionState(ESessionUnrecoverable);
		}
	else
		{
		// Finished with this command object...	
		CompleteAndDestroyCommand(aErrorCode, EFalse);
		}	
	}

/**
Sets the session state, and logs the transision.
All assignments to iSessionState should occur through this method.
*/
void CImapSession::SetSessionState(TSessionState aSessionState)
	{
#ifdef __IMAP_LOGGING

	_LIT8(KTxtSessionNormal, "ESessionNormal");
	_LIT8(KTxtSessionFlushing, "ESessionFlushing");
	_LIT8(KTxtSessionUnrecoverable, "ESessionUnrecoverable");
	_LIT8(KTxtSessionStateUnknown, "Unknown");
	
	TPtrC8 ptrOldState(KTxtSessionStateUnknown);
	TPtrC8 ptrNewState(KTxtSessionStateUnknown);
	
	switch(iSessionState)
		{
		case ESessionNormal: 		ptrOldState.Set(KTxtSessionNormal); 		break;
		case ESessionFlushing: 		ptrOldState.Set(KTxtSessionFlushing); 		break;
		case ESessionUnrecoverable:	ptrOldState.Set(KTxtSessionUnrecoverable);	break;
		}
	switch(aSessionState)
		{
		case ESessionNormal: 		ptrNewState.Set(KTxtSessionNormal); 		break;
		case ESessionFlushing: 		ptrNewState.Set(KTxtSessionFlushing); 		break;
		case ESessionUnrecoverable:	ptrNewState.Set(KTxtSessionUnrecoverable);	break;
		}
	
	_LIT8(KLogFormat, "CImapSession::iSessionState %S ==> %S");
	__LOG_FORMAT((iLogId, KLogFormat, &ptrOldState, &ptrNewState));
	
#endif //__IMAP_LOGGING
	
	iSessionState = aSessionState;	
	}

/**
Sets the current pending operation, and logs the transision.
All assignments to iPendingOperation should occur through this method.
*/
void CImapSession::SetPendingOperation(TPendingOperation aPendingOperation)
	{
#ifdef __IMAP_LOGGING

	_LIT8(KTxtOpNone, "EOpNone");
	_LIT8(KTxtOpServerGreeting, "EOpServerGreeting");
	_LIT8(KTxtOpSelect, "EOpSelect");
	_LIT8(KTxtOpLogin, "EOpLogin");
	_LIT8(KTxtOpLogout, "EOpLogout");
	_LIT8(KTxtOpClose, "EOpClose");
	_LIT8(KTxtOpWaitForBodyOperationComplete, "EOpWaitForBodyOperationComplete");
	_LIT8(KTxtOpIdleEnter, "EOpIdleEnter");
	_LIT8(KTxtOpIdleWait, "EOpIdleWait");
	_LIT8(KTxtOpUnknown, "Unknown");
	_LIT8(KTxtOpIdleDone, "EOpIdleDone");
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	_LIT8(KTxtOpAuthenticate, "EOpAuthenticate");
#endif
	TPtrC8 ptrOldOperation(KTxtOpUnknown);
	TPtrC8 ptrNewOperation(KTxtOpUnknown);
	
	switch(iPendingOperation)
		{
		case EOpNone:				ptrOldOperation.Set(KTxtOpNone);			break;
		case EOpServerGreeting:		ptrOldOperation.Set(KTxtOpServerGreeting);	break;
		case EOpSelect:				ptrOldOperation.Set(KTxtOpSelect);			break;
		case EOpLogin:				ptrOldOperation.Set(KTxtOpLogin);			break;
		case EOpLogout:				ptrOldOperation.Set(KTxtOpLogout);			break;
		case EOpClose:				ptrOldOperation.Set(KTxtOpClose);			break;
		case EOpWaitForBodyOperationComplete:
									ptrOldOperation.Set(KTxtOpWaitForBodyOperationComplete);	break;
		case EOpIdleEnter:			ptrOldOperation.Set(KTxtOpIdleEnter);		break;
		case EOpIdleWait:			ptrOldOperation.Set(KTxtOpIdleWait);		break;
		case EOpIdleDone:			ptrOldOperation.Set(KTxtOpIdleDone);		break;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		case EOpAuthenticate:		ptrOldOperation.Set(KTxtOpAuthenticate);	break;
#endif
		}
		
	switch(aPendingOperation)
		{
		case EOpNone:				ptrNewOperation.Set(KTxtOpNone);			break;
		case EOpServerGreeting:		ptrNewOperation.Set(KTxtOpServerGreeting);	break;
		case EOpSelect:				ptrNewOperation.Set(KTxtOpSelect);			break;
		case EOpLogin:				ptrNewOperation.Set(KTxtOpLogin);			break;
		case EOpLogout:				ptrNewOperation.Set(KTxtOpLogout);			break;
		case EOpClose:				ptrNewOperation.Set(KTxtOpClose);			break;
		case EOpWaitForBodyOperationComplete:
									ptrNewOperation.Set(KTxtOpWaitForBodyOperationComplete);	break;
		case EOpIdleEnter:			ptrNewOperation.Set(KTxtOpIdleEnter);		break;
		case EOpIdleWait:			ptrNewOperation.Set(KTxtOpIdleWait);		break;
		case EOpIdleDone:			ptrNewOperation.Set(KTxtOpIdleDone);		break;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		case EOpAuthenticate:		ptrNewOperation.Set(KTxtOpAuthenticate);	break;
#endif
		}
	
	_LIT8(KLogFormat, "CImapSession::iPendingOperation %S ==> %S");
	__LOG_FORMAT((iLogId, KLogFormat, &ptrOldOperation, &ptrNewOperation));
	
#endif //__IMAP_LOGGING
	
	iPendingOperation = aPendingOperation;	
	}

/**
Sets the state we believe the IMAP Server is in, and logs the transision.
All assignments to iServerState should occur through this method.
*/
void CImapSession::SetServerState(TImapServerState aServerState)
	{
	
#ifdef __IMAP_LOGGING

	_LIT8(KTxtServerStateNone, "EServerStateNone");
	_LIT8(KTxtServerStateNotAuthenticated, "EServerStateNotAuthenticated");
	_LIT8(KTxtServerStateAuthenticated, "EServerStateAuthenticated");
	_LIT8(KTxtServerStateSelected, "EServerStateSelected");
	_LIT8(KTxtServerStateUnknown, "Unknown");

	TPtrC8 ptrOldState(KTxtServerStateUnknown);
	TPtrC8 ptrNewState(KTxtServerStateUnknown);
	
	switch(iServerState)
		{
		case EServerStateNone: 				ptrOldState.Set(KTxtServerStateNone); 				break;
		case EServerStateNotAuthenticated: 	ptrOldState.Set(KTxtServerStateNotAuthenticated); 	break;
		case EServerStateAuthenticated: 	ptrOldState.Set(KTxtServerStateAuthenticated); 		break;
		case EServerStateSelected: 			ptrOldState.Set(KTxtServerStateSelected); 			break;
		}
		
	switch(aServerState)
		{
		case EServerStateNone: 				ptrNewState.Set(KTxtServerStateNone); 				break;
		case EServerStateNotAuthenticated: 	ptrNewState.Set(KTxtServerStateNotAuthenticated); 	break;
		case EServerStateAuthenticated: 	ptrNewState.Set(KTxtServerStateAuthenticated); 		break;
		case EServerStateSelected: 			ptrNewState.Set(KTxtServerStateSelected); 			break;
		}
	
	_LIT8(KLogFormat, "CImapSession::iServerState %S ==> %S");
	__LOG_FORMAT((iLogId, KLogFormat, &ptrOldState, &ptrNewState));
	
#endif //__IMAP_LOGGING
	
	iServerState = aServerState;	
	}

/**
Returns the logger file identifier
@return Logger file identifier
*/
EXPORT_C TInt CImapSession::LogId() const
	{
	return iLogId;
	}

TImapOverflowDetector::TImapOverflowDetector()
	: iOverflowDetected(EFalse)
	{}

/**
Returns whether Overflow() has been called.
This method resets the object's overflow flag, so that subsequent calls to this method
will return EFalse until the next time Overflow is called.
@return whether Overflow() has been called.
*/
TBool TImapOverflowDetector::OverflowDetected()
	{
	if (iOverflowDetected)
		{
		iOverflowDetected = EFalse;
		return ETrue;
		}
	return EFalse;
	}

/**
Implements TDes8Overflow by recording that an overflow has been detected.
*/
void TImapOverflowDetector::Overflow(TDes8& /*aDes*/)
	{
	iOverflowDetected = ETrue;
	}


/**
Issue the IMAP AuthenticateL command.
@param aStatus Signals completion of the request
@param  iCurrentAuthProfile  indicates which authenticate(cram-md5,plain,login)method to use
@param aStatus 		Signals completion of the request
*/
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
EXPORT_C void CImapSession::AuthenticateL(TRequestStatus& aStatus,CImapAuthMechanismHelper::TImapAuthProfileFlag iCurrentAuthProfile)
	{
	
	__LOG_TEXT(iLogId, "CImapSession::AuthenticateL()");
	__ASSERT_DEBUG(iServerState == EServerStateNotAuthenticated, TImapServerPanic::ImapPanic(TImapServerPanic::ELoginWhenServerStateUnknown));
	__ASSERT_DEBUG(iPendingOperation == EOpNone, TImapServerPanic::ImapPanic(TImapServerPanic::ELoginWhenInvalidOperationPending));
	if (CompleteIfStreamsClosed(aStatus)) 
		{
		return;
		}
	
	SetPendingOperation(EOpAuthenticate); // << Server state becomes Authenticated
	switch(iCurrentAuthProfile)
		{
		case CImapAuthMechanismHelper::EPlain:
			{
			StartCommandL(aStatus, CImapAuthPlainMechanismHelper::NewL(iImapSettings, iSelectedFolderInfo, iLogId));
			}
			break;
		case CImapAuthMechanismHelper::ELogin:
			{
			StartCommandL(aStatus, CImapAuthLoginMechanismHelper::NewL(iImapSettings,iSelectedFolderInfo,iLogId));
			}
			break;
		case CImapAuthMechanismHelper::ECramMD5:
			{
			StartCommandL(aStatus, CImapAuthCramMd5MechanismHelper::NewL(iImapSettings,iSelectedFolderInfo,iLogId));
			}
			break;
		default:
			{
			User::Leave(KErrNotSupported);
			break;	
			}	
	    }
	}
#endif
