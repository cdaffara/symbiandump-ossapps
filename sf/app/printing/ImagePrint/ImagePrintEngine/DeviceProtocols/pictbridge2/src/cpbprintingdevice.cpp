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
* Description:  Contains the CPBPrintingDevice class definition.
*
*/


#include <e32std.h>
#include <pictbridge.h>
#include <s32file.h>
#include <f32file.h>

#include "cpbprintingdevice.h"
#include "printcapabilitycodes.h"
#include "printmessagecodes.h"
#include "imageprint.h"
#include "cpbconnect.h"
#include "clog.h"

//  CONSTANTS
namespace
	{
	const TInt KPBVersionMajor = 0;
	const TInt KPBVersionMinor = 0;
	const TInt KPBVersionBuild = 1;
	}

const TInt KConfigLength = 16;
const TInt KMaxRetry = 10;
_LIT( KConfigInfoFile, "config.DPS");


MProtPrintingDevice* CPBPrintingDevice::NewL()
	{
	return new (ELeave) CPBPrintingDevice();
	}


CPBPrintingDevice::CPBPrintingDevice() :
CActive( CActive::EPriorityStandard ), iState(EPBUninitialized), iDisconnectPrinter(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CPBPrintingDevice::ConstructL(const TDesC& /*aDLLPath*/)
	{
	LOG("CPBPrintingDevice::ConstructL begin");	
	
	iDpsEngine = CDpsEngine::GetEngineL();		
	iConnect = CPBConnect::NewL( *iDpsEngine, *this );
	
    iCongFileName = HBufC::NewL( iDpsEngine->DpsFolder().Length() + KConfigLength );
    TInt k = iDpsEngine->DpsFolder().Length() + KConfigLength;
    iCongFileName->Des().Copy( iDpsEngine->DpsFolder() );
    iCongFileName->Des().Append( KConfigInfoFile );
 	
	iCancelRetry = 0;

    // for Cert software to indicate that printing state and idle state 
    iPrintingStateComplete = 0;
	
	iUserSelectedPaperSize = EDpsPaperSizeDefault;
	iNewJobOK = ETrue;
	
	LOG("CPBPrintingDevice::ConstructL end");
	}


CPBPrintingDevice::~CPBPrintingDevice()
	{
	LOG("CPBPrintingDevice::~CPBPrintingDevice begin");
	Cancel();
	delete iConnect;
	delete iCongFileName;
	if ( iDpsEngine )
		{
		iDpsEngine->Delete();
		}
	REComSession::DestroyedImplementation( iDtor_ID_Key );
	LOG("CPBPrintingDevice::~CPBPrintingDevice end");
	}


TVersion CPBPrintingDevice::Version()
	{
	return TVersion(KPBVersionMajor, KPBVersionMinor, KPBVersionBuild);
	}

TUint CPBPrintingDevice::SupportedProtocols()
	{
	return KImagePrint_PrinterProtocol_PictBridge;
	}

void CPBPrintingDevice::StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocol)
	{		
	LOG1("CPBPrintingDevice::StartDiscoveryL aProtocol: %d", aProtocol);
	iDiscoveryObserver = &aObserver;
		
	if ( IsActive() )
		{
		LOG1("CPBPrintingDevice::StartDiscoveryL invalid state %d, cannot start discovery", iState);
		iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrInvalidSequence, 0 );
		return;
		}
				
	if ( !( aProtocol & KImagePrint_PrinterProtocol_PictBridge ) )
		{
		LOG("CPBPrintingDevice::StartDiscoveryL NO protocol match, ending discovery...");
		iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrNotSupported, 0 );
		return;
		}

	if ( !iPrinterConnected )
		{
		LOG("CPBPrintingDevice::StartDiscoveryL no printer connected, ending discovery");
		iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrNone, 0 );
		return;
		}


	// clear capability map
	iCapToDps.Close();

    iConfigS.iReqParam.Reset();
    iDpsEngine->GetDpsConfigL( iConfigS.iReqParam );
    iDpsEngine->DoDpsRequestL( &iConfigS, iStatus );
	iState = EPBDiscovery;
    SetActive();
	LOG("CPBPrintingDevice::StartDiscoveryL setting state to Discovery");
		
	LOG("CPBPrintingDevice::StartDiscoveryL end");
	}


TInt CPBPrintingDevice::RemoveCachedDeviceL( TInt aDeviceID )
	{
  	LOG1("CPBPrintingDevice::RemoveCachedDeviceL, aDeviceID: %d", aDeviceID);
  	return KErrInvalidData;
	}

void CPBPrintingDevice::CancelDiscovery( TBool aDoCallBack )
	{
  	LOG2("CPBPrintingDevice::CancelDiscovery aDoCallBack %d, iState %d", aDoCallBack, iState );  	
  	if ( iState == EPBDiscovery )
	  	{
	  	Cancel();
		if ( aDoCallBack && iDiscoveryObserver )
			{
			//We're not interested if the call back leaves
			TRAP_IGNORE( iDiscoveryObserver->DiscoveryStatusL( EDiscoveryCancelling, KErrNone, 0 ) );
			//We're not interested if the call back leaves
			TRAP_IGNORE( iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrNone, 0 ) );
			}
		iState = EPBUninitialized;
	  	}  	
	LOG("CPBPrintingDevice::CancelDiscovery end");
	}


TInt CPBPrintingDevice::CreatePrintJobL(TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver)
	{
  	LOG2("CPBPrintingDevice::CreatePrintJobL begin, deviceid %d imagecount %d", aDeviceID, aImages.Count());
	iPrintEventObserver = &aObserver;		

	if ( aDeviceID != 0 )
		{
		LOG("CPBPrintingDevice::CreatePrintJobL invalid device");
		return KErrArgument;
		}
		
	if ( iState != EPBReady && iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::CreatePrintJobL invalid state %d", iState);
		if(iDisconnectPrinter) // If printer is disconnected then return KErrDisconnected.
			return KErrDisconnected;
		else 
			return KErrInvalidSequence;
		}
		
    iStart.iReqParam.Reset();                                                                               
		
	// these must be appended in same order as PBJobSettingIndex
	
    TDpsArgsInt arg;    
    arg.iElement = EDpsArgPaperSize;
    arg.iContent = EDpsPaperSizeDefault;
    iStart.iReqParam.iJobConfig.Append(arg);

    arg.iElement = EDpsArgQuality;
    arg.iContent = EDpsPrintQualityDefault;
    iStart.iReqParam.iJobConfig.Append(arg);
				
    arg.iElement = EDpsArgLayout;
    arg.iContent = EDpsLayoutDefault;
    iStart.iReqParam.iJobConfig.Append(arg);
                        	
	for ( TInt i = 0; i < aImages.Count(); ++i )
		{
	    TDpsPrintInfo info;
	    info.Reset();
	    const TDesC& image = *aImages[i];
	    LOG1("CPBPrintingDevice::CreatePrintJobL image %S", &image);
	    info.iFile.Copy( image );
	    iStart.iReqParam.iPrintInfo.Append( info );
		}

	LOG("CPBPrintingDevice::CreatePrintJobL setting state to JobReady");
	iState = EPBJobReady;
	
	LOG("CPBPrintingDevice::CreatePrintJobL end");
	return KErrNone;
	}

void CPBPrintingDevice::SubmitPrintJobL()
	{
	LOG("CPBPrintingDevice::SubmitPrintJobL begin");

	// Checking that job is ready and printer has send aStatus.iRepParam.iNewJobOk
	// Check PrinterNotify function
	if ( iState == EPBJobReady && iNewJobOK )
		{
		iDpsEngine->DoDpsRequestL( &iStart, iStatus );
		iState = EPBSubmittingJob;
		SetActive();
		LOG("CPBPrintingDevice::SubmitPrintJobL setting state to SubmittingJob");
		}
	else
		{
		LOG1("CPBPrintingDevice::SubmitPrintJobL invalid state %d", iState);
		if( iDisconnectPrinter )
		    {
		    User::Leave( KErrDisconnected );
		    }
		else
		    {
		  	// Job or printer not ready, inform user
		  	if( iPrintEventObserver )
		  	    {
    		  	LOG("CPBPrintingDevice::SubmitPrintJobL sending EPbStatusErrorReasonHardwarePrinterBusy message to UI");
    		  	iPrintEventObserver->PrinterStatusEvent( EPbStatusErrorReasonHardwarePrinterBusy );
		  	    }
		    else
		        {
       			LOG1("CPBPrintingDevice::SubmitPrintJobL invalid state %d", iState);
       			User::Leave( KErrInvalidSequence );
		        }
		    }
		
		}
	
	LOG("CPBPrintingDevice::SubmitPrintJobL end");
	}

TInt CPBPrintingDevice::CancelPrintJob()
	{
	LOG("CPBPrintingDevice::CancelPrintJob begin");

    if( iCancelRetry > KMaxRetry )
        {
        return KErrInUse;
        }
	
	if ( iState == EPBSubmittingJob || iState == EPBPrinting || iState == EPBCancelling )
		{
        Cancel();
        iAbort.iReqParam.iAbortStyle = EDpsAbortStyleImmediately;
        TRAPD( err, iDpsEngine->DoDpsRequestL( &iAbort, iStatus ) );
        if ( err != KErrNone )
	        {
	        LOG1("CPBPrintingDevice::CancelPrintJob FATAL, DoDpsRequestL failed %d, setting state to Uninitialized", err);
	       	iState = EPBUninitialized;
	       	return KErrGeneral;
	        }        
        iState = EPBCancelling;
        SetActive();
        LOG("CPBPrintingDevice::CancelPrintJob setting state to Cancelling");
		}	
	else if ( iState == EPBJobReady )
		{
		LOG("CPBPrintingDevice::CancelPrintJob state is JobReady so doing nothing");
		return KErrNone;
		}
	else 
		{
		LOG1("CPBPrintingDevice::CancelPrintJob invalid state %d", iState);
		return KErrInvalidSequence;
		}
		
	LOG("CPBPrintingDevice::CancelPrintJob end");
	return KErrNone;
	}

TInt CPBPrintingDevice::ContinuePrintJobL()
	{
	LOG1("CPBPrintingDevice::ContinuePrintJobL returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CPBPrintingDevice::GetNumPrintPages()
	{
	LOG("CPBPrintingDevice::GetNumPrintPages begin");
	
	if ( iState != EPBJobReady && iState != EPBSubmittingJob && iState != EPBPrinting )
		{
		LOG1("CPBPrintingDevice::GetNumPrintPages invalid state %d, returning 0", iState);
		return 0;
		}

	TInt imagesPerPage = 1;
	TDpsArgsInt& config = iStart.iReqParam.iJobConfig[EPBSettingLayout];
	switch ( config.iContent )
		{        
        case EDpsLayout2Up:
			imagesPerPage = 2;
        	break;        
        case EDpsLayout4Up:
			imagesPerPage = 4;
        	break;        
        case EDpsLayout6Up:
			imagesPerPage = 6;
        	break;        
        case EDpsLayout9Up:
			imagesPerPage = 9;
        	break;        	        			        		        						
		default:
			break;
		}

	TInt numPrintPages = (iStart.iReqParam.iPrintInfo.Count() + imagesPerPage - 1) / imagesPerPage;
	LOG1("CPBPrintingDevice::GetNumPrintPages end, result %d", numPrintPages);
	return numPrintPages;
	}

TInt CPBPrintingDevice::GetPrintJobStatus()
	{
	LOG1("CPBPrintingDevice::GetPrintJobStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CPBPrintingDevice::GetPrinterStatus(TInt /*aDeviceID*/)
	{
	LOG1("CPBPrintingDevice::GetPrinterStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CPBPrintingDevice::GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs)
	{
	LOG1("CPBPrintingDevice::GetDeviceCapabilityIDsL begins with aDeviceID: %d", aDeviceID);

	if ( aDeviceID != 0 )
		{
		LOG("CPBPrintingDevice::GetDeviceCapabilityL invalid device");
		return KErrArgument;
		}		
					
	if ( iState != EPBReady && iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::GetDeviceCapabilityL invalid state %d", iState);
		return KErrInvalidSequence;
		}	
	
	aCapabilityIDs.Append( EPrintCapabLayout );
	aCapabilityIDs.Append( EPrintCapabQuality );
	aCapabilityIDs.Append( EPrintCapabPaperSize );
		
	LOG("CPBPrintingDevice::GetDeviceCapabilityIDsL end");
	return KErrNone;
	}

TInt CPBPrintingDevice::GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability)
	{		
	LOG2("CPBPrintingDevice::GetDeviceCapabilityL begins with aDeviceID %d, aCapabilityID %d", aDeviceID, aCapabilityID);

	if ( aDeviceID != 0 )
		{
		LOG("CPBPrintingDevice::GetDeviceCapabilityL invalid device");
		return KErrArgument;
		}		
					
	if ( iState != EPBReady && iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::GetDeviceCapabilityL invalid state %d", iState);
		return KErrInvalidSequence;
		}	

	if ( aCapabilityID == EPrintCapabPaperSize )
		{
		// paper sizes
		iCap.iRepParam.Reset();
		iCap.iReqParam.iCap = EDpsArgPaperSizes;
		iDpsEngine->DoDpsRequestL(&iCap, iStatus);
		SetActive();
		iWait.Start();
		HandlePapersizesResponseL( aCapability );
		
		if ( !aCapability.iEnumCount )
			{
			LOG("CPBPrintingDevice::GetDeviceCapabilityL paper sizes not supported, set to default");
	    	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSizeAuto;
	    	iCapToDps.InsertL( EPrintCapabPaperSizeAuto, EDpsPaperSizeDefault );
			}				    		
	    }
	else if ( aCapabilityID == EPrintCapabQuality )
		{
		// qualities
		LOG("CPBPrintingDevice::GetDeviceCapabilityL qualities");
		iCap.iRepParam.Reset();
		iCap.iReqParam.iCap = EDpsArgQualities;
		iDpsEngine->DoDpsRequestL( &iCap, iStatus );
		SetActive();
		iWait.Start();
		HandleQualitiesResponseL( aCapability );
		}
	else if ( aCapabilityID == EPrintCapabLayout )
		{			
		// layouts		
		LOG("CPBPrintingDevice::GetDeviceCapabilityL layouts");
		iCap.iRepParam.Reset();
		iCap.iReqParam.iCap = EDpsArgLayouts;
		iCap.iReqParam.iAttribute = iUserSelectedPaperSize;
		LOG("CPBPrintingDevice::GetDeviceCapabilityL layouts dodpsrequest");		
		iDpsEngine->DoDpsRequestL(&iCap, iStatus);
		SetActive();
		LOG("CPBPrintingDevice::GetDeviceCapabilityL layouts wait");
		iWait.Start();
		HandleLayoutsResponseL( aCapability );
		}
	else
		{
		LOG("CPBPrintingDevice::GetDeviceCapabilityL invalid capabilityID");
		return KErrArgument;
		}	    
		
	LOG1("CPBPrintingDevice::GetDeviceCapabilityL iCapToDps count %d", iCapToDps.Count());

	LOG("CPBPrintingDevice::GetDeviceCapabilityL end");
	return KErrNone;
	}

TInt CPBPrintingDevice::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
	LOG1("CPBPrintingDevice::GetJobSetting begin, aCapabilityID %d", aCapabilityID);

	TInt err = KErrNone;		
	if ( iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::GetJobSetting invalid state %d", iState);
		err = KErrInvalidSequence;
		}	
	else if ( aCapabilityID == EPrintCapabPaperSize )
		{
		err = GetPapersizeJobSetting( aValue );
	    }
	else if ( aCapabilityID == EPrintCapabQuality )
		{
		err = GetQualityJobSetting( aValue );
		}
	else if ( aCapabilityID == EPrintCapabLayout )
		{
		err = GetLayoutJobSetting( aValue );
		}
	else
		{
		LOG("CPBPrintingDevice::GetJobSetting invalid capabilityID");
		err = KErrArgument;
		}	    
		
	LOG2("CPBPrintingDevice::GetJobSetting end, return code %d value %d", err, aValue);
	return err;
	}

TInt CPBPrintingDevice::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	LOG2("CPBPrintingDevice::SetJobSettingL begin, aCapabilityID %d aValue %d", aCapabilityID, aValue);

	aAffectedCapability = aCapabilityID;
	
	RArray<TDpsArgsInt>& config = iStart.iReqParam.iJobConfig;
	TInt err = KErrNone;
	if ( iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::SetJobSettingL invalid state %d", iState);
		err = KErrInvalidSequence;
		}
	else if ( aCapabilityID == EPrintCapabPaperSize )
		{
		TInt* dpsValue = iCapToDps.Find( aValue );
		if ( dpsValue )
		    {
			config[EPBSettingPaperSize].iContent = *dpsValue;
		
			// for CertSW: needs layouts for particular paper sizes
			iUserSelectedPaperSize = config[EPBSettingPaperSize].iContent;
		    }
		else
			{
			LOG("CPBPrintingDevice::SetJobSettingL invalid value");
			err = KErrInvalidData;
			}
	    }
	else if ( aCapabilityID == EPrintCapabQuality )
		{
		TInt* dpsValue = iCapToDps.Find( aValue );
		if ( dpsValue )
			{
			config[EPBSettingQuality].iContent = *dpsValue;
			}
		else
			{
			LOG("CPBPrintingDevice::SetJobSettingL invalid value");
			err = KErrInvalidData;
			}
		}
	else if ( aCapabilityID == EPrintCapabLayout )
		{
		TInt* dpsValue = iCapToDps.Find( aValue );
		if ( dpsValue )
			{
			config[EPBSettingLayout].iContent = *dpsValue;
			}
		else
			{
			LOG("CPBPrintingDevice::SetJobSettingL invalid value");
			err = KErrInvalidData;
			}
		}
	else
		{
		LOG("CPBPrintingDevice::SetJobSettingL invalid capabilityID");
		err = KErrArgument;
		}	    
		
	LOG1("CPBPrintingDevice::SetJobSettingL end, return code %d", err);
	return err;	
	}


TInt CPBPrintingDevice::GetNumPreviewPages()
	{
	LOG1("CPBPrintingDevice::GetNumPreviewPages ends with: %d", 1);
	return 1;
	}

TInt CPBPrintingDevice::GetJobTemplateIconL( TInt /*aTemplateID*/, TInt& /*aFsBitmapHandle*/ )
	{
	return KErrNone;
	}


TInt CPBPrintingDevice::CreatePreviewImage(TInt /*aPageNumber*/)
	{
	return KErrNotSupported;
	}

void CPBPrintingDevice::RegisterIdleObserver( MProtIdleObserver *aObserver )
	{
	iIdleObserver = aObserver;
	}

void CPBPrintingDevice::DoCancel()
	{
	iDpsEngine->CancelDpsRequest();
	if ( iWait.IsStarted() )
		{
		LOG("CPBPritingDevice::DoCancel() calling iWait.AsyncStop()");
		iWait.AsyncStop();
		}	
	}

void CPBPrintingDevice::RunL()
	{
	LOG2("CPBPrintingDevice::RunL begin, status %d state %d", iStatus.Int(), iState);
		
   	if ( iWait.IsStarted() )
    	{
    	LOG("CPBPrintingDevice::RunL calling iWait.AsyncStop() and returning");
    	iWait.AsyncStop();
    	return;
    	}
    
    if ( iStatus.Int() == KErrNone ) // everything ok
        {
    	switch( iState )
    		{
    		case EPBDiscovery:
    			{    	   			
    			iPrinter.iPrinterID = 0;
                iPrinter.iProtocol = KImagePrint_PrinterProtocol_PictBridge;
    	        iPrinter.iProperties = 0;

    			if( iConfigS.iResult.iMajorCode == EDpsResultOk ) 
    			    {
    			    iPrinter.iVendor = TPrinter::EVendorNone;
    			
    			    HBufC* buf = HBufC::NewLC( iConfigS.iRepParam.iProductName.Length() );
    			    TPtr p = buf->Des();
    			    p.Copy( iConfigS.iRepParam.iProductName );
    			    if ( p.Length() > 84 )
    				    {
    				    TPtrC p2 = p.Left( 84 );
    				    iPrinter.iDisplayName = p2;
    				    }
    			    else
    				    {
    				    iPrinter.iDisplayName = p;
    				    }
    			    
    			    CleanupStack::PopAndDestroy( buf );
    			    LOG1("CPBPrinterDevice::RunL config displayname %S", &iPrinter.iDisplayName);
    			    
    			    // Config info have to be saved in file, because it can retrieved only once
    			    // when cable is plugged in the device
    			    SaveConfigInfoL();
    			    }
    			
                if( iConfigS.iResult.iMajorCode == EDpsResultNotExecuted )
                    {
                    // Read info from file
                    ReadConfigInfoL();
    			    }

    		    iDiscoveryObserver->FoundDeviceL( iPrinter );
    		    iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrNone, 0 );
    		    iState = EPBReady;

    			// Close Dps request to avoid -14 error
    			iDpsEngine->CancelDpsRequest();
    		    iDpsEngine->CancelDpsEventNotify();

    			LOG("CPBPrintingDevice::RunL setting state to Ready");
    			break;
    			}    		
    		case EPBSubmittingJob:
    			LOG2("CPBPrintingDevice::RunL submittingjob result major 0x%x minor 0x%x", iStart.iResult.iMajorCode, iStart.iResult.iMinorCode);
    			if ( iStart.iResult.iMajorCode == EDpsResultOk )
    				{
    				LOG("CPBPrintingDevice::RunL setting state to Printing");
    				iState = EPBPrinting;
    				
    				// for Cert software remove dialog from UI 
    				iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ENoMessage );
    				}
    			else
    				{
    				LOG("CPBPrintingDevice::RunL error occurred, setting state to JobReady");
    				iState = EPBJobReady;
    				if ( iPrintEventObserver )
    					{
    					TPrintMessageCodes err = EPbStatusErrorReasonNoReason;
    					if ( iStart.iResult.iMajorCode == EDpsResultNotSupported &&
    						 iStart.iResult.iMinorCode == EDpsResultMissingParam )
    						{
    						// image file is probably missing
    						err = EPbFileNotFound;
    						}						
    					iPrintEventObserver->PrinterStatusEvent( err );
    					iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ECancelling );
    					}
    				}
    			break;    			
    		case EPBCancelling:			
    			LOG2("CPBPrintingDevice::RunL cancelling result major 0x%x minor 0x%x", iAbort.iResult.iMajorCode, iAbort.iResult.iMinorCode);
    			
    			if( iAbort.iResult.iMajorCode == EDpsResultNotExecuted )
    			    {
    			    iCancelRetry++;
    			    CancelPrintJob();
    			    }
    			
    			if( iAbort.iResult.iMajorCode == EDpsResultOk )
    			    {
    			    if ( iPrintEventObserver )
    				    {
    				    iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ECancelling );
    				    }

    			        LOG("CPBPrintingDevice::RunL setting state to JobReady");
    			        iState = EPBJobReady;
    			        iCancelRetry = 0;
    			        }
    			break;    		
    		default:
    			LOG1("CPBPrintingDevice::RunL unrecognized state %d", iState);
    			break;
		}	
    } 
	else // iStatus != KErrNone -> Handle error here 
	    {
	    switch( iStatus.Int()  )
    		{
    		case KErrTimedOut:
    			LOG( "CPBPrintingDevice::RunL KErrTimedOut" );
				if ( iPrintEventObserver )
			        {
				    LOG("CPBPrintingDevice::Timeout, finish printing...");
				    iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ECancelling );
				    }
    			break;
    		default:
    		    // Handle error here.
    		    LOG2( "CPBPrintingDevice::RunL error, status %d state %d", iStatus.Int(), iState );
    		    break;
    		}
	    }
	
	LOG("CPBPrintingDevice::RunL end");
	}

TInt CPBPrintingDevice::RunError( TInt aError )
	{
	LOG1("CPBPrintingDevice::RunError code %d", aError);
	if ( iWait.IsStarted() )
		{		
		LOG("CPBPrintingDevice::RunError calling iWait.AsyncStop()");
		iWait.AsyncStop();
		}
	return KErrNone;
	}

		
void CPBPrintingDevice::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
	{
	LOG("CPBPrintingDevice::SetNumsOfCopiesL begin");
		
	if ( iState != EPBJobReady )
		{
		LOG1("CPBPrintingDevice::SetNumsOfCopiesL invalid state %d", iState);
		aErr = KErrInvalidSequence;
		}		
	else if ( aNumsOfCopies.Count() != iStart.iReqParam.iPrintInfo.Count() )
		{
		LOG2("CPBPrintingDevice::SetNumsOfCopiesL counts don't match, %d %d", aNumsOfCopies.Count(), iStart.iReqParam.iPrintInfo.Count());
		aErr = KErrInvalidData;
		}
	else
		{	
		for ( TInt i = 0; i < aNumsOfCopies.Count(); ++i )
			{
			iStart.iReqParam.iPrintInfo[i].iCopies = aNumsOfCopies[i];
			}			
		aErr = KErrNone;
		}
	
	LOG("CPBPrintingDevice::SetNumsOfCopiesL end");
	}


void CPBPrintingDevice::ConnectionNotifyL( TInt aStatus )
	{
	LOG2("CPBPrintingDevice::ConnectionNotifyL begin, status %d iState %d", aStatus, iState);
    switch ( aStatus )
        {        
        case CDpsEngine::ENotConnected:
        	LOG("CPBPrintingDevice::ConnectionNotify not connected");
        	break;
        case CDpsEngine::EPrinterConnected:
        	LOG("CPBPrintingDevice::ConnectionNotifyL printer connected");
        	iPrinterConnected = ETrue;
        	iDisconnectPrinter = EFalse;
	        if ( iIdleObserver )
		        {
		        LOG("CPBPrintingDevice::ConnectionNotifyL informing idle observer about connect");
		        TEvent event = { KImagePrint_PrinterProtocol_PictBridge, ENotSpecified, EConnect };
		        iIdleObserver->StatusEvent( event, KErrConnected, ENoMessage );
	    	    }
        	break;        
        case CDpsEngine::EPrinterDisconnected:
					iPrinterConnected = EFalse;
					iDisconnectPrinter = ETrue;
        	if ( iState != EPBUninitialized )
	        	{
	        	Cancel();
	        	if ( iState == EPBDiscovery )
		        	{
		        			//We're not interested if the call back leaves
							TRAP_IGNORE( iDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, KErrNone, 0 ) );
		        	}		        	
					LOG("CPBPrintingDevice::ConnectionNotifyL printer disconnected, setting state to Uninitialized");
	        	iState = EPBUninitialized;
	        	}
	        if ( iIdleObserver )
		        {
		        LOG("CPBPrintingDevice::ConnectionNotifyL informing idle observer about disconnect");
		        TEvent event = { KImagePrint_PrinterProtocol_PictBridge, ENotSpecified, EDisconnect };
		        iIdleObserver->StatusEvent( event, KErrDisconnected, ENoMessage );
	    	    }
	        break;	        
        case CDpsEngine::EOtherConnected:
        	LOG("CPBPrintingDevice::ConnectionNotify other connected");
        	break;        
        case CDpsEngine::EWrongPrintModeConnected: 
            LOG("CPBPrintingDevice::ConnectionNotify Other personality");
            iPrinterConnected = ETrue;
            iDisconnectPrinter = EFalse;
	        if ( iIdleObserver )
		        {
		        LOG("CPBPrintingDevice::ConnectionNotifyL informing idle observer about connect");
		        TEvent event = { KImagePrint_PrinterProtocol_PictBridge, ENotSpecified, EConnect };
		        iIdleObserver->StatusEvent( event, KErrConnected, ENoMessage );
	    	    }
        	break;                        
        default:
        	LOG1("CPBPrintingDevice::ConnectionNotifyL unrecognized status %d", aStatus);
	        break;
        }
	LOG("CPBPrintingDevice::ConnectionNotifyL end");
	}

void CPBPrintingDevice::JobNotify( const TDpsGetJobStatus& aStatus )
	{		
	LOG1("CPBPrintingDevice::JobNotify begin, state %d", iState);
    LOG2("CPBPrintingDevice::JobNotify progress %d, prtPID %d", aStatus.iRepParam.iProgress, aStatus.iRepParam.iPrtPID);
    LOG1("CPBPrintingDevice::JobNotify copyID %d", aStatus.iRepParam.iCopyID);
    LOG2("CPBPrintingDevice::JobNotify filepath %S, images printed %d", &aStatus.iRepParam.iFilePath, aStatus.iRepParam.iImagesPrinted);        
    
    if ( iPrintEventObserver )
	    {
	    iPrintEventObserver->PrintJobProgressEvent( EActive, aStatus.iRepParam.iProgress, ENoMessage );
	    }
    
	LOG("CPBPrintingDevice::JobNotify end");
	}

void CPBPrintingDevice::PrinterNotify( const TDpsGetPrinterStatus& aStatus )
	{		
	LOG1("CPBPrintingDevice::PrinterNotify begin, state %d", iState );
    LOG2("CPBPrintingDevice::PrinterNotify service status 0x%x, job status 0x%x", aStatus.iRepParam.iPrintStatus, aStatus.iRepParam.iJobStatus);
    LOG2("CPBPrintingDevice::PrinterNotify error status 0x%x, job end reason 0x%x", aStatus.iRepParam.iErrorStatus, aStatus.iRepParam.iJobEndReason.iMajor);
    LOG1("CPBPrintingDevice::PrinterNotify aStatus.iRepParam.iNewJobOk 0x%x,", aStatus.iRepParam.iNewJobOk);
    
    // New job checking that job is ok and device status is ok.
    iNewJobOK = aStatus.iRepParam.iNewJobOk;
    
    TPBProtocolState oldState = iState;
    if ( iState == EPBPrinting )
		{
		// for SWCert software to indicate that printing state has executed
		iPrintingStateComplete = 1;
		
		if ( aStatus.iRepParam.iJobStatus == EDpsJobStatusEndedOk )
			{
			LOG("CPBPrintingDevice::PrinterNotify job finished successfully, setting state to JobReady");
			if ( iPrintEventObserver )
				{
				LOG("CPBPrintingDevice::PrinterNotify job finished iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ENoMessage );");
				iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ENoMessage );
				}
			iState = EPBJobReady;
			}
		else if ( aStatus.iRepParam.iJobStatus != EDpsJobStatusNotEnded )
			{
			LOG("CPBPrintingDevice::PrinterNotify job aborted, setting state to JobReady");
			if ( iPrintEventObserver )
				{
				iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ECancelling );
				}
			iState = EPBJobReady;
			}
		}

    if ( iState == EPBJobReady )
        { 
        // for Cert software: CertSW doesn't give any indication about aStatus.iRepParam.iJobStatus 
        // it only indicate printer state. We have to send message to UI to remove generig progress bar.
        if ( aStatus.iRepParam.iPrintStatus == EDpsPrintServiceStatusIdle && iPrintingStateComplete )
            {
			// for Cert software to indicate that idle state has executed
			iPrintingStateComplete = 0;
			
			if ( iPrintEventObserver )
				{
			    LOG("CPBPrintingDevice::PrinterNotify job finished iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ENoMessage );");
			    
			    // for Cert software remove dialog from UI 
			    iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ENoMessage );
				}
            }
        }

    if ( !iPrintEventObserver )
	    {
	    LOG("CPBPrintingDevice::PrinterNotify no event observer, returning");
	    return;
	    }
    
	TPrintMessageCodes err = DpsJobError( aStatus.iRepParam.iJobEndReason );
    if ( err == ENoMessage && aStatus.iRepParam.iErrorStatus == EDpsErrorStatusFatal )
		{
		// Show fatal error
		err = EPbStatusErrorReasonHardwareFatal;
		}	        
    
    if ( err == ENoMessage && aStatus.iRepParam.iErrorStatus == EDpsErrorStatusWarning )
		{
		// Show warning
		err = EPbStatusErrorReasonWarning;
		}	        

    if ( err != ENoMessage )
	    {
	    if ( oldState == EPBPrinting )
		    {
		    LOG1("CPBPrintingDevice::PrinterNotify calling PrintJobErrorEvent with code %d", err);
		    iPrintEventObserver->PrintJobErrorEvent( err );
			}
		else
			{
			LOG1("CPBPrintingDevice::PrinterNotify calling PrinterStatusEvent with code %d", err);
			iPrintEventObserver->PrinterStatusEvent( err );
			}
	    }
    				
	LOG("CPBPrintingDevice::PrinterNotify end");
	}

void CPBPrintingDevice::HandlePapersizesResponseL( TPrintCapability& aCapability )
	{
    LOG2("CPBPrintingDevice::HandlePapersizesResponseL result major 0x%x minor 0x%x", iCap.iResult.iMajorCode, iCap.iResult.iMinorCode);
	LOG2("CPBPrintingDevice::HandlePapersizesResponseL cap 0x%x count %d", iCap.iRepParam.iCap, iCap.iRepParam.iContent.Count());	
	
	aCapability.iCapabilityID = EPrintCapabPaperSize;
	aCapability.iType = TPrintCapability::Int;
	aCapability.iDefaultValue = EPrintCapabPaperSizeAuto;
	aCapability.iEnumCount = 0;
	
	if ( iCap.iResult.iMajorCode == EDpsResultOk )
		{		
	    for ( TInt i = 0; i < iCap.iRepParam.iContent.Count() && aCapability.iEnumCount < KMaxEnumAmount; ++i )
	        {
	        LOG1("CPBPrintingDevice::HandlePapersizesResponseL content 0x%x", iCap.iRepParam.iContent[i]);
	        switch ( iCap.iRepParam.iContent[i] )
		        {
		        case EDpsPaperSizeDefault:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSizeAuto;
		        	iCapToDps.InsertL( EPrintCapabPaperSizeAuto, EDpsPaperSizeDefault );
		        	break;		        	
	        	case EDpsPaperSizeLetter:
	        		aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSizeLetter;
	        		iCapToDps.InsertL( EPrintCapabPaperSizeLetter, EDpsPaperSizeLetter );
	        		break;	        		
	        	case EDpsPaperSizeA4:
	        		aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSizeA4;
	        		iCapToDps.InsertL( EPrintCapabPaperSizeA4, EDpsPaperSizeA4 );
	        		break;	        		
	        	case EDpsPaperSizeA6:
	        		aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSizeA6;
	        		iCapToDps.InsertL( EPrintCapabPaperSizeA6, EDpsPaperSizeA6 );
	        		break;	        		
	        	case EDpsPaperSize4x6:
	        		aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSize4x6;
	        		iCapToDps.InsertL( EPrintCapabPaperSize4x6, EDpsPaperSize4x6 );
	        		break;
	        	case EDpsPaperSize2L:
	        		aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabPaperSize5x7;
	        		iCapToDps.InsertL( EPrintCapabPaperSize5x7, EDpsPaperSize2L );
	        		break;	        			        		
	        	default:
	        		break;
	    	    }
	        }
	    }
	}

	
void CPBPrintingDevice::HandleQualitiesResponseL( TPrintCapability& aCapability )
	{
    LOG2("CPBPrintingDevice::HandleQualitiesResponseL result major 0x%x minor 0x%x", iCap.iResult.iMajorCode, iCap.iResult.iMinorCode);
	LOG2("CPBPrintingDevice::HandleQualitiesResponseL cap 0x%x count %d", iCap.iRepParam.iCap, iCap.iRepParam.iContent.Count());	
	
	aCapability.iCapabilityID = EPrintCapabQuality;
	aCapability.iType = TPrintCapability::Int;
	aCapability.iDefaultValue = EPrintCapabQualityDefault;
	aCapability.iEnumCount = 0;

	if ( iCap.iResult.iMajorCode == EDpsResultOk )
		{		
	    for ( TInt i = 0; i < iCap.iRepParam.iContent.Count() && aCapability.iEnumCount < KMaxEnumAmount; ++i )
	        {
	        LOG1("CPBPrintingDevice::HandleQualitiesResponseL content 0x%x", iCap.iRepParam.iContent[i]);
	        switch ( iCap.iRepParam.iContent[i] )
		        {
		        case EDpsPrintQualityDefault:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabQualityDefault;
		        	iCapToDps.InsertL( EPrintCapabQualityDefault, EDpsPrintQualityDefault );
		        	break;
		        case EDpsPrintQualityNormal:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabQualityNormal;
		        	iCapToDps.InsertL( EPrintCapabQualityNormal, EDpsPrintQualityNormal );
		        	break;
		        case EDpsPrintQualityDraft:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabQualityDraft;
		        	iCapToDps.InsertL( EPrintCapabQualityDraft, EDpsPrintQualityDraft );
		        	break;
		        case EDpsPrintQualityFine:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabQualityFine;
		        	iCapToDps.InsertL( EPrintCapabQualityFine, EDpsPrintQualityFine );
		        	break;		        	
		        default:
		        	break;
		        }
		    }
		}
	if ( !aCapability.iEnumCount )
		{
		LOG("CPBPrintingDevice::HandleQualitiesResponseL not supported, set to default");
    	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabQualityDefault;
    	iCapToDps.InsertL( EPrintCapabQualityDefault, EDpsPrintQualityDefault );
		}
	}

void CPBPrintingDevice::HandleLayoutsResponseL( TPrintCapability& aCapability )
	{		
    LOG2("CPBPrintingDevice::HandleLayoutsResponseL result major 0x%x minor 0x%x", iCap.iResult.iMajorCode, iCap.iResult.iMinorCode);
	LOG2("CPBPrintingDevice::HandleLayoutsResponseL cap 0x%x count %d", iCap.iRepParam.iCap, iCap.iRepParam.iContent.Count());
	
	aCapability.iCapabilityID = EPrintCapabLayout;
	aCapability.iType = TPrintCapability::Int;
	aCapability.iDefaultValue = EPrintCapabLayout1Up;
	aCapability.iEnumCount = 0;

	if ( iCap.iResult.iMajorCode == EDpsResultOk )
		{		
	    for ( TInt i = 0; i < iCap.iRepParam.iContent.Count() && aCapability.iEnumCount < KMaxEnumAmount; ++i )
	        {
	        LOG1("CPBPrintingDevice::HandleLayoutsResponseL content 0x%x", iCap.iRepParam.iContent[i]);
	        switch ( iCap.iRepParam.iContent[i] )
		        {
		        case EDpsLayout1Up:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout1Up;
		        	iCapToDps.InsertL( EPrintCapabLayout1Up, EDpsLayout1Up );
		        	break;		        
		        case EDpsLayout2Up:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout2Up;
		        	iCapToDps.InsertL( EPrintCapabLayout2Up, EDpsLayout2Up );
		        	break;		        
		        case EDpsLayout4Up:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout4Up;
		        	iCapToDps.InsertL( EPrintCapabLayout4Up, EDpsLayout4Up );
		        	break;		        
		        case EDpsLayout6Up:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout6Up;
		        	iCapToDps.InsertL( EPrintCapabLayout6Up, EDpsLayout6Up );
		        	break;		        
		        case EDpsLayout9Up:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout9Up;
		        	iCapToDps.InsertL( EPrintCapabLayout9Up, EDpsLayout9Up );
		        	break;		        	
		        // do not support this EDpsLayoutBorderless, set default value
		        case EDpsLayoutBorderless:
		        	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout1UpBorderless;
		        	iCapToDps.InsertL( EPrintCapabLayout1UpBorderless, EDpsLayoutDefault );
		        	break;		        		        	
		        default:
		        	break;
		        }			
		    }
		}
	if ( !aCapability.iEnumCount )
		{
		LOG("CPBPrintingDevice::HandleLayoutsResponseL not supported, set to default");
    	aCapability.iEnumCodes[aCapability.iEnumCount++] = EPrintCapabLayout1Up;
    	iCapToDps.InsertL( EPrintCapabLayout1Up, EDpsLayoutDefault );
		}
	}

TInt CPBPrintingDevice::GetPapersizeJobSetting( TInt& aValue )
	{
	LOG("CPBPrintingDevice::GetPapersizeJobSetting begin");

	TDpsArgsInt& paper = iStart.iReqParam.iJobConfig[EPBSettingPaperSize];
	switch ( paper.iContent )
		{
		case EDpsPaperSizeDefault:
			aValue = EPrintCapabPaperSizeAuto;
			return KErrNone;		
    	case EDpsPaperSizeLetter:
			aValue = EPrintCapabPaperSizeLetter;
			return KErrNone;    		
    	case EDpsPaperSizeA4:
			aValue = EPrintCapabPaperSizeA4;
			return KErrNone;    		
    	case EDpsPaperSizeA6:
			aValue = EPrintCapabPaperSizeA6;
			return KErrNone;    		
    	case EDpsPaperSize4x6:
			aValue = EPrintCapabPaperSize4x6;
			return KErrNone;			
    	case EDpsPaperSize2L:
    		aValue = EPrintCapabPaperSize5x7;
    		return KErrNone;
		default:
			LOG1("CPBPrintingDevice::GetPapersizeJobSetting unknown %x", paper.iContent);
			break;
		}				

	LOG("CPBPrintingDevice::GetPapersizeJobSetting end");
	return KErrInvalidData;
	}

TInt CPBPrintingDevice::GetQualityJobSetting( TInt& aValue )
	{
	LOG("CPBPrintingDevice::GetQualityJobSetting begin");

	TDpsArgsInt& config = iStart.iReqParam.iJobConfig[EPBSettingQuality];
	switch ( config.iContent )
		{
        case EDpsPrintQualityDefault:
			aValue = EPrintCapabQualityDefault;
			return KErrNone;
        case EDpsPrintQualityNormal:
			aValue = EPrintCapabQualityNormal;
			return KErrNone;
        case EDpsPrintQualityDraft:
			aValue = EPrintCapabQualityDraft;
			return KErrNone;
        case EDpsPrintQualityFine:
			aValue = EPrintCapabQualityFine;
			return KErrNone;			
		default:
			LOG1("CPBPrintingDevice::GetQualityJobSetting unknown %x", config.iContent);
			break;
		}				

	LOG("CPBPrintingDevice::GetQualityJobSetting end");
	return KErrInvalidData;
	}

TInt CPBPrintingDevice::GetLayoutJobSetting( TInt& aValue )
	{
	LOG("CPBPrintingDevice::GetLayoutJobSetting begin");

	TDpsArgsInt& config = iStart.iReqParam.iJobConfig[EPBSettingLayout];
	switch ( config.iContent )
		{
		case EDpsLayoutDefault:
            // Not supported default value is EPrintCapabLayout1UpBorderless
            // because other protocols use it too
            // EPrintCapabLayout1UpBorderless is mapped with EDpsLayoutDefault
            aValue = EPrintCapabLayout1UpBorderless; 
        case EDpsLayout1Up:
			aValue = EPrintCapabLayout1Up;
        	return KErrNone;        
        case EDpsLayout2Up:
			aValue = EPrintCapabLayout2Up;
        	return KErrNone;        
        case EDpsLayout4Up:
			aValue = EPrintCapabLayout4Up;
        	return KErrNone;        
        case EDpsLayout6Up:
			aValue = EPrintCapabLayout6Up;
        	return KErrNone;        
        case EDpsLayout9Up:
			aValue = EPrintCapabLayout9Up;
        	return KErrNone;        	
        case EDpsLayoutBorderless:
			aValue = EPrintCapabLayout1UpBorderless;
        	return KErrNone;        			        		        						
		default:
			LOG1("CPBPrintingDevice::GetLayoutJobSetting unknown %x", config.iContent);
			break;
		}

	LOG("CPBPrintingDevice::GetLayoutJobSetting end");
	return KErrInvalidData;
	}

TPrintMessageCodes CPBPrintingDevice::DpsJobError( const TDpsJobEndReason& aReason )
	{
	TPrintMessageCodes err = ENoMessage;
    switch ( aReason.iMajor )
        {
        case EDpsJobErrorPaper:
            LOG1("CPBPrintingDevice::DpsJobError paper error 0x%x", aReason.iPaperMinor);
            HandlePaperError( aReason, err );
	        break;        
        case EDpsJobErrorInk:
            LOG1("CPBPrintingDevice::DpsJobError ink error 0x%x", aReason.iInkMinor);
            HandleInkError( aReason, err );
    	    break;        
        case EDpsJobErrorHardware:
            LOG1("CPBPrintingDevice::DpsJobError hardware error 0x%x", aReason.iHardMinor);
            HandleHwError( aReason, err );
	        break;        
        case EDpsJobErrorFile:
            LOG1("CPBPrintingDevice::DpsJobError file error 0x%x", aReason.iFileMinor);
            HandleFileError( aReason, err );   
    	    break;        
        default:
        	break;
        }
    return err;
	}
void CPBPrintingDevice::HandlePaperError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr )
	{
	  switch ( aReason.iPaperMinor )
        {
	    case EDpsPaperEmpty:
	    	aErr = EPbStatusErrorReasonPaperEmpty;
	    	break;	    	
	    case EDpsPaperLoad:
	    	aErr = EPbStatusErrorReasonPaperLoad;
	    	break;	    	
	    case EDpsPaperEject:
	    	aErr = EPbStatusErrorReasonPaperEject;
	    	break;	    	
	    case EDpsPaperMedia:
	    	aErr = EPbStatusErrorReasonPaperMedia;
	    	break;	    	
	    case EDpsPaperJam:
	    	aErr = EPbStatusErrorReasonPaperJam;
	    	break;	    
	    case EDpsPaperNearlyEmpty:
	    	aErr = EPbStatusErrorReasonPaperNearlyEmpty;
	    	break;	    
	    case EDpsPaperTypeSizeNoMatch:
	    	aErr = EPbStatusErrorReasonPaperCombination;
	    	break;        	
        default:
        	aErr = EPbStatusErrorReasonPaper;
        	break;
        }
	}

void CPBPrintingDevice::HandleInkError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr )
	{
	 switch ( aReason.iInkMinor )
        {
        case EDpsInkEmpty:
        	aErr = EPbStatusErrorReasonInkEmpty;
        	break;        	
	    case EDpsInkLow:
	    	aErr = EPbStatusErrorReasonInkLow;
	    	break;	    	
		case EDpsInkWaste:
			aErr = EPbStatusErrorReasonInkWaste;
			break;
        default:
        	aErr = EPbStatusErrorReasonInk;
        	break;        	
        }
	}

void CPBPrintingDevice::HandleHwError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr )
	{
	 switch ( aReason.iHardMinor )
        {
        case EDpsHardFatal:
        	aErr = EPbStatusErrorReasonHardwareFatal;
        	break;        	
	    case EDpsHardServiceCall:
	    	aErr = EPbStatusErrorReasonHardwareServiceCall;
	    	break;	    	
	    case EDpsHardNotAvailable:
	    	aErr = EPbStatusErrorReasonHardwarePrinterUnavailable;
	    	break;	    	
	    case EDpsHardBusy:
	    	aErr = EPbStatusErrorReasonHardwarePrinterBusy;
	    	break;	    	
	    case EDpsHardLever:
	    	aErr = EPbStatusErrorReasonHardwareLever;
	    	break;	    	
	    case EDpsHardCoverOpen:
	    	aErr = EPbStatusErrorReasonHardwareCoverOpen;
	    	break;	    	
	    case EDpsHardNoMarkingHead:
	    	aErr = EPbStatusErrorReasonHardwareNoMarkingAgent;
	    	break;	    	
	    case EDpsHardInkCoverOpen:
	    	aErr = EPbStatusErrorReasonHardwareCoverOpen;
	    	break;	    	
	    case EDpsHardNoInkCartridge:
	    	aErr = EPbStatusErrorReasonHardwareNoInkCartridge;
	    	break;
        default:
        	aErr = EPbStatusErrorReasonHardware;
        	break;
        }                        
	}

void CPBPrintingDevice::HandleFileError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr )
	{
	 switch ( aReason.iFileMinor )
        {
        case EDpsFilePrintInfo:
        	aErr = EPbStatusErrorReasonFilePrintInfo;
        	break;        	
        case EDpsFileDecode:
        	aErr = EPbStatusErrorReasonFileFileDecode;
        	break;        	
        default:
        	aErr = EPbStatusErrorReasonFile;
        	break;
        }
	}

TBool CPBPrintingDevice::HasCapability( TPrintCapability& aCapability, TPrintCapabilityOptions aOption )
	{
	for ( TInt i = 0; i < aCapability.iEnumCount; ++i )
		{
		if ( aCapability.iEnumCodes[i] == aOption )
    		{
    		return ETrue;
    		}
		}
	return EFalse;
	}

void CPBPrintingDevice::SaveConfigInfoL()
	{
	LOG("CPBPrintingDevice::SaveConfigInfoL begin");
	
	RFs rFs;
    RFile file;

   	User::LeaveIfError( rFs.Connect() );
    CleanupClosePushL( rFs );
    
    User::LeaveIfError( file.Replace( rFs, iCongFileName->Des() ,EFileWrite ) ); // create the file for writing

	RFileWriteStream stream( file );
    CleanupClosePushL( stream );
    
    ExternalizeL( stream );

	CleanupStack::PopAndDestroy( &stream );
	CleanupStack::PopAndDestroy( &rFs );
	
	LOG("CPBPrintingDevice::SaveConfigInfoL end");
	}

void CPBPrintingDevice::ExternalizeL( RWriteStream& aStream ) const
	{
	LOG("CPBPrintingDevice::ExternalizeL begin");
	
	aStream << iPrinter.iDisplayName;
	aStream.WriteUint32L( iPrinter.iVendor ); 
	aStream.CommitL();
	
	LOG("CPBPrintingDevice::ExternalizeL end");
	}

void CPBPrintingDevice::ReadConfigInfoL()
    {
    LOG("CPBPrintingDevice::ReadConfigInfoL begin");
    
    RFs rFs;
    RFile file;

	User::LeaveIfError( rFs.Connect() );
    CleanupClosePushL( rFs );
    
    User::LeaveIfError( file.Open( rFs, iCongFileName->Des() ,EFileRead ) ); 
    
	RFileReadStream readStream( file );
    CleanupClosePushL( readStream );

    readStream >> iPrinter.iDisplayName;
    iPrinter.iVendor = readStream.ReadUint32L(); 
	
 	CleanupStack::PopAndDestroy( &readStream );
	CleanupStack::PopAndDestroy( &rFs );
   
    LOG("CPBPrintingDevice::ReadConfigInfoL end");
    }

//  End of File
