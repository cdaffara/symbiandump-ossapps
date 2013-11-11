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


// INCLUDE FILES
#include "loadgen_maincontainer.h"
#include "loadgen.hrh"
#include "loadgen_document.h"
#include "loadgen_appui.h"
#include "loadgen_model.h"
#include <loadgen_extraicons.mbg>

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

_LIT(KExtraIconsPath, "\\resource\\apps\\loadgen_extraicons.mif");

// ===================================== MEMBER FUNCTIONS =====================================

void CLoadGenMainContainer::ConstructL(const TRect& aRect)
    {
    iModel = static_cast<CLoadGenDocument*>(reinterpret_cast<CEikAppUi*>(iEikonEnv->AppUi())->Document())->Model();
    iModel->SetMainContainer(this);

    CreateWindowL();
    SetRect(aRect);
    SetBlank();
    
    // init listbox
    iListBox = new(ELeave) CLoadGenCAknSingleGraphicStyleListBox;
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, EAknListBoxMarkableList);
    iListBox->View()->SetListEmptyTextL(_L("No loads activated\n(Select new load from Options to generate new load)"));

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
    
    // append icons from loadgen_extraicons.mif
    TFileName extraIconsPath;
    extraIconsPath.Copy(KExtraIconsPath);
    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName()); // get path where this app is installed
    extraIconsPath.Insert(0, parse.Drive()); // drive letter
    
    CFbsBitmap* redBitmap = NULL;
    CFbsBitmap* redBitmapMask = NULL;
    CFbsBitmap* greenBitmap = NULL;
    CFbsBitmap* greenBitmapMask = NULL;
    
    AknIconUtils::CreateIconL(redBitmap, redBitmapMask, extraIconsPath, EMbmLoadgen_extraiconsQgn_prob_status_red, EMbmLoadgen_extraiconsQgn_prob_status_red_mask);
    AknIconUtils::CreateIconL(greenBitmap, greenBitmapMask, extraIconsPath, EMbmLoadgen_extraiconsQgn_prob_status_green, EMbmLoadgen_extraiconsQgn_prob_status_green_mask);

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
    }

// --------------------------------------------------------------------------------------------

CLoadGenMainContainer::~CLoadGenMainContainer()
    {
    if (iListBox)
        delete iListBox;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::SizeChanged()
{
    TSize outputRectSize;
    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, outputRectSize);
    TRect outputRect(outputRectSize);
    
    if (iListBox)
        iListBox->SetRect(outputRect);
}

// --------------------------------------------------------------------------------------------

TInt CLoadGenMainContainer::CountComponentControls() const
    {
    if (iListBox)
        return 1;
    else
        return 0;
    }

// --------------------------------------------------------------------------------------------

CCoeControl* CLoadGenMainContainer::ComponentControl(TInt /*aIndex*/) const
    {
    if (iListBox)
        return iListBox;
    else
        return NULL;
    }

// --------------------------------------------------------------------------------------------

TInt CLoadGenMainContainer::CurrentListBoxItemIndex()
    {
    if (iListBox)
        {
        return iListBox->CurrentItemIndex();
        }
    else
        return KErrNotFound;    
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::SetListBoxTextArrayL(CDesCArray* aTextArray)
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

const CArrayFix<TInt>* CLoadGenMainContainer::ListBoxSelectionIndexes()
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

TInt CLoadGenMainContainer::ListBoxSelectionIndexesCount()
    {
    if (iListBox)
        {
        return iListBox->SelectionIndexes()->Count();
        }
    else
        return KErrNotFound;
    }
                        
// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
    }
    
// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::SetDefaultTitlePaneTextL()
    {
    _LIT(KTitleText, "Load Gener.");

    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( KTitleText );
    }
        
// --------------------------------------------------------------------------------------------

TKeyResponse CLoadGenMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if(aType != EEventKey)
        return EKeyWasNotConsumed;
    
    if (iListBox && iListBox->Model()->NumberOfItems() > 0)
        {
        /*
        if (aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete)
            {
            iModel->StopSelectedOrHighlightedItemsL();
            return EKeyWasConsumed;
            }
        else    
            {
            return iListBox->OfferKeyEventL(aKeyEvent, aType);
            }
        */
        TBool shiftKeyPressed = (aKeyEvent.iModifiers & EModifierShift) ||
                                (aKeyEvent.iModifiers & EModifierLeftShift) ||
                                (aKeyEvent.iModifiers & EModifierRightShift);
        TBool controlKeyPressed = (aKeyEvent.iModifiers & EModifierCtrl) || 
                                  (aKeyEvent.iModifiers & EModifierRightCtrl); 
                                  
        
        // handle OK/Enter keys
        if ((aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) && (shiftKeyPressed || controlKeyPressed))
            {
            return iListBox->OfferKeyEventL(aKeyEvent, aType); //send to listbox if modifiers
            }

        // handle OK/Enter keys
        else if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
            {
            iModel->ShowItemActionMenuL();
            }

        // handle delete key
        else if (aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete)
            {
            iModel->StopSelectedOrHighlightedItemsL();
            }

        else
            return iListBox->OfferKeyEventL(aKeyEvent, aType);
        }
    
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenMainContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
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

void CLoadGenMainContainer::HandleResourceChange(TInt aType)
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
// --------------------------------------------------------------------------------------------

void CLoadGenCAknSingleGraphicStyleListBox::SizeChanged()
    {
    // call the base class function first
    CAknSingleGraphicStyleListBox::SizeChanged();

    // customize the list box
    TRAP_IGNORE(SizeChangedL());
    }

// --------------------------------------------------------------------------------------------

void CLoadGenCAknSingleGraphicStyleListBox::SizeChangedL()
    {
    const TInt KEmptySize = 2;
    
    // disable separator line
    ItemDrawer()->ColumnData()->SetSeparatorLinePosition(ENoLine);
    
    // enable marquee
    ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    
    // set fonts
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    ItemDrawer()->ColumnData()->SetColumnFontL(0, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(1, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(2, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(3, font);

    // set row height
    TInt rowHeight = font->HeightInPixels()+5;    
    TSize itemCellSize = View()->ItemDrawer()->ItemCellSize();
    itemCellSize.iHeight = rowHeight; 
    SetItemHeightL(rowHeight);
    View()->ItemDrawer()->SetItemCellSize(itemCellSize);
    
    // set icon sizes
    TInt normalIconSize = rowHeight * 0.7;
    TInt markingIconSize = rowHeight * 0.8;
    ItemDrawer()->ColumnData()->SetSubCellIconSize(0, TSize(normalIconSize,normalIconSize));
    ItemDrawer()->ColumnData()->SetSubCellIconSize(2, TSize(markingIconSize,markingIconSize));
    ItemDrawer()->ColumnData()->SetSubCellIconSize(3, TSize(KEmptySize,KEmptySize));

    // set column widths
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(0, normalIconSize + 2*normalIconSize * 0.15);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(1, itemCellSize.iWidth-KEmptySize-markingIconSize);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(2, markingIconSize);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(3, KEmptySize);

    // set baseline for the text
    ItemDrawer()->ColumnData()->SetColumnBaselinePosL(1, font->HeightInPixels()-font->BaselineOffsetInPixels());

    // set margins
    TMargins iconMargins;
    iconMargins.iBottom = 0;
    iconMargins.iLeft = normalIconSize * 0.15;
    iconMargins.iRight = normalIconSize * 0.15;
    iconMargins.iTop = 0;

    TMargins textMargins;
    textMargins.iBottom = 0;
    textMargins.iLeft = markingIconSize * 0.15;
    textMargins.iRight = markingIconSize * 0.15;
    textMargins.iTop = 0;

    ItemDrawer()->ColumnData()->SetColumnMarginsL(0, iconMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(1, textMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(2, iconMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(3, iconMargins);
    
/*
    const TInt KEmptySize = 2;
    
    // enable marquee
    ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    
    // set fonts
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
    ItemDrawer()->ColumnData()->SetColumnFontL(0, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(1, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(2, font);
    ItemDrawer()->ColumnData()->SetColumnFontL(3, font);

    // set row height
    TInt rowHeight = font->HeightInPixels()+7;    
    TSize itemCellSize = View()->ItemDrawer()->ItemCellSize();
    itemCellSize.iHeight = rowHeight; 
    SetItemHeightL(rowHeight);
    View()->ItemDrawer()->SetItemCellSize(itemCellSize);

    // set icon sizes
    TInt normalIconSize = rowHeight * 0.6;
    TInt markingIconSize = rowHeight * 0.8;
    ItemDrawer()->ColumnData()->SetSubCellIconSize(0, TSize(normalIconSize,normalIconSize));
    ItemDrawer()->ColumnData()->SetSubCellIconSize(2, TSize(markingIconSize,markingIconSize));
    ItemDrawer()->ColumnData()->SetSubCellIconSize(3, TSize(KEmptySize,KEmptySize));

    // set column widths
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(0, normalIconSize);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(1, itemCellSize.iWidth-KEmptySize-markingIconSize);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(2, markingIconSize);
    ItemDrawer()->ColumnData()->SetColumnWidthPixelL(3, KEmptySize);

    // set baseline for the text
    ItemDrawer()->ColumnData()->SetColumnBaselinePosL(1, font->HeightInPixels()-font->BaselineOffsetInPixels());

    // set margins
    TMargins iconMargins;
    iconMargins.iBottom = 0;
    iconMargins.iLeft = 0;
    iconMargins.iRight = 0;
    iconMargins.iTop = 0;

    TMargins textMargins;
    textMargins.iBottom = 0;
    textMargins.iLeft = markingIconSize * 0.15;
    textMargins.iRight = markingIconSize * 0.15;
    textMargins.iTop = 0;

    ItemDrawer()->ColumnData()->SetColumnMarginsL(0, iconMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(1, textMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(2, iconMargins);
    ItemDrawer()->ColumnData()->SetColumnMarginsL(3, iconMargins);
*/
    }

// --------------------------------------------------------------------------------------------
        
// End of File  
