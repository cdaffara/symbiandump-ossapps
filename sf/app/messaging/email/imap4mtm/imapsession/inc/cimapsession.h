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

#ifndef __CIMAPSESSION_H__
#define __CIMAPSESSION_H__

#include <e32std.h>
 
#include "cimaplistfolderinfo.h"
#include "cimapfetchresponse.h"
#include "cimapcommand.h"

#include "msocketconnectobserver.h"
#include "minputstreamobserver.h"
#include "moutputstreamobserver.h"
#include "cimapobservabletimer.h"

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	#include "cimapauthhelpers.h"
#endif

// Forward Declarations
class CImSendMessage;
class CImapServerGreetingInfo;
class CImapCapabilityInfo;
class CImapFolderInfo;
class CFetchBodyInfo;
class CImapFetchBodyResponse;
class CImapSettings;
class CImapMailStore;

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	class CImapAuthMechanismHelper;
#endif
/**
Detects and records overflows during calls to RBuf8::AppendFormat().
This class does nothing to resolve the overflow, but gives methods such as 
CImapSession::AppendAndGrowFormatL() the opportunity to detect the success 
of calling RBuf8::AppendFormat(), so that they can retry with a larger
buffer if an overflow occured.
@internalTechnology
@prototype
*/
class TImapOverflowDetector : public TDes8Overflow
	{
public:
	TImapOverflowDetector();
	TBool OverflowDetected();

private:
	// Implements TDes8Overflow
	void Overflow(TDes8& aDes);

private:
	TBool iOverflowDetected;
	};

/**
Represents a connected session with the IMAP server.
This class provides a method for each command that can be sent to the IMAP server.
Only one command may be run at a time.
@internalTechnology
@prototype
*/
class CImapSession : public CBase
				   , public MInputStreamObserver
				   , public MOutputStreamObserver
				   , public MImapTimerObserver
	{
public:
	enum TImapServerState
		{
		EServerStateNone,
		EServerStateNotAuthenticated,
		EServerStateAuthenticated,
		EServerStateSelected,
		};
		
	IMPORT_C static CImapSession* NewL(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, MInputStream& aInputStream, MOutputStream& aOutputStream);
	~CImapSession();

	IMPORT_C TImapServerState ServerState();
	IMPORT_C CImapFolderInfo* SelectedFolderInfo();
	IMPORT_C const CImapServerGreetingInfo& ServerGreetingInfo();
	IMPORT_C const CImapCapabilityInfo& CapabilityInfo();

	// Support for IDLE mode (Authenticated or Selected state required for IDLE)
	IMPORT_C TBool ImapIdleSupported() const;
	
	IMPORT_C void EnterIdleL(TRequestStatus& aStatus);
	IMPORT_C void WaitForIdleEvent(TRequestStatus& aStatus);
	IMPORT_C void DoneIdle(TRequestStatus& aStatus);
	
	// Support for reading unsolicited server events.
	// For use when IDLE mode is not available.
	IMPORT_C void WaitForServerEventL(TRequestStatus& aStatus);
			
	// Imap Commands
	
	//  > Establishing a Connection
	IMPORT_C void ReadServerGreetingL(TRequestStatus& aStatus);
	
	//  > Any State Commands
	IMPORT_C void CapabilityL(TRequestStatus& aStatus);
	IMPORT_C void NoopL(TRequestStatus& aStatus);
	IMPORT_C void LogoutL(TRequestStatus& aStatus);
	
	//  > Not-Authenticated State Commands
	IMPORT_C void LoginL(TRequestStatus& aStatus, const TDesC8& aUserName, const TDesC8& aPassword);
	IMPORT_C void StartTlsL(TRequestStatus& aStatus);
	
	//  > Authenticated State Commands
	IMPORT_C void SelectL(TRequestStatus& aStatus, CImapFolderInfo* aFolderInfo, TBool aSelectBox = ETrue);
	IMPORT_C void ExamineL(TRequestStatus& aStatus, CImapFolderInfo* aFolderInfo);
	IMPORT_C void CreateL(TRequestStatus& aStatus, const TDesC& aMailboxName);
	IMPORT_C void DeleteL(TRequestStatus& aStatus, const TDesC& aMailboxName);
	IMPORT_C void RenameL(TRequestStatus& aStatus, const TDesC& aExistingMailboxName, const TDesC& aNewMailboxName);
	IMPORT_C void SubscribeL(TRequestStatus& aStatus, const TDesC& aMailboxName);
	IMPORT_C void UnsubscribeL(TRequestStatus& aStatus, const TDesC& aMailboxName);
	IMPORT_C void ListL(TRequestStatus& aStatus, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList);
	IMPORT_C void LsubL(TRequestStatus& aStatus, const TDesC& aReferenceName, const TDesC& aMailboxName, RArrayImapListFolderInfo& aFolderList);
	IMPORT_C void StatusL(TRequestStatus& aStatus, const TDesC& aMailboxName, const TDesC8& aDataItemNames, CImapFolderInfo& aFolderInfo);
	IMPORT_C void AppendL(TRequestStatus& aStatus, CImSendMessage& aSource, const TDesC& aDestinationMailboxName);
	
	//  > Selected State Commands
	IMPORT_C void CloseL(TRequestStatus& aStatus);
	IMPORT_C void SearchL(TRequestStatus& aStatus, const TDesC8& aSearchCriteria, RArray<TUint>& aMatchingMessageIds);
	IMPORT_C void FetchFlagsL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, RArrayMessageFlagInfo& aOutFlagInfo);
	IMPORT_C void FetchBodyL(TRequestStatus& aStatus, TUint aMessageUid, TBool aPeek, CFetchBodyInfo& aFetchBodyInfo, CImapFetchBodyResponse& aFetchBodyResponse);
	IMPORT_C void FetchBodyStructureAndHeadersL(TRequestStatus& aStatus, TUint aMessageUid, const TDesC8& aFieldNames, CImapFetchResponse& aOutFetchResponse);
	IMPORT_C void FetchBodyStructureAndHeadersL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC8& aFieldNames, MImapFetchStructureObserver& aObserver);
	IMPORT_C void StoreL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC8& aMessageDataItem, const TDesC8& aValue, TBool aUseSilent, RArrayMessageFlagInfo& aOutMessageFlagInfo);
	IMPORT_C void CopyL(TRequestStatus& aStatus, const TDesC8& aSequenceSet, const TDesC& aDestinationMailboxName);
	IMPORT_C void ExpungeL(TRequestStatus& aStatus);

	IMPORT_C static HBufC8* CreateSequenceSetLC(RArray<TUint>& aMessageUids);
	
	IMPORT_C void Cancel();
	IMPORT_C void FlushCancelledCommand(TRequestStatus& aStatus);
	
	// Returns logger file identifier
	IMPORT_C TInt LogId() const;
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	IMPORT_C void AuthenticateL(TRequestStatus& aStatus,CImapAuthMechanismHelper::TImapAuthProfileFlag iCurrentAuthProfile);
#endif
	// Non-IMPORTED methods
	void FetchBodyOperationComplete(TInt aErrorCode);
	void AsyncSendFailed(TInt aErr);

	MInputStream* InputStream();
	MOutputStream* OutputStream();

private:
	/**
	Specifies which operation to perform in DoAsyncCallBackL()
	*/
	enum TAsyncCallBackOpCode
		{
		EAsyncCallBackOpNone,
		EAsyncCallBackOpProcessInputBuffer,
		};
	
	/**
	Specifies the high-level state of the session
	*/	
	enum TSessionState
		{
		/**
		Session running as normal.
		*/
		ESessionNormal,
		/**
		Cancel has been called and the command needs to be flushed.
		Session will return to a normal state after FlushCancelledCommand has completed successfully.
		*/
		ESessionFlushing,
		/**
		An unrecoverable error has occurred.  
		When entering this state, the input and output streams will be closed automatically.
		But the session object itself needs to be destroyed by its owner, and a new session started in its place.
		*/
		ESessionUnrecoverable,
		};
		
	/**
	Specifies any special processing that needs to be performed after a command completes
	*/	
	enum TPendingOperation
		{
		EOpNone,
		EOpServerGreeting,
		EOpSelect,
		EOpLogin,
		EOpLogout,
		EOpClose,
		EOpWaitForBodyOperationComplete,
		EOpIdleEnter,
		EOpIdleWait,
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		EOpAuthenticate,
#endif
		EOpIdleDone

		};
	
	CImapSession(CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, MInputStream& aInputStream, MOutputStream& aOutputStream);
	void ConstructL();

	void StartCommandL(TRequestStatus& aStatus, CImapCommand* aCommand);
	void RequestReadData();

	void ProcessInputBufferAsync();	
	void ProcessInputBufferL();
	TBool TryFindLiteralBlockL(TPtrC8& aDataToDeliver);
	TBool TryFindLineL(TPtrC8& aDataToDeliver);

	// Implement MInputStreamObserver
	void ReceivedDataIndL(const TDesC8& aBuffer);
	void SecureServerCnf();
	void InputStreamCloseInd(TInt aError);
	
	// Implement MOutputStreamObserver
	void SendDataCnf();
	void OutputStreamCloseInd(TInt aError);

	void CommandComplete();
	TBool CompleteIfStreamsClosed(TRequestStatus& aStatus);
	void CompleteAndDestroyCommand(TInt aCompletionCode, TBool aCloseStreams);
	void DoCancel();
	
	TBool DoPendingOperationForOk();
	void DoPendingOperationForFail();
	void DoPendingOperationForCancel();
	TBool DoPendingOperationForIntermediateResponse();

	// Implement MImapTimerObserver
	void OnTimerL(const CImapObservableTimer& aSourceTimer);

	// Managing iReport
	void Queue(TRequestStatus& aStatus);
	void Complete(TInt aStatus);
	void DoComplete(TInt& aStatus);
	
	// Async callback
	void RequestAsyncCallBack(TAsyncCallBackOpCode aAsyncCallBackOpCode);
	
	static TInt AsyncCallBack(TAny* aSelf);
	void AsyncCallBack();
	void DoAsyncCallBackL();
	
	// State Setters (includes logging)
	void SetSessionState(TSessionState aSessionState);
	void SetPendingOperation(TPendingOperation aPendingOperation);
	void SetServerState(TImapServerState aServerState);
	
	static void AppendAndGrowFormatL(RBuf8& aBuffer, TRefByValue<const TDesC8> aFormat, TImapOverflowDetector* aOverflowHandler, ...);
	
private:
	CImapSettings& iImapSettings;
	CImapMailStore& iImapMailStore;

	MInputStream* iInputStream;
	MOutputStream* iOutputStream;
	
	TPtrC8 iInputBuffer;
	CBufFlat* iInputCache;
	
	TImapServerState iServerState;
	
	TPendingOperation iPendingOperation;
	
	TAsyncCallBackOpCode iAsyncCallBackOpCode;
	CAsyncCallBack* iAsyncCallBack;
	
	// When flushing a cancelled command, 
	// timeout if the expected data is not received from the server in time.
	CImapObservableTimer* iFlushCommandTimeout;
	
	TSessionState iSessionState;
	
	/**
	Records information about the currently selected folder.
	This object is passed into CImapSession by SelectL() and ExamineL().
	CImapSession takes ownership of the object, and destroying the current object.
	CImapSession ensures that iSelectedFolderInfo is NULL whenever it is NOT in EServerStateSelected state.
	*/
	CImapFolderInfo* iSelectedFolderInfo;
	
	CImapCommand* iCurrentCommand; // Is NULL when no command is in progress.  OWNED.
	TInt iCommandLiteralRequestSize;
	
	TInt iNextTagId;
	
	// Capabilities
	CImapServerGreetingInfo* iServerGreetingInfo;
	CImapCapabilityInfo* iCapabilityInfo;
	
	TRequestStatus* iReport;

	TInt iLogId;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	CImapAuthMechanismHelper::TImapAuthProfileFlag iCurrentAuthProfile;
#endif
	};
								 
#endif // __CIMAPSESSION_H__
