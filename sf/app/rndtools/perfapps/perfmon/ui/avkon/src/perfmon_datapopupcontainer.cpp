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
#include "perfmon_datapopupcontainer.h"
#include "perfmon.hrh"
#include "perfmon_document.h"
#include "perfmon_appui.h"
#include "perfmon_model.h"

#include <AknUtils.h>

_LIT(KFreeFormat,"%S free %S%S");

const TInt KLeftMargin = 2;
const TInt KPopupWidth = 94;
const TInt KCPUExtraLength = 8;
const TInt KPopupExtraSpaceBig = 15;
const TInt KPopupExtraSpace = 3;


// ===================================== MEMBER FUNCTIONS =====================================

void CPerfMonDataPopupContainer::ConstructL(const TRect& /*aRect*/)
    {
    iModel = static_cast<CPerfMonDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    iFont = LatinPlain12();
    iFontSize = iFont->FontMaxHeight();
 
    // set windowgroup so that it always on top and does not receive focus
    iWindowGroup = RWindowGroup(iCoeEnv->WsSession());
    User::LeaveIfError(iWindowGroup.Construct((TUint32)&iWindowGroup));
    iWindowGroup.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);
    iWindowGroup.EnableReceiptOfFocus(EFalse);
    
    CreateWindowL(&iWindowGroup);
    //SetRect(aRect);
    SetPositionAndSize();
    SetBlank();

    ActivateL();
    }

// --------------------------------------------------------------------------------------------

CPerfMonDataPopupContainer::~CPerfMonDataPopupContainer()
    {
    iWindowGroup.Close();    
    }
    
// --------------------------------------------------------------------------------------------

void CPerfMonDataPopupContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetBrushColor(KRgbWhite);
    gc.Clear(aRect);
    
    // check if sample array has been constructed
    if (iModel->SampleEntryArray())
        {
        // init font
        gc.SetPenColor(KRgbBlack);
        gc.UseFont( iFont );
        
        // draw a rect around the popup
        gc.DrawRect(aRect);
        
        TInt posCounter(1);
        
        // draw CPU % if enabled
        if (iModel->Settings().iDataPopupSources.iSrcEnabled[ESourceCPU])
            {
            TBuf<64> cpuText;
            cpuText.Copy(_L("CPU: "));
            
            // loop all CPUs
            for (TInt i=iModel->CPU0PositionInSamples(); i<iModel->CPU0PositionInSamples()+iModel->AmountOfCPUs(); i++)
                {
                // check samples available
                if (iModel->SampleEntryArray()->At(i).iSampleDataArray->Count() > 0)
                    {
                    TSampleData& currentSample = iModel->SampleEntryArray()->At(i).iSampleDataArray->At(0);
                    
                    // append % value
                    cpuText.AppendNum( currentSample.iSize > 0 ? TInt( (1 - ((TReal)(currentSample.iFree) / (TReal)currentSample.iSize)) * 100) : 0 );
                    cpuText.Append(_L("% "));
                    }
                }

            gc.DrawText(cpuText, TPoint(KLeftMargin, iFontSize*posCounter));
            posCounter++;
            }
        
        // draw RAM and Drive values
        for (TInt i=iModel->RAMPositionInSamples(); i<iModel->PowerPositionInSamples(); i++)
            {
            // check if this setting has been enabled and it has some data
            if (iModel->Settings().iDataPopupSources.iSrcEnabled[ESourceRAM + i - iModel->RAMPositionInSamples()] && iModel->SampleEntryArray()->At(i).iSampleDataArray->Count() > 0)
                {
                TSampleData& currentSample = iModel->SampleEntryArray()->At(i).iSampleDataArray->At(0);    

                TBuf<32> freeBuf;
                freeBuf.AppendNum(currentSample.iFree, TRealFormat(KDefaultRealWidth, 0));

                TBuf<32> buf;
                buf.Format(KFreeFormat, &iModel->SampleEntryArray()->At(i).iDescription, &freeBuf, &iModel->SampleEntryArray()->At(i).iUnitTypeShort);
                gc.DrawText(buf, TPoint(KLeftMargin, iFontSize*posCounter));
                
                posCounter++;
                }
            }

        // draw power value
        // check if this setting has been enabled and it has some data
        if (iModel->Settings().iDataPopupSources.iSrcEnabled[ESourcePwr] && iModel->SampleEntryArray()->At(iModel->PowerPositionInSamples()).iSampleDataArray->Count() > 0)
            {
            TSampleData& currentSample = iModel->SampleEntryArray()->At(iModel->PowerPositionInSamples()).iSampleDataArray->At(0);    

            TBuf<32> powerText;
            powerText.Copy(_L("Power "));

            powerText.AppendNum(currentSample.iSize - currentSample.iFree, TRealFormat(KDefaultRealWidth, 0));
            powerText.AppendFormat(_L("%S"), &iModel->SampleEntryArray()->At(iModel->PowerPositionInSamples()).iUnitTypeShort);

            gc.DrawText(powerText, TPoint(KLeftMargin,iFontSize*posCounter));

            posCounter++;
            }

        gc.DiscardFont();        
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonDataPopupContainer::SizeChanged()
    {
    DrawNow();
    }   

// --------------------------------------------------------------------------------------------

void CPerfMonDataPopupContainer::SetPositionAndSize()
    {
    CWsScreenDevice* screenDevice = iEikonEnv->ScreenDevice();

    const TInt popupWidth = iModel->AmountOfCPUs() * KCPUExtraLength + KPopupWidth;

    // top right
    if (iModel->Settings().iDataPopupLocation == EDataPopupLocationTopRight)
        {
        // screen orientation is landscape with softkeys on right
        if (AknLayoutUtils::CbaLocation()==AknLayoutUtils::EAknCbaLocationRight)
            {
            SetRect(
                TRect(
                    screenDevice->SizeInPixels().iWidth-popupWidth-KPopupExtraSpaceBig,
                    0,
                    screenDevice->SizeInPixels().iWidth-KPopupExtraSpaceBig,
                    iModel->Settings().iDataPopupSources.EnabledSourcesCount()*iFontSize + KPopupExtraSpace
                    ));
            }

        // any other orientation
        else
            {
            SetRect(
                TRect(
                    screenDevice->SizeInPixels().iWidth-popupWidth,
                    0,
                    screenDevice->SizeInPixels().iWidth,
                    iModel->Settings().iDataPopupSources.EnabledSourcesCount()*iFontSize + KPopupExtraSpace
                    ));
            }        
        }

    // bottom middle
    else if (iModel->Settings().iDataPopupLocation == EDataPopupLocationBottomMiddle)
        {
        SetRect(
            TRect(
                screenDevice->SizeInPixels().iWidth/2-popupWidth/2,
                screenDevice->SizeInPixels().iHeight - iModel->Settings().iDataPopupSources.EnabledSourcesCount()*iFontSize - KPopupExtraSpace,
                screenDevice->SizeInPixels().iWidth/2+popupWidth/2,
                screenDevice->SizeInPixels().iHeight
                ));
        }
    }

// --------------------------------------------------------------------------------------------

void CPerfMonDataPopupContainer::UpdateVisibility(TBool aForeground)
    {
    // application has been brought to foregound
    if (aForeground)
        {
        if (iModel->Settings().iDataPopupVisibility==EDataPopupVisbilityAlwaysOn)
            {
            MakeVisible(ETrue);
            }
        else
            {
            MakeVisible(EFalse);
            }    
        }
    
    // application has been sent to background
    else
        {
        if (iModel->Settings().iDataPopupVisibility==EDataPopupVisbilityAlwaysOn
            || iModel->Settings().iDataPopupVisibility==EDataPopupVisbilityBackgroundOnly)
            {
            MakeVisible(ETrue);
            }
        else
            {
            MakeVisible(EFalse);
            }              
        }    
    }
            
// --------------------------------------------------------------------------------------------

void CPerfMonDataPopupContainer::DrawUpdate()
    {
    DrawDeferred(); 
    }
    
// --------------------------------------------------------------------------------------------
       
// End of File  
