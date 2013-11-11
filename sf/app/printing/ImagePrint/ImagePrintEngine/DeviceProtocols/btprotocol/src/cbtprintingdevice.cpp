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


//  EXTERNAL INCLUDES
#include <e32std.h>
#include <barsc.h>
#include <btprotocol.rsg>
#include <fbs.h>

#include "cbtprintingdevice.h"
#include "printcapabilitycodes.h"
#include "printmessagecodes.h"
#include "rsutils.h"
#include "crsbtdevice.h"
#include "cbtdevicecontainer.h"
#include "clog.h"
#include "tbtmapper.h"
#include "crsbtdiscoverengine.h"


//  CONSTANTS
namespace
	{
	/// Bluetooth BPP/OPP Version Major.
	const static TInt KBtVersionMajor = 3;
	/// Bluetooth BPP/OPP Version Minor.
	const static TInt KBtVersionMinor = 0;
	/// Bluetooth BPP/OPP Version Build.
	const static TInt KBtVersionBuild = 1;
	}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::NewL
//
//--------------------------------------------------------------------------------------------
MProtPrintingDevice* CBtPrintingDevice::NewL()
{
	return new (ELeave) CBtPrintingDevice();
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CBtPrintingDevice
//
//--------------------------------------------------------------------------------------------
CBtPrintingDevice::CBtPrintingDevice() :
	CActive(CActive::EPriorityStandard),
	iState( ENone ),
	iPrintingJob( NULL ),
	iPrinterController( NULL ),
	iDeviceContainer( NULL ),
	iRequestedProtocols( 0 ),
	iPrintError( KErrNone ),
	iPrintErrorMsgCode( 0 ),
	iPendingCount(KPendingLimit),
	iPrinterTimer( NULL ),
	iProggress( 0 )
{
	iDiscoveryObserver = NULL;
	iPrintEventObserver = NULL;
	CActiveScheduler::Add(this);
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::ConstructL(const TDesC& /*aDLLPath*/)
{
	LOG("CBtPrintingDevice ****************************************************");
	LOG("[CBtPrintingDevice::ConstructL]\t begin");

	iPrinterController 	= CBtPrinterController::NewL(*this);
	iDeviceContainer 	= CBtDeviceContainer::NewL();
	iPrinterTimer 		= CPeriodic::NewL(CActive::EPriorityStandard);
	LOG("CBtPrintingDevice::ConstructL end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::~CBtPrintingDevice
//
//--------------------------------------------------------------------------------------------
CBtPrintingDevice::~CBtPrintingDevice()
{
	LOG("CBtPrintingDevice::~CBtPrintingDevice begin");

	Deactivate();

	if(iPrinterTimer)
	{
		if(iPrinterTimer->IsActive())
			iPrinterTimer->Cancel();
		delete iPrinterTimer;
	}

	if(iPrinterController)
	{
		iPrinterController->Stop();
		delete iPrinterController;
	}

	if(iPrintingJob)
		delete iPrintingJob;

	if(iDeviceContainer)
		delete iDeviceContainer;

	REComSession::DestroyedImplementation( iDtor_ID_Key );

	LOG("CBtPrintingDevice::~CBtPrintingDevice end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::RunL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::RunL()
{
	LOG1("[CBtPrintingDevice::RunL]\t iStatus: %d", iStatus.Int());

	if(KErrNone != iStatus.Int())
	{
		User::Leave(iStatus.Int());
	}

	switch( iState )
	{
		case EFinish:
			FinishPrinting(iStatus.Int());
			break;
		case EGetJobAttributes:
			GetJobAttributesL();
			break;
		case EGetPrinterState:
			GetPrinterStateL();
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::RunError
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::RunError(TInt aError)
{
	LOG2("[CBtPrintingDevice::RunError]\t iStatus: %d, aError: %d", iStatus.Int(), aError);

	FinishPrinting(aError);

	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::DoCancel
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::DoCancel()
{
	LOG1("[CBtPrintingDevice::DoCancel]\t iStatus: %d", iStatus.Int());
	ResetTimer();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::Deactivate
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::Deactivate()
{
	LOG2("[CBtPrintingDevice::Deactivate]\t iStatus: %d iState %d", iStatus.Int(), iState);
	ResetTimer();
	Cancel();
	iStatus = KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::Version
//
//--------------------------------------------------------------------------------------------
TVersion CBtPrintingDevice::Version()
{
	return TVersion(KBtVersionMajor, KBtVersionMinor, KBtVersionBuild);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::SupportedProtocols
//
//--------------------------------------------------------------------------------------------
TUint CBtPrintingDevice::SupportedProtocols()
{
	return KImagePrint_PrinterProtocol_BPP |
		   KImagePrint_PrinterProtocol_OPP_Printer |
		   KImagePrint_PrinterProtocol_OPP_PC;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::DiscoveryError
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::DiscoveryError(TInt aError)
{
	LOG1("[CBtPrintingDevice::DiscoveryError]\t aError: %d", aError);
	if(0 < iDeviceContainer->Count())
	{
		iState = EReady;
	}
	else
	{
		iState = ENone;
	}

  	iPrinterController->CancelDiscovery();
  	Deactivate();

	if( iDiscoveryObserver )
	{
		if(KErrNone == aError)// discovery finished, update not-found cached devices
			UpdateCacheList();

		//We're not interested if the call back leaves
		TRAP_IGNORE(iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, aError));
	}

	LOG("[CBtPrintingDevice::DiscoveryError]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::StartDiscoveryL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocol)
{
	LOG1("[CBtPrintingDevice::StartDiscoveryL]\t aProtocol: %d", aProtocol);
	iDiscoveryObserver = &aObserver;

	// already discovering
	if(EDiscovering == iState)
		return;

	if((aProtocol & SupportedProtocols()))
	{
		iRequestedProtocols = aProtocol;
		iDeviceContainer->Reset();
		iPrinterController->Stop();

		// If GetUsedDevicesL fails, no can do. Just continue.
		LOG("[CBtPrintingDevice::StartDiscoveryL]\t Get used devices");
		TInt leave;
		TRAP(leave, GetUsedDevicesL() );
		LOG1("[CBtPrintingDevice::StartDiscoveryL]\t Get used devices leaves with %d", leave);

		LOG("[CBtPrintingDevice::StartDiscoveryL]\t Calling iPrinterController");
	  	iPrinterController->StartDiscoveryL(aProtocol);

		iState = EDiscovering;
	}
	else
	{
		LOG("CBtPrintingDevice::StartDiscoveryL NO protocol match, ending discovery...");
		if( iDiscoveryObserver )
		{
			//We're not interested if the call back leaves
			TRAP_IGNORE(iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNone));
		}
	}
	LOG("[CBtPrintingDevice::StartDiscoveryL]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::RemoveCachedDeviceL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::RemoveCachedDeviceL(TInt aDeviceID)
{
	LOG1("CBtPrintingDevice::RemoveCachedDeviceL aDeviceID: %d", aDeviceID);

	TInt err = KErrNone;
	CRsBtDevice* device = FindDevice(aDeviceID);
	if( device )
	{
		device->SetUsed(EFalse);
		iDeviceContainer->StoreL();
	}
	else
	{
		err = KErrInvalidData;
	}

	LOG1("CBtPrintingDevice::RemoveCachedDeviceL end with: %d", err);
	return err;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::UpdateCacheListL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::UpdateCacheList()
{
	LOG("[CBtPrintingDevice::UpdateCacheList]\t");

	for(TInt i = 0; i < iDeviceContainer->Count(); ++i)
	{
		CRsBtDevice* device = iDeviceContainer->At(i);
		if(device && !device->IsJustFound())
			DeviceDisappeared(*device);
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CancelDiscovery
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::CancelDiscovery(TBool aDoCallBack)
{
  	LOG1("[CBtPrintingDevice::CancelDiscovery]\t aDoCallBack: %d", aDoCallBack);
	if(0 < iDeviceContainer->Count())
	{
		iState = EReady;
	}
	else
	{
		iState = ENone;
	}
  	iPrinterController->CancelDiscovery();
  	Deactivate();

	if( aDoCallBack && iDiscoveryObserver )
	{
		//We're not interested if the call back leaves
		TRAP_IGNORE(iDiscoveryObserver->DiscoveryStatusL(EDiscoveryCancelling, KErrNone));
	}
	LOG("[CBtPrintingDevice::CancelDiscovery]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CreatePrintJobL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::CreatePrintJobL(TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver)
{
	LOG("[CBtPrintingDevice::CreatePrintJobL]\t begin");
  	LOG2("[CBtPrintingDevice::CreatePrintJobL]\t aDeviceID: %d, aImages.Count(): %d", aDeviceID, aImages.Count());

	iPrintEventObserver = &aObserver;

	CRsBtDevice *printer = FindDevice( aDeviceID );

	if( printer == NULL )
	{
		LOG("CBtPrintingDevice::CreatePrintJobL no device found, returning error");
		return KErrInvalidData;
	}
	if(iPrintingJob && 
	  (CPrintJob::EStateSubmitted == iPrintingJob->JobState() ||
	   CPrintJob::EStateSending == iPrintingJob->JobState() ||
	   CPrintJob::EStatePrinting == iPrintingJob->JobState()) )
	{
		LOG("[CBtPrintingDevice::CreatePrintJobL]\t Printing job already");
		return KErrInvalidSequence;
	}

	iPendingCount = 0;

	GetCapabilities(*printer);

	InitJobL(printer, aImages);

	LOG("[CBtPrintingDevice::CreatePrintJobL]\t job created");

	iPrintingJob->UpdateJobState(CPrintJob::EStateCreated);

	LOG1("[CBtPrintingDevice::CreatePrintJobL]\t end - status: %d", iState);

	return KErrNone;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::SubmitPrintJobL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::SubmitPrintJobL()
{
	LOG("[CBtPrintingDevice::SubmitPrintJobL] begin");

	TInt err = KErrInvalidSequence;
	if( iPrintingJob && iPrintingJob->JobState() == CPrintJob::EStateCreated )
	{
		// We try to find the Device specified in our list.
		CRsBtDevice* device = static_cast<CRsBtDevice*>(User::LeaveIfNull(
				FindDevice(iPrintingJob->PrinterId())));

		LOG1("[CBtPrintingDevice::SubmitPrintJobL] iPrintingJob.iPrinterID: %d", iPrintingJob->PrinterId() );
		// save used bluetooth profile
		iUsedBtProfile = device->UsedProtocol();
		if( !(device->UsedProtocol() & SupportedProtocols()) )
		{
			LOG("[CBtPrintingDevice::SubmitPrintJobL]\t can't select a protocol");
			err = KErrInvalidData;
		}
		else
		{
			LOG1("[CBtPrintingDevice::SubmitPrintJobL]\t Submitting to printer \"%S\"", &(device->FriendlyName()));
			iState = EJobProgress;

			StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
			iPendingCount = 0;
			iProggress = 0;

			TRAP(err, iPrinterController->CreateJobL(*device, *iPrintingJob) );
			LOG1("[CBtPrintingDevice::SubmitPrintJobL]\t CreateJobL leaves with %d", err);
			if(KErrNone == err)
			{
				device->SetUsed(ETrue);
				iDeviceContainer->StoreL();
				
				if( iPrintingJob->JobState() != CPrintJob::EStateSending &&
					iPrintingJob->JobState() != CPrintJob::EStatePrinting )
				{
					iPrintingJob->UpdateJobState(CPrintJob::EStateSubmitted);
				}
									
			}
			else
			{
					CreateJobResponse(KErrNotFound, err);
			}
		}
	}
	else
		LOG("[CBtPrintingDevice::SubmitPrintJobL]\t ***Invalid Sequence***. Job state wrong or job not created");

	if(KErrNone != err)
	{
		LOG1("[CBtPrintingDevice::SubmitPrintJobL]\t return error %d", err);
		if (iPrintEventObserver)
			iPrintEventObserver->PrintJobErrorEvent(err);
		else
			User::Leave(err);
	}
	LOG("[CBtPrintingDevice::SubmitPrintJobL] end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CancelPrintJob
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::CancelPrintJob()
{
	LOG("[CBtPrintingDevice::CancelPrintJob]\t begin");
	TInt retVal( KErrInvalidSequence );

	LOG1("[CBtPrintingDevice::CancelPrintJob]\t iPrintingJob: %d", iPrintingJob);

	if(iPrintingJob)
	{
        retVal = KErrNone;

		LOG1("[CBtPrintingDevice::CancelPrintJob]\t iPrintingJob->JobState(): %d", iPrintingJob->JobState());
		if(iPrintingJob->JobState() == CPrintJob::EStateCancel)
		{
			LOG("[CBtPrintingDevice::CancelPrintJob]\t Already cancelling. Return.");
			retVal = KErrCancel;
		}
		else if( iPrintingJob->JobState() == CPrintJob::EStateSubmitted || 
                 iPrintingJob->JobState() == CPrintJob::EStateSending || 
                 iPrintingJob->JobState() == CPrintJob::EStatePrinting )
		{
			// cancel job
			TInt leave; // for remove compiling errors
			TRAP(leave, CancelJobL());
			if( leave == KErrDisconnected )
			    {
			  	LOG("[CBtPrintingDevice::CancelPrintJob]\t calling OPPJobRollBack...");
			  	OPPJobRollBack();
				}
			LOG1("[CBtPrintingDevice::CancelPrintJob]\t CancelJobL() leaves with %d.", leave);
		}
		else
        {
			LOG1("[CBtPrintingDevice::CancelPrintJob]\t No print job, just cancel all (forced stop) if connections exist: %d", iPrinterController->IsConnected());
			FinishPrinting( KErrInvalidSequence );

		}
	}
	
	if( iPrintEventObserver )
	    {
		iPrintEventObserver->PrintJobProgressEvent( EDone, iPrinterController->GetPercentSent(), ECancelling );
        }

	LOG1("CBtPrintingDevice::CancelPrintJob end with: %d", retVal);
	return retVal;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::OPPJobRollBack
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::OPPJobRollBack()
{
    LOG("[CBtPrintingDevice::OPPJobRollBack ]\t begin");
    
    Cancel();
    TInt err = KErrNone;
    iState = EFinish;
    TRequestStatus* tmpStatus = &iStatus;
    User::RequestComplete( tmpStatus, err );
    SetActive();
    
    LOG("[CBtPrintingDevice::OPPJobRollBack ]\t end");
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::ContinuePrintJobL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::ContinuePrintJobL()
{
	LOG1("CBtPrintingDevice::ContinuePrintJobL returns with: %d", KErrNotSupported);
	return KErrNotSupported;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetNumPrintPages
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetNumPrintPages()
{
	LOG("[CBtPrintingDevice::GetNumPrintPages]\t");

	if( iPrintingJob )
		return iPrintingJob->Sheets();
	else
		return KErrInvalidSequence;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetPrintJobStatus
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetPrintJobStatus()
{
	LOG1("CBtPrintingDevice::GetPrintJobStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetPrinterStatus
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetPrinterStatus(TInt /*aDeviceID*/)
{
	LOG1("CBtPrintingDevice::GetPrinterStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetDeviceCapabilityIDsL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs)
{
	LOG1("[CBtPrintingDevice::GetDeviceCapabilityIDsL]\t begins with aDeviceID: %d", aDeviceID);

	TInt err = KErrNone;
	aCapabilityIDs.Reset();

	CRsBtDevice *printer = FindDevice( aDeviceID );
	if( printer == NULL )
	{
		LOG("[CBtPrintingDevice::GetDeviceCapabilityIDsL]\t no device found, returning error");
		return KErrInvalidData;
	}

	GetCapabilities(*printer);

	printer->GetCapabilityIDs(aCapabilityIDs);

	LOG1("[CBtPrintingDevice::GetDeviceCapabilityIDsL]\t ends with err: %d", err);
	return err;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetDeviceCapabilityL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability)
{
	LOG1("CBtPrintingDevice::GetDeviceCapabilityL begins with aDeviceID: %d", aDeviceID);
	LOG1("CBtPrintingDevice::GetDeviceCapabilityL begins with aCapabilityID: %d", aCapabilityID);

	TInt err = KErrNone;

	CRsBtDevice* printer = FindDevice(aDeviceID);
	if( printer )
	{
		printer->GetCapability(aCapabilityID, aCapability);
	}
	else
	{
		err = KErrInvalidData;
	}
	LOG1("CBtPrintingDevice::GetDeviceCapabilityL ends with err: %d", err);
	return err;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetJobSetting
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetJobSetting(TInt aCapabilityID, TInt& aValue)
{
	LOG1("CBtPrintingDevice::GetJobSetting begins with aCapabilityID: %d", aCapabilityID);

	TInt res = KErrInvalidSequence;
	aValue = KErrNotFound;

	if(iPrintingJob)
		res = iPrintingJob->GetPrintSetting(aCapabilityID, aValue);

	return res;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::SetJobSettingL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	LOG1("[CBtPrintingDevice::SetJobSettingL]\t begins with aCapabilityID: %d", aCapabilityID);
	LOG1("[CBtPrintingDevice::SetJobSettingL]\t begins with aValue: %d", aValue);
	LOG1("[CBtPrintingDevice::SetJobSettingL]\t begins with aAffectedCapability: %d", aAffectedCapability);

	if( !iPrintingJob || iPrintingJob->JobState() == CPrintJob::EStateNone )
	{
		LOG1("[CBtPrintingDevice::SetJobSettingL]\t iPrintingJob: %d || job state == EJobStateNone; return KErrInvalidSequence", iPrintingJob);
		return KErrInvalidSequence;
	}

	aAffectedCapability = 0;

	// Get the Target printer and capability
	CRsBtDevice* printer = FindDevice( iPrintingJob->PrinterId() );
	if( printer == NULL )
	{
		LOG1("[CBtPrintingDevice::SetJobSettingL]\t printer == NULL; return %d", KErrInvalidData);
		return KErrInvalidData;
	}

	TInt res = printer->SetDefaultCapability(aCapabilityID, aValue);
	if(KErrNone == res)
	{
		iPrintingJob->SetPrintSetting(aCapabilityID, aValue);
		aAffectedCapability = aCapabilityID;
		TRAPD(err, iDeviceContainer->StoreL());
		if(KErrNone != err)
		{
			LOG1("[CBtPrintingDevice::SetJobSettingL]\t Storing fails: %d", err);
		}
	}
	LOG1("[CBtPrintingDevice::SetJobSettingL]\t ends. aAffectedCapability: %d", aAffectedCapability);
	return res;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetNumPreviewPages
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetNumPreviewPages()
{
	LOG1("CBtPrintingDevice::GetNumPreviewPages ends with: %d", 1);
	return 1;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetJobTemplateIconL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::GetJobTemplateIconL(TInt /*aTemplateID*/, TInt& /*aFsBitmapHandle*/)
{
	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CreatePreviewImage
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::CreatePreviewImage(TInt /*aPageNumber*/)
{
	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CreateJobResponse
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::CreateJobResponse( TInt aJobId, TInt aErrorCode )
{
	LOG2("[CBtPrintingDevice::CreateJobResponse]\t iState: %d, Error: %d", iState, aErrorCode);

	ResetTimer();

	if (ECancelJob == iState)
	{
		LOG2("[CBtPrintingDevice::CreateJobResponse]\t wrong sequence: iState: %d, iPrintingJob->JobState(): %d", iState, iPrintingJob->JobState());
		return;
	}

	if(KErrNone != aErrorCode)
	{
		LOG1("[CBtPrintingDevice::CreateJobResponse]\t error occured: %d. Finish.", aErrorCode);
		ManagePrintingState(KErrNotFound, EBtStateAborted, EBtStateIdle, KErrNone, aErrorCode);
		return;
	}

    iPrintingJob->SetJobId(aJobId);

	iState = EJobProgress;

	LOG( "[CBtPrintingDevice]\t                                 ******");
	LOG1("[CBtPrintingDevice]\t -Printing started with job id:  * %2d", aJobId);
	LOG( "[CBtPrintingDevice]\t                                 ******");

	// Printing started - restart timer
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );

}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CancelJobResponse
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::CancelJobResponse(TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::CancelJobResponse]\t Error: %d", aErrorCode);
	ManagePrintingState(iPrintingJob->JobId(), EBtStateCancelled, EBtStateIdle, KErrNone, aErrorCode);

}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetPrinterStateResponse
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetPrinterStateResponse(TInt aState, TInt aStateReasons,
								 				TInt aOperationStatus, TInt aDeviceId)
{
	LOG2("[CBtPrintingDevice::GetPrinterStateResponse]\t iState: %d, Error: %d", iState, aOperationStatus);

	if(EGetPrinterState != iState)
		return;

	Deactivate();

	CRsBtDevice* printer = FindDevice( aDeviceId );
	if(!printer)
	{
		LOG("[CBtPrintingDevice::GetPrinterStateResponse]\t panic PANIC panic: printer not found");
		return;
	}

	LOG2("[CBtPrintingDevice::GetPrinterStateResponse] Printer state: %d - %d", aState, aOperationStatus);
	if(KErrNone != aOperationStatus)
	{
		// handle operation status in ManagePrintingState as a printer state
		aState = EBtStateStopped;
		aStateReasons = aOperationStatus;
	}

	if(EBtStateStopped == aState)
	{
		ManagePrintingState(KErrNotFound, EBtStateUnknown, aState, aStateReasons, aOperationStatus);
		return;
	}
	else if (EBtStateIdle == aState)
	{
		// maybe we cannot touch to iState: it maybe set to somethin on purpose...
		//iState = EReady;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetEventResponse
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetEventResponse(TInt aJobId, TInt aJobState, TInt aPrinterState,
					  					 TInt aStateReasons, TInt aOperationStatus)
{
	LOG("[CBtPrintingDevice::GetEventResponse]\t");

	ManagePrintingState(aJobId, aJobState, aPrinterState, aStateReasons, aOperationStatus);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetJobAttributesResponse
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetJobAttributesResponse( TInt  aJobState,
												  const TDesC8&  aJobName,
												  const TDesC8&  /*aOriginator*/,
												  TInt aSheetsCompleted,
												  TInt aPendingJobs,
												  TInt aOperationStatus )
{

	LOG1("[CBtPrintingDevice::GetJobAttributesResponse]\t iState: %d", iState);
	LOG2("[CBtPrintingDevice::GetJobAttributesResponse]\t aOperationStatus: %d / 0x%X", aOperationStatus, aOperationStatus);
	LOG2("[CBtPrintingDevice::GetJobAttributesResponse]\t aJobState: %d aSheetsCompleted: %d", aJobState, aSheetsCompleted);
	LOG82("[CBtPrintingDevice::GetJobAttributesResponse]\t aPendingJobs: %d aJobName: %S", aPendingJobs, &aJobName);

	if(EFinish == iState)
	{
		LOG("[CBtPrintingDevice::GetJobAttributesResponse]\t Finishing... return.");
		return;
	}

	if(KErrNone == aOperationStatus)
	{
		LOG("[CBtPrintingDevice::GetJobAttributesResponse]\t aOperationStatus OK");

		if(0 < aPendingJobs)
		{
			LOG1("[CBtPrintingDevice::GetJobAttributesResponse]\t pending: %d", aPendingJobs);
			aJobState = EBtStateWaiting;
		}
		else if(iPrintingJob->Sheets() == aSheetsCompleted)
		{
			LOG("[CBtPrintingDevice::GetJobAttributesResponse]\t Job Completed.");
			aJobState = EBtStateCompleted;
			
			if(CPrintJob::EStateCancel == iPrintingJob->JobState())
				iPrintingJob->UpdateJobState(CPrintJob::EStatePrinting);
		}
		else if(KPendingLimit == iPendingCount)
		{
			LOG1("[CBtPrintingDevice::GetJobAttributesResponse]\t timeout. pending too long: %d", iPendingCount);
			aOperationStatus = EPbCheckPrinter;
		}
		else if (aJobState == EBtStateStopped || aJobState == EBtStateAborted || aJobState == EBtStateUnknown || aJobState == EBtStateWaiting || aJobState == EBtStateCompleted)
		{
			Deactivate();

			LOG1("[CBtPrintingDevice::GetJobAttributesResponse]\t aJobState NOK: %d. Get printer attributes", aJobState);
			TRequestStatus* tmpStatus = &iStatus;
			iState = EGetPrinterState;
			User::RequestComplete(tmpStatus, KErrNone);

			SetActive();
			return;
		}
		else
		{
			iState = EGetJobAttributes;
			aJobState = EBtStatePrinting;
		}
	}

	ManagePrintingState(iPrintingJob->JobId(), aJobState, EBtStatePrinting, ENoMessage, aOperationStatus);

	return;
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::FindDevice
//
//--------------------------------------------------------------------------------------------
CRsBtDevice* CBtPrintingDevice::FindDevice(TInt aDeviceID)
{
	LOG1("[CBtPrintingDevice::FindDevice] begins with aDeviceID: %d", aDeviceID);
	TInt pos = iDeviceContainer->Find(aDeviceID);
	if(KErrNotFound != pos)
	{
		return iDeviceContainer->At(pos);
	}
	LOG("[CBtPrintingDevice::FindDevice] device NOT found");
	return NULL;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::SetNumsOfCopiesL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
	{
	if( !iPrintingJob || iPrintingJob->JobState() != CPrintJob::EStateCreated )
	{
		aErr = KErrInvalidSequence;
		return;
	}

    LOG1("[CBtPrintingDevice::SetNumsOfCopiesL]\t iPrintingJob->JobState(): %d", iPrintingJob->JobState());

	LOG2("[CBtPrintingDevice::SetNumsOfCopiesL]\t ImageCount(): %d, copies count: %d ",
				iPrintingJob->ImageCount(), aNumsOfCopies.Count());
	if( iPrintingJob->ImageCount() != aNumsOfCopies.Count() )
	{
		aErr = KErrInvalidData;
		return;
	}

	aErr = iPrintingJob->SetNumsOfCopies(aNumsOfCopies);
	LOG1("CBtPrintingDevice::SetNumsOfCopiesL aErr: %d", aErr);
	}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::InitJobL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::InitJobL(CRsBtDevice *aPrinter, RPointerArray<TDesC>& aImages)
{
	LOG("[CBtPrintingDevice::InitJobL]");

	if (iPrintingJob)
	{
		delete iPrintingJob;
		iPrintingJob = NULL;
	}

	// Change format of file list
	RArray<TFileName> imageList;
	CleanupClosePushL(imageList);
	for (TInt i=0; i < aImages.Count(); i++)
	{
		LOG1("[InitJobL]\t %S", aImages[i]);
		imageList.Append( *(aImages[i]) );
	}

	// Get local BT name
	TBuf8<0x100> name;
	GetLocalBTNameL(name);

	// Create job object
	iPrintingJob = CPrintJob::NewL(aPrinter, imageList, name);
	CleanupStack::PopAndDestroy(&imageList);

	RArray<TInt> capabIDs;
	CleanupClosePushL(capabIDs);
	aPrinter ->GetCapabilityIDs(capabIDs);

	for(TInt i = 0; i < capabIDs.Count(); ++i)
	{
		TPrintCapability capab;
		aPrinter->GetCapability(capabIDs[i], capab);
		iPrintingJob->SetPrintSetting(capabIDs[i], capab.iDefaultValue);
	}
	CleanupStack::PopAndDestroy(&capabIDs);

}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetCapabilities
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetCapabilities(CRsBtDevice& aPrinter, TBool aForcedGet)
	{
	// gets printer capabilities. If cannot get, sets defaults.
	LOG1("[CBtPrintingDevice::GetCapabilities] already found %d", aPrinter.CapabilityCount());

	// Capabilities already got
	if (0 < aPrinter.CapabilityCount() && !aForcedGet )
		return;

	TDeviceState tmpState = iState;
	iPendingCount = 0;
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );

	/*Get printer's capabilities*/
	TRAPD(leave, iPrinterController->GetCapabilitiesL(aPrinter) );
	LOG1("[CBtPrintingDevice::GetCapabilities] GetCapabilitiesL leaves with %d", leave);

	if ( ( leave == KErrNotFound && aPrinter.UsedProtocol() == KImagePrint_PrinterProtocol_BPP ) ||
		 ( leave == KErrArgument && aPrinter.UsedProtocol() == KImagePrint_PrinterProtocol_BPP ) )
		{		
		LOG( "Capabilities not received from BPP Printer - Thus Force OPP ON to avoid problems ");
		LOG( "This case indicates that there is possible problem in the BPP printer. ");
		aPrinter.SetUsedProtocol( KImagePrint_PrinterProtocol_OPP_Printer );
		aPrinter.SetSupportedProtocols( KImagePrint_PrinterProtocol_OPP_Printer );
		iUsedBtProfile = KImagePrint_PrinterProtocol_OPP_Printer;
		leave = KErrNone;		
		}
	ResetTimer();
	iPendingCount = 0;
	iState = tmpState;

	if(KErrNone != leave)
		{
		// cleanup connections
		Cancel();
		iPrinterController->Stop();

		RArray<TPrintCapability> c;
		GetDefaultCapabilities(c);
		for (TInt i = 0; i < c.Count(); ++i)
			{
			aPrinter.AddCapability(c[i]);
			}
		c.Close();
		}
	else
		{
		// store connected printer
		aPrinter.SetUsed(ETrue);
		// If this fails, no can do. Continue.
		TRAP_IGNORE( iDeviceContainer->StoreL() );
		}
	}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetDefaultCapabilities
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetDefaultCapabilities(RArray<TPrintCapability>& aCapabilities)
{
	// Layout
	TPrintCapability layout;
	layout.iCapabilityID = EPrintCapabLayout;
	layout.iType = TPrintCapability::Enum;
	layout.iDefaultValue = EPrintCapabLayout1Up;
	layout.iLow = 0;
	layout.iHigh = 0;
	layout.iEnumCodes[0] = layout.iDefaultValue;
	layout.iEnumCount = 1;

	// Quality
	TPrintCapability quality;
	quality.iCapabilityID = EPrintCapabQuality;
	quality.iType = TPrintCapability::Enum;
	quality.iDefaultValue = EPrintCapabQualityDefault;
	quality.iLow = 0;
	quality.iHigh = 0;
	quality.iEnumCodes[0] = quality.iDefaultValue;
	quality.iEnumCount = 1;

	// Paper size
	TPrintCapability paper;
	paper.iCapabilityID = EPrintCapabPaperSize;
	paper.iType = TPrintCapability::Enum;
	paper.iDefaultValue = EPrintCapabPaperSizeAuto;
	paper.iLow = 0;
	paper.iHigh = 0;
	paper.iEnumCodes[0] = paper.iDefaultValue;
	paper.iEnumCount = 1;

	aCapabilities.Append(layout);
	aCapabilities.Append(quality);
	aCapabilities.Append(paper);

}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::FoundDevice
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::FoundDeviceL(CRsBtDevice& aDevice)
{
	LOG2("[CBtPrintingDevice::FoundDeviceL]\t protocol: %d, DeviceId: %d", aDevice.UsedProtocol(), aDevice.DeviceId());

	TBool inform = EFalse;

	if( aDevice.UsedProtocol() == KImagePrint_PrinterProtocol_BPP )
		{
		inform = ETrue;
		}
	else if( aDevice.UsedProtocol() == KImagePrint_PrinterProtocol_OPP_Printer )
		{
		inform = ETrue;
		}
	else if( aDevice.UsedProtocol() == KImagePrint_PrinterProtocol_OPP_PC )
		{
		inform = ETrue;
		}
	else
		{
		// avoid lint error
		inform = EFalse;
		}


	if( iDiscoveryObserver && inform)
		{
		TPrinter printer = aDevice.ToTPrinter();
		iDiscoveryObserver->FoundDeviceL( printer );
		LOG("[CBtPrintingDevice::FoundDeviceL] observer informed");
		}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::AddBtDeviceL
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::AddBtDeviceL(CRsBtDevice& aDevice)
{

	LOG("[CBtPrintingDevice::AddBtDeviceL]\t");

	CRsBtDevice* device = NULL;

	TInt pos = iDeviceContainer->Find(aDevice);
	LOG1("[CBtPrintingDevice::AddBtDeviceL]\t pos: %d", pos);
	if(KErrNotFound != pos)
	{
		LOG("[CBtPrintingDevice::AddBtDeviceL]\t device exists");
		device = iDeviceContainer->At(pos);
		// create TPrinter of old device for informing of update
		TPrinter oldPrinter = device->ToTPrinter();

		// update existing
		TBool modified = device->UpdateUsedL(aDevice);
		device->SetJustFound(ETrue);
		
		// Be sure the if updated device is updated in observer
		if( iDiscoveryObserver && modified)
			{
			iDiscoveryObserver->RemoveDeviceL( oldPrinter );
			LOG1("[CBtPrintingDevice::FoundDeviceL] observer informed: remove \"%S\"", &oldPrinter.iDisplayName);
			TPrinter printer = device->ToTPrinter();
			iDiscoveryObserver->FoundDeviceL( printer );
			LOG1("[CBtPrintingDevice::FoundDeviceL] observer informed: found \"%S\"", &printer.iDisplayName);
			}

		return KErrNone;
	}

	device = CRsBtDevice::NewL(aDevice);
	LOG1("[CBtPrintingDevice::AddBtDeviceL]\t newDevice address: %d", (TInt)device);

	device->SetUsed(EFalse);
	device->SetDisappeared(EFalse);
	device->ComposeUsedProtocol(iRequestedProtocols);

	LOG1("[CBtPrintingDevice::AddBtDeviceL]\t usedProtocol: %d", device->UsedProtocol());

	iDeviceContainer->AppendL(device);

	LOG("[CBtPrintingDevice::AddBtDeviceL]\t preparing info for observer");

	FoundDeviceL( *device );
	return KErrNone;
}
//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetUsedDevicesL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetUsedDevicesL()
{
	LOG("[CBtPrintingDevice::GetUsedDevicesL]\t");
	iDeviceContainer->RestoreL(); 

	LOG1("[CBtPrintingDevice::GetUsedDevicesL]\t found %d devices.", iDeviceContainer->Count());
	for(TInt i = 0; i < iDeviceContainer->Count(); ++i)
		{
		iDeviceContainer->At(i)->ComposeUsedProtocol(iRequestedProtocols);
		FoundDeviceL( *(iDeviceContainer->At(i)) );
		}
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::FinishPrinting
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::FinishPrinting(TInt aErrorCode)
{
	LOG("[CBtPrintingDevice::FinishPrinting]\t ");

	iState = EFinish;

	LOG("[CBtPrintingDevice::FinishPrinting]\t -Reset timer");
	Deactivate();

	LOG("[CBtPrintingDevice::FinishPrinting]\t -Init ids");
	iPendingCount = KPendingLimit;

	// if error occures when geting capabilities, the job not created yet.
	if(iPrintingJob)
	{
		//Cancel job. if leaves, there is no chance to cancel the job -> continue
		//else return, the timeout or cancel job response will complete finish
		TRAPD(leave, CancelJobL());
		if(KErrNone != leave)
			LOG1("[CBtPrintingDevice::FinishPrinting]\t -CancelJobL() leaves with %d", leave);
		else
			return;

		iPrintingJob->UpdateJobState(CPrintJob::EStateCreated);
	}

	LOG1("[CBtPrintingDevice::FinishPrinting]\t FinishPrinting: state=%d", iState);

	// Inform controller
	iPrinterController->FinishPrinting();

	if(KErrNone != aErrorCode)
	{
		if (iPrintEventObserver)
			iPrintEventObserver->PrintJobErrorEvent(aErrorCode);
	}

	iState = EReady;
	LOG("[CBtPrintingDevice::FinishPrinting]\t -out");
}


//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::DeviceDiscovered
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::DeviceDiscovered( CRsBtDevice& aDevice )
{
	LOG("[CBtPrintingDevice::DeviceDiscovered]\t");
	TInt err; // for remove compiling errors
	TRAP(err, AddBtDeviceL(aDevice));
	LOG1("[CBtPrintingDevice::DeviceDiscovered]\t Device added with leave: %d", err);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::DeviceDisappeared
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::DeviceDisappeared( CRsBtDevice& aDevice )
{
	LOG("[CBtPrintingDevice::DeviceDisappeared]\t");

	CRsBtDevice* device = FindDevice(aDevice.DeviceId());
	if(!device)
	{
		LOG("[CBtPrintingDevice::DeviceDisappeared]\t Device not found. Return.");
		return;
	}

	TPrinter printer = device->ToTPrinter();
	LOG1("[CBtPrintingDevice::DeviceDisappeared]\t printer.iDisplayName: %S", &(printer.iDisplayName));
	device->SetDisappeared(ETrue);

	// Ignore leave; the call back
	TRAP_IGNORE(iDiscoveryObserver->RemoveDeviceL(printer));

	LOG("[CBtPrintingDevice::DeviceDisappeared]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::ResetTimer
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::ResetTimer()
{
	LOG("[CBtPrintingDevice::ResetTimer]\t");

	if(iPrinterTimer)
	{
		if(iPrinterTimer->IsActive())
			iPrinterTimer->Cancel();
	}
	iPendingCount = 0;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::StartTimer
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::StartTimer(TTimeIntervalMicroSeconds32 aDelay,
							TTimeIntervalMicroSeconds32 anInterval,
							TCallBack aCallBack)
{
	LOG("[CBtPrintingDevice::StartTimer]\t");

	ResetTimer();

	iPrinterTimer->Start(aDelay, anInterval, aCallBack);

}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::ConnectionTimeout
//
//--------------------------------------------------------------------------------------------
TBool CBtPrintingDevice::HandleTimeout(TAny *aWho)
{
	CBtPrintingDevice *cbTarget = static_cast<CBtPrintingDevice*>(aWho);
	return cbTarget->DoHandleTimeout();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::DoConnectionTimeout
//
//--------------------------------------------------------------------------------------------
TBool CBtPrintingDevice::DoHandleTimeout()
{
	LOG1("[CBtPrintingDevice::DoHandleTimeout]\t iState: %d", iState);

	TInt err = KErrTimedOut;
	if(iUsedBtProfile != KImagePrint_PrinterProtocol_OPP_Printer)
	{
		if(KPendingLimit > iPendingCount)
		{
			++iPendingCount;
			LOG1("[CBtPrintingDevice::DoHandleTimeout]\t pending: %d", iPendingCount);
	
			//Continue waiting...
			return ETrue;
		}

		// when timeout occures during printing, try anyway get printer state
		if(KPendingLimit == iPendingCount && EJobProgress == iState)
		{
			err = KErrNone;
			iState = EGetJobAttributes;
			--iPendingCount;
		}
	}
	else
	{
		
		if(KOppPendingLimit > iPendingCount)
		{
			++iPendingCount;
			LOG1("[CBtPrintingDevice::DoHandleTimeout]\t opp print pending: %d", iPendingCount);
	
			//Continue waiting...
			return ETrue;
		}

		// when timeout occures during printing, try anyway get printer state
		if(KOppPendingLimit == iPendingCount && EJobProgress == iState)
		{
			err = KErrNone;
			iState = EFinish;
			--iPendingCount;
		}
	}
	
	Cancel();

	TRequestStatus* tmpStatus = &iStatus;
	User::RequestComplete(tmpStatus, err);
	SetActive();

	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::ManagePrintingState
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::ManagePrintingState(TInt aJobId, TInt aJobState, TInt aPrinterState,
					  					 TInt aStateReasons, TInt aOperationStatus)
{
	LOG1("[CBtPrintingDevice::ManagePrintingState]\t begin: iState: %d", iState);
	LOG2("[CBtPrintingDevice::ManagePrintingState]\t aOperationStatus: %d / 0x%X", aOperationStatus, aOperationStatus);
	LOG2("[CBtPrintingDevice::ManagePrintingState]\t jobId: %d state: %d", aJobId, aJobState);
	LOG2("[CBtPrintingDevice::ManagePrintingState]\t aPrinterState: %d aStateReasons: %d", aPrinterState, aStateReasons);

	// already stopping, cancelling or idle... No reason to continue
	if( EFinish == iState || EReady == iState )
		return;

	Deactivate();
	TInt percents = 0;

	// Always inform anyway. No matter the state.
	// First the error cases:
 	TInt err = KErrNone;

 	// Set tmpState
 	TDeviceState tmpState = EFinish;

	if(KErrNone != aOperationStatus)
	{
		// if cancelling, ignore errors
		if(ECancelJob == iState && KErrDisconnected == aOperationStatus)
			return;

		err = aOperationStatus;
	}
	else if(EBtStateStopped == aPrinterState)
	{
		LOG("[CBtPrintingDevice::ManagePrintingState] Printer state stopped.");
		if(iPrintEventObserver)
			iPrintEventObserver->PrinterStatusEvent(aStateReasons, KErrNone);

		// Mustn't give error here:
		// inform via PrinterStatusEvent not at FinishPrinting via PrintJobErrorEvent
		err = KErrNone;
	}
	else
	{
		switch (aJobState)
		{
			case EBtStateUnknown:
			case EBtStateStopped:
			case EBtStateAborted:
				err = aStateReasons;
				break;
			case EBtStateWaiting:
				LOG("[CBtPrintingDevice::ManagePrintingState]\t Waiting. Cancel.");
				// Don't care if cancel fails: no can do anyway.
				CancelPrintJob();
				err = EPbStatusErrorReasonHardwarePrinterBusy;
				break;
			case EBtStateCompleted:
				LOG("[CBtPrintingDevice::ManagePrintingState]\t Completed.");
				LOG1("[CBtPrintingDevice::ManagePrintingState]\t iPrinting->JobState: %d", iPrintingJob->JobState());

				if(CPrintJob::EStatePrinting == iPrintingJob->JobState() || 
				   CPrintJob::EStateSending == iPrintingJob->JobState())
				{
					if(iPrintEventObserver)
						iPrintEventObserver->PrintJobProgressEvent(EDone, 100, ENoMessage);
				}
				
				else
				{
					err = EObexSendError;
				}
				
				iPrintingJob->UpdateJobState(CPrintJob::EStateCreated);
				break;
			case EBtStateCancelled:
				LOG("[CBtPrintingDevice::ManagePrintingState]\t Cancelled");
								
				// If Bt print is cancelled by Image Print, show error "Check status of Printer"
				err = EPbStatusErrorReasonNoReason;				
				break;
			// continue cases
			case EBtStatePrinting:
				iPrintingJob->UpdateJobState(CPrintJob::EStatePrinting);
				percents = UpdateProggress();
				LOG1("[CBtPrintingDevice::ManagePrintingState]\t Printing proggress %d", percents);

				if(iPrintEventObserver)
					iPrintEventObserver->PrintJobProgressEvent(EActive, percents, EPrinting);

				tmpState = EGetJobAttributes;
				StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
				break;
			case EBtStateSending:
				iPrintingJob->UpdateJobState(CPrintJob::EStateSending);
				percents = UpdateProggress();
				LOG1("[CBtPrintingDevice::ManagePrintingState]\t Sending proggress %d", percents);

				if(iPrintEventObserver)
					iPrintEventObserver->PrintJobProgressEvent(EActive, percents, ECopying);

				tmpState = EJobProgress;
				StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
			return;
			default:
				tmpState = iState;
				break;
		}
	}

	iPrintError = err;

	TRequestStatus* tmpStatus = &iStatus;
	iState = tmpState;
	User::RequestComplete(tmpStatus, err);

	SetActive();
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetJobAttributesL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetJobAttributesL()
{
	LOG1("[CBtPrintingDevice::GetJobAttributesL]\t begin: iState: %d", iState);

	CRsBtDevice* printer = static_cast<CRsBtDevice*>(User::LeaveIfNull(
					FindDevice(iPrintingJob->PrinterId())));

	iPrinterController->GetJobAttributesL(*printer, iPrintingJob->JobId());
	iState = EGetJobAttributes;

	LOG("[CBtPrintingDevice::GetJobAttributesL]\t out");
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetPrinterAttributesL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetPrinterStateL()
{
	LOG1("[CBtPrintingDevice::GetPrinterAttributesL]\t begin: iState: %d", iState);

	CRsBtDevice* printer = static_cast<CRsBtDevice*>(User::LeaveIfNull(
					FindDevice(iPrintingJob->PrinterId())));

	iPrinterController->GetPrinterStateL(*printer);
	iState = EGetPrinterState;

	LOG("[CBtPrintingDevice::GetPrinterAttributesL]\t out");
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CancelJobL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::CancelJobL()
{
	LOG("[CBtPrintingDevice::CancelJobL]\t begin");

	// If not submitted, return
	if( !iPrintingJob || 
	   (CPrintJob::EStateSubmitted != iPrintingJob->JobState() &&
	   	CPrintJob::EStateSending != iPrintingJob->JobState() &&
	   	CPrintJob::EStatePrinting != iPrintingJob->JobState()) )
		User::Leave(KErrInvalidSequence);

    LOG1("[CBtPrintingDevice::CancelJobL]\t begin at iPrintingJob->JobState(): %d", iPrintingJob->JobState());

	CRsBtDevice* printer = static_cast<CRsBtDevice*>(User::LeaveIfNull(
					FindDevice(iPrintingJob->PrinterId())));

	iPrintingJob->UpdateJobState(CPrintJob::EStateCancel);

	iPrinterController->CancelJobL(*printer, iPrintingJob->JobId());
	iState = ECancelJob;

	TRequestStatus* tmpStatus = &iStatus;
	User::RequestComplete(tmpStatus, KErrNone);
	SetActive();

	LOG("[CBtPrintingDevice::CancelJobL]\t out");
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(HandleTimeout, this) );
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::CheckProggress
//
//--------------------------------------------------------------------------------------------
TInt CBtPrintingDevice::UpdateProggress()
{
	TInt percents = iPrinterController->GetPercentSent();

	if(iProggress != percents && 0 != percents)
	{
		iProggress = percents;
		iPendingCount = 0;
	}

	// don't return 100% until the job is reported to be completed.
	// that way also the physical printing is included in percents.
	if(95 < iProggress)
		return 95;
	else
		return iProggress;
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::GetLocalBTNameL
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::GetLocalBTNameL(TDes8& aName)
{
	RsUtils::GetLocalBTNameL(aName);
	LOG81("[CBtPrintingDevice::GetLocalBTNameL]\t GetLocalBTNameL(%S)", &aName);
}

//--------------------------------------------------------------------------------------------
//
// CBtPrintingDevice::OPPSendProgress
//
//--------------------------------------------------------------------------------------------
void CBtPrintingDevice::OPPSendProgress( TInt aProgress )
	{
	if(iPrintEventObserver)
		{	
		// If sendind is finished ( Bytes sent == Job size) it's OK to remove progress bar.
		if (aProgress == 100)
			{
			iPrintEventObserver->PrintJobProgressEvent(EDone, 100, ENoMessage);			
			}
		else
			{
			// If sending is in progress update progress bar.
			iPrintEventObserver->PrintJobProgressEvent(EActive, aProgress, EPrinting);
			}
		}
	}

//  End of File
