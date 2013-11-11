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
* Description:  
*
*/


// INCLUDE FILES
#include "perfmon_graphscontainer.h"
#include "perfmon.hrh"
#include "perfmon_document.h"
#include "perfmon_appui.h"
#include "perfmon_model.h"

#include <AknUtils.h>

const TInt KAmountOfMicroSecondsFitsScreen = 20 * 1000000;
#define KRgbCustomGrey TRgb(0x808080)

_LIT(K100p, "100%"); 
_LIT(K50p, "50%"); 
_LIT(K0p, "0%"); 

_LIT(KPercentageFormat,"%S %d%%");

const TInt KMicroToSecondMultiplier = 1000000;

// ===================================== MEMBER FUNCTIONS =====================================

void CPerfMonGraphsContainer::ConstructL(const TRect& aRect)
    {
    iModel = static_cast<CPerfMonDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    //iFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    iFont = LatinBold12();

    CreateWindowL();
    SetRect(aRect);
    SetBlank();

    ActivateL();
    }

// --------------------------------------------------------------------------------------------

CPerfMonGraphsContainer::~CPerfMonGraphsContainer()
    {
    }
    
// --------------------------------------------------------------------------------------------

void CPerfMonGraphsContainer::Draw(const TRect& aRect) const
    {
    // draw black background
    CWindowGc& gc = SystemGc();
    gc.SetBrushColor(KRgbBlack);
    gc.Clear(aRect);
    
    // activate font and get size
    gc.UseFont(iFont);
    TUint fontSize = iFont->FontMaxHeight();
    //TInt fontBaseOffset = iFont->DescentInPixels();
    

    // calculate time factor
    TReal scaleFactor = (TReal) aRect.Width() / (TReal) KAmountOfMicroSecondsFitsScreen;

    // calculate area height which is used to draw the grpahs
    TInt drawAreaHeight = aRect.Height() - fontSize - fontSize;
        
    
    // check if sample array has been constructed
    if (iModel->SampleEntryArray())
        {
        
        // draw vertical time lines first
        TInt verticalBarPeriodInSecs = iModel->Settings().iGraphsVerticalBarPeriod;
        
        if (verticalBarPeriodInSecs >= 1 && iModel->SampleEntryArray()->At(0).iSampleDataArray->Count() > 0)
            {
            // get time from the first sample
            TSampleData& firstSample = iModel->SampleEntryArray()->At(0).iSampleDataArray->At(0);
            TInt64 currentMicroSeconds = firstSample.iTimeFromStart.Int64();
            
            // calculate amount of microseconds exceeding value by using the modulo operator
            TInt remainderInMicroSeconds = currentMicroSeconds % (verticalBarPeriodInSecs * 1000000); 
            
            // calculate first x pos
            TInt vbarXpos = aRect.Width() - (remainderInMicroSeconds * scaleFactor);
            
            // calculate the amount in seconds
            TInt barSeconds = (currentMicroSeconds - remainderInMicroSeconds) / KMicroToSecondMultiplier;

            
            // continue drawing periodically the vertical lines
            while (vbarXpos >= 0 && barSeconds >= 0)
                {
                // draw vertical line    
                gc.SetPenColor(KRgbDarkRed);
                gc.DrawLine(TPoint(vbarXpos,fontSize+1), TPoint(vbarXpos,aRect.Height()-fontSize));
                
                // draw seconds value
                gc.SetPenColor(KRgbCustomGrey);
                TBuf<16> secsBuf;
                secsBuf.AppendNum(barSeconds);    
                secsBuf.Append(_L("s"));
                gc.DrawText(secsBuf, TPoint(vbarXpos-(iFont->TextWidthInPixels(secsBuf)/2), aRect.Height()));    

                // calculate new position
                vbarXpos -= verticalBarPeriodInSecs * 1000000 * scaleFactor;
                barSeconds -= verticalBarPeriodInSecs;
                }
            }
        
        // draw the basic grid
        gc.SetPenColor(KRgbCustomGrey);
    
        gc.DrawLine(TPoint(0,fontSize), TPoint(aRect.Width(),fontSize));  // upper line
        gc.DrawText(K100p, TPoint(0,fontSize));
    
        gc.DrawLine(TPoint(0,aRect.Height()/2), TPoint(aRect.Width(),aRect.Height()/2));  // mid line
        gc.DrawText(K50p, TPoint(0,aRect.Height()/2));
    
        gc.DrawLine(TPoint(0,aRect.Height()-fontSize), TPoint(aRect.Width(),aRect.Height()-fontSize));  // bottom line
        gc.DrawText(K0p, TPoint(0,aRect.Height()-fontSize));

        TInt c(0);
            
        // draw graphs for each sampled type
        for (TInt i=0; i<iModel->SampleEntryArray()->Count(); i++)
            {
            // check if this setting has been enabled and it has some data
            if (iModel->Settings().iGraphsSources.iSrcEnabled[iModel->SampleEntryPosToSettingPos(i)] && iModel->SampleEntryArray()->At(i).iSampleDataArray->Count() > 0)
                {
                // set pen color for the graph
                gc.SetPenColor(iModel->SampleEntryArray()->At(i).iGraphColor);
                
                // remember the position where drawing started
                /*TReal*/TInt currentXPos(aRect.Width()); // start drawing from right            
                /*TReal*/TInt currentYPos(0);
                
                // draw samples
                for (TInt j=0; j<iModel->SampleEntryArray()->At(i).iSampleDataArray->Count() - 1; j++)
                    {
                    TSampleData& currentSample = iModel->SampleEntryArray()->At(i).iSampleDataArray->At(j);
                    TSampleData& previousSample = iModel->SampleEntryArray()->At(i).iSampleDataArray->At(j+1);
                    
                    // calculate X position for previous (j+1)
                    /*TReal*/TInt previousXPos = currentXPos -
                        ( (Abs(previousSample.iTimeFromStart.Int64() - currentSample.iTimeFromStart.Int64())) * scaleFactor );
                    

                    // calculate initial Y position
                    if (j==0)
                        {
                        currentYPos = currentSample.iSize > 0 ? (TReal)(currentSample.iFree) / (TReal)currentSample.iSize * drawAreaHeight + fontSize : aRect.Height()-fontSize;
                        }

                    // calculate Y position for previous (j+1)
                    /*TReal*/TInt previousYPos = previousSample.iSize > 0 ? (TReal)(previousSample.iFree) / (TReal)previousSample.iSize * drawAreaHeight + fontSize : aRect.Height()-fontSize; 
                    
                    
                    // draw a line between the previous and current
                    gc.DrawLine(TPoint((TInt)previousXPos,(TInt)previousYPos), TPoint((TInt)currentXPos,(TInt)currentYPos));
                    
                    
                    // draw current value in %
                    if (j==0) // draw the value of first sample
                        {
                        TBuf<16> buf;
                        buf.Format(KPercentageFormat, &iModel->SampleEntryArray()->At(i).iDescription, currentSample.iSize > 0 ? TInt( (1 - ((TReal)(currentSample.iFree) / (TReal)currentSample.iSize)) * 100) : 0 );
      
                        gc.DrawText(buf, TPoint(0,fontSize+fontSize+c*fontSize));
                        c++;                    
                        }
                    
                    
                    // stop drawing if we have run out of space
                    if (previousXPos < 0)
                        break;
                    
                    // remeber previous values
                    currentXPos = previousXPos;
                    currentYPos = previousYPos;
                    }
                }
            }
        }

    gc.DiscardFont();        
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CPerfMonGraphsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    return CCoeControl::OfferKeyEventL(aKeyEvent, aType);
    }
        
// --------------------------------------------------------------------------------------------

void CPerfMonGraphsContainer::HandleResourceChange(TInt aType)
    {
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);
        }
    else
        CCoeControl::HandleResourceChange(aType);    
    }

// --------------------------------------------------------------------------------------------

void CPerfMonGraphsContainer::DrawUpdate()
    {
    DrawDeferred();    
    }
    
// --------------------------------------------------------------------------------------------
            
// End of File  
