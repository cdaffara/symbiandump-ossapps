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
* Description:  Defines the CBtPrinterController class.
*
*/


#include <obex.h>

#include "crsbtdevice.h"
#include "cbtprintercontroller.h"
#include "cbtsoapcodec.h"
#include "clog.h"
#include "tbtmapper.h"
#include "cbtdiscover.h"
#include "printmessagecodes.h"
#include "cbtstatuschannel.h"
#include "cbtjobchannel.h"
#include "rsutils.h"
#include "btnotifierapi.h"

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::NewL
//
//--------------------------------------------------------------------------------------------
CBtPrinterController* CBtPrinterController::NewL(MBtPCObserver& aObs)
{
		CBtPrinterController *self = NewLC(aObs);
		CleanupStack::Pop(); // self

		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::NewLC
//
//--------------------------------------------------------------------------------------------
CBtPrinterController* CBtPrinterController::NewLC(MBtPCObserver& aObs)
{
		CBtPrinterController *self = new (ELeave) CBtPrinterController(aObs);
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CBtPrinterController
//
//--------------------------------------------------------------------------------------------
CBtPrinterController::CBtPrinterController(MBtPCObserver& aObs):
	iObs( aObs ),
	iJobChannel(NULL),
	iObjectChannel(NULL)
{
	LOG("[CBtPrinterController::CBtPrinterController]\t");
}


//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::~CBtPrinterController
//
//--------------------------------------------------------------------------------------------
CBtPrinterController::~CBtPrinterController()
{
	LOG("[CBtPrinterController::~CBtPrinterController]\t");

	ClearConnections();

	if(iBtDiscoverEngine)
	{
		delete iBtDiscoverEngine;
		iBtDiscoverEngine = NULL;
	}

	if(iObjectChannel)
	{
		delete iObjectChannel;
		iObjectChannel = NULL;
	}

	if(iJobChannel)
	{
		delete iJobChannel;
		iJobChannel = NULL;
	}

	delete iQuery;
	
	iImageList.Reset();
	iImageList.Close();

}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::ConstructL()
{
	LOG("[CBtPrinterController::ConstructL]\t begin");
	iJobId = KErrNotFound;

	iBtDiscoverEngine = CRsBtDiscoverEngine::NewL(*this);

	LOG("[CBtPrinterController::ConstructL]\t channels");
	iObjectChannel = NULL;
	iJobChannel = CBtJobChannel::NewL(iObs);

	iQuery = NULL;
	iProtocol = 0;

	InitJobVariables();
}


//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::ConstructL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrinterController::GetPercentSent()
{
	LOG("[CBtPrinterController::GetPercentSent]\t");

	return iPercents;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::Stop
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::Stop()
{
	LOG("[CBtPrinterController::Stop]\t");

	iJobChannel->Cancel();

	ClearConnections();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::IsConnected
//
//--------------------------------------------------------------------------------------------
TBool CBtPrinterController::IsConnected()
{
	LOG("[CBtPrinterController::IsConnected]\t");
	TBool connected = EFalse;

	if(iJobChannel && iJobChannel->IsConnected())
	{
		connected = ETrue;
		LOG1("[CBtPrinterController::IsConnected]\t iJobChannel->IsConnected() %d", connected);
	}

	if(iObjectChannel && iObjectChannel->IsConnected())
	{
		connected = ETrue;
		LOG1("[CBtPrinterController::IsConnected]\t iObjectChannel->IsConnected() %d", connected);
	}

	LOG1("[CBtPrinterController::IsConnected]\t return %d",connected);
	return connected;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::FinishPrinting
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::FinishPrinting()
{
	LOG("[CBtPrinterController::FinishPrintingL]\t begin");

	// Already finishing
	if(iFinish)
	{
		LOG("[CBtPrinterController::FinishPrintingL]\t already finishing");
		return;
	}

	iFinish = ETrue;

	Stop();

	LOG("[CBtPrinterController::FinishPrintingL]\t end");
    return;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::StartDiscoveryL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::StartDiscoveryL( TUint aProtocol)
{
    LOG1("[CBtPrinterController::StartDiscoveryL]\t aProtocol: %d", aProtocol);

	iProtocol = aProtocol;
	iQuery = CBtOnQuery::NewL( *this );
	iQuery->IsBluetoothOn();

	LOG("CBtPrinterController::StartDiscoveryL end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CancelDiscoveryL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::CancelDiscovery()
{
	LOG("[CBtPrinterController::CancelDiscovery]\t begin");
	iBtDiscoverEngine->Stop();

	LOG1( "[CBtPrinterController::CancelDiscovery]\t iBtDiscoverEngine->IsActive() = %d", iBtDiscoverEngine->IsActive() );

	Stop();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::GetCapabilitiesL
//
// Fully independent and synchronous method. Does not require any existing
// connections and all connections will be deleted after execution.
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::GetCapabilitiesL(CRsBtDevice& aDevice)
{
	LOG("[CBtPrinterController::GetCapabilitiesL]\t");
	iJobChannel->Cancel();
	iJobChannel->GetCapabilitiesL(aDevice);
	LOG("[CBtPrinterController::GetCapabilitiesL]\t out");
}


//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CreateJobL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::CreateJobL(CRsBtDevice& aDevice, CPrintJob& aJob)
{
	LOG("[CBtPrinterController::CreateJobL]\t begin");

	iJobChannel->Cancel();

	iImageList.Reset();
	aJob.Images(iImageList);
	iImageCount = iImageList.Count();
	LOG1("[CBtPrinterController::CreateJobL]\t iImageList.Count(): %d", iImageList.Count());
	if(0 == iImageCount)
		User::Leave(KErrInvalidData);

	iJobId = KErrNotFound;

	iJobChannel->StartL(aDevice);

	iJobChannel->CreateJobL(aJob);
	LOG("[CBtPrinterController::CreateJobL]\t CreateJobL done");

	LOG1("[CBtPrinterController::CreateJobL]\t aDevice.UsedProtocol() %d", aDevice.UsedProtocol());
	if(KImagePrint_PrinterProtocol_BPP == aDevice.UsedProtocol())
	{
		LOG("[CBtPrinterController::CreateJobL]\t before establishing");
		EstablishObjectChannelL( aDevice );
	}
	else
	{
		iJobId = 1;
	}

	LOG("[CBtPrinterController::CreateJobL]\t out");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::GetPrinterStateL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::GetPrinterStateL(CRsBtDevice& aDevice)
{
	LOG("[CBtPrinterController::GetPrinterStateL]\t begin");
	iJobChannel->Cancel();

	if(!iJobChannel->IsConnected())
	{
		iJobChannel->StartL(aDevice);
	}

	iJobChannel->GetPrinterStateL();

	LOG("[CBtPrinterController::GetPrinterStateL]\t OUT");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::GetJobAttributesL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::GetJobAttributesL(CRsBtDevice& aDevice, TInt aJobId)
{
	LOG("[CBtPrinterController::GetJobAttributesL]\t begin");

	if(!iJobChannel || !iJobChannel->IsConnected()) 
	    {
	    LOG("[CBtPrinterController::GetJobAttributesL]\t User::Leave(KErrInvalidSequence);");
	    User::Leave(KErrInvalidSequence);
	    }
		
	LOG("[CBtPrinterController::GetJobAttributesL]\t iJobId = aJobId;");
	iJobId = aJobId;

	LOG("[CBtPrinterController::GetJobAttributesL]\t iJobChannel->GetJobAttributesL(aJobId);");
	iJobChannel->GetJobAttributesL(aJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::CancelJobL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::CancelJobL(CRsBtDevice& aDevice, TInt aJobId)
{
	LOG("[CBtPrinterController::CancelJob(TInt)]\t begin");

	if(!iJobChannel || !iJobChannel->IsConnected())
		User::Leave(KErrInvalidSequence);

	iJobChannel->Cancel();

	iJobId = aJobId;
	
	iJobChannel->CancelJobL(iJobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::GetEventL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::GetEventL(TInt aJobId)
{
	LOG("[CBtPrinterController::GetEvent]\t begin");
	iJobId = aJobId;

	if(!iJobChannel || !iJobChannel->IsConnected())
		User::Leave(KErrInvalidSequence);

	iJobChannel->GetEventL(aJobId);
}

//----------------------------------

///////////////////////////////
// Private
///////////////////////////////
// From MRsBtDiscoverEngineObserver.
//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::HandleDiscoveryEvent
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::HandleDiscoveryEvent(TInt aEvent, TInt aErrorCode)
{
	LOG2("[CBtPrinterController::HandleDiscoveryEvent]\t aEvent: %d, aErrorCode: %d", aEvent, aErrorCode);

	switch ( aEvent )
	{
		case MBtDiscoveryObserver::EFoundBtPrinter:
			LOG("[CBtPrinterController::HandleBtDiscoverEngineEventL]\t EFoundBtPrinter");
			iObs.DeviceDiscovered(iBtDiscoverEngine->GetDevice());
			break;
		case MBtDiscoveryObserver::ERemoveBtPrinter:
			LOG("[CBtPrinterController::HandleBtDiscoverEngineEventL]\t ERemoveBtPrinter");
			iObs.DeviceDisappeared(iBtDiscoverEngine->GetDevice());
			break;
		case MBtDiscoveryObserver::EDoneFindingPrinters:
			LOG("[CBtPrinterController::HandleBtDiscoverEngineEventL]\t EDoneFindingPrinters");
			iObs.DiscoveryError(KErrNone);
			break;
		case MBtDiscoveryObserver::EErrorDuringDiscover:
			LOG("[CBtPrinterController::HandleBtDiscoverEngineEventL]\t EErrorDuringDiscover");
			iObs.DiscoveryError(aErrorCode);
			break;
		default:
			break;
	}
}

// From MBtObjectServerObserver
//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::HandleObjectServerEvent
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::HandleObjectServerEvent(TInt aStatus, TInt aInfo)
{
	LOG2("[CBtPrinterController::OperationStatus]\t aStatus: %d, aInfo: %d", aStatus, aInfo);

	switch( aStatus )
	{
		case KObjectServerStopped:
			LOG("[CBtPrinterController::OperationStatus]\t iObjectChannel stopped");
			iServerError = KErrNone;
			break;
	    case KObjectServerStarted:
			LOG("[CBtPrinterController::OperationStatus]\t KObjectServerStarted");
			iServerError = KErrNone;
			iPercents = 0;
			iSentImages = 0;
			break;
		case KObjectServerAllFilesSent: // not used!
			iServerError = KErrNone;
			break;
		case KObjectServerPackageSent:
			iServerError = KErrNone;
			iPercents = 100 * ((iSentImages * 100) + aInfo) / (iImageCount * 100);
			LOG2("[CBtPrinterController::OperationStatus]\t iPercents %d, aInfo %d", iPercents, aInfo);
			iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, ENoMessage, KErrNone);
			break;
		case KObjectServerFileSent:
			LOG("[CBtPrinterController::OperationStatus]\t KObjectServerOneFileSent");
			iServerError = KErrNone;

			iPercents = 100 * ((iSentImages * 100) + aInfo) / (iImageCount * 100);

			++iSentImages;
			LOG2("[CBtPrinterController::OperationStatus]\t sent images: %d/%d", iSentImages, iImageCount);
			if(iSentImages == iImageCount)
			{
				iObs.GetEventResponse(iJobId, EBtStatePrinting, EBtStatePrinting, ENoMessage, KErrNone);
			}
			else
			{
				iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, ENoMessage, KErrNone);
			}
			break;
		case KErrNone:
			break;
		case KObjectServerBTDeviceMismatch:
			LOG("[CBtPrinterController::OperationStatus]\t KObjectServerBTDeviceMismatch");
			break;
		case KObjectServerTransportUp:
			LOG("[CBtPrinterController::OperationStatus]\t KObjectServerTransportUp");
			iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, ENoMessage, KErrNone);
		case KObjectServerConnected:
			iServerError = KErrNone;
			break;
		case KObjectServerError:
			LOG1("[CBtPrinterController::OperationStatus]\t KObjectServerError: %d", aInfo);
			iServerError = aInfo;
			//break here: other messages takes care of correct actions
			break;
		case KObjectServerDisconnected:
			LOG1("[CBtPrinterController::OperationStatus]\t disconnected, iServerError %d", iServerError);
		case KObjectServerTransportDown:
			LOG1("[CBtPrinterController::OperationStatus]\t transport down. iServerError %d", iServerError);
			if(KErrNone != iServerError)
				iObs.GetEventResponse(iJobId, EBtStateSending, EBtStatePrinting, iServerError, KErrDisconnected);
			iServerError = KErrNone;
			break;
		default:
			LOG("[CBtPrinterController::OperationStatus]\t default...");
			break;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::InitSyncVariables
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::InitJobVariables()
{
	iPercents = 0;
	iImageCount = 0;
	iServerError = KErrNone;
	iFinish = EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::ClearConnections
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::ClearConnections()
{
	LOG("[CBtPrinterController::ClearConnections]\t ");

	if(iBtDiscoverEngine)
		iBtDiscoverEngine->Cancel();

	if(iObjectChannel)
	{
		iObjectChannel->Stop();
		LOG("[CBtPrinterController::ClearConnections]\t **** iObjectChannel stop");
	}

    if(iJobChannel)
    {
		LOG("[CBtPrinterController::ClearConnections]\t **** iJobChannel stop");
		iJobChannel->Disconnect();
    }

	InitJobVariables();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::EstablishObjectChannelL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::EstablishObjectChannelL( CRsBtDevice& aDevice )
{
	if( 0 == iImageList.Count())
		User::Leave(KErrInvalidSequence);

	LOG("[CBtPrinterController::EstablishObjectChannelL]\t create iObjectChannel");
	if(iObjectChannel)
	{
		delete iObjectChannel;
		iObjectChannel = NULL;
	}
	iObjectChannel = CBtObjectServer::NewL( aDevice.BDAddr(), iImageList, *this );
	iObjectChannel->StartL();

	iImageList.Reset();

}


//--------------------------------------------------------------------------------------------
//
// CBtPrinterController::BTOnCompleteL
//
//--------------------------------------------------------------------------------------------
void CBtPrinterController::BTOnCompleteL( TBool aResult )
    {
   	LOG("[CBtPrinterController::BTOnComplete] begin \t ");
    
 	if( !aResult ) 
    {
		LOG("[CBtPrinterController::BTOnComplete]\t cancelling discovery, bt is not activated \t ");
    	CancelDiscovery();
    	iObs.DiscoveryError( ENoConnection );
		}
	

	iBtDiscoverEngine->Start( iProtocol );
   
   	LOG("[CBtPrinterController::BTOnComplete] end \t ");
    }

//  End of File
