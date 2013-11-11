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
#include "profiler_gui_samplersettingsviewdlg.h"
#include "profiler_gui_model.h"
#include "profiler_gui.hrh"
#include "profiler_gui_std.h"
#include <piprofilerui.rsg>

#include <aknsettingitemlist.h>
#include <CAknMemorySelectionSettingItem.h> 
#include <aknlists.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <aknnotewrappers.h> 

// LITERALS
_LIT8(KCPUSamplerName, "gpp");
_LIT(KEmptySettingItem, "");

// ===================================== MEMBER FUNCTIONS =====================================

CProfilerSamplerSettingsViewDlg* CProfilerSamplerSettingsViewDlg::NewL(TSamplerAttributes& aSettings)
    {
    CProfilerSamplerSettingsViewDlg* self = new(ELeave) CProfilerSamplerSettingsViewDlg(aSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CProfilerSamplerSettingsViewDlg::~CProfilerSamplerSettingsViewDlg()
    {
    // restore default navi pane
    if(iNaviContainer)
        iNaviContainer->Pop();
    
    if (iSettingItemArray)
        {
        iSettingItemArray->ResetAndDestroy();
        delete iSettingItemArray; 
        }
    
    }

// --------------------------------------------------------------------------------------------

CProfilerSamplerSettingsViewDlg::CProfilerSamplerSettingsViewDlg(TSamplerAttributes& aSettings) 
	: 	iSettings(aSettings)
    {
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::ConstructL()
    {
    // construct a menu bar
    CAknDialog::ConstructL(R_PROFILER_SAMPLER_SETTINGS_MENUBAR);
   
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    
    // set empty navi pane label
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    iNaviContainer->PushDefaultL();
    
    TBuf<64> settingsTitle;

    settingsTitle.Zero();
    settingsTitle.Copy(iSettings.iName);
    
    // set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( settingsTitle );

    iItemCount = iSettings.iItemCount;
    
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    switch (aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            ShowSettingPageL(EFalse);
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::TabChangedL(TInt /*aIndex*/)
    {
    iListBox->SetCurrentItemIndex(0);

    SetVisibilitiesOfSettingItemsL();   
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::ProcessCommandL(TInt aCommandId)
    {
    CAknDialog::ProcessCommandL(aCommandId);

    switch (aCommandId)
        {
        case EProfilerGuiCmdSettingsChange:
            ShowSettingPageL(ETrue);
            break;
        case EProfilerGuiCmdSettingsExit:
            TryExitL(EAknCmdExit);
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CProfilerSamplerSettingsViewDlg::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {

    return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }    

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::PreLayoutDynInitL()
    {
    iListBox = static_cast<CAknSettingStyleListBox*>( Control(EProfilerSamplerSettingItemList) );
    iListBox->SetMopParent(this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetListBoxObserver(this);

    iSettingItemArray = new(ELeave) CAknSettingItemArray(16, EFalse, 0);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iSettingItemArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);

    UpdateListBoxL();    
    }

// --------------------------------------------------------------------------------------------

TBool CProfilerSamplerSettingsViewDlg::OkToExitL(TInt aButtonId)
    {
    // save all settings when exiting, only numeric values are needed to saved since copied from TBuf
    for(TInt i(1);i<iItemCount+1;i++)
    	{
    	if(iSettingItemArray->At(i)->IsHidden() == EFalse)
    	    {
            switch(i)
    	        {
                case 1:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem1.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem1.iValue.Num(iItemBuf[i]);
                        }
                    break;
                    }
                case 2:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem2.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem2.iValue.Num(iItemBuf[i]);
                        }
                    break;
                    }
                case 3:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem3.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem3.iValue.Num(iItemBuf[i]);
                        }
                    break;
                    }
                case 4:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem4.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem4.iValue.Num(iItemBuf[i]);
                        }

                    break;
                    }
                case 5:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem5.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem5.iValue.Num(iItemBuf[i]);
                        }

                    break;
                    }
                case 6:
                    {
                    // check if settings value type integer
                    if(iSettings.iSettingItem6.iType == TSettingItem::ESettingItemTypeInt)
                        {
                        iSettings.iSettingItem6.iValue.Num(iItemBuf[i]);
                        }

                    break;
                    }
    	        }
    	    }
    	}
	return CAknDialog::OkToExitL(aButtonId);
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::ShowSettingPageL(TInt aCalledFromMenu)
    {
    TInt listIndex = iListBox->CurrentItemIndex();
    TInt realIndex = iSettingItemArray->ItemIndexFromVisibleIndex(listIndex);
    CAknSettingItem* item = iSettingItemArray->At(realIndex);
    item->EditItemL(aCalledFromMenu);
    item->StoreL();
    SetVisibilitiesOfSettingItemsL();
    DrawNow();
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::SetVisibilitiesOfSettingItemsL()
    {
    if (iSettingItemArray->Count() > 0)
        {
        if(iSettings.iSampleRate > 0)
        	{
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQuerySampleRate])->SetHidden(EFalse);
        	}
        else
        	{
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQuerySampleRate])->SetHidden(ETrue);
        	}
        if(iItemCount>0)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem1])->SetHidden(EFalse);
        if(iItemCount>1)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem2])->SetHidden(EFalse);
        if(iItemCount>2)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem3])->SetHidden(EFalse);
        if(iItemCount>3)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem4])->SetHidden(EFalse);
        if(iItemCount>4)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem5])->SetHidden(EFalse);
        if(iItemCount>5)
        	((*iSettingItemArray)[EProfilerGuiGenericSamplerQueryItem6])->SetHidden(EFalse);

        iSettingItemArray->RecalculateVisibleIndicesL();
        iListBox->HandleItemAdditionL();
        iListBox->UpdateScrollBarsL();
        }
    }   

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::UpdateListBoxL()
    {
    iSettingItemArray->ResetAndDestroy();
    
    // create items
    TInt ordinal(0);
    
    AddSettingItemL(EProfilerGuiGenericSamplerQuerySampleRate,
					R_SAMPLE_RATE_SETTING_TITLE,
                    R_SAMPLER_RATE_SETTING_PAGE,
                    NULL,
                    ordinal++);    
    
    if(iItemCount>0)
	AddSettingItemL(EProfilerGuiGenericSamplerQueryItem1,
					R_ITEM1_SETTING_TITLE,
					NULL,
					NULL,
					ordinal++);
    if(iItemCount>1)
    AddSettingItemL(EProfilerGuiGenericSamplerQueryItem2,
					R_ITEM2_SETTING_TITLE,
					NULL,
                    NULL,
                    ordinal++);
    if(iItemCount>2)
    AddSettingItemL(EProfilerGuiGenericSamplerQueryItem3,
					R_ITEM3_SETTING_TITLE,
					NULL,
                    NULL,
                    ordinal++);
    if(iItemCount>3)
    AddSettingItemL(EProfilerGuiGenericSamplerQueryItem4,
					R_ITEM4_SETTING_TITLE,
					NULL,
                    NULL,
                    ordinal++);
    if(iItemCount>4)
    AddSettingItemL(EProfilerGuiGenericSamplerQueryItem5,
					R_ITEM5_SETTING_TITLE,
					NULL,
                    NULL,
                    ordinal++);
    if(iItemCount>5)
    AddSettingItemL(EProfilerGuiGenericSamplerQueryItem6,
					R_ITEM6_SETTING_TITLE,
					NULL,
                    NULL,
                    ordinal++);

    SetVisibilitiesOfSettingItemsL(); 
    }

// --------------------------------------------------------------------------------------------

CAknSettingItem* CProfilerSamplerSettingsViewDlg::GetSettingItemL(TSettingItem& aItem, TInt aIndex, TInt& aSettingPageResource)
    {
    CAknSettingItem* settingItem = NULL;
    
    // set default setting page dialog style resource
    aSettingPageResource = R_GENERIC_TEXT_SETTING_PAGE;
    
    // check if setting item type integer
    if(aItem.iType == TSettingItem::ESettingItemTypeInt)
        {
        TLex* lex = new TLex(aItem.iValue);
        // check if value valid
        if(lex->Val(iItemBuf[aIndex]) != KErrNone)
            {
            // if value not valid set to 0 
            iItemBuf[aIndex] = 0;
            }
        settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aIndex, iItemBuf[aIndex]);
        aSettingPageResource = R_NUMERIC_SETTING_PAGE;
        delete lex;
        return settingItem;
        }
    else    // else manipulate as a text setting
        {
        settingItem = new(ELeave) CAknTextSettingItem(aIndex, aItem.iValue);
        settingItem->SetEmptyItemTextL(KEmptySettingItem); 
        return settingItem;
        }
    }

// --------------------------------------------------------------------------------------------

void CProfilerSamplerSettingsViewDlg::AddSettingItemL(TInt aId,
                                                 TInt aTitleResource,
                                                 TInt aSettingPageResource,
                                                 TInt aAssociatedResource,
                                                 TInt aOrdinal)
    {
    // create a setting item
    CAknSettingItem* settingItem = NULL;

    TBuf<KMaxSettingUITextLength> ptr;
    switch(aId)
        {
        case EProfilerGuiGenericSamplerQuerySampleRate:
            {
            settingItem = new (ELeave) CAknIntegerEdwinSettingItem(aId, iSettings.iSampleRate);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem1:
            {
            ptr.Copy(iSettings.iSettingItem1.iUIText);
            settingItem = GetSettingItemL(iSettings.iSettingItem1, aId, aSettingPageResource);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem2:
            {
            ptr.Copy(iSettings.iSettingItem2.iUIText); 
            settingItem = GetSettingItemL(iSettings.iSettingItem2, aId, aSettingPageResource);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem3:
            {
            ptr.Copy(iSettings.iSettingItem3.iUIText);
            settingItem = GetSettingItemL(iSettings.iSettingItem3, aId, aSettingPageResource);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem4:
            {
            ptr.Copy(iSettings.iSettingItem4.iUIText); 
            settingItem = GetSettingItemL(iSettings.iSettingItem4, aId, aSettingPageResource);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem5:
            {
            ptr.Copy(iSettings.iSettingItem5.iUIText);
            settingItem = GetSettingItemL(iSettings.iSettingItem5, aId, aSettingPageResource);
            break;
            }
        case EProfilerGuiGenericSamplerQueryItem6:
            {
            ptr.Copy(iSettings.iSettingItem6.iUIText); 
            settingItem = GetSettingItemL(iSettings.iSettingItem6, aId, aSettingPageResource);
            break;
            }
		default:
			{
			// Id not supported
			User::Leave(KErrNotSupported);
			break;
			}
        }

    CleanupStack::PushL(settingItem);

    // get title text
    HBufC* itemTitle;
    
    if( ptr.Compare(KNullDesC) == 0 )
        {
        itemTitle = StringLoader::LoadLC(aTitleResource);
        
        // special case: check if CPU sampler
        if(iSettings.iShortName.CompareF(KCPUSamplerName) == 0)
            {
            // change setting page using different value range, since > 10000 ms rates are too big
            aSettingPageResource = R_CPU_SAMPLER_RATE_SETTING_PAGE;
            }

        // construct the setting item
        settingItem->ConstructL(EFalse, aOrdinal, *itemTitle, NULL, aSettingPageResource,
                                EAknCtPopupSettingList, NULL, aAssociatedResource);
        // append the setting item to settingitem array
        iSettingItemArray->InsertL(aOrdinal, settingItem);

        CleanupStack::PopAndDestroy(); //itemTitle
        }
    else
        {
        // construct the setting item
        settingItem->ConstructL(EFalse, aOrdinal, ptr, NULL, aSettingPageResource,
                                EAknCtPopupSettingList, NULL, aAssociatedResource);

        // append the setting item to settingitem array
        iSettingItemArray->InsertL(aOrdinal, settingItem);
        }

    CleanupStack::Pop(); //settingItem
    }

// --------------------------------------------------------------------------------------------
// End of File

