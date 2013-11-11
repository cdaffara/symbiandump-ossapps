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
// Client MTM for the IMAP4 protocol
// 
//


#if !defined (__IMPCMTM_H__)
#define __IMPCMTM_H__

#if !defined (__MTCLBASE_H_)
#include <mtclbase.h>
#endif

#if !defined (__MIUTHDR_H_)
#include <miuthdr.h>
#endif

#if !defined (__MIUTPARS_H__)
#include <miutpars.h>		//TImMessageField
#endif

#if !defined (__IMAPSET_H__)
#include <imapset.h>
#endif

#if !defined (__IMAPCMDS_H__)
#include <imapcmds.h>
#endif

#if !defined(__OFFOP_H__)
#include <offop.h>
#endif

#if !defined(__MIUTMSG_H__)
#include <miutmsg.h>
#endif



// Forward declarations.
class CImap4ClientSessionObserver;
class CImImap4GetMail;
class CImEmailAutoSend;

//
// Imap4 Client MTM  
class CImap4ClientMtm : public CBaseMtm, public MImUndoOffLineOperation

/** IMAP4 client MTM interface, providing access to the email services using the IMAP4 
protocol.

The class provides support for:

- connecting to remote IMAP4 mail servers
- synchronising message headers with mailboxes on these servers
- getting message bodies and attachments
- copying and moving messages
- creating messages that forward or reply to IMAP4 messages
- server notification of new messages using IMAP IDLE
- queueing operations requested when there is no connection to a mail server, to be 
performed when a connection is made (offline operations).

Note that CImap4ClientMtm does not support many of the generic functions defined by 
the @c CBaseMtm base class. In particular:

- Message address information, body, subjects, and attachments are not manipulated 
through CImap4ClientMtm. Consequently, SaveMessageL() has an empty 
implementation, and is not used. Message access and manipulation is instead done 
through CImEmailMessage, which understands the email specific message formats, 
including MHTML (RFC 2557). The RFC 822 email header, which includes subject and 
address information, is encapsulated in a CImHeader object, which can be obtained from 
a CImEmailMessage object. IMAP also has its own specific flags, such as draft, 
answered, and deleted: this can be accessed through TMsvEmailEntry, which extends the 
message server's generic TMsvEntry to add email-specific information.

- The idea of a default service used by base class functions DefaultServiceL(), 
ChangeDefaultServiceL(), and RemoveDefaultServiceL() is not supported.

IMAP specific commands are issued through the InvokeAsyncFunctionL() function, with 
the commands defined in the #TImap4Cmds enumeration. Details of particular commands 
are given in the documentation for that enumeration, but the following describes some of 
the key concepts required to use those commands. Note that usually these commands do 
not correspond directly to the IMAP protocol commands described in RFC 3501. Instead, 
they are at a higher level, to simplify the logic required for an email client program. 

--------------------------------------

Connection and services

Settings for connection to and use of an IMAP4 mail server and its mailboxes are 
encapsulated in an CImImap4Settings object, created and accessed through 
CEmailAccounts. Preferences for the network connection (e.g. the ISP to use) to be made 
to access an IMAP server are also accessed through CEmailAccounts.

The settings are associated with a service-type entry in the Message Server's message 
store. Message and folder type entries under the service entry store a local copy of the 
mailboxes and messages that are present on an IMAP server. For this reason, the service 
is sometimes referred to as the local mirror of an IMAP server.

By default, it is assumed that the user has a primary mailbox on the server called INBOX, 
though this can be changed if necessary using CImImap4Settings::SetFolderPathL(). (To 
avoid confusion, the term "Inbox" used in the IMAP MTM documentation refers to this 
mailbox, rather than the Message Server's standard local Inbox folder.)

The settings store the user's log-in details required to access the IMAP server, and other 
settings not related to connection, but which affect the behaviour of commands, such as 
synchronisation, made to the service.

--------------------------------------

Synchronisation

The IMAP4 MTM provides the means to synchronise message headers on a remote 
IMAP mailboxes with the messages headers on the phone. Note that synchronisation only 
refers to message headers. Once headers are synchronised, populate operations (i.e. 
getting the message bodies) can be done.

The basic steps involved in a full synchronisation are:

- The headers of messages in the Inbox are synchronised. 
- The local list of folders is synchronised with those existing on the remote IMAP service. 
Depending on the service settings, the local or remote settings of subscribed mailboxes 
may be updated (see below for more information). 
- The headers of messages in subscribed folders are synchronised. 

Queued offline operations (see below) pending on the Inbox or subscribed folders are 
performed prior to the relevant folder being synchronised.

Synchronisations can be performed in the background or the foreground. A foreground 
synchronisation means no other requests, such as message fetching, can be made to the 
MTM, until the synchronisation is complete. A background synchronisation allows some 
types of command to be given while it is in progress. Commands that write to the remote 
server are not allowed however while a background synchronisation is in progress. A 
client is informed of the state of a background synchronisation through the 
MMsvImapConnectionObserver callback interface.

Synchronisations commands are made through  
CImap4ClientMtm::InvokeAsyncFunctionL(). There a variety of commands available 
that give different options for compounding synchronisation
with other commands, such as connection, and synchronising only particular folders. See 
TImap4Cmds for details. Progress information for synchronisation commands can be 
obtained from the CMsvOperation object returned by the InvokeAsyncFunctionL(). 
Synchronisation progress information is encapsulated in a TImap4SyncProgress object.

Settings that affect how a synchronisation is performed include:

- Subscriptions: when an IMAP service had been synchronised, the Messaging server will 
contain entries for all folders on the remote server. These entries will be marked invisible 
within the TMsvEntry for the folder, implying that they should be invisible to the user. 
Messages in these folders are not synchronised unless the folder has been subscribed to. 
Note a folder subscription can be set either locally (i.e. just on the phone), using 
the #KIMAP4MTMLocalSubscribe command, or remotely (on the server), possibly through email 
clients on other devices. A service can be set to synchronise folders using either or both 
of these types of subscription (CImImap4Settings::SetSynchronise()). Local and remote 
subscriptions can themselves be synchronised in various ways (CImImap4Settings::SetSuscribe()).

- Filters: a filter prevents certain e-mail messages from being synchronised onto the 
device when a client requests a synchronisation. Filters can include anything permitted by 
the IMAP Search command, including date, size, content, and message flags.

- Limits: service settings can limit the number of emails synchronised to the inbox 
(CImImap4Settings::SetInboxSynchronisationLimit()), and to other folders 
(CImImap4Settings::SetMailboxSynchronisationLimit()).

- Sync rate: one of the synchronisation commands 
(KIMAP4MTMConnectAndSyncCompleteAfterDisconnect) periodically resynchronises 
the Inbox until the service is disconnected. CImImap4Settings::SyncRate() sets the 
refresh period.

--------------------------------------

Getting and copying messages

After messages headers have been synchronised, message bodies and attachments can be 
fetched from the remote email server. Getting message parts and saving them in the 
mirror service is calling populating them.

Commands for these actions can be made through  
CImap4ClientMtm::InvokeAsyncFunctionL(). There are a large number of commands, 
for different combinations of these options:

- action type: whether to just populate messages, or to also copy or move them to a local 
folder 
- message selection: whether to get all, new, or selected messages
- connection: whether to make a new connection or assume an existing connection
- disconnect: whether to disconnect or stay online after operation is complete

For the populate type commands, further options can be set that control the message 
getting behaviour.  Basic options control, encapsulated in TImImap4GetMailInfo, specify 
whether body text and/or attachments are fetched, and a maximum message size. There 
are also options, encapsulated in TImImap4GetPartialMailInfo, that allow size limits to 
be separately specified for body text and/or attachments. If the the body is larger than the 
limit, then the body is partially downloaded up to the limit. Only attachments smaller 
than the specified size are downloaded. A partially downloaded message can later be 
fully downloaded.

For the copy or move type commands,  a TImImap4GetMailInfo parameter is supplied, in 
order to specify the destination folder for the messages, and a maximum message size. 

Progress information for getting commands can be obtained from the CMsvOperation 
object returned by the InvokeAsyncFunctionL(). Progress information is encapsulated in 
a TImap4GenericProgress object.

Fetching and then copying or moving specified messages can also be performed by using 
the standard Messaging Framework @c CMsvEntry::CopyL() and @c 
CMsvEntry::MoveL() functions on entries under the remote service. If these functions are 
used, then the entire message is fetched without size limits.

CMsvEntry functions can also be used to:

- create a folder on a remote server
- delete messages
- copy or move messages from a local folder into a remote folder
- copy or move messages between remote folders

Note that changing an existing entry through CMsvEntry is not supported. An MTM-
specific command #KIMAP4MTMRenameFolder is instead provided to rename a remote 
folder.

The page "CMsvEntry functions for IMAP4 message entries", linked to in the "See also" 
section below, provides more details on using CMsvEntry.

--------------------------------------

Offline operations

Some operations can only be performed while online, while other commands may 
be stored while offline for processing when next connected. An attempt to perform a 
command while offline that requires the MTM to be connected results in immediate 
completion with the error code KErrDisconnected.

Permitted offline operations include:

- copy
- move
- delete

Queued offline operations are usually performed when a connection is made, prior to the  
relevant folder being synchronised. Delete operations can alternatively be set to be done 
on disconnection using the service setting 
CImImap4Settings::SetDeleteEmailsWhenDisconnecting().

Note that:
- Offline operations are only permitted if the service setting SetDisconnectedUserMode() 
is true.
- It is possible to undo pending offline operations using the commands 
#KIMAP4MTMCancelOffLineOperations and #KIMAP4MTMUndeleteAll.

--------------------------------------

IMAP IDLE support

IMAP IDLE (RFC 2177) is an optional expansion of the IMAP email accessing protocol 
that allows the server to send updates to the client that messages have been created or 
deleted in real time. The IDLE command is sent from the client to the server when the 
client is ready to accept unsolicited mailbox update messages. Whether the client requests 
the server to provide IDLE support is set in the CImImap4Settings::SetImapIdle() service 
setting. When the IMAP MTM receives such a notification, it synchronises the changed 
folder. Email clients can be notified of such changes by setting a MMsvEntryObserver 
observer on a folder.

@publishedAll
@released
*/
	{
public:
   	IMPORT_C static CImap4ClientMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aSession);
	
	~CImap4ClientMtm();
	void HandleEntryEvent(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	void StoreL(); // uses a CMsvStore from the Session
	void RestoreL();
	CMsvOperation* ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
	CMsvOperation* ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
	TUint ValidateMessage(TUint aPartList);
	TMsvPartList Find(const TDesC& aTextToFind, TMsvPartList aPartList);
	void SaveMessageL(); 
	void LoadMessageL();
	IMPORT_C void StoreSettingsL();	// uses a CMsvStore from the session
	IMPORT_C void RestoreSettingsL();

	// --- RTTI functions ---
	TInt QueryCapability(TUid aCapability, TInt& aResponse);
	void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);
	CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus);
	// Addressees have no meaning in the text mtm.
	void AddAddresseeL(const TDesC& aRealAddress);
	void AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
	void RemoveAddressee(TInt aIndex);

	// Attachment functions to support the SendAs API

	IMPORT_C virtual void AddAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddAttachmentL(RFile& aFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddLinkedAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddEntryAsAttachmentL(TMsvId aAttachmentId, TRequestStatus& aStatus);
	IMPORT_C virtual void CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C void CreateMessageL(TMsvId aServiceId);
	
	IMPORT_C virtual TMsvId DefaultServiceL() const;
	IMPORT_C virtual void RemoveDefaultServiceL();
	IMPORT_C virtual void ChangeDefaultServiceL(const TMsvId& aService);

public: // Returning a list of all the offline operations for a service entry.
    IMPORT_C CImOperationQueueList* QueueListL(CMsvEntry& aServiceEntry);
public:  // Wrapper to settings
    IMPORT_C const CImImap4Settings& Imap4Settings() const;
    IMPORT_C void SetImap4SettingsL(const CImImap4Settings& aSettings);
public: // inherited from MUndoOffLine
    virtual void UndoOffLineChangesL(const CImOffLineOperation& aDeleted, TMsvId aFolderId); 
protected:
	CImap4ClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aSession);
	void ConstructL();
	void ContextEntrySwitched(); // called after the context of this instance has been changed to another entry

private:
	TBool ValidateAddress(const TPtrC& anAddress);
	void SendOnNextConnectionL();
	TMsvPartList DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList);
	void FilterAllOrNewMailsL(TInt aFunctionId,const CMsvEntrySelection& aSelection,TDes8& aParameter);
	void FilterMailSelectionL(const CMsvEntrySelection& aSelection,TDes8& aParameter);
	CMsvOperation* CopyMoveOrPopulateL(TInt aFunctionId,TDes8& aParameter,TRequestStatus& aCompletionStatus);
	void ConvertToPartialPopulate(TDes8& aParameter);
	// To check whether a partial fetch or full fetch of message has to be done.
	TBool IsPartialPopulate(TDes8& aParameter);

private:
	CImImap4Settings iImImap4Settings;
	TImMessageField iTImMessageField;
	CImap4ClientSessionObserver* iImap4ClientSessionObserver;
	HBufC* iEmailAddressFormatString;	// defines format of email address used by "Send as" API eg _L("/"%S/" <%S>")
	CImHeader* iHeader;
	CMsvEntrySelection* iMsvEntrySelection;
	CImImap4GetMail*	iImIMAP4GetMail;
	CImEmailOperation*	iImEmailOperation;
	TPckgBuf<TImImap4GetPartialMailInfo>	iImap4GetPartialMailInfo;
	};

class CImImap4GetMail : public CMsvOperation
/** Encapsulates an operation to copy, move, and populate (i.e. download the full 
message body) IMAP4 emails from the remote inbox to any local folder.

Note that the same operations are available by calling CImap4ClientMtm::InvokeAsyncFunctionL() 
with a suitable command. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C CMsvOperation* GetMailL(TInt aFunctionId, CImap4ClientMtm& aImap4ClientMtm, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aImap4GetMailInfo, TRequestStatus& aObserverRequestStatus);
	~CImImap4GetMail();
	void DoCancel();
	void RunL();
	const TDesC8& ProgressL();
	const TDesC8& FinalProgress();
private:
	CImImap4GetMail(CMsvSession& aMsvSession, CImap4ClientMtm& aImap4ClientMtm, TRequestStatus& aObserverRequestStatus);
	void ConstructL(TInt aFunctionId, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aImap4GetMailInfo);
	void SelectNextStateL();					// selects next state to go to
	void ChangeStateL();					// initiates the next state operation
	void SelectAndChangeToNextStateL();
	void RequestComplete(TInt aError);
	void Complete();
	void ConnectToMailboxL();
	void CopyMoveNewMessagesL(TBool aCopy);
	void CopyMoveMessageSelectionL(TBool aCopy);
	void CopyMoveAllMessagesL(TBool aCopy);
	void PopulateNewMessagesL();
	void PopulateAllMessagesL();
	void PopulateMessageSelectionL();
	void DisconnectFromMailboxL();
	void ResetProgress();
	void StoreProgressL();
private:
	enum TImImap4GetMailState
		{
		EConnectToMailbox,
		ECopyNewMessages,
		EMoveNewMessages,
		EPopulateNewMessages,
		ECopyMessageSelection,
		EMoveMessageSelection,
		EPopulateMessageSelection,
		ECopyAllMessages,
		EMoveAllMessages,
		EPopulateAllMessages,
		EDisconnectFromMailbox,
		EFinished
		};
	
	CImap4ClientMtm&				iImap4ClientMtm;
	CMsvEntrySelection*				iMsvEntrySelection;
	CMsvOperation*					iMsvOperation;

	TImap4GenericProgress			iProgress;
	TImap4GenericProgress			iErrorProgress;
	TImImap4GetMailState			iState;
	TInt							iCommand;
	TPckgBuf<TImap4GenericProgress>	iProgressBuf;
	TPckgBuf<TImImap4GetPartialMailInfo>	iImap4GetPartialMailInfo;
	};

#endif // __IMPCMTM_H__
