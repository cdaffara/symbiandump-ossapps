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
* Description:  Declares CUPPrintFactory class
*
*/



#include <f32file.h>

#include "cupprintfactory.h"
#include "upconsts.h"
#include "cuplogger.h"

_LIT( KHtmFileName, "UPnPPrint_%d.xhtml" );
_LIT(KUPnPSettingsFile, "settings.txt");

// (TInt64 == 21 chars)*2 + , + \n --> must fit in 50
const TInt KSettingsLength = 50;


// -----------------------------------------------------------------------------
// CUPPrintFactory::NewL
// -----------------------------------------------------------------------------
//
CUPPrintFactory* CUPPrintFactory::NewL( )
{
	CUPPrintFactory* self = new (ELeave) CUPPrintFactory();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
}


// -----------------------------------------------------------------------------
// CUPPrintFactory::~CUPPrintFactory
// Destructor
// -----------------------------------------------------------------------------
//
CUPPrintFactory::~CUPPrintFactory( )
{
	// No can do if deleting fails -> ignore leave
    TRAP_IGNORE( DeleteXhtmlFilesL() );
    iFileSession.Close();
    iCapabilities.Close();
}

// -----------------------------------------------------------------------------
// CUPPrintFactory::CreateXhtmlFileL
// -----------------------------------------------------------------------------
//
void CUPPrintFactory::CreateXhtmlFileL(const RArray<CImageInfo>& aJobURIs, const TInt aLayout, 
								  const TInt aPaperSize, const TInt aQuality,
								  TDes& aXhtmlPrintFile, TInt& aPageCount)
{
	if ( aJobURIs.Count() < 1 )
	{
		User::Leave( KErrArgument );
	}

    iFileCount++;

    //Create XHTML-print file
	aXhtmlPrintFile.Format(KHtmFileName(), iFileCount);
	
	// parse XHTML-file
	CXhtmlFileComposer *fileParser = CXhtmlFileComposer::NewLC(KUPnPXhtmlPath());
	fileParser->CreateXhtmlFileL(aJobURIs, aLayout, aPaperSize, aQuality, aXhtmlPrintFile, aPageCount);
	CleanupStack::PopAndDestroy(fileParser);
}

// -----------------------------------------------------------------------------
// CUPPrintFactory::DeleteXhtmlFilesL
// -----------------------------------------------------------------------------
//
void CUPPrintFactory::DeleteXhtmlFilesL( )
{
    CFileMan* fileMan = CFileMan::NewL( iFileSession );
    CleanupStack::PushL( fileMan ); 
    fileMan->RmDir( KUPnPXhtmlPath() );
    CleanupStack::PopAndDestroy(fileMan);
}


// -----------------------------------------------------------------------------
// CUPPrintFactory::FileCount
// -----------------------------------------------------------------------------
//
TInt CUPPrintFactory::FileCount()
{
	return iFileCount;
}

// -----------------------------------------------------------------------------
// CUPPrintFactory::GetCapabilityIDs
// -----------------------------------------------------------------------------
//
void CUPPrintFactory::GetCapabilityIDs(RArray<TInt>& aCapabilityIDs )
{
	for (TInt i=0; i < iCapabilities.Count(); i++)
	{
		aCapabilityIDs.Append(iCapabilities[i].iCapabilityID);
	}
}

// -----------------------------------------------------------------------------
// CUPPrintFactory::GetCapability
// -----------------------------------------------------------------------------
//
TInt CUPPrintFactory::GetCapability(const TInt aCapabilityID, TPrintCapability& aCapability)
{
	for (TInt i=0; i < iCapabilities.Count(); i++)
	{
		if (aCapabilityID == iCapabilities[i].iCapabilityID)
		{
			// copy capability to ouput param
			aCapability.iCapabilityID 	= aCapabilityID;
			aCapability.iDefaultValue 	= iCapabilities[i].iDefaultValue;
			aCapability.iType 			= iCapabilities[i].iType;
			aCapability.iEnumCount 		= 0;

			
			for (TInt j=0; j < iCapabilities[i].iEnumCount; j++)
			{
				aCapability.iEnumCodes[j] = iCapabilities[i].iEnumCodes[j];
				aCapability.iEnumCount++;
			}
			
			return KErrNone;
		}
	}
	
	return KErrNotSupported;
}

//--------------------------------------------------------------------------------------------
// CUPPrintFactory::GetPrintSetting
//--------------------------------------------------------------------------------------------
TInt CUPPrintFactory::GetPrintSetting(TInt aCapabilityID, TInt& aValue)
{
	for (TInt i=0; i<iCapabilities.Count(); i++)
	{
		if (iCapabilities[i].iCapabilityID == aCapabilityID)
		{
			aValue = iCapabilities[i].iDefaultValue;
			return KErrNone;
		}
	}
	return KErrNotSupported;
}


// -----------------------------------------------------------------------------
// CUPPrintFactory::SetPrintSetting
// -----------------------------------------------------------------------------
//
TInt CUPPrintFactory::SetPrintSetting(TInt aCapabilityID, TInt aValue)
{
	for (TInt i=0; i<iCapabilities.Count(); i++)
	{
		if (iCapabilities[i].iCapabilityID == aCapabilityID)
		{
			// validate value
			for (TInt j=0; j<iCapabilities[i].iEnumCount; j++)
			{
				if ( aValue == iCapabilities[i].iEnumCodes[j] )
				{
					iCapabilities[i].iDefaultValue = aValue;
					TRAPD(err, SetDefaultSettingsL());
					if(KErrNone != err)
						LOG1("[CUPPrintFactory::SetPrintSetting]\t Writing default values failed. Error: %d", err);
					return KErrNone;
				}
			}
			return KErrArgument;
		}
	}
	
	return KErrNotSupported;
}

// PRIVATE
// -----------------------------------------------------------------------------
// CUPPrintFactory::CUPPrintFactory
// CPP constructor
// -----------------------------------------------------------------------------
//
CUPPrintFactory::CUPPrintFactory()
{
    iFileCount = 0;
}


// -----------------------------------------------------------------------------
// CUPPrintFactory::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUPPrintFactory::ConstructL( )
{
	User::LeaveIfError( iFileSession.Connect() );
	iFileSession.SetSessionPath( KUPnPDataPath() );	

	// Create data and XHTML-file folder
	TInt err = iFileSession.MkDirAll( KUPnPDataPath() );
	if (err == KErrNone || err == KErrAlreadyExists)
	{
		err = iFileSession.MkDirAll( KUPnPXhtmlPath() );
	}

	if (err != KErrNone && err != KErrAlreadyExists)
	{
		User::Leave(err);
	}

	// Init capabilities
	InitCapabilities();

}

// -----------------------------------------------------------------------------
// CUPPrintFactory::InitCapabilities
// -----------------------------------------------------------------------------
//
void CUPPrintFactory::InitCapabilities()
{
	// Layout
	TPrintCapability layout;
	layout.iCapabilityID = EPrintCapabLayout;
	layout.iType = TPrintCapability::Int;
	layout.iDefaultValue = EPrintCapabLayout1UpBorderless;

	// Supported layouts
	layout.iEnumCodes[0] = EPrintCapabLayout1UpBorderless;
	layout.iEnumCodes[1] = EPrintCapabLayout1UpBorder;
	layout.iEnumCodes[2] = EPrintCapabLayout2Up;
	layout.iEnumCodes[3] = EPrintCapabLayout4Up;
	layout.iEnumCodes[4] = EPrintCapabLayout6Up;
	layout.iEnumCodes[5] = EPrintCapabLayout9Up;
	layout.iEnumCodes[6] = EPrintCapabLayout12Up;
	layout.iEnumCodes[7] = EPrintCapabLayout16Up;
	layout.iEnumCount = 8;
	
	// Quality
	TPrintCapability quality;
	quality.iCapabilityID = EPrintCapabQuality;
	quality.iType = TPrintCapability::Int;
	quality.iDefaultValue = EPrintCapabQualityNormal;

	// Supported quality
	quality.iEnumCodes[0] = EPrintCapabQualityNormal;
	quality.iEnumCount = 1;
	
	// Paper size
	TPrintCapability paper;
	paper.iCapabilityID = EPrintCapabPaperSize;
	paper.iType = TPrintCapability::Int;
	paper.iDefaultValue = EPrintCapabPaperSize4x6;

	// Supported paper sizes
	paper.iEnumCodes[0] = EPrintCapabPaperSize4x6;
	paper.iEnumCodes[1] = EPrintCapabPaperSize5x7;
	paper.iEnumCodes[2] = EPrintCapabPaperSizeA6;
	paper.iEnumCodes[3] = EPrintCapabPaperSizeA4;
	paper.iEnumCodes[4] = EPrintCapabPaperSizeLetter;
	paper.iEnumCount = 5;
	
	// Add to settings array
	TRAPD(err, GetDefaultSettingsL(layout.iDefaultValue, paper.iDefaultValue, quality.iDefaultValue))
	if(KErrNone != err)
		LOG1("[CUPPrintFactory::InitCapabilities]\t Reading default values failed. Error: %d", err);

	iCapabilities.Append(layout);
	iCapabilities.Append(quality);
	iCapabilities.Append(paper);
	
}

//--------------------------------------------------------------------------------------------
// CUPPrintFactory::GetDefaultSettingsL
//--------------------------------------------------------------------------------------------
void CUPPrintFactory::GetDefaultSettingsL(TInt& aLayout, TInt& aSize, TInt& aQuality)
{
	LOG("[CUAPrintingJobContainer::GetDefaultSettingsL]\t");
	
	RFile file;
	RFs fs;
	User::LeaveIfError(file.Open(iFileSession, KUPnPSettingsFile, EFileRead));
	CleanupClosePushL(file);
	
	TInt fileSize = 250;
	file.Size(fileSize);
	
	if(0 >= fileSize)
		fileSize = 250;
	
	// Create buffer
	HBufC8* fileBuffer = HBufC8::NewL(fileSize);
	CleanupStack::PushL(fileBuffer);

	TPtr8 bufferPtr( fileBuffer->Des() );
	bufferPtr.Zero();
	
	User::LeaveIfError(file.Read(bufferPtr));
	LOG81("bufferPtr: %S", &bufferPtr);

	TInt stopper = 0;
	TInt pos = bufferPtr.Find(KUPnPLineFeed8());
	while (pos > 0 && stopper < 20)
	{
		++stopper;
		LOG("----------------");

		TLex8 lex(bufferPtr.Left(pos));
		LOG81("bufferPtr.Left(pos): %S", &(bufferPtr.Left(pos)));

		// get id
		if ((lex.Peek()).IsDigit()) 
		{
			TInt id;
			TInt err =  lex.Val(id);
			LOG1("id: %d", id);
			
			bufferPtr = bufferPtr.Right(bufferPtr.Length() - lex.Offset()-1);
			lex.Assign(bufferPtr);

			//get value
			if ((lex.Peek()).IsDigit()) 
			{
				switch( id ) 
				{
					case EPrintCapabLayout:
						User::LeaveIfError(lex.Val(aLayout));
						LOG1("aLayout: %d", aLayout);
						break;
					case EPrintCapabPaperSize:
						User::LeaveIfError(lex.Val(aSize));
						LOG1("aSize: %d", aLayout);
						break;
					case EPrintCapabQuality:
						User::LeaveIfError(lex.Val(aQuality));
						LOG1("aQuality: %d", aQuality);
						break;
					default:
						break;
				}

				bufferPtr = bufferPtr.Right(bufferPtr.Length() - lex.Offset()-1);
				lex.Assign(bufferPtr);
			}
		}
		// find next
		pos = bufferPtr.Find(KUPnPLineFeed8());
	}
	CleanupStack::PopAndDestroy(2); //fileBuffer, file
}

//--------------------------------------------------------------------------------------------
// CUPPrintFactory::SetDefaultSettingsL
//--------------------------------------------------------------------------------------------
void CUPPrintFactory::SetDefaultSettingsL()
{
	LOG("[CUAPrintingJobContainer::SetDefaultSettingsL]\t");
	
	// Create buffer
	HBufC8* settingsBuf = HBufC8::NewL(KSettingsLength * iCapabilities.Count());
	CleanupStack::PushL(settingsBuf);

	for (TInt i=0; i<iCapabilities.Count(); i++)
	{
		
		TBuf8<50> settingsStr;
		settingsStr.AppendNum(iCapabilities[i].iCapabilityID);
		settingsStr.Append(KUPnPComma());
		settingsStr.AppendNum(iCapabilities[i].iDefaultValue);
		settingsStr.Append(KUPnPLineFeed8());

		settingsBuf->Des().Append(settingsStr);
	}
	
	RFile file;
	User::LeaveIfError(file.Replace(iFileSession, KUPnPSettingsFile, EFileWrite));
	CleanupClosePushL(file);
	file.Write(settingsBuf->Des());

	CleanupStack::PopAndDestroy(2); //file, settingsBuf
}

// End of file
