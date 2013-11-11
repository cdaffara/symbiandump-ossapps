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
* Description:  Defines the CBtChannelBase class.
*
*/


#include <obex.h>

#include "crsbtdevice.h"
#include "cbtchannelbase.h"
#include "cbtsoapcodec.h"
#include "clog.h"
#include "tbtmapper.h"
#include "cbtdiscover.h"
#include "printmessagecodes.h"
#include "rsutils.h"

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::CBtChannelBase
//
//--------------------------------------------------------------------------------------------
CBtChannelBase::CBtChannelBase(MBtPCObserver& aObs):
	CActive(CActive::EPriorityStandard),
	iObs( aObs ),
	iState( EStateNone ),
	iJobId( KErrNotFound ),
	iChannel( NULL ),
	iSoapCodec( NULL ),
	iObexBufObject( NULL ),
	iObexNullObject( NULL ),
	iObexBody( NULL ),
	iDevice( NULL ),
	iWait( NULL ),
	iResponseError(KErrNone),
	iStop(EFalse)
{
	LOG("[CBtChannelBase::CBtChannelBase]\t");
	CActiveScheduler::Add(this);
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::~CBtChannelBase
//
//--------------------------------------------------------------------------------------------
CBtChannelBase::~CBtChannelBase()
{
	LOG("[CBtChannelBase::~CBtChannelBase]\t");

	if(iWait)
	{
		StopWaiting();
		delete iWait;
	}

	ClearConnection();

	if(iSoapCodec)
		delete iSoapCodec;

	if(iObexNullObject)
		delete iObexNullObject;

	if(iObexBufObject)
		delete iObexBufObject;

	if(iObexBody)
		delete iObexBody;

	if(iChannel)
	{
		delete iChannel;
		iChannel = NULL;
	}
	if (iPasswordQuery)
	{
		delete iPasswordQuery;
		iPasswordQuery = NULL;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::ConstructL()
{
	LOG("[CBtChannelBase::ConstructL]\t begin");

	iWait = new (ELeave) CActiveSchedulerWait;

	LOG("[CBtChannelBase::ConstructL]\t iSoapCodec");
	iSoapCodec = CBtSoapCodec::NewL();

	// Create obex objects here: do not NEVER delete them but in destructor
	iObexBody = CBufFlat::NewL(8);
	iObexBufObject = CObexBufObject::NewL(iObexBody);
	iObexNullObject = CObexNullObject::NewL();
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::SetDevice
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::SetJobId( TInt aId )
{
	iJobId = aId;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::Device
//
//--------------------------------------------------------------------------------------------
TInt CBtChannelBase::JobId() const
{
	return iJobId;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::SetDevice
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::SetDevice( CRsBtDevice& aDevice )
{
	iDevice = &aDevice;
	LOG1("[CBtChannelBase::SetDevice]\t iDevice %d", iDevice);
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::Device
//
//--------------------------------------------------------------------------------------------
const CRsBtDevice& CBtChannelBase::Device() const
{
	return *iDevice;
}


//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::StartL
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::StartL( CRsBtDevice& aDevice )
{
	LOG1("[CBtChannelBase::StartL]\t state %d", iState);

	User::LeaveIfNull(&aDevice);
	
	Stop();

	iStop = EFalse;

	TBool btOn = RsUtils::IsBluetoothOn();
	LOG1("[CBtChannelBase::StartL]\t btOn: %d", btOn);
	if( !btOn )
		User::Leave( EObexConnectError );

	SetDevice( aDevice );
	iState = EStateNone;
	ConnectL();
	WaitL();

	LOG("[CBtChannelBase::StartL]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::Stop
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::Stop()
{
	LOG("[CBtChannelBase::Stop]\t");

	iStop = ETrue;

	if( IsActive() )
	{
		Cancel();
	}
	else
		StopWaiting();
	
	ClearConnection();
	LOG("[CBtChannelBase::Stop]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::DoCancel
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::DoCancel()
{
	LOG("[CBtChannelBase::DoCancel]\t");

	StopWaiting();
	
	LOG1("[CBtChannelBase::DoCancel]\t iChannel %d", iChannel);
	if(iChannel)
	{
		LOG1("[CBtChannelBase::DoCancel]\t iStatus.Int(): 0x%X", iStatus.Int());
		if(KRequestPending == iStatus.Int())
		{
			DeleteChannel();
		}
	}
	
	LOG1("[CBtChannelBase::DoCancel]\t out iStatus 0x%X", iStatus.Int());
}

//------------------------------
// InCalls
//------------------------------

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::IsConnected
//
//--------------------------------------------------------------------------------------------
TBool CBtChannelBase::IsConnected()
{
	TBool connected = EFalse;

	if(iChannel && iChannel->IsConnected())
	{
		connected = ETrue;
	}

	LOG2("[CBtChannelBase::IsConnected]\t iChannel: %d, connected: %d", iChannel, connected);

	return connected;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::Disconnect
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::Disconnect()
{
	TBool conn = IsConnected();
	LOG1("[CBtChannelBase::Disconnect]\t IsConnected(): %d", conn);
	
	if( conn )
	{
		if(IsActive())
		{
			LOG("[CBtChannelBase::Disconnect]\t iChannel abort");
			iChannel->Abort();
		}
		else
		{
			LOG("[CBtChannelBase::Disconnect]\t iChannel disconnect");
			iState = EStateDisconnecting;
			iChannel->Disconnect(iStatus);
			Activate();
		}
		iStop = ETrue;
	}
	else
	{
		DeleteChannel();
		iState = EStateNone;
	}

	LOG1("[CBtChannelBase::Disconnect]\t out. iState: %d", iState);
	return;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::FinishPrinting
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::FinishPrinting()
{
	LOG("[CBtChannelBase::FinishPrintingL]\t begin");

	// Already finishing
	if(EStateFinish == iState)
	{
		LOG("[CBtChannelBase::FinishPrintingL]\t already finishing");
		return;
	}

	iState = EStateDisconnecting;

	LOG1("[CBtChannelBase::FinishPrintingL]\t IsActive() %d", IsActive());
	if(IsActive())
	{
		//return: the iState causes disconnection printing when RunL called.
		return;
	}

	IsConnected();
		Disconnect();

	LOG("[CBtChannelBase::FinishPrintingL]\t end");
    return;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::SendSoapRequestL
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::SendSoapRequestL(const TInt aRequest, const TInt aJobId)
{
	SendSoapRequestL(aRequest, iStatus, aJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::SendSoapRequestL
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::SendSoapRequestL(const TInt aRequest,
								  TRequestStatus& aStatus,
								  const TInt aJobId)
{
	LOG1("[CBtChannelBase::SendSoapRequestL]\t begin for %d", aRequest);
	User::LeaveIfNull(iChannel);
	User::LeaveIfNull(iObexBufObject); // panics if iObexBody == NULL

	/* Create soap from file */
	if(!iSoapCodec)
	{
		iSoapCodec = CBtSoapCodec::NewL();
	}
    TPtrC8 soap = iSoapCodec->CreateSoapL(aRequest, aJobId);

	LOG2("[CBtChannelBase::SendSoapRequestL]\t Soap created for %d. len: %d",aRequest, soap.Length());

	iObexBufObject->Reset();

	iObexBody->InsertL(0, soap);
	LOG1("[CBtChannelBase::SendSoapRequestL]\t data size %d", iObexBody->Size());

	iObexBufObject->SetTypeL(KBtMimeTypeSoap());

	LOG("[CBtChannelBase::SendSoapRequestL]\t Issuing get.");
	iChannel->Get(*iObexBufObject, aStatus);

	Activate();
	LOG("[CBtChannelBase::SendSoapRequestL]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::GetSoapResponseL
//
//--------------------------------------------------------------------------------------------
TInt CBtChannelBase::GetSoapResponseL()
{
	LOG("[CBtChannelBase::GetSoapResponseL]\t");

	if( 0 >= iObexBody->Size() )
		User::Leave(KErrInvalidData);

	LOG("[CBtChannelBase::GetSoapResponseL]\t Data in obex body.");
	LOG1("[CBtChannelBase::GetSoapResponseL]\t iDevice: %d", iDevice);
	TInt action = EBtUnknownAction;

	//give obexbody to parser
	if(!iSoapCodec)
	{
		iSoapCodec = CBtSoapCodec::NewL();
	}

   	//read
	iSoapCodec->ReadSoapL(action, *iObexBody);
	LOG1("[CBtChannelBase::GetSoapResponseL]\t soap read. Action %d", action);

	// Reset object: data is stored in iSoapCodec.
	// NOTE: reset resets also iObexBody
	iObexBufObject->Reset();

	LOG("[CBtChannelBase::GetSoapResponseL]\t done");
	return action;
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::Activate
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::Activate()
{
	// If already active do not activate again
	LOG2("[CBtChannelBase::Activate]\t for %d, IsActive %d",iState, IsActive());
	if( IsActive() )
		return;

	SetActive();
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::CompleteRequest
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::CompleteRequest(TControlState aStateCompleted, TInt aError)
{
	LOG2("[CBtChannelBase::CompleteRequest]\t aStateCompleted %d, aError %d",aStateCompleted, aError);
	iState = aStateCompleted;

	LOG2("[CBtChannelBase::CompleteRequest]\t IsActive() %d, iStatus %d", IsActive(), iStatus.Int());
	if(IsActive() && KRequestPending == iStatus.Int())
	{
		TRequestStatus* statusPtr = &iStatus;
		User::RequestComplete(statusPtr, aError);
	}
	if(EStateNone != aStateCompleted)
	{
		Activate();
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::StartWaiting
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::WaitL()
{
	if( !(iWait->IsStarted()) && KRequestPending == iStatus.Int())
	{
		LOG("[CBtChannelBase::WaitL]\t");
		iWait->Start();

		LOG2("[CBtChannelBase::WaitL]\t Continue, iStatus: %d at iState: %d", iStatus.Int(), iState);
		LOG1("[CBtChannelBase::WaitL]\t iStop: %d",iStop);
		if(KErrNone != iStatus.Int() && KRequestPending != iStatus.Int() && !iStop)
			User::Leave(iStatus.Int());
		LOG("[CBtChannelBase::WaitL]\t out");
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::StopWaiting
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::StopWaiting()
{
	if( (iWait->IsStarted()) )
	{
		LOG("[CBtChannelBase::StopWaiting]\t");
		iWait->AsyncStop();
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::ClearConnection
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::ClearConnection()
{
	LOG1("[CBtChannelBase::ClearConnection]\t iState %d", iState);

	Cancel();

	iObexBufObject->Reset();
	iObexNullObject->Reset();
	iDevice = NULL;

    if(iChannel)
    {
		LOG("[CBtChannelBase::ClearConnection]\t iChannel clear");
		//Sends the abort operation to receiver and completes request with KErrAbort
		Disconnect();	
    }
	LOG("[CBtChannelBase::ClearConnection]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::DeleteChannel
//
//--------------------------------------------------------------------------------------------
void CBtChannelBase::DeleteChannel()
{
	LOG1("[CBtChannelBase::DeleteChannel]\t iChannel %d delete", iChannel);
    if(iChannel)
    {
		delete iChannel;
		iChannel = NULL;
    }
	iStop = EFalse;

	LOG1("[CBtChannelBase::DeleteChannel]\t iChannel %d out", iChannel);
}

//--------------------------------------------------------------------------------------------
//
// CBtChannelBase::GetUserPasswordL
//
//--------------------------------------------------------------------------------------------

void CBtChannelBase::GetUserPasswordL( const TDesC &aRealm )
{
    LOG("[CBtChannelBase::GetUserPasswordL begin]\t");

    iPasswordQuery = CBtPasswordQuery::NewL( *this );
    
    iPasswordQuery->StartActiveObjectL();
  
    LOG("[CBtChannelBase::GetUserPasswordL end]\t");
}

void CBtChannelBase::PasswordQueryCompleteL ( TObexPasskeyBuf aResultPckg )
	{     
	    iPckg = aResultPckg(); 
	    
	    TBuf<256> data;
	    data.Copy(iPckg);
	    
	    // Send user password
	    iChannel->UserPasswordL(data);
	}

//  End of File
