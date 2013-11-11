/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES
#include <imageconversion.h>
#include <cvtimageconverter.h>
#include <rphcltserver.h>
#include <cphcltimagehandler.h>
#include <bautils.h>

#include "cmultiframeprovider.h"
#include "cmultiframeloopao.h"
#include "cmultiframedataao.h"

#include "CVSStillImageDataProvider.h"
#include "cvtimagebitmap.h"

static const TInt KQcifWidth = 176;
static const TInt KQcifHeight = 144;

// Default bitmap size is VGA
static const TInt KDefaultBitmapSizeWidth = 640;
static const TInt KDefaultBitmapSizeHeight = 480;

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ TScalingMediator ===============================

// -----------------------------------------------------------------------------
// TScalingMediator::TScalingMediator( )
// -----------------------------------------------------------------------------
//
TScalingMediator::TScalingMediator()
    {
    }

// -----------------------------------------------------------------------------
// TScalingMediator::ScalingFinished( )
// -----------------------------------------------------------------------------
//
void TScalingMediator::ScalingFinished( TInt aError )
		{
		__IF_DEBUG(Print(_L("TScalingMediator::ScalingFinished() >>")));
    iWait->Signal( aError );
   	__IF_DEBUG(Print(_L("TScalingMediator::ScalingFinished() error %d <<"), aError));
		}
// -----------------------------------------------------------------------------
// TScalingMediator::ScalingFinished( )
// -----------------------------------------------------------------------------
//
void TScalingMediator::SetWaiter( CVSActiveWait< CMultiframeProvider >* aWait )
			{
			iWait = aWait;
			}

// ============================ CMultiframeProvider ===============================

// -----------------------------------------------------------------------------
// CMultiframeProvider::NewL( )
// -----------------------------------------------------------------------------
//
CMultiframeProvider* CMultiframeProvider::NewL( MMultiframeprovider* aObserver, TDisplayMode aDisplayMode )
    {
    __IF_DEBUG(Print(_L("VideoSource: [%d] CMultiframeProvider::NewL() >>"), RThread().Id().operator TUint()));
    CMultiframeProvider* self =
    new (ELeave) CMultiframeProvider( aObserver, aDisplayMode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource: [%d] CMultiframeProvider::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }
// -----------------------------------------------------------------------------
// CMultiframeProvider::CMultiframeProvider(
//  MVSDataProviderObserver* aObserver )
// -----------------------------------------------------------------------------
//
CMultiframeProvider::CMultiframeProvider(
    MMultiframeprovider* aObserver , TDisplayMode aDisplayMode ): iDisplayMode( aDisplayMode ), iObserver( aObserver ), iBlank( EFalse ), iGSImage( EFalse )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::CMultiframeProvider() >>"), RThread().Id().operator TUint()));
    //Set target VF size QCIF as default
    iTargetSize.SetSize( KQcifWidth , KQcifHeight );
    iVFSize.SetSize( KQcifWidth , KQcifHeight );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::CMultiframeProvider() <<"), RThread().Id().operator TUint()));
    }
// -----------------------------------------------------------------------------
// CMultiframeProvider::~CMultiframeProvider()
// -----------------------------------------------------------------------------
//
CMultiframeProvider::~CMultiframeProvider()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::~CMultiframeProvider() >>"), RThread().Id().operator TUint()));
    iFs.Close();
    iFreezeCS.Close();
    delete iImageDecoder; iImageDecoder = 0;
    delete iVtImageScalerInit;
    delete iVtImageScalerScale;
    delete iActiveWaitInit;
    delete iActiveWaitScale;
    delete iActiveWaitInitScale;
    delete iConverted;
    delete iMask;
    delete iTempBM;
    delete iDataTimer;
    delete iLoopAo;
    if ( iScaled != iBuffer1 && iScaled != iBuffer2 )
        {
    	  delete iScaled;
        }
    delete iGSBitmap;
    if ( !iBlank || !iGSImage )
    	  {
    	  delete iBuffer1;
		    delete iBuffer2;
    	  }
    delete iFirstVfBuf;
    delete iSecondVfBuf;
    iFreeQueue.Close();
    iDefaultImageFile.Close();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::~CMultiframeProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::ConstructL()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ConstructL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConstructL() >>"), RThread().Id().operator TUint()));
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFreezeCS.CreateLocal() );

    // Create waiters and mediators for scaling purposes
    iActiveWaitInit = new (ELeave) CVSActiveWait< CMultiframeProvider >( this );
    iActiveWaitScale = new (ELeave) CVSActiveWait< CMultiframeProvider >( this );
    iActiveWaitInitScale = new (ELeave) CVSActiveWait< CMultiframeProvider >( this );
    iInitMediator.SetWaiter( iActiveWaitInitScale );
    iScalingMediator.SetWaiter( iActiveWaitScale );
    iVtImageScalerInit = CVtImageScaler::NewL( iInitMediator, CVtImageScaler::EBilinear );
    iVtImageScalerScale = CVtImageScaler::NewL( iScalingMediator, CVtImageScaler::EBilinear );
		iDataTimer = new (ELeave) CMultiframeDataAO( this );
		iDataTimer->SetObserver( iObserver );
    iLoopAo = new (ELeave) CMultiframeloopAO( this );
    iFirstVfBuf = CVtImageBitmap::NewL( TSize( KQcifWidth >> 2, KQcifHeight >> 2 ), iDisplayMode );
    iSecondVfBuf = CVtImageBitmap::NewL( TSize( KQcifWidth >> 2, KQcifHeight >> 2 ), iDisplayMode );
    iSecondVFBuffer = EFalse;
    iFirstVFBuffer = ETrue;
    iIsVfStopped = EFalse;
    iWaitForConverting = EFalse;
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConstructL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::IniatializeGSL()
//
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::IniatializeGSL( const TGeneralSettingsImageType aType )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::IniatializeGSL() >>"), RThread().Id().operator TUint()));
	  if ( aType == EGSStillImage )
        {
        // Open connection to phone server
        RPhCltServer phoneClient;
        User::LeaveIfError( phoneClient.Connect() );
        CleanupClosePushL( phoneClient );
    
    	  CPhCltImageHandler* stillH = CPhCltImageHandler::NewL();
        CleanupStack::PushL( stillH );
    
    	  // Load images
    	  CPhCltImageParams* imageParams;
        imageParams = stillH->CPhCltBaseImageParamsL( EPhCltTypeVTStill );
        CleanupStack::PushL( imageParams );
        User::LeaveIfError( stillH->LoadImages( imageParams ) );
    
        // Duplicate to image
    	  if( imageParams->Count() > 0 )
           {
           __IF_DEBUG(Print(_L("iImageCount > 0")));
           iGSBitmap = CVtImageBitmap::NewL( imageParams->GetImageL( 0 ) );
           }
        else
           {
           __IF_DEBUG(Print(_L("iImageCount <= 0 -> User::Leave( KErrNotFound )")));
           User::Leave( KErrNotFound );
           }
        // Cleanup
    	  CleanupStack::PopAndDestroy( 3 ); // phoneClient, stillH, imageParams
    	  iScaled = CVtImageBitmap::NewL( TSize( KQcifWidth, KQcifHeight ), iDisplayMode );
    		
        // If bitmap has different display mode than screen, then new bitmap
        // is created and image is converted to screen's display mode
        if ( iGSBitmap->Bitmap().DisplayMode() != iDisplayMode || 
                iGSBitmap->Bitmap().ExtendedBitmapType() != KNullUid )
            {
            CVtImageBitmap* temp = CVtImageBitmap::NewL(
                iGSBitmap->Bitmap().SizeInPixels(), iDisplayMode );
            CleanupStack::PushL( temp );
            // Color conversion is done by blitting it to other bitmap
            CFbsBitmapDevice* fbd = CFbsBitmapDevice::NewL( &temp->Bitmap() );
            CleanupStack::PushL( fbd );
            CFbsBitGc* fgc = NULL;
            User::LeaveIfError( fbd->CreateContext( fgc ) );
            CleanupStack::PushL( fgc );
            fgc->BitBlt( TPoint(), &iGSBitmap->Bitmap() );
            CleanupStack::PopAndDestroy( 2 ); // fbd, fgc
            CleanupStack::Pop(); // temp
            delete iGSBitmap; iGSBitmap = NULL;
            iGSBitmap = temp;
            }
        iGSImage = ETrue;
        iCount = 1;     
		    iActiveWaitInit->InitializeWaiter( &CMultiframeProvider::ConversionFinished );
        iActiveWaitInit->Signal( KErrNone );
        }
    else
        { // default still image prepared by using file handle
        InitializeL( NULL );
        }
    iInitialization = ETrue;
    iDelay = 0;    
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::IniatializeGSL() <<"), RThread().Id().operator TUint()));
	}
// -----------------------------------------------------------------------------
// CMultiframeProvider::IniatializeBlank()
//
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::IniatializeBlankL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::IniatializeBlank() >>"), RThread().Id().operator TUint()));
    iBlank = ETrue;
    iInitialization = ETrue;
    iDelay = 0;
    iCount = 1;

    CFbsBitmap* tempBitmap = 0;

    CreateBlankBitmapL(
        tempBitmap,
        TSize( KQcifWidth, KQcifHeight ),
        iDisplayMode,
#ifdef _DEBUG
        KRgbYellow
#else
        KRgbBlack
#endif
        );

    CleanupStack::PushL( tempBitmap );
    iScaled = CVtImageBitmap::NewL( tempBitmap->Handle() );
    CleanupStack::PopAndDestroy(); // tempBitmap
    tempBitmap = 0;

    iActiveWaitInit->InitializeWaiter( &CMultiframeProvider::BMSScalingFinished );
    iActiveWaitInit->Signal( KErrNone );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::IniatializeBlank() <<"), RThread().Id().operator TUint()));
    }
 // -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CreateBlankBitmapL(
//  CFbsBitmap*& aBitmap, const TSize& aSize, const TDisplayMode& aMode,
//  const TRgb& aColor )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::CreateBlankBitmapL(
    CFbsBitmap*& aBitmap,
    const TSize& aSize,
    const TDisplayMode& aMode,
    const TRgb& aColor )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CreateBlankBitmapL() >>"), RThread().Id().operator TUint()));
    if( aBitmap )
       {
       delete aBitmap;
       aBitmap = 0;
       }
    aBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( aBitmap->Create( aSize, aMode ) );
    CFbsBitmapDevice* fbd = CFbsBitmapDevice::NewL( aBitmap );
    CleanupStack::PushL( fbd );
    CFbsBitGc* fgc = 0;
    User::LeaveIfError( fbd->CreateContext( fgc ) );
    CleanupStack::PushL( fgc );

    fgc->SetPenColor( aColor );
    fgc->SetBrushColor( aColor );
    fgc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    fgc->DrawRect( TRect( aSize ) );

    CleanupStack::PopAndDestroy( 2 ); // fbd, fgc
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CreateBlankBitmapL() <<"), RThread().Id().operator TUint()));
    }
// -----------------------------------------------------------------------------
// CMultiframeProvider::Iniatialize()
//
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::InitializeL( const TFileName* aFileName )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::InitializeL() >>"), RThread().Id().operator TUint()));
		// Create bitmaps used for conversion and scaling  
		iBuffer1 = CVtImageBitmap::NewL( TSize( KQcifWidth, KQcifHeight ), iDisplayMode );
		iBuffer2 = CVtImageBitmap::NewL( TSize( KQcifWidth, KQcifHeight ), iDisplayMode );
    User::LeaveIfError( iFreeQueue.Append( iBuffer1 ) );
		User::LeaveIfError( iFreeQueue.Append( iBuffer2 ) );
		iLoopAo->SetFreeBitmaps( 2 );
			
		// Create ICL decoder in its own thread for current file  
		TInt decodingoptions( 
		CImageDecoder::TOptions( CImageDecoder::EOptionAlwaysThread | CImageDecoder::EAllowGeneratedMask ) );	
        
    if ( aFileName )
    		{
    		__IF_DEBUG(Print(_L(" file name exist, creating CImageDecoder")));
    		iImageDecoder = CImageDecoder::FileNewL( iFs, *aFileName, (CImageDecoder::TOptions )decodingoptions /*CImageDecoder::EOptionNone*/ );
    	  }
    else
        {
        __IF_DEBUG(Print(_L(" default vt image file => use phone server")));
        // default VT image file handle obtained from phone server
        RPhCltServer phoneClient;
        User::LeaveIfError( phoneClient.Connect() );
        __IF_DEBUG(Print(_L("  connected to phone server")));
        CleanupClosePushL( phoneClient );
        CPhCltImageHandler* handler = CPhCltImageHandler::NewL();
        CleanupStack::PushL( handler );                          
        __IF_DEBUG(Print(_L(" created image handler")));
        User::LeaveIfError( handler->OpenDefaultVtImage( iDefaultImageFile ) );
        decodingoptions &= ~CImageDecoder::EOptionAlwaysThread;
        __IF_DEBUG(Print(_L(" creating image decoder")));
        iImageDecoder = CImageDecoder::FileNewL( iDefaultImageFile, ContentAccess::EPlay, (CImageDecoder::TOptions )decodingoptions );
        __IF_DEBUG(Print(_L(" created image decoder")));
        CleanupStack::PopAndDestroy( 2 ); // handler, phoneClient
        }
    // Use ICL to calculate frame count 
		iCount = iImageDecoder->FrameCount();
		__IF_DEBUG(Print(_L(" frame count=%d"),iCount));
		//If advanced flags are on show only first frame!
		for ( TInt index = 0; index < iCount; ++index )
    		 {
    		 TFrameInfo frameInfo( iImageDecoder->FrameInfo( index ) );
    		 __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::InitializeL() frameInfo.iFlags %b >>"), RThread().Id().operator TUint(), frameInfo.iFlags));
    		 if ( frameInfo.iFlags & TFrameInfo::ELeaveInPlace )
    		     {
    			   iDelay = 0;
    		     iCount = 1;	
    			   }
    		 }        	
		//Set num of frames to viewer AO
		iLoopAo->SetFrameCount( iCount );
		//For default loopping frame sequence is disabled
		iAnimation = EFalse;
		iInitialization = ETrue;
		//Start ICL image conversion (decoding) and local scaling 
		ConvertAndScaleL( KFirstFrameIndx );	
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::InitializeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::SetVFScalingTargetSize()
//
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::SetVFScalingTargetSize( TSize aSize )
	  {
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::SetVFScalingTargetSize() >>"), RThread().Id().operator TUint()));
  	iTargetSize = aSize;
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::SetVFScalingTargetSize() W %d H %d <<"), RThread().Id().operator TUint(), aSize.iWidth , aSize.iHeight ));
  	}

// -----------------------------------------------------------------------------
// CMultiframeProvider::ConvertAndScaleL()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ConvertAndScaleL( TInt aFrameIndex )
	  {
	  // Variables related to Bitmap size
	  // calculation
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConvertAndScale()  aFrameIndex %d>>"), RThread().Id().operator TUint(),aFrameIndex));
	  TSize convertedBitmapSize;
	  TFrameInfo frameInfo( iImageDecoder->FrameInfo( aFrameIndex ) );
	  if ( !iInitialization )
		    {
			  TInt delayindex;
			  delayindex = aFrameIndex - 1;
			  if ( delayindex <  KFirstFrameIndx )
					  {
						delayindex = ( iCount - 1 );
					  }
			  TFrameInfo delayframeInfo( iImageDecoder->FrameInfo( delayindex ) );
			  iDelay = delayframeInfo.iDelay;
		    }
	  else
		    {
		    iDelay = 1;
		    }
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConvertAndScale()  iDelay %d>>"), RThread().Id().operator TUint(),iDelay.Int64() ));
#ifdef _DEBUG
    TUint threadId( RThread().Id() );
    switch ( frameInfo.iFrameDisplayMode )
        {
        case EColor4K:
            RDebug::Print( _L( "VideoSource [%d]: FrameDPmode = EColor4K" ),
            	threadId );
            break;
        case EColor64K:
            RDebug::Print( _L( "VideoSource [%d]: FrameDPmode = EColor64K" ),
            	threadId );
            break;
        case EColor16M:
          	  RDebug::Print( _L( "VideoSource [%d]: FrameDPmode = EColor16M" ),
            	threadId );
            break;
        case EColor16MU:
          	  RDebug::Print( _L( "VideoSource [%d]: FrameDPmode = EColor16MU" ),
            	threadId );
           	 break;
        default:
           	 RDebug::Print( _L( "VideoSource [%d]: FrameDPmode = %d" ),
            	threadId, frameInfo.iFrameDisplayMode );
           	 break;
        }

    RDebug::Print( _L( "VideoSource [%d]: CMultiframeProvider:: frameInfo.iFlags %d" ),
        threadId,frameInfo.iFlags );
#endif // _DEBUG

 	  // Use imagedecoder to calculate how many times source picture needs to be divided by 2
 	  // to fit in default (VGA) picture
	  TInt reductionfactor = iImageDecoder->ReductionFactor( frameInfo.iOverallSizeInPixels,
		    TSize( KDefaultBitmapSizeWidth, KDefaultBitmapSizeHeight ) );
 	  if ( iImageDecoder->ReducedSize( frameInfo.iOverallSizeInPixels, reductionfactor, convertedBitmapSize ) )
    	  {
    	  convertedBitmapSize = frameInfo.iOverallSizeInPixels;
    	  }

#ifdef _DEBUG
	   RDebug::Print( _L( "VideoSource [%d]: convertedBitmapSize Width = %d convertedBitmapSize Height = %d" ),
		    threadId, convertedBitmapSize.iWidth, convertedBitmapSize.iHeight );
#endif // _DEBUG
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConvertAndScale() iConverted"), RThread().Id().operator TUint()));
	      iConverted = CVtImageBitmap::NewL( convertedBitmapSize, iDisplayMode );
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConvertAndScale() iConverted"), RThread().Id().operator TUint()));
#ifdef _DEBUG
	  RDebug::Print( _L( "VideoSource [%d]: convBitmap Width = %d convBitmap Height = %d" ),
		    threadId, iConverted->Size().iWidth, iConverted->Size().iHeight );
#endif // _DEBUG

	  iStartTime.HomeTime();
	  if ( iFreeQueue.Count() < 2 )
       	{
       	__IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ConvertAndScale() iFreeQueue.Count() < 2 "), RThread().Id().operator TUint()));
       	iOldBM = ETrue;
       	iOldBitmap = iScaled;
       	}
    iScaled	= iFreeQueue[ 0 ];
	  iFreeQueue.Remove( 0 );
	  iLoopAo->RemoveFreeBitmaps( 1 );
	  iActiveWaitInit->InitializeWaiter( &CMultiframeProvider::ConversionFinished );
	  // Check if masking is supported
	  if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
		    {
		    // Create mask for bitmap
		    iMask = new ( ELeave )CFbsBitmap();
		    User::LeaveIfError( iMask->Create( convertedBitmapSize, EGray256 ));
		    iTempBM = CVtImageBitmap::NewL( convertedBitmapSize, iDisplayMode );
		    iImageDecoder->Convert( iActiveWaitInit->RequestStatus(), iTempBM->Bitmap(), *iMask, aFrameIndex );
		    }
	  // No mask
	  else
		    {
		    iImageDecoder->Convert( iActiveWaitInit->RequestStatus(), iConverted->Bitmap(), aFrameIndex );
		    }
	  __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConvertAndScale() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CMultiframeProvider::ConversionFinished( TInt aError )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ConversionFinished( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ConversionFinished() >>"), RThread().Id().operator TUint()));
    if ( iCount == 1 || aError )
        {
        // delete decoder if not needed any more 
        // 1) single frame image, no further frames to convert
        // 2) error occured, decoder won't be used anyway
        __IF_DEBUG(Print(_L(" single frame image => destruct iImageDecoder, err %d"), aError ) );
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    // no harm done if other than this used, handle would already be 0
    iDefaultImageFile.Close(); 
    if ( aError == KErrNone )
        {
				iActiveWaitInitScale->InitializeWaiter( &CMultiframeProvider::BMSScalingFinished );
      	if ( iGSImage )
       			{
       			__IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ConversionFinished() scale GS "), RThread().Id().operator TUint()));
       			iVtImageScalerInit->Scale( *iGSBitmap, *iScaled );
      			}
      	else
      			{
       			__IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ConversionFinished() scale "), RThread().Id().operator TUint()));
       			// Blit mask into a converted bitmap
       			if ( iMask )
       					{
       					CGraphicsContext* bitmapcontext = NULL;
       					CFbsBitmapDevice* bitmapdevice = NULL;
       					// Create bm device
       					TRAPD( error, bitmapdevice = CFbsBitmapDevice::NewL( &( iConverted->Bitmap() ) ) );
       					if ( error != KErrNone )
       							{
       							iObserver->NotifyImageHandlingError( aError );
       							delete bitmapdevice;
       							delete iTempBM;
       							delete iMask;
       							iMask = NULL;
       							iTempBM = NULL;
       							return;
       							}
       					// Create GC for bm
       					error = bitmapdevice->CreateContext( bitmapcontext );
       					if ( error != KErrNone )
       							{
       							iObserver->NotifyImageHandlingError( aError );
       							delete bitmapdevice;
       							delete bitmapcontext;
       							delete iTempBM;
       							delete iMask;
       							iMask = NULL;
       							iTempBM = NULL;
       							return;
       							}
       					// Fill destination bitmap with white color
       					TRect rect( iConverted->Size() );
       					bitmapcontext->SetBrushStyle( CGraphicsContext:: ESolidBrush );
       					bitmapcontext->SetPenStyle( CGraphicsContext::ESolidPen );
       					bitmapcontext->SetBrushColor( KRgbWhite );
       					bitmapcontext->SetPenColor( KRgbWhite);
       					bitmapcontext->DrawRect( rect );

       					// Draw mask and data bitmap into a same destination bm
       					bitmapcontext->DrawBitmapMasked( iConverted->Size(), &( iTempBM->Bitmap() ), iConverted->Size(), iMask, EFalse );
       					delete bitmapdevice;
       					delete bitmapcontext;
       					delete iTempBM;
       					delete iMask;
       					iMask = NULL;
       					iTempBM = NULL;
       				 	__IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ConversionFinished() iMask"), RThread().Id().operator TUint()));
       					}
       			iVtImageScalerInit->Scale( *iConverted, *iScaled );
      			}
        	}
    // Allways inform Decoding problems
    else
       	{
        iObserver->NotifyImageHandlingError( aError );
        delete iTempBM;
       	delete iMask;
       	iMask = NULL;
       	iTempBM = NULL;
        // GIF used when error happen
        if ( iCount > 1 )
            {
            iCount = 1;
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ConversionFinished() error %d <<"), RThread().Id().operator TUint(), aError));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::BMSScalingFinished( TInt aError )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::BMSScalingFinished( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::BMSScalingFinished() >>"), RThread().Id().operator TUint()));
    if ( aError == KErrNone )
        {
        VFScalingReady();
        }
    else
        {
#if 0
        if ( aError != KErrCancel ) // If KErrCancel, then ignore error!
            {
        	iObserver->NotifyImageHandlingError( aError );
        	}
        //Needs to be deleted also in cancel case
        delete iConverted; iConverted = NULL;
#endif
        delete iConverted; iConverted = NULL;
        iObserver->NotifyImageHandlingError( aError );
        
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::BMSScalingFinished() err %d <<"), RThread().Id().operator TUint(), aError));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::AddBitmapToFreeQueueL( CVtImageBitmap* aBitmap )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::AddBitmapToFreeQueueL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::AddBitmapToFreeQueue() >>"), RThread().Id().operator TUint()));
    if ( iOldBM )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::AddBitmapToFreeQueue() OLD BM"), RThread().Id().operator TUint()));
        iOldBM = EFalse;
        User::LeaveIfError( iFreeQueue.Append( iOldBitmap ) );
        }
    else
        {
        User::LeaveIfError( iFreeQueue.Append( iScaled ) );
        }
    iLoopAo->SetFreeBitmaps( 1 );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::AddBitmapToFreeQueue() <<"), RThread().Id().operator TUint()));
    }
    
// -----------------------------------------------------------------------------
// CMultiframeProvider::UseSecondVfBuffer()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::UseSecondVfBuffer()
    {
    iSecondVFBuffer = ETrue;
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::VFScalingReady()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::VFScalingReady()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady() >>"), RThread().Id().operator TUint()));
    delete iConverted; iConverted = 0;

    // Calculate frame update delay here!
    TTime refpointtime;
    refpointtime.HomeTime();
    TTimeIntervalMicroSeconds deltaTime = refpointtime.MicroSecondsFrom( iStartTime );
    iFrameUpdateTime = iDelay.Int64() - deltaTime.Int64();
    if ( iFrameUpdateTime < 0 )
    		{
    		iFrameUpdateTime = 1;
    		}
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady() iFrameUpdateTime %d>>"), RThread().Id().operator TUint(), iFrameUpdateTime.Int64()));
  	// Scale VF to fit in requested size
  	// size is as default QCIF
    if ( iIsVfStopped )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady()VF Stop>>"), RThread().Id().operator TUint()));
        if ( iActiveWaitScale->IsActive() )
            {
            //ScaleCopy running
            //Waiting is OK
            __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady()ScaleCopy running>>"), RThread().Id().operator TUint()));
            }
        else
            {
            //ScaleCopy is not running, Jump ScaleCopy
            __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady()jump ScaleCopy>>"), RThread().Id().operator TUint()));
            iActiveWaitScale->InitializeWaiter( &CMultiframeProvider::VFRescalingFinished );
            iActiveWaitScale->Signal(KErrNone);
            }
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady()Normal run>>"), RThread().Id().operator TUint()));
        iVFUpdate = EFalse;
        ScaleCopy( iTargetSize );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFScalingReady() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::ScaleCopy( TSize& aSize )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ScaleCopy( TSize& aSize,  TBool aVFUpdate  )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ScaleCopy() >>"), RThread().Id().operator TUint()));

    iVFUpdate = aVFUpdate;
    iVFSize = aSize;
    if ( iSecondVFBuffer && !iFirstVFBuffer )
        {
        iVFbitmap = iSecondVfBuf;
        iSecondVFBuffer = EFalse;
        iFirstVFBuffer = ETrue;
        }
    else
        {
        iVFbitmap = iFirstVfBuf;
        iFirstVFBuffer = EFalse;
        }
    //If Scaling is ongoing
    if ( iActiveWaitScale->IsActive() )
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ScaleCopy(): active, storing size"), RThread().Id().operator TUint()));
        iWantedVFSize = aSize;
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ScaleCopy(): scaling NOW"), RThread().Id().operator TUint()));
        iActiveWaitScale->InitializeWaiter( &CMultiframeProvider::VFRescalingFinished );
        TInt result( iVFbitmap->Bitmap().Resize( aSize ) );
        __IF_DEBUG(Print(_L("VideoSource [%d]: CMultiframeProvider::ScaleCopy() result %d <<"), RThread().Id().operator TUint(), result));
        if ( result != KErrNone )
            {
            iActiveWaitScale->Signal( result );
            }
        else
            {
            iVtImageScalerScale->Scale( *iScaled, *iVFbitmap );
            }
        }
    }
// -----------------------------------------------------------------------------
// CMultiframeProvider::NextFrame()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::NextFrame()
    {
    iFreezeCS.Wait();
    iInitialization = EFalse;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::NextFrame() >>"), RThread().Id().operator TUint()));
    
    if ( iNumOfConsumers < 1 )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::NextFrame() Consumer < 1>>"), RThread().Id().operator TUint()));
        iFreezeCS.Signal();
        return;
        }
    
    if (iCount > 1 && !iActiveWaitInit->IsActive() )
        {
        if (iWaitForConverting)
            {
            __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::NextFrame() Because CMultiframeloopAO::RunL is executing, so Signal is not needed"), RThread().Id().operator TUint()));
            }
        else
            {
            iLoopAo->Signal();
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::NextFrame() <<"), RThread().Id().operator TUint()));
    iFreezeCS.Signal();
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::IncreaseDataConsumer()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::IncreaseDataConsumer()
    {
    iFreezeCS.Wait();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::IncreaseDataConsumer() >>"), RThread().Id().operator TUint()));
    ++iNumOfConsumers;
    iFreezeCS.Signal();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::IncreaseDataConsumer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::DecreaseDataConsumer()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::DecreaseDataConsumer()
	  {
		iFreezeCS.Wait();
		__IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::DecreaseDataConsumer() >>"), RThread().Id().operator TUint()));
		--iNumOfConsumers;
		if ( iNumOfConsumers < 1 )
    	  {
  		  iLoopAo->CancelDataLoop();
        iDataTimer->CancelTimeLoop();
  	  	}
  	__IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::DecreaseDataConsumer() <<"), RThread().Id().operator TUint()));  	
  	iFreezeCS.Signal();
	  }
// -----------------------------------------------------------------------------
// CMultiframeProvider::ScaleVF( const TSize& aSize )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ScaleVF( const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScaleVF() >>"), RThread().Id().operator TUint()));
    iActiveWaitScale->InitializeWaiter( &CMultiframeProvider::VFRescalingFinished );
    TInt result( iVFbitmap->Bitmap().Resize( aSize ) );
    ClearVFScalingTargetSize();
    if ( result != KErrNone )
        {
        iActiveWaitScale->Signal( result );
        }
    else
        {
        iVtImageScalerScale->Scale( *iScaled, *iVFbitmap );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScaleVF() result %d <<"), RThread().Id().operator TUint(), result));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::VFRescalingFinished( TInt aError )
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::VFRescalingFinished( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFRescalingFinished() >>"), RThread().Id().operator TUint()));
    if ( aError == KErrNone )
        {
        if ( iWantedVFSize != TSize() )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFRescalingFinished() REScaling needed"), RThread().Id().operator TUint()));
            ScaleVF( iWantedVFSize );
            ClearVFScalingTargetSize();
            }
        else
            {
            iDataTimer->CancelTimeLoop();
            
            if ( !iIsVfStopped && !iVFUpdate )
                {
                //Normally, VF and VS work
                iVFCurrent = iVFbitmap;
                iYUVBitMap = iScaled;
                iDataTimer->LocalBuffer(iVFCurrent);
                iDataTimer->RemoteBuffer(iYUVBitMap);
                __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScalecopyRemoteAndUpdate() normal, iFrameUpdateTime %d>>"), RThread().Id().operator TUint(),
                        iFrameUpdateTime.Int64()));
                iDataTimer->Activate(iFrameUpdateTime, iInitialization, iVFUpdate);
                }
            else if ( iIsVfStopped && !iVFUpdate )
                {
                //VF doesn't work.
                iYUVBitMap = iScaled;
                iDataTimer->RemoteBuffer(iYUVBitMap);
                __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScalecopyRemoteAndUpdate() VF Stop, iFrameUpdateTime %d>>"), RThread().Id().operator TUint(),
                        iFrameUpdateTime.Int64()));                
                iDataTimer->Activate( iFrameUpdateTime, iInitialization, iVFUpdate);
                }
            else if ( iIsVfStopped && iVFUpdate )
                {
                //VF restart 
                iIsVfStopped = EFalse;
                iVFUpdate = EFalse;
                iVFCurrent = iVFbitmap;
                iYUVBitMap = iScaled;
                iDataTimer->LocalBuffer(iVFCurrent);
                iDataTimer->RemoteBuffer(iYUVBitMap);
                __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScalecopyRemoteAndUpdate() VF Restart iFrameUpdateTime 1>>"),
                        RThread().Id().operator TUint()));
                iDataTimer->Activate(1, iInitialization, iVFUpdate);
                }
            else
                {
                //maybe this condition never execute.
                iVFCurrent = iVFbitmap;
                iDataTimer->LocalBuffer(iVFCurrent);
                __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ScalecopyRemoteAndUpdate() last one, iFrameUpdateTime %d>>"), RThread().Id().operator TUint(),
                        iFrameUpdateTime.Int64()));                
                iDataTimer->Activate( 1, iInitialization, iVFUpdate);
                }
            }
        }
#if 0
    else if ( ( aError == KErrCancel ) )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFRescalingFinished() CANCEL"), RThread().Id().operator TUint()));
        }
    else if( aError != KErrCancel ) // If KErrCancel, then ignore error!
        {
        iObserver->NotifyImageHandlingError( aError );
        }
#endif
    else
        {
        iObserver->NotifyImageHandlingError( aError );
        }
    
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::VFRescalingFinished() err %d <<"), RThread().Id().operator TUint(), aError));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::IsVFStopped()
// -----------------------------------------------------------------------------
//
TBool CMultiframeProvider::IsVFStopped()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::IsVFStopped() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::IsVFStopped() <<"), RThread().Id().operator TUint()));
    return iIsVfStopped;
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::SetVFStop()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::SetVFStop()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::SetVFStop() >>"), RThread().Id().operator TUint()));
    iIsVfStopped = ETrue;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::SetVFStop() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::WaitForConverting()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::WaitForConverting()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::WaitForConverting() >>"), RThread().Id().operator TUint()));
    iWaitForConverting = ETrue;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::WaitForConverting() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::WaitForConverting()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ReleaseForConverting()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ReleaseForConverting() >>"), RThread().Id().operator TUint()));
    iWaitForConverting = EFalse;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ReleaseForConverting() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::ClearVFScalingTargetSize()
// -----------------------------------------------------------------------------
//
void CMultiframeProvider::ClearVFScalingTargetSize()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ClearVFScalingTargetSize() >>"), RThread().Id().operator TUint()));
    iWantedVFSize = TSize();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::ClearVFScalingTargetSize() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CMultiframeProvider::CancelInitializing()
// -----------------------------------------------------------------------------
//
TBool CMultiframeProvider::CancelInitializing()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CancelInitializing() >>"), RThread().Id().operator TUint()));
    TBool lResult = EFalse;
    
    if ( NULL == iImageDecoder )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CancelInitializing() iImageDecoder is NULL ==="), RThread().Id().operator TUint()));
        // In this condition, Convert is already finished
        if ( iVtImageScalerInit->IsActive() )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d] Line[%d]: CMultiframeProvider::CancelInitializing() iVtImageScalerInit->Cancel ==="), 
                    RThread().Id().operator TUint(), __LINE__));
            iVtImageScalerInit->Cancel();
            }
        else if ( iActiveWaitInitScale->IsActive() )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d] Line[%d]: CMultiframeProvider::CancelInitializing() iActiveWaitInitScale->Cancel ==="), 
                    RThread().Id().operator TUint(), __LINE__));
            iActiveWaitInitScale->Cancel();
            }            
        else if ( iVtImageScalerScale->IsActive() )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d] Line[%d]: CMultiframeProvider::CancelInitializing() iVtImageScalerScale->Cancel ==="), 
                    RThread().Id().operator TUint(), __LINE__));
            iVtImageScalerScale->Cancel();
            }
        else if ( iActiveWaitScale->IsActive() )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d] Line[%d]: CMultiframeProvider::CancelInitializing() iActiveWaitScale->Cancel ==="), 
                    RThread().Id().operator TUint(), __LINE__));
            iActiveWaitScale->Cancel();
            }
        else
            {
            //In this condition, the convert,scale,copyScale were finished,
            //so do nothing, Waiting for InitializeReady()
            __IF_DEBUG(Print(_L("VideoSource[%d] Line[%d]: CMultiframeProvider::CancelInitializing() Nothong to do,wait finish ==="), 
                    RThread().Id().operator TUint(), __LINE__));
            }
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CancelInitializing() cancel convert ==="), RThread().Id().operator TUint()));
        //Convert is not finished, so we can cancel it, and return ETrue
        iImageDecoder->Cancel();
        lResult = ETrue;
        }
    
    __IF_DEBUG(Print(_L("VideoSource[%d]: CMultiframeProvider::CancelInitializing() <<"), RThread().Id().operator TUint()));
    return lResult;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File

