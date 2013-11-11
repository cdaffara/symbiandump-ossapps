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
#include "profiler_gui_maincontainer.h"
#include "profiler_gui.hrh"
#include "profiler_gui_document.h"
#include "profiler_gui_appui.h"
#include "profiler_gui_model.h"
#include <piprofilerui_extraicons.mbg>

#include <aknlists.h>
#include <eikclb.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <aknnotewrappers.h>
#include <AknIconArray.h> 
#include <f32file.h>
#include <AknIconUtils.h>
#include <AknDef.h>
#include <akntitle.h>
#include <eikspane.h> 

_LIT(KExtraIconsPath, "\\resource\\apps\\piprofilerui_extraicons.mif");

// ===================================== MEMBER FUNCTIONS =====================================

void CProfilerGuiMainContainer::ConstructL(const TRect& aRect)
    {
    iModel = static_cast<CProfilerGuiDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    iModel->SetMainContainer(this);

    CreateWindowL();
    SetRect(aRect);
    SetBlank();
    
    // init listbox
    iListBox = new(ELeave) CAknSingleGraphicStyleListBox;
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, EAknListBoxMarkableList);
    iListBox->View()->SetListEmptyTextL(_L("No plugins found yet"));

    // if description length longer than screen width, scroll the text
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    
    // create icon array and add marking indicator to it
    CAknIconArray* iconArray = new(ELeave) CAknIconArray(1);
    CleanupStack::PushL(iconArray);
    CFbsBitmap* markBitmap = NULL;
    CFbsBitmap* markBitmapMask = NULL;
    
    TRgb defaultColor;
    defaultColor = iEikonEnv->Color(EColorControlText);

    AknsUtils::CreateColorIconL(AknsUtils::SkinInstance(),
            KAknsIIDQgnIndiMarkedAdd,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG13,
            markBitmap,
            markBitmapMask,
            AknIconUtils::AvkonIconFileName(),
            EMbmAvkonQgn_indi_marked_add,
            EMbmAvkonQgn_indi_marked_add_mask,
            defaultColor
            );
     
    CGulIcon* markIcon = CGulIcon::NewL(markBitmap, markBitmapMask);
    iconArray->AppendL(markIcon);                       
    
    // append icons from profiler_gui_extraicons.mif
    TFileName extraIconsPath;
    extraIconsPath.Copy(KExtraIconsPath);
    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName()); // get path where this app is installed
    extraIconsPath.Insert(0, parse.Drive()); // drive letter
    
    CFbsBitmap* redBitmap = NULL;
    CFbsBitmap* redBitmapMask = NULL;
    CFbsBitmap* greenBitmap = NULL;
    CFbsBitmap* greenBitmapMask = NULL;
    
    AknIconUtils::CreateIconL(redBitmap, redBitmapMask, extraIconsPath, EMbmPiprofilerui_extraiconsQgn_prob_piprofilerui_status_disabled, EMbmPiprofilerui_extraiconsQgn_prob_piprofilerui_status_disabled_mask);
    AknIconUtils::CreateIconL(greenBitmap, greenBitmapMask, extraIconsPath, EMbmPiprofilerui_extraiconsQgn_prob_piprofilerui_status_enabled, EMbmPiprofilerui_extraiconsQgn_prob_piprofilerui_status_enabled_mask);

    CGulIcon* redIcon = CGulIcon::NewL(redBitmap, redBitmapMask);
    iconArray->AppendL(redIcon);  
    
    CGulIcon* greenIcon = CGulIcon::NewL(greenBitmap, greenBitmapMask);
    iconArray->AppendL(greenIcon);  

    // set icon array
    CleanupStack::Pop(); // iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetListBoxObserver(this);
    
    // set size of the listbox
    TSize outputRectSize;
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, outputRectSize);
    TRect outputRect(outputRectSize);
    iListBox->SetRect(outputRect);

    iListBox->ActivateL();    
    
    ActivateL();
    
    iInfoPopup = CAknInfoPopupNoteController::NewL();
    iInfoPopup->SetTimePopupInView(5000);
    iInfoPopup->SetTimeDelayBeforeShow(500);
    iInfoPopup->SetTextL(_L("TIP: Check also sampler specific settings!")); 
    iInfoPopup->ShowInfoPopupNote();
    }

// --------------------------------------------------------------------------------------------

CProfilerGuiMainContainer::~CProfilerGuiMainContainer()
    {
    delete iInfoPopup;
    
    if (iListBox)
        delete iListBox;
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::SizeChanged()
{
    TSize outputRectSize;
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, outputRectSize);
    TRect outputRect(outputRectSize);
    
    if (iListBox)
        iListBox->SetRect(outputRect);
}

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiMainContainer::CountComponentControls() const
    {
    if (iListBox)
        return 1;
    else
        return 0;
    }

// --------------------------------------------------------------------------------------------

CCoeControl* CProfilerGuiMainContainer::ComponentControl(TInt /*aIndex*/) const
    {
    if (iListBox)
        return iListBox;
    else
        return NULL;
    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiMainContainer::CurrentListBoxItemIndex()
    {
    if (iListBox)
        {
        return iListBox->CurrentItemIndex();
        }
    else
        return KErrNotFound;    
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::SetListBoxTextArrayL(CDesCArray* aTextArray)
    {
    if (iListBox)
        {    
        iListBox->Model()->SetItemTextArray(aTextArray);
        iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
        iListBox->HandleItemAdditionL();
        iListBox->UpdateScrollBarsL();
        }
    }

// --------------------------------------------------------------------------------------------

const CArrayFix<TInt>* CProfilerGuiMainContainer::ListBoxSelectionIndexes()
    {
    if (iListBox)
        {
        const CListBoxView::CSelectionIndexArray* indices = iListBox->SelectionIndexes();
        return static_cast<const CArrayFix<TInt>*>(indices);
        }
    else
        return NULL;
    }

// --------------------------------------------------------------------------------------------

TInt CProfilerGuiMainContainer::ListBoxSelectionIndexesCount()
    {
    if (iListBox)
        {
        return iListBox->SelectionIndexes()->Count();
        }
    else
        return KErrNotFound;
    }
                        
// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
    }
    
// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::SetDefaultTitlePaneTextL()
    {
    _LIT(KTitleText, "PIProfiler");

    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( KTitleText );
    }
        
// --------------------------------------------------------------------------------------------

TKeyResponse CProfilerGuiMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if(aType != EEventKey)
        return EKeyWasNotConsumed;
    
    if (iListBox && iListBox->Model()->NumberOfItems() > 0)
        {
        // handle OK/Enter keys
        if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
            {
            // check if profiler state is idle
            if(iModel->iState == MProfilerStatusObserver::EIdle)
                {
                // show action menu only if state is idle
                iModel->ShowItemActionMenuL();
                }
            }
        else
            {
            return iListBox->OfferKeyEventL(aKeyEvent, aType);
            }
        }
    
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    switch (aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            iModel->ShowItemActionMenuL();
            }
            break;
        default:
            break;
        }
    }
            
// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::HandleResourceChange(TInt aType)
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);

        TSize outputRectSize;
        AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, outputRectSize);
        TRect outputRect(outputRectSize);
        iListBox->SetRect(outputRect);
        }
    else
        {
        CCoeControl::HandleResourceChange(aType);
        }
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiMainContainer::ShowWriterInfoPopupL(const TDesC& aNote)
	{
	if(!iInfoPopup)
	    {
	    iInfoPopup = CAknInfoPopupNoteController::NewL();
	    }
    // Hide the note. The last note may be visible when creating the second
	iInfoPopup->HideInfoPopupNote();
    iInfoPopup->SetTimePopupInView(5000);
    iInfoPopup->SetTimeDelayBeforeShow(500);
    iInfoPopup->SetTextL(iModel->GetWriterInfoNoteL(aNote));
    iInfoPopup->ShowInfoPopupNote();
	}


// End of File  
