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

#ifndef __CIMAP4SERVERMTM_H__
#define __CIMAP4SERVERMTM_H__

#include <e32std.h>
#include <imapset.h>
#include <mtsr.h>
#include <tnonoperationmtmdata.h>

class CBaseServerMtm;
class CImapOfflineControl;
class CImapProtocolController;

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
class CImapUpsResponseWaiter;
#endif

// package buffer for progress info.
/**
@internalComponent
@prototype
*/
typedef TPckgBuf<TImap4CompoundProgress> TImap4ProgressBuf;

// States that MTM can be in
/**
@internalComponent
@prototype
*/
enum TImap4ServerMtmState
	{
	EMtmStateIdle							=  0,
	EMtmStateCopyToLocal					=  1,	
	EMtmStateCopyFromLocal					=  2,
	EMtmStateCopyWithinService				=  3,
	EMtmStateMoveToLocal					=  4,
	EMtmStateMoveFromLocal					=  5,
	EMtmStateMoveWithinService				=  6,
//	EMtmStateDeleteAfterMove				=  7,	// n/a for new implementation
	EMtmStateDelete							=  8,
	EMtmStateDeleteFolder					=  9,
	EMtmStateCreateFolder					= 10,
	EMtmStateRenameFolder					= 11,
	EMtmStatePrimaryConnect					= 12,
//	EMtmStateSecondaryConnect				= 13,	// n/a for new implementation
	EMtmStateForegroundSync					= 14,
	EMtmStateMiscCommand					= 15,
	EMtmStateWaitingForBackgroundToFinish	= 16,
//	EMtmStateSyncCompleted					= 17,	// n/a for new implementation
	EMtmStatePopulate						= 18,
//	EMtmStateLogging						= 19,	// n/a for new implementation
	EMtmStateOffLineDelete					= 20,
	EMtmStateOffLineUndelete				= 21,
	EMtmStateOffLineCopyToLocal				= 22,
	EMtmStateOffLineMoveToLocal				= 23,
	EMtmStateOffLineCopyFromLocal			= 24,
	EMtmStateOffLineMoveFromLocal			= 25,
	EMtmStateOffLineCopyWithinService		= 26,
	EMtmStateOffLineMoveWithinService		= 27,
	EMtmStateOffLinePopulate				= 28,
//	EMtmStateSecondarySessionIdle			= 29,
	EMtmStateAuthoriseConnection			= 30,
	EMtmStateUpdateFlag						= 31
	};


/**
@internalTechnology
@prototype
*/
class CImap4ServerMtm : public CBaseServerMtm
	{
public:
	IMPORT_C static CImap4ServerMtm* NewL( CRegisteredMtmDll& aRegisteredMtmDll,
										   CMsvServerEntry* aEntry );
	virtual ~CImap4ServerMtm();
	void CopyToLocalL( const CMsvEntrySelection& aSelection, 
				  	   TMsvId aDestination, 
				  	   TRequestStatus& aStatus );
	
	void CopyFromLocalL( const CMsvEntrySelection& aSelection,
				   		 TMsvId aDestination, 
				   		 TRequestStatus& aStatus );
	void CopyWithinServiceL( const CMsvEntrySelection& aSelection, 
							 TMsvId aDestination, 
							 TRequestStatus& aStatus );
	void MoveToLocalL( const CMsvEntrySelection& aSelection, 
				  	   TMsvId aDestination, 
				  	   TRequestStatus& aStatus );
	void MoveFromLocalL( const CMsvEntrySelection& aSelection,
						 TMsvId aDestination, 
						 TRequestStatus& aStatus );
	void MoveWithinServiceL( const CMsvEntrySelection& aSelection, 
							 TMsvId aDestination, 
							 TRequestStatus& aStatus );
	void DeleteAllL( const CMsvEntrySelection& aSelection, 
					 TRequestStatus& aStatus );
	void CreateL( TMsvEntry aNewEntry,
				  TRequestStatus& aStatus );
	void ChangeL( TMsvEntry aNewEntry,
				  TRequestStatus& aStatus );
	void StartCommandL( CMsvEntrySelection& aSelection, 
					    TInt aCommand, 
						const TDesC8& aParameter, 
						TRequestStatus& aStatus );
	TBool CommandExpected();
	const TDesC8& Progress();
	TInt ChangeEntriesL(const CMsvEntrySelection& aSelection, TInt aMark, TRequestStatus& aStatus);
protected:
	IMPORT_C TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);

private:
	CImap4ServerMtm( CRegisteredMtmDll& aRegisteredMtmDll, 
					 CMsvServerEntry* aServerEntry);
	void ConstructL();

	// from CBaseServerMtm
	void DoCancel();
	void DoRunL();
	void DoComplete(TInt aError);
	
	void Queue(TRequestStatus& aStatus);
	
	TBool PruneMessages(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	
	void GetSystemProgress(TMsvSystemProgress& aOutSysProg);
	
	void GetProgress(TImap4CompoundProgress& imap4Progress);

	TInt GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer);

private:
	// Owned heap objects
	CImapProtocolController* iImapProtocolController;
	CImapOfflineControl* 	 iImapOfflineControl;
	
	TRequestStatus*          iRequest;	// no ownership
	
	// CImapServerMtm internal state
	TImap4ServerMtmState	iState;
	TBool					iBatchInProgress;
	TImap4ProgressBuf 		iProgressBuffer;
	// Last operation performed was an offline operation
	TBool 					iLastOpWasOffline;
	CMsvServerEntry*		iServerEntry;
	
	/**
	For Interaction with UPS server.
	*/
#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	TThreadId 				iClientThreadId;
	TBool 					iHasCapability;
	CImapUpsResponseWaiter*	iWaiter;
#endif

	TNonOperationMtmDataAccessPointIdBuffer iMtmDataAccessPointIdBuffer;
	};

#endif	//__CIMAP4SERVERMTM_H__
