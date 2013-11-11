/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#include "gifanimator.h"

#include <s32file.h>
#include <fbs.h>
#include <gdi.h>
#include <imageconversion.h>
#include "enginewrapper.h"

    
// ---------------------------------------------------------------------------

TInt CGifAnimator::CreateGifAnimation(const TDesC& aFileName, TSize aDimensions, CVideoFrameArray* aVideoFrameArray, EngineWrapper& aEngineWrapper)
    {
	CGifAnimator* self = new(ELeave) CGifAnimator;
	CleanupStack::PushL(self);
	TRAPD(err, self->StartL(aFileName, aDimensions, aVideoFrameArray, aEngineWrapper));
	CleanupStack::PopAndDestroy();
	return err;
    }

// ---------------------------------------------------------------------------

CGifAnimator::CGifAnimator()   
    {
    }

// ---------------------------------------------------------------------------

void CGifAnimator::StartL(const TDesC& aFileName, const TSize& aDimensions, CVideoFrameArray* aVideoFrameArray, EngineWrapper& aEngineWrapper)
    {
    
    __ASSERT_ALWAYS(aFileName.Length() > 0, User::Panic(_L("GifAnim"), 100));
    __ASSERT_ALWAYS(aDimensions.iHeight > 0, User::Panic(_L("GifAnim"), 101));
    __ASSERT_ALWAYS(aDimensions.iWidth > 0, User::Panic(_L("GifAnim"), 102));
    __ASSERT_ALWAYS(aVideoFrameArray != NULL, User::Panic(_L("GifAnim"), 103));
    
    
    // show a progress dialog
    aEngineWrapper.ShowProgressBar(aVideoFrameArray->Count()-1);
    
    // open the file for writing
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iOutFile.Replace(iFs, aFileName, EFileWrite));
    
    // write header
    WriteHeaderL(aDimensions);
    
    // process each frame of the animation
    for (TInt i=0; i<aVideoFrameArray->Count(); i++)
        {
        // write headers and raster block
        TVideoFrame frame = aVideoFrameArray->At(i);
        WriteGraphicControlL(frame);
        CFbsBitmap* bitmap = GetBitmapLC(frame, aDimensions);
        WriteImageDescriptorL(frame);
        WriteRasterDataL(bitmap);
        CleanupStack::PopAndDestroy(); //bitmap

        // update the progress bar
        aEngineWrapper.IncrementProgressbarValue();
        }

    // write footer and finish
    WriteFooterL();
    FinishL();
    
    // remove the progress dialog from the screen
    aEngineWrapper.CloseProgressbar();
    }    

// ---------------------------------------------------------------------------

CGifAnimator::~CGifAnimator()
    {
    
    if (iImageEncoder)
        delete iImageEncoder;
    
    if (iGIFImageData)
        delete iGIFImageData;
    
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteHeaderL(const TSize& aDimensions)
    {
    WriteInt8L('G');
    WriteInt8L('I');
    WriteInt8L('F');
    WriteInt8L('8');
    WriteInt8L('9');
    WriteInt8L('a');

    WriteInt16L(aDimensions.iWidth);  // width of animation
    WriteInt16L(aDimensions.iHeight); // height of animation
    
    // logical screen descriptor
    TUint8 packedFlags = 0;
	packedFlags |= 8 - 1; // size of colour table is number of bits in each color table minus one (bits 0-2)
	packedFlags |= (8 - 1) << 4; // colour resolution ie maximum size of the original colour palette (bits 4-6)
	packedFlags |= 0x80; // use global colour table (bit 7)
    
    WriteInt8L(packedFlags);
    
    WriteInt8L(TRANSPARENCY_INDEX); // background color index
    
    WriteInt8L(0); // pixel aspect ratio, 0=not used
    
    // write the Symbian default palette since that's what is used
    CPalette* palette = CPalette::NewDefaultL(EColor256);
    CleanupStack::PushL(palette);
    
    for (TInt i=0; i<palette->Entries(); i++)
        {
        TRgb entry = palette->GetEntry(i);

        WriteInt8L(entry.Red());
        WriteInt8L(entry.Green());
        WriteInt8L(entry.Blue());
        }
    
    CleanupStack::PopAndDestroy(); //palette
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteGraphicControlL(const TVideoFrame& aFrame)
    {
    TInt packedFlags(0);
    
    // enable transparency if needed
    if (aFrame.iEnableTransparency)
        packedFlags |= 0x01; 

    // set disposal method:
    // 0 = disposal method not specified, 1 = do not dispose of graphic,
    // 2 = overwrite graphic with background color, 3 = overwrite graphic with previous graphic
    TInt disposalMethod = 1;
    packedFlags |= ((disposalMethod << 2) & 0x1c);
    
    WriteInt8L(0x21); // GIF extension
    WriteInt8L(0xf9); // GIF graphic control block
    WriteInt8L(0x04); // block size
    WriteInt8L(packedFlags); // packed
    WriteInt16L(aFrame.iDelay); // delay
    WriteInt8L(TRANSPARENCY_INDEX); // transparent color index
    WriteInt8L(0); // block terminator, always 0
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteImageDescriptorL(const TVideoFrame& aFrame)
    {
    WriteInt8L(0x2c); // GIF image descriptor
    WriteInt16L(aFrame.iXPos);
    WriteInt16L(aFrame.iYPos);
    WriteInt16L(aFrame.iWidth);
    WriteInt16L(aFrame.iHeight);
    WriteInt8L(0); // packed flags, none specified in this case
    }
    
// ---------------------------------------------------------------------------

CFbsBitmap* CGifAnimator::GetBitmapLC(TVideoFrame& aFrame, const TSize& aDimensions)
    {
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
    // read the bitmap from the temporary file
    RFile bitmapFile;
    User::LeaveIfError( bitmapFile.Open(iFs, aFrame.iFileStorePath, EFileRead) );
    RFileReadStream readStream(bitmapFile);
    bitmap->InternalizeL(readStream);
    readStream.Close();
    bitmapFile.Close();

    // delete the temporary file since it's not needed anymore
    iFs.Delete(aFrame.iFileStorePath);
    
    // resize the bitmap to match the video dimensions if it is a full screen one
    if (aFrame.iFillsWholeScreen && (aFrame.iWidth != aDimensions.iWidth || aFrame.iHeight != aDimensions.iHeight))
        {
        if (bitmap->Resize(aDimensions) == KErrNone)
            {
            // also update dimensions of this frame to match the dimensions of the video            
            aFrame.iWidth = aDimensions.iWidth;
            aFrame.iHeight = aDimensions.iHeight;
            }
        }
    
    return bitmap;
    }
    
// ---------------------------------------------------------------------------

void CGifAnimator::WriteRasterDataL(CFbsBitmap* aBitmap)
    {
    // reset the encoder
    if (iImageEncoder)
        {
	    delete iImageEncoder;
        iImageEncoder = NULL;
        }
    
    // make sure the buffer for conversion is empty    
    if (iGIFImageData)
        {
	    delete iGIFImageData;
        iGIFImageData = NULL;
        }        

    
    TRequestStatus Stat = KRequestPending;
    
    // init & convert the bitmap to GIF format
    iImageEncoder = CImageEncoder::DataNewL(iGIFImageData, CImageEncoder::EOptionAlwaysThread, KImageTypeGIFUid);
    iImageEncoder->Convert( &Stat, *aBitmap );
    User::WaitForRequest(Stat);
   
   
    // check any erros in converting 
    User::LeaveIfError( Stat.Int() );
    
    // check if we have valid data
    if (iGIFImageData == NULL || iGIFImageData->Des().Length()<793)
        User::Leave(KErrNoMemory);
    
    // in GIF files generated by Symbian, the raster data always starts at offset 791,
    // initial code size in GIF encoding should be 8 since we have a 8bpp image,
    // also check that the second last byte is the terminator 0,
    // if this check fails in newer releases of S60, proper parsing of GIF format is probably needed
    TUint8* imagePtr = &iGIFImageData->Des()[0];
    if (imagePtr[791] != 8 || imagePtr[iGIFImageData->Des().Length()-2] != 0)
        User::Leave(KErrNotSupported);
    
    // write the raster data block to the file
    TUint writeLength = iGIFImageData->Des().Length() - 1 - 791;
    imagePtr+=791;
    TPtr8 writePtr(imagePtr, writeLength, writeLength);
    User::LeaveIfError( iOutFile.Write(writePtr) );
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteFooterL()
    {
    WriteInt8L(0x3b); // GIF trailer
    }
    
// ---------------------------------------------------------------------------

void CGifAnimator::FinishL()
    {
    iOutFile.Close();
    iFs.Close();
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteInt8L(TInt aValue)
    {
    HBufC8* buf = HBufC8::NewMaxLC(1);

    TUint8* ptr = &buf->Des()[0];
    ptr[0] = TUint8(aValue);
    
    User::LeaveIfError( iOutFile.Write(buf->Des()) );
    
    CleanupStack::PopAndDestroy(); // buf
    }

// ---------------------------------------------------------------------------

void CGifAnimator::WriteInt16L(TInt aValue)
    {
    HBufC8* buf = HBufC8::NewMaxLC(2);

    TUint8* ptr = &buf->Des()[0];
    ptr[0] = TUint8(aValue);
    ptr[1] = TUint8(aValue>>8);
        
    User::LeaveIfError( iOutFile.Write(buf->Des()) );
    
    CleanupStack::PopAndDestroy(); // buf
    }

// ---------------------------------------------------------------------------

