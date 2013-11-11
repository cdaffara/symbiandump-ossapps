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



#ifndef __GIFANIMATOR_H__
#define __GIFANIMATOR_H__

#include <e32std.h>
#include <e32base.h>
#include "sgengine.h"



class CImageEncoder;

class MDialogCallback
    {
public:
    virtual void DialogDismissedL( TInt aButtonId ) = 0;
    };


class CGifAnimator : public CBase 
	{
public:
	static int CreateGifAnimation(const TDesC& aFileName, TSize aDimensions, CVideoFrameArray* aVideoFrameArray, EngineWrapper& aEngineWrapper);
	~CGifAnimator();

private:
    CGifAnimator();
    void StartL(const TDesC& aFileName, const TSize& aDimensions, CVideoFrameArray* aVideoFrameArray, EngineWrapper& aEngineWrapper);
    void WriteHeaderL(const TSize& aDimensions);
    void WriteGraphicControlL(const TVideoFrame& aFrame);
    void WriteImageDescriptorL(const TVideoFrame& aFrame);
    CFbsBitmap* GetBitmapLC(TVideoFrame& aFrame, const TSize& aDimensions);
    void WriteRasterDataL(CFbsBitmap* aBitmap);
    void WriteFooterL();
    void FinishL();
    void WriteInt8L(TInt aValue);
    void WriteInt16L(TInt aValue);
    
    
    
private:
    RFs                         iFs;
    RFile                       iOutFile;
	CImageEncoder*              iImageEncoder;
	HBufC8*                     iGIFImageData;
    
    };

		
	
#endif // __GIFANIMATOR_H__
