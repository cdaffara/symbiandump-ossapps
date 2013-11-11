// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SMSSERV_H_
#define SMSSERV_H_

#include <etelmm.h>
#include <smuthdr.h>
#include <schedulebaseservermtm.h>
#include <smutset.h>

class CSmsOutboxSend;
class CSmsSimUtils;

class CSmsServerMtm : public CScheduleBaseServerMtm
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CSmsServerMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aServerEntry);
	~CSmsServerMtm();

	void CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
//	void DeleteL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
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
	void CompleteCommand(TInt aError);
	void DoComplete(TInt aError);

//	--- Functions derived from CScheduleBaseServerMtm ---
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	void ConstructL();
	void RestoreScheduleSettingsL();
private:
	CSmsServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aServerEntry);
	void RestoreSettingsL();
	void LoadResourceFileL();
	void FindServiceId();
	void PopulateSchedulePackage(const TDesC8& aParameter, const TBool aMove, TMsvSchedulePackage& aPackage) const;
	void ScheduleCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);
	void SimUtilsL(const CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);

private:
	TMsvId 					iSmsServiceId;
	TSmsProgress 			iProgress;
	TSmsProgressBuf 		iProgressBuf;	
	CSmsOutboxSend* 		iOutboxSend;
	CSmsSimUtils* 			iSimUtils;
	enum TSmsServerMtmState
		{
		ESmsServerMtmWaiting,
		ESmsServerMtmSending,
		ESmsServerMtmScheduling,
		ESmsServerMtmSimUtils
		} iState;
#if (defined SYMBIAN_USER_PROMPT_SERVICE)		
	TThreadId				iClientThreadId;
	TBool 					iHasCapability;
#endif	
	};

#endif
