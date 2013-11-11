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
// This is the public API header file for SMTS.DLL - 
// the transport driver which sends Imail using the 
// SMTP Internet protocol.
// 
//

#if !defined(__SMTS_H__)
#define __SMTS_H__
#if !defined(__MTSR_H__)
#include <mtsr.h>
#endif

#include <smtpset.h>
#include <commdb.h>
#include <tnonoperationmtmdata.h>

class CMsgImOutboxSend;

/**
@internalTechnology
@released
*/
typedef TPckgBuf<TImSmtpProgress> TImSmtpProgressBuf;

class CSmtpSrvMtm : public CBaseServerMtm
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CSmtpSrvMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry);
	~CSmtpSrvMtm();

	void CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	//
	void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);
	//
	TBool CommandExpected();
	//
	const TDesC8& Progress();
	//

protected:
	void DoCancel();
	void DoRunL();
	void DoComplete(TInt aError);
	TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
private:
	CSmtpSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry*  aEntry);
	void ConstructL();
	void DoSendMessagesL(TRequestStatus& aStatus);
	void CopyToOurSelectionL(const CMsvEntrySelection& aSourceSelection);
	void AddWaitingInOutboxToSelectionL();
	TInt SetServiceToConnected(TBool aConnected);
	TInt GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer);

private:

	TMsvId					iServiceId;
	CMsgImOutboxSend*		iOutboxSend;
	TRequestStatus*			iReport;
	TImSmtpProgressBuf		iProgressBuffer;
	TInt					iSessionError;
	TUint32					iIntrAddress;
	CMsvEntrySelection* 	iSendSelection;

#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	TThreadId				iClientThreadId;
	TBool 					iHasCapability;
#endif

	TNonOperationMtmDataAccessPointIdBuffer iMtmDataAccessPointIdBuffer;

	};

#endif
