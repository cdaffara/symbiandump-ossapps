/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CIpsPlgSosBasePlugin.
*
*/


#ifndef IPSPLGSOSBASEPLUGIN_H
#define IPSPLGSOSBASEPLUGIN_H

#include "CFSMailPlugin.h"
#include "MFSMailRequestObserver.h"
#include <rconnmon.h>
#include "ipsplgsosbaseplugin.hrh"
#include "ipsplgcommon.h"
#include "ipsplgsingleopwatcher.h"
#include "ipsplgtimeroperation.h"
//<Qmail>
#include "ipsstateextension.h"
//</Qmail>
class CMsvSession;
class CIpsPlgTimerOperation;
class CIpsPlgMsgMapper;
class CImEmailMessage;
class CIpsPlgMruList;
class CIpsPlgSettingsObserver;
class CRepository;
class CIpsPlgSearch;
// <qmail> CIpsSetDataApi removed
class CIpsPlgSmtpService;
class CIpsPlgSyncStateHandler;
class CIpsPlgEventHandler;

/**
 *  FreestyleIpsServices plugin class
 *
 *  @lib ipssosplugin.lib
 *  @since FSEmail 2.0
 */
NONSHARABLE_CLASS ( CIpsPlgSosBasePlugin ) :
    public CFSMailPlugin,
    public MIpsPlgSingleOpWatcher,
    public MFSMailRequestObserver, // a dummy observer
    public MIpsPlgTimerOperationCallBack,
    public MStateObserverCallback
    {
//<Qmail>
public: //from MStateObserverCallback
    void ActiveFolderChanged(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId);

public://from CExtendableEmail

    CEmailExtension* ExtensionL( const TUid& aInterfaceUid );
//</Qmail>
public:

    /**
    * ~CIpsPlgSosBasePlugin
    * Destructor
    */
    IMPORT_C virtual ~CIpsPlgSosBasePlugin();

public: //from MIpsPlgSingleOpWatcher

    /**
    *
    */
    void OpCompleted(
        CIpsPlgSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode );

public: //from MFSMailRequestObserver

    /**
    * no real implementation for this. we're just a dummy observer,
    * so we can internally run our own operations, like DisconnectL
    */
    virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

public: // from MIpsPlgTimerOperationCallBack

    /**
    * Called when activity timer fires
    */
    virtual void HandleTimerFiredL( const TFSMailMsgId& aMailboxId );

public: // from CFSMailPlugin

    virtual void SetMailboxName(
         const TFSMailMsgId& aMailboxId,
         const TDesC& aMailboxName );

    virtual TFSMailBoxStatus GetMailBoxStatus(
         const TFSMailMsgId& aMailBoxId );

    virtual TFSMailMsgId SpecifiedSendingMailbox();

    virtual void ListMailBoxesL( RArray<TFSMailMsgId>& aMailboxes);

    virtual CFSMailBox* GetMailBoxByUidL( const TFSMailMsgId& aMailBoxId);

    virtual void DeleteMailBoxByUidL(
        const TFSMailMsgId& aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );

    virtual TDesC& GetBrandingIdL( const TFSMailMsgId& aMailBoxId);

    virtual void MoveMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aMessageIds,
        const TFSMailMsgId& aSourceFolderId,
        const TFSMailMsgId& aDestinationFolderId );

    virtual TInt MoveMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aMessageIds,
        const TFSMailMsgId& aSourceFolderId,
        const TFSMailMsgId& aDestinationFolderId,
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );

    virtual void CopyMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aMessageIds,
        RArray<TFSMailMsgId>& aNewMessages,
        const TFSMailMsgId& aSourceFolderId,
        const TFSMailMsgId& aDestinationFolderId );

    virtual const TFSProgress GetLastSyncStatusL(
        const TFSMailMsgId& aMailBoxId );

    virtual TInt CancelSyncL( const TFSMailMsgId& aMailBoxId );

    // FOLDER HANDLING

    virtual CFSMailFolder* GetFolderByUidL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId );

    virtual CFSMailFolder* CreateFolderL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TDesC& aFolderName,
        const TBool aSync );

    virtual void DeleteFolderByUidL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId);

    // MESSAGE FETCH AND STORE

    virtual MFSMailIterator* ListMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TFSMailDetails aDetails,
        const RArray<TFSMailSortCriteria>& aSorting );

    virtual CFSMailMessage* GetMessageByUidL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailDetails aDetails);

    virtual void DeleteMessagesByUidL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const RArray<TFSMailMsgId>& aMessages );

//<qmail>
    virtual void DeleteMessagesByUidL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const RArray<TFSMailMsgId>& aMessages,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId);
//</qmail>

    // MESSAGE STORE OPERATIONS

    virtual CFSMailMessage* CreateMessageToSendL(
        const TFSMailMsgId& aMailBoxId );

// <qmail>
    /**
     * Creates new email message to message store asynchronously
     *
     * @param aMailBoxId msv entry id to mailbox which setting are used
     * @param aOperationObserver Observer for the operation
     * @param aRequestId Id of the operation
     */
    virtual void CreateMessageToSendL(
	    const TFSMailMsgId& aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
// </qmail>

    virtual CFSMailMessage* CreateForwardMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        const TDesC& aHeaderDescriptor );

// <qmail>
    /**
     * Creates a forward email message to message store asynchronously
     *
     * @param aMailBoxId msv entry id to mailbox which setting are used
     * @param aOriginalMessageId id of the forwarded message
     * @param aOperationObserver Observer for the operation
     * @param aRequestId Id of the operation
     */
    virtual void CreateForwardMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        const TDesC& aHeaderDescriptor = KNullDesC );
// </qmail>

    virtual CFSMailMessage* CreateReplyMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        const TBool aReplyToAll,
        const TDesC& aHeaderDescriptor );

// <qmail>
    /**
     * Creates new reply message to message store asynchronously
     *
     * @param aMailBoxId msv entry id to mailbox which setting are used
     * @param aOriginalMessageId id of the replied message
     * @param aReplyToAll reply to all recipients
     * @param aOperationObserver Observer for the operation
     * @param aRequestId Id of the operation
     */
    virtual void CreateReplyMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        const TBool aReplyToAll,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        const TDesC& aHeaderDescriptor );
// </qmail>

    virtual void StoreMessageL(
        const TFSMailMsgId& aMailBoxId,
        CFSMailMessage& aMessage );


    // <qmail>
    virtual void StoreMessagesL(
            const TFSMailMsgId& aMailBoxId,
            RPointerArray<CFSMailMessage> &messages,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId );
    // </qmail>

    virtual void GetMessagesL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const RArray<TFSMailMsgId>& aMessageIds,
        RPointerArray<CFSMailMessage>& aMessageList,
        const TFSMailDetails aDetails );

    virtual void ChildPartsL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentId,
        RPointerArray<CFSMailMessagePart>& aParts);

    virtual CFSMailMessagePart* NewChildPartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TFSMailMsgId& aInsertBefore,
        const TDesC& aContentType);

    virtual CFSMailMessagePart* NewChildPartFromFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TDesC& aContentType,
        const TDesC& aFilePath );

    // <qmail>
    virtual void NewChildPartFromFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TDesC& aContentType,
        const TDesC& aFilePath,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
    // </qmail>

    /**
     * Creates attachment based on file handle
     * @param aMailBoxId specifies the mailbox
     * @param aParentFolderId specifies the folder where message is
     * @param aMessageId specifies message
     * @param aParentPartId
     * @param aContentType mime type
     * @param aFile handle to open file
     */
    virtual CFSMailMessagePart* NewChildPartFromFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TDesC& aContentType,
        RFile& aFile );

    virtual CFSMailMessagePart* CopyMessageAsChildPartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TFSMailMsgId& aInsertBefore,
        const CFSMailMessage& aMessage);

    virtual void RemoveChildPartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TFSMailMsgId& aPartId);

    // <qmail>
    virtual void RemoveChildPartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aParentPartId,
        const TFSMailMsgId& aPartId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
    // </qmail>

    virtual CFSMailMessagePart* MessagePartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId);

    virtual TInt GetMessagePartFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId,
        RFile& aFileHandle,
        TBool aForWriting = EFalse);

    virtual void CopyMessagePartFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId,
        const TDesC& aFilePath);

    virtual void GetContentToBufferL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId,
        TDes& aBuffer,
        const TUint aStartOffset);

    virtual void SetContentL(
        const TDesC& aBuffer,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId);

    virtual void RemovePartContentL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const RArray<TFSMailMsgId>& aPartIds);

    virtual void SetPartContentFromFileL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aMessagePartId,
        const TDesC& aFilePath);

    virtual void StoreMessagePartL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aParentFolderId,
        const TFSMailMsgId& aMessageId,
        CFSMailMessagePart& aMessagePart);

    //<qmail>
    virtual void StoreMessagePartsL(
        RPointerArray<CFSMailMessagePart>& aMessagePart,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
    //<qmail>

    virtual void UnregisterRequestObserver( TInt aRequestId );

    // MESSAGE SENDING

    virtual void SendL(TFSMailMsgId aMessageId );

    virtual void SendMessageL( CFSMailMessage& aMessage );

    virtual void SendMessageL(
        CFSMailMessage& aMessage,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );

    // OPERATION HANDLING

    virtual void GoOnlineL( const TFSMailMsgId& aMailBoxId );

    virtual void GoOfflineL( const TFSMailMsgId& aMailBoxId );

    virtual TFSProgress StatusL(const TInt aRequestId);

    virtual void CancelL(const TInt aRequestId);

    // Search API

    virtual void SearchL(
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aFolderIds,
        const RPointerArray<TDesC>& aSearchStrings,
        const TFSMailSortCriteria& aSortCriteria,
        MFSMailBoxSearchObserver& aSearchObserver );

    virtual void CancelSearch( const TFSMailMsgId& aMailBoxId );

    virtual void ClearSearchResultCache( const TFSMailMsgId& aMailBoxId );

    virtual void AddObserverL(MFSMailEventObserver& aObserver);

    virtual void RemoveObserver(MFSMailEventObserver& aObserver);

    virtual void SubscribeMailboxEventsL(const TFSMailMsgId& aMailBoxId,
        MFSMailEventObserver& aObserver);

    virtual void UnsubscribeMailboxEvents(
        const TFSMailMsgId& aMailBoxId,
        MFSMailEventObserver& aObserver);

    virtual TSSMailSyncState CurrentSyncState(
        const TFSMailMsgId& aMailBoxId);

    virtual TInt WizardDataAvailableL( );

    /**
     * reads connection id from plugin
     *
     * @param  aConnectionId reference to connection id
     * @return KErrNone or error code
     */
     virtual TInt GetConnectionId( TFSMailMsgId aMailBoxId, TUint32& aConnectionId );

    /**
     * checks from plugin if connection is allowed when roaming
     *
     * @param  aConnectionAllowed
     * @return KErrNone or error code
     */
     virtual TInt IsConnectionAllowedWhenRoaming(
             TFSMailMsgId aMailBoxId,
             TBool& aConnectionAllowed );

    virtual void AuthenticateL(
        MFSMailRequestObserver& aOperationObserver,
        TInt aRequestId );

    virtual void SetCredentialsL(
       const TFSMailMsgId& aMailBoxId,
       const TDesC& aUsername,
       const TDesC& aPassword );
// <qmail>       
    virtual TUid Id( ) const;
// </qmail>
public:

    /**
    * Function to identify which protocol plugin we are working with
    * @return MTM id
    * @since FSEmail 2.0
    */
    TUid MtmId() const;

    /**
     * Returns the implementation ID of the plugin
     *
     * @return Implementation ID of the plugin
     * @since FSEmail 2.0
     */
   TUint PluginId() const;

    /**
     * Disconnects mailbox
     *
     * @param aMailbox unique mailbox id
     * @param FSObserver can be pointed at this class (dummy observer)
     * @param aRequestId FS async operation request id, can be zero (dummy)
     * @param aRemoveAccountAlso boolean to indicate should accound be
     *                           removed afted disconnect, default is EFalse
     * @since FSEmail 2.0
     */
    virtual void DisconnectL(
        const TFSMailMsgId& aMailBoxId,
        MFSMailRequestObserver& aObserver,
        const TInt aRequestId,
        TBool aRemoveAccountAlso = EFalse );

    /**
     * Fetch content of new mail, used in event handler
     * needed because imap idle
     *
     * @param aMailbox unique mailbox id
     * @param aNewId msv id of new mail
     * @param aParentId new mail's parent entry id
     * @since FSEmail 2.0
     */
    virtual void PopulateNewMailL(
        const TFSMailMsgId& aMailboxId,
        TMsvId aNewId,
        TMsvId aParentId ) = 0;

    /**
     * Gets reference to activity timer object, used when clearing
     * activity timer in operation complete function (online operation)
     * activity timer disconnects mailbox if no operations are running in
     * certain period of time
     *
     * @param aMailbox unique mailbox id
     * @return reference to mailbox's activity timer object
     * @since FSEmail 2.0
     */
    CIpsPlgTimerOperation& ActivityTimerL( const TFSMailMsgId& aMailBoxId );

    /*
     * Gets reference to sync state handler class
     * @return reference to sync state handler
     * @since FSEmail 2.0
     */
    CIpsPlgSyncStateHandler& GetSyncStateHandler();

    /**
     * This is called after msv session ready event comes from msv server
     * @since FSEmail 2.0
     */
    void CompleteConstructL();


    /**
     * called if session terminated event is sent by msv server
     * @since FSEmail 2.0
     */
    void SessionTerminated();

    /**
    * Does checks to see is connection attempt possible and allowed
    * @param aMailboxId mailbox to do the checks to
    * @param aReason (possible) reason code if EFalse is returned
    * @return EFalse if can't proceed with connect
    * @since FSEmail 2.0
    */
    TBool CanConnectL( const TFSMailMsgId& aMailboxId,
        TInt& aReason );

    /*
     * Gludge type check is plugin instance running under ui process
     * needed in event handler
     * @since FSEmail 2.0
     */
    TBool IsUnderUiProcess();

    // <qmail> new function
    /*
     * Checks whether given mailbox has ongoing operations or not
     * @param aMailboxId
     * @return true/false
     */
    TBool HasOperations( const TFSMailMsgId& aMailboxId );
    // </qmail>
protected:

    /*
     * Constructor
     * @param aFSPluginId uint of fs plugin implementation, imap or pop
     */
    CIpsPlgSosBasePlugin( const TUint aFSPluginId );

    /*
     * Called in subclasses ConstructL functions
     */
    void BaseConstructL();

    /**
     * passes completion event to protocol plugin
     * If certain operation completion requires special handling,
     * protocol plugin should keep the id of the operation
     * so it can know, that this certain operation has completed.
     */
    virtual void HandleOpCompletedL(
        CIpsPlgSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode ) = 0;

    /**
    * Sends pending messages from outbox, if user manually start
    * send-receive operation.
    */
    void EmptyOutboxL( const TFSMailMsgId& aMailBoxId );

    /**
    * Checks if offline mode is activated
    */
    TBool OfflineModeSetL();

    /**
    * Checks if given local feature is turned on
    */
    TBool CIpsPlgSosBasePlugin::LocalFeatureL(
        const TUid& aCenRepUid,
        const TUint32 aKeyId,
        const TUint32 aFlag );

    /**
    * Checks if we are roaming and are we allowed to
    * connect if we are.
    */
    TBool RoamingCheckL();

    /**
    * Check network registration status i.e. are we in gsm or wlan network
    */
    TInt RegistrationStatusL();

    /**
    * Called from Imap Plugin's MoveMessagesL if destination folder
    * is draft folder.
    */
    void MoveMessagesToDraftL(
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aMessageIds,
        const TFSMailMsgId& aSourceFolderId,
        const TFSMailMsgId& aDestinationFolderId );

    //<Qmail>
    /**
     * function to handle active folder changed events
     */
    virtual void HandleActiveFolderChangeL(
            const TFSMailMsgId& aActiveMailboxId,
            const TFSMailMsgId& aActiveFolderId);
    //</Qmail>
private:

    /**
     * Returns a cached entry objects or creates new objects and keeps them
     * in the cache.
     * Returned objects should not be deleted in the calling method.
     * Pointers are set NULL in the case of error.
     * Pointers are valid until the method is called next time
     * Method should be used only for entries representing messages (not
     * folders or attachements).
     */
    void GetMessageEntryL(
        TMsvId aId,
        CMsvEntry*& aMessageEntry,
        CImEmailMessage*& aImEmailMessage );

    /**
	 * Takes ownership of the cached objects or creates new ones
	 */
	void TakeMessageEntryLC(
		TMsvId aId,
        CMsvEntry*& aMessageEntry,
        CImEmailMessage*& aImEmailMessage );

    /**
     * Return objects to cache, deleting old ones in the cache if necessary
     */
    void ReturnMessageEntry(
        CMsvEntry* aMessageEntry,
        CImEmailMessage* aImEmailMessage );

    /*
     * Cleans up the cached messages entries that have accessed with
     * GetMessageEntryL()
     */
    void CleanCachedMessageEntries();

    /**
    * Cancel all online operations, use before disconnect
    */
    void CancelAllOnlineOperations( const TFSMailMsgId& aMailboxId );

    /**
    * Checks is there any connect operations running, meaning
    * that we are "connected" state
    */
    TBool ConnOpRunning( const TFSMailMsgId& aMailBoxId  );

    /**
    * Cancel, delete and remove operation from iOperations array.
    * Send Sync Completed event to plugin if operation is sync op
    */
    void DeleteAndRemoveOperation(
            const TInt aOpArrayIndex, TInt aCompleteCode );

    /**
     * Fixes the forward (or reply) message header parts (that are copied
     * to msg content) in case that caller has provided an own header
     * descriptor for these parts.
     */
    void FixReplyForwardHeaderL(
            CFSMailMessage* aMessage,
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aOriginalMessageId,
            const TDesC& aHeaderDescriptor );

private:

    /**
    * Maps symbian msv api's folder type to fs folder type
    */
    TFSFolderType GetFolderType(
            CMsvEntry* aEntry,
            TFSMailMsgId aFolderId );

    /**
    * Set folders to blocklist in online, offline or in both cases.
    * Operations (move/copy) allowed only between remote folders
    * (+IMAP inbox) in online mode and Outbox->Drafts
    */
    void BlockCopyMoveFromFoldersL(
        CMsvEntry* aFolderEntry,
        TFSMailMsgId aFolderId,
        CFSMailFolder& aFSMailFolder );

    /**
     * Set IMEI code to service's TEntry.iDescription
     */
    void StoreIMEIToMailboxL( const TMsvId aMailboxId );

    /**
     * Deletes the activity timer related to the specified mailbox, if it exists.
     * If no timer is found, method returns silently
     * @param aMailBoxId specifies the mailbox
     */
    void DeleteActivityTimer( const TFSMailMsgId& aMailBoxId  );

protected: // internal enumerations and data structures

    enum TSyncStatesInCenRep
        {
        ESyncFinishedSuccessfully = 103,
        ESyncError,
        ESyncCancelled
        };

private:

    CIpsPlgSosBasePlugin();

protected:

    // uint of fs plugin implementation, pop of imap
    const TUint iFSPluginId;

    // Symbian message server session
    CMsvSession* iSession;

    // Message mapper instance
    CIpsPlgMsgMapper* iMsgMapper;

    //array of operation watchers
    RPointerArray<CIpsPlgSingleOpWatcher>   iOperations;

    //array of activity timers
    RPointerArray<CIpsPlgTimerOperation>   iActivitytimers;

    // contains smtp protocol related functions
	CIpsPlgSmtpService* iSmtpService;

    // Cached Symbian message data structures
    CMsvEntry* iCachedEntry;
    CImEmailMessage* iCachedEmailMessage;

    // read and write most recent used address list
    CIpsPlgMruList* iMruList;

    // Search Engine
    CIpsPlgSearch*  iSearch;

    // <qmail> iSettingsApi removed

    // maps symbian events to fs events
    CIpsPlgEventHandler*    iEventHandler;

    // keeps track is sync started in some plugin instance
    // or in always online needed when mailbox status is asked
    CIpsPlgSyncStateHandler* iSyncStateHandler;

    RConnectionMonitor              iConMon;

    // flag indicates is msv session ready
    TBool iSessionOk;

    TBuf<KIpsPlgMaxPhoneIdLength>  iIMEI;

    // branding id i.e. "yahoo.com" or "google"
    HBufC*  iBrandingId;

    // flag indicates is instance under FSEmail.exe
    TBool iIsUnderUiProcess;

    //<Qmail>
    CIpsStateExtension*    iStateExtension;//owned
    //</Qmail>
	};

#endif /* IPSPLGSOSBASEPLUGIN_H */

// End of File
