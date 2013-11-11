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
* Description:  Contains the CImagePrintBody class definition.
*
*/


#include <bluetooth/hci/hcierrors.h>

#include "cimageprintbody.h"
#include "crsimageparser.h"
#include "clog.h"

CImagePrintBody* CImagePrintBody::NewL()
    {
	CImagePrintBody* self = new (ELeave) CImagePrintBody();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

CImagePrintBody::~CImagePrintBody()
	{
	LOG("CImagePrintBody::~CImagePrintBody begin");			
	delete iClassLoader;
	iImageFileNames.ResetAndDestroy();
	delete iIdleDiscovery;
	LOG("CImagePrintBody::~CImagePrintBody end");
	}

TInt CImagePrintBody::StartDiscovery(MPrinterDiscoveryObserver& aObserver, TUint aProtocols)
	{
	LOG1("CImagePrintBody::StartDiscovery aProtocols: %d", aProtocols);
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
	
	LOG("CImagePrintBody::StartDiscovery end");
	return KErrNone;
	}

void CImagePrintBody::CancelDiscovery()
	{
	LOG("CImagePrintBody::CancelDiscovery begin");
	iDiscoveryCancelled = ETrue;
	if( iCurrentProtocol )
		{
		iCurrentProtocol->CancelDiscovery();
		}
	LOG("CImagePrintBody::CancelDiscovery end");		
	}


TInt CImagePrintBody::CreatePrintJobL(TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver)
	{
	LOG("CImagePrintBody::CreatePrintJobL begin");		
	iPrinterPrintEventObserver = &aObserver;
	iCurrentPrintJobProtocol = NULL;

	if( iCurrentProtocol )
		{		
		iCurrentProtocol->CancelDiscovery( EFalse );
		}

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
	
	LOG1("CImagePrintBody::CreatePrintJobL return: %d", err);	
	return err;
	}


void CImagePrintBody::SubmitPrintJobL()
	{	
	LOG1("CImagePrintBody::SubmitPrintJobL iCurrentPrintJobProtocol: %d", iCurrentPrintJobProtocol);
	TInt err( KErrNone );
			
	if( !iCurrentPrintJobProtocol )
		{
		err = KErrInvalidSequence;		
		}		
	if( !err )		
		{
		TInt invalidImages;
		err = ValidateImagesL( iImageFileNames, invalidImages );
		LOG1("CImagePrintBody::SubmitPrintJobL ValidateImagesL returns: %d", err);	
		}
		
	User::LeaveIfError( err );		
	
	iCurrentPrintJobProtocol->SubmitPrintJobL();
		
	LOG("CImagePrintBody::SubmitPrintJobL end");
	}


TInt CImagePrintBody::CancelPrintJob()
	{
	LOG("CImagePrintBody::CancelPrintJob begin");
	TInt err( KErrNone );		
	if( !iCurrentPrintJobProtocol )
		{
		err = KErrInvalidSequence;
		}
	if( !err )
		{
		err = iCurrentPrintJobProtocol->CancelPrintJob();	
		}		
	LOG1("CImagePrintBody::CancelPrintJob return: %d", err);		
	return err;
	}


TInt CImagePrintBody::ContinuePrintJobL()
	{
	LOG("CImagePrintBody::ContinuePrintJobL begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->ContinuePrintJobL();
		}	
	LOG1("CImagePrintBody::ContinuePrintJobL return: %d", err);
	return err;
	}


TInt CImagePrintBody::GetNumPrintPages()
	{
	LOG("CImagePrintBody::GetNumPrintPages begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetNumPrintPages();
		}
	LOG1("CImagePrintBody::GetNumPrintPages return: %d", err);
	return err;
	}

TInt CImagePrintBody::GetJobStatus()
	{
	LOG("CImagePrintBody::GetJobStatus begin");
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetPrintJobStatus();
		}
	LOG1("CImagePrintBody::GetJobStatus return: %d", err);
	return err;
	}

TInt CImagePrintBody::GetPrinterStatus(TInt aPrinterID)
	{
	LOG1("CImagePrintBody::GetPrinterStatus aPrinterID: %d", aPrinterID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->GetPrinterStatus( DeviceID );
		}
	LOG1("CImagePrintBody::GetPrinterStatus return: %d", err);
	return err;
	}

TInt CImagePrintBody::GetPrinterCapabilityIDsL(TInt aPrinterID, RArray<TInt>& aCapabilityIDs)
	{
	LOG1("CImagePrintBody::GetPrinterCapabilityIDsL aPrinterID: %d", aPrinterID);		
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

	LOG1("CImagePrintBody::GetPrinterCapabilityIDsL return: %d", err);
	return err;
	}

TInt CImagePrintBody::GetPrinterCapabilityL(TInt aPrinterID, TInt aCapabilityID, TPrintCapability& aCapability)
	{
	LOG1("CImagePrintBody::GetPrinterCapabilityL aPrinterID: %d", aPrinterID);
	LOG1("CImagePrintBody::GetPrinterCapabilityL aCapabilityID: %d", aCapabilityID);
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
	
	LOG1("CImagePrintBody::GetPrinterCapabilityL return: %d", err);	
	return err;
	}

TInt CImagePrintBody::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
	LOG1("CImagePrintBody::GetJobSetting aCapabilityID: %d", aCapabilityID);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetJobSetting( aCapabilityID, aValue );
		}
	LOG1("CImagePrintBody::GetJobSetting return: %d", err);
	return err;
	}

TInt CImagePrintBody::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	LOG1("CImagePrintBody::SetJobSettingL aCapabilityID: %d", aCapabilityID);
	LOG1("CImagePrintBody::SetJobSettingL aValue: %d", aValue);
	TInt err( KErrInvalidSequence );		
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->SetJobSettingL( aCapabilityID, aValue, aAffectedCapability );
		}
	LOG1("CImagePrintBody::SetJobSettingL return: %d", err);
	return err;		
	}

TInt CImagePrintBody::GetNumPreviewPages()
	{
	LOG("CImagePrintBody::GetNumPreviewPages");
	TInt err( KErrInvalidSequence );		
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetNumPreviewPages();
		}
	LOG1("CImagePrintBody::GetNumPreviewPages return: %d", err);
	return err;	
	}

TInt CImagePrintBody::GetJobTemplateIconL( TInt aTemplateID, TInt& aFbsBitmapHandle )
	{
	LOG1("CImagePrintBody::GetJobTemplateIconL aTemplateID: %d", aTemplateID);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->GetJobTemplateIconL( aTemplateID, aFbsBitmapHandle );
		}
	LOG1("CImagePrintBody::GetJobTemplateIconL return: %d", err);
	return err;
	}

TInt CImagePrintBody::CreatePreviewImage(TInt aPageNumber)
	{
	LOG1("CImagePrintBody::CreatePreviewImage aPageNumber: %d", aPageNumber);
	TInt err( KErrInvalidSequence );
	if( iCurrentPrintJobProtocol )
		{
		err = iCurrentPrintJobProtocol->CreatePreviewImage(aPageNumber);
		}
	LOG1("CImagePrintBody::CreatePreviewImage return: %d", err);
	return err;
	}

TInt CImagePrintBody::RemoveCachedPrinterL(TInt aPrinterID)
	{
	LOG1("CImagePrintBody::RemoveCachedPrinterL aPrinterID: %d", aPrinterID);
	TInt DeviceID;
	MProtPrintingDevice* theProtToQuery = NULL;
	TInt err = FindInternalDeviceReference( aPrinterID, DeviceID, theProtToQuery );
	if( !err )
		{
		err = theProtToQuery->RemoveCachedDeviceL( DeviceID );
		}
	LOG1("CImagePrintBody::RemoveCachedPrinterL return: %d", err);
	return err;
	}

TUint CImagePrintBody::SupportedProtocols()
	{
	LOG("CImagePrintBody::SupportedProtocols begin");
	TUint prots = iClassLoader->SupportedProtocols();
	LOG1("CImagePrintBody::SupportedProtocols return: %d", prots);
	return prots;
	}

void CImagePrintBody::RegisterIdleObserver(MIdleObserver *aObserver)
	{
	LOG("CImagePrintBody::RegisterIdleObserver begin");
	iPrinterIdleObserver = aObserver;
	LOG("CImagePrintBody::RegisterIdleObserver end");
	}

// From MProtDiscoveryObserver.
void CImagePrintBody::FoundDeviceL(TPrinter& aDeviceInfo)
	{
	LOG("CImagePrintBody::FoundDeviceL begin");		
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
	LOG("CImagePrintBody::FoundDeviceL end");	
	}

void CImagePrintBody::DiscoveryStatusL( TInt aStatus, TInt aErrorCode, TInt aErrorStringCode )
	{
	LOG1("CImagePrintBody::DiscoveryStatusL aStatus: %d", aStatus);
	LOG1("CImagePrintBody::DiscoveryStatusL aErrorCode: %d", aErrorCode);
	LOG1("CImagePrintBody::DiscoveryStatusL aErrorStringCode: %d", aErrorStringCode);
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
				LOG1("CImagePrintBody::DiscoveryStatusL doDiscovery: %d", doDiscovery);
				LOG1("CImagePrintBody::DiscoveryStatusL iDiscoveryCancelled: %d", iDiscoveryCancelled);
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
	LOG("CImagePrintBody::DiscoveryStatusL end");
	}


void CImagePrintBody::RemoveDeviceL(TPrinter& aDeviceInfo)
	{	
	LOG("CImagePrintBody::RemoveDeviceL begin");
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
	LOG("CImagePrintBody::RemoveDeviceL end");	
	}

// From MProtPrintEventObserver.
void CImagePrintBody::PrintJobProgressEvent(TInt aStatus, TInt aPercent, TInt aJobStateCode)
	{
	LOG1("CImagePrintBody::PrintJobProgressEvent aStatus: %d", aStatus);
	LOG1("CImagePrintBody::PrintJobProgressEvent aPercent: %d", aPercent);
	LOG1("CImagePrintBody::PrintJobProgressEvent aJobStateCode: %d", aJobStateCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrintJobProgressEvent(aStatus, aPercent, aJobStateCode);
	LOG("CImagePrintBody::PrintJobProgressEvent end");
	}

void CImagePrintBody::PrintJobErrorEvent(TInt aErrorCode, TInt aErrorStringCode)
	{
	LOG1("CImagePrintBody::PrintJobErrorEvent aErrorCode: %d", aErrorCode);
	LOG1("CImagePrintBody::PrintJobErrorEvent aErrorStringCode: %d", aErrorStringCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrintJobErrorEvent(aErrorCode, aErrorStringCode);
	LOG("CImagePrintBody::PrintJobErrorEvent end");
	}

void CImagePrintBody::PrinterStatusEvent(TInt aErrorCode, TInt aErrorStringCode)
	{
	LOG1("CImagePrintBody::PrinterStatusEvent aErrorCode: %d", aErrorCode);
	LOG1("CImagePrintBody::PrinterStatusEvent aErrorStringCode: %d", aErrorStringCode);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PrinterStatusEvent(aErrorCode, aErrorStringCode);
	LOG("CImagePrintBody::PrinterStatusEvent end");
	}

void CImagePrintBody::PreviewImageEvent(TInt aFsBitmapHandle)
	{
	LOG1("CImagePrintBody::PreviewImageEvent aFsBitmapHandle: %d", aFsBitmapHandle);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->PreviewImageEvent(aFsBitmapHandle);
	LOG("CImagePrintBody::PreviewImageEvent end");
	}

void CImagePrintBody::ShowMessageL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CImagePrintBody::ShowMessageL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CImagePrintBody::ShowMessageL aMsgLine2Code: %d", aMsgLine2Code);
	if( iPrinterPrintEventObserver )
		iPrinterPrintEventObserver->ShowMessageL(aMsgLine1Code, aMsgLine2Code);
	LOG("CImagePrintBody::ShowMessageL end");
	}

TBool CImagePrintBody::AskYesNoQuestionL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CImagePrintBody::AskYesNoQuestionL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CImagePrintBody::AskYesNoQuestionL aMsgLine2Code: %d", aMsgLine2Code);
	TBool res( EFalse );
	if( iPrinterPrintEventObserver )
		{
		res = iPrinterPrintEventObserver->AskYesNoQuestionL(aMsgLine1Code, aMsgLine2Code);	
		}
	LOG1("CImagePrintBody::AskYesNoQuestionL return: %d", res);	
	return res;
	}

const TDesC& CImagePrintBody::AskForInputL(TInt aMsgLine1Code, TInt aMsgLine2Code)
	{
	LOG1("CImagePrintBody::ShowMessageL aMsgLine1Code: %d", aMsgLine1Code);
	LOG1("CImagePrintBody::ShowMessageL aMsgLine2Code: %d", aMsgLine2Code);
	if( iPrinterPrintEventObserver )
		{
		return iPrinterPrintEventObserver->AskForInputL(aMsgLine1Code, aMsgLine2Code);	
		}
	else
		{
		return KNullDesC;	
		}				
	}

void CImagePrintBody::StatusEvent(const TEvent& aEvent, TInt aError, TInt aMsgCode)
	{
	LOG1("CImagePrintBody::StatusEvent aError: %d", aError);
	LOG1("CImagePrintBody::StatusEvent aMsgCode: %d", aMsgCode);
	LOG1("CImagePrintBody::StatusEvent aEvent.iProtocol: %d", aEvent.iProtocol);
	LOG1("CImagePrintBody::StatusEvent aEvent.iSeverity: %d", aEvent.iSeverity);
	LOG1("CImagePrintBody::StatusEvent aEvent.iEventType: %d", aEvent.iEventType);
	
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
	LOG("CImagePrintBody::StatusEvent end");
	}

CImagePrintBody::CImagePrintBody() :
							iCurrentProtocol( NULL ),
							iCurrentPrintJobProtocol( NULL ) 
	{
	LOG("CImagePrintBody::CImagePrintBody begin");	
	LOG("CImagePrintBody::CImagePrintBody end");
	}

void CImagePrintBody::ConstructL()
	{
	LOG("CImagePrintBody::ConstructL begin");	
	iClassLoader = CProtocolsLoader::NewL();
	iClassLoader->Reset();
	for( TInt x=0; x < iClassLoader->GetNumberOfProtocolsAvailable(); ++x )
		{
		iClassLoader->GetNextProtocol()->RegisterIdleObserver( this );
		}
	
	iIdleDiscovery = CIdle::NewL( CActive::EPriorityIdle );
	ActiveScheduler().SetIdleObserver( *this );
	LOG("CImagePrintBody::ConstructL end");
	}
	
CImagePrintScheduler& CImagePrintBody::ActiveScheduler() const
	{
	LOG("CImagePrintBody::ActiveScheduler begin");
	CImagePrintScheduler* scheduler = static_cast<CImagePrintScheduler*>( CActiveScheduler::Current() );
	LOG("CImagePrintBody::ActiveScheduler end");
	return *scheduler;
	}

TInt CImagePrintBody::FindInternalDeviceReference(TInt aExternalDeviceID, TInt& aInternalDeviceID, MProtPrintingDevice*& aProtToUse)
	{
	LOG1("CImagePrintBody::FindInternalDeviceReference aExternalDeviceID: %d", aExternalDeviceID);
	TInt err( KErrNotFound );
	
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
	
	LOG1("CImagePrintBody::FindInternalDeviceReference return: %d", err);	
	return err;
	}

TInt CImagePrintBody::ValidateImagesL(const RPointerArray<HBufC>& aImageList, TInt &aInvalidCount)
	{
	LOG1("CImagePrintBody::ValidateImagesL aImageList.Count(): %d", aImageList.Count());
	aInvalidCount = 0;
	TInt lastError = KErrNone;
	CRsImageParser *imageParser = CRsImageParser::NewLC();

	for( TInt nCnt=0; nCnt < aImageList.Count(); ++nCnt )
		{
		LOG1("CImagePrintBody::ValidateImagesL nCnt: %d", nCnt);
		LOG1("CImagePrintBody::ValidateImagesL image: %S", aImageList[nCnt]);
		TBool valid;
		imageParser->ValidateL(*(aImageList[nCnt]), valid, lastError);
		LOG1("CImagePrintBody::ValidateImagesL valid: %d", valid);
		LOG1("CImagePrintBody::ValidateImagesL lastError: %d", lastError);
		if( !valid )
			{
			++aInvalidCount;
			}	
		}
		
	CleanupStack::PopAndDestroy(imageParser);
	LOG1("CImagePrintBody::ValidateImagesL aInvalidCount: %d", aInvalidCount);
	LOG1("CImagePrintBody::ValidateImagesL ends with: %d", lastError);
	return lastError;
	}
	
void CImagePrintBody::TryNextDiscovery()
	{
	LOG("CImagePrintBody::TryNextDiscovery begin");
	iIdleDiscovery->Cancel();
	iIdleDiscovery->Start(TCallBack(TryNextDiscoveryL, this));
	LOG("CImagePrintBody::TryNextDiscovery end");	
	}

void CImagePrintBody::DoTryNextDiscoveryL()
	{
	LOG("CImagePrintBody::DoTryNextDiscoveryL begin");	
	TUint doDiscovery = iProtocolsRequested & iClassLoader->SupportedProtocols();
	LOG1("CImagePrintBody::DoTryNextDiscoveryL doDiscovery: %d", doDiscovery);	
	if( doDiscovery )
		{
		TBool found( EFalse );
		TInt protId( 0 );
    	TInt count = iClassLoader->GetNumberOfProtocolsAvailable();
    	LOG1("CImagePrintBody::DoTryNextDiscoveryL count: %d", count);
    	for( TInt i = 0; i < count && !found; i++ )
	        {
	        iCurrentProtocol = iClassLoader->GetNextProtocol();
	        protId = iCurrentProtocol->SupportedProtocols();
	        if( iProtocolsRequested & protId ) found = ETrue;
	        }
		LOG1("CImagePrintBody::DoTryNextDiscoveryL found: %d", found);
		LOG1("CImagePrintBody::DoTryNextDiscoveryL protId: %d", protId);
		if( found )
	        {
	        TUint temp = KMaxTUint ^ protId;
	        iProtocolsRequested &= temp;
	        LOG1("CImagePrintBody::DoTryNextDiscoveryL iProtocolsRequested: %d", iProtocolsRequested);
	        ActiveScheduler().SetCurrentProtocol( protId );
	        iCurrentProtocol->StartDiscoveryL( *this, protId );
	        }	        
		}
	else
		{
		iPrinterDiscoveryObserver->DiscoveryStatusL( EDiscoveryFinished, 0, 0 );
		}			
	LOG("CImagePrintBody::DoTryNextDiscoveryL end");
	}
	
TInt CImagePrintBody::TryNextDiscoveryL(TAny* aObj)	
	{
	LOG("CImagePrintBody::TryNextDiscoveryL begin");
	CImagePrintBody* obj = static_cast<CImagePrintBody*>(aObj);
	obj->DoTryNextDiscoveryL();		
	LOG("CImagePrintBody::TryNextDiscoveryL end");
	return EFalse;
	}

void CImagePrintBody::SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr )
	{	
	LOG1("CImagePrintBody::SetNumberOfCopiesL aNumsOfCopies.Count(): %d", aNumsOfCopies.Count());

	if( iCurrentPrintJobProtocol )
		{
		iCurrentPrintJobProtocol->SetNumsOfCopiesL( aNumsOfCopies, aErr );
		LOG1("CImagePrintBody::SetNumberOfCopiesL aErr: %d", aErr);	
		}
		
	LOG("CImagePrintBody::SetNumberOfCopiesL end");	
	}

void CImagePrintBody::HandleSessionDisconnect( const MIdleObserver* aIdleObserver,
											const MPrinterDiscoveryObserver* aDiscoveryObserver,
											const MPrintEventObserver* aEventObserver )
	{
	LOG1("CImagePrintBody::HandleSessionDisconnect aIdleObserver address: %d", aIdleObserver);
	LOG1("CImagePrintBody::HandleSessionDisconnect iPrinterIdleObserver address: %d", iPrinterIdleObserver);	
	LOG1("CImagePrintBody::HandleSessionDisconnect aDiscoveryObserver address: %d", aDiscoveryObserver);
	LOG1("CImagePrintBody::HandleSessionDisconnect iPrinterDiscoveryObserver address: %d", iPrinterDiscoveryObserver);	
	LOG1("CImagePrintBody::HandleSessionDisconnect aEventObserver address: %d", aEventObserver);	
	LOG1("CImagePrintBody::HandleSessionDisconnect iPrinterPrintEventObserver address: %d", iPrinterPrintEventObserver);	
	if( aIdleObserver == iPrinterIdleObserver )	iPrinterIdleObserver = NULL;
	if( aEventObserver == iPrinterPrintEventObserver ) iPrinterPrintEventObserver = NULL;
	if( aDiscoveryObserver == iPrinterDiscoveryObserver ) iPrinterDiscoveryObserver = NULL;
	LOG("CImagePrintBody::HandleSessionDisconnect end");
	}

//  End of File
