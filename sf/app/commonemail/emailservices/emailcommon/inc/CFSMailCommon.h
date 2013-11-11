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
* Description:  email framework common definitions
*
*/


#ifndef __CFSMAILCOMMON_H
#define __CFSMAILCOMMON_H

// <qmail>
#include <e32std.h>
// </qmail>
#include "nmcommon.h" //NmId

/** folder type definitions */  
enum TFSFolderType
    {
    EFSInbox = 1,
    EFSOutbox,
    EFSDraftsFolder,
    EFSSentFolder,
    EFSDeleted,
    EFSOther
    };
    
/** email flags */  
enum TFSMsgFlag
    {
    EFSMsgFlag_Read = 1,                 // Message is read (or "seen") on the server
    EFSMsgFlag_Read_Locally = 2,         // Message is read on the client
    EFSMsgFlag_Low = 4,                  // Message has low priority
    EFSMsgFlag_Important = 8,            // Message has high priority
    EFSMsgFlag_FollowUpComplete = 16,    // The message follow-up is complete
    EFSMsgFlag_FollowUp = 32,            // Message is flagged (a flag is showing next to msg in Outlook)
    EFSMsgFlag_Attachments = 64,         // Message has attachments
    EFSMsgFlag_Multiple = 128,           // Message has more than one recipient
    EFSMsgFlag_CalendarMsg = 256,        // Message is a calendar message
    EFSMsgFlag_Answered = 512,           // The message was replied to
    EFSMsgFlag_Forwarded = 1024,         // The message was forwarded
    EFSMsgFlag_OnlyToMe = 2048,          // The message was sent only to this user
    EFSMsgFlag_RemoteDeleted = 4096,     // The message has been deleted on the server
    EFSMsgFlag_HasMsgSender = 8192,      // The message has one or more senders 
	// <qmail> Not using EFSMsgFlag_BodyTruncated </qmail>
    };

/** email list sorting options */  
enum TFSMailSortField
    {
    EFSMailDontCare,            // user accepts any sort order
    EFSMailSortByDate,          // sorting is done by date
    EFSMailSortBySender,        // sorting is done by sender
    EFSMailSortByRecipient,     // sorting is done by recipients
    EFSMailSortBySubject,       // sorting is done by subject
    EFSMailSortByPriority,      // sorting is done by priority
    EFSMailSortByFlagStatus,    // follow up and completed   
    EFSMailSortByUnread,        // sorting is based on unread
    EFSMailSortBySize,          // sorting is done by size
    EFSMailSortByAttachment     // sorting is based on if message has attachments
    };

/** email list sort order */  
enum TFSMailSortOrder
    {
    EFSMailDescending,
    EFSMailAscending    
    };

/** email list sort criteria definition */  
class TFSMailSortCriteria
    {
    public:
        TFSMailSortField iField;
        TFSMailSortOrder iOrder;
    };

/** mailbox status */
enum TFSMailBoxStatus
    {
    EFSMailBoxOffline = 1,
    EFSMailBoxOnline
    };

/** mailbox capabilities */
enum TFSMailBoxCapabilities
    {
    // Mailbox supports creating subfolders.
    // Not supported by UI in 1.0 release.
    EFSMBoxCapaCreateSubfolder = 1,
    
    // Mailbox supports responding to meeting requests.
    EFSMBoxCapaMeetingRequestRespond,
    
    // Mailbox supports meeting request creation.
    EFSMBoxCapaMeetingRequestCreate,
    
    // Mailbox supports meeting request update.
    EFSMBoxCapaCanUpdateMeetingRequest,     
    
    // Mailbox supports user folder manipulation (rename and delete).
    // Not supported by UI in 1.0 release.
    EFSMBoxCapaManipulateUserFolders,
    
    // Mailbox supports moving messages to and from user folders.
    // Not supported in 1.0 release. 
    // (NOT USED. To be defined in the future.)
    EFSMBoxCapaMoveMessageToUserFolder,
    
    // Mailbox can move messages in offline or handle go online itself
    EFSMBoxCapaMoveMessageNoConnectNeeded,
    
    // Mailbox supports moving messages between folders.
    EFSMBoxCapaMoveToFolder,
    
    // Mailbox supports copying messages between folders.
    // Not supported by UI in 1.0 release.
    EFSMBoxCapaCopyToFolder,
    
    // Mailbox supports smart reply feature.
    // Returned by Intellisync and ActiveSync. UI will fetch rest of message if not supported.
    EFSMBoxCapaSmartReply,
    
    // Mailbox supports smart forward feature.
    // Returned by Intellisync and ActiveSync. UI will fetch rest of message if not supported.
    EFSMBoxCapaSmartForward,
    
    // Mailbox handles itself integration to Symbian Messaging.
    // Not supported by any protocol in 1.0 release.
    //
    // (Should be returned for mailboxes that handle possible Symbian Messaging 
    // integration themselves. I.e. FW must not ask FS Integration MTM to replicate 
    // these mailboxes to Symbian Messaging. It is intended to prevent duplicate 
    // accounts in Symbian Messaging in the case when protocol plugin actually 
    // already has visible accounts in Symbian Messaging.)
    EFSMBoxCapaSymbianMsgIntegration,
    
    // Mailbox implements new email notifications itself.
    // Not supported by any protocol in 1.0 release.
    //
    // (EFSMBoxCapaNewEmailNotifications is intended to be defined for mailboxes that 
    // handle new email notifications some other way than relying to FW 
    // displaying them (e.g. via NCNList). If this is defined for a mailbox then 
    // FW will not monitor and display new email notifications for the mailbox.
    // The aim is to prevent possible duplicate notifications to user.)
    EFSMBoxCapaNewEmailNotifications,
    
    // Mailbox supports MRU feature.
    // Framework can check this before updating the list for optimization. Intellisync and ActiveSync need to set.
    // Capability is not checked in UI. Plugin just returns a null if there’s no MRU list and the UI doesn’t show anything.
    EFSMBoxCapaSupportsMRU,
     
    // Mailbox can be deleted.
    // Set by Intellisync and ActiveSync only if the server supports.
    EFSMBoxCapaCanBeDeleted,
        
    // Mailbox supports out of office feature.
    // Set by Intellisync and ActiveSync. Not used by UI -> can be deprecated?
    // EFSMBoxCapaSupportsOutOfOffice,
    
    // Mailbox supports setting OoO on/off.
    // Set by Intellisync and ActiveSync. Not used by UI -> can be deprecated?
    // EFSMBoxCapaOutOfOfficeSet,
    
    // Mailbox supports editing out of office.
    // Set by Intellisync and ActiveSync. Not used by UI -> can be deprecated?
    // EFSMBoxCapaOutOfOfficeEdit,
    
    // Mailbox supports saving to drafts automically when "create message to send" and 
    // "save message" (in framework api) are used.
    EFSMBoxCapaSupportsSaveToDrafts,
    
    // Remote Contact Lookup is supported via this mailbox.
    // Set by Intellisync and ActiveSync.
    EFSMBoxCapaSupportsRCL,
    
    // Mailbox supports followup. Email UI hides Follow Up flagging if mailbox does not support it.
    EFSMBoxCapaSupportsFollowUp,
    
    // Mailbox supports for "do you want to delete"prompt.
    EFSMBoxCapaSupportsDeletedItemsFolder,

    // Mailbox does not support editing original message with smart reply or forward
    EFSMBoxCapaReadOnlySmartQuote,

    // Mailbox supports sync
    EFSMBoxCapaSupportsSync,

    // Mailbox supports email reply-to feature
    EFSMBoxCapaSupportsReplyTo,

    // Mailbox supports tracking meeting request attendee status
    // only supported by IntelliSync
    EFSMBoxCapaMeetingRequestAttendeeStatus,

    EFSMBoxCapaMRRequiresReplyEmail,
    
    // Mailbox supports embedded reply / forward message feature.
    EFSMBoxCapaEmbeddedReplyForwardMessage,
    
    // Mailbox supports removing entry from calendar
    EFSMBoxCapaRemoveFromCalendar,

    // Mailbox supports attachments in meeting requests
    EFSMboxCapaSupportsAttahmentsInMR,
    
    // Mailbox supports saving of embedded messages
    EFSMboxCapaSupportsSavingOfEmbeddedMessages,
    
    // Mailbox supports smart editing feature.
    // Returned by Intellisync and ActiveSync. If defined, UI will fetch message body fully. 
    EFSMBoxCapaSmartEdit
    };

/** email details included in plugin responses */
enum TFSMailDetails 
    {
    EFSMsgDataIdOnly = 1,       // Doesn't get any data. The object just contains the ID.
    EFSMsgDataDate = 2,         // Get received date only. To be used when showing messages sorted by date.
                                // The rest would be retreived when needed but date is needed to build the list.
    EFSMsgDataSubject = 4,      // Like above but when sorting by subject
    EFSMsgDataSender = 8,       // Likewise for sender address.
    EFSMsgDataEnvelope = 16,    // Date, subject, Sender, To, Cc at least.
    EFSMsgDataStructure = 32    // the part structure including mime type, size and name headers.
    };

    enum TFSPartFetchState
    {
    EFSDefault,
    EFSNone,
    EFSPartial,
    EFSFull,
    EFSEmailStructureUnknown
    };

/** class for message part / message / folder / mailbox id */
class TFSMailMsgId
    {
    public:

    inline TFSMailMsgId( ) // constructs NULL id
    { iPluginId.iUid = 0; iId = 0; iNullId = ETrue; iSeparator = EFalse; };
    inline TFSMailMsgId(TUid aPluginId, TUint aId)
    { iPluginId = aPluginId; iId = aId; iNullId = EFalse; iSeparator = EFalse; };
    inline TFSMailMsgId(TUint aPluginId, TUint aId)
    { iPluginId.iUid = aPluginId; iId = aId; iNullId = EFalse; iSeparator = EFalse; };
// <qmail>
  	inline TFSMailMsgId(NmId aId)
	{ iPluginId.iUid = aId.pluginId32();
	  iId = aId.id32();
	  (aId.id() == 0) ? iNullId = ETrue : iNullId = EFalse;
	  iSeparator = EFalse;
	};
// </qmail>
    inline TBool operator==(TFSMailMsgId aId) const
    { if(iNullId != aId.IsNullId()) return EFalse;
      if(iPluginId.iUid != aId.PluginId().iUid) return EFalse;
      if(iId != aId.Id()) return EFalse; return ETrue; };
    inline TBool operator!=(TFSMailMsgId aId) const
    { if(iNullId != aId.IsNullId()) return ETrue;
      if(iPluginId.iUid != aId.PluginId().iUid) return ETrue;
      if(iId != aId.Id()) return ETrue; return EFalse; }
    inline TBool IsNullId() const { return iNullId; };
    inline void  SetNullId() { iPluginId.iUid = 0; iId = 0; iNullId = ETrue; } ;
    inline TUid PluginId() const { return iPluginId; };
    inline TUint Id() const { return iId; };
    inline void  SetPluginId(TUid aPluginId) { iPluginId = aPluginId; iNullId = EFalse; };
    inline void  SetId(TUint aId) { iId = aId; iNullId = EFalse; };
    inline void SetSeparator( TBool aSeparator ) { iSeparator = aSeparator; }
    inline TBool IsSeparator() const { return iSeparator; }
// <qmail>
  	inline NmId GetNmId()
  	{
    	NmId nmid;
    	if( !iNullId ) {
      		nmid.setId32(iId);
      		nmid.setPluginId32(iPluginId.iUid);
    	}
    	return nmid;
  	};
// </qmail>

    protected:

    TBool   iNullId;
    TUid    iPluginId;      // ecom implementation id
    TUint   iId;            // message, folder, mailbox id
    TBool   iSeparator;     // if object is separator
    };

/* Predefined constants for ContentTypes and parameters for ContentType and ContentDisposition */
_LIT(KFSMailContentTypeMultipartMixed, "multipart/mixed");
_LIT(KFSMailContentTypeMultipartAlternative, "multipart/alternative");
_LIT(KFSMailContentTypeMultipartDigest, "multipart/digest");
_LIT(KFSMailContentTypeMultipartParallel, "multipart/parallel");
_LIT(KFSMailContentTypeMultipartRelated,  "multipart/related");
_LIT(KFSMailContentTypeTextPlain, "text/plain");
_LIT(KFSMailContentTypeTextHtml, "text/html");
_LIT(KFSMailContentTypeMessage, "message/rfc822");
_LIT(KFSMailContentTypeParamName, " name=");
_LIT(KFSMailContentTypeParamCharset, " charset=");
_LIT(KFSMailContentTypeTextCalendar, "text/calendar");


_LIT(KFSMailContentDispAttachment, "attachment");
//<qmail>
_LIT(KFSMailContentDispInline, "inline");
//</qmail>
_LIT(KFSMailContentDispParamFilename, " filename=");

/** mailbox synchronization states */
enum TSSMailSyncState // Are all of these sensible as mailbox sync states (e.g. PasswordVerified)
    {
    Idle,
    StartingSync, // establishing connection to server
    DataSyncStarting,
    EmailSyncing, // For any email folder not explicitly listed.
    InboxSyncing, // System folders the plugin may report. Can use just EmailSyncing instead.
    OutboxSyncing,
    SentItemsSyncing,
    DraftsSyncing,
    CalendarSyncing,
    ContactsSyncing,
    TasksSyncing,
    NotesSyncing,
    FilesSyncing,
    FinishedSuccessfully,
    SyncError,
    SyncCancelled,
    PasswordExpiredError,
    PasswordVerified,
    PasswordNotVerified,
    PushChannelOff,
    PushChannelEstablished,
    PushChannelOffBecauseBatteryIsLow,
    OutOfDiskSpace,
    ServerConnectionError // <qmail> new error code for sync
    };

/**
 *  email framework exception event response callback
 *
 *  @since S60 v3.1
 */
class MFSMailExceptionEventCallback
    {
    public:

    /**
     * Exception event resonse to plugin from FSMailServer after it has
     * processed the event (shown the corresponding dialog to user).
     * There can be several messages on the screen, but only one
     * of each message type per mailbox. So mailbox-message type
     * pairs should be enough to identify the responded event.
     *
     * @param aMailboxId Mailbox that sent the exception event
     * @param aEventType Event type of the responded event
     *                   (as TFsEmailNotifierSystemMessageType)
     * @param aResponse ETrue if user selected OK, Continue, etc. (LSK)
     *                  EFalse if user selected Cancel (RSK)
     */
     virtual void ExceptionEventCallbackL( TFSMailMsgId aMailboxId, TInt aEventType, TBool aResponse ) = 0;

     };


/** plugin / message store events definitions

Note! Store related events (backup/restore, store locked,
wiped, authenticated/unauthenticated) would benefit from
having a concept of plugin/service or store available in client API.
These have been simplified behind only two events:
TFSMailboxUnavailable and TFSMailboxAvailable.
*/
    enum TFSMailEvent
        {
        // Mailbox is unavailable
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSMailboxUnavailable,

        // Mailbox is available
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSMailboxAvailable,

        // New mailbox created
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventNewMailbox, 

        // New mailbox creation failure
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxCreationFailure, 

        // Mailbox renamed
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxRenamed,
        
        // Mailbox deleted
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxDeleted,

        //
        // Following events need to be subscribed per mailbox
        //

        // Mailbox capability changed
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxCapabilityChanged,

        // Mailbox settings changed (OPEN: can this replace TFSEventMailboxCapabilityChanged?)
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxSettingsChanged,

        // Mailbox went online
        // aParam1: NULL
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxOnline,

        // Mailbox went offline
        // <qmail>
        // aParam1: TInt possible errorCode.
        //          Plugin may specify an error code here if it sees that connection was cut off unexpectedly;
        //          Client can then have a separate handling for this exception.
        //          KErrDisconnected should be used in situations where network coverage was dropped off unexpectedly.
        // </qmail>
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxOffline,

        // Mailbox sync state changed
        // aParam1: TSSMailSyncState* newState
        // aParam2: NULL
        // aParam3: NULL
        TFSEventMailboxSyncStateChanged,

// <qmail>
        // New mails created
        // aParam1: RArray<TFSMailMsgId>* aNewEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventNewMail, 

        // Mails changed
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventMailChanged,
        
        // Mails deleted
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventMailDeleted,

        // Mails moved
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aNewParentFolder
        // aParam3: TFSMailMsgId* aOldParentFolder
        TFSEventMailMoved,

        // Mails copied
        // aParam1: RArray<TFSMailMsgId>* aNewEntries
        // aParam2: TFSMailMsgId* aNewParentFolder
        // aParam3: TFSMailMsgId* aOldParentFolder
        TFSEventMailCopied,

        // New folders created
        // aParam1: RArray<TFSMailMsgId>* aNewEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventNewFolder, 

        // Folders changed
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventFolderChanged,
        
        // Folders deleted
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aParentFolder
        // aParam3: NULL
        TFSEventFoldersDeleted,

        // Folders moved
        // aParam1: RArray<TFSMailMsgId>* aEntries
        // aParam2: TFSMailMsgId* aNewParentFolder
        // aParam3: TFSMailMsgId* aOldParentFolder
        TFSEventFoldersMoved,
// </qmail>

        // exception / error happened 
        // aParam1: TInt (TFsEmailNotifierSystemMessageType) aEventType
        // aParam2: TDesC* aCustomMessage or NULL (optional)
        // aParam3: MFSMailExceptionEventCallback* aCallback or NULL (optional)
        TFSEventException,
        
        // Mail deleted from viewer
        // aParam1: RArray<TFSMailId>* aEntries
        // aParam2: TFSMailId* aParentFolder
        // aParam3: NULL
        TFSEventMailDeletedFromViewer,
        };
        
/**
 * Progress data structure to indicate different kind of progress
 * information coming from plugins to user. Usually user uses
 * progress events to update progress bar in user interface.
 *
 * Download progress indication : 
 *   - first event iProgressStatus = EFSStatus_Started
 *   - next event iProgressStatus = EFSStatus_Status
 *   - last event iProgressStatus = EFSStatus_RequestComplete
 *   - iMaxCount downloadable object total size
 *   - iCounter currently downloaded object size
 *   - iError KErrNone
 *
 * Download request cancelled by user or by plugin :
 *
 *   - iProgressStatus = EFSStatus_RequestCancelled
 *   - iMaxCount & iCounter optional, latest values or ignore/unknown = -1
 *   - iError = symbian error code KErrCancel
 *
 * Download error :
 *
 *   - iProgressStatus = EFSStatus_RequestCancelled
 *   - iMaxCount & iCounter optional, latest values or ignore/unknown = -1
 *   - iError symbian error code
 * 
 * Connection establisment progress :
 *
 *   - iProgressStatus ; EFSStatus_Waiting, EFSStatus_Connecting,
 *     EFSStatus_Connected, EFSStatus_Authenticating, EFSStatus_Authenticated
 *     depending how plugin implements this
 *   - iMaxCount & iCounter optional, latest values or ignore/unknown = -1
 *   - iError symbian error code, KErrNone default
 */
class TFSProgress
    {

public:

    /** possible progress indication states  */
    enum TFSProgressStatus
    {
        EFSStatus_Waiting = 1,
        EFSStatus_Started,
        EFSStatus_Connecting,
        EFSStatus_Connected,
        EFSStatus_Authenticating,
        EFSStatus_Authenticated,
        EFSStatus_Status,
        EFSStatus_RequestComplete,
        EFSStatus_RequestCancelled,
    };

public: // data
                
    /** event progress / completion status*/
    TFSProgressStatus   iProgressStatus;
                    
    /** max value in progress countingc   */
    TInt                iMaxCount;

    /** progress counter                  */
    TInt                iCounter;

    /** error code (Symbian error codes)  */
    TInt                iError;

// <qmail>	
	/** Related data parameter (used for async message creation) **/	
    TAny* iParam;
// </qmail>
    };
    
// <qmail> Protocol plugins leave with this if API is not implemented
const TInt KErrFSMailPluginNotSupported = KErrNotSupported - 1000;
// </qmail>

#endif // __CFSMAILCOMMON
