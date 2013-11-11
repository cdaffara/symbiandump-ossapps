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
* Description:  Declares CUPnPPrintingDevice class
*
*/


#include <upnpsettings.h>
#include <fbs.h>
#include <upnpitem.h>
#include <upnphttpmessage.h>
#include <upnphttpmessagefactory.h>
#include <upnpicons.mbg>

#include "cupnpprintingdevice.h"
#include "cuplogger.h"
#include "upconsts.h"

_LIT( KIconsFileName, "imageprintdata\\protocols\\upnpicons.mbm" );

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::NewL
//
//--------------------------------------------------------------------------------------------
MProtPrintingDevice* CUPnPPrintingDevice::NewL()
	{
		LOG("[CUPnPPrintingDevice]\t ConstructL");
		return new (ELeave) CUPnPPrintingDevice();
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::ConstructL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::ConstructL(const TDesC& /*aDLLPath*/)
	{
		LOG("[CUPnPPrintingDevice]\t ConstructL");

		iPrinterContainer 	= CUPPrinterContainer::NewL();
		iPrinterTimer 		= CPeriodic::NewL(CActive::EPriorityStandard);
		iPrintFactory 	= CUPPrintFactory::NewL();

		LOG("[CUPnPPrintingDevice]\t Done.");
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::~CUPnPPrintingDevice
//
//--------------------------------------------------------------------------------------------
CUPnPPrintingDevice::~CUPnPPrintingDevice()
	{
	  if (iPrinterTimer){
      iPrinterTimer->Cancel();
      delete iPrinterTimer;
    }

		if (iPrinterControl)
			delete iPrinterControl;

		if (iFbsBitmap)
			delete iFbsBitmap;

		if (iPrintingJob)
			delete iPrintingJob;
    
    if (iPrinterContainer)
		  delete iPrinterContainer;
  
    if (iPrintFactory)
      delete iPrintFactory;

    REComSession::DestroyedImplementation( iDtor_ID_Key );
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DiscoveryTimeoutCBL
//
//--------------------------------------------------------------------------------------------
TBool CUPnPPrintingDevice::DiscoveryTimeoutCBL(TAny *aWho)
	{
		CUPnPPrintingDevice *cbTarget = static_cast<CUPnPPrintingDevice*>(aWho);
		return cbTarget->DoDiscoveryTimeoutL(ETrue);
	}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::StatusTimeoutCBL
//
//--------------------------------------------------------------------------------------------
TBool CUPnPPrintingDevice::StatusTimeoutCBL(TAny *aWho)
	{
		CUPnPPrintingDevice *cbTarget = static_cast<CUPnPPrintingDevice*>(aWho);
		TInt res = KErrNone;
		TInt err = KErrNone;
		TRAP(err, res = cbTarget->DoStatusTimeoutL());
		if(KErrNone != err)
		{
			cbTarget->FinishPrinting(KErrTimedOut, err);
			//Don't run the callback again
			res = EFalse;
		}
		return res;
	}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SubmitTimeoutCBL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::SubmitTimeoutCBL(TAny *aWho)
	{
		CUPnPPrintingDevice *cbTarget = static_cast<CUPnPPrintingDevice*>(aWho);
		cbTarget->DoSubmitTimeoutL();
		return EFalse; // Don't run the callback again
	}




// ===============================
// From MProtPrintingDevice.
// ===============================
// General
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::Version
//
//--------------------------------------------------------------------------------------------
TVersion CUPnPPrintingDevice::Version()
	{
		LOG("[CUPnPPrintingDevice]\t Version");
		return TVersion(KVersionMajor, KVersionMinor, KVersionBuild);
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SupportedProtocols
//
//--------------------------------------------------------------------------------------------
TUint CUPnPPrintingDevice::SupportedProtocols()
	{
	return KImagePrint_PrinterProtocol_UPnP;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::RegisterIdleObserver
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::RegisterIdleObserver(MProtIdleObserver* /*aObserver*/)
{

}


// Discovery.
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::StartDiscoveryL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocol)
{
	LOG("[CUPnPPrintingDevice]\t StartDiscoveryL");
	iDiscoveryObserver = &aObserver;
	iPrintersFound = EFalse;


	TRAPD(err, StartDiscoveryInternalL(aProtocol))
	if(KErrNone != err)
	{
		iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, err, 0);
	}
		
	return;
}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::StartDiscoveryL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::StartDiscoveryInternalL(TUint aProtocol)
	{
		LOG("[CUPnPPrintingDevice]\t StartDiscoveryInternalL");

		if (!(aProtocol &  KImagePrint_PrinterProtocol_UPnP))
		{
			iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNotSupported, 0);
			return;
		}

		switch ( iState )
		{
			case EUPnPDiscovery: // Already discovering
				break; 
			case EUPnPUninitialized:
			{
				LOG("[CUPnPPrintingDevice]\t starting discovery");				
				// (Re)start printer control point
				RestartControlPointL();
				// No devices available - return printers from cache and start timer
				SendPrinterListL(EFalse);				
			}
				break;
			case EUPnPJobReady:
			case EUPnPReady:
			{
				LOG("[CUPnPPrintingDevice]\t returning latest printer list");
				SendPrinterListL();
			}
				break;
			case EUPnPCancellingDiscovery:
			case EUPnPCancelling:
			case EUPnPPrintingStarted:
			case EUPnPPrinting:
			default:
			{
				LOG("[CUPnPPrintingDevice]\t cannot start discovery");
				iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrInvalidSequence, 0);
				break;
			}

		}
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::RemoveCachedDeviceL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::RemoveCachedDeviceL(TInt aDeviceID)
	{
		LOG("[CUPnPPrintingDevice::RemoveCachedDeviceL]");
		return iPrinterContainer->RemoveCachedDeviceL(aDeviceID);
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CancelDiscovery
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::CancelDiscovery(TBool aDoCallBack)
	{
		LOG("[CUPnPPrintingDevice]\t CancelDiscovery");
		if (iState == EUPnPDiscovery)
		{
			iState = EUPnPCancellingDiscovery;
			if (aDoCallBack && iDiscoveryObserver)
			{
				//We're not interested if the call back leaves
				TRAP_IGNORE(iDiscoveryObserver->DiscoveryStatusL(EDiscoveryCancelling, KErrNone, 0));
			}
		
			// This needs to be trapped since this function is not named as leaving - no need to chekc the error value
			TRAP_IGNORE( DoDiscoveryTimeoutL( EFalse ) );
		}
	}

// Print
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CreatePrintJobL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::CreatePrintJobL(TInt aDeviceId, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver)
	{
		LOG("[CUPnPPrintingDevice]\t CreatePrintJobL");
		iPrintEventObserver = &aObserver;

		switch ( iState )
		{
			case EUPnPReady:
			case EUPnPJobReady:
			{
				LOG("[CUPnPPrintingDevice]\t initializing job container");
				InitJobL(aDeviceId, aImages);
				return KErrNone;
			}
			case EUPnPCancelling:
			case EUPnPPrintingStarted:
			case EUPnPPrinting:
			case EUPnPCancellingDiscovery:
			case EUPnPUninitialized:
			default:
			{
				LOG("[CUPnPPrintingDevice]\t cannot create job");
				break;
			}

		}
		return KErrInvalidSequence;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SubmitPrintJobL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::SubmitPrintJobL()
	{

		LOG("[CUPnPPrintingDevice]\t SubmitPrintJobL");
		TInt err = KErrNone;

		switch ( iState )
		{
			case EUPnPPrintingStarted:
			case EUPnPPrinting:
				break; 	// Already printing
			case EUPnPJobReady:
			{
				LOG("[CUPnPPrintingDevice]\t submitting jobs");

				// Share jobs
				TRAP(err, iPrintingJob->ShareImagesL());
				if (err != KErrNone)
				{
					LOG1("[CUPnPPrintingDevice]\t -Sharing failed: %d", err);
					break;
				}

				LOG1("[CUPnPPrintingDevice::SubmitPrintJobL]\t start printing with state=%d", iState);
				iState = EUPnPSubmittingJob;
				TRAP(err, StartPrintingL());
				if (err != KErrNone)
					iState = EUPnPJobReady;

				LOG1("[CUPnPPrintingDevice::SubmitPrintJobL]\t StartPrintingL leaves with %d", err);
				LOG1("[CUPnPPrintingDevice::SubmitPrintJobL]\t state=%d", iState);
			}
				break;
			case EUPnPDiscovery:
			case EUPnPCancelling:
			case EUPnPCancellingDiscovery:
			case EUPnPUninitialized:
			case EUPnPReady:
			default:
			{
				LOG("[CUPnPPrintingDevice]\t Invalid sequence to submit job");
				err = KErrInvalidSequence;
			}
				break;
		}

		if(KErrNone != err)
		{
			if (iPrintEventObserver)
				iPrintEventObserver->PrintJobErrorEvent(EObexSendError, err);
			else
				User::Leave(err);
		}
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CancelPrintJob
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::CancelPrintJob()
	{
		LOG("[CUPnPPrintingDevice]\t CancelPrintJob");
		return DoCancelPrinting();
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::ContinuePrintJobL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::ContinuePrintJobL()
	{
		return KErrInvalidSequence;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetNumPrintPages
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetNumPrintPages()
{
	if( iPrintingJob )
		return iPrintingJob->Sheets();
	else
		return KErrInvalidSequence;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetPrintJobStatus
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetPrintJobStatus()
	{
		if (iState == EUPnPPrinting)
			return KErrInvalidSequence;
		return KErrNone;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetPrinterStatus
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetPrinterStatus(TInt /*aDeviceId*/)
	{
		return KErrNone;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetDeviceCapabilityIDsL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetDeviceCapabilityIDsL(TInt /*aDeviceID*/, RArray<TInt>& aCapabilityIDs)
	{
		LOG("[CUPnPPrintingDevice]\t GetDeviceCapabilityIDsL");

		if (iState == EUPnPUninitialized)
		{
			LOG("[CUPnPPrintingDevice]\t No devices discovered");
			return KErrInvalidSequence;
		}

		// Return supported settings from XHTML file factory
		iPrintFactory->GetCapabilityIDs(aCapabilityIDs);
		return KErrNone;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetDeviceCapabilityL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetDeviceCapabilityL(TInt /*aDeviceID*/, TInt aCapabilityID, TPrintCapability& aCapability)
	{
		LOG("[CUPnPPrintingDevice]\t GetDeviceCapabilityL");

		if (iState == EUPnPUninitialized)
		{
			LOG("[CUPnPPrintingDevice]\t No devices discovered");
			return KErrInvalidSequence;
		}

		// Return value from XHTML-print file factory
		return iPrintFactory->GetCapability(aCapabilityID, aCapability);
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetJobSetting
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
		LOG("[CUPnPPrintingDevice]\t GetJobSetting");
		if (iState != EUPnPJobReady)
		{
			LOG("[CUPnPPrintingDevice]\t Invalid state");
			return KErrInvalidSequence;
		}

		return iPrintFactory->GetPrintSetting(aCapabilityID, aValue);
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SetJobSettingL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
		LOG("[CUPnPPrintingDevice]\t SetJobSettingL");
		if (iState != EUPnPJobReady)
		{
			LOG("[CUPnPPrintingDevice]\t Invalid state");
			return KErrInvalidSequence;
		}

		// Set setting in job container
		aAffectedCapability = aCapabilityID;
		return iPrintFactory->SetPrintSetting(aCapabilityID, aValue);
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetNumPreviewPages
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetNumPreviewPages()
	{
		return KErrNotSupported;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetJobTemplateIconL
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::GetJobTemplateIconL(TInt /*aTemplateID*/, TInt& aFsBitmapHandle)
	{
		TInt res = KErrNone;

		if(iFbsBitmap)
			aFsBitmapHandle = iFbsBitmap->Handle();
		else
		{
			iFbsBitmap = new (ELeave) CFbsBitmap();
			TFileName iconsFileName;
			iconsFileName.Append( KDC_RESOURCE_FILES_DIR );
			iconsFileName.Append( KIconsFileName );
			res = iFbsBitmap->Load( iconsFileName ,EMbmUpnpiconsIcon );

			if( res == KErrNone )
				aFsBitmapHandle = iFbsBitmap->Handle();
			else
			{
				aFsBitmapHandle = 0;
				delete iFbsBitmap;
				iFbsBitmap = NULL;
			}
		}

		return res;
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CreatePreviewImage
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::CreatePreviewImage(TInt /*aPageNumber*/)
	{
		return KErrNotSupported;
	}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SetNumsOfCopiesL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
{
	if(iState != EUPnPJobReady)
		aErr = KErrInvalidSequence;
	else
		aErr = iPrintingJob->SetNumsOfCopies(aNumsOfCopies);
	
	return;
}

// ==========================================================================================
// From MPCPObserver
// ==========================================================================================
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CreateJobResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::CreateJobResponse( CUpnpAction* aAction, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::CreateJobResponse]\t Error: %d", aErrorCode);
	
	if(!aAction)
		return;

	if (iState == EUPnPCancelling || iPrintingJob->SessionId() != aAction->SessionId())
		return;
	
	if(KErrNone != aErrorCode)
	{
		FinishPrinting(EObexConnectError, aErrorCode);
		return;
	}

	ResetTimer();
    iPrintingJob->SetJobId(aAction->ArgumentValue(KJobId()));

	iState = EUPnPPrintingStarted;

	TBuf8<KMaxIdLength> id;
	iPrintingJob->GetJobId(id);
	
	LOG("[CUPnPPrintingDevice]\t                                 ******");
	LOG81("[CUPnPPrintingDevice]\t -Printing started with job id:  * %S", &id);
	LOG("[CUPnPPrintingDevice]\t                                 ******");

	// Printing started - restart timer
	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(StatusTimeoutCBL, this) );
}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CancelJobResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::CancelJobResponse( CUpnpAction* /*aAction*/, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::CancelJobResponse]\t Error: %d", aErrorCode);

	FinishPrinting(aErrorCode, KErrNone);
	if (iPrintEventObserver && KErrNone == aErrorCode)
		iPrintEventObserver->PrintJobProgressEvent(EDone, iPrintingJob->Progress(), ECancelling);
}
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetPrinterAttributesResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::GetPrinterAttributesResponse( CUpnpAction* aAction, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::GetPrinterAttributesResponse]\t Error: %d", aErrorCode);

	if(!aAction)
		return;

	if (KErrNone != aErrorCode)
	{
		FinishPrinting(KErrGeneral, aErrorCode);
		return;
	}

	// Cancel and restart timer
	// Read printer attributes from action and update job status
	TUPnPState state = EStateUnknown;
	TRAPD(err, state = iPrintingJob->UpdateJobState(aAction->ArgumentValue( KPrinterState() ),
													 aAction->ArgumentValue( KPrinterStateReasons() ),
													 aAction->ArgumentValue( KJobIdList() ),
													 aAction->ArgumentValue( KJobId() ),
													 aAction->ArgumentValue( KSheetsCompleted() ) ) );	
	if(KErrNone != err)
	{
		FinishPrinting(KErrGeneral, err);
		return;
	}
	
	UpdatePrintingStatus(state);
}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetJobAttributesResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::GetJobAttributesResponse( CUpnpAction* aAction, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::GetJobAttributesResponse]\t Error: %d", aErrorCode);

	if (KErrNone != aErrorCode)
	{
		LOG1("[CUPnPPrintingDevice:.GetJobAttributesResponse]\t - GetPrinterAttributesL with error: %d", aErrorCode);

		TInt err = KErrNone;
		CUpnpDevice* printer = GetPrinter();
		if(!printer)
			err = KErrNoMemory;
		else
			TRAP(err, iPrinterControl->GetPrinterAttributesL(printer));

		if(KErrNone != err)
			FinishPrinting(KErrGeneral, err);
		
		return;
	}

	if(!aAction)
		return;
	
	LOG81("[CUPnPPrintingDevice::GetJobAttributesResponse]\t sheets: ", &(aAction->ArgumentValue( KSheetsCompleted() )));
	LOG81("[CUPnPPrintingDevice::GetJobAttributesResponse]\t name: ", &(aAction->ArgumentValue( KJobName() )));
	LOG81("[CUPnPPrintingDevice::GetJobAttributesResponse]\t origin: ", &(aAction->ArgumentValue( KUserName() )));
	LOG81("[CUPnPPrintingDevice::GetJobAttributesResponse]\t id: ", &(aAction->ArgumentValue( KJobId() )));

	TBuf8<KMaxIdLength> id;
	iPrintingJob->GetJobId(id);

	TUPnPState state = EStateUnknown;
	state = iPrintingJob->JobMediaSheetsCompletedUpdated(id, aAction->ArgumentValue( KSheetsCompleted() ) );
	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetMarginsResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::GetMarginsResponse( CUpnpAction* /*aAction*/, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::GetMarginsResponse]\t Error: %d", aErrorCode);
	if (KErrNone != aErrorCode)
	{
		FinishPrinting(KErrGeneral, aErrorCode);
		return;
	}

}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetMediaListResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::GetMediaListResponse( CUpnpAction* /*aAction*/, TInt aErrorCode )
{
	LOG1("[CUPnPPrintingDevice::GetMediaListResponse]\t  Error: %d", aErrorCode);
	if (KErrNone != aErrorCode)
	{
		FinishPrinting(KErrGeneral, aErrorCode);
		return;
	}

}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::StateUpdatedResponse
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::StateUpdatedResponse( const TDesC8& /*aPrinterState*/, 
												const TDesC8& /*aPrinterStateReasons*/,
                                           		const TDesC8& /*aJobIdList*/, 
                                           		const TDesC8& /*aJobEndState*/,
                                           		const TDesC8& /*aJobMediaSheetsCompleted*/, 
                                           		const TDesC8& /*aJobAbortState*/,
                                           		const TDesC8& /*aContentCompleteList*/ )
{
	LOG("[CUPnPPrintingDevice::StateUpdatedResponse]\t");
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::PrinterStateUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::PrinterStateUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::PrinterStateUpdated]\t:%S",&aEvent);
	iStatusTimeout = EFalse;
	TUPnPState state = iPrintingJob->PrinterStateUpdated(aEvent);	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::PrinterStateReasonsUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::PrinterStateReasonsUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::PrinterStateReasonsUpdated]\t: %S",&aEvent);
	iStatusTimeout = EFalse;
	TUPnPState state = iPrintingJob->PrinterStateReasonsUpdated(aEvent);	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::JobIdListUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::JobIdListUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::JobIdListUpdated]\t:%S",&aEvent);
	iStatusTimeout = EFalse;
	TUPnPState state = iPrintingJob->JobIdListUpdated(aEvent);	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::JobEndStateUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::JobEndStateUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::JobEndStateUpdated]\t:%S",&aEvent);

	TUPnPState state = EStateUnknown;
	TRAPD(err, state = iPrintingJob->JobEndStateUpdatedL(aEvent));	
	if(KErrNone != err)
	{
		LOG1("[CUPnPPrintingDevice::JobEndStateUpdated]\t leaving with error %d. Continue with timer.",err);
		return;
	}
	
	iStatusTimeout = EFalse;
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::JobMediaSheetsCompletedUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::JobMediaSheetsCompletedUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::JobMediaSheetsCompletedUpdated]\t:%S",&aEvent);
	iStatusTimeout = EFalse;
	TUPnPState state = iPrintingJob->JobMediaSheetsCompletedUpdated(aEvent);	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::ContentCompleteListUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::ContentCompleteListUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::ContentCompleteListUpdated]\t:%S",&aEvent);
	iStatusTimeout = EFalse;
	TUPnPState state = iPrintingJob->ContentCompleteListUpdated(aEvent);	
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::JobAbortStateUpdated
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::JobAbortStateUpdated( const TDesC8& aEvent )
{
	// no need to check the states: we're not printing
	if(EUPnPPrintingStarted != iState && EUPnPPrinting != iState)
		return;
	
	LOG81("[CUPnPPrintingDevice::JobAbortStateUpdated]\t:%S",&aEvent);

	TUPnPState state = EStateUnknown;
	TRAPD(err, state = iPrintingJob->JobAbortStateUpdatedL(aEvent));	
	if(KErrNone != err)
	{
		LOG1("[CUPnPPrintingDevice::JobAbortStateUpdatedL]\t leaving with error %d. Continue with timer.",err);
		return;
	}
	
	iStatusTimeout = EFalse;
	UpdatePrintingStatus(state);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DeviceDiscoveredL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::DeviceDiscoveredL( CUpnpDevice* aDevice )
{

	LOG("[CUPnPPrintingDevice::DeviceDiscovered]\t ");
	TInt id;
	TInt err = KErrNone;

	if(!aDevice)
		return;
	
	//Trap this: the iDiscoveryObserver will othervise never know about error
	TRAP(err, iPrinterContainer->AddPrinterL(*aDevice, id));

	// Inform about error and leave
	if(KErrNone != err && KErrAlreadyExists != err)
	{
		if(iDiscoveryObserver && iState == EUPnPDiscovery)
		{
			iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, err, 0);
		}
		User::Leave(err);
	}
	
	iPrintersFound = ETrue;
	if (iState == EUPnPUninitialized)
	{
		iState = EUPnPReady;
	}

	if(iState == EUPnPDiscovery && iDiscoveryObserver && KErrAlreadyExists != err)
	{

		TInt index = iPrinterContainer->PrinterIndex(id);
		TPrinter printer = iPrinterContainer->ToTPrinter(index);
		iDiscoveryObserver->FoundDeviceL(printer);
	}
	
	if (iState == EUPnPSubmittingJob)
	{
		// Check if the discovered printer is the one we're trying to print with
		if (iPrintingJob->PrinterId() == id)
		{
			// Stop submit timer and submit job
			ResetTimer();
			TRAP(err, SubmitJobToPcpL(*aDevice, id) );
			
			// If error occured inform client - leaving not necessary here
			if (err != KErrNone)
			{
				if (iPrintEventObserver)
				{
					iPrintEventObserver->PrintJobErrorEvent(EObexConnectError, err);
				}
				iState = EUPnPJobReady;				
			}
		}
	}
	LOG("[CUPnPPrintingDevice::DeviceDiscovered]\t -out");
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DeviceDisappeared
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::DeviceDisappeared( CUpnpDevice* aDevice )
{
	LOG("[CUPnPPrintingDevice]\t DeviceDisappeared");
	if ( aDevice )
		{
		// Notify protocol client if we're in discovery state
		if(iState == EUPnPDiscovery && iDiscoveryObserver)
		{
			TInt index = iPrinterContainer->PrinterIndex( *aDevice );
			if (index >= 0)
			{
				LOG("[CUPnPPrintingDevice]\t informing observer");
				TPrinter printer = iPrinterContainer->ToTPrinter(index);
				
				// Notification needs to be trapped because this is not a leaving function
				TRAP_IGNORE(iDiscoveryObserver->RemoveDeviceL(printer));
	
				iPrinterContainer->RemovePrinter(index);			
			}
		}
		else
		{
			LOG("[CUPnPPrintingDevice]\t saving to disappeared list");
			// Trapped because this is not a leaving function
			TRAP_IGNORE(iPrinterContainer->PrinterDisappeared( *aDevice ));
		}
		}
	if ( iPrinterContainer->PrinterCount() < 1 )
		{
		iPrintersFound = EFalse;
		}
}


/////////////////////////
// Protected.
/////////////////////////


/////////////////////////
// Private.
/////////////////////////
//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::CUPnPPrintingDevice
//
//--------------------------------------------------------------------------------------------
CUPnPPrintingDevice::CUPnPPrintingDevice()
	{
		iPrinterTimer = NULL;
		iState = EUPnPUninitialized;
		iPrintersFound = EFalse;
		iTriedReadCacheAndFail = EFalse;
		ResetTimer();
	}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::RestartControlPointL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::RestartControlPointL()
	{
		if (iPrinterControl)
		{
			delete iPrinterControl;
			iPrinterControl = NULL;
		}
 
		iFriendlyName.Copy(KUPnPUserName());
		
	CUpnpSettings *rep = CUpnpSettings::NewL( KCRUidUPnPStack );
    CleanupStack::PushL(rep);
		//Get Access Point from central repository
    TInt iapId = -1;
    User::LeaveIfError( rep->Get( CUpnpSettings::KUPnPStackIapId, iapId ));
    
    //Check accessPoint and leave if not ok
    if( iapId < 0 )
    {
      if(iDiscoveryObserver)
        iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrAbort, 0);
      User::Leave( KErrAbort );
    }

		CleanupStack::PopAndDestroy(rep);
		

    // Create printer control point - search starts right away
		LOG("[CUPnPPrintingDevice]\t - Printer control point");
		iPrinterControl = CPrinterControlPoint::NewL(*this);
	}


//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SendPrinterListL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::SendPrinterListL(TBool aSync)
{
LOG("[CUPnPPrintingDevice]\t SendPrinterListL Begin");
	TInt err = KErrNone;
	if (aSync)
	{
		// Remove disappeared devices from array
		iPrinterContainer->SyncPrinterArray();
	}

	if (iPrinterContainer->PrinterCount() < 1 && !iTriedReadCacheAndFail)
	{
		// Read printer cache
		LOG("[CUPnPPrintingDevice]\t SendPrinterListL xxx trying read cache file");
		TRAP(err, iPrinterContainer->ReadCacheL());
		if(KErrNone != err)
		{
			LOG1("[CUPnPPrintingDevice]\t SendPrinterListL xxx some error file reading %d", err);
			if(err == KErrNotFound)
			{
				iPrintersFound = EFalse;
				iTriedReadCacheAndFail = ETrue;
			} 
			else 
				User::Leave(err);
		} 
		else
		{
			LOG("[CUPnPPrintingDevice]\t SendPrinterListL xxx cachefile readed");
			iPrintersFound = EFalse;
		}
		
		if(aSync)
		{
			RestartControlPointL();
		}
	}
	
	if (iPrinterContainer->PrinterCount() > 0 && iDiscoveryObserver)
	{
		for (TInt i=0; i< iPrinterContainer->PrinterCount(); i++)
		{
			TPrinter t = iPrinterContainer->ToTPrinter(i);
			iDiscoveryObserver->FoundDeviceL( t );
		}
	}

	if (!iPrintersFound)
	{
		// start timed search
		StartTimer(KPrinterDiscoveryTimeout, KPrinterDiscoveryTimeout, TCallBack(DiscoveryTimeoutCBL, this) );
		iState = EUPnPDiscovery;
		iTriedReadCacheAndFail = EFalse;
	}
	else if(iDiscoveryObserver)
	{
		iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNone, 0);	
	}
	else
    {
	    User::LeaveIfNull(iDiscoveryObserver);
    }
	LOG("[CUPnPPrintingDevice]\t SendPrinterListL end");
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DoDiscoveryTimeoutL
//
//--------------------------------------------------------------------------------------------
TBool CUPnPPrintingDevice::DoDiscoveryTimeoutL(TBool aDoCallBack)
{
	LOG("[CUPnPPrintingDevice]\t DoDiscoveryTimeOut");

	ResetTimer();
	switch( iState )
	{
		case EUPnPCancellingDiscovery:
			{
				LOG("[CUPnPPrintingDevice]\t cancelling discovery");
				
				if (iPrinterContainer->PrinterCount() > 0)
				{
					iPrintersFound = ETrue;
				}
				
				if (aDoCallBack && iDiscoveryObserver)
				{
					iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNone, 0);
				}
				
			}
			break;
		case EUPnPDiscovery:
			{
				LOG("[CUPnPPrintingDevice]\t discovery timeout");
				
				// Notify about cached devices, which weren't discovered
				RPointerArray<CUPPrinter> cachedPrinters;
				CleanupClosePushL(cachedPrinters);
				iPrinterContainer->GetDisappearedPrinters(cachedPrinters);
				for (TInt i=0; i < cachedPrinters.Count(); i++)
				{
					TPrinter printer = cachedPrinters[i]->ToTPrinter();
					iDiscoveryObserver->RemoveDeviceL(printer);		
				}
				
				CleanupStack::PopAndDestroy(); //cachedPrinters

				// Remove disappeared printers and cached printers, which weren't found printers from container
				iPrinterContainer->SyncPrinterArray();

        if (iPrinterContainer->PrinterCount() > 0)
				{
					iPrintersFound = ETrue;
				}

				// Notify client's observer
				if (aDoCallBack && iDiscoveryObserver)
					iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNone, 0);
				
			}
			break;
		default:
			break;
	}

	if (!iPrintersFound)
	{
		LOG("[CUPnPPrintingDevice]\t no printers found");
		iState = EUPnPUninitialized;
	}
	else
	{
		iState = EUPnPReady;
	}

	//Don't run this callback again
	return EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DoStatusTimeoutL
//
//--------------------------------------------------------------------------------------------
TBool CUPnPPrintingDevice::DoStatusTimeoutL()
{
	LOG("[CUPnPPrintingDevice]\t DoStatusTimeOut");

	if (iState == EUPnPCancelling || iState == EUPnPSubmittingJob)
	{
		// Printer did not response to cancel or create job
		LOG("[CUPnPPrintingDevice]\t - no response for cancel or create.");
		FinishPrinting(KErrGeneral, EPbCheckPrinter);
		return EFalse; // do not call again
	}

	if (iState == EUPnPPrinting || iState == EUPnPPrintingStarted)
	{
		--iPendingCount;

		// No printing status notifications received
		CUpnpDevice* printer = GetPrinter();
		if (!printer || iStatusTimeout || 0 > iPendingCount)
		{
			// print error - cancel printing
			LOG("[CUPnPPrintingDevice]\t - print error - cancel printing.");
			LOG1("[CUPnPPrintingDevice]\t - iStatusTimeout: %d", iStatusTimeout);
			LOG1("[CUPnPPrintingDevice]\t - iPendingCount: %d", iPendingCount);
			ResetTimer();
	  		if (iPrintEventObserver)
				iPrintEventObserver->PrintJobErrorEvent(KErrTimedOut, EPbCheckPrinter);
			DoCancelPrinting();
			return EFalse; // do not call again
		}

		iStatusTimeout = ETrue;
		
		LOG1("[CUPnPPrintingDevice]\t - GetPrinterAttributesL with iState: %d", iState);
		iPrinterControl->GetPrinterAttributesL(printer);

		/*LOG1("[CUPnPPrintingDevice]\t - GetJobAttributesL with iState: %d", iState);
		TBuf8<KMaxIdLength> id;
		iPrintingJob->GetJobId(id);
		iPrinterControl->GetJobAttributesL(printer, id);*/
	}

	return ETrue; // call again
}



//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::InitJobL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::InitJobL(TInt aDeviceId, RPointerArray<TDesC>& aImages)
{
	LOG("[CUPnPPrintingDevice::InitJobL]\t");

	// Check that device exists
	CUPPrinter* printer = iPrinterContainer->Printer(aDeviceId);
	if (!printer)
	{
		LOG("[CUPnPPrintingDevice]\t printer not found");
		User::Leave(KErrArgument);
	}

	// Create job
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
		LOG81("[InitJobL]\t ", aImages[i]);
		imageList.Append( *(aImages[i]) );
	}

	// Create job object
	iPrintingJob = CUPPrintingJob::NewL(printer, imageList, iPrintFactory, iFriendlyName);
	CleanupStack::PopAndDestroy(&imageList);

	// Update state and return
	iState = EUPnPJobReady;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::StartPrintingL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::StartPrintingL()
{
	LOG("[CUPnPPrintingDevice::StartPrintingL]\t");
	ResetTimer();

	// Get pointer to correct CDevice
	CUPPrinter* upPrinter = static_cast<CUPPrinter*>(User::LeaveIfNull(
					iPrinterContainer->Printer(iPrintingJob->PrinterId())));

	CUpnpDevice* printer = upPrinter->Device();

	if (!printer)
	{
		LOG("[CUPnPPrintingDevice]\t -UPnP Printer pointer was NULL");
		
		// Try to discover selected printer - start timer
		StartTimer(KPrinterSubmitTimeout, KPrinterSubmitTimeout, TCallBack(SubmitTimeoutCBL, this) );
		return;
	} 

	// Send create job to PCP
	SubmitJobToPcpL(*printer, iPrintingJob->PrinterId());
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DoSubmitTimeoutL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::DoSubmitTimeoutL()
{
	LOG("[CUPnPPrintingDevice]\t CUPnPPrintingDevice::DoSubmitTimeoutL");

	ResetTimer();

	TRAPD(err, SubmitJobToPcpL(iPrintingJob->PrinterId()));				
		
	if( err != KErrNone)
	{
		if (iPrintEventObserver)
			iPrintEventObserver->PrintJobErrorEvent(EObexConnectError, err);
		iState = EUPnPJobReady;
	}
	
}



//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SubmitJobToPcpL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::SubmitJobToPcpL(TInt aDeviceID)
{
	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL(TInt)]\t");

	// Get pointer to correct CDevice
	CUPPrinter* printer = NULL;

	TInt printerIndex = iPrinterContainer->PrinterIndex(aDeviceID);
	if (printerIndex >= 0)
	{
		printer =
			static_cast<CUPPrinter*> (User::LeaveIfNull(iPrinterContainer->Printer(printerIndex)));
        SubmitJobToPcpL(*(printer->Device()), aDeviceID);
	}
	else
	{
	    User::LeaveIfNull(printer);
    }
	
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::SubmitJobToPcpL
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::SubmitJobToPcpL(CUpnpDevice& aPrinter, TInt aPrinterID)
{
	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL(CDevice&, TInt)]\t");

	// Init settings
	TPtrC8 userName;
	iPrintingJob->GetUserName(userName);

	// Get and convert xhtml file
	TFileName8 fileName;

	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL]\t -before GetPrintFileL call");
	iPrintingJob->GetPrintFileL(fileName);
	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL]\t -after GetPrintFile call");
	
	User::LeaveIfNull(&aPrinter);
	
	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL]\t -before CreateJobL call");
	TInt sessionId = iPrinterControl->CreateJobL(&aPrinter, fileName, userName, KUPnPOne(), KUPnPDeviceSetting(),
					 KUPnPDeviceSetting(), KUPnPDeviceSetting(), KUPnPDeviceSetting(), KUPnPDeviceSetting(), KUPnPDeviceSetting());
	LOG("[CUPnPPrintingDevice::SubmitJobToPcpL]\t -after CreateJobL call");
	
	iPrintingJob->SetSessionId(sessionId);
	iPendingCount = KPendingLimit + iPrintingJob->ImageCount();

	LOG1("[CUPnPPrintingDevice]\t -Printing started successfully - timer can expire %d times", iPendingCount);

	//Add printer in cache if doesn't exist
	iPrinterContainer->UpdateCacheL(aPrinterID);

	StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(StatusTimeoutCBL, this) );
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::DoCancelPrintJob()
//
//--------------------------------------------------------------------------------------------
TInt CUPnPPrintingDevice::DoCancelPrinting()
{
	LOG("[CUPnPPrintingDevice]\t DoCancelPrinting()");

	if (iState == EUPnPPrintingStarted || iState == EUPnPPrinting || iState == EUPnPSubmittingJob)
	{
		// Cancel printer response timer first
		LOG("[CUPnPPrintingDevice]\t -Canceling timer");
		ResetTimer();

		// Cancel current job
		iState = EUPnPCancelling;
		CUpnpDevice* printer = GetPrinter();
		if (!printer)
		{
			LOG("[CUPnPPrintingDevice]\t Invalid printer");
			return KErrArgument;
		}

		TInt err;
		TBuf8<KMaxIdLength> id;
		iPrintingJob->GetJobId(id);
	
		if (id.Length() >= 1)
		{
			LOG81("[CUPnPPrintingDevice]\t Cancel with job id %S", &id);
			TRAP(err, iPrinterControl->CancelJobL(printer, id))
		}
		else
		{
			LOG81("[CUPnPPrintingDevice]\t Cancel with session id %S", iPrintingJob->SessionId());
			TRAP(err, iPrinterControl->CancelJobL(iPrintingJob->SessionId()) );
		}

		if(KErrNone != err)
		{
			LOG1("[CUPnPPrintingDevice]\t Cancel failed with error %d -> roughly finish printing ", err);
			FinishPrinting(KErrGeneral, err);
			return err;
		}

		// Start timer for cancel response
		StartTimer(KPrinterResponseTimeout, KPrinterResponseTimeout, TCallBack(StatusTimeoutCBL, this) );

		return KErrNone;
	}

	if (iState == EUPnPJobReady)
	{
		return KErrNone;
	}

	return KErrInvalidSequence;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::UpdatePrintingStatus()
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::UpdatePrintingStatus(TUPnPState aJobState)
{
	LOG1("[CUPnPPrintingDevice::UpdatePrintingStatus]\t - JobState: %d", aJobState);

	/* Printer stopped */
	if(EPrinterStateStopped == aJobState || EStatePending == aJobState)
	{
		iStatusTimeout = EFalse;
		// Finish printing if reason found. Otherwise wait timer or new message
		if(ENoMessage != iPrintingJob->JobState()->Reason())
		{
			LOG1("[CUPnPPrintingDevice]\t - state NOK -> Cancel with %d", iPrintingJob->JobStateReason());
	  		if (iPrintEventObserver)
	  			iPrintEventObserver->PrintJobErrorEvent(KErrGeneral, iPrintingJob->JobStateReason());
			DoCancelPrinting();
		}
		return;
	}

	// Job aborted
	if(EJobStateAborted == aJobState)
	{
		LOG("[CUPnPPrintingDevice]\t - EJobStateAborted -> Finish");
		FinishPrinting(KErrGeneral, iPrintingJob->JobStateReason());
		return;
	}

	if(EStateReady == aJobState)
	{
		// Printing finished for current job
		LOG("[CUPnPPrintingDevice]\t - Printing finished");
		iPrintEventObserver->PrintJobProgressEvent(EDone, iPrintingJob->Progress(), ENoMessage);

		// FinishPrinting
		FinishPrinting();
  		return;
	}

	iPrintEventObserver->PrintJobProgressEvent(EActive, iPrintingJob->Progress(), ENoMessage);
	iStatusTimeout = EFalse;
	LOG("[CUPnPPrintingDevice::UpdatePrintingStatus]\t - out");
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::FinishPrinting
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::FinishPrinting()
{
	LOG("[CUPnPPrintingDevice::FinishPrinting]\t ");

	LOG("[CUPnPPrintingDevice]\t -Reset timer");
	ResetTimer();

	LOG("[CUPnPPrintingDevice]\t -Unshare");
	iPrintingJob->UnShareImages();

	LOG("[CUPnPPrintingDevice]\t -Init ids");
	iPrintingJob->SetJobId(KNullDesC8());
	iPrintingJob->SetSessionId(KErrNotFound);
	iPendingCount = KPendingLimit;

	LOG("[CUPnPPrintingDevice]\t -Update state");
	if(NULL != GetPrinter())
		iState = EUPnPJobReady;
	else
		iState = EUPnPReady;
	LOG1("[CUPnPPrintingDevice]\t FinishPrinting: state=%d", iState);


	// Inform control point
	// No can do if this leaves -> ignore leave
	TRAP_IGNORE(iPrinterControl->PrintingFinishedL(GetPrinter()));
	LOG("[CUPnPPrintingDevice::FinishPrinting]\t -out");
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::FinishPrinting
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::FinishPrinting(TInt aErrorCode, TInt aErrorMessage)
{
	FinishPrinting();

	if (iPrintEventObserver && KErrNone != aErrorCode)
		iPrintEventObserver->PrintJobErrorEvent(aErrorCode, aErrorMessage);
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::GetPrinter
//
//--------------------------------------------------------------------------------------------
CUpnpDevice* CUPnPPrintingDevice::GetPrinter()
{
	LOG("[CUPnPPrintingDevice::GetPrinter]");

	if(iPrintingJob)
	{
		CUPPrinter* uaPrinter = iPrinterContainer->Printer(iPrintingJob->PrinterId());
		if (uaPrinter)
		{
			return uaPrinter->Device();
		}
	}
	return NULL;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::ResetTimer
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::ResetTimer()
{
	LOG("[CUPnPPrintingDevice::ResetTimer]\t");

	if(iPrinterTimer)
		iPrinterTimer->Cancel();
	iStatusTimeout = EFalse;
}

//--------------------------------------------------------------------------------------------
//
// CUPnPPrintingDevice::ResetTimer
//
//--------------------------------------------------------------------------------------------
void CUPnPPrintingDevice::StartTimer(TTimeIntervalMicroSeconds32 aDelay,
									 TTimeIntervalMicroSeconds32 anInterval,
									 TCallBack aCallBack)
{
	LOG("[CUPnPPrintingDevice::StartTimer]\t");

	ResetTimer();

	iPrinterTimer->Start(aDelay, anInterval, aCallBack );

}

//  End of File
