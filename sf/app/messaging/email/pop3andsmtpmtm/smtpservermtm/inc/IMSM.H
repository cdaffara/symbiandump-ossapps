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
// This is the public API header file for IMSM.DLL - 
// the transport driver which sends Imail using the 
// SMTP Internet protocol.
// 
//

#if !defined(_IMSM_H__)
#define _IMSM_H__

#if !defined (__IMSK_H__)
#include <imsk.h>				// EActivePriorityHigh
#endif

#include "mimmobileserver.h"

/**
@internalComponent
@released
*/
const TInt KImSmtpFilePriority		 = (EActivePriorityHigh);	
const TInt KImSmtpSessionPriority  	 = (KImSmtpFilePriority);
const TInt KMsgImOutboxSendPriority  = (KImSmtpSessionPriority);


// forward declarations
class CImSmtpSession;
class CMsvEntrySelection;
class CMsvServerEntry;
class CSmtpSessionManager;
class CImMobilityManager;
class CSmtpSettings;
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
class CSmtpUpsResponseWaiter;
#endif
class CMsgImOutboxSend : public CActive, public MImMobileServer
/**
@internalComponent
@released
*/
	{
public:		
	static CMsgImOutboxSend* NewLC(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry,TMsvId aService);
	static CMsgImOutboxSend* NewL(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry,TMsvId aService);
	~CMsgImOutboxSend();

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	void StartL(TRequestStatus& aStatus, TThreadId aClientThreadId, TBool aHasCapability);
#endif		
	void StartL(TRequestStatus& aStatus);
	
	const TImSmtpProgress&	Progress();
	TBool SessionIsConnected();
	TInt NextFile();									// public (but not imported) 
														// so SmtpSession() can use function
	void SetLastMessageStatusL(const TTime&, TInt);	// ditto
	inline const CMsvEntrySelection& EntrySelection() const;

	CDesCArray& BccRcptArray();
	void ResetBccRcptArrayL();

	// From MImMobileServer
	void PrepareForNewCarrier(TImMobilityAction aAction, TBool aIsSeamless);
	void CarrierLost();
	void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless);
	void MobilityError(TUint aError);
	const TDesC8& MobilityProgress();
	TInt GetAccessPointIdForConnection(TUint32& aAccessPointId) const;

protected:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void ConstructL( TMsvId aService );
	CMsgImOutboxSend(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry);

private:
	enum TState
		{
		EStateIdle,
		EStateUserPrompting,
		EStateConnectingSession,
		EStateSendingFiles,
		EStateClosingSession,
		EStateWaitingNewCarrier,
		EStateMobilityError
		};

	enum TMobilityOperation
		{
		EMobilityOperationIdle,
		EMobilityOperationStoppingCurrent,
		EMobilityOperationCompletingCurrent,
		EMobilityOperationMigrating
		};

private:
	void SetupStartL();
	void StartConnectingL();
	void PrepareForNewCarrierAfterOperation(TMobilityOperation aMobilityOperation);
	void SessionConnectedL();
	void SentFiles();
	void SessionClosed();
	void MigratedL();
	void SessionConnectionFailed();
	void SignalMigrate();
	void StartWaitingNewCarrier();
	void CancelForMigrate();
	void Complete(TInt status);

	void UpdateSummaryInfo(TInt&);
	void SetBodyEncoding(TMsgOutboxBodyEncoding);
	void DisconnectUnsentMessagesL();
	TInt CalculateError(TInt aCompletionReason);
	void RestoreBccRecipientsToHeaderL();
	void CleanUpOnDestructL();
	TBool PreparingForMigration();

private:
	const CMsvEntrySelection&	iEntrySelection;	//	Array of message Ids.. (filename)	
	CMsvServerEntry&	  		iServerEntry;	//	Handle on individual
												//	element of iEntrySelection
	CImSmtpSession*				iSession;
	CSmtpSettings* iSettings;

	TImSmtpProgress				iProgress;
	TRequestStatus*				iReport;	


    TInt 						iCurrentMessageNo;     //  Replaces count in NextFile()
    TInt 						iTotalMessages;        //  Relaces msgNo in NextFile();
	TBool 						iSetDisconnected;
	CDesCArrayFlat* 			iBccRcptArray;

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	CSmtpUpsResponseWaiter*		iWaiter;
#endif

	TState iState;
	TMobilityOperation iMobilityOperation;

	CSmtpSessionManager* iSessionManager;
	CImMobilityManager* iMobilityManager;
	TBool iCancellingForMigrate;
	TBool iDecrementMessageCountAfterMigration;

	TPckgBuf<TImSmtpProgress> iMobilityProgressBuffer;	
	};

#endif
