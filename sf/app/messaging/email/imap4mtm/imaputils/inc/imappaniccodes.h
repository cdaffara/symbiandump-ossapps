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

#if !defined(__IMAPPANICCODES_H__)
#define __IMAPPANICCODES_H__

#include <e32base.h>

/**
@internalComponent
@prototype
*/
class TImapServerPanic
	{
public:
	enum TImpsPanic
		{
		//Panics for Imapsession dll(0-999)
		EIdleWhenServerUnAuthenticated = 0,
		EIdleWhenInvalidOperationPending = 1,

		EIdleWaitWhenServerUnAuthenticated = 2,
		EIdleWaitWhenOperationPendingNotIdle = 3,
		EIdleWaitWhenSessionNonNormal = 4,
		
		EIdleDoneWhenServerUnAuthenticated = 5,
		EIdleDoneWhenWhenInvalidOperationPending = 6,
		EIdleDoneWhenSessionNonNormal = 7,

		EWaitForServerEventWhenServerUnAuthenticated = 8,
		EWaitForServerEventWhenInvalidOperationPending = 9,

		EReadServerGreetingWhenServerUnAuthenticated = 10,
		EReadServerGreetingWhenInvalidOperationPending = 11,

		ECapabilityWhenServerStateUnknown = 12,
		ECapabilityWhenInvalidOperationPending = 13,

		ENoopWhenServerStateUnknown = 14,
		ENoopWhenInvalidOperationPending = 15,
		ENoopOutputBufferNotNull = 16,

		ELogOutWhenNotSelected = 17,
		ELogOutWhenInvalidOperationPending = 18,

		ELoginWhenServerStateUnknown = 19,
		ELoginWhenInvalidOperationPending = 20,
		ELoginResponseParseStateUnknown = 21,

		EStartTLSWhenServerUnAuthenticated = 22,
		EStartTLSWhenInvalidOperationPending = 23,

		ESelectWhenServerUnAuthenticated = 24,
		ESelectWhenInvalidOperationPending = 25,

		EExamineWhenServerUnAuthenticated = 26,
		EExamineWhenInvalidOperationPending = 27,

		ESessionImapFolderInfoIsNull = 28,

		ECreateWhenServerUnAuthenticated = 29,
		ECreateWhenInvalidOperationPending = 30,
		
		EDeleteWhenServerUnAuthenticated = 31,
		EDeleteWhenInvalidOperationPending = 32,
		
		ERenameWhenServerUnAuthenticated = 33,
		ERenameWhenInvalidOperationPending = 34,

		ESubscribeWhenServerUnAuthenticated = 35,
		ESubscribeWhenInvalidOperationPending = 36,
		
		EUnSubscribeWhenServerUnAuthenticated = 37,
		EUnSubscribeWhenInvalidOperationPending = 38,
		
		EListWhenServerUnAuthenticated = 39,
		EListWhenInvalidOperationPending = 40,
		
		ELsubWhenServerUnAuthenticated = 41,
		ELsubWhenInvalidOperationPending = 42,

		EStatusWhenServerUnAuthenticated = 43,
		EStatusWhenInvalidOperationPending = 44,
		
		EAppendWhenServerUnAuthenticated = 45,
		EAppendWhenInvalidOperationPending = 46,
		
		ECloseWhenNotSelected = 47,
		ECloseWhenInvalidOperationPending = 48,

		ESearchWhenNotSelected = 49,
		ESearchInvalidOperationPending = 50,
		
		EFetchFlagsWhenNotSelected = 51,
		EFetchFlagsInvalidOperationPending = 52,
		
		FetchBodyStructureSingleWhenNotSelected = 53,
		FetchBodyStructureSingleWhenInvalidOperationPending = 54,
		
		FetchBodyStructureMultiWhenNotSelected = 55,
		FetchBodyStructureMultiWhenInvalidOperationPending = 56,
		
		EFetchBodyWhenNotSelected = 57,
		EFetchBodyWhenInvalidOperationPending = 58,
		
		EStoreWhenNotSelected = 59,
		EStoreWhenInvalidOperationPending = 60,
		
		ECopyWhenNotSelected = 61,
		ECopyWhenInvalidOperationPending = 62,
		
		EExpungeWhenNotSelected = 63,
		EExpungeWhenInvalidOperationPending = 64,

		// General Session Panic codes
		ESessionInvalidSessionState = 65,
		ESessionInvalidPendingOp = 66,
		ESessionNullCurrentCommand = 67, // See also ESessionNotNullCurrentCommand
		ESessionCommandNotFlushing = 68,
		ESessionUnknownTimer = 69,
		ESessionLiteralSizeZero = 70,
		ESessionNullInputStream = 71,
		ESessionBadInputStreamState = 72,
		ESessionNullOutputStream = 73,
		ESessionBadOutputStreamState = 74,
		ESessionStreamsNotBothNull = 75,
		ESessionReceivedSecureServerCnf = 76,
		ESessionReportStatusAlreadyQueued = 77,
		ESessionNullReportStatus = 78,
		ESessionAsyncCallBackOpInvalid = 79,
		ESessionInputBufferNotEmpty = 80,
		ESessionInvalidOverFlowHandler = 81,
		ESessionInvalidCommandResponseCode = 82,
		ESessionNullSelectedFolderInfo = 83,
		
		// General Command Panic codes
		ECommandInvalidParseState = 84,
		ECommandInvalidResponseCode = 85,
		ECommandInvalidParseBlockResult = 86,
		ECommandInvalidTagType = 87,
		ECommandMismatchedParseStateAndResponseCode = 88,
		ECommandResponseLiteralDataNotZero = 89,
		ECommandResponseLiteralDataNotNonZero = 90,
		ECommandNotFlushing = 91,
		ECommandNoTaggedResponsesToFlush = 92,
		ECommandOutputBufferNotNull = 93,
		ECommandOutputStreamIsNull = 94,
			
		EHeaderFieldNonLiteral = 95,
		EListBaseParseStateInvalid = 96,
		EAtomParserInvalidParserState = 97,
		EAtomParserLiteralFetchStateWhenProcessingLine = 98,
		EAtomParserBufferIsNull = 99,
		EAtomParserLiteralBlockLengthMismatch = 100,
		EAtomWalkerStackIsEmpty = 101,
		EAtomWalkerNothingToWalkUpTo = 102,
		EAtomWalkerNullStackEntry = 103,
		EAtomWalkerWalkedAcrossToNull = 104,
		
		EBodyStructureBuilderInvalidProcessBlockState = 105,
		EBodyStructureBuilderInvalidParseStep = 106,
		EBodyStructureBuilderInvalidBodyStructureType = 107,
		EBodyStructureBuilderRootNotOwned = 108,
		EBodyStructureBuilderExpectedRootAtomOnlyOnStack = 109,
		EBodyStructureBuilderCurrentAtomIsNotRoot = 110,
		EBodyStructureBuilderStackNotEmpty = 111,
		EBodyStructureBuilderStackIsEmpty = 112,
		
		EFetchBodyStructureUnparsedDataExists = 113,
		EFetchBodyStructureUnexpectedState = 114,
		EFetchBodyStructureExpectedRequestForLine = 115,
		
		EFolderInfoNegativeExistsCount = 116,
		EListBaseAtomParserAlreadyExists = 117,
		EListBaseAtomParserIsNull = 118,
		EIdleStateInvalid = 119,	
		ESelectFolderDataIsNull = 120,
		ESelectMailboxNameIsEmpty = 121,
		EAppendInvalidState = 122,
		ESessionNotNullCurrentCommand = 123, // See also ESessionNullCurrentCommand

		ECommandInvalidParseState1 = 124,
		ECommandInvalidParseState2 = 125,
		ECommandInvalidParseState3 = 126,
		ECommandInvalidParseState4 = 127,
		ECommandInvalidParseState5 = 128,
		ECommandInvalidParseState6 = 129,
		ECommandInvalidParseState7 = 130,
		ECommandInvalidParseState8 = 131,
		ECommandInvalidParseState9 = 132,
		ECommandInvalidParseState10 = 133,

		EStoreOperationCompleteWithPendingRequest = 134,
		ETParseBlockResultInvalidLiteralSize = 135,

		// Session manager
		EDisconnectWhenDisconnectListNotEmpty1 = 900,
		EDisconnectWhenDisconnectListNotEmpty2 = 901,
		EDisconnectWhenDisconnectListNotEmpty3 = 902,
		EUnexpectedOutputStreamCloseInd = 903,
		EUnexpectedSecureClientCnf = 904,
		EUnexpectedInputStreamCloseInd = 905,
		EUnexpectedStateForDoRunL = 906,
		EUnexpectedStateForDoCancel = 907,
		EOutputStreamUnexpectedlyNull = 908,
		EUnexpectedStateForDoCompleteWithError = 909,
		EUnexpectedServerGreetingResponse = 910,
		EUnexpectedStateForGetSession = 911,
		EUnexpectedStateForDisConnect = 912,
		ERConnectionNotDefined = 913,


		//Panic codes for imapSettings dll(1000 - 1999) 
		EImapSettingsNoDefaultAttachmentFile = 1000,
		EImapSettingsNoMessageFooter = 1001,
		EImapSettingsInvalidService = 1002,


		//Panic codes for imapcharconv.dll(3000-3999)
		ECharconvTlsNotInstantiated = 3000,


		//Panic codes for mailstore dll(4000 - 4999)
		EMailStoreDecodeDataNull = 4000,
		EMailStoreFetchBodyBadCAFHandle = 4001,
		EMailStoreQueueEntryObjectNotFound = 4002,
		EMailStoreNoQueEntryRequest = 4003,
		EMailStoreNoDataChunks = 4004,
		EMailStoreDataChunkOutOfRange = 4005,
		EMailStoreQueueEntryObjectAlreadyExists = 4006,
		EMailStoreDecodeDataNotNull = 4007,

		//Panic codes for Impapprotocolcontroller dll(5000-5999)
		EIdleControllerStateUnknown = 5000,
		EIdleControllerImapSessionNull = 5001,
		EIdleControllerAlreadyActive = 5002,
		
		EOpFetchBodyUnknownPart = 5003,
		ENoBackgroundSyncInProgress = 5004,
		EConnectAndSyncBgSyncOpIsNotNull = 5005,
		
		ECopyFromLocalCompoundIsNotNull = 5006,
		ECopyFromLocalCompoundUnexpectedState = 5007,
		
		ECopyToLocalCompoundIsNotNull = 5008,
		ECopyToLocalCompoundUnexpectedState = 5009,
		ECopyToLocalSessionIsNull = 5010,
		
		ECopyWithinServiceCompoundIsNotNull = 5011,
		ECopyWithinServiceCompoundUnexpectedState = 5012,
		
		EDeleteCompoundIsNotNull = 5013,
		EDeleteCompoundUnexpectedState = 5014,
		
		EDeleteFolderCompoundIsNotNull = 5015,
		EDeleteFolderCompoundUnexpectedState = 5016,
		
		EDisconnectCompoundIsNotNull = 5017,
		EDisconnectCompoundUnexpectedState = 5018,
		
		EFullSyncFolderCompoundIsNotNull = 5019,
		EFullSyncSelectedFolderCompoundIsNotNull = 5020,
		ESyncFolderCompoundUnexpectedState = 5021,
		
		ENewOnlySyncFolderCompoundIsNotNull = 5022,
		EMoveFromLocalCompoundIsNotNull = 5023,
		EMoveToLocalCompoundIsNotNull = 5024,
		EMoveWithinServiceCompoundIsNotNull = 5025,
		
		ERenameCompoundIsNotNull = 5026,
		ERenameCompoundUnexpectedState = 5027,
		
		ESelectCompoundIsNotNull = 5028,
		ESelectCompoundUnexpectedState = 5029,
		
		ESynchroniseTreeCompoundIsNotNull = 5030,
		ESyncTreeCompoundUnexpectedState = 5031,
		ESyncServiceCompoundUnexpectedState = 5032,
		ESynchroniseAllCompoundIsNotNull = 5033,
		EPopulateCompoundIsNotNull = 5034,
		
		ECreateCompoundIsNotNull = 5035,
		ECreateCompoundUnexpectedState = 5036,
		
		EInvalidMsvTypeToCommand = 5037,
		
		EIdleSessionIsNull = 5038,
		EIdleControllerUnexpectedStateAtStart = 5039,
		EIdleControllerIsActiveAtStart = 5040,
		EIdleControllerAlreadyInUse = 5041,
		EIdleControllerLeaveInDoRunL = 5042,
				
		EFetchBodyUnknownMsvType = 5043,
		
		EBackgroundSyncSessionIsNull = 5044,
		EBackgroundSyncUnexpectedState = 5045,

		EIdleControllerInternalCancelOnBadState = 5046,
		EIdleControllerRunLCalledDuringInternalCancel = 5047,
		
		EProtocolControllerUnexpectedRequestedOp = 5048,
		EProtocolControllerUnexpectedCurrentOp = 5049,

		ECopyFromLocalCompoundCancelUnexpectedState = 5050,
		ECopyToLocalCompoundCancelUnexpectedState = 5051,
		ECopyWithinServiceCompoundCancelUnexpectedState = 5052,
		ECreateCompoundCancelUnexpectedState = 5053,
		EDeleteCompoundCancelUnexpectedState = 5054,
		EDeleteFolderCompoundCancelUnexpectedState = 5055,
		EDisconnectCompoundCancelUnexpectedState = 5056,
		ERenameCompoundCancelUnexpectedState = 5057,
		ESelectCompoundCancelUnexpectedState = 5058,
		ESyncFolderCompoundCancelUnexpectedState = 5059,
		ESyncServiceCompoundCancelUnexpectedState = 5060,
		ESyncTreeCompoundCancelUnexpectedState = 5061,
		EBackgroundSyncCancelUnexpectedState = 5062,
		EDisconnectUnexpectedMigrateState			= 5063,
		EProtocolControllerUnexpectedMigrateState	= 5064,
		EKillCurrentConnUnexpectedState             = 5065,
		ENewCarrierActiveUnexpectedMigrateState		= 5066,
		EProcessOpCompleteForMigrateUnexpectedState = 5067,
		EPrepareForNewCarrierUnexpectedState        = 5068,
		
		EProtocolControllerCancelBadMigrateState    = 5069,
		EProConPreForNewCarBadMigrateState			= 5070,
		EProConKillCurrentBadMigrateState			= 5071,
		EMigrateCompoundIsNotNull					= 5072,
		EUpdateCompoundUnexpectedState				= 5073,

		//Panic codes for imaptransporthandler dll(6000 - 6999)

		/** An input stream API was used when it is in the wrong state. */
		EBadInputStreamState			= 6000,
		/** An output stream API was used when it is in the wrong state. */
		EBadOutputStreamState			= 6001,
		/** The input stream state machine has broken. */
		EBadInputStreamError			= 6002,
		/** The output stream state machine has broken.*/
		EBadOutputStreamError			= 6003,
		/** The socket listener is in an unexpected state. */
		EBadSocketListenerState			= 6004,
		/** The input stream was used before an observer bound to it. */
		EInputStreamNotBound			= 6005,
		/** The output stream was used before an observer bound to it.*/
		EOutputStreamNotBound			= 6006,
		/** The output stream was used before an observer bound to it.*/
		EOutputStreamNotBoundSecure	= 6007,
		/** The socket connector is in an unexpected state. */
		EBadSocketConnectorState		= 6008,
		/** A NULL pointer to a connection object has been given. */
		EExpectedConnectionNotSupplied	= 6009,
		/** A secure socket was used before starting the secure handshake. */
		ETlsSocketNotStarted			= 6010,
		/** The input stream was suspended when it already was. */
		EInputStreamAlreadySuspended	= 6011,
		/** The input stream was resumed when it was not suspended. */
		EInputStreamNotSuspended		= 6012,
		/** About to request more data from the socket, but the receiving buffer is not empty. */
		EInputStreamBufferNotEmpty		= 6013,


		//Panics for imapsyncmanager dll(7000 - 7999)
		EMsgnrOutOfRange = 7000,
		ESyncManagerCancelUnexpectedState = 7001,
		ECantChangeUID = 7002,
		ECantChangeMsvId = 7003,
		EIndexEmpty = 7004,
		EImapSyncManagerInvalidFolderID = 7005,
		EInvalidfoldername = 7006,
		EAddLocalFolderInvalidSynchStrategy = 7007,
		EAddLocalFolderInvalidSubscribeStrategy = 7008,
		ENegativeServerResponseSynchStateBegin = 7009,
		ENegativeServerResponseSynchStateEnd = 7010,
		EUnexpectedStateSyncFolderTreeDoRunL = 7011,
		EUnexpectedStateSyncFolderTreeDoCancel = 7012,

		//Panic codes for IMPS dll (8000 - 8999)
		EDeleteOfUnknownType = 8000

		};
 public:
	IMPORT_C static void ImapPanic(TImpsPanic aPanic);
	};

#endif //__IMAPPANICCODES_H__

