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
* Description:  Contains the CPrintJob class definition and the PJS enum.
*
*/


#include <e32std.h>
#include <pathinfo.h>
#include <driveinfo.h>

#include "cprintjob.h"
#include "printcapabilitycodes.h"
#include "imageprint.h"
#include "clog.h"
#include "tbtmapper.h"
#include "btprotocolconsts.h"




// Public functions
//--------------------------------------------------------------------------------------------
//
// CPrintJob::NewL
//
//--------------------------------------------------------------------------------------------
CPrintJob* CPrintJob::NewL(CRsBtDevice* aDevice, RArray<TFileName>& aImages,
							const TDesC8& aUserName)
{
	CPrintJob* self = new (ELeave) CPrintJob(aDevice);
	CleanupStack::PushL(self);
	self->ConstructL(aImages, aUserName);
	CleanupStack::Pop();	// self
	return self;
}


//--------------------------------------------------------------------------------------------
//
// CPrintJob::~CPrintJob
//
//--------------------------------------------------------------------------------------------
CPrintJob::~CPrintJob()
{
	iImages.Reset();
	iImages.Close();
	iDevice = NULL;
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::SetJobId
//
//--------------------------------------------------------------------------------------------
void CPrintJob::SetJobId(const TInt aId)
{
	iJobId = aId;
}


//--------------------------------------------------------------------------------------------
//
// CPrintJob::GetJobId
//
//--------------------------------------------------------------------------------------------
TInt CPrintJob::JobId()
{
	return iJobId;
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::GetJobName
//
//--------------------------------------------------------------------------------------------
void CPrintJob::GetJobName(TDes8& aJobName)
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
// CPrintJob::ImageCount
//
//--------------------------------------------------------------------------------------------
TInt CPrintJob::ImageCount()
{
	return iImages.Count();
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::Images
//
//--------------------------------------------------------------------------------------------
void CPrintJob::Images(RArray<CImageInfo>& aImages)
{
	for (TInt i=0; i < iImages.Count(); i++)
	{
		aImages.Append(iImages[i]);
	}
}


//--------------------------------------------------------------------------------------------
//
// CPrintJob::PrinterId
//
//--------------------------------------------------------------------------------------------
TInt CPrintJob::PrinterId()
{
	if(Device())
		return Device()->DeviceId();
	
	return KErrNotFound;
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::GetUserName
//
//--------------------------------------------------------------------------------------------
void CPrintJob::GetUserName(TPtrC8& aUserName)
{
	aUserName.Set(iUserName);
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::GetUserName
//
//--------------------------------------------------------------------------------------------
CRsBtDevice* CPrintJob::Device()
{
	if(iDevice)
		return iDevice;
	else
		return NULL;
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::SetNumsOfCopies
//
//--------------------------------------------------------------------------------------------
TInt CPrintJob::SetNumsOfCopies( const RArray<TInt>& aNumsOfCopies)
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
// CPrintJob::GetPrintFileL
//
//--------------------------------------------------------------------------------------------
void CPrintJob::GetPrintFileL(TDes8& aPrintFile)
{
	LOG("[CPrintJob::GetPrintFileL]\t begin");
	
	TInt layout;
	TInt paper;
	TInt quality;

	GetPrintSetting(EPrintCapabLayout, layout);
	GetPrintSetting(EPrintCapabPaperSize, paper);
	GetPrintSetting(EPrintCapabQuality, quality);

	TFileName tmpFile;
	tmpFile.Copy(aPrintFile);

	CXhtmlFileComposer* fileComposer = CXhtmlFileComposer::NewLC(KNullDesC());

	LOG("[CPrintJob::GetPrintFileL]\t create file");
	fileComposer->CreateXhtmlFileL( iImages, layout, paper, quality, tmpFile, iSheets );
	
	CleanupStack::PopAndDestroy(fileComposer);
	aPrintFile.Copy(tmpFile);

	LOG("[CPrintJob::GetPrintFileL]\t file created");
}


//--------------------------------------------------------------------------------------------
//
// CPrintJob::Sheets
//
//--------------------------------------------------------------------------------------------
TInt CPrintJob::Sheets()
{
	if(0 == iSheets)
	{
		TInt layout;
		TInt layoutNbr = 1;
		TInt imageNbr = 0;

		// We don't care about return value here: if GetPrintSetting
		// fails, we can anyway tell there will be at least an image/page
		GetPrintSetting(EPrintCapabLayout, layout);

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
		{
			++iSheets;
		}
	}
	LOG1("[CPrintJob::Sheets]\t iSheets = %d", iSheets);
	return iSheets;
}

// Protected functions
//--------------------------------------------------------------------------------------------
//
// CPrintJob::ConstructL
//
//--------------------------------------------------------------------------------------------
void CPrintJob::ConstructL(RArray<TFileName>& aImages, const TDesC8& aUserName)
{
	if (aImages.Count() < 1)
	{
		User::Leave(KErrArgument);
	}

	_LIT(KObex, "obex:");
	_LIT(KMmc, "\\mmc");
	_LIT(KLocal, "\\media");
	_LIT(KSlash, "/");
	_LIT(KBackSlash, "\\");
	
	// Initialize iImages array
	for (TInt i=0 ; i<aImages.Count(); i++)
	{
		// Skip empty strings
		if (aImages[i].Length() < 1)
		{
			continue;
		}

		aImages[i].LowerCase();
		
		CImageInfo imageInfo;
		imageInfo.SetId(KErrNotFound);
		imageInfo.SetIndex(i);
		imageInfo.SetCopies(1);	
		
		// Image file path
		imageInfo.SetFilePathL(aImages[i]);

		// Parse obex "uri"
		TFileName tmpUri;
		
		TParse parse;
		parse.Set(aImages[i], NULL, NULL);

		// Get drive id and root path when multiple drive is enabled.
		TInt driveId;
		TFileName localRoot;
		User::LeaveIfError(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, driveId));
		User::LeaveIfError(PathInfo::GetRootPath(localRoot, driveId));

		localRoot.LowerCase();
		
		tmpUri.Copy(KObex());
		// Note here: the correct format would be uuid.LongForm(), 
		// but in currently supported services it is not required.
		tmpUri.AppendNum(KBTSDPDPROService, EHex);

		TFileName drive = parse.Drive();
		
		TInt pos = localRoot.Find(drive);
		if(KErrNotFound < pos)
		{
			tmpUri.Append(KLocal()); 
		}
		else
		{
			tmpUri.Append(KMmc());
		}
		tmpUri.Append(parse.Path());
		tmpUri.Append(parse.NameAndExt());

		pos = tmpUri.Find(KBackSlash());
		while(KErrNotFound != pos)
		{
			tmpUri.Replace(pos, KBackSlash().Length(), KSlash());
			pos = tmpUri.Find(KBackSlash());
		}

		LOG1("[CPrintJob::ConstructL]\t tmpUri: \"%S\"", &tmpUri);
		imageInfo.SetUriL(tmpUri);
//		LOG82("[CPrintJob::ConstructL]\t iFilePath: \"%S\", iUri: \"%S\"", &imageInfo.iFilePath, &imageInfo.iUri);

		iImages.Append(imageInfo);
	}

	iUserName.Copy(aUserName);
	iSheets = 0;
}


// Private functions
//--------------------------------------------------------------------------------------------
//
// CPrintJob::CPrintJob
//
//--------------------------------------------------------------------------------------------
CPrintJob::CPrintJob(CRsBtDevice* aDevice) :
	iJobId(KErrNotFound),
	iDeviceId(KErrNotFound),
	iDevice(aDevice),
	iUserName(KNullDesC8()),
	iSheets(0),
	iJobState(CPrintJob::EStateNone)
{
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::UpdateJobState
//
//--------------------------------------------------------------------------------------------
void CPrintJob::UpdateJobState(const TJobState aState)
{
	iJobState = aState;
}

//--------------------------------------------------------------------------------------------
//
// CPrintJob::JobState
//
//--------------------------------------------------------------------------------------------
CPrintJob::TJobState CPrintJob::JobState()
{
	return iJobState;
}

//--------------------------------------------------------------------------------------------
// CPrintJob::GetPrintSetting
//--------------------------------------------------------------------------------------------
TInt CPrintJob::GetPrintSetting(TInt aCapabilityID, TInt& aValue)
{
	if(Device())
	{
		for(TInt i = 0; i < Device()->CapabilityCount(); ++i)
		{
			TPrintCapability c;
			if(KErrNone == Device()->GetCapability(aCapabilityID, c) )
			{
				aValue = c.iDefaultValue;
				LOG2("[CPrintJob::GetPrintSetting]\t %d = %d", aCapabilityID, aValue);
				return KErrNone;
			}
		}
	}

	aValue = KErrNotFound;
	
	LOG("[CPrintJob::GetPrintSetting]\t out: KErrInvalidData");
	return KErrInvalidData;
}

//--------------------------------------------------------------------------------------------
// CPrintJob::SetPrintSetting
//--------------------------------------------------------------------------------------------
TInt CPrintJob::SetPrintSetting(TInt aCapabilityID, TInt aValue)
{
	LOG2("[CPrintJob::SetPrintSetting]\t %d = %d", aCapabilityID, aValue);

	if(Device())
		return Device()->SetDefaultCapability(aCapabilityID, aValue);
	
	LOG("[CPrintJob::GetPrintSetting]\t out: KErrInvalidData");
	return KErrInvalidSequence;
}

//  End of File
