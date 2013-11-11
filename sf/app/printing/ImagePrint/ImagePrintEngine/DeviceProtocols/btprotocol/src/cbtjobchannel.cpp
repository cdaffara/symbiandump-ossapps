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
* Description:  
*
*/


/**
 *  @file CBtJobChannel.h
 *	@brief	Defines the CBtJobChannel class.
 */

#include <obex.h>

#include "crsbtdevice.h"
#include "cbtjobchannel.h"
#include "cbtsoapcodec.h"
#include "clog.h"
#include "tbtmapper.h"
#include "cbtdiscover.h"
#include "printmessagecodes.h"
#include "cbtstatuschannel.h"
#include "rsutils.h"
#include "cbttimer.h"
#include "btprotocolconsts.h"

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::NewL
//
//--------------------------------------------------------------------------------------------
CBtJobChannel* CBtJobChannel::NewL(MBtPCObserver& aObs)
{
		CBtJobChannel *self = NewLC(aObs);
		CleanupStack::Pop(); // self

		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::NewLC
//
//--------------------------------------------------------------------------------------------
CBtJobChannel* CBtJobChannel::NewLC(MBtPCObserver& aObs)
{
		CBtJobChannel *self = new (ELeave) CBtJobChannel(aObs);
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CBtJobChannel
//
//--------------------------------------------------------------------------------------------
CBtJobChannel::CBtJobChannel(MBtPCObserver& aObs):
	CBtChannelBase(aObs),
	iJobFile(KNullDesC8()),
	iJobFileNum(0),
	iImageCount(0),
	iFileObjectSize(0),
	iObexFileObject(NULL),
	iPercents(0),
	iTimer(NULL),
	iStatusChannel(NULL)
{
	LOG("[CBtJobChannel::CBtJobChannel]\t");
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::~CBtJobChannel
//
//--------------------------------------------------------------------------------------------
CBtJobChannel::~CBtJobChannel()
{
	LOG("[CBtJobChannel::~CBtJobChannel]\t");
	Stop();

	// ignore leave: if cleaning fails, no can do.
	TRAP_IGNORE(CleanXhtmlDirL())
	
	if(iObexFileObject)
		delete iObexFileObject;
	
	if(iTimer)
		delete iTimer;
	
	if(iStatusChannel)
	{
		delete iStatusChannel;
		iStatusChannel = NULL;
	}
	
	// deleting iChannel here because review-x report error.
	if(iChannel)
	{
		delete iChannel;
		iChannel = NULL;
	}

	iImageList.Reset();
	iImageList.Close();
	
	if (iUpdateProgress)
		{
		delete iUpdateProgress;
		iUpdateProgress = NULL;
		}

	LOG("[CBtJobChannel::~CBtJobChannel]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::ConstructL()
{
	LOG("[CBtJobChannel::ConstructL]\t begin");

	CBtChannelBase::ConstructL();

	iObexFileObject = CObexFileObject::NewL();
	iStatusChannel = CBtStatusChannel::NewL(iObs);

	iState = EStateNone;
	iPercents = 0;
	iImageCount = 0;
	
	// Create periodic timer to update progress of printing. 
	iUpdateProgress = CPeriodic::NewL( CActive::EPriorityStandard );
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::RunL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::RunL()
{
	LOG2("[CBtJobChannel::RunL]\t BEGIN iStatus %d at state %d", iStatus.Int(), iState);

	 //Finish if required
	if( EStateFinish == iState )
	{
		ClearConnection();
		return;
	}

	// leave if "not allowed" error
	DoLeaveL( iStatus.Int() );

	switch( iState )
	{ 
		case EStateConnecting:
			RemoveObexCmdTimeOut();
			iObexNullObject->Reset();
			LOG("[CBtJobChannel::RunL]\t EStateConnecting");
			StopWaiting();
			break;
        case EStateDisconnecting:
			StopWaiting();
			// Go to disconnection again in case it is not finished yet...
			Disconnect();
			break;
		case EStateGettingCapabilities:
			GetSoapResponseL();
			GetPrinterCapabilitiesResponseL();

			StopWaiting();

			iState = EStateNone;
			break;
		case EStateGettingPrinterState:
			GetSoapResponseL();
			GetPrinterStateResponseL();

			iState = EStateNone;
			break;
		case EStateCreatingJob:
		{
			GetSoapResponseL();
			TInt res = CreateJobResponseL();
			StopWaiting();
			// No reason to send doc in error case
			if(KErrNone == res)
			{
				LOG("[CBtJobChannel::RunL]\t CreateJobResponseL() returns %d");
				SendXhtmlDocumentL();
			}
		}
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

			iState = EStateNone;
			break;
		case EStateSendingXhtml:
			iState = EStateNone;
			iObexBufObject->Reset();
			iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, ENoMessage, iStatus.Int());
			break;
		case EStateSendingOPP:						
			SendOPPL();
			break;
		default:
			break;		
	}
	LOG("[CBtJobChannel::RunL]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::RunError
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::RunError(TInt aError)
	{
	LOG1("[CBtJobChannel::RunError]\t state %d", iState);
	LOG2("[CBtJobChannel::RunError]\t error %d, iStatus %d", aError, iStatus.Int() );

	TInt id = KErrNotFound;

	StopWaiting();

	switch ( iState )
		{
		case EStateGettingPrinterState:
			if(iDevice)
				id = iDevice->DeviceId();

			iState = EStateFinish;
			iObs.GetPrinterStateResponse(EBtStateIdle, ENoMessage, aError, id);
			break;
		case EStateCreatingJob:
			iState = EStateFinish;
			iObs.CreateJobResponse(KErrNotFound, aError);
			break;
		case EStateCancellingJob:
			iState = EStateFinish;
			iObs.CancelJobResponse(aError);
			break;
		case EStateGettingJobAttributes:
			iState = EStateFinish;
			iObs.GetJobAttributesResponse(iJobId, KNullDesC8(), KNullDesC8(), KErrNotFound, KErrNotFound, aError);
			break;
		case EStateSendingXhtml:
		case EStateSendingOPP:
		default:
			iState = EStateFinish;
			iObs.GetEventResponse(KErrNotFound, KErrNotFound, KErrNotFound, KErrNotFound, aError);
			LOG("[CBtJobChannel::RunError]\t GetEventResponse sent" );
			break;
		}

	iResponseError = aError;
	return KErrNone; // othervise error returns to CActive: that's what we don't want.
	}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::DoCancel
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::DoCancel()
{
	LOG2("[CBtJobChannel::DoCancel]\t at state %d, iStatus 0x%X", iState, iStatus.Int());

	StopWaiting();

	iState = EStateNone;
	
	CBtChannelBase::DoCancel();
	iStatusChannel->Cancel();
	LOG1("[CBtJobChannel::DoCancel]\t out iStatus 0x%X", iStatus.Int());
}

//------------------------------
// InCalls
//------------------------------


//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::HandleTimeoutL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::HandleTimeoutL()
{
	LOG1("[CBtJobChannel::HandleTimeoutL]\t at state %d", iState);

	if(EStateSendingOPP != iState)
		return;
	
	TInt state(EBtStateSending);
	if( 100 == iPercents )
	{
		state = EBtStateCompleted;
	}
	else if(iTimer)
		iTimer->Start(KSentPercentsDelay);

	iObs.GetEventResponse(iJobId, state, EBtStatePrinting, KErrNone, KErrNone);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetPercentSent
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::GetPercentSent()
{
	LOG1("[CBtJobChannel::GetPercentSent]\t at state %d", iState);

	iPercents = 100;
	
	if(EStateSendingOPP == iState)
	{	
		// Store current bytes sent. 
		TReal32 bytesSent = iObexFileObject->BytesSent();
	
		TInt temp = iProgressOfFullImages - iFileObjectSize;
		iPercents = ( 100 * ( ( temp + bytesSent ) / iJobSize) );

		LOG2("[CBtJobChannel::GetPercentSent]\t iJobId %d, iImageCount: %d", iJobId, iImageCount);
		LOG2("[CBtJobChannel::GetPercentSent]\t BytesSent %d, iFileObjectSize %d", iObexFileObject->BytesSent(), iFileObjectSize);
	}
	
	LOG1("[CBtJobChannel::GetPercentSent]\t at state %d", iPercents);
	return iPercents;	
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CreateJobL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::CreateJobL(CPrintJob& aJob)
	{
	LOG("[CBtJobChannel::CreateJob]\t begin");

	if(!IsConnected())
		User::Leave( KErrDisconnected );

	Cancel();

	iProgressOfFullImages = 0;

	++iJobFileNum;
	iJobFile.Format(KXHTMLFileName8(), iJobFileNum);
	aJob.GetPrintFileL(iJobFile);

	iImageList.Reset();
	aJob.Images(iImageList);

	iJobSize = CalculateJobSizeL();

	iImageCount = iImageList.Count();
	LOG1("[CBtJobChannel::CreateJob]\t iImageList.Count(): %d", iImageList.Count());

	iJobId = KErrNotFound;
	iResponseError = KErrNone;

	LOG1("[CBtJobChannel::CreateJob]\t aDevice.UsedProtocol() %d", iDevice->UsedProtocol());
	if(KImagePrint_PrinterProtocol_BPP == iDevice->UsedProtocol())
		{
		iState = EStateCreatingJob;
		SendSoapRequestL(EBtCreateJob, iJobFileNum);
		// do asynchronously to indicate to start the object server
		WaitL();
	}
	else
	{
		iState = EStateSendingOPP;
		iJobId = 1;
		SendOPPL();
		Activate();
	}

	LOG1("[CBtJobChannel::CreateJob]\t Request sent. iResponseError: %d", iResponseError);
	if( KErrNone != iResponseError )
		{
		User::Leave( iResponseError );
		}
	LOG("[CBtJobChannel::CreateJob]\t out");
	}


//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetCapabilitiesL
//
// Fully independent and synchronous method. Does not require any existing 
// connections and all connections will be deleted after execution.
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::GetCapabilitiesL(CRsBtDevice& aDevice)
{
	LOG("[CBtJobChannel::GetCapabilitiesL]\t");
	Cancel();

	if( KImagePrint_PrinterProtocol_BPP != aDevice.UsedProtocol() )
		User::Leave(KErrNotSupported);

	if( !RsUtils::IsBluetoothOn() )
		User::Leave(KErrCancel);
	
	CRsBtDevice* tmpDevice = iDevice;
	iDevice = &aDevice;
	TBool connected = IsConnected();
	iResponseError = KErrNone;

	ConnectL();
	WaitL();
		
	LOG1("[CBtJobChannel::GetCapabilitiesL]\t ConnectL done: %d", IsConnected());

	iState = EStateGettingCapabilities;
	SendSoapRequestL(EBtGetPrinterAttributes);
	LOG1("[CBtJobChannel::GetCapabilitiesL]\t SendSoapRequestL done: %d", IsConnected());

	WaitL();
	
	if(!connected)
	{
		ClearConnection();
	}
	iDevice = tmpDevice;
	tmpDevice = NULL;

	LOG("[CBtJobChannel::GetCapabilitiesL]\t OUT");
	User::LeaveIfError(iResponseError);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetPrinterStateL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::GetPrinterStateL()
{
	LOG1("[CBtJobChannel::GetPrinterStateL]\t begin for protocol %d", iDevice->UsedProtocol());
	if(KImagePrint_PrinterProtocol_BPP != iDevice->UsedProtocol())
	{
		iObs.GetPrinterStateResponse(EBtStatePrinting, ENoMessage, KErrNone, iDevice->DeviceId());
		return;
	}
	
	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	iStatusChannel->StartL(*iDevice);
	iStatusChannel->GetPrinterStateL();
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetJobAttributesL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::GetJobAttributesL(TInt aJobId)
{
	LOG("[CBtJobChannel::GetJobAttributesL]\t begin");
	
	if(!iDevice)
	{
	    User::Leave(KErrDisconnected);   
	}
	
	LOG1("[CBtJobChannel::GetJobAttributesL]\t begin for protocol %d", iDevice->UsedProtocol());
	if(KImagePrint_PrinterProtocol_BPP != iDevice->UsedProtocol())
	{
		TInt state(EBtStateSending), completed(0);
		if(100 == iPercents)
		{
			state = EBtStateCompleted;
			completed = iImageCount;
		}
			
		iObs.GetJobAttributesResponse(state, KNullDesC8(), KNullDesC8(), completed, 0, KErrNone);
		return;
	}
	
	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	LOG("[CBtJobChannel::GetJobAttributesL]\t start status channel if required");
	if(!iStatusChannel->IsConnected())
		iStatusChannel->StartL(*iDevice);

	LOG1("[CBtJobChannel::GetJobAttributesL]\t iStatusChannel->GetJobAttributesL(%d)", iJobId);
	iStatusChannel->GetJobAttributesL(aJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetEventL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::GetEventL(TInt aJobId)
{
	LOG("[CBtJobChannel::GetJobAttributesL]\t begin");
	if(KImagePrint_PrinterProtocol_BPP != iDevice->UsedProtocol())
	{
		TInt state(EBtStateSending);
		if(100 == iPercents)
		{
			state = EBtStateCompleted;
		}
			
		iObs.GetEventResponse(iJobId, state, EBtStatePrinting, KErrNone, KErrNone);
		return;
	}
	
	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	iStatusChannel->StartL(*iDevice);
	iStatusChannel->GetEventL(aJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CancelJobL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::CancelJobL(TInt aJobId)
{
	LOG("[CBtJobChannel::CancelJob(TInt)]\t begin");

	if(!IsConnected())
		User::Leave(KErrDisconnected);
	
	Cancel();

	iJobId = aJobId;

	iState = EStateCancellingJob;
	SendSoapRequestL(EBtCancelJob, iJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::SendOPPL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::SendOPPL()
	{
	LOG("[CBtJobChannel::SendOPPL]\t begin");
	TInt tmpNumberOfCopies(0);
	// Delete file object
	iObexFileObject->Reset();

	if(iTimer)
		iTimer->Cancel();

	// all sent
	if(0 >= iImageList.Count())
		{
		LOG1("[CBtJobChannel::SendOPPL]\t Finishing... iImageList.Count(): %d", iImageList.Count());

		iJobFile.Zero();
		iImageList.Reset();

		iState = EStateNone;

		ClearConnection();

		iObs.GetEventResponse(iJobId, EBtStateCompleted, EBtStateIdle, KErrNone, KErrNone);
		iJobId = KErrNotFound;

		if(iTimer)
			delete iTimer;
		iTimer = NULL;

		return;
		}

	LOG2("[CBtJobChannel::SendOPPL]\t iChannel: %d, iChannel->IsConnected(): %d", iChannel, iChannel->IsConnected());
	if(!IsConnected())
		User::Leave(KErrDisconnected);

	iState = EStateSendingOPP;

	TFileName fn;
	iImageList[0].GetFilePathL(fn);
	tmpNumberOfCopies = iImageList[0].Copies();

	LOG1("[CBtJobChannel::SendOPPL]\t init file object with \"%S\"", &fn);
	iObexFileObject->InitFromFileL(fn);
	// check that does user wants print more than one copy of the currently printing picture.
	if(tmpNumberOfCopies > 1)
		{
		tmpNumberOfCopies--;
		iImageList[0].SetCopies(tmpNumberOfCopies);
		}
	else
		{
		iImageList.Remove(0);
		}

	LOG81("[CBtJobChannel::SendOPPL]\t set type \"%S\"...", &KBtMimeTypeJpeg());
	iObexFileObject->SetTypeL(KBtMimeTypeJpeg());

	LOG1("[CBtJobChannel::SendOPPL]\t ...and name \"%S\"", &fn);
	iObexFileObject->SetNameL(fn);

	iFileObjectSize = iObexFileObject->Length();

	if(!iTimer)
		iTimer = CBtTimer::NewL(*this);

	iTimer->Start(KSentPercentsDelay);

	LOG("[CBtJobChannel::SendOPPL]\t put");
	iChannel->Put(*iObexFileObject, iStatus);

	iProgressOfFullImages = iProgressOfFullImages + iFileObjectSize;
	
	if (iUpdateProgress)
		{
			iUpdateProgress->Cancel();
		}
	
	// Start periodic timer to track data transfer process.
	// Event interval is setup to 1 second.
	iUpdateProgress->Start(5000, KProgressUpdateInterval, TCallBack( CheckProgress, this ));	
	
	iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, KErrNone, KErrNone);

	Activate();
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CheckProgress
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::CheckProgress( TAny *aObj )
	{
	CBtJobChannel* obj = (CBtJobChannel*)aObj;
	obj->UpdateProgress();
	return 1;
	}
//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::UpdateProgress
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::UpdateProgress()
	{
	TInt percentBuffer = GetPercentSent();	
	
	// If all data has been sent, it's OK to stop timer 
	if ( percentBuffer == 100 )
		{
			iUpdateProgress->Cancel();
		}
	
	// Update progress bar using MBtPCObserver. 
	iObs.OPPSendProgress(percentBuffer);
	}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::SendXhtmlDocumentL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::SendXhtmlDocumentL()
	{
	LOG("[CBtJobChannel::SendXhtmlDocumentL]\t begin");

	if( !iDevice || !iChannel || !iChannel->IsConnected() )
		User::Leave( EObexConnectError );

	iState = EStateSendingXhtml;

	TFileName fn;
	fn.Copy( iJobFile );

	iObexFileObject->Reset();

	LOG("[CBtJobChannel::SendXhtmlDocumentL]\t fill in iObexBufObject");
	iObexFileObject->InitFromFileL( fn );

	TBuf8<50> mime;
	mime.Copy( KBtMimeTypeXhtml() );
	mime.ZeroTerminate();
	iObexFileObject->SetTypeL( mime );

	// name initialized automatically...

	// Job ID (application parameters)
	TUint32 jobId = iJobId;
	TBuf8<20> triplet;

	triplet.Zero();
	triplet.Append((TChar)KBtObexTagJobId);
	triplet.Append((TChar)KBtDataLength4);
	//append value
	triplet.Append((TChar) ((jobId & 0xFF000000) >> 24) );
	triplet.Append((TChar) ((jobId & 0xFF0000) >> 16) );
	triplet.Append((TChar) ((jobId & 0xFF00) >> 8) );
	triplet.Append((TChar) (jobId & 0xFF) );
	triplet.ZeroTerminate();

#ifdef _DEBUG
	const TUint8* aPtr = triplet.Ptr();
	_LIT(KBTHexFormatString,"%02x");
	TBuf<50> line;
	for (TInt j=0; j<triplet.Length(); j++)
		line.AppendFormat(KBTHexFormatString,aPtr[j]);
	LOG1("[CBtJobChannel::SendXhtmlDocumentL]\t line: %S", &line);
#endif

	iObexFileObject->SetAppParamL(triplet);

	LOG("[CBtJobChannel::SendXhtmlDocumentL]\t putting...");

	iChannel->Put(*iObexFileObject, iStatus);

	Activate();
	LOG("[CBtJobChannel::SendXhtmlDocumentL]\t end");
}

//------------------------------
// Responses
//------------------------------
TInt CBtJobChannel::CreateJobResponseL()
{
	LOG("[CBtJobChannel::CreateJobResponseL]\t ");
	User::LeaveIfNull(iSoapCodec);

	iJobId = KErrNotFound;
	TInt err = KErrGeneral;

	iSoapCodec->CreateJobResponseL(iJobId, err);
	iResponseError = err;

	iObs.CreateJobResponse(iJobId, err);
	LOG("[CBtJobChannel::CreateJobResponseL]\t out");
	return err;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetPrinterStateResponseL
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::GetPrinterStateResponseL()
{
	LOG("[CBtJobChannel::GetPrinterStateResponseL]\t");
	User::LeaveIfNull(iSoapCodec);

	TInt state = EBtStateUnknown;
	TInt reasons = EPbCheckPrinter;
	TInt opStatus = KErrGeneral;

	iSoapCodec->GetPrinterStateResponseL(state, reasons, opStatus);
	iObs.GetPrinterStateResponse(state, reasons, opStatus, iDevice->DeviceId());

	LOG("[CBtJobChannel::GetPrinterAttributesResponseL]\t out");
	return opStatus;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetPrinterCapabilitiesResponseL
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::GetPrinterCapabilitiesResponseL()
{
	LOG("[CBtJobChannel::GetPrinterCapabilitiesResponseL]\t");
	User::LeaveIfNull(iSoapCodec);

	RArray<TPrintCapability> capab;
	CleanupClosePushL(capab);
	TInt opStatus = KErrGeneral;

	iSoapCodec->GetPrinterCapabilitiesResponseL(capab, opStatus);

	if(iDevice)
	{
		for(TInt i = 0; i < capab.Count(); ++i)
		{
			iDevice->AddCapability(capab[i]);
		}
		LOG("[CBtJobChannel::GetPrinterCapabilitiesResponseL]\t returning to iObs...");
	}
	else
	{
		opStatus = KErrNotFound;
	}

	CleanupStack::PopAndDestroy(&capab);
	LOG("[CBtJobChannel::GetPrinterCapabilitiesResponseL]\t out");
	return opStatus;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::GetJobAttributesResponseL
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::GetJobAttributesResponseL()
{
	LOG("[CBtJobChannel::GetJobAttributesResponseL]\t");
	User::LeaveIfNull(iSoapCodec);

	TInt state, sheets, pendingJobs, opStatus;
	TBtAttrString8 jobName;
	TBtAttrString8 originator;

	iSoapCodec->GetJobAttributesResponseL( iJobId, state, jobName, originator,sheets, pendingJobs, opStatus);
	iObs.GetJobAttributesResponse(state, jobName, originator,sheets, pendingJobs, opStatus);

	return opStatus;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CancelJobResponseL
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::CancelJobResponseL()
{
	LOG("[CBtJobChannel::CancelJobResponseL]\t");
	User::LeaveIfNull(iSoapCodec);

	TInt err = KErrNotFound;
	iSoapCodec->CancelJobResponseL(iJobId, err);

	LOG1("[CBtJobChannel::CancelJobResponseL]\t err %d", err);
	iObs.CancelJobResponse(err);

	return err;
}

///////////////////////////////
// Private
///////////////////////////////
//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::ConnectL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::ConnectL()
	{
	LOG("[CBtJobChannel::ConnectL]\t");

	User::LeaveIfNull( iDevice );

	if( IsConnected() )
		{
		LOG1("[CBtJobChannel::ConnectL]\t return isConnected: %d", IsConnected());
    	return;
    }

	// delete old if exists
    if ( iChannel )
		{
		delete iChannel;
		iChannel = NULL;
		}

	iState = EStateConnecting;

	// set common parts of protocol info
	TObexBluetoothProtocolInfo protocolInfo;
	protocolInfo.iTransport.Copy( KRFCOMMDesC );
	protocolInfo.iAddr.SetBTAddr( iDevice->BDAddr() );//Address of server bt device

	// connect profile specifically
	LOG1("[CBtJobChannel::ConnectL]\t aDevice.UsedProtocol(): %d", iDevice->UsedProtocol());
	if( KImagePrint_PrinterProtocol_BPP == iDevice->UsedProtocol() )
		{
		LOG("[CBtJobChannel::ConnectL]\t connect BPP");
		protocolInfo.iAddr.SetPort( iDevice->BPPPort() );//Service channel of server rfcomm

		TUUID uuid( KBTSDPDirectPrinting );
		iObexNullObject->Reset();
		iObexNullObject->SetTargetL( uuid.Des() );

		// create channel
		iChannel = CObexClient::NewL( protocolInfo );
		iChannel->SetCallBack( *this );
		iChannel->Connect( *iObexNullObject, iStatus );
		}
	else
		{
		LOG("[CBtJobChannel::ConnectL]\t connect OPP");
		protocolInfo.iAddr.SetPort(iDevice->OPPPort());

		// create channel
		iChannel = CObexClient::NewL( protocolInfo );
		iChannel->SetCallBack(*this);	
		iChannel->SetCommandTimeOut(KObexCmdTimeOut);			
		iChannel->Connect(iStatus);
	}

   	Activate();

	LOG("[CBtJobChannel::ConnectL]\t out.");
	return;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::IsError
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::DoLeaveL(TInt aError)
{
	LOG2("[CBtJobChannel::DoLeaveL]\t aError %d at state %d", aError, iState); 

	if(KErrNone == aError)
		return;
	
	if(EStateDisconnecting == iState)
		return;
	
	// obex connection objects returns 'KErrCompletion' at destructor. Ignore.
	if(KErrCompletion == aError)
		return;
	
	LOG("[CBtJobChannel::DoLeaveL]\t Leave"); 
	User::Leave(aError);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::ClearConnection
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::ClearConnection()
{
	LOG1("[CBtJobChannel::ClearConnection]\t iState %d", iState);
	
    if(iStatusChannel)
    {
		LOG("[CBtJobChannel::ClearConnections]\t **** iStatusChannel stop");
		iStatusChannel->Stop();
    }

	CBtChannelBase::ClearConnection();

	iObexFileObject->Reset();

	iState = EStateNone;

	iUpdateProgress->Cancel();
	
	iImageCount = 0;
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CleanXhtmlDirL
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::CleanXhtmlDirL()
{
	LOG1("[CBtJobChannel::CleanXhtmlDirL]\t iJobFileNum %d", iJobFileNum);
	
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	
	while(0 < iJobFileNum)
	{
		TFileName fn;
		fn.Format(KXHTMLFileName, iJobFileNum);
		TInt err = fm->Delete(fn);
		LOG2("[CBtJobChannel::CleanXhtmlDirL]\t delete file %S, err %d", &fn, err);
		--iJobFileNum;
	}

	CleanupStack::PopAndDestroy( 2 );	// fm, fs
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::RemoveObexCmdTimeOut
//
//--------------------------------------------------------------------------------------------
void CBtJobChannel::RemoveObexCmdTimeOut()
{
	iChannel->SetCommandTimeOut(0);
}

//--------------------------------------------------------------------------------------------
//
// CBtJobChannel::CalculateJobSizeL
//
//--------------------------------------------------------------------------------------------
TInt CBtJobChannel::CalculateJobSizeL()
	{
	
	TInt imgCount= iImageList.Count();	
	TInt size = 0;
	
	for (TInt i = 0; i<imgCount; i++)
		{	
		TFileName fn;
		iImageList[i].GetFilePathL(fn);
		iObexFileObject->InitFromFileL(fn);
		TInt tempSize = iObexFileObject->Length();
		iObexFileObject->Reset();	
		size = size + tempSize;
		}
	return size;
	}

//  End of File
