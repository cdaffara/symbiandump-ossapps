/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the CBtStatusChannel class.
*
*/


#include <obex.h>

#include "crsbtdevice.h"
#include "cbtstatuschannel.h"
#include "cbtsoapcodec.h"
#include "clog.h"
#include "tbtmapper.h"
#include "cbtdiscover.h"
#include "printmessagecodes.h"


//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::NewL
//
//--------------------------------------------------------------------------------------------
CBtStatusChannel* CBtStatusChannel::NewL(MBtPCObserver& aObs)
{
		CBtStatusChannel *self = NewLC(aObs);
		CleanupStack::Pop();	// self
		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::NewLC
//
//--------------------------------------------------------------------------------------------
CBtStatusChannel* CBtStatusChannel::NewLC(MBtPCObserver& aObs)
{
		CBtStatusChannel *self = new (ELeave) CBtStatusChannel(aObs);
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::CBtStatusChannel
//
//--------------------------------------------------------------------------------------------
CBtStatusChannel::CBtStatusChannel(MBtPCObserver& aObs):
	CBtChannelBase( aObs )
{
	LOG("[CBtStatusChannel::CBtStatusChannel]\t");
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::~CBtStatusChannel
//
//--------------------------------------------------------------------------------------------
CBtStatusChannel::~CBtStatusChannel()
{
	LOG("[CBtStatusChannel::~CBtStatusChannel]\t");
	Cancel();
	ClearConnection();
	
	// iChannel have to delete here or otherwise Review-x report critical error.
	if(iChannel)
	{
		delete iChannel;
		iChannel = NULL;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::ConstructL()
{
	LOG("[CBtStatusChannel::ConstructL]\t begin");
	CBtChannelBase::ConstructL();
	LOG("[CBtStatusChannel::ConstructL]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::RunL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::RunL()
{
	LOG2("[CBtStatusChannel::RunL]\t BEGIN iStatus %d at state %d", iStatus.Int(), iState);

	 //Finish if required
	if( EStateFinish == iState )
	{
		Disconnect();
		return;
	}

	DoLeaveL( iStatus.Int() );

	switch( iState )
	{
		case EStateConnecting:
			iObexNullObject->Reset();
			StopWaiting();
			break;
        case EStateDisconnecting:
			Disconnect();
			StopWaiting();
            break;
		case EStateGettingPrinterState:
			GetSoapResponseL();
			GetPrinterStateResponseL();
			iState = EStateNone;
			break;
		case EStateCancellingJob:
			GetSoapResponseL();
			CancelJobResponseL();
			iDevice = NULL;
			iState = EStateNone;
			break;
		case EStateGettingJobAttributes:
			GetSoapResponseL();
			GetJobAttributesResponseL();
			break;
		case EStateNone:
			User::Leave( KErrCancel );
			break;
		case EStateGetEvent:
			iState = EStateGettingEvent;
			SendSoapRequestL(EBtGetEvent, iJobId);
			WaitL();
			break;
		case EStateGettingEvent:
			GetSoapResponseL();
			GetEventResponseL();
			StopWaiting();
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::RunError
//
//--------------------------------------------------------------------------------------------
TInt CBtStatusChannel::RunError(TInt aError)
{
	LOG1("[CBtStatusChannel::RunError]\t state %d", iState);
	LOG2("[CBtStatusChannel::RunError]\t error %d, iStatus %d", aError, iStatus.Int() );

	TInt id = KErrNotFound;

	StopWaiting();
	
	switch( iState )
	{
		case EStateGettingPrinterState:
			if(iDevice)
				id = iDevice->DeviceId();

			iState = EStateFinish;
			iObs.GetPrinterStateResponse(EBtStateIdle, ENoMessage, aError, id);
			break;
		case EStateCancellingJob:
			iState = EStateFinish;
			iObs.CancelJobResponse(aError);
			break;
		case EStateGettingJobAttributes:
			iState = EStateFinish;
			iObs.GetJobAttributesResponse(EBtStateUnknown, KNullDesC8(), KNullDesC8(), KErrNotFound, KErrNotFound, aError);
			break;
		case EStateGetEvent:
		case EStateGettingEvent:
			iObs.GetEventResponse(iJobId, EBtStateUnknown, EBtStateIdle, KErrNone, aError);
			break;
		default:
			iState = EStateFinish;
			iObs.GetEventResponse(KErrNotFound, KErrNotFound, KErrNotFound, KErrNotFound, aError);
			LOG("[CBtPrinterController::RunError]\t GetEventResponse sent" );
			break;
	}
	iResponseError = aError;
	return KErrNone; // othervise error returns to CActive: that's what we don't want.
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::DoCancel
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::DoCancel()
{
	LOG2("[CBtStatusChannel::DoCancel]\t at state %d, iStatus 0x%X", iState, iStatus.Int());

	StopWaiting();

	iState = EStateNone;

	CBtChannelBase::DoCancel();
	LOG1("[CBtStatusChannel::DoCancel]\t out iStatus 0x%X", iStatus.Int());
}

//------------------------------
// InCalls
//------------------------------

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetPrinterAttributesL
//
//--------------------------------------------------------------------------------------------
TInt CBtStatusChannel::GetPrinterStateL()
{
	LOG("[CBtStatusChannel::GetPrinterStateL]\t begin");
	if(iStop)
		return KErrNone;
	
	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	Cancel();

	iState = EStateGettingPrinterState;
	SendSoapRequestL(EBtGetPrinterState);

	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetJobAttributesL
//
//--------------------------------------------------------------------------------------------
TInt CBtStatusChannel::GetJobAttributesL(TInt aJobId)
{
	LOG1("[CBtStatusChannel::GetJobAttributesL]\t begin with id %d", aJobId);
	if(iStop)
		return KErrNone;

	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	if( KErrNotFound == aJobId)
		User::Leave(KErrInvalidData);
	
	LOG1("[CBtStatusChannel::GetJobAttributesL]\t wait connection? %d", iStatus.Int());
	WaitL();

	iJobId = aJobId;
	LOG1("[CBtStatusChannel::GetJobAttributesL]\t iJobId %d", iJobId);

	iState = EStateGettingJobAttributes;
	SendSoapRequestL(EBtGetJobAttributes, iJobId);

	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetEventL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::GetEventL(TInt aJobId)
{
	LOG("[CBtStatusChannel::GetEvent]\t begin");
	if(iStop)
		return;
	
	if(!IsConnected())
		User::Leave(KErrDisconnected);

	if( 0 > aJobId)
		User::Leave(KErrInvalidData);
	
	if(EStateGetEvent == iState || EStateGettingEvent == iState)
		return;
		
	Cancel();

	iJobId = aJobId;

	iState = EStateGetEvent;
	Activate();
}

//----------------------------------

//------------------------------
// Responses
//------------------------------
//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetPrinterStateResponseL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::GetPrinterStateResponseL()
{
	LOG("[CBtStatusChannel::GetPrinterStateResponseL]\t");
	User::LeaveIfNull(iSoapCodec);
	iState = EStateNone;

	TInt state = EBtStateUnknown;
	TInt reasons = EPbCheckPrinter;
	TInt opStatus = KErrGeneral;

	iSoapCodec->GetPrinterStateResponseL(state, reasons, opStatus);

	TInt id = KErrNotFound;
	if(iDevice)
		id = iDevice->DeviceId();

	iObs.GetPrinterStateResponse(state, reasons, opStatus, id);

}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetJobAttributesResponseL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::GetJobAttributesResponseL()
{
	LOG("[CBtStatusChannel::GetJobAttributesResponseL]\t");
	User::LeaveIfNull(iSoapCodec);
	iState = EStateNone;

	TInt  jobState = EBtStateUnknown; 
	TBtAttrString8 jobName(KNullDesC8());
	TBtAttrString8 originator(KNullDesC8());
	TInt sheetsCompleted = KErrNotFound; 
	TInt pendingJobs = KErrNotFound;
	TInt opStatus = KErrNotSupported;
	
	iSoapCodec->GetJobAttributesResponseL(iJobId, jobState, jobName, originator, 
											sheetsCompleted, pendingJobs, opStatus);

	iObs.GetJobAttributesResponse(jobState, jobName, originator, 
									sheetsCompleted, pendingJobs, opStatus);
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::CancelJobResponseL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::CancelJobResponseL()
{
	LOG("[CBtStatusChannel::CancelJobResponseL]\t");
	User::LeaveIfNull(iSoapCodec);
	iState = EStateNone;

	TInt err = KErrNotFound;
	iSoapCodec->CancelJobResponseL(iJobId, err);

	LOG1("[CBtStatusChannel::CancelJobResponseL]\t err %d", err);
	iObs.CancelJobResponse(err);
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::GetEventResponseL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::GetEventResponseL()
{
	LOG("[CBtStatusChannel::GetEventResponseL]\t");
	User::LeaveIfNull(iSoapCodec);
	iState = EStateNone;

	TInt jobState, printerState, stateReasons, operationStatus;

	iSoapCodec->GetEventResponseL(iJobId, jobState, printerState, stateReasons, operationStatus);
	iObs.GetEventResponse(iJobId, jobState, printerState, stateReasons, operationStatus);
}
///////////////////////////////
// Private
///////////////////////////////
//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::ConnectL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::ConnectL()
{
	LOG2("[CBtStatusChannel::ConnectL]\t isConnected: %d, iDevice %d", IsConnected(), iDevice);

	User::LeaveIfNull(iDevice);

	LOG1("[CBtStatusChannel::ConnectedL]\t aDevice.UsedProtocols(): %d", iDevice->UsedProtocol());
    if( !(KImagePrint_PrinterProtocol_BPP & iDevice->UsedProtocol()) )
	   	User::Leave(KErrNotSupported);

    if(IsConnected())
    {
		LOG1("[CBtStatusChannel::ConnectL]\t isConnected: %d", IsConnected());
    	return;
    }

	if(iChannel)
	{
		delete iChannel;
		iChannel = NULL;
	}

    TObexBluetoothProtocolInfo protocolInfo;
    protocolInfo.iTransport.Copy( KRFCOMMDesC );
    protocolInfo.iAddr.SetBTAddr(iDevice->BDAddr() );//Address of server bt device
   	protocolInfo.iAddr.SetPort(iDevice->STSPort() );//Service channel of server rfcomm

	TUUID uuid(KBTSDPPrintingStatus);
	iObexNullObject->Reset();
	iObexNullObject->SetTargetL(uuid.Des());

    // create channel
    iChannel = CObexClient::NewL( protocolInfo );
	iChannel->SetCallBack(*this);
	iChannel->Connect(*iObexNullObject, iStatus);

	iState = EStateConnecting;
	Activate();
}

//--------------------------------------------------------------------------------------------
//
// CBtStatusChannel::DoLeaveL
//
//--------------------------------------------------------------------------------------------
void CBtStatusChannel::DoLeaveL(TInt aError)
{
	LOG2("[CBtStatusChannel::DoLeaveL]\t aError %d at state %d", aError, iState); 

	if(KErrNone == aError)
		return;
	
	if(EStateDisconnecting == iState)
		return;
	
	if(KErrCancel == aError)
		return;
	
	if(KErrCompletion == aError)
		return;
	
	LOG("[CBtStatusChannel::DoLeaveL]\t Leave"); 
	User::Leave(aError);
}

//  End of File
