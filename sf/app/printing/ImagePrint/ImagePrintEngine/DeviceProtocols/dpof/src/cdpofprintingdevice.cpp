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
* Description:  Contains the CDPOFPrintingDevice class definition.
*
*/


#include <e32std.h>
#include <barsc.h>
#include <dpof.rsg>
#include <bautils.h>

#include "cdpofprintingdevice.h"
#include "cconfirmationquery.h"
#include "cprintercapabilities.h"
#include "cprinterelement.h"
#include "imgprintkonst.h"
#include "tsetting.h"
#include "rsutils.h"
#include "printcapabilitycodes.h"
#include "printmessagecodes.h"
#include "imageprint.h"
#include "clog.h"
#include "cprintjob.h"

//  CONSTANTS
namespace
	{
	/// DPOF Version Major.
	const TInt KDPOFVersionMajor = 0;
	/// DPOF Version Minor.
	const TInt KDPOFVersionMinor = 0;
	/// DPOF Version Build.
	const TInt KDPOFVersionBuild = 1;

	_LIT( KResourceFileName, "imageprintdata\\protocols\\dpof.rsc" );
	
	const TInt KMmcPrinterId = 1;
	_LIT( KMmcPrinterName, "Memory Card" );
	}


MProtPrintingDevice* CDPOFPrintingDevice::NewL()
	{
	return new (ELeave) CDPOFPrintingDevice();
	}


CDPOFPrintingDevice::CDPOFPrintingDevice() : CActive( CActive::EPriorityStandard ),
											iPrintError( KErrNone ),
											iPrintErrorMsgCode( 0 )
	{
	CActiveScheduler::Add(this);
	}

void CDPOFPrintingDevice::ConstructL(const TDesC& /*aDLLPath*/)
	{
	LOG("CDPOFPrintingDevice::ConstructL begin");
	
	iCancelledByUser = EFalse;
	
	iPrintJob = new (ELeave) CPrintJob();
	
	iMappingArray.AppendL(TMapping(1,EPrintCapabLayout1UpMedium));
	iMappingArray.AppendL(TMapping(2,EPrintCapabLayout2Up));
	iMappingArray.AppendL(TMapping(4,EPrintCapabLayout4Up));
	iMappingArray.AppendL(TMapping(6,EPrintCapabLayout6Up));
	iMappingArray.AppendL(TMapping(9,EPrintCapabLayout9Up));
	iMappingArray.AppendL(TMapping(12,EPrintCapabLayout12Up));
	iMappingArray.AppendL(TMapping(16,EPrintCapabLayout16Up));
		
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	// Loading nearest Loc File.
	
	TFileName resourceFileName;
	resourceFileName.Append( KDC_RESOURCE_FILES_DIR );
	resourceFileName.Append( KResourceFileName );
	BaflUtils::NearestLanguageFile( fs, resourceFileName );
	RResourceFile resourceFile;
	resourceFile.OpenL( fs, resourceFileName );
	
	CleanupClosePushL( resourceFile );
	iCapabilities = LoadCapabilityL( resourceFile, DPOF_CAPABILITIES );
	CleanupStack::PopAndDestroy( 2 ); // resourceFile, fs

	iDPOF = CRsDpofEngine::NewL( *this );
	
	SetMMCPrinterL();

	LOG("CDPOFPrintingDevice::ConstructL end");
	}

void CDPOFPrintingDevice::SetMMCPrinterL()
	{
	LOG("CDPOFPrintingDevice::SetMMCPrinterL begin");
    
    // New implementation
	LOG("CDPOFPrintingDevice::SetMMCPrinterL calling iDPOF->SetMMCPathL ...");
	iDPOF->SetMMCPathL();
	
	if( iDPOF->HasMmc() )
		{
		CPrinterElement* printElem = CPrinterElement::NewLC();
		printElem->iPrinter.iDisplayName = KMmcPrinterName;
		printElem->iPrinter.iPrinterID = KMmcPrinterId;
		printElem->iPrinter.iProperties = TPrinter::SupportsPreview;
		printElem->iPrinter.iProtocol = KImagePrint_PrinterProtocol_DPOF;
		printElem->iTransport = DPOFTrans;
		printElem->iPrinterClass = EDefault;
		printElem->iPrinterCapabilities = CPrinterCapabilities::NewL( iCapabilities, printElem->iPrinterClass );
		User::LeaveIfError( iDeviceList.Append( printElem ) );
		CleanupStack::Pop( printElem );
		printElem = NULL;
		}

	LOG("CDPOFPrintingDevice::SetMMCPrinterL end");
	}


CDPOFPrintingDevice::~CDPOFPrintingDevice()
	{
	LOG("CDPOFPrintingDevice::~CDPOFPrintingDevice begin");
	Cancel();
	delete iDPOF;
	delete iCapabilities;
	iDeviceList.ResetAndDestroy();
	iDeviceList.Close();	
	iMappingArray.Close();
	delete iPrintJob;
	iReadyRequests.Close();	
	REComSession::DestroyedImplementation( iDtor_ID_Key );

	if (iConfirmationQuery)
	{
		delete iConfirmationQuery;
		iConfirmationQuery = NULL;
	}
	
	if (iPQ)
	{
		delete iPQ;
		iPQ = NULL;
	}
	
	if (iQuestion)
		{
		delete iQuestion;
		iQuestion = NULL;
		}
		
	LOG("CDPOFPrintingDevice::~CDPOFPrintingDevice end");
	}


TVersion CDPOFPrintingDevice::Version()
	{
	return TVersion(KDPOFVersionMajor, KDPOFVersionMinor, KDPOFVersionBuild);
	}

TUint CDPOFPrintingDevice::SupportedProtocols()
	{
	return KImagePrint_PrinterProtocol_DPOF;
	}


void CDPOFPrintingDevice::StartDiscoveryL(MProtDiscoveryObserver& aObserver, TUint aProtocol)
	{
	LOG1("CDPOFPrintingDevice::StartDiscoveryL aProtocol: %d", aProtocol);
	iDiscoveryObserver = &aObserver;
	Cancel();
	if( aProtocol & KImagePrint_PrinterProtocol_DPOF )
		{
		LOG("CDPOFPrintingDevice::StartDiscoveryL protocol match, starting discovery...");
		iDPOF->StartDiscovery( iStatus );
		if( !IsActive() ) SetActive();
		}
	else
		{
		LOG("CDPOFPrintingDevice::StartDiscoveryL NO protocol match, ending discovery...");
		AddReadyRequestAndSetActive( EDiscoveryDone );		
		}	
	LOG("CDPOFPrintingDevice::StartDiscoveryL end");
	}


TInt CDPOFPrintingDevice::RemoveCachedDeviceL( TInt aDeviceID )
	{
  	LOG1("CDPOFPrintingDevice::RemoveCachedDeviceL aDeviceID: %d", aDeviceID);
  	CPrinterElement* targetPrinter = FindDeviceL( aDeviceID );
  	LOG1("CDPOFPrintingDevice::RemoveCachedDeviceL targetPrinter address: %d", (TInt)targetPrinter);
  	return ( targetPrinter ) ? KErrNone : KErrInvalidData;
	}

void CDPOFPrintingDevice::CancelDiscovery( TBool aDoCallBack )
	{
  	LOG1("CDPOFPrintingDevice::CancelDiscovery aDoCallBack: %d", aDoCallBack);
	if( aDoCallBack )
		{				
		AddReadyRequestAndSetActive( ECancelDiscovery );
		}
	LOG("CDPOFPrintingDevice::CancelDiscovery end");
	}


TInt CDPOFPrintingDevice::CreatePrintJobL(TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver)
	{
  	LOG1("CDPOFPrintingDevice::CreatePrintJobL aDeviceID: %d", aDeviceID);
  	LOG1("CDPOFPrintingDevice::CreatePrintJobL aImages.Count(): %d", aImages.Count());

	iPrintEventObserver = &aObserver;
	iPrintJob->iState = PJSNone;  // Reset value until Job is fully setup in case of a leave.
	CPrinterElement *TargetPrinter = FindDeviceL( aDeviceID );
	if( TargetPrinter == NULL )
		{
		LOG("CDPOFPrintingDevice::CreatePrintJobL no device found, returning error");
		return KErrInvalidData;
		}

	iPrintJob->iTransport = DPOFTrans;
	iPrintJob->iPrinterID = aDeviceID;
	TInt NumImages = aImages.Count();
	iPrintJob->iImages.ResetAndDestroy();
	for (TInt i = 0 ; i < NumImages; i++)
		{
		HBufC* Image = aImages[i]->AllocLC();
		User::LeaveIfError( iPrintJob->iImages.Append( Image ) );
		CleanupStack::Pop(Image);
		}
	LOG("CDPOFPrintingDevice::CreatePrintJobL image names copied");

	// Now we can access the capabilities and fill in the printer settings.
	RArray<TInt> CapabilityIDs;
	CleanupClosePushL( CapabilityIDs );
	GetDeviceCapabilityIDsL( aDeviceID, CapabilityIDs );
	LOG1("CDPOFPrintingDevice::CreatePrintJobL capability ids count: %d", CapabilityIDs.Count());

	iPrintJob->iSettings.Reset();
	for (TInt i = 0; i < CapabilityIDs.Count(); i++)
		{
		TPrintCapability capability;
		GetDeviceCapabilityL(aDeviceID, CapabilityIDs[i], capability);
		TInt nCurrentValue;
		TBool bHasValue = TargetPrinter->iPrinterCapabilities->GetCurrentValue(CapabilityIDs[i], nCurrentValue);
		TSetting Setting;
		Setting.iCapabilityID = capability.iCapabilityID;
		if (bHasValue)
			Setting.iValue = nCurrentValue;
		else
			Setting.iValue = capability.iDefaultValue;

		User::LeaveIfError(iPrintJob->iSettings.Append(Setting));
		}
	LOG("CDPOFPrintingDevice::CreatePrintJobL capabilities copied");

	CleanupStack::PopAndDestroy(); // CapabilityIDs

	iPrintJob->iState = PJSCreated;
	LOG("CDPOFPrintingDevice::CreatePrintJobL end");
	return KErrNone;
	}


void CDPOFPrintingDevice::SubmitPrintJobL()
	{
	LOG("CDPOFPrintingDevice::SubmitPrintJobL begin");
	
    if( iDPOF->AutoPrintFileExists() ) 
        {
        ShowConfirmationQueryL(); 
        }
    else 
    	{
    	OverwriteExistingJobL ();    	
    	}    

		LOG("CDPOFPrintingDevice::SubmitPrintJobL end");
	    }

void CDPOFPrintingDevice::ShowConfirmationQueryL() 
    {
    LOG("CDPOFPrintingDevice::ShowConfirmationQueryL begin");
  	
  	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	TFileName resourceFileName;
	resourceFileName.Append ( KDC_RESOURCE_FILES_DIR );
	resourceFileName.Append ( KResourceFileName );
	BaflUtils::NearestLanguageFile( fs, resourceFileName );
	RResourceFile resFile;
	resFile.OpenL( fs, resourceFileName );
	CleanupClosePushL( resFile );
	HBufC8 *resBuffer = resFile.AllocReadLC( R_QTN_PRINT_DPOF_CONFIRMATION_DIALOG );
	TResourceReader reader;
	reader.SetBuffer( resBuffer );
	TPtrC ptrQuestion = reader.ReadTPtrC();
	iQuestion = ptrQuestion.AllocL();
	CleanupStack::PopAndDestroy( 3 ); // resBuffer, resFile, fs

	iPQ = CAknGlobalConfirmationQuery::NewL();
    
    iConfirmationQuery = CConfirmationQuery::NewL( *this );
    
    iConfirmationQuery->StartWaitingAnswer( );        
    
    iPQ->ShowConfirmationQueryL( iConfirmationQuery->iStatus, *iQuestion, R_AVKON_SOFTKEYS_YES_NO );

    LOG("CDPOFPrintingDevice::ShowConfirmationQueryL end");
    }


TInt CDPOFPrintingDevice::CancelPrintJob()
	{
	LOG1("CDPOFPrintingDevice::CancelPrintJob begin with job state: %d", iPrintJob->iState);
	TInt retVal(KErrNone);

	if( iPrintJob->iState == PJSSubmitted )
		{
		LOG("CDPOFPrintingDevice::CancelPrintJob calling iDPOF->Stop()");
		iDPOF->Stop();				
		iPrintJob->iState = PJSCreated;
		AddReadyRequestAndSetActive( EPrintJobDone );
		}
	else
		{
		retVal = KErrInvalidSequence;
		}

	LOG1("CDPOFPrintingDevice::CancelPrintJob end with code: %d", retVal);
	return retVal;
	}

TInt CDPOFPrintingDevice::ContinuePrintJobL()
	{
	LOG1("CDPOFPrintingDevice::ContinuePrintJobL returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CDPOFPrintingDevice::GetNumPrintPages()
	{
	LOG1("CDPOFPrintingDevice::GetNumPrintPages begin with job state: %d", iPrintJob->iState);
	if( iPrintJob->iState == PJSNone )
		{
		LOG("CDPOFPrintingDevice::GetNumPrintPages job state == PJSNone, ending with 0");
		return 0;
		}


	TInt ImagesPerPage = 1;
	TInt NumPrintPages;
	TInt NumImages = iPrintJob->iImages.Count();

	// Get Num images per page from the mapping table.
	TSetting Setting;
	FindSetting(EPrintCapabLayout, Setting);

	for (TInt j = 0; j < iMappingArray.Count(); j++)
		{
		if (iMappingArray[j].iCode == Setting.iValue)
			{
			ImagesPerPage = iMappingArray[j].iImagesPerPage;
			break;
			}
		}

	if( ImagesPerPage == 0 ) // Stickers, Uses only one image, and populates a single page
		NumPrintPages = 1;
	else
		NumPrintPages = (NumImages + ImagesPerPage - 1) / ImagesPerPage;

	LOG1("CDPOFPrintingDevice::GetNumPrintPages returns: %d", NumPrintPages);
	return NumPrintPages;
	}

TInt CDPOFPrintingDevice::GetPrintJobStatus()
	{
	LOG1("CDPOFPrintingDevice::GetPrintJobStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CDPOFPrintingDevice::GetPrinterStatus(TInt /*aDeviceID*/)
	{
	LOG1("CDPOFPrintingDevice::GetPrinterStatus returns with: %d", KErrNotSupported);
	return KErrNotSupported;
	}

TInt CDPOFPrintingDevice::GetDeviceCapabilityIDsL(TInt aDeviceID, RArray<TInt>& aCapabilityIDs)
	{
	LOG1("CDPOFPrintingDevice::GetDeviceCapabilityIDsL begins with aDeviceID: %d", aDeviceID);
	TInt err(KErrNone);
	aCapabilityIDs.Reset();
	CPrinterElement* TargetPrinter = FindDeviceL(aDeviceID);
	if( TargetPrinter )
		{
		TargetPrinter->iPrinterCapabilities->GetIDs(aCapabilityIDs);
		}
	else
		{
		err = KErrInvalidData;
		}
	LOG1("CDPOFPrintingDevice::GetDeviceCapabilityIDsL ends with err: %d", err);
	return err;
	}

TInt CDPOFPrintingDevice::GetDeviceCapabilityL(TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability)
	{
	LOG1("CDPOFPrintingDevice::GetDeviceCapabilityL begins with aDeviceID: %d", aDeviceID);
	LOG1("CDPOFPrintingDevice::GetDeviceCapabilityL begins with aCapabilityID: %d", aCapabilityID);
	TInt err(KErrNone);
	CPrinterElement* TargetPrinter = FindDeviceL(aDeviceID);
	if( TargetPrinter )
		{
		TargetPrinter->iPrinterCapabilities->GetCapability(aCapabilityID, aCapability);
		}
	else
		{
		err = KErrInvalidData;
		}
	LOG1("CDPOFPrintingDevice::GetDeviceCapabilityL ends with err: %d", err);
	return err;
	}

TInt CDPOFPrintingDevice::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
	LOG1("CDPOFPrintingDevice::GetJobSetting begins with aCapabilityID: %d", aCapabilityID);
	TInt err(KErrNone);
	if( iPrintJob->iState == PJSNone )
		{
		err = KErrInvalidSequence;
		}
	else
		{
		TSetting Setting;
		if( FindSetting(aCapabilityID, Setting) )
			{
			aValue = Setting.iValue;
			}
		else
			{
			err = KErrInvalidData;
			}
		}
	LOG1("CDPOFPrintingDevice::GetJobSetting ends with err: %d", err);
	return err;
	}

TInt CDPOFPrintingDevice::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	LOG1("CDPOFPrintingDevice::SetJobSettingL begins with aCapabilityID: %d", aCapabilityID);
	LOG1("CDPOFPrintingDevice::SetJobSettingL begins with aValue: %d", aValue);
	LOG1("CDPOFPrintingDevice::SetJobSettingL begins with aAffectedCapability: %d", aAffectedCapability);

	if( iPrintJob->iState == PJSNone )
		{
		LOG("CDPOFPrintingDevice::SetJobSettingL job state == PJSNone");
		LOG1("CDPOFPrintingDevice::SetJobSettingL ends with: %d", KErrInvalidSequence);
		return KErrInvalidSequence;
		}

	aAffectedCapability = 0;
	TSetting Setting;
	if( FindSetting(aCapabilityID, Setting) == EFalse )
		{
		LOG("CDPOFPrintingDevice::SetJobSettingL FindSetting == EFalse");
		LOG1("CDPOFPrintingDevice::SetJobSettingL ends with: %d", KErrInvalidData);
		return KErrInvalidData;
		}

	// Get the Target printer and capability
	TPrintCapability TargetCapability;
	CPrinterElement* TargetPrinter = FindDeviceL( iPrintJob->iPrinterID );
	if( TargetPrinter == NULL )
		{
		LOG("CDPOFPrintingDevice::SetJobSettingL TargetPrinter == NULL");
		LOG1("CDPOFPrintingDevice::SetJobSettingL ends with: %d", KErrInvalidData);
		return KErrInvalidData;
		}

	TargetPrinter->iPrinterCapabilities->GetCapability(aCapabilityID, TargetCapability);

	// Check Validity against the target capability ranges
	if ((TargetCapability.iType == TPrintCapability::Int) ||
		(TargetCapability.iType == TPrintCapability::Float))
		{
		if (((aValue < TargetCapability.iLow) && (TargetCapability.iLow != -1)) ||
			((aValue > TargetCapability.iHigh) && (TargetCapability.iHigh != -1)))
			{
			LOG("CDPOFPrintingDevice::SetJobSettingL capability ranges error");
			LOG1("CDPOFPrintingDevice::SetJobSettingL ends with: %d", KErrInvalidData);
			return KErrInvalidData;
			}

		}
	else // enum
		{
		TBool Valid = EFalse;
		for (TInt i = 0; i < TargetCapability.iEnumCount; i++)
			{
			if (aValue == TargetCapability.iEnumCodes[i])
				{
					Valid = ETrue;
					break;
				}
			}
		if (!Valid)
			{
			LOG("CDPOFPrintingDevice::SetJobSettingL !Valid");
			LOG1("CDPOFPrintingDevice::SetJobSettingL ends with: %d", KErrInvalidData);
			return KErrInvalidData;
			}
		}

	for (TInt i = 0; i < iPrintJob->iSettings.Count(); i++)
		{
		if (aCapabilityID == iPrintJob->iSettings[i].iCapabilityID)
			{
			iPrintJob->iSettings[i].iValue = aValue;
			break;
			}
		}

	aAffectedCapability = TargetPrinter->iPrinterCapabilities->SetCurrentValueL(aCapabilityID, aValue);
	LOG1("CDPOFPrintingDevice::SetJobSettingL ends with aAffectedCapability: %d", aAffectedCapability);
	LOG1("CDPOFPrintingDevice::SetJobSettingL ends with code: %d", KErrNone);
	return KErrNone;
	}


TInt CDPOFPrintingDevice::GetNumPreviewPages()
	{
	LOG1("CDPOFPrintingDevice::GetNumPreviewPages ends with: %d", 1);
	return 1;
	}

TInt CDPOFPrintingDevice::GetJobTemplateIconL( TInt /*aTemplateID*/, TInt& /*aFsBitmapHandle*/ )
	{
	return KErrNone;
	}


TInt CDPOFPrintingDevice::CreatePreviewImage(TInt /*aPageNumber*/)
	{
	return KErrNone;
	}


void CDPOFPrintingDevice::HandleDpofEngineEventL(const TInt aStatus, const TInt aRetCode)
	{
	LOG1("CDPOFPrintingDevice::HandleDpofEngineEventL begins with aStatus: %d", aStatus);
	LOG1("CDPOFPrintingDevice::HandleDpofEngineEventL begins with aRetCode: %d", aRetCode);

	iPrintError = aRetCode;
	iPrintErrorMsgCode = 0;

	if( (aRetCode != KErrNone) && !(aRetCode==KErrCancel && aStatus==EJobCancelled) )
		{				
		AddReadyRequestAndSetActive( EPrintError );
		}
	else
		{
		switch(aStatus)
			{
			case EDeleteDirectory:
			case ECopyImageFiles:
			case EDPOFGenerate:							
				AddReadyRequestAndSetActive( EPrintJobProgress );
				break;				
			case EJobError:								
				AddReadyRequestAndSetActive( EPrintError );
				break;				
			case EJobCompleted:
			case EJobCancelled:								
				AddReadyRequestAndSetActive( EPrintJobDone );
				break;	
			default:
				break;
			}
		}
	LOG("CDPOFPrintingDevice::HandleDpofEngineEventL end");
	}

void CDPOFPrintingDevice::DoCancel()
	{
	}

void CDPOFPrintingDevice::RunL()
	{
	LOG1("CDPOFPrintingDevice::RunL begins with iStatus.Int: %d", iStatus.Int());
	PrintSessionStatus PEStatus = (PrintSessionStatus)iStatus.Int();

	switch ( PEStatus )
		{
		case KErrNone:
			break;
		case EPrinterDiscovered:
			{
			TPrinter foundPrinter;
			foundPrinter.iDisplayName = KMmcPrinterName;
			foundPrinter.iPrinterID = KMmcPrinterId;
			foundPrinter.iProperties = TPrinter::SupportsPreview;
			foundPrinter.iProtocol = KImagePrint_PrinterProtocol_DPOF;
			foundPrinter.iVendor = TPrinter::EVendorNone;

			LOG("CDPOFPrintingDevice::RunL printer discovered, informing observer");
			if( iDiscoveryObserver )
				{
				iDiscoveryObserver->FoundDeviceL( foundPrinter );
				}
			AddReadyRequestAndSetActive( EDiscoveryDone );			
			break;
			}
		case EDiscoveryDone:
			{
			LOG("CDPOFPrintingDevice::RunL discovery done, informing observer");
			if( iDiscoveryObserver )
				{
				iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrNone, 0);
				}
			break;
			}
		case ECancelDiscovery:
			{
			LOG("CDPOFPrintingDevice::RunL discovery cancelled, informing observer");
			if( iDiscoveryObserver )
				{
				iDiscoveryObserver->DiscoveryStatusL(EDiscoveryFinished, KErrCancel, 0);
				}
			break;
			}
		case EPrintError:
			{
			LOG("CDPOFPrintingDevice::RunL print error, informing observer");
			LOG1("CDPOFPrintingDevice::RunL print error iPrintError: %d", iPrintError);
			LOG1("CDPOFPrintingDevice::RunL print error iPrintErrorMsgCode: %d", iPrintErrorMsgCode);
			if( iPrintEventObserver )
				{
				iPrintEventObserver->PrintJobErrorEvent( iPrintError, iPrintErrorMsgCode );
				}
			iPrintJob->iState = PJSCreated;
			break;
			}
		case EPrintJobProgress:
			{
			LOG("CDPOFPrintingDevice::RunL job progress, informing observer");
			if( iPrintEventObserver )
				{
				iPrintEventObserver->PrintJobProgressEvent(EActive, iDPOF->GetPrintPercentage(), ECopying);
				}			
			break;
			}
		case EPrintJobDone:
			{
			iPrintJob->iState = PJSCreated;  // go back to created so it can be re-submitted
			if( iPrintEventObserver )
				{
				LOG1("CDPOFPrintingDevice::RunL job done percentage: %d", iDPOF->GetPrintPercentage());

				if (iDPOF->WasCancelled() )
					{
					LOG("CDPOFPrintingDevice::RunL job cancelled");
					iPrintEventObserver->PrintJobProgressEvent(EDone, iDPOF->GetPrintPercentage(), ECancelling);
					}
				else
					{
					LOG("CDPOFPrintingDevice::RunL job done");
					iPrintEventObserver->PrintJobProgressEvent(EDone, 100, ENoMessage);
					}
				}
			break;
			}

		default:
			{
			LOG("CDPOFPrintingDevice::RunL default, informing observer");
			if( iPrintEventObserver )
				{
				iPrintEventObserver->PrintJobErrorEvent(KErrGeneral, 0);
				}
			iPrintJob->iState = PJSCreated;
			break;
			}
		}

	CheckReadyRequestAndSetActive();
	LOG("CDPOFPrintingDevice::RunL end");
	}


CCapabilityArray* CDPOFPrintingDevice::LoadCapabilityL(RResourceFile& aResFile, TInt aCapID)
	{
	LOG1("CDPOFPrintingDevice::LoadCapabilityL begins with capID: %d", aCapID);
	HBufC8* res = aResFile.AllocReadLC(aCapID);
	TResourceReader theReader;
	theReader.SetBuffer( res );
	CCapabilityArray *capabilities = CCapabilityArray::NewL( theReader );
	CleanupStack::PopAndDestroy(); // res
	LOG("CDPOFPrintingDevice::LoadCapabilityL end");
	return capabilities;
	}


CPrinterElement* CDPOFPrintingDevice::FindDeviceL(TInt aDeviceID)
	{
	LOG1("CDPOFPrintingDevice::FindDeviceL begins with aDeviceID: %d", aDeviceID);
	// Add this here to fix problem
	// if application is started without MMC and then user insert MMC in the phone ->cannot find MMC
	if( iDeviceList.Count() == 0 )
	    {
	    SetMMCPrinterL();
	    }
	for( TInt i = 0; i < iDeviceList.Count(); i++ )
		{
		if(iDeviceList[i]->iPrinter.iPrinterID == aDeviceID )
			{
			LOG("CDPOFPrintingDevice::FindDeviceL device found");
			return iDeviceList[i];
			}
		}
	LOG("CDPOFPrintingDevice::FindDeviceL device NOT found");
	return NULL;
	}

TBool CDPOFPrintingDevice::FindSetting(TInt aCapabilityID, TSetting& aSetting)
	{
	LOG1("CDPOFPrintingDevice::FindSetting begins with aCapabilityID: %d", aCapabilityID);
	for(TInt i = 0; i < iPrintJob->iSettings.Count(); i++)
		{
		if (aCapabilityID == iPrintJob->iSettings[i].iCapabilityID)
			{
			aSetting = iPrintJob->iSettings[i];
			LOG("CDPOFPrintingDevice::FindSetting setting found");
			return ETrue;
			}
		}
	LOG("CDPOFPrintingDevice::FindSetting setting NOT found");
	return EFalse;
	}
/*	
TInt CDPOFPrintingDevice::AddReadyRequestAndSetActive( TInt aRequest )
	{
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive aRequest: %d", aRequest);				
	TInt err = iReadyRequests.Append( aRequest );
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive err: %d", err);
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive iReadyRequests.Count(): %d", iReadyRequests.Count());
	if( !err )
		{
		TInt request = iReadyRequests[0];
		LOG1("CDPOFPrintingDevice::AddRequestAndSetActive request: %d", request);
		iReadyRequests.Remove( 0 );
		iStatus = KRequestPending;
		TRequestStatus* TempTRS = &iStatus;
		if( !IsActive() ) SetActive();
		User::RequestComplete( TempTRS, request );
		}				
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive return: %d", err);
	return err;
	}
	
TInt CDPOFPrintingDevice::CheckReadyRequestAndSetActive()
	{
	LOG("CDPOFPrintingDevice::CheckReadyRequestAndSetActive begin");
	TInt count = iReadyRequests.Count();
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive count: %d", count);
	TInt err( KErrNotFound );
	if( count )
		{	
		TInt request = iReadyRequests[0];
		LOG1("CDPOFPrintingDevice::AddRequestAndSetActive request: %d", request);
		iReadyRequests.Remove( 0 );
		iStatus = KRequestPending;
		TRequestStatus* TempTRS = &iStatus;
		if( !IsActive() ) SetActive();
		User::RequestComplete( TempTRS, request );
		err = KErrNone;
		}			
	LOG1("CDPOFPrintingDevice::CheckReadyRequestAndSetActive return: %d", err);
	return err;
	}
*/

TInt CDPOFPrintingDevice::AddReadyRequestAndSetActive( TInt aRequest )
	{
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive aRequest: %d", aRequest);				
	TInt err = iReadyRequests.Append( aRequest );
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive err: %d", err);
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive iReadyRequests.Count(): %d", iReadyRequests.Count());
	if( !err )
		{
		LOG1("CDPOFPrintingDevice::AddRequestAndSetActive IsActive(): %d", IsActive());
		if( !IsActive() )
			{
			TInt request = iReadyRequests[0];
			LOG1("CDPOFPrintingDevice::AddRequestAndSetActive request: %d", request);
			iReadyRequests.Remove( 0 );
			iStatus = KRequestPending;
			TRequestStatus* TempTRS = &iStatus;
			SetActive();
			User::RequestComplete( TempTRS, request );
			}				
		}				
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive return: %d", err);
	return err;
	}
	
TInt CDPOFPrintingDevice::CheckReadyRequestAndSetActive()
	{
	LOG("CDPOFPrintingDevice::CheckReadyRequestAndSetActive begin");
	TInt count = iReadyRequests.Count();
	LOG1("CDPOFPrintingDevice::AddRequestAndSetActive count: %d", count);
	TInt err( KErrNotFound );
	if( count )
		{
		LOG1("CDPOFPrintingDevice::AddRequestAndSetActive IsActive(): %d", IsActive());
		if( !IsActive() )
			{
			TInt request = iReadyRequests[0];
			LOG1("CDPOFPrintingDevice::AddRequestAndSetActive request: %d", request);
			iReadyRequests.Remove( 0 );
			iStatus = KRequestPending;
			TRequestStatus* TempTRS = &iStatus;
			SetActive();
			User::RequestComplete( TempTRS, request );
			err = KErrNone;
			}							
		}			
	LOG1("CDPOFPrintingDevice::CheckReadyRequestAndSetActive return: %d", err);
	return err;
	}	

void CDPOFPrintingDevice::ConfirmationQueryCompleteL ( TInt aError )
		{	
			TBool answer = ( aError == EAknSoftkeyYes );	
		    if( answer ) 
		        {
		        iCancelledByUser = EFalse;         // user want to write file
		        OverwriteExistingJobL ();
		        }
		    else
		        {
		        iCancelledByUser = ETrue;
		        }
		    
	    if( iDPOF->AutoPrintFileExists() ) 
	        {
	        if( iCancelledByUser ) 
	            {
	   	   	    iPrintEventObserver->PrintJobProgressEvent( EDone, 100, ECancellingNoMessage );
	   	        LOG("CDPOFPrintingDevice::SubmitPrintJobL end");
	   	        return;
	            }
	        } 
		}

void CDPOFPrintingDevice::OverwriteExistingJobL ()
	{
	iPrintError = KErrNone;
	iPrintErrorMsgCode = 0;
	
	CRsDpofEngine::TRsDpofImageLayout layout;
	iDPOF->SetImageFileNamesL(iPrintJob->iImages);
	TSetting layoutSetting;
	FindSetting(EPrintCapabLayout, layoutSetting);
	LOG1("CDPOFPrintingDevice::OverwriteExistingJobL layoutSetting.iValue: %d", layoutSetting.iValue);
	switch(layoutSetting.iValue)
		{
		case EPrintCapabLayout1UpSmall:
			layout = CRsDpofEngine::EOnePerPageSmall;
			break;
		case EPrintCapabLayout1UpMedium:
		case EPrintCapabLayout1UpLarge:
			layout = CRsDpofEngine::EOnePerPageLarge;
			break;
		case EPrintCapabLayout2Up:
			layout = CRsDpofEngine::ETwoPerPage;
			break;
		case EPrintCapabLayout4Up:
			layout = CRsDpofEngine::EFourPerPage;
			break;
		case EPrintCapabLayout6Up:
			layout = CRsDpofEngine::ESixPerPage;
			break;
		case EPrintCapabLayout9Up:
			layout = CRsDpofEngine::ENinePerPage;
			break;
		case EPrintCapabLayout12Up:
			layout = CRsDpofEngine::ETwelvePerPage;
			break;
		case EPrintCapabLayout16Up:
			layout = CRsDpofEngine::ESixteenPerPage;
			break;
		default:
			layout = CRsDpofEngine::EOnePerPageMedium;
			break;
		}
	
	iDPOF->SetImageLayout(layout);
	LOG1("CDPOFPrintingDevice::OverwriteExistingJobL 1 iStatus.Int(): %d", iStatus.Int());
	TInt err = iDPOF->PrintL();
	LOG1("CDPOFPrintingDevice::OverwriteExistingJobL iDPOF->PrintL() error: %d", err);
	LOG1("CDPOFPrintingDevice::OverwriteExistingJobL 2 iStatus.Int(): %d", iStatus.Int());
	
	if( err )
		{
		iPrintError = err;
		iPrintErrorMsgCode = ENoMessage;				
		AddReadyRequestAndSetActive( EPrintError );
		}
	else
		{
		LOG("CDPOFPrintingDevice::OverwriteExistingJobL job submitted");
		iPrintJob->iState = PJSSubmitted;
		if(!IsActive()) SetActive();
		}
	}

		
void CDPOFPrintingDevice::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
	{
	LOG("CDPOFPrintingDevice::SetNumsOfCopiesL begin");
	LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL iPrintJob->iState: %d", iPrintJob->iState);
	TInt err( KErrNone );
	if( iPrintJob->iState != PJSCreated )
		{
		err = KErrInvalidSequence;
		}
	if( !err )
		{
		LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL iPrintJob->iImages.Count(): %d", iPrintJob->iImages.Count());
		LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL aNumsOfCopies.Count(): %d", aNumsOfCopies.Count());	
		if( iPrintJob->iImages.Count() != aNumsOfCopies.Count() )
			{
			err = KErrInvalidData;
			}
		}
	if( !err )
		{
		TInt count = aNumsOfCopies.Count();
		for( TInt i = 0; i < count && !err; i++ )
			{
			LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL i: %d", i);
			LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL aNumsOfCopies[i]: %d", aNumsOfCopies[i]);
			if( aNumsOfCopies[i] <= 0 )
				{
				err = KErrInvalidData;
				}
			}					
		}
	if( !err )
		{
		iDPOF->SetNumsOfCopiesL( aNumsOfCopies );
		}	
	aErr = err;
	LOG1("CDPOFPrintingDevice::SetNumsOfCopiesL aErr: %d", aErr);
	}

//  End of File
