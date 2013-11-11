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


#include "HtiTextRcg.h"
#include <HtiLogging.h>

const static TInt KDefaultStrategy = EHintEdge;

TInt CompareTPoint(const TPoint& aP1,const TPoint& aP2)
    {
    //this functions is used only to avoid equal points when creating FGA or BGA
    //so only equality of points is important, order doesnt matter
    if ( aP1.iY == aP2.iY )
        return aP1.iX - aP2.iX;
    return aP1.iY - aP2.iY;
    }

CHtiTextRcg::CHtiTextRcg()
    {
    iAvgDiffMin = KDefaultAvgDiffMin;
    //minimal SS for foreground, with plain color should be 0
    iFgSSMin = KDefaultFgSSMin;
    iFgAvgDiffMin = KDefaultFgAvgDiffMin;

    iFGAAmount = KDefaultFGAAmount;
    iBGAAmount = KDefaultBGAAmount;

    SetHint(KDefaultStrategy);
    }

CHtiTextRcg::~CHtiTextRcg()
    {
    iFGASet.Close();
    iBGASet.Close();
    }

void CHtiTextRcg::SetHint(TInt aHint)
    {
    //selects strategy and algorithm parameters
    switch ( aHint )
        {
        case EHintEdge:
            {
            //AA strategy
            iCurrentStrategy = EHintEdge;
            }
            break;
        case EHintNone:
        default:
            {
            //default strategy
            iCurrentStrategy = EHintNone;
            }
        }
    }


TBool CHtiTextRcg::RecognizeTextL(CFbsBitmap* aScreenshot,
                        const TDesC& aText,
                        const CFont* aFont,
                        TRect& aResult)
    {
HTI_LOG_FUNC_IN("RecognizeTextL");
    TInt returnValue = KWorstCase;

    CFbsBitmap* gray = ColorDownL(aScreenshot);
    CleanupStack::PushL(gray);

    switch ( iCurrentStrategy )
        {
        case EHintEdge:
            {
            returnValue = RecognizeAAL(gray,aText, aFont, aResult);
            }
            break;
        case EHintNone:
        default:
            {
            returnValue = RecognizeBinL(gray,aText, aFont, aResult);
            }
        }

    CleanupStack::PopAndDestroy(gray);
HTI_LOG_FUNC_OUT("RecognizeTextL");
    return returnValue < KSuccessThresold;
    }

TInt CHtiTextRcg::RecognizeBinL(CFbsBitmap* aScreenshot,
                            const TDesC& aText,
                            const CFont* aFont,
                            TRect& aResult)
{
    HTI_LOG_FUNC_IN("RecognizeBinL");
    CFbsBitmap* searchFirstLetter = GetTextBitmapL(aText, aFont, 1);
    CleanupStack::PushL(searchFirstLetter);
    HTI_LOG_FORMAT("pattern size w %d", searchFirstLetter->SizeInPixels().iWidth);
    HTI_LOG_FORMAT("pattern size h %d", searchFirstLetter->SizeInPixels().iHeight);

    if ( !AnalyzePatternL(searchFirstLetter) )
        {
        CleanupStack::PopAndDestroy(searchFirstLetter);
        return KWorstCase;
        }
    CFbsBitmap* searchText = GetTextBitmapL(aText, aFont);
    CleanupStack::PushL(searchText);

      //search range (0,0) - (reg.Size() - searchText.SizeInPixels)
    TPoint end(aScreenshot->SizeInPixels().iWidth, aScreenshot->SizeInPixels().iHeight);
    end -= searchText->SizeInPixels();
    end += TPoint(1,1);

    //search itself
    for ( TPoint p( 0, 0 ); p.iY < end.iY; p.iY++ )
        {
        for ( p.iX = 0; p.iX < end.iX; p.iX++ )
            {
            TInt t = ImageDiffBinSampleL(aScreenshot, p, searchFirstLetter);
            if ( t == 0 )
                {
                //check full word
                TInt wordD = ImageDiffBinFullL(aScreenshot, p, searchText);
                if ( wordD == 0 )
                    {
                    aResult.iTl.iX = p.iX;
                    aResult.iTl.iY = p.iY;
                    aResult.SetSize(searchText->SizeInPixels());
                    CleanupStack::PopAndDestroy(searchText);
                    CleanupStack::PopAndDestroy(searchFirstLetter);
                    HTI_LOG_FUNC_OUT("RecognizeBinL");
                    return 0;
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(searchText);
    CleanupStack::PopAndDestroy(searchFirstLetter);

    HTI_LOG_FUNC_OUT("RecognizeBinL");
    return KWorstCase;
}


TInt CHtiTextRcg::RecognizeAAL(CFbsBitmap* aScreenshot,
                            const TDesC& aText,
                            const CFont* aFont,
                            TRect& aResult)
{
    HTI_LOG_FUNC_IN("RecognizeAAL");
    CFbsBitmap* searchFirstLetter = GetTextBitmapL(aText, aFont, 1);
    CleanupStack::PushL(searchFirstLetter);
    if ( !AnalyzePatternL(searchFirstLetter) )
        {
        CleanupStack::PopAndDestroy(searchFirstLetter);
        return KWorstCase;
        }

    CFbsBitmap* searchText = GetTextBitmapL(aText, aFont);
    CleanupStack::PushL(searchText);


    //search range (0,0) - (reg.Size() - searchText.SizeInPixels)
    TPoint end(aScreenshot->SizeInPixels().iWidth, aScreenshot->SizeInPixels().iHeight);
    end -= searchText->SizeInPixels();
    end += TPoint(1,1);

    //search itself
    TInt min = KSuccessThresold;
    TInt wordMin = KSuccessThresold;

    for ( TPoint p( 0, 0 ); p.iY < end.iY; p.iY++ )
        {
        for ( p.iX = 0; p.iX < end.iX; p.iX++ )
            {
            TInt t = ImageDiffAASampleL(aScreenshot, p, searchFirstLetter);
            if ( t < min )
                {
                //check full word
                TInt wordD = ImageDiffAAFullL(aScreenshot, p, searchText);
                if ( wordD < wordMin )
                    {
                    wordMin = wordD;
                    min = t;
                    aResult.iTl.iX = p.iX;
                    aResult.iTl.iY = p.iY;
                    aResult.SetSize(searchText->SizeInPixels());
                    if ( wordMin == 0 )
                        {
                        CleanupStack::PopAndDestroy(searchText);
                        CleanupStack::PopAndDestroy(searchFirstLetter);
                        HTI_LOG_FUNC_OUT("RecognizeAAL");
                        return 0;
                        }
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(searchText);
    CleanupStack::PopAndDestroy(searchFirstLetter);

    HTI_LOG_FUNC_OUT("RecognizeAAL");

    return wordMin;
}

TBool CHtiTextRcg::AnalyzePatternL(CFbsBitmap * aPattern)
{
    HTI_LOG_FUNC_IN("AnalyzePatternL");
    if ( aPattern->SizeInPixels().iWidth == 0 ||
         aPattern->SizeInPixels().iHeight == 0 )
        {
        return EFalse;
        }
    //points are selected as follow
    //take pair of FG-BG points which located next to each other
    MinMax(aPattern,
           iMaskFgColor, //min, black font
           iMaskBgColor);//max, white bg

    if ( iMaskFgColor == iMaskBgColor ) //pattern is empty
        {
        return EFalse;
        }

    TLinearOrder<TPoint> pointOrder(CompareTPoint);
    TSize borders = aPattern->SizeInPixels();
    iFGASet.Reset();
    iBGASet.Reset();

    TBitmapUtil bmpIterator(aPattern);
    //lock bitmap
    bmpIterator.Begin( TPoint(0,0));

    //first take center lines and find take at least two pairs
    //vertical1
    TPoint startPoint(borders.iWidth/2, 0);
    bmpIterator.SetPos(startPoint);

    TInt lastColor = bmpIterator.GetPixel()&0xff;
    TInt lastColorPos = 0;
    bmpIterator.IncYPos();
    TInt i=1;
    TInt found = 0;

    while ( found < 2 && i < borders.iHeight )
        {
        TInt c = bmpIterator.GetPixel()&0xff;

        if ( lastColor != c )
            {
            if ( c == iMaskFgColor )
                {
                iFGASet.InsertInOrder(TPoint(startPoint.iX, i), pointOrder);
                iBGASet.InsertInOrder(TPoint(startPoint.iX, lastColorPos), pointOrder);
                lastColor = c;
                lastColorPos = i;
                ++found;
                }
            else if ( c == iMaskBgColor )
                {
                iBGASet.InsertInOrder(TPoint(startPoint.iX, i), pointOrder);
                iFGASet.InsertInOrder(TPoint(startPoint.iX, lastColorPos), pointOrder);
                lastColor = c;
                lastColorPos = i;
                ++found;
                }
            }
        else
            {
            lastColorPos = i;
            }

        ++i;
        bmpIterator.IncYPos();
        }

    //horizontal1
    startPoint.SetXY(0,borders.iHeight/2);
    bmpIterator.SetPos(startPoint);
    lastColor = bmpIterator.GetPixel()&0xff;
    bmpIterator.IncXPos();
    i=1;
    found = 0;
    lastColorPos = 0;

    while ( found < 2 && i < borders.iWidth )
        {
        TInt c = bmpIterator.GetPixel()&0xff;

        if ( lastColor != c )
            {
            if ( c == iMaskFgColor )
                {
                iFGASet.InsertInOrder(TPoint(i, startPoint.iY), pointOrder);
                iBGASet.InsertInOrder(TPoint(lastColorPos, startPoint.iY), pointOrder);
                lastColor = c;
                lastColorPos = i;
                ++found;
                }
            else if ( c == iMaskBgColor )
                {
                iBGASet.InsertInOrder(TPoint(i, startPoint.iY), pointOrder);
                iFGASet.InsertInOrder(TPoint(lastColorPos, startPoint.iY), pointOrder);
                lastColor = c;
                lastColorPos = i;
                ++found;
                }
            }
        else
            {
            lastColorPos = i;
            }
        ++i;
        bmpIterator.IncXPos();
        }

    //unlock bitmap
    bmpIterator.End();

    iFGAAmount = iFGASet.Count();
    iBGAAmount = iBGASet.Count();

    HTI_LOG_FUNC_OUT("AnalyzePatternL");
    return ETrue;

}

TInt CHtiTextRcg::ImageDiffAASampleL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2)
    {

    if (iFGASet.Count()==0 || iBGASet.Count()==0)
        return KWorstCase;

    TSize aSize = aBitmap2->SizeInPixels();

    //straight average difference
    TBitmapUtil bmpIterator1(aBitmap1);
    TBitmapUtil bmpIterator2(aBitmap2);

    bmpIterator1.Begin( aOrigin1 );

    //1. check FGA points are equal
    bmpIterator1.SetPos( aOrigin1 + iFGASet[0]);
    iTestFgColor = bmpIterator1.GetPixel()&0xff;

    for ( TInt i = 1; i < iFGAAmount;++i )
        {
        bmpIterator1.SetPos( aOrigin1 + iFGASet[i]);
        TInt c = bmpIterator1.GetPixel()&0xff;
        if ( Abs(c-iTestFgColor) > iFgAvgDiffMin )
            {
            bmpIterator2.End();
            bmpIterator1.End();
            return KWorstCase;
            }
        }
    // if we are here all FGA points are equal to colorFGA
    //2. check that avg BGA point value is not equal to colorFGA
    iTestBgColor = 0;
    for ( TInt i = 0; i < iBGAAmount; ++i )
        {
        bmpIterator1.SetPos( aOrigin1 + iBGASet[i]);
        iTestBgColor += bmpIterator1.GetPixel()&0xff;
        }
    iTestBgColor /= iBGAAmount;
    //if difference is too small leave with false
    if ( Abs(iTestBgColor-iTestFgColor) <  iAvgDiffMin )
        {
        bmpIterator2.End();
        bmpIterator1.End();
        return KWorstCase;
        }

    //all checking based on FGA and BGA are correct, chance to have match
    //3. calculate sum of diff between colorFGA and ALL FG points
    bmpIterator1.End();
    bmpIterator1.Begin( aOrigin1 );
    bmpIterator2.Begin( TPoint(0,0), bmpIterator1 );

    TInt nofF = 0;
    TInt sum = 0;
    TBool iterFlag = EFalse;

    TInt rowDelta = 2;
    TInt columnDelta = 1;

    TBool intellFlagBG;
    iTestNormCoef = (Abs(iMaskFgColor-iMaskBgColor)<<KNormCoefAcc)/Abs(iTestFgColor-iTestBgColor);

    for ( TInt i = 0; i < aSize.iHeight; i += rowDelta )
        {
        intellFlagBG = EFalse;
        for ( TInt j = 0; j < aSize.iWidth; j += columnDelta )
            {
            TInt c1 = (bmpIterator1.GetPixel())&0xff;
            TInt c2 = (bmpIterator2.GetPixel())&0xff;

            if ( c2 != iMaskBgColor ) // if foreground
                {
                if ( c2 == iMaskFgColor ) //should be "pure" FG
                    {
                        if (  Abs( c1 - iTestFgColor ) > iFgAvgDiffMin )
                        {
                            bmpIterator2.End();
                            bmpIterator1.End();
                            return KWorstCase;
                        }
                        intellFlagBG = ETrue;
                    }
                else if ( intellFlagBG ) // AA pixels
                    {
                    //calculate diff. in relative diff in aa pixel
                    //in mask and searh image
                    //based on assumtion that aa pixels color
                    // relative to fg color should correlate
                    TInt normD = (Abs(c1-iTestFgColor)*iTestNormCoef)>>KNormCoefAcc;
                    sum += Abs(Abs(iMaskFgColor-c2) - normD );

                    ++nofF;
                    intellFlagBG = EFalse;

                    }
                }
            for ( TInt l = 0; l < columnDelta; l++ )
                {
                if ( iterFlag )
                    {
                    bmpIterator1.DecXPos();
                    bmpIterator2.DecXPos();
                    }
                else
                    {
                    bmpIterator1.IncXPos();
                    bmpIterator2.IncXPos();
                    }
                }
            }

        for ( int k = 0; k < rowDelta; k++ )
        {
            bmpIterator1.IncYPos();
            bmpIterator2.IncYPos();
        }
        for ( int l = 0; l < columnDelta; l++ )
            {
            if ( iterFlag )
                {
                bmpIterator1.IncXPos();
                bmpIterator2.IncXPos();
                }
            else
                {
                bmpIterator1.DecXPos();
                bmpIterator2.DecXPos();
                }
            }
        iterFlag = !iterFlag;
        }


    bmpIterator2.End();
    bmpIterator1.End();

    if ( nofF == 0 )
        {
        return 0;
        }
    return sum / nofF;
    }


TInt CHtiTextRcg::ImageDiffAAFullL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2)
    {
    TSize aSize = aBitmap2->SizeInPixels();
    //straight average difference
    TBitmapUtil bmpIterator1(aBitmap1);
    TBitmapUtil bmpIterator2(aBitmap2);

    bmpIterator1.Begin( aOrigin1 );
    bmpIterator2.Begin( TPoint(0,0), bmpIterator1 );

    TInt nofF = 0;
    TInt sumF = 0;
    TBool intellFlagBG;
    TBool iterFlag = EFalse;
    TInt rowDelta = 2;
    TInt columnDelta = 1;

    for ( TInt i = 0; i < aSize.iHeight; i += rowDelta )
        {
        intellFlagBG = EFalse;
        for ( TInt j = 0; j < aSize.iWidth; j += columnDelta )
            {
            TInt c1 = ( bmpIterator1.GetPixel() ) & 0xff;
            TInt c2 = ( bmpIterator2.GetPixel() ) & 0xff;

            if ( c2 != iMaskBgColor ) // if foreground
                {
                if ( c2 == iMaskFgColor ) //should be pure FG
                    {
                        if (  Abs(c1 - iTestFgColor) > iFgAvgDiffMin )
                        {
                            bmpIterator2.End();
                            bmpIterator1.End();
                            return KWorstCase;
                        }
                        intellFlagBG = ETrue;
                    }
                else if ( intellFlagBG ) // AA pixels
                    {
                    //calculate diff. in relative diff in aa pixel
                    //in mask and searh image
                    //based on assumtion that aa pixels color
                    // relative to fg color should correlate
                    TInt normD = (Abs(c1-iTestFgColor)*iTestNormCoef)>>KNormCoefAcc;
                    sumF += Abs(Abs(iMaskFgColor-c2) - normD );

                    ++nofF;
                    intellFlagBG = EFalse;
                    }
                }
            for ( TInt l = 0; l < columnDelta; l++ )
                {
                if ( iterFlag )
                    {
                    bmpIterator1.DecXPos();
                    bmpIterator2.DecXPos();
                    }
                else
                    {
                    bmpIterator1.IncXPos();
                    bmpIterator2.IncXPos();
                    }
                }
            }

        for ( TInt k = 0; k < rowDelta; k++ )
        {
            bmpIterator1.IncYPos();
            bmpIterator2.IncYPos();
        }
        for ( TInt l = 0; l < columnDelta; l++ )
            {
            if ( iterFlag )
                {
                bmpIterator1.IncXPos();
                bmpIterator2.IncXPos();
                }
            else
                {
                bmpIterator1.DecXPos();
                bmpIterator2.DecXPos();
                }
            }
        iterFlag = !iterFlag;
        }
    bmpIterator2.End();
    bmpIterator1.End();

    if ( nofF == 0 )
        return 0;

    return sumF/nofF;
    }

TInt CHtiTextRcg::ImageDiffBinSampleL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2)
    {
    TSize aSize = aBitmap2->SizeInPixels();
    if ( iFGASet.Count() == 0 || iBGASet.Count() == 0 )
        return KWorstCase;

    //straight average difference
    TBitmapUtil bmpIterator1(aBitmap1);
    TBitmapUtil bmpIterator2(aBitmap2);

    bmpIterator1.Begin( aOrigin1 );

    //1. check FGA points are equal
    bmpIterator1.SetPos( aOrigin1 + iFGASet[0]);
    TInt colorFGA = bmpIterator1.GetPixel()&0xff;

    for ( TInt i = 1; i < iFGAAmount; ++i )
        {
        bmpIterator1.SetPos( aOrigin1 + iFGASet[i] );
        TInt c = bmpIterator1.GetPixel()&0xff;
        if ( c != colorFGA )
            {
            bmpIterator2.End();
            bmpIterator1.End();
            return KWorstCase;
            }
        }
    // if we are here all FGA points are equal to colorFGA
    //2. check that avg BGA point value is not equal to colorFGA
    TInt avgColorBGA = 0;
    for ( TInt i = 0; i < iBGAAmount; ++i )
        {
        bmpIterator1.SetPos( aOrigin1 + iBGASet[i] );
        avgColorBGA += bmpIterator1.GetPixel() & 0xff;
        }
    avgColorBGA /= iBGAAmount;
    //if difference is too small leave with false
    if ( Abs(avgColorBGA-colorFGA) <  iAvgDiffMin )
        {
        bmpIterator2.End();
        bmpIterator1.End();
        return KWorstCase;
        }

    //all checking based on FGA and BGA are correct, chance to have math
    //3. calculate sum of diff between colorFGA and ALL FG points
    bmpIterator1.End();
    bmpIterator1.Begin( aOrigin1 );
    bmpIterator2.Begin( TPoint(0,0), bmpIterator1 );

    TBool iterFlag = EFalse;

    TInt rowDelta = 1;
    TInt columnDelta = 1;

    for ( TInt i = 0; i < aSize.iHeight; i += rowDelta )
        {
        for ( TInt j = 0; j < aSize.iWidth; j += columnDelta )
            {
            TInt c1 = ( bmpIterator1.GetPixel() ) & 0xff;
            TInt c2 = ( bmpIterator2.GetPixel() ) & 0xff;

            if ( c2 == iMaskFgColor ) // if foreground
            {
                if ( colorFGA != c1 )
                {
                    bmpIterator2.End();
                    bmpIterator1.End();
                    return KWorstCase;
                }
            }
            for ( TInt l = 0; l < columnDelta; l++ )
                {
                if ( iterFlag )
                    {
                    bmpIterator1.DecXPos();
                    bmpIterator2.DecXPos();
                    }
                else
                    {
                    bmpIterator1.IncXPos();
                    bmpIterator2.IncXPos();
                    }
                }
            }

        for ( TInt k = 0; k < rowDelta; k++ )
        {
            bmpIterator1.IncYPos();
            bmpIterator2.IncYPos();
        }
        for ( TInt l = 0; l < columnDelta; l++ )
            {
            if ( iterFlag )
                {
                bmpIterator1.IncXPos();
                bmpIterator2.IncXPos();
                }
            else
                {
                bmpIterator1.DecXPos();
                bmpIterator2.DecXPos();
                }
            }
        iterFlag = !iterFlag;
        }


    bmpIterator2.End();
    bmpIterator1.End();

    return 0;
    }

TInt CHtiTextRcg::ImageDiffBinFullL(CFbsBitmap * aBitmap1, TPoint aOrigin1,
                 CFbsBitmap * aBitmap2)
    {
    TSize aSize = aBitmap2->SizeInPixels();
    //straight average difference
    TBitmapUtil bmpIterator1(aBitmap1);
    TBitmapUtil bmpIterator2(aBitmap2);

    bmpIterator1.Begin( aOrigin1 );
    bmpIterator2.Begin( TPoint(0,0), bmpIterator1 );

    //TInt nofF = 0;
    TInt nofB = 0;

    TInt sumB = 0;
    //TInt sumF = 0;
    TBool intellFlagBG;
    TBool iterFlag = EFalse;
    TInt rowDelta = 1;
    TInt columnDelta = 1;
    TInt fgColor = -1;
    for ( TInt i = 0; i < aSize.iHeight; i += rowDelta )
        {
        intellFlagBG = EFalse;
        for ( TInt j = 0; j < aSize.iWidth; j += columnDelta )
            {
            TInt c1 = ( bmpIterator1.GetPixel() ) & 0xff;
            TInt c2 = ( bmpIterator2.GetPixel() ) & 0xff;

            if ( c2 == iMaskFgColor ) // if FG
            {
                if ( c1 != fgColor )
                    {
                    if ( fgColor != -1 )
                        {
                        //failed
                        bmpIterator2.End();
                        bmpIterator1.End();
                        return KWorstCase;
                        }
                    else
                        {
                        fgColor = c1; //init fgColor
                        }
                    }
                intellFlagBG = ETrue;
            }
            else if ( c2 == iMaskBgColor && intellFlagBG )
            {
                sumB += c1;
                ++nofB;
                intellFlagBG = EFalse;
            }
            for ( TInt l = 0; l < columnDelta; l++ )
                {
                if ( iterFlag )
                    {
                    bmpIterator1.DecXPos();
                    bmpIterator2.DecXPos();
                    }
                else
                    {
                    bmpIterator1.IncXPos();
                    bmpIterator2.IncXPos();
                    }
                }
            }

        for ( TInt k = 0; k < rowDelta; k++ )
        {
            bmpIterator1.IncYPos();
            bmpIterator2.IncYPos();
        }
        for ( TInt l = 0; l < columnDelta; l++ )
            {
            if ( iterFlag )
                {
                bmpIterator1.IncXPos();
                bmpIterator2.IncXPos();
                }
            else
                {
                bmpIterator1.DecXPos();
                bmpIterator2.DecXPos();
                }
            }
        iterFlag = !iterFlag;
        }
    bmpIterator2.End();
    bmpIterator1.End();

    if ( nofB == 0 ) //something wrong, should be some BG
        return KWorstCase;

    TInt avgB = sumB / ( nofB );

    if ( Abs( fgColor - avgB ) < iAvgDiffMin )
        {
        return KWorstCase;
        }

    return 0;
    }

void CHtiTextRcg::MinMax(CFbsBitmap * aBitmap, TInt& aMin, TInt& aMax)
    {
    //straight average difference
    TSize aSize = aBitmap->SizeInPixels();
    TBitmapUtil bmpIterator(aBitmap);

    bmpIterator.Begin( TPoint(0,0) );

    aMin = KMaxTInt;
    aMax = -1;
    for ( TInt i = 0; i < aSize.iHeight; ++i )
        {
        for ( TInt j = 0; j < aSize.iWidth; ++j )
            {
            TInt c = ( bmpIterator.GetPixel() ) & 0xff;

            if ( c < aMin )
                {
                aMin = c;
                }
            else if ( c > aMax )
                {
                aMax = c;
                }

            if ( i & 1 )
                {
                bmpIterator.DecXPos();
                }
            else
                {
                bmpIterator.IncXPos();
                }
            }
        bmpIterator.IncYPos();

        if ( i & 1 )
            {
            bmpIterator.IncXPos();
            }
        else
            {
            bmpIterator.DecXPos();
            }
        }

    bmpIterator.End();
    }

CFbsBitmap* CHtiTextRcg::ColorDownL( CFbsBitmap * aBitmap )
    {
    TSize bmpSize = aBitmap->SizeInPixels();
    CFbsBitmap* result = new ( ELeave )  CFbsBitmap();
    User::LeaveIfError( result->Create( bmpSize, EGray256 ) );

    TBitmapUtil srcBmpIterator( aBitmap );
    TBitmapUtil resultBmpIterator( result );

    srcBmpIterator.Begin( TPoint( 0, 0 ) );
    resultBmpIterator.Begin( TPoint( 0, 0 ), srcBmpIterator );

    TPoint point( 0, 0 );
    for ( point.iY = 0; point.iY < bmpSize.iHeight; ++point.iY )
        {
        point.iX = 0;
        srcBmpIterator.SetPos( point );
        resultBmpIterator.SetPos( point );
        for ( ; point.iX < bmpSize.iWidth; ++point.iX )
            {
            TUint32 c = srcBmpIterator.GetPixel();
            TRgb col( c );
            resultBmpIterator.SetPixel( col.Gray256() );
            srcBmpIterator.IncXPos();
            resultBmpIterator.IncXPos();
            }
        }

    resultBmpIterator.End();
    srcBmpIterator.End();

    return result;
    }

CFbsBitmap* CHtiTextRcg::GetTextBitmapL( const TDesC& aText,
                                         const CFont* fontUsed,
                                         const TInt aLength )
    {
    return GetTextBitmapL( aText, fontUsed, KRgbBlack, KRgbWhite,
            EGray256, aLength );
    }



CFbsBitmap* CHtiTextRcg::GetTextBitmapL( const TDesC& aText,
                                    const CFont* fontUsed,
                                    TRgb aForeground,
                                    TRgb aBackground,
                                    TDisplayMode aDisplayMode,
                                    const TInt aLength )
{
    HTI_LOG_FUNC_IN( "CHtiTextRcg::GetTextBitmapL" )
    // Measure the text to get needed bitmap size and baseline point
    CFont::TMeasureTextOutput output;
    TInt reqWidth = fontUsed->MeasureText( aText, NULL, &output );
    reqWidth = Max( reqWidth, output.iBounds.Width() );

    // If only partial text requested, calculate new width but keep the
    // height (and baseline) as it needs to be the same as for the full text
    // for the text recognition to work.
    if ( aLength < aText.Length() )
        {
        CFont::TMeasureTextOutput partialOutput;
        reqWidth = fontUsed->MeasureText( aText.Left( aLength ), NULL,
                &partialOutput );
        reqWidth = Max( reqWidth, partialOutput.iBounds.Width() );
        }

    TSize bmpSize( reqWidth, output.iBounds.Height() );
    HTI_LOG_FORMAT( "Bitmap width = %d", bmpSize.iWidth );
    HTI_LOG_FORMAT( "Bitmap height = %d", bmpSize.iHeight );

    // Create the bitmap
    CFbsBitmap* result = new ( ELeave ) CFbsBitmap();
    User::LeaveIfError( result->Create( bmpSize, aDisplayMode ) );

    CFbsBitGc* bitmapContext = NULL;
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( result );
    CleanupStack::PushL( bitmapDevice );
    User::LeaveIfError( bitmapDevice->CreateContext( bitmapContext ) );
    CleanupStack::PushL( bitmapContext );
    bitmapContext->SetBrushColor( aBackground );
    bitmapContext->Clear();
    bitmapContext->UseFont( fontUsed );
    bitmapContext->SetPenColor( aForeground );

    // Set the baseline point and draw the text
    TPoint pos( 0, bmpSize.iHeight - output.iBounds.iBr.iY );
    HTI_LOG_FORMAT( "Baseline Y = %d", pos.iY );
    if ( aLength < aText.Length() )
        {
        bitmapContext->DrawText( aText.Left( aLength ), pos );
        }
    else
        {
        bitmapContext->DrawText( aText, pos );
        }

    CleanupStack::PopAndDestroy( 2 );
    HTI_LOG_FUNC_OUT( "CHtiTextRcg::GetTextBitmapL" )
    return result;
}


// End of file
