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

#ifndef __IMAPSET_H__
#define __IMAPSET_H__

 


#include <msvuids.h>
#include <miutset.h>

class TBearerTypes;
class TImImap4GetPartialMailInfo;

/** Default IMAP4 server port number. */
const TUint32 KIMAPDefaultPortNumber = 143;

/** Default inbox refresh rate in seconds. */
const TInt KImapDefaultInboxSyncRate = 300;

/** Default idle timeout in seconds. */
const TInt KImapDefaultIdleTimeout = 1740;

/** Default fetch size in bytes. */
const TUint KImapDefaultFetchSizeBytes = 20480;



/** @deprecated */
const TInt32 KImImapSynchroniseAll = -1;

/** @deprecated */
const TInt32 KImImapSynchroniseNone	= 0;


/**
Describes the folder subscription synchronisation strategy.

This specifies whether the local or remote folder subscription flags, or a
combination of both, are used to determine which mailboxes are synchronised.
For an overview of the synchronisation process, see CImap4ClientMtm.

@see CImImap4Settings::SetSynchronise()
@see CImImap4Settings::Synchronise()

@publishedAll
@released
*/
enum TFolderSyncType
	{
	/** Folders that are marked for subscription either locally or remotely are synchronised. */
	EUseCombination=0,
	/** Only folders marked for subscription locally are synchronised. */
	EUseLocal,
	/** Only folders marked for subscription remotely are synchronised. */
	EUseRemote
	};


/**
Specifies methods of synchronising IMAP4 subscription information with a server. 

Subscription settings that have been set on folders on the local device, and 
subscription settings that have been set on the corresponding mailboxes on the 
remote server can be synchronised, as part of the synchronisation process
(see the overview for CImap4ClientMtm).

@see CImImap4Settings::SetSubscribe()
@see CImImap4Settings::Subscribe()

@publishedAll
@released
*/
enum TFolderSubscribeType
	{
	/** Do not alter local or remote subscription information. */
	EUpdateNeither=0,
	/** Update local copy of subscription information when it is received from the 
	remote IMAP4 server at the beginning of the mail session. */
	EUpdateLocal,
	/** Update remote copy of subscription information so that it matches the local 
	subscription information. */
	EUpdateRemote,
	/** Merge local and remote subscription information, store updated information 
	on the remote server and store merged data on the local message store. */
	EUpdateBoth
	};


/**
Defines which components of a message to fetch when copying or moving from
a server. 

@see CImImap4Settings::SetGetMailOptions()
@see CImImap4Settings::GetMailOptions()

@publishedAll
@released
*/
enum TImap4GetMailOptions
	{
	/** Get message headers. */
	EGetImap4EmailHeaders,
	/** Get message body. */
	EGetImap4EmailBodyText,
	/** Get message body and attachments. */
	EGetImap4EmailBodyTextAndAttachments,
	/** Get message attachments. */
	EGetImap4EmailAttachments,
	/** Get message body, including non-plain or non-HTML alternative text parts (or attachments). */
	EGetImap4EmailBodyAlternativeText
	};

/**
Defines which components of a message should be fetched partially when 
fetching the message from a server.

@see
CImImap4Settings

@publishedAll
@released
*/
enum TImImap4PartialMailOptions
	{
	/** No size limits, full message to be fetched.*/
	ENoSizeLimits,
	/** Get body text only. 
	Gets text/plain part only if text/plain part size + text/html part size
	is greater than body text size limit.
	Gets text/plain part and text/html part if text/plain part size + text/html part size
	is less than body text size limit of the message.*/
	EBodyTextOnly,
	/** Gets all the attachments less than attachment size limit.*/
	EAttachmentsOnly,
	/**  Gets the body text less than or equal to body text size limit and attachments 
	less than or equal to attachment size limit.*/
	EBodyTextAndAttachments,
	/** Gets the body text less than total size limit and also attachments whose size 
	is within total size limit minus fetched body parts.*/
	ECumulative,
	/** Gets the body text less than total size limit and all non-HTML text attachments whose size
	is within total size limit minus fetched body parts.*/	
	EBodyAlternativeText
	}; 

/**
Flags to indicate what type of IMAP4 progress information is being returned. 

@publishedAll
@released
*/
enum TImap4ProgressType
	{
	/** Progress information is an TImap4GenericProgress object. */
	EImap4GenericProgressType,
	/** Progress information is an TImap4SyncProgress object. */
	EImap4SyncProgressType
	};


class TImap4GenericProgress
/**
Progress information for an IMAP4 get operation.

For the 'get mail when already connected' operations, the progress state can 
be obtained from TImap4GenericProgress::iState. In the case of the 'connect 
and get mail and disconnect' and 'connect and get mail and stay online', the 
actual progress defined in TImap4GenericProgress::iState will be of the form 
of 'Copy New Mail', 'Populating Message Selection' etc. The sub-operation 
state can then be obtained in the form of 'Connecting', 'Fetching', 'Disconnecting' 
etc. from TImap4GenericProgress::iImap4SubStateProgress.

@see
CImImap4GetMail 

@publishedAll
@released
*/
	{
public:
	IMPORT_C TInt ConnectionState() const;
	IMPORT_C TInt ConnectionIAP() const;

public:
	/** Operation type. */
	enum TImap4GenericProgressOp
		{
	/** Connect operation. */
		EConnect,
	/** Disconnect operation. */
		EDisconnect, 
	/** Sync operation. */
		ESync,
	/** Select operation. */
		ESelect,		
	/** Copy messages to local operation. */
		ECopyToLocal,
	/** Copy messages within remote server operation. */
		ECopyWithinService,
	/** Copy messages from local operation. */
		ECopyFromLocal,
	/** Move messages to local operation. */
		EMoveToLocal,
	/** Move messages within remote server operation. */
		EMoveWithinService,
	/** Move messages from local operation. */
		EMoveFromLocal,
	/** Message population operation. */
		EPopulate,		
	/** Delete operation. */
		EDelete,		
	/** Offline delete operation. */
		EOffLineDelete,
	/** Offline undelete operation. */
		EOffLineUndelete,
	/** Offline delete operation. */
		EOffLineCopyToLocal,
	/** Offline move messages to local operation. */
		EOffLineMoveToLocal,
	/** Offline copy messages from local operation. */
		EOffLineCopyFromLocal,
	/** Offline move messages from local operation. */
		EOffLineMoveFromLocal,
	/** Offline copy messages within remote server operation. */
		EOffLineCopyWithinService,
	/** Offline move messages within remote server operation. */
		EOffLineMoveWithinService,
	/** Offline message population operation. */
		EOffLinePopulate
		};

	/** Operation state information. */
	enum TImap4GenericProgressState
		{
	/** Disconnected. */
		EDisconnected,
	/** Connecting. */
		EConnecting,
	/** Idle. */
		EIdle,
	/** Selecting the folder to use. */
		ESelecting,
	/** Fetching messages to mirror server. */
		EFetching,
	/** Appending local messages to remote server. */
		EAppending,
	/** Copying local messages to local or within service. */
		ECopying,
	/** Deleting an item, including expunging a folder. */
		EDeleting,
	/** Synchronising folder after an operation. */
		ESyncing,
	/** Disconnecting. */
		EDisconnecting,
	/** Sync already in progress. */
		EBusy,
	/** CImImap4GetMail move operation in progress. */
		EMoving,
	/** CImImap4GetMail copy new mail operation in progress. */
		ECopyNewMail,
	/** CImImap4GetMail move new mail operation in progress. */
		EMoveNewMail,
	/** CImImap4GetMail copy selected messages operation in progress. */
		ECopyMailSelection,
	/** CImImap4GetMail move selected messages operation in progress. */
		EMoveMailSelection,
	/** CImImap4GetMail copy all messages operation in progress. */
		ECopyAllMail,
	/** CImImap4GetMail move all messages operation in progress. */
		EMoveAllMail,
	/** CImImap4GetMail populate new messages operation in progress. */
		EPopulateNewMail,
	/** CImImap4GetMail populate all messages operation in progress. */
		EPopulateAllMail,
	/** CImImap4GetMail populate selected messages operation in progress. */
		EPopulateMailSelection
		};

	/** Progress information type: always EImap4GenericProgressType. */
	TImap4ProgressType iType;
	/** Operation type. */
	TImap4GenericProgressOp iOperation;
	/** 
	The progress state.
	
	For 'connect and get mail and disconnect' and 'connect and get mail and stay 
	online' type operations, additional sub-operation state can be obtained from 
	iImap4SubStateProgress.
	*/
	TImap4GenericProgressState iState; 
	/** 
	The progress sub-state.
	
	This only applies to 'connect and get mail and disconnect' and 'connect and 
	get mail and stay online' type operations.
	*/
	TImap4GenericProgressState iImap4SubStateProgress;
	/** In the EConnecting state, this field holds the connection iap value, 
	otherwise it holds the number of messages still to be processed. */
	TInt		iMsgsToDo;
	/** In the EConnecting state, this field holds the connection stage value, 
	otherwise it holds the number of messages processed. */
	TInt		iMsgsDone;
	/** Number of message parts still to be processed. */
	TInt		iPartsToDo;
	/** Number of message parts processed. */
	TInt		iPartsDone;
	/** Number of bytes (for a multipart fetch) still to be processed. */
	TInt		iBytesToDo;
	/** Number of bytes (for a multipart fetch) processed. */
	TInt		iBytesDone;
	/** Error code. */
	TInt		iErrorCode;
	/** Message ID returned from the operation. */
	TMsvId		iReturnedMsvId;
	/** The total size of messages to be downloaded (bytes). */
	TInt		iTotalSize;
	};


class TImap4SyncProgress
/**
IMAP4 synchronisation progress information. 

The client may then use an object of this type created by the server MTM to 
track synchronisation progress.

@publishedAll
@released
*/
	{
public:
	/** Synchronisation state. */
	enum TImap4SyncProgressState
		{
	/** Idle. */
		EIdle,
	/** Busy. */
		EBusy,
	/** Connecting. */
		EConnecting,
	/** Disconnecting. */
		EDisconnecting,
	/** Synchronising inbox. */
		ESyncInbox,
	/** Synchronising folder tree. */
		ESyncFolderTree,
	/** Checking remote subscription. */
		ECheckRemoteSubscription,
	/** Updating remote subscription. */
		EUpdateRemoteSubscription,
	/** Synchronising other information. */
		ESyncOther,
	/** Deleting. */
		EDeleting,
	/** Processing pending operations. */
		EProcessingPendingOps
		};

	/** Progress information type: always EImapSyncProgressType. */
	TImap4ProgressType iType;		
	/** Synchronisation state. */
	TImap4SyncProgressState iState;	//	Where is the sync up to?
	/** 
	"Folders to do" count for synchronisation. 
	This is meaningful during the #ESyncOther and #EDeleting 
	synchronisation stages.
	*/
	TInt		iFoldersToDo;
	/**
	"Folders done" count for synchronisation. 
	This is meaningful during the #ESyncOther and #EDeleting 
	synchronisation stages.
	*/
	TInt		iFoldersDone;
	/** 
	"Message to do" count for synchronisation.
	This is meaningful during the #ESyncInbox, #ESyncOther and #EProcessingPendingOps
	synchronisation stages.
	*/
	TInt		iMsgsToDo;
	/**
	"Message done" count for synchronisation.
	This is meaningful during the #ESyncInbox, #ESyncOther and #EProcessingPendingOps
	synchronisation stages.
	*/
	TInt		iMsgsDone;
	/**
	Number of headers fetched during synchronisation.
	*/
	TInt		iHeadersFetched;
	/**
	Number of folders orphaned during synchronisation.
	
	An orphaned folder is a local folder which existed 
	under the service, but which did not exist
	on the remote server when synchronisation was done.	
	*/
	TInt		iOrphanedFolders;
	/**
	Number of new folders during synchronisation.
	*/
	TInt		iNewFolders;
	/**
	Number of orphaned messages during synchronisation.

	An orphaned message is one which existed locally 
	under the service, but which did not exist
	on the remote server when synchronisation was done.	
	*/
	TInt		iOrphanedMessages;
	/**
	Number of remote messages tagged for deletion.
	
	In IMAP, messages can be flagged as deleted, before 
	they are later permanently removed.
	*/
	TInt		iRemoteMessagesDeleteTagged;
	/** Number of messages fetched during synchronisation. 
	
	In the current implementation, this is always 0. Use
	iHeadersFetched to get the number of headers fetched.	
	*/
	TInt		iMessagesFetchedOK;
	/** Number of message parts fetched during synchronisation. 
	
	In the current implementation, this is always 0.
	*/
	TInt		iMessagePartsFetchedOK;
	/** Number of message parts not found during synchronisation. 
	
	In the current implementation, this is always 0.
	*/
	TInt		iMessagePartsNotFound;
	/** Number of folders not found during synchronisation. 
	
	This can occur if the remote server returns an error when 
	a client attempts to select a folder during synchronisation. 
	*/
	TInt		iFoldersNotFound;
	/** Any error-code that the client needs to be informed of. */
	TInt		iErrorCode;
	};

class TImap4CompoundProgress
/**
IMAP operation progress information.

The class supplies both the two IMAP progress types, generic and synchronisation.

The IMAP4 server MTM owns two IMAP4 sessions that are used independently of 
each other to perform 'background' and 'foreground' operations. In a typical 
situation, the user may be downloading the body parts of a message (populating) 
in the foreground whilst in the background a full synchronisation may be underway. 
In this scenario, the generic progress will give an indication of the 'foreground' 
activity and the synchronisation progress will give an indication of the 'background' 
progress. The synchronisation progress component of the compound progress 
will always show the synchronisation progress irrespective of whether the 
synchronisation is running in the foreground or the background. 

@publishedAll
@released
*/
	{
public:
	/** Generic progress. */
	TImap4GenericProgress	iGenericProgress;
	/** Synchronisation progress. */
	TImap4SyncProgress		iSyncProgress;
	};


class CImImap4Settings : public CImBaseEmailSettings
/**
Settings for connection to and use of an IMAP4 mail server and its mailboxes.

Messaging clients should use an instance of this class to specify and retrieve
configuration settings that are used by an IMAP4 service when executing email
operations.

Service settings include:

- server log on settings (user name and password)
- message header synchronisation strategy and synchronisation limits
- message getting options, such as maximum size. Note that these options 
  are not used by the IMAP MTM. They can be used by client applications to 
  store user preferences, and be used by the client when issuing get commands.
- miscelleanous settings, including whether IMAP IDLE should be used

CImImap4Settings objects are created and accessed through
CEmailAccounts. CEmailAccounts can also be used to get the 
connection settings, such as the ISP to use, used to access an IMAP server.

Settings for a service are associated with a service-type entry in the Message 
Server'smessage store. However the settings are not actually stored in the 
message store, but in the device's Central Repository data store. 

@see CEmailAccounts

@publishedAll
@released
*/
	{
public:
	IMPORT_C CImImap4Settings();
	IMPORT_C virtual ~CImImap4Settings();
	IMPORT_C void Reset();
	IMPORT_C const TPtrC8 LoginName() const;
	IMPORT_C void SetLoginNameL(const TDesC8& aLoginName);
	IMPORT_C const TPtrC8 Password() const;
	IMPORT_C void SetPasswordL(const TDesC8& aPassword);
	IMPORT_C const TPtrC8 FolderPath() const;
	IMPORT_C void SetFolderPathL(const TDesC8& aFolderPath);
	IMPORT_C const TPtrC8 SearchString() const;
	IMPORT_C void SetSearchStringL(const TDesC8& aSearchString);
	IMPORT_C TText8 PathSeparator() const;
	IMPORT_C void SetPathSeparator(const TText8 aPathSeparator);
	IMPORT_C TBool DisconnectedUserMode() const;
	IMPORT_C void SetDisconnectedUserMode(TBool aFlag);
	IMPORT_C void SetSynchronise(const TFolderSyncType aType);
	IMPORT_C TFolderSyncType Synchronise() const;
	IMPORT_C void SetSubscribe(const TFolderSubscribeType aType);
	IMPORT_C TFolderSubscribeType Subscribe() const;
	IMPORT_C TBool AutoSendOnConnect() const;
	IMPORT_C void SetAutoSendOnConnect(TBool aFlag);
	IMPORT_C TUint MaxEmailSize() const;
	IMPORT_C void SetMaxEmailSize(const TUint aMaxEmailSize);
	IMPORT_C TBool DeleteEmailsWhenDisconnecting() const;
	IMPORT_C void SetDeleteEmailsWhenDisconnecting(TBool aFlag);
	IMPORT_C TBool AcknowledgeReceipts() const;
	IMPORT_C void SetAcknowledgeReceipts(TBool aFlag);
	IMPORT_C TImap4GetMailOptions GetMailOptions() const;
	IMPORT_C void SetGetMailOptions(TImap4GetMailOptions aGetMailOptions);
	IMPORT_C CImImap4Settings& CopyL(const CImImap4Settings& aCImImap4Settings);
	IMPORT_C TBool operator==(const CImImap4Settings& aCImImap4Settings) const;
	IMPORT_C TInt32 InboxSynchronisationLimit() const;
	IMPORT_C void SetInboxSynchronisationLimit(const TInt32 aInboxSyncLimit);
	IMPORT_C TInt32 MailboxSynchronisationLimit() const;
	IMPORT_C void SetMailboxSynchronisationLimit(const TInt32 aMailboxSyncLimit);
	IMPORT_C TBool UpdatingSeenFlags() const;
	IMPORT_C void SetUpdatingSeenFlags(TBool aFlag);
	IMPORT_C TInt SyncRate() const;
	IMPORT_C void SetSyncRateL(TInt aSyncRate);
	IMPORT_C TUint FetchSize() const;
	IMPORT_C void SetFetchSizeL(TUint aFetchSizeBytes);
	IMPORT_C TBool ImapIdle() const;
	IMPORT_C void SetImapIdle(TBool aFlag);
	IMPORT_C TInt ImapIdleTimeout() const;
	IMPORT_C void SetImapIdleTimeoutL(TInt aIdleTimeout);
	IMPORT_C TImImap4PartialMailOptions PartialMailOptions() const;
	IMPORT_C void SetPartialMailOptionsL(TImImap4PartialMailOptions aPartialMailOptions); 
	IMPORT_C TInt32 BodyTextSizeLimit() const;
	IMPORT_C void SetBodyTextSizeLimitL(TInt32 aBodyTextSizeLimit);
	IMPORT_C TInt32 AttachmentSizeLimit() const;
	IMPORT_C void SetAttachmentSizeLimitL(TInt32 aAttachmentSizeLimit);
	IMPORT_C TBool UseExpunge() const;
	IMPORT_C void SetUseExpunge(TBool aFlag);
	IMPORT_C void SetTlsSslDomainL(const TDesC8& aDomainName);
	IMPORT_C TPtrC8 TlsSslDomain() const;
	IMPORT_C void SetUseSyncDownloadRules(TBool aFlag);
	IMPORT_C TBool UseSyncDownloadRules();
	IMPORT_C void SetFolderSyncDisabled(TBool aFlag);
	IMPORT_C TBool FolderSyncDisabled();
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	IMPORT_C TBool IMAP4Auth() const;
	IMPORT_C void SetIMAP4Auth(TBool aFlag);
	IMPORT_C TBool FallBack() const;
	IMPORT_C void SetFallBack(TBool aFlag);
#endif

private:
	class TImImap4SettingsExtension
		{
	public:
		inline TImImap4SettingsExtension();
	public:
		HBufC8* iLoginName;
		HBufC8* iPassword;
		HBufC8* iFolderPath;
		HBufC8*	iSearchString;
		HBufC8* iTlsSslDomain;
		TInt	iSyncRate;
		TInt	iImapIdleTimeout;
		TUint	iFetchSizeBytes;
		TImImap4PartialMailOptions	iPartialMailOptions;
		TInt32	iBodyTextSizeLimit;
		TInt32	iAttachmentSizeLimit;
		};
	
	inline TImImap4SettingsExtension* Extension() const;
	inline void CheckExtensionExistsL();

	enum TImImap4EmailSettings
		{
		KImap4EmailSettingsClearFlag				= 0x00000000,
		KImap4BaseEmailSettingsLastUsedFlag			= CImBaseEmailSettings::EBaseEmailSettingsLastUsedFlag, //0x00000002
		KImap4EmailDisconnectedModeFlag				= KImap4BaseEmailSettingsLastUsedFlag << 1, 	//0x00000004
		KImap4EmailAutoSendFlag						= KImap4EmailDisconnectedModeFlag << 1, 		//0x00000008
		KImap4EmailDeleteEmailsWhenDisconnecting	= KImap4EmailAutoSendFlag << 1, 				//0x00000010
		KImap4EmailAcknowledgeReceipts				= KImap4EmailDeleteEmailsWhenDisconnecting << 1,//0x00000020
		KImap4EmailUpdatingSeenFlags				= KImap4EmailAcknowledgeReceipts << 1, 			//0x00000040
		KImap4EmailIdleFlag							= KImap4EmailUpdatingSeenFlags << 1, 			//0x00000080
		KImap4EmailExpungeFlag						= KImap4EmailIdleFlag << 1,
		KImap4EmailUseSyncDownloadRules				= KImap4EmailExpungeFlag << 1, 					//0x00000200
		KImap4EmailSettingsFolderSyncDisabled		= KImap4EmailUseSyncDownloadRules << 1, 		//0x00000400
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		KImap4EmailSettingsAuthenticationFlag 		= KImap4EmailSettingsFolderSyncDisabled << 1, 	//0x00000800
		KImap4EmailSettingsFallBackFlag	 			= KImap4EmailSettingsAuthenticationFlag << 1, 	//0x00001000
		KImap4EmailSettingsLastUsedFlag				= KImap4EmailSettingsFallBackFlag   			//0x00001000
#else
		KImap4EmailSettingsLastUsedFlag				= KImap4EmailSettingsFolderSyncDisabled    		//0x00000400
#endif
		};

	TImImap4SettingsExtension*	iExtension; // renamed iReceiptAddress

	TInt32  iInboxSyncLimit;
	TInt32  iMailboxSyncLimit;

	TText8  iPathSeparator;
	TFolderSyncType iSynchroniseStrategy;
	TFolderSubscribeType iSubscriptionStrategy;
	TUint32	iMaxEmailSize;
	TImap4GetMailOptions iGetMailOptions;
	};


class TImImap4GetMailInfo
/**
Specifies options used when retrieving messages with IMAP.

An TImImap4GetMailInfo is passed as a packaged parameter 
to CImap4ClientMtm::InvokeAsyncFunctionL() when a message
getting command is issued.

Options include the maximum message size, and what 
message parts are required.  

@see CImImap4GetMail
@see TImap4Cmds

@publishedAll
@released
*/
	{
public:
	/** The maximum message size (bytes). 
	Messages of greater size are not fetched. */
	TInt32					iMaxEmailSize;
	/** The required message parts. */
	TImap4GetMailOptions	iGetMailBodyParts;
	/** The destination folder ID, specifying the
	target folder when messages are copied or moved. */
	TMsvId					iDestinationFolder;
	};

/**
Specifies message criteria that are used as a filter when retrieving messages partially.

Options include the maximum size for the whole message,
the maximum size for the body of the message,
the maximum size for the attachments with the message, and 
the message parts required.

An TImImap4GetPartialMailInfo is passed as a packaged parameter 
to CImap4ClientMtm::InvokeAsyncFunctionL() when a message
populate command is issued.

@see
CImImap4GetMail

@publishedAll
@released
*/

class TImImap4GetPartialMailInfo : public TImImap4GetMailInfo
	{
public:
	/** The maximum size limit for the total message (bytes). */
	TInt32		iTotalSizeLimit;
	/** The maximum size limit for body of the message (bytes). */
	TInt32		iBodyTextSizeLimit;
	/** The maximum size limit for the attachments  (bytes). */
	TInt32		iAttachmentSizeLimit;
	/** The required message parts. */
	TImImap4PartialMailOptions	iPartialMailOptions;
	};
	
inline CImImap4Settings::TImImap4SettingsExtension* CImImap4Settings::Extension() const
	{
	return iExtension;
	}

inline void CImImap4Settings::CheckExtensionExistsL()
	{
	if (!iExtension)
		iExtension=new (ELeave) CImImap4Settings::TImImap4SettingsExtension;
	}
inline CImImap4Settings::TImImap4SettingsExtension::TImImap4SettingsExtension() 
: iLoginName(NULL) ,iPassword(NULL) ,iFolderPath(NULL),iSearchString(NULL),iTlsSslDomain(NULL), iSyncRate(KImapDefaultInboxSyncRate), iImapIdleTimeout(KImapDefaultIdleTimeout), iFetchSizeBytes(KImapDefaultFetchSizeBytes),
	iPartialMailOptions(ENoSizeLimits),iBodyTextSizeLimit(KMaxTInt),iAttachmentSizeLimit(KMaxTInt)
	{
	}


#endif // __IMAPSET_H__
