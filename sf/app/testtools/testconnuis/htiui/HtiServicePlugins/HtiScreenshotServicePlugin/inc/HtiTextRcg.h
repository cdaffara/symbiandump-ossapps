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
* Description:  Text recognition algorithm implementation.
*
*/


#ifndef __HTI_TEXT_RCG_
#define __HTI_TEXT_RCG_

#include <fbs.h>
#include <gulfont.h>
#include <e32math.h>

const TInt KDefaultAvgDiffMin = 50;
const TInt KDefaultFgSSMin = 50;
const TInt KDefaultFgAvgDiffMin = 5; //in "pure" (not AA) pixels
const TInt KWorstCase = 0xFF;
const TInt KSuccessThresold = 50;
const TInt KNormCoefAcc = 2;

const TInt KDefaultFGAAmount = 5;
const TInt KDefaultBGAAmount = 10;

const TInt KFgPatternColor = 0x00; //black
const TInt KBgPatternColor = 0xff; //white

TInt CompareTPoint(const TPoint& aP1,const TPoint& aP2);

enum THtiTextRecgHints
    {
    EHintNone = 0,
    EHintEdge = 1
    };

/**
* The class implements text recognition algorithm on screenshots
* based on system fonts
*
*/
class CHtiTextRcg : public CBase
{
public:
    CHtiTextRcg();
    virtual ~CHtiTextRcg();

    void SetHint(TInt aHint);

    TBool RecognizeTextL(CFbsBitmap* aScreenshot,
                        const TDesC& aText,
                        const CFont* aFont,
                        TRect& aResult);

    /**
    * aResult will contain the coordinates of the found text
    *  Return 0 if match is perfect, 255 if text was not found
    * return value from 0 to 255 means some error, the lower it is the better match
    *
    */
    //brute force optim
    TInt RecognizeAAL(CFbsBitmap* aScreenshot,const TDesC& aText, const CFont* aFont, TRect& aResult);
    //brute force clean optim
    TInt RecognizeBinL(CFbsBitmap* aScreenshot,const TDesC& aText, const CFont* aFont, TRect& aResult);

    //static image processing routines
    //convert image to 256-greyscale
    static CFbsBitmap* ColorDownL(CFbsBitmap * aBitmap);
    //generates bitmap of the text using the font given

    static CFbsBitmap* GetTextBitmapL( const TDesC& aText,
                            const CFont* fontUsed,
                            const TInt aLength = KMaxTInt );

    static CFbsBitmap* GetTextBitmapL( const TDesC& aText,
                            const CFont* fontUsed,
                            TRgb aForeground,
                            TRgb aBackground,
                            TDisplayMode aDisplayMode,
                            const TInt aLength = KMaxTInt );

    //calculates min max of a greyscale bitmap in question
    static void MinMax(CFbsBitmap * aBitmap, TInt& aMin, TInt& aMax);

    /**
    * use FGA and BGA to speed-up comparation
    * this function only returns 255 or 0
    * Analyses only min and max colors
    */
    TInt ImageDiffBinSampleL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2);

    /**
    * Iterations like in Diff5 but metrics from Diff6
    * Used for full word checking if Diff6 is 0 for a letter
    * this function only returns 255 or 0
    */
    TInt ImageDiffBinFullL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2);

    /**
    * use FGA and BGA to speed-up comparation and AA pixels
    */
    TInt ImageDiffAASampleL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2);

    /**
    * Iterations like in Diff5 but metrics from Diff6/DiffAASample
    * Used for full word checking if Diff6 is 0 for a letter
    */
    TInt ImageDiffAAFullL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2);

    /*
    * Analyze pattern and fills in FGA and BGA sets
    * Return ETrue if pattern is valid (non empty)
    */
    TBool AnalyzePatternL(CFbsBitmap * aPattern);

private:
    //algorithm parameters
    //minimal abs avarage difference beetwen foreground and background
    TInt iAvgDiffMin;
    //minimal SS for foreground, with plain color should be 0
    TInt iFgSSMin;
    /**
    * minimal avg diff for foreground, used in Diff3
    */
    TInt iFgAvgDiffMin;

    //TInt64 iSeed;

    //following vars set by AnalyzePatternL
    TInt iFGAAmount;
    TInt iBGAAmount;
    //foreground assesment point set
    RArray<TPoint> iFGASet;
    //background assesment point set
    RArray<TPoint> iBGASet;

    //font color in reference image
    TInt iMaskFgColor;//
    //background color in reference image
    TInt iMaskBgColor;//

    //used for AA algorithm
    //set in Sample function and used in Full function
    //for optimization
    TInt iTestFgColor;
    TInt iTestBgColor;

    // normalization coeff to compare test image with mask directly
    // |iMaskFgColor-iMaskBgColor|*NormCoefAcc)/|iTestFgColor-iTestBgColor|
    // where NormCoefAcc multiplyer is used to increase accuracy (all ints)
    // and is implemented as shif by KNormCoefAcc bits
    // back shift is performed after color is multiplied by iTestNormCoef
    TInt iTestNormCoef;

    /**
    * Current recognition method
    * Changed by SetHint
    */
    TInt iCurrentStrategy;
};

#endif //__HTI_TEXT_RCG_

// End of file
