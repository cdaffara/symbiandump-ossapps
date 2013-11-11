/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Email Interface base implementation header file.
*
*/


#ifndef __BASEPLUGIN_H__
#define __BASEPLUGIN_H__

#include <e32std.h>
#include <e32base.h>

#include "CFSMailPlugin.h"
#include "mmrorganizer.h"

//<cmail>
#include "MsgStoreFolderUtils.h"
#include "MsgStoreMailBox.h"
#include "MsgStoreMailBoxObserver.h"
#include "MsgStoreMessage.h"
#include "MsgStoreObserver.h"
#include "DebugLogMacros.h"
//</cmail>

//<qmail> Entire CBasePlugin class is exported in 10.1
#include "baseplugindef.h"
//</qmail>

#include "BaseMrInfoObject.h"
#include "Map.h"


class CMsgStore;

class CBaseMrInfoProcessor;
class TDayOfWeekFtor;
class CFetchRequester;
class CMailboxInfo;
class CSearchHandler;
class HMailIterator;
class CDelayedOp;
class CDelayedOpsManager;
class MDelayedOpsManager;

//<qmail>
/**
* reply/forward message creation parameters
*/
struct TReplyForwardParams
    {
    HBufC* iHeader; // reply/forward header, ownership stays in UI
    HBufC* iSmartTag; // smart reply/forward tag, ownership stays in UI
    };
//</qmail>

/**
 * Notes for derived classes:
 * GetPluginId - you must override this to return the ECOM id of the plugin.
 */
class BASEPLUGIN_EXPORT CBasePlugin :
    public CFSMailPlugin,
    public MMsgStoreObserver,
    protected MMsgStoreMailBoxObserver

    {
    friend class HMailIterator;
    friend class CBaseMrInfoProcessor;
    friend class CMailboxInfo;
    friend class CSearchHandler;
    //there is a need for a better way to access internal plugin state by
    //private components.
    friend class CDelayedDeleteMessagesOp;
    friend class CDelayedSetContentOp;
    friend class CDelayedMessageStorerOp;
	// <qmail>
    friend class CDelayedAddNewOrRemoveChildPartOp;
    friend class CDelayedOp;
	// </qmail>
	protected:
    struct TOngoingFetchInfo;
    struct TCacheLine
        {
        CMsgStoreMessage* iMsg;
        CMsgStoreMessagePart* iBody;
        RPointerArray<CMsgStoreMessagePart> iMsgChildren;
        RPointerArray<CMsgStoreMessagePart> iBodyChildren;
        };

    public:
        static CBasePlugin* NewL();
        static CBasePlugin* NewLC();
        virtual ~CBasePlugin();
    protected:
         CBasePlugin();
         void ConstructL();

    // CFSMailPlugin //
    public:

        /**
         * This methoed must be called before destruction of
         * CBasePlugin derived class. To fix bug EIZU-7XVF2Z.
         */
         void DeleteDelayedOpsManager();
         virtual void ListMailBoxesL(
            RArray<TFSMailMsgId>& aMailboxes );

         virtual CFSMailBox* GetMailBoxByUidL(
            const TFSMailMsgId& aMailBox);

	     virtual void DeleteMailBoxByUidL(
	        const TFSMailMsgId& aMailBoxId,
	        MFSMailRequestObserver& aOperationObserver,
			const TInt aRequestId );

         virtual TDesC& GetBrandingIdL(
            const TFSMailMsgId& aMailboxId );

         virtual void MoveMessagesL(
            const TFSMailMsgId& aMailBoxId,
            const RArray<TFSMailMsgId>& aMessageIds,
            const TFSMailMsgId& aSourceFolderId,
            const TFSMailMsgId& aDestinationFolderId );

         virtual void CopyMessagesL(
            const TFSMailMsgId& aMailBoxId,
            const RArray<TFSMailMsgId>& aMessageIds,
            RArray<TFSMailMsgId>& aNewMessages,
            const TFSMailMsgId& aSourceFolderId,
            const TFSMailMsgId& aDestinationFolderId );

         void GoOnlineL(
            const TFSMailMsgId& aMailBoxId );

         void GoOfflineL(
            const TFSMailMsgId& aMailBoxId );

         const TFSProgress GetLastSyncStatusL(
            const TFSMailMsgId& aMailBoxId );

         void RefreshNowL(
            const TFSMailMsgId& aMailBoxId,
         	MFSMailRequestObserver& aOperationObserver,
         	TInt aRequestId,
         	const TBool aSilentConnection=EFalse );

         virtual CFSMailFolder* GetFolderByUidL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId );

         virtual CFSMailFolder* CreateFolderL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            const TDesC& aFolderName,
            const TBool aSync );

         virtual void DeleteFolderByUidL(
            const TFSMailMsgId& aMailBox,
            const TFSMailMsgId& aFolder );

         virtual void ListFoldersL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            RPointerArray<CFSMailFolder>& aFolderList );

         virtual void ListFoldersL(
            const TFSMailMsgId& aMailBoxId,
            RPointerArray<CFSMailFolder>& aFolderList );

         virtual TFSMailMsgId GetStandardFolderIdL(
            const TFSMailMsgId& aMailBoxId,
            const TFSFolderType aFolderType );

         virtual MFSMailIterator* ListMessagesL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            const TFSMailDetails aDetails,
            const RArray<TFSMailSortCriteria>& aSorting );

         virtual CFSMailMessage* GetMessageByUidL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            const TFSMailMsgId& aMessageId,
            const TFSMailDetails aDetails );

        /**
         * The delete is executed asynchronously. The base plugin will queue
         * the request and return immediately.
         */
         virtual void DeleteMessagesByUidL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            const RArray<TFSMailMsgId>& aMessages );

         virtual CFSMailMessage* CreateMessageToSendL(
            const TFSMailMsgId& aMailBox );
// <qmail>
        virtual void CreateMessageToSendL(
            const TFSMailMsgId& aMailBoxId,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId );
// </qmail>

         virtual CFSMailMessage* CreateForwardMessageL(
            const TFSMailMsgId& aMailBox,
            const TFSMailMsgId& aOriginal,
            const TDesC& aHeaderDescriptor );

         virtual CFSMailMessage* CreateReplyMessageL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aOriginalMessageId,
            const TBool aReplyToAll,
            const TDesC& aHeaderDescriptor );

         virtual void StoreMessageL(
            const TFSMailMsgId& aMailBoxId,
            CFSMailMessage& aMessage );

// <qmail>
         void StoreMessagesL(
                const TFSMailMsgId& aMailBoxId,
                RPointerArray<CFSMailMessage> &messages,
                MFSMailRequestObserver& aOperationObserver,
                const TInt aRequestId );
// </qmail>

// <qmail>
        virtual void StoreMessagePartsL(
               RPointerArray<CFSMailMessagePart>& aMessageParts,
               MFSMailRequestObserver& aOperationObserver,
               const TInt aRequestId);
// <//qmail>

         virtual void FetchMessagesL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aFolderId,
            const RArray<TFSMailMsgId>& aMessageIds,
            TFSMailDetails aDetails,
            MFSMailRequestObserver& aObserver,
            TInt aRequestId );

         virtual void FetchMessagePartsL(
            const TFSMailMsgId& aMailBoxId,
     		const TFSMailMsgId& aFolderId,
     		const TFSMailMsgId& aMessageId,
     		const RArray<TFSMailMsgId>& aMessagePartIds,
        	MFSMailRequestObserver& aOperationObserver,
			const TInt aRequestId,
        	const TUint aPreferredByteCount );

         virtual void GetMessagesL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
            const RArray<TFSMailMsgId>& aMessageIds,
            RPointerArray<CFSMailMessage>& aMessageList,
            TFSMailDetails aDetails );

         virtual void ChildPartsL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
			const TFSMailMsgId& aMessageId,
     		const TFSMailMsgId& aParentId,
     		RPointerArray<CFSMailMessagePart>& aParts );

         virtual CFSMailMessagePart* NewChildPartL(
            const TFSMailMsgId& aMailBoxId,
        	const TFSMailMsgId& aParentFolderId,
			const TFSMailMsgId& aMessageId,
     		const TFSMailMsgId& aParentPartId,
        	const TFSMailMsgId& aInsertBefore,
        	const TDesC& aContentType );

         virtual CFSMailMessagePart* CopyMessageAsChildPartL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
			const TFSMailMsgId& aMessageId,
        	const TFSMailMsgId& aParentPartId,
        	const TFSMailMsgId& aInsertBefore,
        	const CFSMailMessage& aMessage );

         virtual void RemoveChildPartL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
        	const TFSMailMsgId& aMessageId,
            const TFSMailMsgId& aParentPartId,
            const TFSMailMsgId& aPartId );
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

         virtual void SetPartContentFromFileL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
            const TFSMailMsgId& aMessageId,
            const TFSMailMsgId& aMessagePartId,
            const TDesC& aFilePath );

         virtual void StoreMessagePartL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
         	const TFSMailMsgId& aMessageId,
         	CFSMailMessagePart& aMessagePart );

         virtual CFSMailMessagePart* MessagePartL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId,
            const TFSMailMsgId& aMessageId,
            const TFSMailMsgId& aMessagePartId );

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
            const TDesC& aFilePath );

	     virtual void GetContentToBufferL(
	        const TFSMailMsgId& aMailBoxId,
	        const TFSMailMsgId& aParentFolderId,
	 		const TFSMailMsgId& aMessageId,
	 		const TFSMailMsgId& aMessagePartId,
	 		TDes& aBuffer,
	 		TUint aStartOffset );

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
            const RArray<TFSMailMsgId>& aPartIds );

	     virtual void SearchL(
	        const TFSMailMsgId& aMailBoxId,
			const RArray<TFSMailMsgId>& aFolderIds,
			const RPointerArray<TDesC>& aSearchStrings,
			const TFSMailSortCriteria& aSortCriteria,
			MFSMailBoxSearchObserver& aSearchObserver );

    	 virtual void CancelSearch(
    	    const TFSMailMsgId& aMailBoxId );

         virtual void ClearSearchResultCache(
            const TFSMailMsgId& aMailBoxId );

         virtual void AddObserverL(
            MFSMailEventObserver& aObserver );

         virtual void RemoveObserver(
            MFSMailEventObserver& aObserver );

         virtual void UnregisterRequestObserver(
            TInt aRequestId );

         virtual void SendMessageL(
            CFSMailMessage& aMessage );

         virtual TFSProgress StatusL(
            TInt aRequestId );

         virtual void CancelL(
            TInt aRequestId );

         virtual void SubscribeMailboxEventsL(
            const TFSMailMsgId& aMailboxId,
            MFSMailEventObserver& aObserver );

         virtual void UnsubscribeMailboxEvents(
            const TFSMailMsgId& aMailboxId,
            MFSMailEventObserver& aObserver );
//<qmail>
         virtual TSSMailSyncState CurrentSyncState(
            const TFSMailMsgId& aMailboxId );
//</qmail>
         virtual TFSMailBoxStatus GetMailBoxStatus(
            const TFSMailMsgId& aMailboxId );

         virtual TBool MailboxHasCapabilityL(
            TFSMailBoxCapabilities aCapability,
	 		TFSMailMsgId aMailBoxId );

	    /**
	     * Create a new ChildPart using a file path
	     */
	     virtual CFSMailMessagePart* NewChildPartFromFileL(
		    const TFSMailMsgId& aMailBoxId,
		    const TFSMailMsgId& aParentFolderId,
		    const TFSMailMsgId& aMessageId,
		    const TFSMailMsgId& aParentPartId,
			const TDesC& aContentType,
		    const TDesC& aFilePath );

	    /**
	     * Create a new ChildPart using a file handle
         *  NOTE 1: RFs.ShareProtected() must have been called already before calling this method.
         *  NOTE 2: The ownersip of the RFile handle (aFile) is transferred to this method.
	     */
	     virtual CFSMailMessagePart* NewChildPartFromFileL(
		    const TFSMailMsgId& aMailBoxId,
		    const TFSMailMsgId& aParentFolderId,
		    const TFSMailMsgId& aMessageId,
		    const TFSMailMsgId& aParentPartId,
			const TDesC& aContentType,
		    RFile& aFile );
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
         virtual TInt WizardDataAvailableL();

         virtual void AuthenticateL(
            MFSMailRequestObserver& aOperationObserver,
            TInt aRequestId );

         virtual void SetCredentialsL(
            const TFSMailMsgId& aMailBoxId,
            const TDesC& aUsername,
            const TDesC& aPassword );

         virtual TInt CancelSyncL(
        	const TFSMailMsgId& aMailBoxId );

         virtual void SetMailboxName(
            const TFSMailMsgId& /*aMailboxId*/,
            const TDesC& /*aMailboxName*/ );
        // <qmail>
        virtual TUid Id( ) const;
        // </qmail>

    // MMsgStoreObserver //
    public:
         virtual void SystemEventNotify(
            TMsgStoreSystemEvent aEvent );

         virtual void AccountEventNotify(
            TMsgStoreAccountEvent aEvent,
    		TInt32 aOwnerId,
            const TDesC& aName,
    		const TDesC& aNewName,
    		TMsgStoreId aMailboxId );

         virtual void ModificationNotify(
            TMsgStoreId aMailBoxId,
            TMsgStoreOperation aOperation,
            TMsgStoreContainerType aType,
            TUint32 aFlags,
            TMsgStoreId aId,
            TMsgStoreId aParentId,
            TMsgStoreId aOtherId );

    protected:

         CFSMailAddress* FetchEmailAddressL(
            CMsgStorePropertyContainer& aMessage,
            TUint aIdx );

         CMailboxInfo& GetMailboxInfoL(
            TMsgStoreId aId );

        void GetMailboxDisplayNameL(
            TMsgStoreId aId,
            RBuf& aDisplayName );
         // <qmail>
         virtual TUint GetPluginId() const;
         // </qmail>
        /**
         * Get signature string from protocol specific subclass.
         * Caller is responsible for memory of returned string.
         */
        virtual HBufC* GetSignatureL( const TFSMailMsgId& /* aMailBox */ )
        	{
        	return NULL;
        	}

        /**
         * Get reply-to address from protocol specific subclass.
         * Caller is responsible for memory of returned string.
         */
        virtual HBufC* GetReplyToAddressL( TFSMailMsgId /* aMailBox */ )
            {
            return NULL;
            }

        /**
         * Get MailBox address from protocol specific subclass.
         * Caller is responsible for memory of returned string.
         */
        virtual HBufC* GetMailBoxAddressL( TFSMailMsgId /*aMailBox*/ )
            {
            return NULL;
            }


         virtual void TranslateMsgStorePropsL(
            const TFSMailMsgId& aMailBoxId,
            CMsgStorePropertyContainer& aMessage,
            CFSMailMessagePart& aFsMsg,
            const TFSMailDetails aDetails = EFSMsgDataEnvelope );

        /**
         * Translates common message store meeting requests into Freestyle mr info
         * objects.
         * Sync plugins might be interested in extending the default behavior.
         */
         virtual void TranslateMsgStoreMrL(
            const TFSMailMsgId& aMailBoxId,
            CMsgStorePropertyContainer& aMessage,
            CMsgStorePropertyContainer& aCalendar,
            CFSMailMessagePart& aFsMsg );

        /**
         * Translates a message in Freestyle format to the common Message Store
         * format. Does not store the destination's properties in the msgstore.
         * The caller must call StorePropertiesL when necessary. This way writes
         * to the message store can be minimized.
         *
         * @param aSrc input/source Freestyle message.
         * @param aDst output/destination MessageStore message.
         */
         virtual void TranslateEmailFwMessageL(
            CFSMailMessagePart& aSrc,
            CMsgStoreMessagePart& aDst,
            TBool aInInbox );

         virtual void TranslateEmailFwMrL(
            MMRInfoObject& aSrc,
            CMsgStorePropertyContainer& aDst );

         void TranslateEmailFwAttendeeL(
            MMROrganizer& aSrc,
            RMsgStoreAddress& aDst );

        void DoListFoldersL(
            TFSMailMsgId aMailBoxId,
            TMsgStoreId aFolderId,
            TBool aRecursive,
            RPointerArray<CFSMailFolder>& aFolderList );

        void NotifyEventL(
            TMsgStoreId aMailBoxId,
            TMsgStoreId aId,
            TMsgStoreId aParentId,
            TFSMailEvent aFsEvent,
            TMsgStoreId aOtherId = KMsgStoreInvalidId );

        void NotifyGlobalEventL(
            TFSMailEvent aEvent,
            TFSMailMsgId aMailBox = TFSMailMsgId(),
            TAny* aParam1 = NULL,
            TAny* aParam2 = NULL,
            TAny* aParam3 = NULL );

         virtual void NotifyMailboxEventL(
                TFSMailEvent aEvent,
                TFSMailMsgId aMailBox,
                TAny* aParam1 = NULL,
                TAny* aParam2 = NULL,
                TAny* aParam3 = NULL );

        /**
         * Tries to find a body part of the specified content type. By default
         * looks for the text/plain part.
         * @return the main body text part or NULL if one can not be found.
         */
         CMsgStoreMessagePart* GetBodyPartL(
            CMsgStoreMessage& aMessage,
            const TDesC& aContentType = KFSMailContentTypeTextPlain() );

        /**
         * Creates a forward/reply message via copying the original, using sender(s) as
         * recipient(s).
         *
         * @param aHeaderDescriptor packaged TReplyForwardParams that provide the message
         * quote header and the smart reply label that needs to be inserted in case of a
         * truncated message.
         * @param aKeepAttachments true if the original message attachments are to be kept.
         */
         CFSMailMessage* CreateForwardReplyMessageL(
            const TFSMailMsgId& aMailBox,
            const TFSMailMsgId& aOriginal,
            const TBool aReplyToAll,
            const TDesC& aHeaderDescriptor,
            TBool aKeepAttachments,
            TBool aKeepMeetingRequestInfo = ETrue );

        /**
         * Recursively looks for the part with id aPartId in the part container
         * aParent.
         */
         CMsgStoreMessagePart* FindMessagePartL(
            CMsgStoreMessagePart& aParent,
            TMsgStoreId aPartId );

        /**
         * Get MsgStore account object for the specified mailbox id.
         */
         CMsgStoreAccount* GetAccountForMsgBoxL(
            const TFSMailMsgId& aMailboxId );

        /**
         * Create a MR reply message.
         */
         virtual CFSMailMessage* CreateMrReplyMessageL(
            const TFSMailMsgId& aMailBoxId,
            MMRInfoObject& aMeetingRequest,
            const TFSMailMsgId& aOriginalMessageId );

       /**
        * Applyed when an account was deleted for proper cleanup.
        */
         virtual void HandleMailboxDeleteL(
           const TFSMailMsgId& aMailboxId );


    protected:

        void SystemEventNotifyL(
            TMsgStoreSystemEvent aEvent );

        void AccountEventNotifyL(
            TMsgStoreAccountEvent aEvent,
    		TInt32 aOwnerId,
            const TDesC& aName,
    		const TDesC& aNewName,
    		TMsgStoreId aMailboxId );

        void ModificationNotifyL(
            TMsgStoreId aMailBoxId,
            TMsgStoreOperation aOperation,
            TMsgStoreContainerType aType,
            TUint32 aFlags,
            TMsgStoreId aId,
            TMsgStoreId aParentId,
            TMsgStoreId aOtherId );

        void TranslateMsgStoreDayOfWeek(
            TUint32 aDayOfWeekMask,
            TDayOfWeekFtor& aFtor );

        void TranslateEmailFwDayOfWeek(
            MRRecurrenceRule::TMRRecurrentDay aSrc,
            TUint32& aDst );

        void TranslateEmailFwRecipientsL(
            CMsgStoreMessagePart& aDst,
            const TDesC8& aDstProperty,
            RPointerArray<CFSMailAddress>& aRecipients );

        static void RemoveAllPropertiesL(
            CMsgStorePropertyContainer& aContainer,
            const TDesC8& aName );

         CMailboxInfo& RefreshMailboxCacheL( TMsgStoreId aMailBoxId );

        void UnsubscribeMailboxEventsL(
            const TFSMailMsgId& aMailboxId,
            MFSMailEventObserver& aObserver );

        /**
         * Children should overload to call CFSMailFolderBase::BlockMoveFromL()
         * and CFSMailFolderBase::BlockCopyFromL() for the CFSMailFolder
         */
        virtual void BlockCopyMoveFromFoldersL(
            TFSMailMsgId /*aFolderId*/,
            TFSFolderType /*aFolderType*/,
            CFSMailFolder& /*aFSMailFolder*/ )
            {
            // do nothing
            }

        void DoCancelSearchL(
                const TFSMailMsgId& aMailBoxId );

        void DoClearSearchResultCacheL(
                const TFSMailMsgId& aMailBoxId );

        /**
         * Helper method to avoid duplicated code
         */
	    CFSMailMessagePart* NewChildPartFromFilePathOrHandleL(
		    const TFSMailMsgId& aMailBoxId,
		    const TFSMailMsgId& aParentFolderId,
		    const TFSMailMsgId& aMessageId,
		    const TFSMailMsgId& aParentPartId,
			const TDesC& aContentType,
		    const TDesC& aFilePath,
		    TBool  aUseFileHandle,
		    RFile& aFile );

	    /**
	     * Called by RemovePartContentL for each part that needs its content removed.
	     */
	     virtual void HandleRemovePartContentL(
	    	CMsgStoreMessage& aMsg,
	    	CMsgStoreMessagePart& aPart );

	    /**
	     * When replying or forwarding a message determine the "To:" field's value.
	     */
	    void SetReplyOrForwardToFieldL( CMsgStoreMessage& newMsg );


       /**
         * Used to refresh the cached mailbox display name, which can be changed by the user.  The name
         * is cached along with other mailbox info so that we can tell when it's changed in the message
         * store and only notify the UI when such a change occurs.
         *
         * @param aMailBoxNameHasChanged - used to return to the caller if a change is detected.
         * @param aMailBoxId - the ID of the mailbox
         */
	    void RefreshCachedMailBoxDisplayNameL( TBool& aMailBoxNameHasChanged, const TMsgStoreId& aMailBoxId );

	     virtual void ReportRequestStatusL(
            TMsgStoreId aMailBox,
            TMsgStoreId aOtherId,
            TMsgStoreId aMsgId,
            TMsgStoreId aPartId,
            TBool aCanReportCompletion = EFalse );

        /**
	     * Finds the corresponding download request object for the part with id
	     * of aPartId identified by also the mailbox and the message ids.
	     */
	     TBool FindFetchRequestL(
	    	TMsgStoreId aMailBox,
	    	TMsgStoreId aOtherId,
	    	TMsgStoreId aMsgId,
	    	TMsgStoreId aPartId,
	    	CBasePlugin::TOngoingFetchInfo& aOngoingFetchInfo );

	     TBool DeleteFetchRequestForPart(
	    	TOngoingFetchInfo& aFetchInfo );

	     void DeleteFetchRequest(
	    	TOngoingFetchInfo& aFetchInfo );


    protected:
	     void ResetCache();

	     void ResetBodyCache();

	    /**
	     * This method retrieves the specified message store message from a
	     * cache or directly from the message store if not cached. If the
	     * message was retrieved from the message store it is put into the
	     * cache. The lifetime of the message is managed by the cache but if a
	     * plugin makes a modification that would make the cached message
	     * invalid it must call the ResetCache method.
	     *
	     * @param aMailBoxId
	     * @param aMsgId
	     */
	     CMsgStoreMessage* GetCachedMsgL(
	        TMsgStoreId aMailBoxId,
	        TMsgStoreId aMsgId );

	     CMsgStoreMessagePart* GetCachedBodyL(
	        TMsgStoreId aPartId );

	     RPointerArray<CMsgStoreMessagePart>& GetCachedMsgChildrenL();

	     RPointerArray<CMsgStoreMessagePart>& GetCachedBodyChildrenL();

	     void InvalidateCacheIfNecessary(
	        TMsgStoreId aId,
	        TMsgStoreId aParentId,
	        TMsgStoreId aOtherId );

	     virtual const TDesC& CBasePlugin::CalendarFileName() const;

    public:

	    /**
	     * Direct message sending that does not require having a FW message
	     * instance. The FW's SendMessageL method internally uses this method
	     * after translating the FW message to the MsgStore format.
	     */
	     void SendMessageL(
            CMsgStoreMailBox& aMailBox,
            CMsgStoreMessage& aMsg,
            const TTime& aSentTime );

	    /**
	     * The delayed ops manager is used to queue asynchronous processing of
	     * plugin requests.
	     */
	     MDelayedOpsManager& GetDelayedOpsManager();

    protected:

        CMsgStore* iMsgStore;
        RMap<TInt64, CMailboxInfo> iMailboxes;

        //does not actually own the observers.
        RPointerArray<MFSMailEventObserver> iObservers;
        //async fetch reqs.
        RPointerArray<CFetchRequester> iReqs;
        RPointerArray<CDelayedOp> iDelayedOpReqs;

        TCacheLine iCacheLine;

    private:
    protected:
        MDelayedOpsManager* iDelayedOpsManager;

        /**
         * Descriptor for an ongoing part(attachment) fetch operation.
         */
        struct TOngoingFetchInfo
        	{
            CFetchRequester* iRequest;
            TInt iPartIndex;
            TMsgStoreId iMessageId;
            TInt iRequestIndex;

            TOngoingFetchInfo() :
        		iRequest( NULL ),
        		iPartIndex( KErrNotFound ),
        		iMessageId( KMsgStoreInvalidId ),
        		iRequestIndex( 0 )
            	{ }
        	};

        __LOG_DECLARATION
    };


/**
 * There is mailbox-related state information that is used to bind
 * the FSEF representation and the MsgStore one. This entity groups
 * it in one place.
 */
NONSHARABLE_CLASS( CMailboxInfo ) : public CBase
    {
    public:
        CMailboxInfo( CBasePlugin* aBasePlugin )
            : iBasePlugin( aBasePlugin )
            {
            }

        virtual ~CMailboxInfo()
            {
            if ( iMailBox )
                iMailBox->RemoveObserverL( iBasePlugin );

            iObservers.Close();

            delete iMailBoxName;
            delete iMailBoxDisplayName;
            delete iBrandingId;
            delete iMailBox;
            };

        CMsgStoreMailBox& operator() ()
            {
            return *iMailBox;
            }

    public:
        CMsgStoreMailBox* iMailBox;
        HBufC* iMailBoxName;
        HBufC* iMailBoxDisplayName;
        HBufC* iBrandingId;
        //does not actually own the observers.
        RPointerArray<MFSMailEventObserver> iObservers;
        CBasePlugin* iBasePlugin;

        TMsgStoreRootFolders iRootFolders;
    };


/**
 * Asynchronous attachment/body fetch descriptor.
 */
/**@ move to separate unit.*/
NONSHARABLE_CLASS (CFetchRequester) : public CBase
    {
    friend class CBasePlugin;

    public:
        static CFetchRequester* NewL(
            CBasePlugin& aPlugin,
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& iMessageId,
            const RArray<TFSMailMsgId>& aMessages,
            MFSMailRequestObserver& aObserver,
            TInt aRequestId );

        virtual ~CFetchRequester();

    private:
        CFetchRequester(
            CBasePlugin& aPlugin,
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& iMessageId,
            MFSMailRequestObserver& aObserver,
            TInt aRequestId );
        void ConstructL( const RArray<TFSMailMsgId>& aMessages );

    private:
        CBasePlugin& iPlugin;
    public:
        RArray<TFSMailMsgId> iParts;
        MFSMailRequestObserver& iObserver;
        TFSMailMsgId iMailBoxId;
        TFSMailMsgId iMessageId;
        TInt iRequestId;

        __LOG_DECLARATION
    };

#endif // __BASEPLUGIN_H__
