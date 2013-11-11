// IMCVUTIL.CPP

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

#include <badesca.h>
#include "IMCVUTIL.H"
#include <miuthdr.h>
#include "IMCVTEXT.H"
#include <logwrap.h>
#include <logwraplimits.h>
#include <msvapi.h>

#define __IMUT_NO_IM_EMAIL_EVENT_LOGGING //Remove this line to enable e-mail logging 

EXPORT_C CImcvUtils* CImcvUtils::NewL()
	{
	CImcvUtils* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CImcvUtils* CImcvUtils::NewLC()
	{
	CImcvUtils* self=new (ELeave) CImcvUtils();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImcvUtils::~CImcvUtils()
	{
	}

CImcvUtils::CImcvUtils()
	{
	}

void CImcvUtils::ConstructL()
	{
	iPriority=EMsvMediumPriority;
	}

EXPORT_C void CImcvUtils::ReceiptFieldsL(CDesC8Array& aReceiptFields)
	{
	aReceiptFields.Reset();
	aReceiptFields.AppendL(KImcvReturnReceiptToPrompt);
	aReceiptFields.AppendL(KImcvReturnXReceiptToPrompt);
	aReceiptFields.AppendL(KImcvMsgDispositionTo);
	aReceiptFields.AppendL(KImcvMsgDispositionOptions);
	}

EXPORT_C void CImcvUtils::PriorityFieldsL(CDesC8Array& aPriorityFields)
	{
	aPriorityFields.Reset();
	aPriorityFields.AppendL(KImcvPriorityPrompt);
	aPriorityFields.AppendL(KImcvXPriorityPrompt);
	aPriorityFields.AppendL(KImcvXMSMailPriorityPrompt);
	aPriorityFields.AppendL(KImcvImportancePrompt);
	}

EXPORT_C void CImcvUtils::SendReturnReceiptFieldsL(CDesC8Array& aReturnReceiptFields)
	{ // appends field to the array
	//At the moment it returns KImcvMsgDispositionTo only.  In future it could change!
	aReturnReceiptFields.Reset();
	aReturnReceiptFields.AppendL(KImcvMsgDispositionTo);
	}

EXPORT_C void CImcvUtils::SendPriorityFieldsL(CDesC8Array& aSendPriorityFields)
	{
	aSendPriorityFields.Reset();
	aSendPriorityFields.AppendL(KImcvXPriorityPrompt);
	}


void CImcvUtils::SendImportanceFieldsL(CDesC8Array& aSendImportanceFields)
	{
	aSendImportanceFields.Reset();
	aSendImportanceFields.AppendL(KImcvImportancePrompt);
	}

EXPORT_C TBool CImcvUtils::EvaluateReturnReceiptFields(const TDesC8& aFieldName)
	{
	if (aFieldName.MatchF(KImcvReturnReceiptToPrompt)>=0 || 
		aFieldName.MatchF(KImcvReturnXReceiptToPrompt)>=0 ||
		aFieldName.MatchF(KImcvMsgDispositionTo)>=0)
		return ETrue;
	return EFalse;		
	}


EXPORT_C TMsvPriority CImcvUtils::EvaluatePriorityText(const TDesC8& aPriority)
	{//  	check for high, low and then medium.
	//Precedence: bulk, special-delivery, first-class, list and junk, should not be used to set any Priority as RFC2076 defines
	//them as Non-standard controversial and discouraged
	TInt number=ConvertPriorityIntoNumber(aPriority);	
	if (aPriority.MatchF(KImportanceHighest)>=0 ||
		aPriority.MatchF(KImportanceHigh)>=0 ||
		number<KPriorityNormal ||
	//because match returns found if we search for urgent in a non-urgent string, it should be double checked.
		(aPriority.MatchF(KImPriorityUrgent)>=0 && aPriority.MatchF(KImPriorityNonUrgent)<0))
		{
		iPriority=EMsvHighPriority;  
		return iPriority;
		}
	if (aPriority.MatchF(KImportanceLowest)>=0 ||
		aPriority.MatchF(KImportanceLow)>=0 ||
		aPriority.MatchF(KImPriorityNonUrgent)>=0 ||
		number>KPriorityNormal)
		{//set to low only if it is medium.  
		if (iPriority==EMsvMediumPriority)
			iPriority=EMsvLowPriority;
		}
	return iPriority;
	}

TInt CImcvUtils::ConvertPriorityIntoNumber(const TDesC8& aPriority)
	{
	// if aPriority is null string, set priority to normal
	if (!aPriority.Length())
		return KPriorityNormal;
	// aPriority can be a digit (1..5) or a string or both.  In most cases it is digit.  
	//Hence search for digit in the string first and if it is not found, then search for the strings
	//search for digits
	TInt p=KPriorityHigh; //1
	while(p<6)
		{
		TBuf8<4> buf(KWildChars);  //set the wild chars when matching
		TBuf8<1> num;
		num.Num(p);
		buf.Insert(1,num);
		if (aPriority.Match(buf)>=0) //means found
			return p;
		p++;					  
		}
	return KPriorityNormal;
	}



//
//CImLogMessage
//


EXPORT_C CImLogMessage* CImLogMessage::NewL(RFs& aFs, TInt aPriority)
	{
	CImLogMessage* self = new (ELeave) CImLogMessage(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aPriority);
	CleanupStack::Pop();
	return self;
	}

CImLogMessage::CImLogMessage(TInt aPriority) 
			  :	CMsgActive(aPriority)
	{
	}

void CImLogMessage::ConstructL(RFs& aFs,TInt aPriority)
	{
	iLogWrapper=CLogWrapper::NewL(aFs,aPriority);
	CActiveScheduler::Add(this);
	}

CImLogMessage::~CImLogMessage()
	{
	Cancel();
	Reset();
	delete iLogWrapper;
	}

EXPORT_C CLogEvent& CImLogMessage::LogEvent()
	{
	if (!iLogEvent)
		{
		TRAP_IGNORE(iLogEvent=CLogEvent::NewL());
		}
	return *iLogEvent;
	}

EXPORT_C CLogBase& CImLogMessage::LogBase()
	{
	return iLogWrapper->Log();
	}


EXPORT_C void CImLogMessage::Start(TInt aError,TRequestStatus& aStatus)
	{
	if (aError != KErrNone)
		{
		TBuf<KLogMaxStatusLength> status;
		if (GetString(status, R_LOG_DEL_FAILED) == KErrNone)
			iLogEvent->SetStatus(status);	
		}

	Queue(aStatus);

#ifndef __IMUT_NO_IM_EMAIL_EVENT_LOGGING
	iLogWrapper->Log().AddEvent(*iLogEvent,iStatus);
	SetActive();
#else
	Complete(KErrNone);
#endif

	}

EXPORT_C TInt CImLogMessage::GetString(TDes& aString, TInt aId) const
	{
	return iLogWrapper->Log().GetString(aString, aId);
	}

void CImLogMessage::DoRunL()
	{
	}

void CImLogMessage::DoCancel()
	{
	iLogWrapper->Log().Cancel();
	CMsgActive::DoCancel();
	}

EXPORT_C void CImLogMessage::Reset()
	{
	delete iLogEvent;
	iLogEvent=NULL;
	}

void CImLogMessage::DoComplete(TInt& aStatus)
	{//always complete with KErrnone.  We don't care if logging fails.
	aStatus=KErrNone;
	}

//
// Check the available disk space and leave if it is insufficient
//
EXPORT_C void ImCheckDiskSpace::LeaveIfLowDiskL(RFs& aFs, TInt aCurrentDrive)
	{
	TVolumeInfo volumeInfo;
	User::LeaveIfError(aFs.Volume(volumeInfo, aCurrentDrive));

	if (volumeInfo.iFree < KMinimumDiskSpaceForSync)
		User::Leave(KErrDiskFull);
	}
