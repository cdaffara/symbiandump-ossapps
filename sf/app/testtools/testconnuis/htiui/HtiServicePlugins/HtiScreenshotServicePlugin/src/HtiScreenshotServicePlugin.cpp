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
* Description:  SysInfoPlugin implementation
*
*/


// INCLUDE FILES
#include "../../../symbian_version.hrh"


#include "HtiScreenshotServicePlugin.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <imageconversion.h>
#include <ezcompressor.h>
#include <hal.h>

#include <AknLayoutConfig.h>
#include <apgtask.h> 
#include <AknCapServerDefs.h>

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include <alf/alfdrawer.h>
#endif

// CONSTANTS
const static TUid KScreenshotServiceUid = {0x1020DEC3};

enum TScreenCommands
    {
    // Normal screencapture
    ECmdScreen                = 0x01,
    ECmdScreenRegion          = 0x02,
    ECmdScreenZip             = 0x03,
    ECmdScreenRegionZip       = 0x04,

    // Text recognition
    ECmdTextRcg               = 0x10,
    ECmdTextRcg_u             = 0x11,

    // Text bitmap
    ECmdTextBitmap            = 0x12,
    ECmdTextBitmap_u          = 0x13,

    // Screencapture in series
    ECmdScreenSeries          = 0x21,
    ECmdScreenRegionSeries    = 0x22,
    ECmdScreenZipSeries       = 0x23,
    ECmdScreenRegionZipSeries = 0x24,

    // Selects the screen to use
    ECmdSelectScreen          = 0x30,

    // Gets the current screen size and orientation
    ECmdScreenMode            = 0x3A,
    
    // Rotates the screen to portrait or landscape
    ECmdRotateScreen          = 0x3B,

    // Screencapture on updated part of screen only
    ECmdDeltaCaptureMask           = 0x80,
    ECmdDeltaScreen                = 0x81,
    ECmdDeltaScreenRegion          = 0x82,
    ECmdDeltaScreenZip             = 0x83,
    ECmdDeltaScreenRegionZip       = 0x84,
    ECmdDeltaScreenReset           = 0x85
    //ECmdDeltaScreenSeries          = 0xA1,
    //ECmdDeltaScreenRegionSeries    = 0xA2,
    //ECmdDeltaScreenZipSeries       = 0xA3,
    //ECmdDeltaScreenRegionZipSeries = 0xA4
    };

enum TScreenResponse
    {
    ERspOk = 0xF0,
    ERspNotFound = 0xF1
    };

enum THtiFontAttributes
    {
    EHtiFontAttBold = 0x01,
    EHtiFontAttItalic = 0x02,
    EHtiFontAttNotAA = 0x04,
    EHtiFontAttPrintPositionFlag = 0x08,
    EHtiFontAttPrintPositionValue = 0x10
    };

const static TInt KHtiFontAttSuperscriptValue = 0;
const static TInt KHtiFontAttSubscriptValue = 1;

//1 byte for cmd and 2*4 for 4 coordinates
const static TInt KMinScreenRegionCmdLength = 9;
const static TInt KScreenDisplayOffset = 1;
const static TInt KScreenMIMEOffset = KScreenDisplayOffset + 1;
const static TInt KScreenScreenNumber = KScreenMIMEOffset + 8;
const static TInt KRegionDisplayOffset = KMinScreenRegionCmdLength;
const static TInt KRegionMIMEOffset = KRegionDisplayOffset + 1;
const static TInt KRegionScreenNumber = KRegionMIMEOffset + 8;

const static TInt KSeriesDurationOffset = 1;
const static TInt KSeriesIntervalOffset = KSeriesDurationOffset + 4;
const static TInt KSeriesDisplayOffset = KSeriesIntervalOffset + 4;
const static TInt KSeriesMIMEOffset = KSeriesDisplayOffset + 1;
const static TInt KSeriesScreenNumber = KSeriesMIMEOffset + 8;
const static TInt KMinSeriesCmdLength = KSeriesMIMEOffset;

const static TInt KRegionSeriesTlX = KSeriesDisplayOffset + 1;
const static TInt KRegionSeriesTlY = KRegionSeriesTlX + 2;
const static TInt KRegionSeriesBlX = KRegionSeriesTlY + 2;
const static TInt KRegionSeriesBlY = KRegionSeriesBlX + 2;
const static TInt KRegionSeriesMIMEOffset = KRegionSeriesBlY + 2;
const static TInt KRegionSeriesScreenNumber = KRegionSeriesMIMEOffset + 8;
const static TInt KMinRegionSeriesCmdLength = KRegionSeriesMIMEOffset;

const static TInt KDeltaResetCmdLength = 1;
const static TInt KScreenModeCmdLength = 1;

const static TInt KScreenNrOffset = 1;
const static TInt KSelectScreenCmdLength = 2;
const static TInt KRotateScreenCmdLength = 2;

_LIT( KSeriesShotPath, "c:\\Hti\\SeriesShot\\" );

//errors' descriptions
_LIT8( KErrDescrInvalid, "invalid arguments" );
_LIT8( KErrDescrInvalidMode, "invalid color mode" );
_LIT8( KErrDescrRegiontEmpty, "region is empty" );
_LIT8( KErrDescrRegionNotNormailized, "region is not normalized" );
_LIT8( KErrDescrRegionOutOfScreen, "region is out of screen" );
_LIT8( KErrDescrUnknownCommand, "unknown command" );
_LIT8( KErrDescrFailedConvert, "failed to convert to image format" );
_LIT8( KErrDescrFailedCompress, "failed to compress" );
_LIT8( KErrDescrMIMENotSupported, "MIME type not supported" );
_LIT8( KErrDescrScreenNotSupported, "screen not supported" );

_LIT( KScreenshotPanic, "Screenshot plug-in invalid state" );

//_LIT(KS60Sans, "Series 60 Sans");
//_LIT(KS60SansTitleBold, "Series 60 Sans TitleSmBd");

//const TInt KFonHeighMin = 110;
//const TInt KFonHeighMax = 190;
/*
// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::InitFontCache()
    {
    //temporary
    //just put harcoded data
    //should be either external file or auto-defined based on logical fonts
    //or some test app
    TFontSpec fs;
    fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    //primary font
    fs.iTypeface.iName = KS60Sans;
    fs.iHeight = 161;
    fs.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    iFontCache.Append(fs);

    fs.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);

    //Series 60 Sans TitleSmBd, 183
    fs.iTypeface.iName = KS60SansTitleBold;
    fs.iHeight = 183;
    iFontCache.Append(fs);

    //Series 60 Sans TitleSmBd, 172
    fs.iTypeface.iName = KS60SansTitleBold;
    fs.iHeight = 172;
    iFontCache.Append(fs);

    //Series 60 Sans,           122
    fs.iTypeface.iName = KS60Sans;
    fs.iHeight = 122;
    iFontCache.Append(fs);
    //Series 60 Sans,           116
    fs.iTypeface.iName = KS60Sans;
    fs.iHeight = 116;
    iFontCache.Append(fs);

    //Series 60 Sans TitleSmBd, 138
    fs.iTypeface.iName = KS60SansTitleBold;
    fs.iHeight = 138;
    iFontCache.Append(fs);
    }
*/

// ----------------------------------------------------------------------------
TInt ImageDifferenceL( CFbsBitmap* aImage1, CFbsBitmap* aImage2,
                       CFbsBitmap* &aResult, TRect &aRect )
    {
    HTI_LOG_TEXT( "ImageDifferenceL()" );

    // By default return coordinates of the full image
    aRect = TRect( 0, 0, aImage2->SizeInPixels().iWidth,
                  aImage2->SizeInPixels().iHeight );

//1. check that aImage1 and aImage2 are valid and can be compared
    if ( aImage1 == NULL || aImage2 == NULL )
        {
        HTI_LOG_TEXT( "return KErrArgument" );
        return KErrArgument;
        }

    if ( aImage1->SizeInPixels() != aImage2->SizeInPixels() )
        {
        HTI_LOG_TEXT( "return KErrGeneral (size)" );
        return KErrGeneral;
        }

    if ( aImage1->DisplayMode() != aImage2->DisplayMode() )
        {
        HTI_LOG_TEXT( "return KErrGeneral (displaymode)" );
        return KErrGeneral;
        }


//2. iterate through images from each border and compare to findout outline for diff region
    TSize orgSize = aImage1->SizeInPixels();

    TBitmapUtil srcBmpIterator1( aImage1 );
    TBitmapUtil srcBmpIterator2( aImage2 );

    srcBmpIterator1.Begin( TPoint( 0, 0 ) );
    srcBmpIterator2.Begin( TPoint( 0, 0 ), srcBmpIterator1 );

    TRect diffOutline = TRect( -1, -1, -1, -1 );

    //2.1 top border iteration
    TPoint c( 0,0 );
    for ( ; c.iY < orgSize.iHeight && diffOutline.iTl.iY == -1; ++c.iY )
        {
        c.iX = 0;
        srcBmpIterator1.SetPos( c );
        srcBmpIterator2.SetPos( c );
        for ( ; c.iX < orgSize.iWidth && diffOutline.iTl.iY == -1; ++c.iX )
            {
            if ( srcBmpIterator1.GetPixel() != srcBmpIterator2.GetPixel() )
                {
                diffOutline.iTl.iY = c.iY;
                }

            srcBmpIterator1.IncXPos();
            srcBmpIterator2.IncXPos();
            }
        }

    //2.2 bottom iteration
    c.SetXY( 0, orgSize.iHeight - 1 );
    for ( ; c.iY >= diffOutline.iTl.iY && diffOutline.iBr.iY == -1; --c.iY )
        {
        c.iX = 0;
        srcBmpIterator1.SetPos( c );
        srcBmpIterator2.SetPos( c );
        for (; c.iX < orgSize.iWidth && diffOutline.iBr.iY == -1; ++c.iX )
            {
            if ( srcBmpIterator1.GetPixel() != srcBmpIterator2.GetPixel() )
                {
                diffOutline.iBr.iY = c.iY;
                }

            srcBmpIterator1.IncXPos();
            srcBmpIterator2.IncXPos();
            }
        }

    //2.3 left, goes in vertical lines
    c.SetXY( 0, diffOutline.iTl.iY );
    for ( ; c.iX < orgSize.iWidth && diffOutline.iTl.iX == -1; ++c.iX )
        {
        c.iY = diffOutline.iTl.iY;
        srcBmpIterator1.SetPos( c );
        srcBmpIterator2.SetPos( c );
        for ( ; c.iY <= diffOutline.iBr.iY && diffOutline.iTl.iX == -1; ++c.iY )

            {
            if ( srcBmpIterator1.GetPixel() != srcBmpIterator2.GetPixel() )
                {
                diffOutline.iTl.iX = c.iX;
                }

            srcBmpIterator1.IncYPos();
            srcBmpIterator2.IncYPos();
            }
        }
    //2.4 right, goes in vertical lines
    c.SetXY( orgSize.iWidth - 1, diffOutline.iTl.iY );
    for ( ; c.iX >= diffOutline.iTl.iX && diffOutline.iBr.iX == -1; --c.iX )
        {
        c.iY = diffOutline.iTl.iY;
        srcBmpIterator1.SetPos( c );
        srcBmpIterator2.SetPos( c );
        for ( ; c.iY <= diffOutline.iBr.iY && diffOutline.iBr.iX == -1; ++c.iY )

            {
            if ( srcBmpIterator1.GetPixel() != srcBmpIterator2.GetPixel() )
                {
                diffOutline.iBr.iX = c.iX;
                }

            srcBmpIterator1.IncYPos();
            srcBmpIterator2.IncYPos();
            }
        }
    srcBmpIterator2.End();
    srcBmpIterator1.End();

    //3. if there is some diff create CFbsBitmap in aResult and copy outlined image from aImage2
    if ( diffOutline.iTl.iX == -1 &&
         diffOutline.iTl.iY == -1 &&
         diffOutline.iBr.iX == -1 &&
         diffOutline.iBr.iY == -1 )
        {
        // No difference found
        aRect = TRect( 0, 0, 0, 0 );
        HTI_LOG_TEXT( "return KErrNotFound" );
        return KErrNotFound;
        }

    aRect = diffOutline;

    HTI_LOG_FORMAT( "Tlx - %d", aRect.iTl.iX );
    HTI_LOG_FORMAT( "Tly - %d", aRect.iTl.iY );
    HTI_LOG_FORMAT( "Bty - %d", aRect.iBr.iX );
    HTI_LOG_FORMAT( "Bry - %d", aRect.iBr.iY );

    // The bottom right co-ordinate is not included in the rectange
    // (see TRect documentation) so we need to stretch the rectange
    // for BitBlt to get the correct sized image.

    TRect captureRect( diffOutline.iTl.iX, diffOutline.iTl.iY,
                       diffOutline.iBr.iX + 1, diffOutline.iBr.iY + 1 );

    aResult = new (ELeave) CFbsBitmap();
    User::LeaveIfError( aResult->Create( captureRect.Size(), aImage2->DisplayMode() ) );
    CleanupStack::PushL( aResult );

    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL( aResult );
    CleanupStack::PushL( bmpDevice );

    CFbsBitGc* bmpCtx;
    bmpDevice->CreateContext( bmpCtx );
    bmpCtx->BitBlt( TPoint( 0, 0 ), aImage2, captureRect );

    delete bmpCtx;
    bmpCtx = NULL;

    CleanupStack::PopAndDestroy(); // bmpDevice
    CleanupStack::Pop(); // aResult

    HTI_LOG_TEXT( "return KErrNone" );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
CICLHandler::CICLHandler( CImageEncoder* aService, MICLObserver* anObserver ):
    CActive( EPriorityStandard ),
    iObserver( anObserver ),
    iService( aService )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
CICLHandler::~CICLHandler()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
void CICLHandler::Start()
    {
    SetActive();
    }

// ----------------------------------------------------------------------------
void CICLHandler::RunL()
    {
    iObserver->ICLComplete( iStatus.Int() );
    }

// ----------------------------------------------------------------------------
void CICLHandler::DoCancel()
    {
    iService->Cancel();
    }

/*
// ----------------------------------------------------------------------------
TInt CICLHandler::RunError(TInt aError)
    {

    }
*/

// ----------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
CHtiScreenshotServicePlugin* CHtiScreenshotServicePlugin::NewL()
    {
    CHtiScreenshotServicePlugin* self = new (ELeave) CHtiScreenshotServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
CHtiScreenshotServicePlugin::CHtiScreenshotServicePlugin():
    iScreen( NULL ),
    iEncodedBitmap( NULL ),
    iScreenDevice( NULL ),
    iBitmapEncoder( NULL ),
    iICLHandler( NULL ),
    iCompress( EFalse ),
    iDeltaCapture( EFalse ),
    iPreviousBitmap( NULL )
    {
    }

// ----------------------------------------------------------------------------
CHtiScreenshotServicePlugin::~CHtiScreenshotServicePlugin()
    {
    HTI_LOG_FUNC_IN( "~CHtiScreenshotServicePlugin" );

    iFontCache.Close();

    delete iScreen;
    delete iEncodedBitmap;

    delete iICLHandler;
    delete iBitmapEncoder;

    delete iScreenDevice;

    delete iSeriesShot;

    if ( iPreviousBitmap )
        delete iPreviousBitmap;

    iWs.Close();
    HTI_LOG_FUNC_OUT( "~CHtiScreenshotServicePlugin" );
    }

// ----------------------------------------------------------------------------
// Second phase construction.
void CHtiScreenshotServicePlugin::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::ConstructL" );
    User::LeaveIfError( iWs.Connect() );

    iScreenDevice = new ( ELeave ) CWsScreenDevice( iWs );
    User::LeaveIfError( iScreenDevice->Construct() );

    //InitFontCache();

    iSeriesShot = CSeriesShot::NewL( this );

    iPreviousBitmap = new ( ELeave ) CFbsBitmap;

    //SelectEncoder( KImageTypeBMPUid );
    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::ConstructL" );
    }

// ----------------------------------------------------------------------------
TBool CHtiScreenshotServicePlugin::IsBusy()
    {
    if ( iICLHandler )
        {
        return iICLHandler->IsActive();
        }

    if ( iSeriesShot->IsOngoing() )
        {
        return ETrue;
        }

    return iEncodedBitmap != NULL;
    }

// ----------------------------------------------------------------------------
inline TInt CHtiScreenshotServicePlugin::ParseInt16( const TUint8* aStart )
    {
    return aStart[0] + (aStart[1]<<8);
    }

// ----------------------------------------------------------------------------
inline TInt CHtiScreenshotServicePlugin::ParseInt32( const TUint8* aStart )
    {
    return aStart[0] + (aStart[1]<<8) + (aStart[2]<<16) + (aStart[3]<<24);
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::SendTextRecgReplyL(
                                    const TBool aResult,
                                    const TRect& aLocation,
                                    const TInt aFontIndex)
    {
    HTI_LOG_FUNC_IN( "SendTextRecgReplyL" );
    HBufC8* sendMsg = HBufC8::NewL( 10 );
    CleanupStack::PushL( sendMsg );
    if ( aResult )
        {
        sendMsg->Des().Append( ERspOk );
        TUint16 co = aLocation.iTl.iX;
        sendMsg->Des().Append( (TUint8*)(&co), 2 );
        co = aLocation.iTl.iY;
        sendMsg->Des().Append( (TUint8*)(&co), 2 );
        co = aLocation.iBr.iX;
        sendMsg->Des().Append( (TUint8*)(&co), 2 );
        co = aLocation.iBr.iY;
        sendMsg->Des().Append( (TUint8*)(&co), 2 );
        sendMsg->Des().Append( (TUint8)aFontIndex );
        }
    else
        {
        sendMsg->Des().Append( ERspNotFound );
        sendMsg->Des().AppendFill( 0, 5 );
        }

    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
                            sendMsg,
                            KScreenshotServiceUid) );

    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "SendTextRecgReplyL" );
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::CopyUnicode( TDes & aTo, const TDesC8& aFrom )
{
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::CopyUnicode" );
    //aTo.Copy( reinterpret_cast<const TUint16*>(aFrom.Ptr()), aFrom.Length() );
    TInt len = aFrom.Length()>>1;
    aTo.SetLength( len );
    for ( TInt i = 0; i < len; ++i )
    {
        aTo[i] = (TUint16)aFrom[i<<1] + (((TUint16)aFrom[(i<<1)+1])<<8);
    }
    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::CopyUnicode" );
}

// ----------------------------------------------------------------------------
TInt CHtiScreenshotServicePlugin::ParseString( const TDesC8& aRequest,
                                        TInt anOffset,
                                        TBool aUnicode,
                                        TDes& aResult)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::ParseString" );
    //validate parameters
    //if offset outside the string return empty string
    if ( anOffset >= aRequest.Size() )
        {
        return anOffset;
        }

    TInt len = aRequest[ anOffset ];
    HTI_LOG_FORMAT( "len %d", len );

    if ( len> aResult.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt nextOffset = ( aUnicode ? len * 2 : len ) + anOffset + 1;
    HTI_LOG_FORMAT( "nextOffset %d", nextOffset );
    HTI_LOG_FORMAT( "reqSize %d", aRequest.Size() );
    if ( nextOffset > aRequest.Size() )
        {
        return KErrArgument;
        }

    if ( aUnicode )
        {
        //const TUint8* ptr = aRequest.Mid( anOffset + 1, len * 2 ).Ptr();
        //aResult.Copy( (const TUint16*)ptr, len );
        CopyUnicode( aResult, aRequest.Mid( anOffset + 1, len * 2 ) );
        }
    else
        {
        aResult.Copy( aRequest.Mid( anOffset + 1, len ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::ParseString" );
    return nextOffset;
    }

// ----------------------------------------------------------------------------
TInt CHtiScreenshotServicePlugin::ParseFontSpec( const TDesC8& aRequest,
                    TInt anOffset,
                    TBool aUnicode,
                    TFontSpec& aResult)
    {
    if ( anOffset >= aRequest.Size() )
        {
        return KErrArgument;
        }

    //get font name
    TPtr tn = aResult.iTypeface.iName.Des();
    TInt offset = ParseString( aRequest,
                    anOffset,
                    aUnicode,
                    tn );

    if ( offset > anOffset )
        {
        HTI_LOG_DES(aResult.iTypeface.iName);
        //check that we have valid descr
        if ( offset + 2 <= aRequest.Size() )
            {
            aResult.iHeight = ParseInt16( aRequest.Ptr() + offset );
            HTI_LOG_FORMAT( "font height %d", aResult.iHeight );
            //check style byte
            TUint8 style = aRequest[ offset + 2 ];
            HTI_LOG_FORMAT( "style %d", style );

            //stroke bit
            if ( style & EHtiFontAttBold )
                {
                aResult.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
                }
            else
                {
                aResult.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
                }
            //posture
            if ( style & EHtiFontAttItalic )
                {
                aResult.iFontStyle.SetPosture(EPostureItalic);
                }
            else
                {
                aResult.iFontStyle.SetPosture(EPostureUpright);
                }
            //bitmap glyph type
            if ( style & EHtiFontAttNotAA )
                {
                aResult.iFontStyle.SetBitmapType( EMonochromeGlyphBitmap );
                }
            else
                {
                aResult.iFontStyle.SetBitmapType( EAntiAliasedGlyphBitmap );
                }
            //print position
            if ( style & EHtiFontAttPrintPositionFlag )
                {
                TInt printPos = style & EHtiFontAttPrintPositionValue;
                if ( printPos == KHtiFontAttSuperscriptValue )
                    {
                    aResult.iFontStyle.SetPrintPosition( EPrintPosSuperscript );
                    }
                else if ( printPos == KHtiFontAttSubscriptValue )
                    {
                    aResult.iFontStyle.SetPrintPosition( EPrintPosSubscript );
                    }
                }
            else
                {
                aResult.iFontStyle.SetPrintPosition( EPrintPosNormal );
                }
            return offset + 3;
            }
        else
            {
            return KErrArgument;
            }
        }
    else
        {
        return offset<0?offset:KErrArgument;
        }
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::ProcessTextRcgMessageL(
                                    const TDesC8& aMessage)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::ProcessTextRcgMessageL" );
    TBool unicode = aMessage[0] & 0x1;

    TBuf<0xFF> text;

    TInt offset = ParseString(aMessage, 1, unicode, text);

    HTI_LOG_FORMAT( "offset %d ", offset );
    if ( offset > 1 )
        {
        HTI_LOG_DES(text);

        if ( offset + 1 < aMessage.Size() )
            {
            TInt numOfFonts = aMessage[ offset ];
            HTI_LOG_FORMAT( "num of fonts %d", numOfFonts );
            iFontCache.Reset();
            TInt nextOffset  = offset + 1;
            for ( TInt i = 0; i < numOfFonts; ++i )
                {
                TFontSpec fontSpec;
                nextOffset = ParseFontSpec(aMessage,
                                            nextOffset,
                                            unicode,
                                            fontSpec);
                if ( nextOffset < 0 )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                        nextOffset,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);
                    return;
                    }
                else
                    {
                    iFontCache.Append( fontSpec );
                    }
                }

            //parameters parsing END
            //get screenshot
            TRect empty;
            CreateBitmapL( empty, ENone );

            //call text rcg routines
            TInt fontIndex;
            TRect resultRect;

            //recognize text using fonts from iFontCache
            TBool result = RecognizeTextL( text, resultRect, fontIndex );

            SendTextRecgReplyL( result, resultRect, fontIndex );

            delete iScreen;
            iScreen = NULL;
            }
        else
            {
            //no fonts data
            iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);

            }
        }
    else if ( offset == 1 )
        {
        //empty text
        iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);
        }
    else
        {
        //error
        iDispatcher->DispatchOutgoingErrorMessage(
                        offset,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);
        }
    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::ProcessTextRcgMessageL" );
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::ProcessTextBitmapMessageL(
                                    const TDesC8& aMessage)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::ProcessTextBitmapMessageL" );
    TBool unicode = aMessage[0] & 0x1;

    TDisplayMode displayMode = ENone;
    //check display
    if ( aMessage.Size() > KScreenDisplayOffset )
        {
        displayMode = (TDisplayMode)aMessage[KScreenDisplayOffset];
        if ( displayMode >= EColorLast )
            {
            iDispatcher->DispatchOutgoingErrorMessage(
                            KErrArgument,
                            KErrDescrInvalidMode,
                            KScreenshotServiceUid);
            return;
            }
        }

    //check mime
    TPtrC8 mime;
    if ( aMessage[KScreenMIMEOffset] > 0 &&
        ( aMessage[KScreenMIMEOffset] + KScreenMIMEOffset+1 ) < aMessage.Size() )
        {
        mime.Set( aMessage.Mid(KScreenMIMEOffset+1, aMessage[KScreenMIMEOffset] ) );
        if ( !IsMIMETypeSupported( mime ) )
            {
            iDispatcher->DispatchOutgoingErrorMessage(
                            KErrArgument,
                            KErrDescrMIMENotSupported,
                            KScreenshotServiceUid);
            return;
            }
        }
    else if ( aMessage[KScreenMIMEOffset] != 0 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrInvalidMode,
                        KScreenshotServiceUid);
        return;
        }

    TBuf<0xFF> text;
    TInt preTextOffset = KScreenMIMEOffset + aMessage[KScreenMIMEOffset] + 1;
    TInt offset = ParseString( aMessage, preTextOffset, unicode, text );

    HTI_LOG_FORMAT( "offset %d ", offset );
    if ( offset == preTextOffset )
        {
        //empty text
        iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);
        }
    else if ( offset < preTextOffset )
        {
        //error
        iDispatcher->DispatchOutgoingErrorMessage(
                        offset,
                        KErrDescrInvalid,
                        KScreenshotServiceUid);
        }

    HTI_LOG_DES(text);
    TFontSpec fontSpec;
    offset = ParseFontSpec(aMessage,
                            offset,
                            unicode,
                            fontSpec);
    if ( offset < 0 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            offset,
            KErrDescrInvalid,
            KScreenshotServiceUid);

        return;
        }

    //check colors
    HTI_LOG_TEXT( "check colors" );
    if ( offset + 2*4 != aMessage.Size() )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            offset,
            KErrDescrInvalid,
            KScreenshotServiceUid);
        return;
        }

    //extract colors
    TUint32 fgColor = ParseInt32( aMessage.Ptr() + offset );
    TUint32 bgColor = ParseInt32( aMessage.Ptr() + offset + 4 );
    HTI_LOG_FORMAT( "FG color %d", fgColor );
    HTI_LOG_FORMAT( "BG color %d", bgColor );

    //END parsing
    //generate and return bitmap
    CFont* useFont;

    User::LeaveIfError( iScreenDevice->GetNearestFontToDesignHeightInPixels(
                            useFont, fontSpec ) );

    TDisplayMode dm = displayMode==ENone || displayMode==0?
                                        iScreenDevice->DisplayMode():
                                        displayMode;


    delete iScreen;
    iScreen = NULL;
    iScreen = CHtiTextRcg::GetTextBitmapL(
                            text,
                            useFont,
                            TRgb( fgColor ),
                            TRgb( bgColor ),
                            dm );

    iScreenDevice->ReleaseFont( useFont );

    //Encode iBitmap
    iCompress = EFalse;
    if ( mime.Length() == 0 )
        {
        EncodeBitmapL(); //use default encoder BMP
        }
    else
        {
        HTI_LOG_DES( mime );
        EncodeBitmapL( mime );
        }

    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::ProcessTextBitmapMessageL" );
    }

/*
// ----------------------------------------------------------------------------
TBool CHtiScreenshotServicePlugin::RecognizeTextAllL(
                        const TDesC& aText,
                        TPoint& aResult)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::RecognizeTextAllL" );

    TSize screenRect = iScreenDevice->SizeInPixels();
    TInt nofTF = iScreenDevice->NumTypefaces();
    HTI_LOG_FORMAT( "Number of typefaces %d", nofTF );
    TBool returnValue = EFalse;
    for ( TInt i = 0; i < nofTF; ++i )
        {
        TTypefaceSupport tf;
        iScreenDevice->TypefaceSupport(tf, i);

        HTI_LOG_DES(tf.iTypeface.iName);

        if ( tf.iIsScalable )
            {
            //iterate throuh heighes

            HTI_LOG_FORMAT( "num of heighs %d", tf.iNumHeights );
            HTI_LOG_FORMAT( "min h in tw %d", tf.iMinHeightInTwips );
            HTI_LOG_FORMAT( "max h in tw %d", tf.iMaxHeightInTwips );
            HTI_LOG_FORMAT( "scalable %d", tf.iIsScalable );

            HTI_LOG_TEXT( "-----------------------" );

            TInt minHeight = Max(tf.iMinHeightInTwips, KFonHeighMin );
            TInt maxHeight = Min(tf.iMaxHeightInTwips, KFonHeighMax );

            if ( minHeight > maxHeight )
                {
                continue;
                }

            for ( TInt v = 0; v < 2; ++v )
                {
                TInt lastFontHeight = 0;
                for ( TInt fh = minHeight; fh <= maxHeight; ++fh )
                    {
                    TFontSpec fs( tf.iTypeface.iName, fh );
                    fs.iFontStyle.SetBitmapType( EAntiAliasedGlyphBitmap );
                    switch ( v )
                        {
                        case 1:
                            {
                            fs.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
                            HTI_LOG_TEXT( "BOLD" );
                            }
                            break;
                        default:
                            {
                            HTI_LOG_TEXT( "DEFAULT" );
                            }
                        }
                    HTI_LOG_FORMAT( "hh  %d", fh );

                    CFont* useFont = NULL;

                    iScreenDevice->GetNearestFontToDesignHeightInTwips(useFont, fs);

                    if ( screenRect.iHeight < useFont->HeightInPixels() ||
                         screenRect.iWidth < useFont->MaxNormalCharWidthInPixels()
                        )
                        {
                        break;
                        }

                    if ( useFont->HeightInPixels() == lastFontHeight )
                        {
                        continue;
                        }


                    lastFontHeight = useFont->HeightInPixels();

                    returnValue = iTextRcg.RecognizeTextL(
                                                iScreen,
                                                aText,
                                                useFont,
                                                aResult);
                    //HTI_LOG_TEXT( "ReleaseFont" );
                    iScreenDevice->ReleaseFont(useFont);

                    if ( returnValue )
                        {
                        HTI_LOG_TEXT( "Found" );
                        HTI_LOG_DES( aText );
                        HTI_LOG_DES( tf.iTypeface.iName );
                        HTI_LOG_FORMAT( "Font height in twips %d", fh );
                        HTI_LOG_FORMAT( "X %d", aResult.iX );
                        HTI_LOG_FORMAT( "Y %d", aResult.iY );
                        return returnValue;
                        }
                    }
                }
            }
        else
            {//non scal. font
            TFontSpec fs( tf.iTypeface.iName,0 ); //height doesn't matter for
                                                //not scalable font

            CFont* useFont = NULL;
            //HTI_LOG_TEXT( "GetFont" );
            iScreenDevice->GetNearestFontToDesignHeightInTwips( useFont, fs );

            returnValue = iTextRcg.RecognizeTextL(
                                        iScreen,
                                        aText,
                                        useFont,
                                        aResult );
            //HTI_LOG_TEXT( "ReleaseFont" );
            iScreenDevice->ReleaseFont( useFont );

            if ( returnValue )
                {
                HTI_LOG_TEXT( "Found" );
                HTI_LOG_DES( aText );
                HTI_LOG_DES(tf.iTypeface.iName );
                HTI_LOG_FORMAT( "X %d", aResult.iX );
                HTI_LOG_FORMAT( "Y %d", aResult.iY );
                return returnValue;
                }
            }
        }

    //

    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::RecognizeTextAllL" );
    //return returnValue;
    return EFalse;
    }
*/

// ----------------------------------------------------------------------------
TBool CHtiScreenshotServicePlugin::RecognizeTextL(
                        const TDesC& aText,
                        TRect& aResult,
                        TInt& aFontIndex)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::RecognizeTextL" );

    TSize screenRect = iScreenDevice->SizeInPixels();
    TInt cacheSize = iFontCache.Count();
    HTI_LOG_FORMAT( "Cache size %d", cacheSize );

    TBool returnValue = EFalse;
    for ( TInt i = 0; i < cacheSize; ++i )
        {
        CFont* useFont = NULL;

        User::LeaveIfError(iScreenDevice->GetNearestFontToDesignHeightInPixels(
                                useFont, iFontCache[i] ) );
        if ( iFontCache[i].iFontStyle.BitmapType()==EAntiAliasedGlyphBitmap )
            {
            iTextRcg.SetHint( EHintEdge );
            }
        else
            {
            iTextRcg.SetHint( EHintNone );
            }

        //check that font in valid size
        if ( screenRect.iHeight < useFont->HeightInPixels() ||
             screenRect.iWidth < useFont->MaxNormalCharWidthInPixels()
            )
            {
            break;
            }

        returnValue = iTextRcg.RecognizeTextL( iScreen, aText, useFont,
                aResult );

        iScreenDevice->ReleaseFont( useFont );

        if ( returnValue )
            {
            HTI_LOG_FORMAT( "Found! fontIndex %d", i );
            HTI_LOG_DES( aText );
            HTI_LOG_DES( iFontCache[i].iTypeface.iName );
            HTI_LOG_FORMAT( "TL X %d", aResult.iTl.iX );
            HTI_LOG_FORMAT( "TL Y %d", aResult.iTl.iY );
            HTI_LOG_FORMAT( "BR X %d", aResult.iBr.iX );
            HTI_LOG_FORMAT( "BR Y %d", aResult.iBr.iY );
            aFontIndex = i;
            return returnValue;
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::RecognizeTextL" );
    return EFalse;
    }

/*
TBool CHtiScreenshotServicePlugin::RecognizeTextL(
                        const TDesC& aText,
                        const TDesC& aTypeface,
                        TPoint& aResult)
    {
    HTI_LOG_FUNC_IN( "RecognizeTextL typeface" );
    //const CFont* fontUsed = NULL;// AknLayoutUtils::FontFromName(aTypeface);
    CFont* useFont = NULL;
    TFontSpec fs(aTypeface, 0);

    iScreenDevice->GetNearestFontInTwips(useFont, fs);

    TBool returnValue = iTextRcg.RecognizeTextL(iScreen, aText, useFont, aResult);

HTI_LOG_FUNC_OUT( "RecognizeTextL" );
    return returnValue;
}
*/

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::ProcessMessageL(const TDesC8& aMessage,
                THtiMessagePriority /*aPriority*/)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::ProcessMessage");

    if ( iICLHandler )
        {
        if ( iICLHandler->IsActive() || iEncodedBitmap)
            {
            User::Leave( KErrInUse );
            }
        }

    if ( iSeriesShot->IsOngoing() )
        User::Leave( KErrInUse );

    // update the current screen mode
    TPixelsAndRotation currentPixelsAndRotation;
    iScreenDevice->GetScreenModeSizeAndRotation(
            iScreenDevice->CurrentScreenMode(), currentPixelsAndRotation );
    iScreenDevice->SetScreenSizeAndRotation( currentPixelsAndRotation );

    if ( aMessage.Length() > 0 )
        {
        // set/reset delta capture status
        iDeltaCapture = ( aMessage[0] & ECmdDeltaCaptureMask ) ? ETrue : EFalse;
        if ( iDeltaCapture )
            {
            HTI_LOG_TEXT( "DeltaCapture ETrue" );
            }

        //if text recogn call separate handler
        if ( aMessage[0] == ECmdTextRcg ||
             aMessage[0] == ECmdTextRcg_u )
            {
            ProcessTextRcgMessageL( aMessage );
            return;
            }
        else if ( aMessage[0] == ECmdTextBitmap ||
                  aMessage[0] == ECmdTextBitmap_u )
            {
            ProcessTextBitmapMessageL( aMessage );
            return;
            }

        iCompress = ( aMessage[0] == ECmdScreenZip ) ||
                    ( aMessage[0] == ECmdScreenRegionZip ) ||
                    ( aMessage[0] == ECmdScreenZipSeries ) ||
                    ( aMessage[0] == ECmdScreenRegionZipSeries ) ||
                    ( aMessage[0] == ECmdDeltaScreenZip ) ||
                    ( aMessage[0] == ECmdDeltaScreenRegionZip );

        HTI_LOG_FORMAT( "cmd 0x%x", aMessage[0] );
        TPtrC8 mime;

        switch ( aMessage[0] )
            {
            case ECmdScreen:
            case ECmdScreenZip:
            case ECmdDeltaScreen:
            case ECmdDeltaScreenZip:
                {
                TRect empty;
                TDisplayMode displayMode = ENone;
                //check display
                if ( aMessage.Length() > KScreenDisplayOffset )
                    {
                    displayMode = ( TDisplayMode ) aMessage[KScreenDisplayOffset];
                    if ( displayMode >= EColorLast )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrInvalidMode,
                                        KScreenshotServiceUid );
                        return;
                        }
                    }

                bool screenNumberSet = false;
                //check screen number
                if ( (aMessage.Length() > KScreenScreenNumber) && 
                        ((aMessage[aMessage.Length()-1] == 0) || (aMessage[aMessage.Length()-1] == 1)))
                    {
                    TInt screenNumber = aMessage[aMessage.Length()-1];
                    HTI_LOG_FORMAT( "set screen number: %d", screenNumber );
                    screenNumberSet = true;
                    TInt screens;
                    TInt ret = HAL::Get(HAL::EDisplayNumberOfScreens, screens);
                    if(ret)
                        {
                        HTI_LOG_FORMAT( "HAL::Get failed %d", ret );
                        User::Leave(ret);
                        }
                    HTI_LOG_FORMAT( "HAL::Get number of screens %d", screens );
                    if( ( screenNumber>screens-1 ) || ( screenNumber<0 ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                KErrArgument, KErrDescrScreenNotSupported, KScreenshotServiceUid);
                        return;
                        }
                    SetScreenNumber(screenNumber);
                    }

                CreateBitmapL( empty, displayMode );

                //check mime
                if ( aMessage.Length() > KScreenMIMEOffset )
                    {
                    if(screenNumberSet)
                        {
                        mime.Set( aMessage.Mid( KScreenMIMEOffset, aMessage.Length()-1-KScreenMIMEOffset ) );
                        }
                    else
                        {
                        mime.Set( aMessage.Mid( KScreenMIMEOffset ) );
                        }
                    if ( !IsMIMETypeSupported( mime ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrMIMENotSupported,
                                        KScreenshotServiceUid );
                        return;
                        }
                    }
                }              
                break;

            case ECmdScreenRegion:
            case ECmdScreenRegionZip:
            case ECmdDeltaScreenRegion:
            case ECmdDeltaScreenRegionZip:
                {
                //check screen number
                bool screenNumberSet = false;
                if ( (aMessage.Length() > KRegionScreenNumber) && 
                        ((aMessage[aMessage.Length()-1] == 0) || (aMessage[aMessage.Length()-1] == 1)))
                    {
                    TInt screenNumber = aMessage[aMessage.Length()-1];
                    screenNumberSet = true;
                    TInt screens;
                    TInt ret = HAL::Get(HAL::EDisplayNumberOfScreens, screens);
                    if(ret)
                        {
                        HTI_LOG_FORMAT( "HAL::Get failed %d", ret );
                        User::Leave(ret);
                        }
                    HTI_LOG_FORMAT( "HAL::Get number of screens %d", screens );
                    if( ( screenNumber>screens-1 ) || ( screenNumber<0 ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                KErrArgument, KErrDescrScreenNotSupported, KScreenshotServiceUid);
                        return;
                        }
                    SetScreenNumber(screenNumber);
                    }

                if ( aMessage.Length() >= KMinScreenRegionCmdLength )
                    {
                    TRect region;
                    const TUint8* ptr = aMessage.Ptr();
                    region.iTl.iX = ParseInt16( ptr + 1 );
                    region.iTl.iY = ParseInt16( ptr + 3 );
                    region.iBr.iX = ParseInt16( ptr + 5 );
                    region.iBr.iY = ParseInt16( ptr + 7 );

                    //check empty and normmalizaed
                    if ( !region.IsNormalized() )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrRegionNotNormailized,
                                        KScreenshotServiceUid );
                        return;
                        }

                    if ( region.IsEmpty() )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrRegiontEmpty,
                                        KScreenshotServiceUid );
                        return;
                        }

                    TRect screenRect;
                    screenRect.iBr = iScreenDevice->SizeInPixels().AsPoint();
                    screenRect.iBr.iX++; //TRect::Contains() omitts
                    screenRect.iBr.iY++; //right bottom rows

                    TDisplayMode displayMode = ENone;
                    if ( aMessage.Length() > KRegionDisplayOffset )
                        {
                        displayMode = ( TDisplayMode ) aMessage[KRegionDisplayOffset];
                        if ( displayMode >= EColorLast )
                            {
                            iDispatcher->DispatchOutgoingErrorMessage(
                                            KErrArgument,
                                            KErrDescrInvalidMode,
                                            KScreenshotServiceUid );
                            return;
                            }
                        }

                    if ( screenRect.Contains( region.iTl ) &&
                         screenRect.Contains( region.iBr ) )
                        {
                        CreateBitmapL( region, displayMode );
                        }
                    else
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrRegionOutOfScreen,
                                        KScreenshotServiceUid );
                        return;
                        }
                    
                    //check mime
                    if ( aMessage.Length() > KRegionMIMEOffset )
                        {
                        if(!screenNumberSet)
                            {
                            mime.Set( aMessage.Mid( KRegionMIMEOffset ) );
                            }
                        else
                            {
                            mime.Set( aMessage.Mid( KRegionMIMEOffset, aMessage.Length()-1-KRegionMIMEOffset ) );
                            }
                        if ( !IsMIMETypeSupported( mime ) )
                            {
                            iDispatcher->DispatchOutgoingErrorMessage(
                                            KErrArgument,
                                            KErrDescrMIMENotSupported,
                                            KScreenshotServiceUid );
                            return;
                            }
                        }

                    }
                else
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid );
                    return;
                    }
                }
                break;

            case ECmdScreenSeries:
            case ECmdScreenZipSeries:
                {
                if ( aMessage.Length() < KMinSeriesCmdLength )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid );
                    return;
                    }
                
                bool screenNumberSet = false;
                if ( (aMessage.Length() > KSeriesScreenNumber) && 
                        ((aMessage[aMessage.Length()-1] == 0) || (aMessage[aMessage.Length()-1] == 1)) )
                    {
                    TInt screenNumber = aMessage[aMessage.Length()-1];
                    screenNumberSet = true;
                    TInt screens;
                    TInt ret = HAL::Get(HAL::EDisplayNumberOfScreens, screens);
                    if(ret)
                        {
                        HTI_LOG_FORMAT( "HAL::Get failed %d", ret );
                        User::Leave(ret);
                        }
                    HTI_LOG_FORMAT( "HAL::Get number of screens %d", screens );
                    if( ( screenNumber>screens-1 ) || ( screenNumber<0 ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                KErrArgument, KErrDescrScreenNotSupported, KScreenshotServiceUid);
                        return;
                        }
                    SetScreenNumber(screenNumber);
                    }

                TInt duration = ParseInt32( aMessage.Ptr() + KSeriesDurationOffset );
                TInt interval = ParseInt32( aMessage.Ptr() + KSeriesIntervalOffset );

                TDisplayMode displayMode = ( TDisplayMode ) aMessage[KSeriesDisplayOffset];
                if ( displayMode >= EColorLast )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalidMode,
                                    KScreenshotServiceUid );
                    return;
                    }
                
                if ( aMessage.Length() > KSeriesMIMEOffset )
                    {
                    if(screenNumberSet)
                        {
                        mime.Set( aMessage.Mid( KSeriesMIMEOffset, aMessage.Length()-1-KSeriesMIMEOffset ) );
                        }
                    else
                        {
                        mime.Set( aMessage.Mid( KSeriesMIMEOffset ) );
                        }
                    if ( !IsMIMETypeSupported( mime ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrMIMENotSupported,
                                        KScreenshotServiceUid );
                        return;
                        }
                    }

                TRect empty;
                iSeriesShot->StartL( duration, interval, displayMode, empty, mime );
                }
                return;

            case ECmdScreenRegionSeries:
            case ECmdScreenRegionZipSeries:
                {
                bool screenNumberSet = false;
                if ( (aMessage.Length() > KRegionSeriesScreenNumber) && 
                        ((aMessage[aMessage.Length()-1] == 0) || (aMessage[aMessage.Length()-1] == 1)) )
                    {
                    TInt screenNumber = aMessage[aMessage.Length()-1];
                    screenNumberSet = true;
                    TInt screens;
                    TInt ret = HAL::Get(HAL::EDisplayNumberOfScreens, screens);
                    if(ret)
                        {
                        HTI_LOG_FORMAT( "HAL::Get failed %d", ret );
                        User::Leave(ret);
                        }
                    HTI_LOG_FORMAT( "HAL::Get number of screens %d", screens );
                    if( ( screenNumber>screens-1 ) || ( screenNumber<0 ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                KErrArgument, KErrDescrScreenNotSupported, KScreenshotServiceUid);
                        return;
                        }
                    SetScreenNumber(screenNumber);
                    }

                if ( aMessage.Length() < KMinRegionSeriesCmdLength )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid);
                    return;
                    }
                TInt duration = ParseInt32( aMessage.Ptr() + KSeriesDurationOffset );
                TInt interval = ParseInt32( aMessage.Ptr() + KSeriesIntervalOffset );

                TDisplayMode displayMode = ( TDisplayMode ) aMessage[KSeriesDisplayOffset];
                if ( displayMode >= EColorLast )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalidMode,
                                    KScreenshotServiceUid );
                    return;
                    }

                TRect region;
                const TUint8* ptr = aMessage.Ptr();
                region.iTl.iX = ParseInt16( ptr + KRegionSeriesTlX );
                region.iTl.iY = ParseInt16( ptr + KRegionSeriesTlY );
                region.iBr.iX = ParseInt16( ptr + KRegionSeriesBlX );
                region.iBr.iY = ParseInt16( ptr + KRegionSeriesBlY );

                //check empty and normmalizaed
                if ( !region.IsNormalized() )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrRegionNotNormailized,
                                    KScreenshotServiceUid );
                    return;
                    }

                if ( region.IsEmpty() )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrRegiontEmpty,
                                    KScreenshotServiceUid );
                    return;
                    }

                TRect screenRect;
                screenRect.iBr = iScreenDevice->SizeInPixels().AsPoint();
                screenRect.iBr.iX++; //TRect::Contains() omitts
                screenRect.iBr.iY++; //right bottom rows

                if ( !screenRect.Contains( region.iTl ) ||
                     !screenRect.Contains( region.iBr ) )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrRegionOutOfScreen,
                                    KScreenshotServiceUid );
                    return;
                    }
                
                if ( aMessage.Length() > KRegionSeriesMIMEOffset )
                    {
                    if(screenNumberSet)
                        {
                        mime.Set( aMessage.Mid( KRegionSeriesMIMEOffset, aMessage.Length()-1-KRegionSeriesMIMEOffset ) );
                        }
                    else
                        {
                        mime.Set( aMessage.Mid( KRegionSeriesMIMEOffset ) );
                        }
                    if ( !IsMIMETypeSupported( mime ) )
                        {
                        iDispatcher->DispatchOutgoingErrorMessage(
                                        KErrArgument,
                                        KErrDescrMIMENotSupported,
                                        KScreenshotServiceUid );
                        return;
                        }
                    }

                iSeriesShot->StartL( duration, interval, displayMode, region, mime );
                }
                return;

            case ECmdSelectScreen:
                {
                if ( aMessage.Length() != KSelectScreenCmdLength )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid );
                    return;
                    }

                TInt screenNr = aMessage[KScreenNrOffset];

                TInt screens;
                TInt ret=HAL::Get( HAL::EDisplayNumberOfScreens, screens );
                if ( ret )
                    {
                    HTI_LOG_FORMAT( "HAL::Get failed %d", ret );
                    User::Leave( ret );
                    }


                if ( ( screenNr > screens - 1 ) || ( screenNr < 0 ) )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrScreenNotSupported,
                                    KScreenshotServiceUid );
                    return;
                    }


                HTI_LOG_FORMAT( "Number of screens %d", screens );
                HTI_LOG_FORMAT( "Setting to screen index %d", screenNr );

                // Clear the previous delta bitmap to avoid error
                iPreviousBitmap->Reset();

                // delete old screendevice and create a new one
                delete iScreenDevice;
                iScreenDevice = NULL;
                iScreenDevice = new ( ELeave ) CWsScreenDevice( iWs );
                User::LeaveIfError( iScreenDevice->Construct( screenNr ) );

                TBuf8<1> okMsg;
                okMsg.Append( ECmdSelectScreen );
                iDispatcher->DispatchOutgoingMessage(
                    okMsg.AllocL(), KScreenshotServiceUid );
                }
                return;

            case ECmdDeltaScreenReset:
                {
                if ( aMessage.Length() != KDeltaResetCmdLength )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid );
                    return;
                    }

                iPreviousBitmap->Reset();
                TBuf8<1> okMsg;
                okMsg.Append( ECmdDeltaScreenReset );
                iDispatcher->DispatchOutgoingMessage(
                    okMsg.AllocL(), KScreenshotServiceUid );
                }
                return;

            case ECmdScreenMode:
                {
                if ( aMessage.Length() != KScreenModeCmdLength )
                    {
                    iDispatcher->DispatchOutgoingErrorMessage(
                                    KErrArgument,
                                    KErrDescrInvalid,
                                    KScreenshotServiceUid );
                    return;
                    }

                TInt focusScreen = iWs.GetFocusScreen();
                TPixelsAndRotation sizeAndRotation;
                TDisplayMode mode = ENone;
                TInt thisScreen = iScreenDevice->GetScreenNumber();
                iScreenDevice->GetDefaultScreenSizeAndRotation( sizeAndRotation );
                mode = iScreenDevice->DisplayMode();

                HTI_LOG_FORMAT( "This screen   = %d", thisScreen );
                HTI_LOG_FORMAT( "Screen width  = %d", sizeAndRotation.iPixelSize.iWidth );
                HTI_LOG_FORMAT( "Screen height = %d", sizeAndRotation.iPixelSize.iHeight );
                HTI_LOG_FORMAT( "Rotation      = %d", sizeAndRotation.iRotation );
                HTI_LOG_FORMAT( "Display mode  = %d", mode );
                HTI_LOG_FORMAT( "Focus screen  = %d", focusScreen );
                TBuf8<8> respMsg;
                respMsg.Append( thisScreen );
                respMsg.Append( ( TUint8* )( &( sizeAndRotation.iPixelSize.iWidth ) ), 2 );
                respMsg.Append( ( TUint8* )( &( sizeAndRotation.iPixelSize.iHeight ) ), 2 );
                respMsg.Append( sizeAndRotation.iRotation );
                respMsg.Append( mode );
                respMsg.Append( focusScreen );
                iDispatcher->DispatchOutgoingMessage(
                    respMsg.AllocL(), KScreenshotServiceUid );
                }
                return;
           case ECmdRotateScreen:
               {
               if (aMessage.Length() != KRotateScreenCmdLength)
                   {
                   iDispatcher->DispatchOutgoingErrorMessage(KErrArgument,
                           KErrDescrInvalid, KScreenshotServiceUid);
                   return;
                   }
               HandleRotateScreen(aMessage.Right(aMessage.Length() -1));
               return;
               }
            default:
                //Error: unknown command
                iDispatcher->DispatchOutgoingErrorMessage(
                                KErrArgument,
                                KErrDescrUnknownCommand,
                                KScreenshotServiceUid );
                return;
            } // switch

        //Encode iBitmap
        if ( mime.Length() == 0 )
            {
            EncodeBitmapL(); //use default encoder BMP
            }
        else
            {
            HTI_LOG_DES( mime );
            EncodeBitmapL( mime );
            }
        }
    else
        {
        //error: empty request
        iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrUnknownCommand,
                        KScreenshotServiceUid );
        }

    HTI_LOG_FUNC_OUT( "HtiScreenshotServicePlugin::ProcessMessage" );
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::HandleRotateScreen(const TDesC8& aData)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::HandleRotateScreen" );
             
    TInt orientation = aData[0];
    if (orientation > 1 || orientation < 0)
        {
        iDispatcher->DispatchOutgoingErrorMessage(KErrArgument,
                KErrDescrInvalid, KScreenshotServiceUid);
        return;
        }

    TBool isLandScape = orientation;

    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    CWsScreenDevice* screenDevice = new (ELeave) CWsScreenDevice(ws);
    CleanupStack::PushL(screenDevice);
    User::LeaveIfError(screenDevice->Construct());
    TSize currentScreenSize = screenDevice->SizeInPixels();

    TBool needsRotating = ETrue;
    if (currentScreenSize.iWidth > currentScreenSize.iHeight && isLandScape)
        {
        // we are already in landscape 
        HTI_LOG_TEXT("The screen are already in landscape.");
        needsRotating = EFalse;
        }
    if (currentScreenSize.iWidth < currentScreenSize.iHeight
            && (!isLandScape))
        {
        // we are already in portrait 
        HTI_LOG_TEXT("The screen are already in portrait.");
        needsRotating = EFalse;
        }

    CAknLayoutConfig* layoutConfigPtr = CAknLayoutConfig::NewL();
    CleanupStack::PushL(layoutConfigPtr);

    CAknLayoutConfig& layoutConfig = *layoutConfigPtr;

    const CAknLayoutConfig::THardwareStateArray& hwStates =
            layoutConfig.HardwareStates();
    const CAknLayoutConfig::TScreenModeArray& screenModes =
            layoutConfig.ScreenModes();

    TInt newHwStateIndex = KErrNotFound;

    // lets select alternate state from current
    TSize newScreenSize;
    if (needsRotating)
        {
        newScreenSize = TSize(currentScreenSize.iHeight,
                currentScreenSize.iWidth);
        HTI_LOG_FORMAT("Rotate the screen to the new width %d", newScreenSize.iWidth);
        HTI_LOG_FORMAT("Rotate the screen to the new height %d", newScreenSize.iHeight);
        }
    else // basicly select current state again to ensure correct mode is informed to akncapserver
        {
        newScreenSize = TSize(currentScreenSize.iWidth,
                currentScreenSize.iHeight);
        }

    for (TInt i = 0; i < hwStates.Count(); i++)
        {
        const CAknLayoutConfig::THardwareState hwState = hwStates.At(i);

        const CAknLayoutConfig::TScreenMode normal = screenModes.Find(
                hwState.ScreenMode());

        if (normal.SizeInPixels() == newScreenSize)
            {
            newHwStateIndex = i;
            break;
            }
        }

    if (newHwStateIndex >= 0)
        {
        const CAknLayoutConfig::THardwareState newHwState = hwStates.At(
                newHwStateIndex);
        TApaTaskList taskList(ws);
        TApaTask aknCapsrvTask = taskList.FindApp(KAknCapServerUid);
        TInt keyCode = newHwState.KeyCode();
        HTI_LOG_FORMAT( "Send key code %d to akncapserver", keyCode );
        aknCapsrvTask.SendKey(keyCode, 0);
        }

    TBuf8<1> okMsg;
    okMsg.Append(0);
    iDispatcher->DispatchOutgoingMessage(okMsg.AllocL(),
            KScreenshotServiceUid);

    CleanupStack::PopAndDestroy(layoutConfigPtr);
    CleanupStack::PopAndDestroy(screenDevice);
    ws.Close();
             
    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::HandleRotateScreen" );
    }
// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::CreateBitmapL( TRect& aRegion,
                                                 TDisplayMode aMode )
    {
    HTI_LOG_FUNC_IN( "CreateBitmapL" );
    //create bitmap
    TSize imageSize = aRegion.IsEmpty() ? iScreenDevice->SizeInPixels() :
                        aRegion.Size();

    TDisplayMode displayMode = aMode == ENone ?
                                        iScreenDevice->DisplayMode() : aMode;

    delete iScreen;//in case ICLComplete was not called
    iScreen = NULL;
    iScreen = new( ELeave ) CFbsBitmap;
    User::LeaveIfError( iScreen->Create( imageSize, displayMode ) );

	TInt err = KErrNone;
	TRect region;
    if ( aRegion.IsEmpty() )
        {
        err = iScreenDevice->CopyScreenToBitmap( iScreen );
		region = imageSize;
        }
    else
        {
        err = iScreenDevice->CopyScreenToBitmap( iScreen, aRegion );
		region = aRegion;
        }
    if (err == KErrNoMemory)
	    {
		HTI_LOG_TEXT( "screenshot in camera mode" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
		err = CAlfDrawer::FallbackCopyScreenToBitmap(*iScreenDevice, iScreen, region);
#endif
		}

    if ( iDeltaCapture )
        {
        HTI_LOG_TEXT( "DeltaCapture enabled" );


        CFbsBitmap* differenceBitmap = NULL;
        TInt err = ImageDifferenceL( iPreviousBitmap,
                                     iScreen,
                                     differenceBitmap,
                                     iDeltaRect );

        iPreviousBitmap->Reset();
        iPreviousBitmap->Duplicate( iScreen->Handle() );

        if ( err == KErrNone )
            {
            delete iScreen;
            iScreen = differenceBitmap;
            }
        else if ( err == KErrNotFound )
            {
            delete iScreen;
            iScreen = NULL;

            if ( !iSeriesShot->IsOngoing() )
                {
                // Nothing has changed on the screen.
                // Send just iDeltaRect coordidates
                HBufC8* buf = HBufC8::NewL( 4 * 2 ); // 2 bytes for each coordinate
                buf->Des().SetLength( 4 * 2 );
                TUint16* ptr = (TUint16*) buf->Des().Ptr();
                ptr[0] = (TUint16) iDeltaRect.iTl.iX;
                ptr[1] = (TUint16) iDeltaRect.iTl.iY;
                ptr[2] = (TUint16) iDeltaRect.iBr.iX;
                ptr[3] = (TUint16) iDeltaRect.iBr.iY;
                // Response also sent in ICLComplete
                iDispatcher->DispatchOutgoingMessage( buf, KScreenshotServiceUid );
                }
            }
        }

    HTI_LOG_FUNC_OUT( "CreateBitmapL" );
    }
/*

// ----------------------------------------------------------------------------
void CleanupRArray( TAny* object )
    {
    ((RImageTypeDescriptionArray*)object)->ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::SelectEncoder( const TUid aEncoderUid )
    {
    //select encoder
    RImageTypeDescriptionArray imageTypeArray;
    CImageEncoder::GetImageTypesL( imageTypeArray );
    CleanupStack::PushL( TCleanupItem(CleanupRArray, &imageTypeArray) );

    //select specified encoder
    TBool found = EFalse;
    for ( TInt i = 0; i < imageTypeArray.Count(); ++i )
        {
        if ( imageTypeArray[i]->ImageType() == aEncoderUid )
            {
            iImageEncoderType = imageTypeArray[i]->ImageType();
            iImageEncoderSubtype = imageTypeArray[i]->SubType();
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); //imageTypeArray
    }
*/

// ----------------------------------------------------------------------------
TBool CHtiScreenshotServicePlugin::IsMIMETypeSupported(TDesC8 &aMime)
    {
    HTI_LOG_DES(aMime);
    RFileExtensionMIMETypeArray array;
    CImageEncoder::GetFileTypesL(array);
    for ( TInt i = 0; i < array.Count(); i++ )
        {
        if ( array[i]->MIMEType() == aMime )
            {
            HTI_LOG_TEXT( "MIME supported" );
            array.ResetAndDestroy();
            return ETrue;
            }
        }
    HTI_LOG_TEXT( "MIME not supported" );
    array.ResetAndDestroy();
    return EFalse;
    }


// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::EncodeBitmapL(const TDesC8& aImageTypeMIME )
    {
    HTI_LOG_FUNC_IN( "EncodeBitmapL" );
    delete iBitmapEncoder;
    iBitmapEncoder = NULL;
    delete iICLHandler;
    iICLHandler = NULL;

    if ( iScreen )
        {
        HTI_LOG_TEXT( "create encoder" );
        if ( aImageTypeMIME  == KNullDesC8 )
            {
            iBitmapEncoder = CImageEncoder::DataNewL( iEncodedBitmap,
                                        CImageEncoder::EOptionNone,
                                        KImageTypeBMPUid);//,
                                        //iImageEncoderSubtype);
            }
        else
            {
            iBitmapEncoder = CImageEncoder::DataNewL( iEncodedBitmap,
                                        aImageTypeMIME);
            }

        HTI_LOG_TEXT( "create CICLHandler" );
        iICLHandler = new(ELeave) CICLHandler( iBitmapEncoder, this );
        iBitmapEncoder->Convert( &(iICLHandler->iStatus), *iScreen );

        HTI_LOG_TEXT( "CICLHandler start");
        iICLHandler->Start();
        }
    else
        {
        HTI_LOG_TEXT( "Nothing to encode" );
        }

    HTI_LOG_FUNC_OUT( "EncodeBitmapL" );
    }

// ----------------------------------------------------------------------------
TInt CHtiScreenshotServicePlugin::Compress()
    {
    __ASSERT_ALWAYS(iEncodedBitmap!=NULL,User::Panic(KScreenshotPanic, KErrGeneral));
    TInt err = KErrNone;
    HBufC8* zippedTemp = NULL;

    HTI_LOG_FORMAT( "image size %d", iEncodedBitmap->Size() );
    TInt numOfSteps = 4;
    TInt comprBufferIncrease = iEncodedBitmap->Size()/numOfSteps;

    //straight way to handle cases
    //when compressed data larger than uncompressed
    //try until buffer for compr. data twice bigger than original data
    for ( TInt i = 0; i < numOfSteps; ++i )
        {
        delete zippedTemp;
        TRAP( err, zippedTemp = HBufC8::NewL( iEncodedBitmap->Size() +
                                              i*comprBufferIncrease ) );
        if ( err == KErrNone )
            {
            //try to zip
            HTI_LOG_TEXT( "try to zip" );
            TPtr8 zippedTempPtr = zippedTemp->Des();
            TRAP( err, CEZCompressor::CompressL( zippedTempPtr,
                                                 *iEncodedBitmap ) );
            if ( err == KErrNone || err != KEZlibErrBuf )
                {
                break;
                }
            }
        else
            {
            break;
            }
        }

    if ( err == KErrNone )
        {
        delete iEncodedBitmap;
        iEncodedBitmap = zippedTemp;
        }
    else
        {
        HTI_LOG_FORMAT( "compre error %d", err );
        delete zippedTemp;
        }

    return err;
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::ICLComplete( TInt anError)
    {
    HTI_LOG_FUNC_IN( "ICLComplete" );

    //delete what we dont need right away
    delete iBitmapEncoder;
    iBitmapEncoder = NULL;
    delete iICLHandler;
    iICLHandler = NULL;


    if ( anError==KErrNone )
        {
        TInt err = KErrNone;


        //compress
        if ( iCompress )
            {
            HTI_LOG_TEXT( "compress" );
            err = Compress();
            }

        //send
        if ( err == KErrNone )
            {

            if ( !iSeriesShot->IsOngoing() )
                {
                // Not a series shot

                if ( iDeltaCapture )
                    {
                    // DeltaCapture on

                    // If we have encoded the bitmap then we
                    // also have some difference in the bitmap

                    HTI_LOG_TEXT( "Sending image with coordinates..." );

                    HBufC8* buf = HBufC8::NewL( (4*2) + iEncodedBitmap->Size() );
                    buf->Des().SetLength(4*2);
                    TUint16* ptr = (TUint16*) buf->Des().Ptr();
                    ptr[0] = (TUint16) iDeltaRect.iTl.iX;
                    ptr[1] = (TUint16) iDeltaRect.iTl.iY;
                    ptr[2] = (TUint16) iDeltaRect.iBr.iX;
                    ptr[3] = (TUint16) iDeltaRect.iBr.iY;

                    buf->Des().Append(*iEncodedBitmap);

                    delete iEncodedBitmap;
                    iEncodedBitmap = NULL;

                    // Response also sent in CreateBitmapL
                    err = iDispatcher->DispatchOutgoingMessage(buf,
                                            KScreenshotServiceUid);
                    }
                else
                    {
                    // Normal case
                    HTI_LOG_TEXT( "Sending image..." );
                    err = iDispatcher->DispatchOutgoingMessage(iEncodedBitmap,
                                            KScreenshotServiceUid);
                    }

                if (  err == KErrNoMemory )
                    {
                    HTI_LOG_TEXT( "wait for memory" );
                    iDispatcher->AddMemoryObserver( this );
                    }
                else if ( err == KErrNone )
                    {
                    iEncodedBitmap = NULL;
                    }
                else //just drop
                    {
                    HTI_LOG_TEXT( "ERROR: Impossible to send image" );
                    delete iEncodedBitmap;
                    iEncodedBitmap = NULL;
                    }
                }
            }
        else
            {
            iSeriesShot->Cancel();
            iDispatcher->DispatchOutgoingErrorMessage(
                    err,
                    KErrDescrFailedCompress,
                    KScreenshotServiceUid);
            delete iEncodedBitmap;
            iEncodedBitmap = NULL;
            }
        }
    else
        {
        iSeriesShot->Cancel();
        iDispatcher->DispatchOutgoingErrorMessage(
                        anError,
                        KErrDescrFailedConvert,
                        KScreenshotServiceUid);
        delete iEncodedBitmap;
        iEncodedBitmap = NULL;
        }

    if ( iSeriesShot->IsOngoing() )
        {
        iSeriesShot->SaveImage( iEncodedBitmap, iCompress );
        delete iEncodedBitmap;
        iEncodedBitmap = NULL;

        // Check if there's still more to do
        if ( iSeriesShot->IsOngoing() )
            {
            iSeriesShot->TriggerNewShot();
            }
        else
            {
            // - No, timer still active
            // SeriesShot can complete here and in CSeriesShot::TimerExpired
            SeriesShotCompletedL(iSeriesShot->ConstructCompletedMessageL());
            }
        }

    HTI_LOG_FUNC_OUT( "ICLComplete" );
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {
    if ( iEncodedBitmap )
        {
        if ( aAvailableMemory>= iEncodedBitmap->Size() )
            {
            TInt err = iDispatcher->DispatchOutgoingMessage(iEncodedBitmap,
                                KScreenshotServiceUid);

            if ( err == KErrNone)
                {
                iEncodedBitmap = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            else if ( err != KErrNoMemory )
                {
                delete iEncodedBitmap;
                iEncodedBitmap = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            }
        }
    else
        {
        //some error, should not be called
        iDispatcher->RemoveMemoryObserver(this);
        }
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::SeriesShotCompletedL(HBufC8* aMsg)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::SeriesShotCompletedL" );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
                        aMsg,
                        KScreenshotServiceUid) );
    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::SeriesShotCompletedL" );
    }

// ----------------------------------------------------------------------------
TBool CHtiScreenshotServicePlugin::StartShotL(TRect aRegion, TDisplayMode aDisplayMode, TDesC8 &aMimeType)
    {
    HTI_LOG_FUNC_IN( "CHtiScreenshotServicePlugin::StartShot" );
    CreateBitmapL( aRegion, aDisplayMode );

    if ( aMimeType.Length()==0 )
        EncodeBitmapL(); //use default encoder BMP
    else
        EncodeBitmapL( aMimeType );

    HTI_LOG_FUNC_OUT( "CHtiScreenshotServicePlugin::StartShot" );
    return iScreen ? ETrue : EFalse;
    }

// ----------------------------------------------------------------------------
void CHtiScreenshotServicePlugin::SetScreenNumber(TInt aScreenNumber)
    {
    HTI_LOG_FUNC_IN("CHtiScreenshotServicePlugin::SetScreenNumber");
    TInt currentScreen = iScreenDevice->GetScreenNumber();
    HTI_LOG_FORMAT("current screen: %d", currentScreen);
    HTI_LOG_FORMAT("new screen number: %d", aScreenNumber);
    if(aScreenNumber == currentScreen)
        {
        return;
        }

    // Clear the previous delta bitmap to avoid error
    iPreviousBitmap->Reset();
    //delete old screendevice and create a new one
    delete iScreenDevice;
    iScreenDevice = NULL;
    iScreenDevice = new (ELeave) CWsScreenDevice(iWs);
    User::LeaveIfError(iScreenDevice->Construct(aScreenNumber));
    HTI_LOG_FUNC_OUT("CHtiScreenshotServicePlugin::SetScreenNumber");
    }

// ----------------------------------------------------------------------------
CSeriesShot* CSeriesShot::NewL( MSeriesShotObserver* aServicePlugin )
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::NewL" );
    CSeriesShot* self = new (ELeave) CSeriesShot( aServicePlugin );
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CSeriesShot::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
void CSeriesShot::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    }

// ----------------------------------------------------------------------------
CSeriesShot::CSeriesShot( MSeriesShotObserver* aServicePluginObserver ):
    iServicePluginObserver( aServicePluginObserver ),
    iDurationTimer( NULL ),
    iIntervalTimer( NULL ),
    isEncoding( EFalse )
    {
    }

// ----------------------------------------------------------------------------
CSeriesShot::~CSeriesShot()
    {
    Cancel();
    iFs.Close();
    }

// ----------------------------------------------------------------------------
void CSeriesShot::ClearShots()
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::ClearShots" );

    iFs.MkDirAll( KSeriesShotPath );

    // Delete all files
    TFileName files;
    files.Append( KSeriesShotPath );
    files.Append( _L( "*.*" ) );
    HTI_LOG_DES(files);


    CFileMan *fileman = CFileMan::NewL( iFs );
    TInt err = fileman->Delete( files );
    HTI_LOG_FORMAT( "delete %d", err );
    if ( err != KErrNotFound )
        User::LeaveIfError( err );
    delete fileman;


    HTI_LOG_FUNC_OUT( "CSeriesShot::ClearShots" );
    }

// ----------------------------------------------------------------------------
void CSeriesShot::StartL( TTimeIntervalMicroSeconds32 aDuration,
                         TTimeIntervalMicroSeconds32 aInterval,
                         TDisplayMode aDisplayMode,
                         TRect aRegion,
                         TPtrC8 aMime )
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::StartL" );
    HTI_LOG_FORMAT( "Duration      : %d microseconds", aDuration.Int() );
    HTI_LOG_FORMAT( "Interval      : %d microseconds", aInterval.Int() );
    HTI_LOG_FORMAT( "Displaymode   : %d", aDisplayMode );
    HTI_LOG_FORMAT( "TopLeft X     : %d", aRegion.iTl.iX );
    HTI_LOG_FORMAT( "TopLeft Y     : %d", aRegion.iTl.iY );
    HTI_LOG_FORMAT( "BottomRight X : %d", aRegion.iBr.iX );
    HTI_LOG_FORMAT( "BottomRight Y : %d", aRegion.iBr.iY );

    iDisplayMode = aDisplayMode;
    iRegion = aRegion;
    iIndex = 0;

    iMimeType.Zero();
    iMimeType.Append( aMime );
    HTI_LOG_DES( iMimeType );

#ifdef __ENABLE_LOGGING__
    HTI_LOG_TEXT( "Supported MIME types:" );
    RFileExtensionMIMETypeArray array;
    CImageEncoder::GetFileTypesL( array );
    for ( TInt i = 0; i < array.Count(); i++ )
        HTI_LOG_DES( array[i]->MIMEType() );
    array.ResetAndDestroy();
#endif

    iExtension.Zero();
    if ( iMimeType.Length() == 0 )
        iExtension.Append( _L( ".bmp" ) );
    else
        GetMIMEExtension( iMimeType, iExtension );

    ClearShots();

    iDurationTimer = CSeriesShotTimer::NewL( this, EDuration, aDuration );
    iIntervalTimer = CSeriesShotTimer::NewL( this, EInterval, aInterval );
    iDurationTimer->Start();
    TimerExpired( EInterval ); // trigger first shot immidietly

    HTI_LOG_FUNC_OUT( "CSeriesShot::StartL" );
    }

void CSeriesShot::TimerExpired( TInt aId )
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::TimerExpired" );
    switch ( aId )
        {
        case EDuration:
            HTI_LOG_TEXT( "EDuration" );

            delete iDurationTimer;
            iDurationTimer = NULL;

            if ( iIntervalTimer ) // I'm paranoid
                {
                delete iIntervalTimer;
                iIntervalTimer = NULL;
                }
            // SeriesShot can complete here and in CHtiScreenshotServicePlugin::ICLComplete
            if ( isEncoding == EFalse )
                iServicePluginObserver->SeriesShotCompletedL( ConstructCompletedMessageL() );

            break;

        case EInterval:
            HTI_LOG_TEXT( "EInterval" );

            isEncoding = iServicePluginObserver->StartShotL( iRegion, iDisplayMode, iMimeType );

            break;

        default:
            break;
        }
    HTI_LOG_FUNC_OUT( "CSeriesShot::TimerExpired" );
    }

// ----------------------------------------------------------------------------
TBool CSeriesShot::IsOngoing()
    {
    // It still might be encoding when duration timer has expired
    return ( iDurationTimer || isEncoding ) ? ETrue : EFalse;
    }

// ----------------------------------------------------------------------------
void CSeriesShot::SaveImage( TDesC8* aImage, TBool isCompressed )
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::SaveImage" );

    isEncoding = EFalse;

    TFileName filename( KSeriesShotPath );
    filename.AppendFormat( _L( "%04d" ), iIndex );
    iIndex++;
    filename.Append( iExtension );
    if ( isCompressed )
        filename.Append( _L( "z" ) );
    HTI_LOG_DES( filename );

    RFile file;
    User::LeaveIfError( file.Create( iFs, filename, EFileWrite ) );
    User::LeaveIfError( file.Write( *aImage ) );
    file.Close();

    HTI_LOG_FUNC_IN( "CSeriesShot::SaveImage" );
    }

// ----------------------------------------------------------------------------
void CSeriesShot::TriggerNewShot()
    {
    if ( iDurationTimer )
        iIntervalTimer->Start();
    }

// ----------------------------------------------------------------------------
void CSeriesShot::Cancel()
    {
    if ( iDurationTimer )
        {
        delete iDurationTimer;
        iDurationTimer = NULL;
        }
    if ( iIntervalTimer )
        {
        delete iIntervalTimer;
        iIntervalTimer = NULL;
        }
    ClearShots();
    }

// ----------------------------------------------------------------------------
void CSeriesShot::EncodeCompleted()
    {
    isEncoding = EFalse;
    }

// ----------------------------------------------------------------------------
void CSeriesShot::GetMIMEExtension( TDesC8 &aMime, TDes &aExt )
    {
    RFileExtensionMIMETypeArray array;
    CImageEncoder::GetFileTypesL( array );
    for ( TInt i = 0; i < array.Count(); i++ )
        {
        if ( array[i]->MIMEType() == aMime )
            aExt.Append( array[i]->FileExtension() );
        }
    array.ResetAndDestroy();

    if ( aExt == KNullDesC ) // should not happen
        aExt.Append( _L( ".xxx" ) );
    }

// ----------------------------------------------------------------------------
HBufC8* CSeriesShot::ConstructCompletedMessageL()
    {
    HTI_LOG_FUNC_IN( "CSeriesShot::ConstructCompletedMessageL" );
    // Serialshot completed send ok message.

    CDir* dir = NULL;
    User::LeaveIfError( iFs.GetDir(
        KSeriesShotPath, KEntryAttNormal, ESortByName, dir ) );

    TInt msgSize = 0;

    if ( dir->Count() == 0 )
        {
        HTI_LOG_TEXT( "No shots found! Leaving..." );
        User::Leave( KErrNotFound );
        }

    for ( TInt i = 0; i < dir->Count(); i++ )
        {
        msgSize += 1; // for length field
        msgSize += KSeriesShotPath().Length();
        msgSize += (*dir)[i].iName.Length();
        }

    HBufC8* msg = HBufC8::NewL( msgSize );

    for ( TInt i = 0; i < dir->Count(); i++ )
        {
        msg->Des().Append( KSeriesShotPath().Length() + (*dir)[i].iName.Length() );
        msg->Des().Append( KSeriesShotPath );
        msg->Des().Append( (*dir)[i].iName );
        }

    delete dir;

    HTI_LOG_FUNC_OUT( "CSeriesShot::ConstructCompletedMessageL" );
    return msg;
    }

// ----------------------------------------------------------------------------
CSeriesShotTimer* CSeriesShotTimer::NewL( MSeriesShotTimerObserver* aObserver,
                                        TInt aId,
                                        TTimeIntervalMicroSeconds32 aTime )
    {
    HTI_LOG_FUNC_IN( "CSeriesShotTimer::NewL" );
    CSeriesShotTimer* self = new (ELeave) CSeriesShotTimer( aObserver, aId, aTime );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CSeriesShotTimer::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
void CSeriesShotTimer::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CSeriesShotTimer::ConstructL" );
    CTimer::ConstructL();
    if ( !IsAdded() ) // CTimer should add it but it seems that it does NOT!
        {
        CActiveScheduler::Add( this );
        }
    HTI_LOG_FUNC_OUT( "CSeriesShotTimer::ConstructL" );
    }

// ----------------------------------------------------------------------------
CSeriesShotTimer::CSeriesShotTimer( MSeriesShotTimerObserver* aObserver,
                                  TInt aId,
                                  TTimeIntervalMicroSeconds32 aTime ):
    CTimer( EPriorityStandard ),
    iObserver( aObserver ),
    iId( aId ),
    iTime( aTime )
    {
    }

// ----------------------------------------------------------------------------
CSeriesShotTimer::~CSeriesShotTimer()
    {
    }

// ----------------------------------------------------------------------------
void CSeriesShotTimer::RunL()
    {
    iObserver->TimerExpired( iId );
    }

// ----------------------------------------------------------------------------
void CSeriesShotTimer::Start()
    {
    HTI_LOG_FORMAT( "Start CSeriesShotTimer : %d microseconds", iTime.Int() );
    After( iTime );
    }
