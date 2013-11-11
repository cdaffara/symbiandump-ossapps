/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/

#include "glximagedecoder.h"
#include "glximagedecoderwrapper.h"

#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <IclExtJpegApi.h>  // For CExtJpegDecoder
#include <e32math.h>
#include <apgcli.h>

_LIT(KMimeJpeg,"image/jpeg");
_LIT(KMimeJpg,"image/jpg");

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxImageDecoder* CGlxImageDecoder::NewL(GlxImageDecoderWrapper* decoderWrapper)
{
	CGlxImageDecoder* self = new(ELeave)CGlxImageDecoder();
    CleanupStack::PushL(self);
    self->ConstructL(decoderWrapper);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxImageDecoder::CGlxImageDecoder()
                         :CActive( EPriorityStandard )
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

CGlxImageDecoder::~CGlxImageDecoder()
{
    iFs.Close();
    Cancel();
    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	if(iBitmap) {
		delete iBitmap;
		iBitmap = NULL;
	}
}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CGlxImageDecoder::ConstructL(GlxImageDecoderWrapper* decoderWrapper)
{
    iDecoderWrapper = decoderWrapper;
    User::LeaveIfError(iFs.Connect());
    CActiveScheduler::Add( this );
}

// ---------------------------------------------------------------------------
// DoDecodeImageL
// ---------------------------------------------------------------------------	
QSizeF CGlxImageDecoder::DecodeImageL(QString aSourceFileName)
{
	//convert the argument to Symbian Format
	TPtrC16 sourceFileName(reinterpret_cast<const TUint16*>(aSourceFileName.utf16()));
	if (iImageDecoder)
        {
		iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	TRAPD( err, iImageDecoder = CExtJpegDecoder::FileNewL(
            CExtJpegDecoder::EHwImplementation, iFs, sourceFileName, CImageDecoder::EOptionAlwaysThread ) );
    if ( KErrNone != err )
        {
        TRAP(err,iImageDecoder = CExtJpegDecoder::FileNewL(
                CExtJpegDecoder::ESwImplementation, iFs, sourceFileName, CImageDecoder::EOptionAlwaysThread ) );
        if ( KErrNone != err )
            {
            // Not a JPEG - use standard decoder
            iImageDecoder = CImageDecoder::FileNewL( iFs, sourceFileName, CImageDecoder::EOptionAlwaysThread );
            }
        }
	TSize imageSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
	//limit size to 1MP
	TSize decodeSize = imageSize;
	TReal pixelsInImage = imageSize.iWidth*imageSize.iHeight;
	if(pixelsInImage > KTargetSize)
	{
		TReal compressionFactor = 1;
		TInt err = Math::Sqrt(compressionFactor, (KTargetSize/pixelsInImage) );
		if(err != KErrNone) 
		{
			compressionFactor = .1;
		}

		decodeSize = TSize(imageSize.iWidth * compressionFactor, imageSize.iHeight * compressionFactor);
	}
	//if an image is converted to Pixmap with any of its dimension > 2048 
	//the conversion will fail so limiting dimensions to 2000
	//on 2048 there is a buffer corruption so display image is distorted  
	if(decodeSize.iWidth > KMaxDimensionLimit ||decodeSize.iHeight > KMaxDimensionLimit)
    {
        QSize finalSize(decodeSize.iWidth, decodeSize.iHeight);
        finalSize.scale(KMaxDimensionLimit, KMaxDimensionLimit, Qt::KeepAspectRatio);
        decodeSize = TSize(finalSize.width(), finalSize.height());
    }
	//clear the existing Bitmap
	if(iBitmap)
	{
		delete iBitmap;
		iBitmap = NULL;
	}
	// create the destination bitmap and pass it on to the decoder
    if(!iBitmap)
        {
        iBitmap = new (ELeave) CFbsBitmap();
        decodeSize = ReCalculateSizeL(aSourceFileName, decodeSize);
        iBitmap->Create( decodeSize,EColor16MU);
        iImageDecoder->Convert( &iStatus, *iBitmap );
		SetActive();
		}
	return QSizeF(decodeSize.iWidth,decodeSize.iHeight) ;
}

// -----------------------------------------------------------------------------
// DoesMimeTypeNeedsRecalculateL()
// -----------------------------------------------------------------------------
//
TBool CGlxImageDecoder::DoesMimeTypeNeedsRecalculateL(QString aSourceFileName){
    RApaLsSession session;
    TDataType mimeType;
    TUid uid;
    
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    TPtrC16 sourceFileName(reinterpret_cast<const TUint16*>(aSourceFileName.utf16()));
    User::LeaveIfError( session.AppForDocument( sourceFileName, uid, mimeType ) );
    CleanupStack::PopAndDestroy(&session);
    
    if (mimeType.Des().Compare(KMimeJpeg)==0 ||
            mimeType.Des().Compare(KMimeJpg)==0){
        return EFalse;
        }
    else{
        return ETrue; 
        }
    }

// -----------------------------------------------------------------------------
// ReCalculateSize 
// -----------------------------------------------------------------------------
TSize CGlxImageDecoder::ReCalculateSizeL(QString aSourceFileName, TSize aDestSize){
    if(DoesMimeTypeNeedsRecalculateL(aSourceFileName)){
        TSize fullFrameSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
        // calculate the reduction factor on what size we need
        TInt reductionFactor = iImageDecoder->ReductionFactor(fullFrameSize, aDestSize);
        // get the reduced size onto destination size
        TSize destSize;
        User::LeaveIfError(iImageDecoder->ReducedSize(fullFrameSize, reductionFactor, destSize));
        return destSize;
        }
    else{
        return aDestSize;
        }
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CGlxImageDecoder::RunL()
    {
    if( iStatus == KErrNone )
        {   
        iDecoderWrapper->decodedImageAvailable();
        }	
     delete iImageDecoder;
     iImageDecoder = NULL;
     iFs.Close();
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxImageDecoder::DoCancel()
    {
    if(iImageDecoder)
        {
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
	iFs.Close();
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
	//todo add cleanup logic for pixmap
    }

// ---------------------------------------------------------------------------
// GetPixmap
// ---------------------------------------------------------------------------
//
QPixmap CGlxImageDecoder::GetPixmap()
	{
	if(iBitmap)
		{
		iDecodedPixmap = QPixmap::fromSymbianCFbsBitmap(iBitmap);
		delete iBitmap;
		iBitmap = NULL;
		}
	return iDecodedPixmap;
	}

// ---------------------------------------------------------------------------
// ResetDecoder
// ---------------------------------------------------------------------------
//

void CGlxImageDecoder::ResetDecoder()
{
	Cancel();
	if(iBitmap) {
		delete iBitmap;
		iBitmap = NULL;
	}
	iDecodedPixmap = QPixmap();
}

