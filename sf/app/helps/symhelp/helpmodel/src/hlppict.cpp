// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Help Picture module
// 
//

#include "hlppict.h"

// User includes
#include "hlppanic.h"
#include "HLPDB.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "hlpmodel_internal.h"
#endif

// System includes
#include <fbs.h>
#include <coemain.h>


//
// ----> CHlpPicture (source)
//

CHlpPicture::CHlpPicture(CHlpDatabase& aDatabase, CHlpModel& aHlpModel)
	:iDatabase(aDatabase), iHlpModel(&aHlpModel)
	{
	}

CHlpPicture::~CHlpPicture()
	{
	delete iImage;

	if (iHlpModel)
	iHlpModel->RemoveHelpPicture(this);
	}

void CHlpPicture::ConstructL(const CStreamStore& aStore, TStreamId aStreamId)
	{
	// Just restore for now
	__ASSERT_ALWAYS(iHlpModel, Panic(EHlpNoHelpModelPointer));
	const THlpZoomState aZoomState = iHlpModel->ZoomSize();
	RestoreL(aStore, aStreamId, iDatabase, aZoomState);
	}

/** Constructs a new CHlpPicture object and returns a pointer to it, 
pushing it onto the cleanup stack.

@param aStore A reference to a stream store.
@param aStreamId The ID of the stream to be used.
@param aDatabase A reference to a CHlpDatabase object.
@param aHlpModel A reference to a CHlpModel object.
@return A pointer to a CHlpPicture object.
@pre Valid stream store, stream ID, database and Hlpmodel 
objects are passed to the function.
@post A CHlpPicture object has been created and has been pushed 
onto the cleanup stack. */
CHlpPicture* CHlpPicture::NewLC(const CStreamStore& aStore, TStreamId aStreamId, CHlpDatabase& aDatabase, CHlpModel& aHlpModel)
	{
	CHlpPicture* picture = new(ELeave) CHlpPicture(aDatabase, aHlpModel);
	CleanupStack::PushL(picture);
	picture->ConstructL(aStore, aStreamId);
	return picture;
	}

//
//
//

void CHlpPicture::Draw(CGraphicsContext& aGc, const TPoint& aTopLeft, const TRect& aClipRect, MGraphicsDeviceMap* aMap) const
	{
	if	(iImageCountForPicture != KHlpModelMaximumNumberOfImagesForV6Point2Files)
		{
		// If there is only one image to represent this image, then we revert to using
		// the scaling code, as per v6.0 and 6.1
		TSize size;
		GetSizeInPixels(aMap, size);
		TRect destRect(aTopLeft, size);

		TMargins cropMargins;
		GetCropInTwips(cropMargins);
		TSize originalSize;
		GetOriginalSizeInTwips(originalSize);

		TRect sourceRect(TPoint(cropMargins.iLeft,cropMargins.iTop), TPoint(originalSize.iWidth-cropMargins.iRight,originalSize.iHeight-cropMargins.iBottom));
		sourceRect = CCoeEnv::Static()->ScreenDevice()->TwipsToPixels(sourceRect);
		aGc.SetClippingRect(aClipRect);
		aGc.DrawBitmap(destRect, iImage, sourceRect);
		}
	else
		{
		// There are (by default anyway) 3 images available for use at various zoom states,
		// hence we must ensure that we don't scale the bitmap.
		TMargins cropMargins;
		GetCropInTwips(cropMargins);

		TRect sourceRect;
		TRect destRect;

		TSize size(iImage->SizeInPixels());
		destRect = TRect(aTopLeft, size);
		sourceRect = TRect(TPoint(cropMargins.iLeft, cropMargins.iTop), TPoint(size.iWidth - cropMargins.iRight, size.iHeight - cropMargins.iBottom));

		aGc.SetClippingRect(aClipRect);
		aGc.DrawBitmap(destRect, iImage, sourceRect);
		}
	}

void CHlpPicture::ExternalizeL(RWriteStream& aStream) const
 	{
	aStream.WriteUint32L(iImageId);
 	}
 
void CHlpPicture::GetCropInTwips(TMargins& aMargins) const
	{
	aMargins.iLeft = aMargins.iRight = aMargins.iTop = aMargins.iBottom = 0;
	}

/** Converts a bitmap size to twips before any scaling is applied.

@param aSize The size of the bitmap rectangle in pixels.
@pre None.
@post The bitmap rectangle size is expressed in twips. */
void CHlpPicture::GetOriginalSizeInTwips(TSize& aSize) const
	{
	__ASSERT_ALWAYS(iHlpModel, Panic(EHlpNoHelpModelPointer));
	aSize = iImage->SizeInPixels();

	if	(iImageCountForPicture != KHlpModelMaximumNumberOfImagesForV6Point2Files)
		{
		// ER6 and 6.1 scale-based code
		TRect rect(aSize);
		rect = CCoeEnv::Static()->ScreenDevice()->PixelsToTwips(rect);
		aSize = rect.Size();
		}
	else
		{
		// Fetch the zoom 
		const TReal zoomValue = static_cast<TReal>(iHlpModel->CurrentZoomFactor());
		const TReal scaleFactor = static_cast<TReal>(KHlpModelDefaultZoomFactorAsRealNumber / zoomValue);

		// Scale up or down (in pixels) accordingly
		aSize.iWidth = static_cast<TInt>(aSize.iWidth * scaleFactor);
		aSize.iHeight = static_cast<TInt>(aSize.iHeight * scaleFactor);

		// Now scale into twips
		aSize = CCoeEnv::Static()->ScreenDevice()->PixelsToTwips(TRect(TPoint(0,0), aSize)).Size();
		}
	}

void CHlpPicture::HandleHelpModelDestruction()
	{
	iHlpModel = NULL;
	}

void CHlpPicture::SetScaleFactor(TInt /*aScaleFactorWidth*/, TInt /*aScaleFactorHeight*/)
	{
	}

void CHlpPicture::SetCropInTwips(const TMargins& /*aMargins*/)
	{
	}

TInt CHlpPicture::ScaleFactorWidth() const
	{
	return KHlpModelZoomFactorMedium;
	}

TInt CHlpPicture::ScaleFactorHeight() const
	{
	return KHlpModelZoomFactorMedium;
	}

//
//
//

/** Replaces any currently used bitmap with the bitmap that 
corresponds to the new zoom size.

@param aNewZoom The new zoom size.
@pre A valid zoom size is passed to the function.
@post All currently used bitmaps are replaced with bitmaps 
that correspond to the new zoom size. */
void CHlpPicture::HandleZoomChangedL(THlpZoomState aNewZoom)
	{
	CFbsBitmap* bitmap = iDatabase.ImageForIdLC(iImageId, aNewZoom);
	delete iImage;
	iImage = bitmap;
	CleanupStack::Pop();
	}

//
//
//

/** Replaces any currently used bitmap with the bitmap 
that corresponds to the new zoom size.

@param aNewZoom The new zoom size.
@pre A valid zoom size is passed to the function.
@post All currently used bitmaps are replaced with bitmaps 
that correspond to the new zoom size.

@see CHlpDatabase::ImageForIdLC()
@see CHlpDatabase::ImageCountForIdL()
@see RStoreReadStream::OpenLC() */
void CHlpPicture::RestoreL(const CStreamStore& aStore, TStreamId aStreamId, CHlpDatabase& aDatabase, THlpZoomState aZoomState)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore, aStreamId);
	iImageId = stream.ReadUint32L();
	CleanupStack::PopAndDestroy(); // stream
	iImageCountForPicture = aDatabase.ImageCountForIdL(iImageId);

	// Fetch the specified bitmap from the help files image table.
	// This method will leave if the image isn't in the table.
	CFbsBitmap* bitmap = aDatabase.ImageForIdLC(iImageId, aZoomState);
	delete iImage;
	iImage = bitmap;
	CleanupStack::Pop(); // bitmap
	}





