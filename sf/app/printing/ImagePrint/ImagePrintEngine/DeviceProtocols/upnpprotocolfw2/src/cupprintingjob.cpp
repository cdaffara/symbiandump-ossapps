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
* Description:  Declares CUPPrintingJob class
*
*/


#include <upnpdevice.h>
#include <escapeutils.h> //utf8 conversions
#include <upnpprotocolinfo.h>
#include <upnpmediaserversettings.h>

#include "cupprintingjob.h"
#include "cupprinter.h"
#include "cuplogger.h"
#include "cupprintfactory.h"
#include "printcapabilitycodes.h"
#include "cupfilesharingactive.h"



_LIT8(KResource, "res");
_LIT(KMediaTypes, "image/jpeg:*,audio/mpeg:MP3,audio/mp4:AAC_ISO_320,video/mp4:AVC_MP4_BL_CIF15_AAC_520,application/vnd.pwg-xhtml-print+xml:*");

// Public functions
//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::NewL
//
//--------------------------------------------------------------------------------------------
CUPPrintingJob* CUPPrintingJob::NewL(CUPPrinter* aPrinter, RArray<TFileName>& aImages, 
									 CUPPrintFactory* aFileFactory, const TDesC8& aUserName)
{
	CUPPrintingJob* self = new (ELeave) CUPPrintingJob(aFileFactory);
	CleanupStack::PushL(self);
	self->ConstructL(aPrinter, aImages, aUserName);
	CleanupStack::Pop();	// self
	return self;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::~CUPPrintingJob
//
//--------------------------------------------------------------------------------------------
CUPPrintingJob::~CUPPrintingJob()
{
	// Stop sharing
	UnShareImages();
  
  if(iFileSharing)
  	delete iFileSharing;
  
  if(iOrgMediaServerStatus != RUpnpMediaServerClient::EStartedOnline)
  {
  	iMediaServerClient.Stop();
  	if(iOrgMediaServerStatus == RUpnpMediaServerClient::EStartedOffline)
  	{
  	  iMediaServerClient.StartOffline();
  	}
  }
  
  iMediaServerClient.Close();
  
	iImages.Close();

	if(iJobState)
		delete iJobState;
}

//--------------------------------------------------------------------------------------------
//
// CUAPrintingJob::SetSessionId
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::SetSessionId(TInt aId)
{
	iSessionId = aId;
}


//--------------------------------------------------------------------------------------------
//
// CUAPrintingJob::SessionId
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::SessionId()
{
	return iSessionId;
}



//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::SetJobId
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::SetJobId(const TDesC8& aId)
{
	iJobId.Copy(aId);
	iJobState->SetActiveId(aId);
	// SetActiveId initializes iJobState -> set sheets to print
	iJobState->SetSheetsToPrint(Sheets());

	LOG1("[CUPPrintingJob::SetJobId]\t Sheets(). %d", Sheets());
	
}


//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::GetJobId
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::GetJobId(TDes8& aId)
{
	aId.Copy(iJobId);
}


//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::GetJobName
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::GetJobName(TDes8& aJobName)
{
	// Return the name of first image
	if (iImages.Count() > 0)
	{
		iImages[0].GetFilePath(aJobName);
	}
	else
	{
		aJobName.Append(KNullDesC8());
	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::ImageCount
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::ImageCount()
{
	return iImages.Count();
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::Images
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::Images(RArray<CImageInfo>& aImages)
{
	for (TInt i=0; i < iImages.Count(); i++)
	{
		aImages.Append(iImages[i]);
	}
}


//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::PrinterId
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::PrinterId()
{
	return iPrinterId;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::GetUserName
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::GetUserName(TPtrC8& aUserName)
{
	aUserName.Set(iUserName);
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::ShareFileL
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::ShareFileL(CUpnpItem* aItemToShare) 
{
	User::LeaveIfNull(aItemToShare);

	LOG81("[CUPPrintingJob::ShareFileL]\t ShareFileL: %S", &(aItemToShare->RefId()));

	TRAPD( err, iFileSharing->ShareItemL(*aItemToShare));
	
	if (KErrAlreadyExists == err)
	{
		// Resolve http address of the already shared file
		TLex8 tmp(aItemToShare->Id());
		TInt tmpId;
		tmp.Val(tmpId);

		iFileSharing->GetSharedItemL(tmpId, *aItemToShare);

	}
	else if(err == KErrNotFound)
	{
		LOG("[CUPPrintingJob]\t - ShareItemL: file not found");
		User::Leave(EPbFileNotFound);
	}
	else if(err != KErrNone)
	{
		LOG1("[CUPPrintingJob]\t - ShareItemL: %d", err);
		User::Leave(err);
	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::ShareImagesL
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::ShareImagesL()
{
	LOG("[CUPPrintingJob]\t ShareImagesL");
	TBuf8<1024> tmpUri;
	for (TInt i=0; i<iImages.Count(); i++)
	{
		// Skip images with no file path (they are already shared by external repository)
		TFileName8 name;
		iImages[i].GetFilePath(name);
		if (name.Length() < 1)
		{
			continue;
		}

		// Generate id
		TInt itemId = KUPnPIdBase+i;	
	  LOG("[CUPPrintingJob::ConstructL]\t File sharing opened");
		CUpnpItem* upnpItem = UPnPItemLC(itemId, name, KImageType, KJpegMime);

		ShareFileL(upnpItem);
		
		// Get "res"-element value after filesharing.
		RUPnPElementsArray tmpElementArray;
		tmpElementArray = upnpItem->GetElements();
		for(TInt j = 0; j<tmpElementArray.Count();j++)
			if(tmpElementArray[j]->Name() == KResource)
				tmpUri.Copy(tmpElementArray[j]->Value());

		// Parse shared folder address in the "res"-element value.
		TFileName8 itemAddress;		
		TBuf8<1024> tmpPartOfUri;
		tmpPartOfUri.Copy( tmpUri.Mid(28));
		itemAddress.Copy(iMediaServerAddress);
		itemAddress.Append(tmpPartOfUri); 

		
		// Set http address and id to image information
		iImages[i].SetUri(itemAddress, ETrue);
		LOG82("[CPrintJob::ShareImagesL]\t iFilePath: \"%S\", iUri: \"%S\"", &name, &itemAddress);
		
		TPtrC8 tmp = upnpItem->Id();
		TLex8 lex( tmp );
		TInt id;
		lex.Val( id );
		iImages[i].SetId(id);
		
		CleanupStack::PopAndDestroy( upnpItem );

	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::UnShareImages
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::UnShareImages()
{
	LOG("[CUPPrintingJob]\t UnShareImages");
	
	for (TInt i=0; i < iImages.Count(); i++)
	{
		UnShareFile(iImages[i].Id());
	}

	return KErrNone;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::UnShareFile
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::UnShareFile(TInt aItemId)
{
	LOG("[CUPPrintingJob::UnShareFile]");

	if (aItemId < 0)
	{
		// Not shared because no item id is set
		return KErrNone;
	}
		
	TBuf8<KMaxIdLength> itemIdStr;
	itemIdStr.AppendNum(aItemId);
	LOG1("[CUPPrintingJob::UnShareFile]\t Item id = %d", aItemId);

	TRAPD(err, iFileSharing->UnshareItemL(aItemId));
	LOG1("[CUPPrintingJob::UnShareFile]\t err = %d", err);
	return err;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::SetNumsOfCopies
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::SetNumsOfCopies( const RArray<TInt>& aNumsOfCopies)
{
	TInt err = KErrNone;
	
	for(TInt arrayIx = 0, imgIx = 0; arrayIx < aNumsOfCopies.Count(); ++arrayIx)
	{
		if(iImages[imgIx].Index() < aNumsOfCopies.Count())
		{
			TInt copies = aNumsOfCopies[iImages[imgIx].Index()];
			if(0 < copies)
				iImages[imgIx].SetCopies(copies);
			++imgIx;
		}
		else
			err = KErrCorrupt;
	}
	return err;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::GetPrintFileL
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::GetPrintFileL(TDes8& aPrintFile)
{
	TInt layout;
	TInt paper;
	TInt quality;
	TBuf8<1024> tmpUri;
	
	iPrintFactory->GetPrintSetting(EPrintCapabLayout, layout);
	iPrintFactory->GetPrintSetting(EPrintCapabPaperSize, paper);
	iPrintFactory->GetPrintSetting(EPrintCapabQuality, quality);

	TFileName tmpFile;
	tmpFile.Copy(aPrintFile);
	iPrintFactory->CreateXhtmlFileL( iImages, layout, paper, quality, tmpFile, iSheets );
	aPrintFile.Copy(tmpFile);
}


//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::Sheets
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::Sheets()
{
	if(0 == iSheets)
	{
		TInt layout;
		TInt layoutNbr = 0;
		TInt imageNbr = 0;
		
		iPrintFactory->GetPrintSetting(EPrintCapabLayout, layout);

		switch( layout )
		{
			case EPrintCapabLayout1Up:
			case EPrintCapabLayout1UpBorderless:
			case EPrintCapabLayout1UpBorder:
				layoutNbr = 1;
				break;
			case EPrintCapabLayout2Up:
				layoutNbr = 2;
				break;
			case EPrintCapabLayout4Up:
				layoutNbr = 4;
				break;
			case EPrintCapabLayout6Up:
				layoutNbr = 6;
				break;
			case EPrintCapabLayout9Up:
				layoutNbr = 9;
				break;
			case EPrintCapabLayout12Up:
				layoutNbr = 12;
				break;
			case EPrintCapabLayout16Up:
				layoutNbr = 16;
				break;
			default:
				break;
		}

		for (TInt i = 0; i<iImages.Count(); ++i)
		{
			imageNbr += iImages[i].Copies();
		}
		
		iSheets = imageNbr / layoutNbr;	
		if(0 < imageNbr % layoutNbr)
			++iSheets;
	}
	LOG1("[CUPPrintingJob::Sheets]\t iSheets = %d", iSheets);
	return iSheets;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::Progress
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::Progress()
{
	return iJobState->Progress(Sheets());
}


// Protected functions
//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::ConstructL
//
//--------------------------------------------------------------------------------------------
void CUPPrintingJob::ConstructL(CUPPrinter* aPrinter, RArray<TFileName>& aImages, const TDesC8& aUserName)
{
	_LIT(KHttpPrefixLow16, "http://");
	
	if (aImages.Count() < 1)
	{
		User::Leave(KErrArgument);
	}
	
	// Initialize iImages array
	for (TInt i=0 ; i<aImages.Count(); i++)
	{
		// Skip empty strings
		if (aImages[i].Length() < 1)
		{
			continue;
		}
		
		aImages[i].LowerCase();

		CImageInfo *imageInfo = CImageInfo::NewLC();
				
		imageInfo->SetId(KErrNotFound);
		imageInfo->SetIndex(i);
		imageInfo->SetCopies(1);	
		
		if (aImages[i].Compare(KHttpPrefixLow16()) < 0 )
		{
			// Images with file path
			imageInfo->SetFilePathL(aImages[i]);
		}
		else
		{
			// Images with HTTP address	
			imageInfo->SetUriL(aImages[i]);
		}
		iImages.Append( *imageInfo );
		CleanupStack::PopAndDestroy( imageInfo );
	}	
	
	// Set printer 
	iPrinterId = aPrinter->Id();
	
	iUserName.Set(aUserName);
	iJobState = CUPJobState::NewL();
	iSheets = 0;
	
	iFileSharing = CUPFileSharingActive::NewL();
	
  // Connect to the local media server and open file sharing session
  LOG("[CUPPrintingJob::ConstructL]\t Media server connected");
  User::LeaveIfError(iMediaServerClient.Connect());
  
  // get original MS status and start MS if needed. 
  // If MS is in wrong status(started offline mode) then stop MS and start it in online-mode.
  iMediaServerClient.Status(iOrgMediaServerStatus);
  if(iOrgMediaServerStatus != RUpnpMediaServerClient::EStartedOnline)
  {
  	if(iOrgMediaServerStatus == RUpnpMediaServerClient::EStartedOffline)
  	{
  	  iMediaServerClient.Stop();
  	}
  	CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();                        
	TBuf8<1024> media;    
 
	//// Supported Media                                             
	media.Copy(KMediaTypes);
	settings->SetL(UpnpMediaServerSettings::ESupportedMedia, media);
	delete settings;
  	LOG("[CUPPrintingJob::ConstructL]\t Media server start");
    User::LeaveIfError(iMediaServerClient.Start());
  }
  
  // Get Media server ip address and port number.
	TFileName8 tmpAddress;
	TInetAddr timppi;
	TInt err = iMediaServerClient.GetAddress(timppi);
	
	TFileName addr16;
	timppi.Output(addr16);

	// Address
	iMediaServerAddress.Copy( KHttpPrefix() );
	iMediaServerAddress.Append(addr16);

	// Port
	iMediaServerAddress.Append( KUPnPColon() );
	iMediaServerAddress.AppendNum(timppi.Port());
}


// Private functions
//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::CUPPrintingJob
//
//--------------------------------------------------------------------------------------------
CUPPrintingJob::CUPPrintingJob(CUPPrintFactory* aFileFactory) : 
iSessionId(KErrNotFound),
iPrintFactory(aFileFactory),
iXhtmlFileShareId(0)
{
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::UPnPItem
//
//--------------------------------------------------------------------------------------------
CUpnpItem* CUPPrintingJob::UPnPItemLC(TInt aItemId, const TDesC8& aFileName, const TDesC8& aObjectType, const TDesC8& aMimeType)
{
	// Init upnp item
	CUpnpItem* upnpItem = CUpnpItem::NewL();
	CleanupStack::PushL(upnpItem);

	upnpItem->SetRefIdL(aFileName);
	TFileName8 tmpTitle;
	TInt tmpLocate = aFileName.LocateReverse( TChar( '\\' ) );
	tmpLocate++;
	tmpTitle.Copy(aFileName.Mid(tmpLocate));
	upnpItem->SetTitleL(tmpTitle);
	upnpItem->SetObjectClassL(aObjectType);

	TBuf8<KMaxIdLength> itemIdStr;
	itemIdStr.AppendNum(aItemId);
	upnpItem->SetIdL(itemIdStr);
	upnpItem->SetRestricted(0);
	_LIT8(KDefParentId, "0");
	upnpItem->SetParentIdL(KDefParentId);

	// Convert FileName to the unicode that MediaServer can handle it correct.
	HBufC16* buf = EscapeUtils::ConvertToUnicodeFromUtf8L(aFileName);
	CleanupStack::PushL(buf);
  
	CUpnpProtocolInfo* protocolInfo = CUpnpProtocolInfo::NewL();
  CleanupStack::PushL(protocolInfo);
  protocolInfo->SetFirstFieldL(_L8("http-get"));
  protocolInfo->SetSecondFieldL(_L8("*"));
  protocolInfo->SetThirdFieldL(aMimeType);
  protocolInfo->SetFourthFieldL(_L8("*"));
  
  upnpItem->AddResourceL(buf->Des(), protocolInfo->ProtocolInfoL());
  CleanupStack::Pop(protocolInfo);
 	CleanupStack::PopAndDestroy(buf);

	return upnpItem;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::UpdateJobState
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::UpdateJobState(const TDesC8& aPrinterState, const TDesC8& aReason, const TDesC8& aJobList, const TDesC8& aJobId, const TDesC8& aJobMediaSheetsCompleted)
{
//	LOG("[CUPPrintingJob::UpdateJobState]\t");

	return iJobState->UpdateJobState(aPrinterState, aReason, aJobList, aJobId, aJobMediaSheetsCompleted);
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobStateReason
//
//--------------------------------------------------------------------------------------------
TInt CUPPrintingJob::JobStateReason()
{
	return iJobState->Reason();
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobState
//
//--------------------------------------------------------------------------------------------
CUPJobState* CUPPrintingJob::JobState()
{
	return iJobState;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::PrinterStateUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::PrinterStateUpdated( const TDesC8& aEvent )
{
	return iJobState->PrinterStateUpdated(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::PrinterStateReasonsUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::PrinterStateReasonsUpdated( const TDesC8& aEvent )
{
	return iJobState->PrinterStateReasonsUpdated(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobIdListUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::JobIdListUpdated( const TDesC8& aEvent )
{
	return iJobState->JobIdListUpdated(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobEndStateUpdatedL
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::JobEndStateUpdatedL( const TDesC8& aEvent )
{
	return iJobState->JobEndStateUpdatedL(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobMediaSheetsCompletedUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::JobMediaSheetsCompletedUpdated( const TDesC8& aEvent )
{
	return iJobState->JobMediaSheetsCompletedUpdated(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobMediaSheetsCompletedUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::JobMediaSheetsCompletedUpdated(const TDesC8& aId, const TDesC8& aEvent )
{
	return iJobState->JobMediaSheetsCompletedUpdated(aId, aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::JobAbortStateUpdatedL
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::JobAbortStateUpdatedL( const TDesC8& aEvent )
{
	return iJobState->JobAbortStateUpdatedL(aEvent);	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrintingJob::ContentCompleteListUpdated
//
//--------------------------------------------------------------------------------------------
TUPnPState CUPPrintingJob::ContentCompleteListUpdated( const TDesC8& aEvent )
{
	return iJobState->ContentCompleteListUpdated(aEvent);	
}

//  End of File
