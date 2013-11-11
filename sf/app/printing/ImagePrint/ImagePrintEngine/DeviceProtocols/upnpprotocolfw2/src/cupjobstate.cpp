/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPJobState class
*
*/


#include <e32math.h>

#include "cupjobstate.h"
#include "upconsts.h"
#include "cupprintingjob.h"
#include "cuplogger.h"

//--------------------------------------------------------------------------------------------
//
// CUPJobState::NewL
//
//--------------------------------------------------------------------------------------------
CUPJobState* CUPJobState::NewL()
	{
		CUPJobState *self = NewLC();
		CleanupStack::Pop();	// self
		
		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::NewLC
//
//--------------------------------------------------------------------------------------------
CUPJobState* CUPJobState::NewLC()
{
		CUPJobState *self = new (ELeave) CUPJobState();
		CleanupStack::PushL(self);
		self->ConstructL();
		
		return self;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::ConstructL
//
//--------------------------------------------------------------------------------------------
void CUPJobState::ConstructL()
{
	LOG("[CUPFileParser]\t ConstructL");
	Init();
	InitErrorValues();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::CUPJobState
//
//--------------------------------------------------------------------------------------------
CUPJobState::~CUPJobState()
{
	Init();
	iErrorValues.Reset();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::Init
//
//--------------------------------------------------------------------------------------------
void CUPJobState::Init()
{
	iSheetsCompleted = KErrNotFound;
	iSheetsToPrint = 0; // cannot be negative for progress
	iId.Copy(KNullDesC8());
	iReason = ENoMessage;
	iState = EStateUnknown;
	iActiveId.Copy(KNullDesC8());
	iEndState.Init();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::InitErrorValues
//
//--------------------------------------------------------------------------------------------
void CUPJobState::InitErrorValues()
{
	TMapError err;

	err.iErrorCode = ENoMessage;
	err.iErrorString.Set(KUPnPStateNone);
	iErrorValues.Append(err);

	err.iErrorCode = EPbCheckPrinter;
	err.iErrorString.Set(KUPnPStateAttentionRequired);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonPaperJam;
	err.iErrorString.Set(KUPnPStateMediaJam);
	iErrorValues.Append(err);

	err.iErrorCode = EPbCheckPrinter;
	err.iErrorString.Set(KUPnPStatePaused);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonHardwareCoverOpen;
	err.iErrorString.Set(KUPnPStateDoorOpen);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonPaperNearlyEmpty;
	err.iErrorString.Set(KUPnPStateMediaLow);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonPaperEmpty;
	err.iErrorString.Set(KUPnPStateMediaEmpty);
	iErrorValues.Append(err);

	err.iErrorCode = ENoMessage;
	err.iErrorString.Set(KUPnPStateOutputAreaAlmostFull);
	iErrorValues.Append(err);

	err.iErrorCode = EPbCheckPrinter;
	err.iErrorString.Set(KUPnPStateOutputAreaFull);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonInkLow;
	err.iErrorString.Set(KUPnPStateMarkerSupplyLow);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonInkEmpty;
	err.iErrorString.Set(KUPnPStateMarkerSupplyEmpty);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonInk;
	err.iErrorString.Set(KUPnPStateMarkerFailure);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonPaperMedia;
	err.iErrorString.Set(KUPnPStateMediaChangeRequest);
	iErrorValues.Append(err);

	err.iErrorCode = EPbCheckPrinter;
	err.iErrorString.Set(KUPnPStateVendorDefined);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateCopies);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateSides);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateNumberUp);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateOrientationRequested);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateMediaSize);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateMediaType);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStatePrintQuality);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateTextLayout);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateImageLayout);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateImageOrientation);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStatePdlFidelity);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateFontFamily);
	iErrorValues.Append(err);
	
	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateFontSize);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateFormatError);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateHttpError);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonHardware;
	err.iErrorString.Set(KUPnPJobStateHwError);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateObjectFailure);
	iErrorValues.Append(err);

	err.iErrorCode = EPbStatusErrorReasonFile;
	err.iErrorString.Set(KUPnPJobStateUriNotFound);
	iErrorValues.Append(err);

}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::GetErrorValue
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::GetErrorValue(const TDesC8& aError)
{
	LOG81("[CUPJobState::GetErrorValue]\t  for: %S", &aError);
	for (TInt i = 0; i < iErrorValues.Count();++i)
	{
		if(0 == aError.Compare(iErrorValues[i].iErrorString))
		{
//			LOG1("[CUPJobState::GetErrorValue]\t returns: %d", iErrorValues[i].iErrorCode);
			return iErrorValues[i].iErrorCode;
		}
	}
//	LOG("[CUPJobState::GetErrorValue]\t returns: EPbCheckPrinter");
	return EPbCheckPrinter;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetState
//
//--------------------------------------------------------------------------------------------
void CUPJobState::SetState(TUPnPState aState)
{
	LOG1("[CUPJobState::SetState]\t %d", aState);

	if(iActiveId.Length() < 1)
	{
		iState = EStateUnknown;
		return;
	}

	// return if state is one of the terminating states
	if(	EStateReady == iState ||	
		EPrinterStateStopped == iState ||
		EStatePending == iState ||
		EJobStateAborted == iState ||
		EJobStateCanceled == iState )
		return;

	switch ( aState )
	{
		case EPrinterStateStopped:
		case EStatePending:
		case EJobStateAborted:
		case EJobStateCanceled:
		case EStateReady:
			Init();
			iState = aState;
			break;
		case EStatePrinting:
			iState = aState;
			break;
		case EJobStateSuccessful:
			iState = EStatePrinting;
			break;
		default:
			if(0 == iId.Compare(ActiveId()))
			{
				if(SheetsToPrint() == SheetsCompleted())
					iState = EStateReady;
				else if (EStateUnknown == iState)
					iState = EStatePrinting;
			}
			break;
	}
}


//--------------------------------------------------------------------------------------------
//
// CUPJobState::State
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::State()
{
//	LOG1("[CUPJobState::State]\t : %d",iState);
	return iState;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetActiveId
//
//--------------------------------------------------------------------------------------------
void CUPJobState::SetActiveId(const TDesC8& aId)
{
	Init();
	iActiveId.Copy(aId);
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::ActiveId
//
//--------------------------------------------------------------------------------------------
TPtrC8 CUPJobState::ActiveId()
{
	return iActiveId;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetSheetsCompleted
//
//--------------------------------------------------------------------------------------------
void CUPJobState::SetSheetsCompleted(const TDesC8& aSheetsCompleted)
{
//	LOG1("[CUPJobState::SetSheetsCompleted]\t - at the beginning SheetsCompleted(): %d",SheetsCompleted());
//	LOG8S("[CUPJobState::SetSheetsCompleted]\t - aSheetsCompleted: ",aSheetsCompleted);

	if(EStateReady == State())
	{
		iSheetsCompleted = SheetsToPrint();
		LOG1("[CUPJobState::SetSheetsCompleted]\t - State ready, set SheetsToPrint(): %d",SheetsToPrint());
	}
	else
	{
		TLex8 sheetsLex(aSheetsCompleted);
		//Should here be an check about return value of Val??? /TTP
		sheetsLex.Val(iSheetsCompleted);
	}
	LOG1("[CUPJobState::SetSheetsCompleted]\t - iSheetsCompleted: %d",SheetsCompleted());
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SheetsCompleted
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::SheetsCompleted()
{
	return iSheetsCompleted;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetReason
//
//--------------------------------------------------------------------------------------------
void CUPJobState::SetReason(TInt aReason)
{
	iReason = aReason;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::Reason
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::Reason()
{
	return iReason;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetJobList
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::SetJobList(const TDesC8& aJobIdList)
{
	TInt pos = PendingJobId(aJobIdList);
	if(1 < pos)
		SetState(EStatePending);
	else if (1 == pos)
	{
		iId = ActiveId();
		SetState(EStatePrinting);
	}
	
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SetSheetsToPrint
//
//--------------------------------------------------------------------------------------------
void CUPJobState::SetSheetsToPrint(TInt aSheets)
{
	LOG1("[CUPJobState::SetSheetsToPrint]\t aSheets\t = %d", aSheets);
	if(0 < aSheets)
		iSheetsToPrint = aSheets;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::SheetsToPrint
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::SheetsToPrint()
{
//	LOG1("[CUPJobState::SheetsToPrint]\t iSheetsToPrint\t = %d", iSheetsToPrint);
	return iSheetsToPrint;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::Progress
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::Progress(TInt aSheets)
{
	TInt percentage = 0;
	if(State() == EStateReady)
		percentage = 100;
	else if(0 < SheetsCompleted() && 0 < aSheets)
		percentage = (((SheetsCompleted() << 8) / aSheets) * 100) >> 8;
	
//	LOG1("[CUPJobState::Progress]\t SheetsCompleted()\t = %d", SheetsCompleted());
//	LOG1("[CUPJobState::Progress]\t aSheets\t\t\t = %d", aSheets);
	LOG1("[CUPJobState::Progress]\t percentage\t\t\t = %d%%", percentage);

	return percentage;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::Progress
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::Progress()
{
	return Progress(SheetsToPrint());
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::UpdateJobState
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::UpdateJobState(const TDesC8& aPrinterState, const TDesC8& aReason, const TDesC8& aJobList, const TDesC8& aJobId, const TDesC8& aSheetsCompleted)
{
	LOG81("[CUPJobState::UpdateJobState]\t active job: %S", &(ActiveId()));

	LOG81("[CUPJobState]\t - aPrinterState: %S", &aPrinterState);
	LOG81("[CUPJobState]\t - aReason: %S", &aReason);
	LOG81("[CUPJobState]\t - aJobList: %S", &aJobList);
	LOG81("[CUPJobState]\t - aJobId: %S", &aJobId);
	LOG81("[CUPJobState]\t - aSheetsCompleted: %S", &aSheetsCompleted);

	SetReason(GetErrorValue(aReason));

	//Set sheets for progress
	SetSheetsCompleted(aSheetsCompleted);

	// If printer state stopped, no reason to continue
	if(KErrNotFound != aPrinterState.Find(KUPnPPrinterStateStopped()))
	{
		SetState(EPrinterStateStopped);
		return State();
	}

	// If job is in the list, state is printing or pending. Else the job does not exist.
	SetJobList(aJobList);
	if(EStatePending == State())
	{
		SetReason(EPbStatusErrorReasonHardwarePrinterBusy);
	}
	else if(State() == EStatePrinting  && aJobId.Compare(ActiveId()) != 0 )
	{
		LOG82("[CUPJobState::State]\t - Ready: iState: %d, aJobId; %S",iState, &aJobId);
		SetState(EStateReady);
	}

	return State();
}

//// PRIVATE ///


//--------------------------------------------------------------------------------------------
//
// CUPJobState::CUPJobState
//
//--------------------------------------------------------------------------------------------
CUPJobState::CUPJobState()
{
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::PendingJobId
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::PendingJobId(const TDesC8& aJobIdList)
{
	if(0 >= aJobIdList.Length())
		return KErrNotFound;
	
	TInt count = 0;
	TLex8 list(aJobIdList);

	TLex8 idLex(ActiveId());
	TInt activeId;
	if (idLex.Val(activeId) != KErrNone)
	{
		return KErrArgument;
	}

	while(!list.Eos() && count < 50)
	{
		++count;
		TInt val = KErrNotFound;
		if(KErrNone == list.Val(val))
		{
			if(activeId == val)
				return count;
			else
				list.Inc();
		}
	}
	return KErrNotFound;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::ParseState
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::ParseState(const TDesC8& aState)
{
	if(KErrNotFound != aState.Find(KUPnPJobStateSuccessful()))
		return EJobStateSuccessful;
	if(KErrNotFound != aState.Find(KUPnPJobStateAborted()))
		return EJobStateAborted;
	if(KErrNotFound != aState.Find(KUPnPJobStateCanceled()))
		return EJobStateCanceled;
	if(KErrNotFound != aState.Find(KUPnPPrinterStateIdle()))
		return EPrinterStateIdle;
	if(KErrNotFound != aState.Find(KUPnPPrinterStateProcessing()))
		return EPrinterStateProcessing;
	if(KErrNotFound != aState.Find(KUPnPPrinterStateStopped()))
		return EPrinterStateStopped;

	return EStateUnknown;
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::ParseJobEndStateL
//
//--------------------------------------------------------------------------------------------
TInt CUPJobState::ParseJobEndStateL(const TDesC8& aJobEndState)
{
//	LOG("[CUPJobState::ParseJobEndState]\t");
	iEndState.Init();
	TInt res = KErrNotFound;

	// Return if no data
	if(1 > aJobEndState.Length())
		return res;

	// Parse current job and state from JobEndState
	_LIT8(KSeparator, ",");
	
	RArray<TPtrC8> strings;
	CleanupClosePushL(strings);
	strings.Reset();
	
	TPtrC8 ptr(aJobEndState);
	TInt pos = aJobEndState.Find(KSeparator());
	while(KErrNotFound != pos)
	{
		strings.Append(ptr.Left(pos));
		pos += KSeparator().Length();
		ptr.Set(ptr.Right(ptr.Length() - pos));

		pos = ptr.Find(KSeparator());
		if(KErrNotFound == pos)
			strings.Append(ptr);
	}
	
	// Assign strings in member variables if current job's state
	if(1 <= strings.Count() && 0 == strings[0].Compare(ActiveId()))
	{
		if(1 <= strings.Count())
		{
			iId.Copy(strings[0]);
			iEndState.iId.Set(iId);
		}
		if(2 <= strings.Count())
			iEndState.iJobName.Set(strings[1]);
		if(3 <= strings.Count())
			iEndState.iOriginator.Set(strings[2]);
		if(4 <= strings.Count())
		{
			SetSheetsCompleted(strings[3]);
			iEndState.iSheets = SheetsCompleted();
		}
		if(5 <= strings.Count())
			iEndState.iState = ParseState(strings[4]);
		if(6 <= strings.Count())
			iEndState.iReason = GetErrorValue(strings[5]);

		res = KErrNone;
	}
	CleanupStack::PopAndDestroy( &strings );

	return res;

}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::PrinterStateUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::PrinterStateUpdated( const TDesC8& aEvent )
{
	// If printer state not stopped, no reason to assign it
	if(KErrNotFound < aEvent.Find(KUPnPPrinterStateStopped()))
	{
		SetState(EPrinterStateStopped);
	}
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::PrinterStateReasonsUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::PrinterStateReasonsUpdated( const TDesC8& aEvent )
{
	SetReason(GetErrorValue(aEvent));
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::JobIdListUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::JobIdListUpdated( const TDesC8& aEvent )
{
	//Check if pending
	SetJobList(aEvent);
	if(EStatePending == State())
	{
		SetReason(EPbStatusErrorReasonHardwarePrinterBusy);
	}
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::JobEndStateUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::JobEndStateUpdatedL( const TDesC8& aEvent )
{
	// init id
	iId.Copy(KNullDesC8());

	// Parse if contains data and for this job
	TInt res = ParseJobEndStateL(aEvent);
	if(KErrNotFound != res)
	{
		SetState(iEndState.iState);

		// if any abort state updates never received, let's wait the one concerning this case
		if(EJobStateAborted == iEndState.iState && iEndState.iAbortState)
			return EStatePrinting;
	}
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::JobMediaSheetsCompletedUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::JobMediaSheetsCompletedUpdated( const TDesC8& aEvent )
{
	JobMediaSheetsCompletedUpdated(iId, aEvent);
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::JobMediaSheetsCompletedUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::JobMediaSheetsCompletedUpdated(const TDesC8& aId, const TDesC8& aEvent )
{
	LOG82("[CUPJobState::JobMediaSheetsCompletedUpdated]\t - aId: %S aEvent: %S", &aId, &aEvent);

	// id matches to active id
	if(0 == aId.Compare(ActiveId()))
	{
		SetSheetsCompleted(aEvent);

		// -1 for unknown, 0 for pending or processing (or finished)
		if( (-1 == SheetsCompleted()) || (0 == SheetsCompleted()) )
		{
			LOG81("[CUPJobState::JobMediaSheetsCompletedUpdated]\t - aId:%S\t",&aId);
		}
		else if (SheetsToPrint() == SheetsCompleted())
		{
			LOG1("[CUPJobState::JobMediaSheetsCompletedUpdated]\t - state ready -> SheetsToPrint(): %d",SheetsToPrint());
			SetState(EStateReady);
		}
		else
		{
			LOG1("[CUPJobState::JobMediaSheetsCompletedUpdated]\t - state printing -> SheetsToPrint(): %d",SheetsToPrint());
			SetState(EStatePrinting);
		}
	}

	return State();	
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::ContentCompleteListUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::ContentCompleteListUpdated( const TDesC8& /*aEvent*/ )
{
	return State();
}

//--------------------------------------------------------------------------------------------
//
// CUPJobState::JobAbortStateUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPJobState::JobAbortStateUpdatedL( const TDesC8& aEvent )
{
	// Mark flag as true that any abort state messages never received
	iEndState.iAbortState = ETrue;
	
	// Parse if contains data and for this job
	TInt res = ParseJobEndStateL(aEvent);
	if(KErrNotFound != res)
	{
		SetReason(iEndState.iReason);
		SetState(iEndState.iState);
	}

	return State();
}

//  End of File
