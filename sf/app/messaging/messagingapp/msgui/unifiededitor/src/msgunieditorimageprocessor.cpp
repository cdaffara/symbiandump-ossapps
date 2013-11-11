/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       Unified Message Editor - Combined image scaler & compressor
*
*/



// INCLUDE FILES
#include "msgunieditorimageprocessor.h"

#include <imageconversion.h>
#include <bitmaptransforms.h>

#include <MsgMimeTypes.h>



// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// JPEG quality factor related constants:
const TInt KHighQualityMaxWidth = 640;
const TInt KHighQualityMaxHeight = 480;
const TInt KJpegQualityFactorHigh = 80;
const TInt KJpegQualityFactorNormal = 65;

// MPix factor is relative to this value
// 3MPix: Initial  factor 2/3 * Qfactor calculated above
// 5MPix: 2/5*
// 8MPix: 2/8*
const TReal  KQFMPixfactor( 1600*1200 );

// Count of compressing/scaling efforts
const TInt  KIterationCount( 3 );

const TInt KMPixfactorMin = 1;
const TInt KMPixfactorMax = 5;
const TInt KMPixSizeDivider = 4;
           
// When compressing next QFactor is QFactor * (value from table) / 100
const TInt KPercentCoefficientsCounts[KMPixfactorMax][KIterationCount] = 
        {  { 60, 30, 10 },
           { 40, 15, 3 },
           { 25, 10, 2 },
           { 12, 4,  0 },
           { 5, 2,  0 }
        };
        
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniImageProcessor::CUniImageProcessor
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CUniImageProcessor::CUniImageProcessor( MUniImageProcessorCallback* aCallback ) : 
    CActive( EPriorityStandard ),
    iCallback( aCallback ),
    iMPixFactor ( KMPixfactorMin )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CUniImageProcessor::~CUniImageProcessor
//
// Destructor
// ---------------------------------------------------------
//
CUniImageProcessor::~CUniImageProcessor()
    {
    Cancel();
    Reset();
    }

// ---------------------------------------------------------
// CUniImageProcessor::ProcessImageL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniImageProcessor::ProcessImageL(
    RFile& aSourceFile,
    RFile& aDestFile,
    TSize& aTargetSize,
    const TDesC8& aTargetType,
    TBool aAspectRatio,
    TInt aTargetFileSize )
    {
    
    
    if ( iDecoder || iScaler || iEncoder )
        {
        User::Leave( KErrInUse );
        }
    
    iError = KErrNone;
    iFlags &= ~ECompressOnly;
    if ( aTargetType.CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        iCompressTriesLeft = KIterationCount;
        User::LeaveIfError( aSourceFile.Size( iOriginalFileSize ) );
        // Make sure the resulting image won't be larger than the original
        if ( !aTargetFileSize || iOriginalFileSize < aTargetFileSize )
            {
            aTargetFileSize = iOriginalFileSize;
            iFlags |= EImplicitCompress;
            }
        iTargetFileSize = aTargetFileSize;
        }
    else
        {
        iCompressTriesLeft = 0;
        iTargetFileSize = 0;
        iOriginalFileSize = 0;
        }

    iQFactorCalculated = EFalse;
    iMPixFactor = KMPixfactorMin;
    iDestFile = &aDestFile;
    iAspectRatio = aAspectRatio;
    iDecoder = CImageDecoder::FileNewL( aSourceFile, ContentAccess::EPeek );
    iEncoder = CImageEncoder::FileNewL( aDestFile, aTargetType );

    const TFrameInfo& frameInfo = iDecoder->FrameInfo();

    TSize decodeSize( frameInfo.iOverallSizeInPixels );
    if ( !CalculateDecodeAndTargetSizes( aTargetSize, decodeSize, aAspectRatio ) )
        {
        // use bitmap plane scale
        iScaler = CBitmapScaler::NewL();
        iSourceBitmap = new ( ELeave ) CFbsBitmap;
        iFlags |= ESourceBitmapOwned;
        User::LeaveIfError( iSourceBitmap->Create(
            decodeSize,
            frameInfo.iFrameDisplayMode ) );
        }
    
    iDestBitmap = new ( ELeave ) CFbsBitmap;
    iFlags |= EDestBitmapOwned;
    User::LeaveIfError( iDestBitmap->Create(
        aTargetSize,
        frameInfo.iFrameDisplayMode ) );

    CreateImageDataL( aTargetType, aTargetSize );

    DecodeImage();
    }

// ---------------------------------------------------------
// CUniImageProcessor::ProcessImageL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniImageProcessor::ScaleImageL(
    CFbsBitmap* aSourceBitmap,
    RFile& aDestFile,
    const TSize& aTargetSize,
    const TDesC8& aTargetType,
    TBool aAspectRatio )
    {
    
    if ( iDecoder || iScaler || iEncoder )
        {
        User::Leave( KErrInUse );
        }

    iError = KErrNone;
    iFlags &= ~ECompressOnly;
    iCompressTriesLeft = 0;
    iTargetFileSize = 0;
    iOriginalFileSize = 0;

    iQFactorCalculated = EFalse;
    iMPixFactor = KMPixfactorMin;
    iDestFile = &aDestFile;
    iAspectRatio = aAspectRatio;
    iScaler = CBitmapScaler::NewL();
    iEncoder = CImageEncoder::FileNewL( aDestFile, aTargetType );

    iSourceBitmap = aSourceBitmap;

    iDestBitmap = new ( ELeave ) CFbsBitmap;
    iFlags |= EDestBitmapOwned;
    TInt error = iDestBitmap->Create(
        aTargetSize,
        iSourceBitmap->DisplayMode() );
    User::LeaveIfError( error );
    
    CreateImageDataL( aTargetType, aTargetSize );

    ScaleImage( EFalse );
    }

// ---------------------------------------------------------
// CUniImageProcessor::ScaleImageL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniImageProcessor::ScaleImageL(
    CFbsBitmap* aSourceBitmap,
    CFbsBitmap* aDestBitmap,
    TBool aAspectRatio )
    {
    
    if ( iDecoder || iScaler || iEncoder )
        {
        User::Leave( KErrInUse );
        }

    iError = KErrNone;
    iFlags &= ~ECompressOnly;
    iCompressTriesLeft = 0;
    iTargetFileSize = 0;
    iOriginalFileSize = 0;

    iAspectRatio = aAspectRatio;
    iScaler = CBitmapScaler::NewL();
    
    iSourceBitmap = aSourceBitmap;
    iDestBitmap = aDestBitmap;

    ScaleImage( EFalse );
    }

// ---------------------------------------------------------
// CUniImageProcessor::ScaleImageL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniImageProcessor::ScaleImageL(
    RFile& aSourceFile,
    CFbsBitmap*& aDestBitmap,
    CFbsBitmap*& aDestMask,
    TSize& aTargetSize,
    TBool aAspectRatio )
    {
    
    if ( iDecoder || iScaler || iEncoder )
        {
        User::Leave( KErrInUse );
        }

    iError = KErrNone;
    iFlags &= ~ECompressOnly;
    iCompressTriesLeft = 0;
    iTargetFileSize = 0;
    iOriginalFileSize = 0;

    iQFactorCalculated = EFalse;
    iMPixFactor = KMPixfactorMin;
    iAspectRatio = aAspectRatio;
    iDecoder = CImageDecoder::FileNewL( aSourceFile, ContentAccess::EPeek );

    const TFrameInfo& frameInfo = iDecoder->FrameInfo();

    TSize decodeSize( frameInfo.iOverallSizeInPixels );
    TBool optimum = CalculateDecodeAndTargetSizes( aTargetSize, decodeSize, aAspectRatio );
    // Make sure "compress only flag is not set".
    iFlags &= ~ECompressOnly;

    if ( !optimum )
        {
        // use bitmap plane scale
        iScaler = CBitmapScaler::NewL();
        iSourceBitmap = new ( ELeave ) CFbsBitmap;
        iFlags |= ESourceBitmapOwned;
        User::LeaveIfError( iSourceBitmap->Create(
            decodeSize,
            frameInfo.iFrameDisplayMode ) );
        }
    
    iDestBitmap = new ( ELeave ) CFbsBitmap;
    iFlags |= EDestBitmapOwned;
    User::LeaveIfError( iDestBitmap->Create(
        aTargetSize,
        frameInfo.iFrameDisplayMode ) );

    if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
        {
        if ( !optimum )
            {
            iSourceMask = new ( ELeave ) CFbsBitmap;
            iFlags |= ESourceMaskOwned;
            User::LeaveIfError( iSourceMask->Create(
                decodeSize,
                frameInfo.iFlags & TFrameInfo::EAlphaChannel ? EGray256 : EGray2 ) );
            }
        iDestMask = new ( ELeave ) CFbsBitmap;
        iFlags |= EDestMaskOwned;
        User::LeaveIfError( iDestMask->Create(
            aTargetSize,
            frameInfo.iFlags & TFrameInfo::EAlphaChannel ? EGray256 : EGray2 ) );
        delete aDestMask; // just in case
        aDestMask = iDestMask;
        }
    else
        {
        delete aDestMask; // just in case
        aDestMask = NULL;
        }

    delete aDestBitmap; //just in case
    aDestBitmap = iDestBitmap;
    
    iFlags &= ~EDestBitmapOwned;
    iFlags &= ~EDestMaskOwned;

    DecodeImage();
    }

// ---------------------------------------------------------
// CUniImageProcessor::CalculateDecodeAndTargetSizes
//
// ---------------------------------------------------------
//
TBool CUniImageProcessor::CalculateDecodeAndTargetSizes(
        TSize& aTargetSize,
        TSize& aDecodeSize,
        TBool aAspectRatio )
    {
    

    const TFrameInfo& frameInfo = iDecoder->FrameInfo();
    aDecodeSize = frameInfo.iOverallSizeInPixels;

    if ( aTargetSize.iWidth >= aDecodeSize.iWidth &&
         aTargetSize.iHeight >= aDecodeSize.iHeight &&
         !( iFlags & EAllowScalingUp ) )
        {
        // No need to scale at all!
        iFlags |= ECompressOnly;
        aTargetSize = aDecodeSize;
        return ETrue;
        }
    if ( aTargetSize == aDecodeSize )
        {
        // No need to scale at all!
        iFlags |= ECompressOnly;
        return ETrue;
        }

    if ( aAspectRatio )
        {
        // Adjust target size if aspect ratio needs to be maintained.
        TReal scaleRatio = CalculateScaleRatio( aTargetSize, aDecodeSize );
        aTargetSize.iHeight = scaleRatio * aDecodeSize.iHeight;
        aTargetSize.iWidth = scaleRatio * aDecodeSize.iWidth;
        }

    // Width or height should never be zero.
    aTargetSize.iWidth = Max(aTargetSize.iWidth, 1);
    aTargetSize.iHeight = Max(aTargetSize.iHeight, 1);

    TInt wRatio = aDecodeSize.iWidth / aTargetSize.iWidth;
    TInt hRatio = aDecodeSize.iHeight / aTargetSize.iHeight;

    TBool decodeOnly( frameInfo.iFlags & TFrameInfo::EFullyScaleable );

    if ( !decodeOnly &&
        !( aDecodeSize.iWidth % aTargetSize.iWidth ||
           aDecodeSize.iHeight % aTargetSize.iHeight ||
           wRatio != hRatio ||
          (wRatio != 2 && wRatio != 4 && wRatio != 8) ||
          (hRatio != 2 && hRatio != 4 && hRatio != 8) ) )
        {
        decodeOnly = ETrue;
        }

    if ( decodeOnly )
        {
        // Decoding directly to requested target size.
        aDecodeSize = aTargetSize;
        }
    else
        {
        TInt decodeFactor = 8;
        while ( decodeFactor > 1 )
            {
            if ( aDecodeSize.iWidth / decodeFactor >= aTargetSize.iWidth &&
                aDecodeSize.iHeight / decodeFactor >= aTargetSize.iHeight )
                {
                // Smallest 1/2^n factor that creates image larger than target size
                break;
                }
            decodeFactor /= 2;
            }
        if ( ( iFlags & EOnlyDecodeTimeScaling ) && decodeFactor < 8 )
            {
            // Largest 1/2^n factor that creates image smaller than target size
            decodeFactor *= 2;
            decodeOnly = ETrue;
            }
        aDecodeSize.iWidth = ( aDecodeSize.iWidth + decodeFactor - 1 ) / decodeFactor;
        aDecodeSize.iHeight = ( aDecodeSize.iHeight + decodeFactor - 1 ) / decodeFactor;
        if ( decodeOnly )
            {
            // Decoding to target size. Make sure target & decode sizes match.
            // Target size may need to be changed was iOnlyDecodeTimeScaling is used.
            aTargetSize = aDecodeSize;
            }
        }
    return decodeOnly;
    }


// ---------------------------------------------------------
// CUniImageProcessor::DecodeImage
//
// ---------------------------------------------------------
//
void CUniImageProcessor::DecodeImage()
    {
    
    
    iStatus = KRequestPending;
    if ( iScaler )
        {
        if ( iSourceMask )
            {
            iDecoder->Convert( &iStatus, *iSourceBitmap, *iSourceMask );
            }
        else
            {
            iDecoder->Convert( &iStatus, *iSourceBitmap );
            }
        }
    else
        {
        if ( iDestMask )
            {
            iDecoder->Convert( &iStatus, *iDestBitmap, *iDestMask );
            }
        else
            {
            iDecoder->Convert( &iStatus, *iDestBitmap );
            }
        }
    SetActive();
    }

// ---------------------------------------------------------
// CUniImageProcessor::ScaleImage
//
// ---------------------------------------------------------
//
void CUniImageProcessor::ScaleImage( TBool aMask )
    {
    
    
    iStatus = KRequestPending;
    if ( aMask )
        {
        iScaler->Scale( &iStatus, *iSourceMask, *iDestMask, iAspectRatio );
        }
    else
        {
        iScaler->Scale( &iStatus, *iSourceBitmap, *iDestBitmap, iAspectRatio );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CUniImageProcessor::EncodeImageL
//
// ---------------------------------------------------------
//
void CUniImageProcessor::EncodeImageL( TBool aCompressing )
    {
    

    if ( aCompressing )
        {
        delete iEncoder;
        iEncoder = NULL;
        // Make sure that the file is empty.
        User::LeaveIfError( iDestFile->SetSize( 0 ) );
        iEncoder = CImageEncoder::FileNewL( *iDestFile, KMsgMimeImageJpeg );
        }
    iCompressTriesLeft--;
    iStatus = KRequestPending;
    iEncoder->Convert( &iStatus, *iDestBitmap, iFrameImageData );
    SetActive();
    }

// ---------------------------------------------------------
// CUniImageProcessor::DoCancel
//
// ---------------------------------------------------------
//
void CUniImageProcessor::DoCancel()
    {

    if ( iDecoder )
        {
        iDecoder->Cancel();
        }
    if ( iScaler )
        {
        iScaler->Cancel();
        }
    if ( iEncoder )
        {
        iEncoder->Cancel();
        }
    
    iError = KErrCancel;    
    iCallback->ImageProcessingReady( TSize(), 0, EFalse );   
    }

// ---------------------------------------------------------
// CUniImageProcessor::RunL
//
// ---------------------------------------------------------
//
void CUniImageProcessor::RunL()
    {
    TSize size( 0, 0 );
    if ( iStatus < 0 )
        {
        iError = iStatus.Int();
        }
    if ( iError )
        {
        TBool compressed( EFalse );
        if ( iTargetFileSize &&
            iCompressTriesLeft != KIterationCount &&
            !( iFlags & EImplicitCompress ) )
            {
            compressed = ETrue;
            }
        Reset();
        iCallback->ImageProcessingReady( size, 0, compressed );
        }
    else if ( iDecoder )
        {
        delete iDecoder;
        iDecoder = NULL;
        if ( iScaler )
            {
            if ( iSourceMask )
                {
                //scale mask first
                ScaleImage( ETrue );
                }
            else
                {
                ScaleImage( EFalse );
                }
            }
        else if ( iEncoder )
            {
            if ( iFlags & ECompressOnly )
                {
                ResolveQFactorL();
                }
            // In "compress only" case the "aCompressing" flag is
            // EFalse for first time because we do not want
            // to delete & recreate CImageEncoder unnecessarily.
            EncodeImageL( EFalse );
            }
        else
            {
            size = iDestBitmap->SizeInPixels();
            Reset();
            iCallback->ImageProcessingReady( size, iResultFileSize, EFalse );
            }
        }
    else if ( iScaler )
        {
        if ( iSourceMask )
            {
            delete iSourceMask;
            iSourceMask = NULL;
            iFlags &= ~ESourceMaskOwned;
            //mask already scaled, scale bitmap
            ScaleImage( EFalse );
            }
        else
            {
            delete iScaler;
            iScaler = NULL;
            if ( iEncoder )
                {
                EncodeImageL( EFalse );
                }
            else
                {
                size = iDestBitmap->SizeInPixels();
                Reset();
                iCallback->ImageProcessingReady( size, iResultFileSize, EFalse );
                }
            }
        }
    else 
        {
        if ( CheckEncodedFileSize() || 
             ( iCompressTriesLeft <= 0 ) )
            {
            size = iDestBitmap->SizeInPixels();
            TBool compressed( EFalse );
            if ( iTargetFileSize &&
                iCompressTriesLeft != KIterationCount &&
                !( iFlags & EImplicitCompress ) )
                {
                compressed = ETrue;
                }
            Reset();
            iCallback->ImageProcessingReady( size, iResultFileSize, compressed );
            }
        else
            {
            ResolveQFactorL();
            EncodeImageL( ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CUniImageProcessor::RunError
//
// ---------------------------------------------------------
//
TInt CUniImageProcessor::RunError( TInt aError )
    {
    iError = aError;
    Cancel();
    //Complete self.
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniImageProcessor::Reset
//
// ---------------------------------------------------------
//
EXPORT_C void CUniImageProcessor::Reset()
    {
    iDestFile = NULL;
    
    delete iDecoder;
    iDecoder = NULL;
    
    delete iScaler;
    iScaler = NULL;
    
    delete iEncoder;
    iEncoder = NULL;
    
    delete iFrameImageData;
    iFrameImageData = NULL;
    
    if ( iFlags & ESourceBitmapOwned )
        {
        delete iSourceBitmap;
        }
        
    iSourceBitmap = NULL;
    iFlags &= ~ESourceBitmapOwned;
    
    if ( iFlags & EDestBitmapOwned )
        {
        delete iDestBitmap;
        }
    
    iDestBitmap = NULL;
    iFlags &= ~EDestBitmapOwned;
    
    if ( iFlags & ESourceMaskOwned )
        {
        delete iSourceMask;
        }
    
    iSourceMask = NULL;
    iFlags &= ~ESourceMaskOwned;
    
    if ( iFlags & EDestMaskOwned )
        {
        delete iDestMask;
        }
    
    iDestMask = NULL;
    iFlags &= ~EDestMaskOwned;
    }


// ---------------------------------------------------------
// CUniImageProcessor::CreateImageDataL
//
// ---------------------------------------------------------
//
void CUniImageProcessor::CreateImageDataL( const TDesC8& aTargetType, const TSize& aTargetSize )
    {    
    if ( aTargetType.CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        TInt targetQF( KJpegQualityFactorNormal );
        if ( aTargetSize.iWidth <= KHighQualityMaxWidth &&
            aTargetSize.iHeight <= KHighQualityMaxHeight )
            {
            //Use higher quality factor for small images
            targetQF = KJpegQualityFactorHigh;
            }

        // Use original image data if available
        if ( iDecoder )
            {
            iFrameImageData = iDecoder->FrameData().AllocL();
            
            TInt imageDataCount = iFrameImageData->ImageDataCount();
            while ( imageDataCount-- )
                {
                TImageDataBlock* data = iFrameImageData->GetImageData( imageDataCount );
                const TUid type = data->DataType();
                if ( type == KJPGQTableUid )
                    {
                    TJpegQTable* qdata = static_cast<TJpegQTable*>( data );
                    if ( qdata->iTableIndex != TJpegQTable::ELumaTable &&
                        qdata->iTableIndex != TJpegQTable::EChromaTable )
                        {
                        // Jpeg encoder supports only two QTables. Others (probably
                        // another chroma table) must be removed. Otherwise Jpeg
                        // encoder leaves.
                        iFrameImageData->RemoveImageData( imageDataCount );
                        delete qdata;
                        }
                    }
                else if ( type == KJPGImageDataUid )
                    {
                    iJpegImageData = static_cast<TJpegImageData*>( data );
                    // iSourceBitmap is NULL, if image is in right size
                    iOriginalQualityFactor = iJpegImageData->iQualityFactor;
                    if ( iSourceBitmap )
                        {
                        CalculateMPixfactorL();
                        iJpegImageData->iQualityFactor = iOriginalQualityFactor / iMPixFactor ;
                        iQFactorCalculated = ETrue;
                        }                    
                    }
                }                
            }
        if ( !iJpegImageData )
            {
            // Delete possibly existing iFrameImageData. There maybe one at least
            // when converting PNG images to JPEG.
            delete iFrameImageData;
            iFrameImageData = NULL;
            iFrameImageData = CFrameImageData::NewL();
            TJpegImageData* jpegImageData = new ( ELeave ) TJpegImageData;
            CleanupStack::PushL( jpegImageData );
            jpegImageData->iSampleScheme = TJpegImageData::EColor420;

            iOriginalQualityFactor = targetQF;
            if ( iSourceBitmap )
                {
                iMPixFactor  = Max ( KMPixfactorMin, 
                    (   static_cast<TReal>( iSourceBitmap->SizeInPixels().iWidth ) * 
                        static_cast<TReal>( iSourceBitmap->SizeInPixels().iHeight ) ) / KQFMPixfactor );
                iMPixFactor = Min( iMPixFactor, KMPixfactorMax );
                jpegImageData->iQualityFactor = iOriginalQualityFactor / iMPixFactor ;
                iQFactorCalculated = ETrue;
                }
            else
                {
                jpegImageData->iQualityFactor = iOriginalQualityFactor ;
                }
            User::LeaveIfError( iFrameImageData->AppendImageData( jpegImageData ) );
            iJpegImageData = jpegImageData;
            CleanupStack::Pop(); // jpegFormat, ownership transferred
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CUniImageProcessor::CalculateScaleRatio
//
// Calculates correct scaling ratio when aspect ratio needs to be preserved.
// Correct scaling ratio is the smaller of the width and height original image
// size and target image size ratios.
// ----------------------------------------------------------------------------
//
TReal CUniImageProcessor::CalculateScaleRatio( const TSize& aTargetSize,
                                            const TSize& aOriginalSize ) const
    {
    TReal widthRatio = static_cast<TReal>( aTargetSize.iWidth ) / 
                       static_cast<TReal>( aOriginalSize.iWidth );
	TReal heightRatio = static_cast<TReal>( aTargetSize.iHeight ) / 
	                    static_cast<TReal>( aOriginalSize.iHeight );
	return ( widthRatio < heightRatio ) ? widthRatio : heightRatio;
    }

// ---------------------------------------------------------
// CUniImageProcessor::ResolveQFactorL
//
// ---------------------------------------------------------
//
void CUniImageProcessor::ResolveQFactorL()
    {
    
    // Leave if "JPGImageData" was not found.
    if ( !iJpegImageData )
        {
        User::Leave( KErrNotSupported );
        }

    if ( !iQFactorCalculated ) // First try
        {
        if ( iSourceBitmap )
            {
            CalculateMPixfactorL();
            iJpegImageData->iQualityFactor = iOriginalQualityFactor / iMPixFactor ;
            }
        else
            {
            iJpegImageData->iQualityFactor = iOriginalQualityFactor * 
                KPercentCoefficientsCounts[ iMPixFactor - 1 ][Min( KIterationCount-iCompressTriesLeft, KIterationCount )] / 100;
                
            }
        iQFactorCalculated = ETrue;
        }
    else
        {
        iJpegImageData->iQualityFactor = iOriginalQualityFactor * 
            KPercentCoefficientsCounts[ iMPixFactor - 1 ][Min( KIterationCount-iCompressTriesLeft, KIterationCount )] / 100;
        }
    
    if ( iJpegImageData->iQualityFactor <= 0 )
        {
        iJpegImageData->iQualityFactor = 0;
        iCompressTriesLeft = 0; // No use to iterate more than this.
        }
    }

// ---------------------------------------------------------
// CUniImageProcessor::CheckEncodedFileSize
//
// ---------------------------------------------------------
//
TBool CUniImageProcessor::CheckEncodedFileSize()
    {
    TBool retVal = ETrue;

    // Get encoded file size. 
    // Store possible error to "iError".
    iError = iDestFile->Size( iResultFileSize );
    
    if ( iTargetFileSize )
        {
        // Compression requested. Check whether the size is small enough.
        retVal = ( iResultFileSize <= iTargetFileSize );
        }
    return retVal;
    }

// ---------------------------------------------------------
// CUniImageProcessor::CalculateMPixfactorL
//
// ---------------------------------------------------------
//
void CUniImageProcessor::CalculateMPixfactorL()
    {
    if ( iSourceBitmap )
        {
        iMPixFactor  = Max ( KMPixfactorMin, 
            (   static_cast<TReal>( iSourceBitmap->SizeInPixels().iWidth ) * 
                static_cast<TReal>( iSourceBitmap->SizeInPixels().iHeight ) ) / KQFMPixfactor );
        }
    if (iMPixFactor == KMPixfactorMin )
        {
        // Sometimes on the emulator bitmap dimensions show completely wrong values.
        // Whether this happens also on the device is unknown
        // Just in case use files size as second option
        // As image size changes more rapidly as image dimension use extra divider
        if ( iTargetFileSize >= 0 )
            {
            iMPixFactor  = Max( KMPixfactorMin, ( iOriginalFileSize /  iTargetFileSize ) / KMPixSizeDivider );
            }
        }            
    iMPixFactor = Min( iMPixFactor, KMPixfactorMax );
    }

//  End of File
