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
// CImImap4Session header
// 
//

#if !defined(__IMAPSESS_H__)
#define __IMAPSESS_H__

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include <msvstd.h>
#include <miuthdr.h>
#include <imcvcodc.h>
#include <imcvsend.h>
#include "impsmtm.h"
#include <miutatch.h>
#include <cimcaf.h>

#include "imapio.h"
#include "fldindex.h"

// Forward declarations
class CMsvBodyText;
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CImQPDecode;
class CImImap4Settings;
class CImImap4Synchronise;
class CImImap4FolderSync;
class CImCalculateMsgSize;
class CImSendConvert;
class CImConvertCharconv;
class CCnvCharacterSetConverter;
class CImConvertHeader;
class CImapCancelTimer;
class MImapSessionObserver;

// States of ImapSession
/**
@internalComponent
@released
*/
enum TImapState
	{
	EImapStateDisconnected=0,

	EImapStateConnectWait,
	EImapStateGreetingWait,
	EImapStateCapabilityWait,
	EImapStateLoginSendUser,
	EImapStateLoginSendPassword, // 5
	EImapStateLoginWait,
	EImapStateNoSelect,
	EImapStateSelectWait,
	EImapStateSelected,
	EImapStateCreateWait,		// 10
	EImapStateDeleteWait,
	EImapStateDeleteAllWait,
	EImapStateDeleteFolderWait,
	EImapStateRenameWait,
	EImapStateSynchroniseWait,	// 15
	EImapStateFetchWait,
	EImapStateListWait,
	EImapStateLsubWait,
	EImapStateSubscribeWait,
	EImapStateUnsubscribeWait,	// 20
	EImapStateSeparatorWait,
	EImapStateCommandWait,
	EImapStateAppendSizeWait,
	EImapStateAppendPromptWait,
	EImapStateAppendWait,		// 25
	EImapStateAppendResultWait,
	EImapStateDeleteMarkWait,
	EImapStateExpungeWait,
	EImapStateExpungeAllWait,
	EImapStateCloseWait,		// 30
	EImapStateLogoutWait,
	EImapStateMoveEntryWait,
	EImapStateStartTLSWait,
	EImapStateSetSeenWait,
	EImapStateClearSeenWait,	// 35
	EImapStateSendLiteral,
	EImapStateIdleWait,
	EImapStateIdling,
	EImapStateStopIdleWait,
	EImapStateFetchCancelWait	// 40
	};

// States of a synchronise
/**
@internalComponent
@released
*/
enum TImapSyncState
	{
	ENotSyncing=0,

	ESyncOld,
	ESyncNew,
	EFetching,
	ESyncSearch,	
	ESyncListOld,	
	ESyncListNew,
	EGettingStructure
	};

// Modes updating Seen flag
enum TSeenFlagUpdateMode
	{
	EClearSeenFlag=0,
	ESetSeenFlag
	};


// Positive completion errors
const TInt KErrIMAPNO				=1;
const TInt KErrRFC2231Encoded		=2;

// IMAP text
_LIT(KIMAP_INBOX, "INBOX");
_LIT8(KIMAP_OK, "OK");

//TDes8 over flow handler
class TDes8OverflowHandler : public TDes8Overflow
/**
@internalComponent
@released
*/
	{
	public:
		void Overflow(TDes8&){return;} //nullifies USER:23 panic
	};

//cached TMsvEntry data
class TMsvCacheData
/**
@internalComponent
@released
*/
	{
public:
	TBool iOrphan;
	TUint32 iUid;
	};

// Remote directory structure
class CImImap4DirStruct : public CBase
/**
@internalComponent
@released
*/
	{
public:
	CImImap4DirStruct();
	~CImImap4DirStruct();

	void SetLeafnameL(const TDesC& aName);
	TPtrC Leafname();
	
private:
	HBufC*			iLeafname;

public:	
	TBool			iIsMailbox;
	TBool			iIsFolder;
	};

// Class which will defeat the timeout
class CIdleTimeoutTimer : public CTimer
/**
@internalComponent
@released
*/
	{
public:
	static CIdleTimeoutTimer* NewL(CImImap4Session& aOperation);
private:
	CIdleTimeoutTimer(CImImap4Session& aOperation);
	void RunL();
private:
	CImImap4Session& iOperation;
	};

class CImImap4SessionDummyRead : public CActive
/**
@internalComponent
@released
*/
	{
public:
	static CImImap4SessionDummyRead* NewL(CImImap4Session& aOperation, CImapIO& aImapIO, TInt aPriority);
	~CImImap4SessionDummyRead();
	void Start();

private:
	CImImap4SessionDummyRead(CImImap4Session& aOperation, CImapIO& aImapIO, TInt aPriority);
	void DoCancel();
	void RunL();

	CImImap4Session& iSession;
	CImapIO& iImapIO;
	};


class CImImap4SessionIdleRead : public CActive
/**
@internalComponent
@released
*/
	{
public:
	static CImImap4SessionIdleRead* NewL(CImImap4Session& aOperation, TInt aPriority);
	~CImImap4SessionIdleRead();
	void Start(TRequestStatus& aStatus);

private:
	CImImap4SessionIdleRead(CImImap4Session& aOperation, TInt aPriority);
	void DoCancel();
	void RunL();

	CImImap4Session& iOperation;
	TRequestStatus* iOperationStatus;
	};


// The session class
class CImImap4Session : public CMsgActive
/**
@internalComponent
@released
*/
	{
public: // functions
	// construct/destruct
	~CImImap4Session();
	static CImImap4Session *NewL(TInt aId, MImapSessionObserver& aObserver);
	static CImImap4Session *NewLC(TInt aId, MImapSessionObserver& aObserver);

	// Return the service settings of the current service
	CImImap4Settings* ServiceSettings();

	// Connect & disconnect to/from a server, and 'are we connected' status
	void ConnectL(TRequestStatus& aStatus, const TMsvId aService);
	void DisconnectL(TRequestStatus& aStatus);
	TBool Connected();
	TBool Busy();
	TBool IsCancelling() const;
	void CancelAndIdleL(TBool aReissueIdle);

	// Folder information/manipulation
	void ListL(TRequestStatus& aStatus, const TMsvId aFolder, CArrayPtr<CImImap4DirStruct>* aList);
	void LsubL(TRequestStatus& aStatus);
	void LocalSubscribeL(const TMsvId aTarget, const TBool aSubscribe);
	void RemoteSubscribeL(TRequestStatus& aStatus, const TMsvId aTarget, const TBool aSubscribe);
	void Create(TRequestStatus& aStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder);	
	void CreateL(TRequestStatus& aStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder);
	void Rename(TRequestStatus& aStatus, const TMsvId aTarget, const TDesC& aNewName);
	void RenameL(TRequestStatus& aStatus, const TMsvId aTarget, const TDesC& aNewName);
	
	void Delete(TRequestStatus& aStatus, const TMsvId aTarget);
	void Delete(TRequestStatus& aRequestStatus, const CMsvEntrySelection& aTargetSel);
	void DeleteL(TRequestStatus& aRequestStatus, const CMsvEntrySelection& aTargetSel);
	void DeleteAllMessagesL(TRequestStatus& aStatus);

	// Append to a folder
	void Append(TRequestStatus& aStatus, const TMsvId aEntry, const TMsvId aDestination);
	void AppendL(TRequestStatus& aStatus, const TMsvId aEntry, const TMsvId aDestination);

	// Folder selection
	void Select(TRequestStatus& aStatus, const TMsvId aFolder, const TBool aReadWrite);
	void SelectL(TRequestStatus& aStatus, const TMsvId aFolder, const TBool aReadWrite);

	// Post-folder selection commands
	void Synchronise(TRequestStatus& aStatus, const TBool aNewOnly);
	void SynchroniseL(TRequestStatus& aStatus, const TBool aNewOnly);
	void Copy(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination, TBool aUnSelectIfSameFolder);
	void CopyL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination, TBool aUnSelectIfSameFolder);
	void Close(TRequestStatus& aStatus, const TBool aExpunge);
	void CloseL(TRequestStatus& aStatus, const TBool aExpunge);
	void FetchBody(TRequestStatus& aRequestStatus, const TMsvId aPart,TImImap4GetPartialMailInfo aGetPartialMailInfo);

	void StartIdle(TRequestStatus& aRequestStatus);
	void StartIdleL(TRequestStatus& aRequestStatus);
	void StopIdle(TRequestStatus& aRequestStatus);
	void SyncStopIdleL(TRequestStatus& aRequestStatus);
	void StopIdleL(TRequestStatus& aRequestStatus);

	void DoIdleRead(TRequestStatus& aIdleReadStatus);
	void CancelIdleRead();
	void IdleReadError(TInt aError);
	void IdleTimerExpiredL();
	void CancelTimerExpired();

	// Progress
	void IncSyncStats(TImap4SyncProgress& aSync);
	TImap4GenericProgress Progress();
	void ResetStats();

	TBool FolderChanged() const;
	TBool ImapIdleSupported() const;
	TBool IsIdling() const;
	
	// Save msvserverentry that Mtm hands us
	void SetEntry(CMsvServerEntry *aEntry);

	// Log stuff
	void LogText(const TDesC8& aString);
	void LogText(TRefByValue<const TDesC8> aFmt,...);

	// Orphan a local message
	void OrphanMessageL(const TMsvId aMessage);

	// Delete a local message
	void DeleteMessageL(const TMsvId aMessage);

	// Copy a message, leaving the structure behind (with a new TMsvId)
	void CopyMessage(TRequestStatus& aStatus, const TMsvId iSourceFolder, const TMsvId iSource, const TMsvId iDestinationFolder, TMsvId* iNewSource, const TBool aRemoveOriginal);

	// Park entries
	void Park();

	void ReissueIdleL();
	void ReissueDummy();

	void DummyComplete(TInt aError);
	TInt CommandFailure() const;

	// Calculate the total size of the messages to be downloaderd in this selection.
	TInt CalculateDownloadSizeL(const CMsvEntrySelection& aSelection);

	// This selection should not be deleted during a synchronisation
	void SetSynchronisationSelectionL(CMsvEntrySelection& aSelection);

	void SetInbox(TMsvId aInbox);
	TMsvId GetInbox();
	
	// Only going to be set on the secondary session
	void SetPrimarySession(CActiveWrapper* aPrimarySession);
	
	// Returns Current TextServerSession 
	CImTextServerSession* GetTextServerSession();
		
private:	
	CImImap4Session(MImapSessionObserver& aObserver);
	void ConstructL(TInt aId);

	void RunL();
	void DoRunL();
	void DoComplete(TInt& aStatus);

	// The get more/send some calls
	void GetReply(const TBool aPartial);
	void SendMessageL(const TDesC8 &theMessage);
	void SendUntaggedMessageL(const TDesC8 &theMessage);
	void SendUntaggedMessageWithTimeoutL(const TDesC8 &theMessage, TInt aTimeout);
	void NewTag();
	void NewTagSent();
	
	void LostConnection(TInt aError);
	void IssueIdleRead();	

	void Fail(const TInt aError);
	void DoDisconnect();

	// Construct full mailbox/folder path from a TMsvId
	HBufC8* MakePathL(const TMsvId aTarget, const TBool aIncludeLeaf);

	// Cancel current operation
	void DoCancel();

	// convert to and from Modified UTF-7 if needed
	HBufC* DoUnModUTF7LC(TDesC8& aBuffer);

	// Enquote string if needed
	void DoQuoteL(HBufC8*& aBuffer);

	// Update last sync date on folder
	void SyncCompleteL();

	// Get children of a folder: ONLY the message ones, though
	void GetMessageChildrenL(const TMsvId aFolder, CMsvEntrySelection* aChildren);

	// Make sorted folder index
	void MakeSortedFolderIndexL(TBool aUseCachedEntryData=EFalse);

	// Show atom tree (debug only)
	void showtree(CImapAtom* aAtom,TInt aIndent);

	// Reset subscription flags (recursive)
	void ResetSubscriptionFlagsL(const TMsvId aFolder);

	// Start new synchronise phase
	void SynchroniseNewL();
	void SynchroniseNewL(const TUint32 aLowUid,const TUint32 aHighUid);

	// Process replies
	TInt ProcessGreetingL();
	TInt ProcessCommandReplyL();
	TInt ProcessContinuationResponse(CImapAtom *aAtom);
	TInt ProcessUntaggedL(CImapAtom* aAtom, const TBool aCancelled);
	TInt ProcessFetchL(const TUint aMsgnr, CImapAtom* aAtom);
	void ProcessListL(CImapAtom* aAtom);
	void ProcessLsubL(CImapAtom* aAtom);
	TBool ProcessFlagsL(CImapAtom* aAtom, TMsvEmailEntry& aMessage);
	void AppendExtendL(HBufC8** aBufferPtr, const TDesC8& aText, TBool aOnStack=ETrue);
	void ProcessAddressL(HBufC8** aBuffer, CImapAtom* aAtom);
	
	// processing address list using envelope
	void ProcessAddressListL(HBufC8** aBuffer, CDesCArray& aWhere, CImapAtom* aAtom);
	// proccessing address list without using envelope
	void ProcessAddressListL(CDesCArray& aWhere, HBufC8** aAddresses);

	void ProcessEnvelopeL(CImHeader* aHeader, TMsvEntry& aEntry, CImapAtom *aAtom);
	void ProcessHeaderExtraL(CImHeader* aHeader, CImMimeHeader* aMimeHeader, TMsvEmailEntry* aEntry, TPtrC8 aText);
	void ProcessSearchL(CImapAtom *aAtom);

	// generate a file name for an attachment based on its type
	void GetDefaultFilename(TDes& aName, const TMsvEmailEntry& aMessage, const CImMimeHeader* mime);

	void BuildTreeL(TMsvId aParent, CImapAtom *aAtom, const TDesC8& aPath,
		            const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments);
	void BuildTreeOneL(const TMsvId aParent, CImapAtom *aAtom, const TDesC8& aPath,
			           const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments);

	// Fetch helpers
	// AddFetchItem adds item and appropriate fetchable children to
	// the current fetch list
	void AddFetchItemL(TMsvId aPart, TImap4GetMailOptions aPartTypes, TBool& aHasTextParts);
	void AddFetchItemL(TMsvId aPart, TImImap4GetPartialMailInfo aGetPartialMailInfo, TBool& aHasTextParts);

	// FetchAnItem issues command to fetch a fetchable part
	void FetchAnItemL(const TMsvId aItem);

	// Decode received data
	void DecodeAndStoreL(const TPtrC8& aData, const TBool aEndOfStream);
	TInt WriteToAttachmentL(const TDesC8& aText);
	void WriteToBodyL(const TDesC8& aText);

	// Helper function to decode UUEncoded attachment in MIME emails
	TBool CheckUUEStartL(const TDesC8& aSourceLine);

	// Called when we get a tagged completion code
	void CommandCompleteL(TInt aResult);

	// Dummy read operation queueing
	void IssueDummy();
	void CancelDummy();

	// Set entry, leave if error (not related to setentry() which sets iEntry)
	void SetEntryL(const TMsvId aId);

	// Change entry, leave if error
	void ChangeEntryL(const TMsvEntry& aEntry);

	// Change entry, leave if error
	void ChangeEntryBulkL(const TMsvEntry& aEntry);
	// Get children, leave if error
	void GetChildrenL(CMsvEntrySelection& aSelection);

	// Scan content-type list for filename information, copy if necessary
	TInt FindFilename(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename);

	// call FindFilename, default if necessary and QP decode it
	void FindFilenameDecodeL(const CImMimeHeader& aMimeInfo, TFileName& aFilename);

	// Copy a message, leaving the structure behind (with a new TMsvId) - use CopyMessage() instead
	void CopyMessageL(TRequestStatus& aStatus, const TMsvId iSourceFolder, const TMsvId iSource, const TMsvId iDestinationFolder, TMsvId* iNewSource, const TBool aRemoveOriginal);

	// private leaving versions of public non-leaving versions
	void FetchBodyL(TRequestStatus& aStatus, const TMsvId aPart);
	void DeleteL(TRequestStatus& aStatus, const TMsvId aTarget);

	void DeleteEntryL( const TMsvId aTarget);

	// convenience function to fix up the colon-separated header list
	// into a space separated list
	void ColonSeparatorToSpace(TDes8& buf);
	void StripSpace(HBufC8* aBuf);

	void PropagateCompleteFlagL(TMsvId aId, TBool aDoBodyText, TBool aPartialFetched);
	void SendLoginL();
	void SendCapabilityL();
	TPtrC FileNameFromRFC2231(const TDesC8& aEncodedFileName);
	static void SetMessageFlagsL(TMsvEmailEntry& aMessageEntry, CImapAtom* aRootAtom);

	void FetchLargeHeaderL(TUint aUid, TBool aRange);  
	void FetchHeaderL(TUint aUid);
   
  	void DoFetchL();

	// Set or clear a set of message \Seen flags
	TBool ProcessSeenFlagsL(TSeenFlagUpdateMode aUpdateMode);

	// Search through all Sibling Atoms for an Attachment
	TBool DoesAtomContainAttachment(CImapAtom *aAtom);
	TInt32 Minimum(TInt32 aThisPartTypeSizeLimit,TInt32 aTotalMailSizeLimit);
	TInt32 GetFetchSizeL(TInt32 aSizeLeft, TInt32 aSizeDownLoaded);
	void AttachFooterInfoL();
	void CheckForDiskSpaceL(TInt aSizeToBeDownloaded);
	TInt32 FetchSize(TInt32 aMinimumLimit,TInt32 aSizeDownLoaded, TInt32 aSizeLeft);
	void CheckForPartialPopulate(TImImap4GetPartialMailInfo aGetPartialMailInfo);
	void RemoveHtmlPart(TMsvId aPart);
	void ProcessFooterMessageL(TInt aSizeLeft);
	
	void DoSynchroniseL(TBool aNewOnly);
	void DoStartIdleL();
	void DoStopIdleL();
	void DoDisconnectL();
	void CreateAttachmentInfoL(TMsvEmailEntry& aMsvEmailEntry);
	void CreateUidStringL();
	void DoSelectL(const TMsvId aFolder, const TBool aReadWrite);	
private:
	enum TImap4SecurityState
		{
		EUnknown,
		ENegotiating,
		ESecure,
		EUnsecure
		};
	
private:
	// Copy of the service's settings
	CImImap4Settings*		iServiceSettings;
	// Copy of the service's iap settings
	CImIAPPreferences*		iPrefs;

	// Our connection
	CImapIO*				iImapIO;

	// FS handle
	RFs						iFs;

	// Current state and line being processed
	TImapState				iState;
	TImapState				iSavedState;
	TBool					iSendQueued;
	TBool					iReceiveQueued;
	TBool					iGotWholeLine;
	TInt					iCommandsOutstanding;
	TBool					iSizeWait;

	// Generic space to store completion information about commands in progress
	TBool					iCommandFlags[4];
	TMsvId					iCommandIds[4];
	TBuf<256>				iCommandBuf;
	TInt					iCommandSize;

	// Synchronisation state
	TImapSyncState			iSyncState;
	TInt					iFolderPosition;
	TUint					iHighestUid;
	TInt32					iSyncLimit;

	// Current tag
	TInt					iTag;
	TInt					iCancelledTag;

	// Mailbox hierarchy
	TBuf8<1>				iHierarchySeparator;
	TBuf8<64>				iFolderPath;

	// List bits
	CArrayPtr<CImImap4DirStruct>* iList;

	// Seen IMAP4rev1 capability?
	// IMAP capabilities
	TBool					iSeenVersion;
	TBool					iCapabilityStartTLS;
	TBool					iCapabilityLoginDisabled;
	TBool					iCapabilityIdleSupport;

	TImap4SecurityState		iSecurityState;

	// Host/username/password
	TBuf<64>				iHost;
	TInt					iPort;
	HBufC8* 				iUsername;
	HBufC8* 				iPassword;
	TBool					iLiteralUsername;
	TBool					iLiteralPassword;

	// Mailbox state information from remote host
	TMsvId					iMailboxId;
	//TBool					iMailboxIsInbox;
	CImImap4FolderIndex		iFolderIndex;
	TBool					iMailboxReceivedExpunge;
	TBool					iMailboxReceivedExists;
	TBool					iMailboxReceivedFlags;
	TBufC8<128>				iMailboxName;
	TInt					iMailboxSize;
	TInt					iMailboxRecent;
	TBool					iMailboxWritable;
	TUint					iUidValidity;
	TUint					iUidNext;
	CArrayFix<TMsvId>*		iDeletedUids;
	TBool					iSomeUnread;

	CArrayFix<TUint32>*		iSearchList;
	TUint32					iMissingUidLow;
	TUint32					iMissingUidHigh;

	// Fetch in progress
	TMsvId					iMessageId;
	//TMsvId				iMessageDestination;
	TUint					iMessageUid;
	TUint					iMessageFetching;
	
	CBufBase*				iBodyBuf;
	CMsvBodyText*			iBodyText;
	CRichText*				iMessageBody;
	CParaFormatLayer*		iParaLayer;
	CCharFormatLayer*		iCharLayer;
	
	CArrayFix<TMsvId>*		iFetchList;
	TBool					iFetchIsText;
	TFileName				iAttachmentName;
	CImMimeHeader*			iAttachmentMimeInfo;

	TImAttachmentFile*		iAttachmentFile;
	TImAttachmentFileState	iAttachmentFileState;
	HBufC*					iAttachmentFullPath;
	TInt					iSizeOfThisPart;
	TInt					iDecodedSizeOfAllParts;
	TInt 					iSizeLeftToFetch;
	
	// Bool to indicate that we are in between of a UUEncoded start and end tag.
	TBool					iUUDecoding;
	
	// Decoder used for fetch in progres
	TImCodecB64				iB64Decoder;
	TImCodecUU				iUUDecoder;
	TImCodecQP				iQPDecoder;
	TImEncodingType			iEncodingType;
	HBufC8*					iPartialLine;
	HBufC*					iDefaultAttachmentName;

	// Characterset conversion
	CImConvertCharconv*		iCharConv;
	CCnvCharacterSetConverter* iCharacterConverter;
	CImConvertHeader*		iHeaderConverter;
	TBool					iPreparedToConvert;
	TBuf8<20>				iLeftOver;

	// Bits used when appending
	CImCalculateMsgSize*	iMessageSizer;
	CImSendMessage*			iMessageSender;
	TTime					iMessageDate;
	HBufC8*					iLineBuffer;

	// Tree of atoms
	CImapAtom*				iRootAtom;

	// Access to message database
	CMsvServerEntry*		iEntry;
	CMsvServerEntry*		iMoveEntry;
	TMsvId					iMoveSource;		// What we're moving
	TMsvId					iMoveSourceFolder;	// Where we're moving it from
	TMsvId*					iNewSource;			// Where we save the newly created TMsvId in the source folder
	CMsvEntrySelection*		iSelection;
	CMsvEntrySelection*		iSynchronisationSelection;

	// Dummy operation issued to CImapIO to notify us of connection problems?
	CImImap4SessionDummyRead*	iDummyRead;
	
	// Used to issue and handle responses for IMAP IDLE
	CImImap4SessionIdleRead* iIdleRead;

	TBool					iUseIdleCommand;
	CIdleTimeoutTimer*		iIdleTimer;
	TBool					iReissueIdle;
	TBool					iDisconnectAfterIdleStopped;
	TMsvId					iInbox;
	TBool 					iIdleTimerExpired;

	// Hideous kludge for brain-dead CC:Mail servers
	TBool					iTalkingToCCMail;
	TBool					iTalkingToOpenMail;

	TRequestStatus*			iRequestStatus;
	TUint					iCharset;

	TInt					iHeadersFetched;
	TInt					iOrphanedMessages;
	TInt					iRemoteMessagesDeleteTagged;
	TInt					iMsgsDone;
	TInt					iCheckDiskSpaceCounter;
	TInt					iCurrentDrive;

	CArrayFixFlat<TMsvCacheData>* iCachedEntryData;
	CArrayFixFlat<TMsvId>* iSetSeenList;
	CArrayFixFlat<TMsvId>* iClearSeenList;

 	TBool					iJustSentFetch;
 
 	// 
 	TImap4GetMailOptions	iGetOptions;
	TImImap4GetPartialMailInfo	iGetPartialMailInfo;
 	TMsvId					iGetPart;

	// If a Copy/Move/Delete fails with IMAP server. 
	// Store the fact. May need information.
	TInt					iCommandFailure;

	// bearer timeout value in microseconds
	TInt					iIdleTimeout;

	TInt					iStore8BitData;
	TBool					iFetchPartialMail;
	TInt32					iBodyTextSize;
	TInt32					iHtmlEntrySize;
	TInt32					iBodyPartRemainingSize;
	HBufC*					iFooterString;
	TBool					iIsDiskSpaceChecked;
	TMsvId					iHtmlEntryPart; 
	TInt32					iSizeOfToBeFetchedAttachments;
	// if set, the there is no IDLE reissued after a synch
	TBool					iCompoundStopIdle;
	CImCaf*					iCaf;

	TBool					iIsICalendar;
	TBool					iIsVCalendar;

	HBufC8*					iUidString;
	
	// test to see if the uid is present in the fetch response. If it is, then it is part
	// of the fetch command sent by the client. Otherwise it is a server message flag update,
	// so just process flags
	TBool iFoundUid;

	// Indicates that we are stopping idle at the request of the synchronise active object
	TBool iStoppingIdleForSync;

	// Observer for callback notifications from session
	MImapSessionObserver& iObserver;
	
	// Only going to be set on the secondary session to take ownership of primarysession
	CActiveWrapper*			iPrimarySession; 
	
protected:
	// Progress information
	TImap4GenericProgress	iProgress;

	// The service we're connected to
	TMsvId					iServiceId;
	// If we sent FETCH BODY MIME
	TBool					iParsedTime;
	CImapCancelTimer*		iCancelTimer;
	TBool					iCancelAndIdle;
	};

#if 0
class CImImapMoveWait: public CActive
	{
public:
	CImImapMoveWait();
	~CImImapMoveWait();
	inline void Start();
protected:
	void RunL();
	void DoCancel();
	};

inline void CImImapMoveWait::Start()
	{
	SetActive();
	}
#endif

#endif

