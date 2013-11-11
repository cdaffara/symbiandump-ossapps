// IMCVUTIL.H

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
//

#if !defined (__IMCVUTIL_H__)
#define __IMCVUTIL_H__

#if !defined (__E32BASE_H__)
#include <e32base.h>		
#endif

#if !defined (__MSVSTD_H__)
#include <msvstd.h>		
#endif

#if !defined (__MENTACT_H__)
#include <mentact.h>		
#endif

const TInt KMinimumDiskSpaceForSync = 65536;
const TInt KCheckDiskSpaceEveryNMessages = 5;

class CImcvUtils : public CBase	
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C static CImcvUtils* NewL();
	IMPORT_C static CImcvUtils* NewLC();
	~CImcvUtils();
	IMPORT_C static void ReceiptFieldsL(CDesC8Array& aReceiptFields);
	IMPORT_C static void PriorityFieldsL(CDesC8Array& aPriorityFields);
	IMPORT_C static void SendReturnReceiptFieldsL(CDesC8Array& aReturnReceiptFields);
	IMPORT_C static void SendPriorityFieldsL(CDesC8Array& aSendPriorityFields);
	         static void SendImportanceFieldsL(CDesC8Array& aSendImportanceFields);
	IMPORT_C TMsvPriority EvaluatePriorityText(const TDesC8& aPriority);
	IMPORT_C TBool EvaluateReturnReceiptFields(const TDesC8& aFieldName);
private:
	CImcvUtils();
	void ConstructL();
	TInt ConvertPriorityIntoNumber(const TDesC8& aPriority);
private:
	TMsvPriority iPriority;
	};


class CLogWrapper;
class CLogEvent;
class CLogBase;

class CImLogMessage : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	IMPORT_C static CImLogMessage* NewL(RFs& aFs, TInt aPriority = CActive::EPriorityStandard);
	~CImLogMessage();
	IMPORT_C CLogEvent& LogEvent();
	IMPORT_C TInt GetString(TDes& aString, TInt aId) const;
	IMPORT_C CLogBase& LogBase();
	IMPORT_C void Reset();
	IMPORT_C void Start(TInt aError,TRequestStatus& aStatus);	
private:
	CImLogMessage(TInt aPriority);
	void ConstructL(RFs& aFs,TInt aPriority);
	void DoCancel();			// Allows client to stop the SMTP session prematurely
	void DoRunL();			        // Decides what to do when current state completes
	void DoComplete(TInt& aStatus);					// Cleanup code
private:
	CLogWrapper* iLogWrapper;
	CLogEvent* iLogEvent;
	};

class ImCheckDiskSpace
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static void LeaveIfLowDiskL(RFs& aFs, TInt aCurrentDrive);
	};

#endif
