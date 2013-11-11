/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  Contains the CDirectPrintBody class definition.
*
*/


#include <bluetooth/hci/hcierrors.h>

#include "directprintbody.h"
#include "directprintrsimageparser.h"
#include "clog.h"
#include "directprintselectitem.h"
#include "directprintmessage.h"

CDirectPrintBody* CDirectPrintBody::NewL()
    {
	CDirectPrintBody* self = new (ELeave) CDirectPrintBody();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

CDirectPrintBody::~CDirectPrintBody()
	{
	LOG("CDirectPrintBody::~CDirectPrintBody begin");			
	delete iClassLoader;
	iImageFileNames.ResetAndDestroy();
	delete iIdleDiscovery;
	LOG("CDirectPrintBody::~CDirectPrintBody end");
	}

TInt CDirectPrintBody::StartDiscovery(MPrinterDiscoveryObserver& aObserver, TUint aProtocols)
	{
	LOG1("CDirectPrintBody::StartDiscovery aProtocols: %d", aProtocols);
	iPrinterDiscoveryObserver = &aObserver;

	if( iCurrentProtocol )
		{
		iCurrentProtocol->CancelDiscovery( EFalse );
		}

	iCurrentProtocol = NULL;
	iClassLoader->Reset();
	iProtocolsRequested = aProtocols;
	iDiscoveryCancelled = EFalse;
	TryNextDiscovery();		
	
	LOG("CDirectPrintBody::StartDiscovery end");
	return KErrNone;
	}

void CDirectPrintBody::CancelDiscovery()
	{
	LOG("CDirectPrintBody::CancelDiscovery begin");
	iDiscoveryCancelled = ETrue;
	if( iCurrentProtocol )
		{
		iCurrentProtocol->CancelDiscovery();
		}
	LOG("CDirectPrintBody::CancelDiscovery end");
	}


TInt CDirectPrintBody::CreatePrintJobL(TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver)
	{
	LOG("CDirectPrintBody::CreatePrintJobL begin");
	iPrinterPrintEventObserver = &aObserver;
	iCurrentPrintJobProtocol = NULL;

	if( iCurrentProtocol )
		{		
		iCurrentProtocol->CancelDiscovery( EFalse );
		}

#ifdef __IMAGE_PRINT_BASE
	TInt DeviceID;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, iCurrentPrintJobProtocol );
	if( !err )
		{
		TInt count = aImages.Count();
		if( count )
			{
			iImageFileNames.ResetAndDestroy();
			for( TInt i = 0; i < count; ++i )
				{
				HBufC* fileName = aImages[i]->AllocLC();
				iImageFileNames.AppendL( fileName );
				CleanupStack::Pop(); // fileName
				}
							
			ActiveScheduler().SetCurrentProtocol( iCurrentPrintJobProtocol->SupportedProtocols() );
			err = iCurrentPrintJobProtocol->CreatePrintJobL( DeviceID, aImages, *this );	
			}
		else
			{
			err = KErrInvalidData;
			}
		}
#endif // __IMAGE_PRINT_BASE

	TInt DeviceID;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, iCurrentPrintJobProtocol );
	if( !err )
		{
		ActiveScheduler().SetCurrentProtocol( iCurrentPrintJobProtocol->SupportedProtocols() );
		err = iCurrentPrintJobProtocol->CreatePrintJobL( DeviceID, aImages, *this );
		}

	LOG1("CDirectPrintBody::CreatePrintJobL return: %d", err);
	return err;
	}


void CDirectPrintBody::SubmitPrintJobL()
	{
	LOG1("CDirectPrintBody::SubmitPrintJobL iCurrentPrintJobProtocol: %d", iCurrentPrintJobProtocol);
	TInt err( KErrNone );

	if( !iCurrentPrintJobProtocol )
		{
		err = KErrInvalidSequence;
		}
	if( !err )
		{
		TInt invalidImages;
		err = ValidateImagesL( iImageFileNames, invalidImages );
		LOG1("CDirectPrintBody::SubmitPrintJobL ValidateImagesL returns: %d", err);
		}

	User::LeaveIfError( err );

	iCurrentPrintJobProtocol->SubmitPrintJobL();

	LOG("CDirectPrintBody::SubmitPrintJobL end");
	}


TInt CDirectPrintBody::CancelPrintJob()
	{
	LOG("CDirectPrintBody::CancelPrintJob begin");
	TInt err( KErrNone );
	if( !iCurrentPrintJobProtocol )
		{
		err = KErrInvalidSequence;
		}
	if( !err )
		{
		err = iCurrentPrintJobProtocol->CancelPrintJob();
		}
	LOG1("CDirectPrintBody::CancelPrintJob return: %d", err);
	return err;
	}


TInt CDirectPrintBody::ContinuePrintJobL()
	{
	LOG("CDirectPrintBody::ContinuePrintJobL begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->ContinuePrintJobL();
		}
	LOG1("CDirectPrintBody::ContinuePrintJobL return: %d", err);
	return err;
	}


TInt CDirectPrintBody::GetNumPrintPages()
	{
	LOG("CDirectPrintBody::GetNumPrintPages begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetNumPrintPages();
		}
	LOG1("CDirectPrintBody::GetNumPrintPages return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetJobStatus()
	{
	LOG("CDirectPrintBody::GetJobStatus begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetPrintJobStatus();
		}
	LOG1("CDirectPrintBody::GetJobStatus return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetPrinterStatus(TInt aPrinterID)
	{
	LOG1("CDirectPrintBody::GetPrinterStatus aPrinterID: %d", aPrinterID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->GetPrinterStatus( DeviceID );
		}
	LOG1("CDirectPrintBody::GetPrinterStatus return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetPrinterCapabilityIDsL(TInt aPrinterID, RArray<TInt>& aCapabilityIDs)
	{
	LOG1("CDirectPrintBody::GetPrinterCapabilityIDsL aPrinterID: %d", aPrinterID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;

	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->GetDeviceCapabilityIDsL( DeviceID, aCapabilityIDs );
		}
	else if( err == KErrNotFound )
		{
		err = KErrInvalidData;
		}

	LOG1("CDirectPrintBody::GetPrinterCapabilityIDsL return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetPrinterCapabilityL(TInt aPrinterID, TInt aCapabilityID, TDirectPrintCapability& aCapability)
	{
	LOG1("CDirectPrintBody::GetPrinterCapabilityL aPrinterID: %d", aPrinterID);
	LOG1("CDirectPrintBody::GetPrinterCapabilityL aCapabilityID: %d", aCapabilityID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;
	
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->GetDeviceCapabilityL( DeviceID, aCapabilityID, aCapability );
		}
	else if( err == KErrNotFound )
		{
		err = KErrInvalidData;
		}
	
	LOG1("CDirectPrintBody::GetPrinterCapabilityL return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
	LOG1("CDirectPrintBody::GetJobSetting aCapabilityID: %d", aCapabilityID);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetJobSetting( aCapabilityID, aValue );
		}
	LOG1("CDirectPrintBody::GetJobSetting return: %d", err);
	return err;
	}

TInt CDirectPrintBody::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	LOG1("CDirectPrintBody::SetJobSettingL aCapabilityID: %d", aCapabilityID);
	LOG1("CDirectPrintBody::SetJobSettingL aValue: %d", aValue);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->SetJobSettingL( aCapabilityID, aValue, aAffectedCapability );
		}
	LOG1("CDirectPrintBody::SetJobSettingL return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetNumPreviewPages()
	{
	LOG("CDirectPrintBody::GetNumPreviewPages");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetNumPreviewPages();
		}
	LOG1("CDirectPrintBody::GetNumPreviewPages return: %d", err);
	return err;
	}

TInt CDirectPrintBody::GetJobTemplateIconL( TInt aTemplateID, TInt& aFbsBitmapHandle )
	{
	LOG1("CDirectPrintBody::GetJobTemplateIconL aTemplateID: %d", aTemplateID);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetJobTemplateIconL( aTemplateID, aFbsBitmapHandle );
		}
	LOG1("CDirectPrintBody::GetJobTemplateIconL return: %d", err);
	return err;
	}

TInt CDirectPrintBody::CreatePreviewImage(TInt aPageNumber)
	{
	LOG1("CDirectPrintBody::CreatePreviewImage aPageNumber: %d", aPageNumber);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->CreatePreviewImage(aPageNumber);
		}
	LOG1("CDirectPrintBody::CreatePreviewImage return: %d", err);
	return err;
	}

TInt CDirectPrintBody::RemoveCachedPrinterL(TInt aPrinterID)
	{
	LOG1("CDirectPrintBody::RemoveCachedPrinterL aPrinterID: %d", aPrinterID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->RemoveCachedDeviceL( DeviceID );
		}
	LOG1("CDirectPrintBody::RemoveCachedPrinterL return: %d", err);
	return err;
	}

TUint CDirectPrintBody::SupportedProtocols()
	{
	LOG("CDirectPrintBody::SupportedProtocols begin");
	TUint prots = iClassLoader->SupportedProtocols();
	LOG1("CDirectPrintBody::SupportedProtocols return: %d", prots);
	return prots;
	}

void CDirectPrintBody::RegisterIdleObserver(MIdleObserver *aObserver)
	{
	LOG("CDirectPrintBody::RegisterIdleObserver begin");
	iPrinterIdleObserver = aObserver;
	LOG("CDirectPrintBody::RegisterIdleObserver end");
	}

// From MProtDiscoveryObserver.
void CDirectPrintBody::FoundDeviceL(TPrinter& aDeviceInfo)
	{
	LOG("CDirectPrintBody::FoundDeviceL begin");
	if( iPrinterDiscoveryObserver )
		{	
		//Check that device id is valid, there can't be any bits in the protocol field (higher bits)
		if( !( PROTOCOL_ID_FIELD_MASK & aDeviceInfo.iPrinterID ) )
			{
			// Get the protocol index number
			TInt protocolIndex = iClassLoader->GetProtocolIndex();

			// Left shift this number so that it occupies the higher X bits (the PROTOCOL_ID_BIT_COUNT highest bits)
			protocolIndex <<= sizeof(TInt)*8 - PROTOCOL_ID_BIT_COUNT;

			// Add the protocol index number to the device id
			aDeviceInfo.iPrinterID |= protocolIndex;

			// Forward the FoundPrinterL callback to the observer
			iPrinterDiscoveryObserver->FoundPrinterL( aDeviceInfo );
			}
		}
	LOG("CDirectPrintBody::FoundDeviceL end");	
	}

void CDirectPrintBody::DiscoveryStatusL( TInt aStatus, TInt aErrorCode, TInt aErrorStringCode )
	{
	LOG1("CDirectPrintBody::DiscoveryStatusL aStatus: %d", aStatus);
	LOG1("CDirectPrintBody::DiscoveryStatusL aErrorCode: %d", aErrorCode);
	LOG1("CDirectPrintBody::DiscoveryStatusL aErrorStringCode: %d", aErrorStringCode);
	if( iPrinterDiscoveryObserver )
		{
		switch( aStatus )
			{
			case EDiscoveryFinished: // Done with 1 protocol.
			{
				// When Bluetooth discovery is finished, but Bluetooth is not enabled pass error code to UI. 
				if ( aErrorCode == ENoConnection )
					{
					iPrinterDiscoveryObserver->DiscoveryStatusL( aStatus, aErrorCode, aErrorStringCode );
					}
				iCurrentProtocol = NULL;
				TUint doDiscovery = iProtocolsRequested & iClassLoader->SupportedProtocols();
				LOG1("CDirectPrintBody::DiscoveryStatusL doDiscovery: %d", doDiscovery);
				LOG1("CDirectPrintBody::DiscoveryStatusL iDiscoveryCancelled: %d", iDiscoveryCancelled);
				if( ( !doDiscovery && aErrorCode == KErrCancel ) || iDiscoveryCancelled )
					{
					iPrinterDiscoveryObserver->DiscoveryStatusL( aStatus, aErrorCode, aErrorStringCode );
					}
				else
					{
					TryNextDiscovery();
					}
			}
				break;
			default: // Discovering, Cancelling.
				iCurrentProtocol = NULL;
				iPrinterDiscoveryObserver->DiscoveryStatusL( aStatus, aErrorCode, aErrorStringCode );
			break;
			}
		}
	LOG("CDirectPrintBody::DiscoveryStatusL end");
	}


void CDirectPrintBody::RemoveDeviceL(TPrinter& aDeviceInfo)
	{	
	LOG("CDirectPrintBody::RemoveDeviceL begin");
	if( iPrinterDiscoveryObserver )
		{
		if (PROTOCOL_ID_FIELD_MASK & aDeviceInfo.iPrinterID)
			{	//Invalid id, reject printer
			}
		else
			{	// Get the protocol index number
			TInt protocolIndex = iClassLoader->GetProtocolIndex();
			
			// Left shift this number so that it occupies the higher X bits (the PROTOCOL_ID_BIT_COUNT highest bits)
			protocolIndex <<= sizeof(TInt)*8 - PROTOCOL_ID_BIT_COUNT;

			// Add the protocol index number to the device id
			aDeviceInfo.iPrinterID |= protocolIndex;

			// Forward the RemovePrinterL callback to the observer
			iPrinterDiscoveryObserver->RemovePrinterL( aDeviceInfo );
			}
		}
	LOG("CDirectPrintBody::RemoveDeviceL end");	
	}

// From MProtPrintEventObserver.
void CDirectPrintBody::PrintJobProgressEvent(TInt aStatus, TInt aPercent, TInt aJobStateCode)
	{
	LOG1("CDirectPrintBody::PrintJobProgressEvent aStatus: %d", aStatus);
	LOG1("CDirectPrintBody::PrintJobProgressEvent aPercent: %d", aPercent);
	LOG1("CDirectPrintBody::PrintJobProgressEvent aJobStateCode: %d", aJobStateCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrintJobProgressEvent(aStatus, aPercent, aJobStateCode);
	LOG("CDirectPrintBody::PrintJobProgressEvent end");
	}

void CDirectPrintBody::PrintJobErrorEvent(TInt aErrorCode, TInt aErrorStringCode)
	{
	LOG1("CDirectPrintBody::PrintJobErrorEvent aErrorCode: %d", aErrorCode);
	LOG1("CDirectPrintBody::PrintJobErrorEvent aErrorStringCode: %d", aErrorStringCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrintJobErrorEvent(aErrorCode, aErrorStringCode);
	LOG("CDirectPrintBody::PrintJobErrorEvent end");
	}

void CDirectPrintBody::PrinterStatusEvent(TInt aErrorCode, TInt aErrorStringCode)
	{
	LOG1("CDirectPrintBody::PrinterStatusEvent aErrorCode: %d", aErrorCode);
	LOG1("CDirectPrintBody::PrinterStatusEvent aErrorStringCode: %d", aErrorStringCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrinterStatusEvent(aErrorCode, aErrorStringCode);
	LOG("CDirectPrintBody::PrinterStatusEvent end");
	}

void CDirectPrintBody::PreviewImageEvent(TInt aFsBitmapHandle)
	{
	LOG1("CDirectPrintBody::PreviewImageEvent aFsBitmapHandle: %d", aFsBitmapHandle);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PreviewImageEvent(aFsBitmapHandle);
	LOG("CDirectPrintBody::PreviewImageEvent end");
	}

void CDirectPrintBody::ShowMessageL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CDirectPrintBody::ShowMessageL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CDirectPrintBody::ShowMessageL aMsgLine2Code: %d", aMsgLine2Code);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->ShowMessageL(aMsgLine1Code, aMsgLine2Code);
	LOG("CDirectPrintBody::ShowMessageL end");
	}

TBool CDirectPrintBody::AskYesNoQuestionL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CDirectPrintBody::AskYesNoQuestionL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CDirectPrintBody::AskYesNoQuestionL aMsgLine2Code: %d", aMsgLine2Code);
	TBool res( EFalse );
	if( iPrinterPrintEventObserver )
		{
		res = iPrinterPrintEventObserver->AskYesNoQuestionL(aMsgLine1Code, aMsgLine2Code);
		}
	LOG1("CDirectPrintBody::AskYesNoQuestionL return: %d", res);
	return res;
	}

const TDesC& CDirectPrintBody::AskForInputL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CDirectPrintBody::ShowMessageL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CDirectPrintBody::ShowMessageL aMsgLine2Code: %d", aMsgLine2Code);
	if( iPrinterPrintEventObserver )
		{
		return iPrinterPrintEventObserver->AskForInputL(aMsgLine1Code, aMsgLine2Code);
		}
	else
		{
		return KNullDesC;
		}
	}

void CDirectPrintBody::StatusEvent(const TEvent& aEvent, TInt aError, TInt aMsgCode)
	{
	LOG1("CDirectPrintBody::StatusEvent aError: %d", aError);
	LOG1("CDirectPrintBody::StatusEvent aMsgCode: %d", aMsgCode);
	LOG1("CDirectPrintBody::StatusEvent aEvent.iProtocol: %d", aEvent.iProtocol);
	LOG1("CDirectPrintBody::StatusEvent aEvent.iSeverity: %d", aEvent.iSeverity);
	LOG1("CDirectPrintBody::StatusEvent aEvent.iEventType: %d", aEvent.iEventType);
	
	if( iPrinterIdleObserver )
		{
		iPrinterIdleObserver->StatusEvent( aEvent, aError, aMsgCode );
		}
	
	if( aEvent.iEventType == EAsynchronousLeave )
		{
		// a protocol threw an async leave
		// we continue with other protocols
		TUint doDiscovery = iProtocolsRequested & iClassLoader->SupportedProtocols();
		if( doDiscovery ) TryNextDiscovery();
		}
	LOG("CDirectPrintBody::StatusEvent end");
	}

CDirectPrintBody::CDirectPrintBody()
	: iCurrentProtocol( NULL )
	, iCurrentPrintJobProtocol( NULL )
	{
	LOG("CDirectPrintBody::CDirectPrintBody begin");
	LOG("CDirectPrintBody::CDirectPrintBody end");
	}

void CDirectPrintBody::ConstructL()
	{
	LOG("CDirectPrintBody::ConstructL begin");	
	iClassLoader = CDirectPrintProtocolsLoader::NewL();
	iClassLoader->Reset();
	for( TInt x=0; x < iClassLoader->GetNumberOfProtocolsAvailable(); ++x )
		{
		iClassLoader->GetNextProtocol()->RegisterIdleObserver( this );
		}

	iIdleDiscovery = CIdle::NewL( CActive::EPriorityIdle );
	ActiveScheduler().SetIdleObserver( *this );
	LOG("CDirectPrintBody::ConstructL end");
	}

CDirectPrintScheduler& CDirectPrintBody::ActiveScheduler() const
	{
	LOG("CDirectPrintBody::ActiveScheduler begin");
	CDirectPrintScheduler* scheduler = static_cast<CDirectPrintScheduler*>( CActiveScheduler::Current() );
	LOG("CDirectPrintBody::ActiveScheduler end");
	return *scheduler;
	}

TInt CDirectPrintBody::FindInternalDeviceReference(TInt aExternalDeviceID, TInt& aInternalDeviceID, MProtPrintingDevice*& aProtToUse)
	{
	LOG1("CDirectPrintBody::FindInternalDeviceReference aExternalDeviceID: %d", aExternalDeviceID);
	TInt err( KErrNotFound );

#ifdef __IMAGE_PRINT_BASE
	aProtToUse = NULL;

	// Separate the external id into the corresponding fields
	TInt protId = aExternalDeviceID & PROTOCOL_ID_FIELD_MASK;

	TInt deviceId = aExternalDeviceID & DEVICE_ID_FIELD_MASK;
	protId >>= (sizeof(TInt)*8 - PROTOCOL_ID_BIT_COUNT);
	// If TInt is more than 4 bytes the mask should clear everything but the 6 least significant bits
	protId &= PROTOCOL_ID_CLEAR_MASK;	//remove 1's if the original protId was negative

	// Check that the external ID does indeed belong to one of the currently
	// loaded protocols
	if( protId >= 0 && protId < iClassLoader->GetNumberOfProtocolsAvailable() )
		{
		aInternalDeviceID = deviceId;
		aProtToUse = iClassLoader->GetProtocolAt(protId);
		if( aProtToUse ) err = KErrNone;
		}
#endif // __IMAGE_PRINT_BASE
#if 1 // ko
	TInt protId = aExternalDeviceID & PROTOCOL_ID_FIELD_MASK;
	protId >>= (sizeof(TInt)*8 - PROTOCOL_ID_BIT_COUNT);
	protId &= PROTOCOL_ID_CLEAR_MASK;
	if( protId >= 0 && protId < iClassLoader->GetNumberOfProtocolsAvailable() )
		{
		aInternalDeviceID = 128;
		aProtToUse = iClassLoader->GetProtocolAt(protId);
		if( aProtToUse ) err = KErrNone;
		}
#else // ko
	aInternalDeviceID = 128;
	aProtToUse = iClassLoader->GetProtocolAt(0);
	if( aProtToUse ) err = KErrNone;
#endif // ko
	LOG1("CDirectPrintBody::FindInternalDeviceReference return: %d", err);
	return err;
	}

TInt CDirectPrintBody::ValidateImagesL(const RPointerArray<HBufC>& aImageList, TInt &aInvalidCount)
	{
	LOG1("CDirectPrintBody::ValidateImagesL aImageList.Count(): %d", aImageList.Count());
	aInvalidCount = 0;
	TInt lastError = KErrNone;
	CDirectPrintRsImageParser *imageParser = CDirectPrintRsImageParser::NewLC();

	for( TInt nCnt=0; nCnt < aImageList.Count(); ++nCnt )
		{
		LOG1("CDirectPrintBody::ValidateImagesL nCnt: %d", nCnt);
		LOG1("CDirectPrintBody::ValidateImagesL image: %S", aImageList[nCnt]);
		TBool valid;
		imageParser->ValidateL(*(aImageList[nCnt]), valid, lastError);
		LOG1("CDirectPrintBody::ValidateImagesL valid: %d", valid);
		LOG1("CDirectPrintBody::ValidateImagesL lastError: %d", lastError);
		if( !valid )
			{
			++aInvalidCount;
			}
		}

	CleanupStack::PopAndDestroy(imageParser);
	LOG1("CDirectPrintBody::ValidateImagesL aInvalidCount: %d", aInvalidCount);
	LOG1("CDirectPrintBody::ValidateImagesL ends with: %d", lastError);
	return lastError;
	}

void CDirectPrintBody::TryNextDiscovery()
	{
	LOG("CDirectPrintBody::TryNextDiscovery begin");
	iIdleDiscovery->Cancel();
	iIdleDiscovery->Start(TCallBack(TryNextDiscoveryL, this));
	LOG("CDirectPrintBody::TryNextDiscovery end");
	}

void CDirectPrintBody::DoTryNextDiscoveryL()
	{
	LOG("CDirectPrintBody::DoTryNextDiscoveryL begin");
	TUint doDiscovery = iProtocolsRequested & iClassLoader->SupportedProtocols();
	LOG1("CDirectPrintBody::DoTryNextDiscoveryL doDiscovery: %d", doDiscovery);
	if( doDiscovery )
		{
		TBool found( EFalse );
		TInt protId( 0 );
		TInt count = iClassLoader->GetNumberOfProtocolsAvailable();
		LOG1("CDirectPrintBody::DoTryNextDiscoveryL count: %d", count);
		for( TInt i = 0; i < count && !found; i++ )
			{
			iCurrentProtocol = iClassLoader->GetNextProtocol();
			protId = iCurrentProtocol->SupportedProtocols();
			if( iProtocolsRequested & protId ) found = ETrue;
			}
		LOG1("CDirectPrintBody::DoTryNextDiscoveryL found: %d", found);
		LOG1("CDirectPrintBody::DoTryNextDiscoveryL protId: %d", protId);
		if( found )
			{
			TUint temp = KMaxTUint ^ protId;
			iProtocolsRequested &= temp;
			LOG1("CDirectPrintBody::DoTryNextDiscoveryL iProtocolsRequested: %d", iProtocolsRequested);
			ActiveScheduler().SetCurrentProtocol( protId );
			iCurrentProtocol->StartDiscoveryL( *this, protId );
			}
		}
	else
		{
		iPrinterDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, 0, 0 );
		}
	LOG("CDirectPrintBody::DoTryNextDiscoveryL end");
	}

TInt CDirectPrintBody::TryNextDiscoveryL(TAny* aObj)
	{
	LOG("CDirectPrintBody::TryNextDiscoveryL begin");
	CDirectPrintBody* obj = static_cast<CDirectPrintBody*>(aObj);
	obj->DoTryNextDiscoveryL();
	LOG("CDirectPrintBody::TryNextDiscoveryL end");
	return EFalse;
	}

void CDirectPrintBody::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
	{
	LOG1("CDirectPrintBody::SetNumberOfCopiesL aNumsOfCopies.Count(): %d", aNumsOfCopies.Count());

	if( iCurrentPrintJobProtocol )
		{
		iCurrentPrintJobProtocol->SetNumsOfCopiesL( aNumsOfCopies, aErr );
		LOG1("CDirectPrintBody::SetNumberOfCopiesL aErr: %d", aErr);
		}

	LOG("CDirectPrintBody::SetNumberOfCopiesL end");
	}

void CDirectPrintBody::HandleSessionDisconnect( const MIdleObserver* aIdleObserver,
												const MPrinterDiscoveryObserver* aDiscoveryObserver,
												const MPrintEventObserver* aEventObserver )
	{
	LOG1("CDirectPrintBody::HandleSessionDisconnect aIdleObserver address: %d", aIdleObserver);
	LOG1("CDirectPrintBody::HandleSessionDisconnect iPrinterIdleObserver address: %d", iPrinterIdleObserver);
	LOG1("CDirectPrintBody::HandleSessionDisconnect aDiscoveryObserver address: %d", aDiscoveryObserver);
	LOG1("CDirectPrintBody::HandleSessionDisconnect iPrinterDiscoveryObserver address: %d", iPrinterDiscoveryObserver);
	LOG1("CDirectPrintBody::HandleSessionDisconnect aEventObserver address: %d", aEventObserver);
	LOG1("CDirectPrintBody::HandleSessionDisconnect iPrinterPrintEventObserver address: %d", iPrinterPrintEventObserver);
	if( aIdleObserver == iPrinterIdleObserver )	iPrinterIdleObserver = NULL;
	if( aEventObserver == iPrinterPrintEventObserver ) iPrinterPrintEventObserver = NULL;
	if( aDiscoveryObserver == iPrinterDiscoveryObserver ) iPrinterDiscoveryObserver = NULL;
	LOG("CDirectPrintBody::HandleSessionDisconnect end");
	}

TInt CDirectPrintBody::ProtocolCount()
	{
	return iClassLoader->GetNumberOfProtocolsAvailable();
	}

TInt CDirectPrintBody::GetProtocolNameL( TDpMessage& aMessage )
	{
	TDirectPrintSelectItem item;
	TInt index = aMessage.Int0();
	TInt err = iClassLoader->GetProtocolName(index, item.iName, item.iId);

	if (err == KErrNone)
		{
		TPtr8 ptr(reinterpret_cast<TUint8*>(&item), sizeof(item), sizeof(item));
		aMessage.WriteL( 1, ptr );
		}

	return err;
	}

//  End of File
