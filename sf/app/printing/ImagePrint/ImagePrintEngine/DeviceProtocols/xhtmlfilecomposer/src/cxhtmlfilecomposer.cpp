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
* Description:  Defines the CXhtmlFileComposer class.
*
*/


#include <imageconversion.h>

#ifdef _DEBUG
#include <driveinfo.h>
#include <pathinfo.h>
#endif

#include "cxhtmlfilecomposer.h"
#include "printcapabilitycodes.h"
#include "cxfclogger.h"

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::NewL
//
//--------------------------------------------------------------------------------------------
EXPORT_C CXhtmlFileComposer* CXhtmlFileComposer::NewL(const TDesC& aPath)
	{
		CXhtmlFileComposer *self = NewLC(aPath);
		CleanupStack::Pop();	// self

		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::NewLC
//
//--------------------------------------------------------------------------------------------
EXPORT_C CXhtmlFileComposer* CXhtmlFileComposer::NewLC(const TDesC& aPath)
	{
		CXhtmlFileComposer *self = new (ELeave) CXhtmlFileComposer();
		CleanupStack::PushL(self);

		if(0 >= aPath.Length())
		{
			self->ConstructL(KXFCDefaultResultPath());
		}
		else
		{
			self->ConstructL(aPath);
		}

		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::CreateXhtmlFileL
//
//--------------------------------------------------------------------------------------------
EXPORT_C void CXhtmlFileComposer::CreateXhtmlFileL(const RArray<CImageInfo>& aImages, const TInt aLayout,
								  const TInt aPaperSize, const TInt /*aQuality*/,
								  const TDesC& aXhtmlPrintFile,TInt& aPageCount, TBool aDataFile)
{
	LOG("[CXhtmlFileComposer]\t WriteJobControlFileL");

	// Get page and layout related data
	PageData(aPaperSize);

	RArray<TSizeReal> positions;
	CleanupClosePushL(positions);
	LayoutData(aLayout, positions);

	ReadTemplateFileL();

	// init counter variables
	aPageCount = 0;
	TInt posIx = 0;
	TInt copies = 0;
	TInt currentImgNum = 0;
	TInt imageCount = aImages.Count();

	// loop for images
	for(TInt imgIx = 0; imgIx < aImages.Count(); imgIx++)
	{
		/**** Handle copies ****/
		copies = aImages[imgIx].Copies();
		imageCount += copies - 1; // decrease current image from copies

		// Create template blocks for each copy
		while(1 <= copies && imageCount > currentImgNum)
		{
			AddPageL(currentImgNum, posIx, aPageCount);
			AddTemplateStringL(iFrameTemplate, KFrame());

			// add frame position template once per position for all pages
			if(1 == aPageCount)
			{
				AddTemplateStringL(iPositionTemplate, KFramePosition());
				ComposePositionL(posIx, positions);
			}
			ComposeTemplateL(posIx+1, KPosId());
			--copies;
			++currentImgNum;
		}

		// add image details
		AddTemplateStringL(iImageTemplate, KImgDetails());
		ComposeTemplateL(1+imgIx, KDetId());

		// Get correct name for the image
		TFileName8 imageName;
		GetImageNameL(aImages[imgIx], imageName, aDataFile);

		/**** Image size and orientation ****/
		TSizeReal imageSize(0,0);
		TBool framePortrait = EFalse;

		SizeAndOrientationL(aImages[imgIx], imageSize, framePortrait);

		/**** Image scaling & cropping ****/
		TSizeReal scaledSize;
		GetRatioL(scaledSize, imageSize);

		// Turn frame and scaled size in correct orientation again
		if(framePortrait != iImagePortrait)
		{
			scaledSize.Reverse();
			iImageFrame.Reverse();
		}

		ComposeFileL(imageName, scaledSize, aDataFile);

	}// End of loop for images
	CleanupStack::PopAndDestroy( &positions );

	// Compose the correct unit in file
	ComposeUnitL();

	/**** Create result file ****/
	RFile newFile;
	CleanupClosePushL( newFile );
	User::LeaveIfError( newFile.Replace( iFsSession, aXhtmlPrintFile, EFileWrite) );
	newFile.Write( FinalizeBufferL() );
	CleanupStack::PopAndDestroy( &newFile );
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::~CXhtmlFileComposer
//
//--------------------------------------------------------------------------------------------
EXPORT_C CXhtmlFileComposer::~CXhtmlFileComposer()
{
	iFsSession.Close();
	if(iFileBuf)
		delete iFileBuf;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::UnitToPixel
//
//--------------------------------------------------------------------------------------------
EXPORT_C TReal CXhtmlFileComposer::UnitToPixel(TReal aValue)
{
	TReal tmp = aValue;
	if(EUAMillis == iPaperType)
		tmp = aValue/(TReal)KInchDivider;

	tmp = tmp*(TReal)KDpiDivider;

	return tmp;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::UnitToPixel
//
//--------------------------------------------------------------------------------------------
EXPORT_C TSizeReal CXhtmlFileComposer::UnitToPixel(TSizeReal aValue)
{
	TSizeReal tmp = aValue;
	if(EUAMillis == iPaperType)
		tmp = aValue/(TReal)KInchDivider;

	tmp = tmp*(TReal)KDpiDivider;

	return tmp;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::PixelToUnit
//
//--------------------------------------------------------------------------------------------
EXPORT_C TReal CXhtmlFileComposer::PixelToUnit(TReal aValue)
{
	TReal tmp = aValue/(TReal)KDpiDivider;
	if(EUAMillis == iPaperType)
		tmp = tmp*(TReal)KInchDivider;

	return tmp;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::PixelToUnit
//
//--------------------------------------------------------------------------------------------
EXPORT_C TSizeReal CXhtmlFileComposer::PixelToUnit(TSizeReal aValue)
{
	TSizeReal tmp = aValue/(TReal)KDpiDivider;
	if(EUAMillis == iPaperType)
		tmp = tmp*(TReal)KInchDivider;

	return tmp;
}

// PROTECTED

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ConstructL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ConstructL(const TDesC& aPath)
{
	LOG("[CXhtmlFileComposer::ConstructL]\t");

	User::LeaveIfError( iFsSession.Connect() );
	iFsSession.SetSessionPath( aPath );
	iFsSession.MkDirAll( aPath );

	iPaperSize.SetSize(0,0);
	iPaperType = 0;
	iPageMargin = 0;
	iImageFrame.SetSize(0,0);
	iFileBuf = NULL;
	iImagesPerPage = 0;
	iImagePortrait=EFalse;
	iPagePortrait = ETrue;
	iPageTemplate.Copy(KNullDesC8());
	iFrameTemplate.Copy(KNullDesC8());
	iPositionTemplate.Copy(KNullDesC8());
	iImageTemplate.Copy(KNullDesC8());

	LOG("[CXhtmlFileComposer::ConstructL]\t Done.");
}

// PRIVATE

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::CXhtmlFileComposer
//
//--------------------------------------------------------------------------------------------
CXhtmlFileComposer::CXhtmlFileComposer()
{
}


//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::GetRatioL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::GetRatioL(TSizeReal &aNewSize, TSizeReal aImageSize)
{
	LOG("[CXhtmlFileComposer::GetRatioL]\t ********************************");

	TRealX margin = 0;
	TXhtmlSetting marginStr, tmpStr;

	LOG2("[CXhtmlFileComposer::GetRatioL]\t aImageSize: %fx%f", PixelToUnit(aImageSize.iWidth), PixelToUnit(aImageSize.iHeight));
	LOG2("[CXhtmlFileComposer::GetRatioL]\t iImageFrame: %fx%f", PixelToUnit(iImageFrame.iWidth), PixelToUnit(iImageFrame.iHeight));
	LOG2("[CXhtmlFileComposer::GetRatioL]\t image ratio: %f vs. frame ratio %f", aImageSize.Ratio(), iImageFrame.Ratio());

	TReal relativeRatio = aImageSize.Ratio() / iImageFrame.Ratio();
	LOG2("[CXhtmlFileComposer::GetRatioL]\t relativeRatio: %f, iPageMargin %f", relativeRatio, iPageMargin);

	// crop borderless: if iPageMargin == 0, the margin must be negative
	if((1 < relativeRatio && iPageMargin) || (1 > relativeRatio && !iPageMargin))
	{
		LOG("[CXhtmlFileComposer::GetRatioL]\t *Match width*");
		aNewSize.SetSize(iImageFrame.iWidth, iImageFrame.iWidth/aImageSize.Ratio());
		margin = (iImageFrame.iHeight-aNewSize.iHeight)/2;

	    if((!iPagePortrait && !iPageMargin && iImagePortrait) || iPagePortrait)
	    	marginStr.Copy(KTop());
		else
	    	marginStr.Copy(KLeft());
	}
	else
	{
		LOG("[CXhtmlFileComposer::GetRatioL]\t *Match height*");
		aNewSize.SetSize(iImageFrame.iHeight*aImageSize.Ratio(), iImageFrame.iHeight);
		margin = (iImageFrame.iWidth-aNewSize.iWidth)/2;

	    if((!iPagePortrait && !iPageMargin && iImagePortrait) || iPagePortrait)
	    	marginStr.Copy(KLeft());
		else
	    	marginStr.Copy(KTop());
	}

	FormatReal(PixelToUnit(margin), tmpStr);
	marginStr.Append(tmpStr);

	LOG2("[CXhtmlFileComposer::GetRatioL]\t aNewSize: %fx%f", PixelToUnit(aNewSize.iWidth), PixelToUnit(aNewSize.iHeight));
	LOG81("[CXhtmlFileComposer::GetRatioL]\t \"%S\"", &marginStr);

	// margin not needed anymore: compose margin into file buffer
	ComposeTemplateL(marginStr, KImgMargin());
	LOG("[CXhtmlFileComposer::GetRatioL]\t ********************************");
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ComposeUnitL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ComposeUnitL()
{
	LOG("[CXhtmlFileComposer]\t ComposeUnit");

	if(EUAMillis == iPaperType)
	{
		_LIT8(KMm, "mm");
		ComposeTemplateL(KMm(), KUnit());
	}
	else
	{
		_LIT8(KIn, "in");
		ComposeTemplateL(KIn(), KUnit());
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ComposeFile
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ComposeFileL(const TDes8 &aImageName, TSizeReal aImgSize, TBool aDataFile)
{
	LOG("[CXhtmlFileComposer]\t ComposeFile");

	// Format page settings in file
	TXhtmlSetting pageWidthStr, pageHeightStr, pageMarginStr;

	FormatReal(PixelToUnit(iPaperSize.iWidth), pageWidthStr);
	ComposeTemplateL(pageWidthStr, KPageWidth());

	FormatReal(PixelToUnit(iPaperSize.iHeight), pageHeightStr);
	ComposeTemplateL(pageHeightStr, KPageHeight());

	FormatReal(iPageMargin, pageMarginStr);
	ComposeTemplateL(pageMarginStr, KPageMargin());

	// Format image frame settings in file
	TXhtmlSetting imgFrameWidthStr, imgFrameHeightStr;

	FormatReal(PixelToUnit(iImageFrame.iWidth), imgFrameWidthStr);
	ComposeTemplateL(imgFrameWidthStr, KImgFrameWidth());

	FormatReal(PixelToUnit(iImageFrame.iHeight), imgFrameHeightStr);
	ComposeTemplateL(imgFrameHeightStr, KImgFrameHeight());

	// Format image settings in file
	TXhtmlSetting imgWidthStr, imgHeightStr;

	FormatReal(PixelToUnit(aImgSize.iWidth), imgWidthStr);
	ComposeTemplateL(imgWidthStr, KImgWidth());

	FormatReal(PixelToUnit(aImgSize.iHeight), imgHeightStr);
	ComposeTemplateL(imgHeightStr, KImgHeight());

///////////
	HBufC8* data = NULL;
	if(aDataFile)
	{

		TFileName imgFile;
		imgFile.Copy(aImageName);

		LOG("[CXhtmlFileComposer]\t ComposeFileL - read image data");
		// Open the template file and read it into buffer
		RFile file;
		CleanupClosePushL(file);

		User::LeaveIfError( file.Open( iFsSession, imgFile, EFileRead | EFileShareReadersOnly ) );
		LOG("[CXhtmlFileComposer]\t ComposeFileL - file opened");

		TInt fileSize = 0;
		User::LeaveIfError( file.Size(fileSize) );
		LOG1("[CXhtmlFileComposer]\t ComposeFileL - file size %d",fileSize);

		_LIT8(KDataUrl, "data:image/jpeg;base64,");

		data = HBufC8::NewLC(fileSize + KDataUrl().Length());
		TPtr8 dataPtr( data->Des() );

		dataPtr.Zero();
		file.Read(dataPtr);

		dataPtr.Insert(0, KDataUrl());

		ComposeTemplateL(dataPtr, KImgName());
		LOG("[CXhtmlFileComposer]\t ComposeFileL - file composed");

		CleanupStack::PopAndDestroy(2); // data, file
	}
///////////
	else
	{
		// Format image name in file
		ComposeTemplateL(aImageName, KImgName());
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ComposeTemplateL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ComposeTemplateL(const TInt aValue, const TDesC8 &aTemplate)
{
//	LOG("[CXhtmlFileComposer]\t ComposeTemplateL");

	TBuf8<KDefaultRealWidth> valueStr;
	valueStr.Zero();
	valueStr.AppendNum(aValue);

	ComposeTemplateL(valueStr, aTemplate);
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ComposeTemplateL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ComposeTemplateL(const TDesC8 &aStr, const TDesC8 &aTemplate)
{
//	LOG("[CXhtmlFileComposer]\t ComposeTemplateL");

	User::LeaveIfNull(iFileBuf);

	TInt pos = iFileBuf->Find(aTemplate);
	while(KErrNotFound != pos)
	{
		TInt requiredSize = iFileBuf->Des().Length() + aStr.Length();
		if(iFileBuf->Des().MaxLength() < requiredSize)
		{
			iFileBuf = iFileBuf->ReAllocL(requiredSize);
		}
		iFileBuf->Des().Replace(pos, aTemplate.Length(), aStr);
		pos = iFileBuf->Find(aTemplate);
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::SetPageTypeL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::SetPageTypeL()
{
	LOG("[CXhtmlFileComposer]\t SetPageTypeL");

	User::LeaveIfNull(iFileBuf);

	TXhtmlSetting pageType(KPageFirsts());

	TInt pos = iFileBuf->Find(KPageType());
	if(KErrNotFound >= pos)
		User::Leave(KErrCorrupt);

	while(KErrNotFound != pos)
	{
		// check if next exists to assign correct page type in template
		TPtrC8 tmpBuf;
		tmpBuf.Set(iFileBuf->Mid(pos+KPageType().Length()));

		if(KErrNotFound == tmpBuf.Find(KPageType()))
			pageType.Copy(KPageLast());

		// replace correct page type in first found position
		TInt requiredSize = iFileBuf->Des().Length() + pageType.Length();
		if(iFileBuf->Des().MaxLength() < requiredSize)
		{
			iFileBuf = iFileBuf->ReAllocL(requiredSize);
		}
		iFileBuf->Des().Replace(pos, KPageType().Length(), pageType);
		pos = iFileBuf->Find(KPageType());
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::FinalizeBufferL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXhtmlFileComposer::FinalizeBufferL()
{
	LOG("[CXhtmlFileComposer]\t FinalizeBufferL");

	User::LeaveIfNull(iFileBuf);

	SetPageTypeL();

	// Remove markers from the buffer
	TInt pos = iFileBuf->Find(KPage());
	if(KErrNotFound != pos)
		iFileBuf->Des().Delete(pos, KPage().Length());

	pos = iFileBuf->Find(KFrame());
	if(KErrNotFound != pos)
		iFileBuf->Des().Delete(pos, KFrame().Length());

	pos = iFileBuf->Find(KImgDetails());
	if(KErrNotFound != pos)
		iFileBuf->Des().Delete(pos, KImgDetails().Length());

	pos = iFileBuf->Find(KFramePosition());
	if(KErrNotFound != pos)
		iFileBuf->Des().Delete(pos, KFramePosition().Length());

	// Cut the template to the end tag
	pos = iFileBuf->Find(KEndTag());
	if(KErrNotFound == pos)
		User::Leave(KErrCorrupt);

	TInt dataLen = pos + KEndTag().Length();
	TPtrC8 tail = iFileBuf->Des().Left(dataLen);

	return tail;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::PageData
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::PageData(TInt aPaperSize)
{
	LOG("[CXhtmlFileComposer]\t PageData");

	const TSizeReal KSizeA4(210.0, 297.0);
	const TSizeReal KSizeA6(105.0, 148.5);
	const TSizeReal KSizeLetter(8.5, 11.0);
	const TSizeReal KSize4x6(4.0, 6.0);
	const TSizeReal KSize5x7(5.0, 7.0);

	switch(aPaperSize)
	{
		case EPrintCapabPaperSizeA4:
			iPaperType = CXhtmlFileComposer::EUAMillis;
			iPaperSize = UnitToPixel(KSizeA4);
			iPageMargin = 3.0;
			break;
		case EPrintCapabPaperSizeA6:
			iPaperType = CXhtmlFileComposer::EUAMillis;
			iPaperSize = UnitToPixel(KSizeA6);
			iPageMargin = 3.0;
			break;
		case EPrintCapabPaperSizeLetter:
			iPaperType = CXhtmlFileComposer::EUAInches;
			iPaperSize = UnitToPixel(KSizeLetter);
			iPageMargin = 0.12;
			break;
		case EPrintCapabPaperSize5x7:
			iPaperType = CXhtmlFileComposer::EUAInches;
			iPaperSize = UnitToPixel(KSize5x7);
			iPageMargin = 0.12;
			break;
		case EPrintCapabPaperSizeAuto:
		case EPrintCapabPaperSizeSticker:
		case EPrintCapabPaperSize4x6:
		default:
			iPaperType = CXhtmlFileComposer::EUAInches;
			iPaperSize = UnitToPixel(KSize4x6);
			iPageMargin = 0.12;
			break;
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::LayoutData
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::LayoutData(TInt aLayout, RArray<TSizeReal>& aPositions)
{
	LOG1("[CXhtmlFileComposer::LayoutData]\t aLayout: %d", aLayout);

	TSizeReal pos(0,0);

	switch(aLayout)
	{
		case EPrintCapabLayout1UpBorder:
			iImagesPerPage = 1;
			iPagePortrait = EFalse;
			CreateMatrix(1/*rows*/, 1/*columns*/, aPositions);
			break;
		case EPrintCapabLayout2Up:
			iImagesPerPage = 2;
			iPagePortrait = ETrue;
			CreateMatrix(2, 1, aPositions);
			break;
		case EPrintCapabLayout4Up:
			iImagesPerPage = 4;
			iPagePortrait = EFalse;
			CreateMatrix(2, 2, aPositions);
			break;
		case EPrintCapabLayout6Up:
			iImagesPerPage = 6;
			iPagePortrait = ETrue;
			CreateMatrix(3, 2, aPositions);
			break;
		case EPrintCapabLayout9Up:
			iImagesPerPage = 9;
			iPagePortrait = EFalse;
			CreateMatrix(3, 3, aPositions);
			break;
		case EPrintCapabLayout12Up:
			iImagesPerPage = 12;
			iPagePortrait = ETrue;
			CreateMatrix(4, 3, aPositions);
			break;
		case EPrintCapabLayout16Up:
			iImagesPerPage = 16;
			iPagePortrait = EFalse;
			CreateMatrix(4, 4, aPositions);
			break;
		case EPrintCapabLayout1Up:
		case EPrintCapabLayout1UpBorderless:
		default:
			iImagesPerPage = 1;
			iPageMargin = 0;
			iImageFrame = iPaperSize;
			iPagePortrait = EFalse;
			aPositions.Append(pos);
			break;
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::CreateMatrix
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::CreateMatrix(TInt aRows, TInt aColumns, RArray<TSizeReal>& aPositions)
{
	LOG("[CXhtmlFileComposer]\t CreateMatrix");

	TSizeReal pos;
	TSizeReal matrix(aColumns,aRows);
	TSizeReal relativeSize(KFrameRatio, 1);

	if(!iPagePortrait)
	{
		matrix.Reverse();
		relativeSize.Reverse();
	}

	// count print area: paper size - margins
	TSizeReal margins(matrix);
	margins += 1; 
	margins *= UnitToPixel(iPageMargin);
	LOG2("[CXhtmlFileComposer::CreateMatrix]\t margins: %fx%f", PixelToUnit(margins.iWidth), PixelToUnit(margins.iHeight));

	TSizeReal printArea = (iPaperSize - margins);
	LOG2("[CXhtmlFileComposer::CreateMatrix]\t printArea: %fx%f", PixelToUnit(printArea.iWidth), PixelToUnit(printArea.iHeight));

	// Count frame size

	iImageFrame = printArea/matrix;
	LOG2("[CXhtmlFileComposer::CreateMatrix]\t iImageFrame bef. ratio: %fx%f", PixelToUnit(iImageFrame.iWidth), PixelToUnit(iImageFrame.iHeight));

	LOG1("[CXhtmlFileComposer::CreateMatrix]\t frame ratio: %f", iImageFrame.Ratio());

	// force frame ratio to KFrameRatio ratio
	LOG1("[CXhtmlFileComposer::CreateMatrix]\t relativeRatio: %f", relativeSize.Ratio());

	TReal relativeRatio = relativeSize.Ratio()/iImageFrame.Ratio();
	if(1 < relativeRatio)
	{
		iImageFrame.iHeight /= relativeRatio;
	}
	else if(1 > relativeRatio)
	{
		iImageFrame.iWidth *= relativeRatio;
	}

	LOG2("[CXhtmlFileComposer::CreateMatrix]\t iImageFrame aft. ratio: %fx%f", PixelToUnit(iImageFrame.iWidth), PixelToUnit(iImageFrame.iHeight));
	LOG1("[CXhtmlFileComposer::CreateMatrix]\t --> ratio: %f (1.3325/0.7505)", iImageFrame.Ratio());

	// Count the empty space on page to centerise the frame set
	TSizeReal frameSetSize(iImageFrame*matrix);
	TSizeReal emptySpace = (printArea - frameSetSize) / 2;

	LOG2("[CXhtmlFileComposer::CreateMatrix]\t frameSetSize:\t %fx%f", PixelToUnit(frameSetSize.iWidth), PixelToUnit(frameSetSize.iHeight));
	LOG2("[CXhtmlFileComposer::CreateMatrix]\t emptySpace:\t %fx%f", PixelToUnit(emptySpace.iWidth), PixelToUnit(emptySpace.iHeight));

	// if only one image/page, no need to calculate positions.
	if(0 >= aColumns || 0 >= aRows)
	{
		aPositions.Append(pos);
		return;
	}

	emptySpace.Reverse();
	
	for(TInt row = 0; row < aRows; ++row)
	{
		LOG1("[CXhtmlFileComposer::CreateMatrix]\t row: %d", row);
		pos.iWidth = (iImageFrame.iHeight+UnitToPixel(iPageMargin)) * row;
		pos.iWidth += emptySpace.iWidth;
		for(TInt col = 0; col < aColumns; ++col)
		{
			pos.iHeight = (iImageFrame.iWidth+UnitToPixel(iPageMargin)) * col;
			pos.iHeight += emptySpace.iHeight;
			LOG2("[CXhtmlFileComposer::CreateMatrix]\t col: %fx%f", PixelToUnit(pos.iWidth), PixelToUnit(pos.iHeight));
			aPositions.Append(pos);
		}
	}
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::ReadTemplateL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ReadTemplateFileL()
{
	LOG("[CXhtmlFileComposer]\t ReadTemplateL");
	// Open the template file and read it into buffer
	RFile templateFile;
	CleanupClosePushL(templateFile);

#ifdef _DEBUG
	//_LIT(KFile, "e:\\UPnPXHTMLTemplate.xhtml");
	_LIT(KFileName, "UPnPXHTMLTemplate.xhtml");
	TInt drive;
  TFileName path;

  DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, drive);
  PathInfo::GetRootPath(path, drive);
  path.Append(KFileName());
    
  TInt e = templateFile.Open( iFsSession, path, EFileRead);
	if(KErrNone != e)
#endif
	{
		User::LeaveIfError( templateFile.Open( iFsSession, KXHTMLTemplate(), EFileRead) );
	}
	TInt fileSize = 0;
	TInt err = templateFile.Size(fileSize);
	if(KErrNone != err)
		fileSize = KXFCMaxFileSize;

	iFileBuf = HBufC8::NewL(fileSize);
	TPtr8 bufferPtr( iFileBuf->Des() );

	bufferPtr.Zero();
	templateFile.Read(bufferPtr);

	CleanupStack::PopAndDestroy(&templateFile);

	// Separate page, frame position, image details and frame
	// template elements from the template buffer
	GetTemplateStringL(KPageStart(), KPageEnd(), iPageTemplate);
	GetTemplateStringL(KFrameStart(), KFrameEnd(), iFrameTemplate);
	GetTemplateStringL(KPositionStart(), KPositionEnd(),iPositionTemplate);
	GetTemplateStringL(KImgStart(), KImgEnd(), iImageTemplate);
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::GetTemplateStringL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CXhtmlFileComposer::GetTemplateStringL(const TDesC8 &aStart, const TDesC8 &aEnd, TDes8 &aTemplate)
{
	User::LeaveIfNull(iFileBuf);

	TInt start = iFileBuf->Find(aStart);
	if(KErrNotFound == start)
		User::Leave(KErrCorrupt);

	TInt end = iFileBuf->Find(aEnd);
	if(KErrNotFound == end)
		User::Leave(KErrCorrupt);

	TPtrC8 ptr = iFileBuf->Des().Mid(start+aStart.Length(), end-start-aStart.Length());

	if(aTemplate.MaxLength() < ptr.Length())
		User::Leave(KErrOverflow);

	aTemplate = ptr;
	iFileBuf->Des().Delete(start, ptr.Length()+aStart.Length()+aEnd.Length());
	return ptr;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::AddTemplateStringL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::AddTemplateStringL(const TDesC8 &aTemplate,const TDesC8 &aTag)
{
	User::LeaveIfNull(iFileBuf);

	TInt pos = iFileBuf->Find(aTag);
	if(KErrNotFound == pos)
		User::Leave(KErrCorrupt);

	TInt requiredSize = iFileBuf->Des().Length() + aTemplate.Length();
	if(iFileBuf->Des().MaxLength() < requiredSize)
	{
		iFileBuf = iFileBuf->ReAllocL(requiredSize);
	}
	iFileBuf->Des().Insert(pos, aTemplate);
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::GetImageDataL
//
//--------------------------------------------------------------------------------------------
TSizeReal CXhtmlFileComposer::GetImageSizeL(CImageInfo aImage)
{
	TSizeReal imageSize(0,0);
	TFileName name;
	aImage.GetFilePathL(name);
	if(name.Compare(KNullDesC()))
	{
		// Test file existing
		TEntry data;
		TInt err = iFsSession.Entry(name, data);
		User::LeaveIfError(err);

		// Get image size in pixels
		CImageDecoder *id = NULL;
		TRAP(err, id = CImageDecoder::FileNewL(iFsSession, name, KMimeJpeg()));

		if(KErrNone == err)
		{
			TFrameInfo frameInfo;
			frameInfo = id->FrameInfo();
			imageSize = frameInfo.iOverallSizeInPixels;
		}

		if(id)
			delete id;
	}
	LOG2("[CXhtmlFileComposer]\t Image size: %fx%f", imageSize.iWidth, imageSize.iHeight);
	return imageSize;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::AddPageL
//
//--------------------------------------------------------------------------------------------
TBool CXhtmlFileComposer::AddPageL(TInt aCurrentImage, TInt& aPositionIndex, TInt& aPageCount)
{
	TBool ret = EFalse;
	// increase position index by default and reset it in case of new page
	++aPositionIndex;

	User::LeaveIfNull(iFileBuf);
	if(0 >= iImagesPerPage)
		User::Leave(KErrArgument);

	TInt modulo = aCurrentImage % iImagesPerPage;

	if(0 == modulo)
	{
		//delete current page's frame marker before...
		TInt pos = iFileBuf->Find(KFrame());
		if(KErrNotFound != pos)
			iFileBuf->Des().Delete(pos, KFrame().Length());

		// ...add the new page template and reset position index
		AddTemplateStringL(iPageTemplate, KPage());
		aPositionIndex = 0;
		++aPageCount;
		ret = ETrue;
	}
	return ret;
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::GetImageName
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::GetImageNameL(CImageInfo aImageInfo, TDes8 &aImageName, TBool aDataFile)
{
	aImageInfo.GetEncodedUriL(aImageName);
	if(0 >= aImageName.Length() || aDataFile)
	{
		aImageInfo.GetFilePath(aImageName);
		if(0 >= aImageName.Length())
			User::Leave(KErrArgument);
	}
	
	LOG81("[CXhtmlFileComposer]\t Image: %S", &aImageName);
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::SizeAndOrientationL
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::SizeAndOrientationL(const CImageInfo aImageInfo, TSizeReal& aImageSize, TBool& aFramePortrait)
{
	aImageSize = GetImageSizeL(aImageInfo);

	// If the size of any of the sides is zero, set the size as frame size
	if(0 >= aImageSize.iWidth)
		aImageSize = iImageFrame;

	// orientations
	iImagePortrait = (aImageSize.iWidth < aImageSize.iHeight ? ETrue : EFalse);
	aFramePortrait = (iImageFrame.iWidth < iImageFrame.iHeight ? ETrue : EFalse);

	// Set orientation in degrees
	TInt orientation = KXFCZeroDeg;
	if(aFramePortrait != iImagePortrait)
		orientation = KXFC90Deg;

	// Set orientation in template
	TXhtmlSetting orientationStr;
	orientationStr.Format(KDeg(), orientation);
	ComposeTemplateL(orientationStr, KImgOrientation());

	// Set image and image frame temporarily in same orientation
	if(aFramePortrait != iImagePortrait)
		iImageFrame.Reverse();
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::GetImageName
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::ComposePositionL(TInt aPositionIndex, RArray<TSizeReal>& aPositions)
{
	TSizeReal position(0,0);
	if(aPositions.Count() <= aPositionIndex)
		aPositionIndex = 0;

	if(0 < aPositions.Count())
		position = aPositions[aPositionIndex];

	// Format position
	TXhtmlSetting  positionTopStr, positionLeftStr;

	FormatReal(PixelToUnit(position.iWidth), positionTopStr);
	ComposeTemplateL(positionTopStr, KPositionTop());

	FormatReal(PixelToUnit(position.iHeight), positionLeftStr);
	ComposeTemplateL(positionLeftStr, KPositionLeft());
}

//--------------------------------------------------------------------------------------------
//
// CXhtmlFileComposer::FormatReal
//
//--------------------------------------------------------------------------------------------
void CXhtmlFileComposer::FormatReal(TRealX aNumber, TDes8& aString)
{
	TRealFormat form;
	form.iPlaces = 4;
	form.iPoint = TChar('.');
	form.iType = KDoNotUseTriads | KRealFormatFixed;

	aString.Num(aNumber, form);
	aString.Append(KUnit());
}

// End of file
