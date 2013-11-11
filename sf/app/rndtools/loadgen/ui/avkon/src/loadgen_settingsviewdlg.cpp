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
#include "loadgen_settingsviewdlg.h"
#include "loadgen_model.h"
#include "loadgen.hrh"
#include "loadgen_std.h"
#include <loadgen.rsg>

#include <aknsettingitemlist.h>
#include <CAknMemorySelectionSettingItem.h> 
#include <aknlists.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <aknnotewrappers.h> 

// ===================================== MEMBER FUNCTIONS =====================================

CLoadGenSettingsViewDlg* CLoadGenSettingsViewDlg::NewL(TLoadGenSettings& aSettings)
    {
    CLoadGenSettingsViewDlg* self = new(ELeave) CLoadGenSettingsViewDlg(aSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CLoadGenSettingsViewDlg::~CLoadGenSettingsViewDlg()
    {
    // restore default navi pane
    if (iNaviContainer)
        iNaviContainer->Pop();
    
    if (iSettingItemArray)
        {
        iSettingItemArray->ResetAndDestroy();
        delete iSettingItemArray; 
        }
    
    delete iDecoratedTabGroup;   
    }

// --------------------------------------------------------------------------------------------

CLoadGenSettingsViewDlg::CLoadGenSettingsViewDlg(TLoadGenSettings& aSettings) : iSettings(aSettings)
    {
    }

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::ConstructL()
    {
    // construct a menu bar
    CAknDialog::ConstructL(R_LOADGEN_SETTINGS_MENUBAR);
   
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    
    // set empty navi pane label
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    //iNaviContainer->PushDefaultL();
    
    // set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( _L("Settings") );
    
    // create a tab group
    iDecoratedTabGroup = iNaviContainer->CreateTabGroupL(this);
    iTabGroup = static_cast<CAknTabGroup*>(iDecoratedTabGroup->DecoratedControl());
    iTabGroup->SetTabFixedWidthL(KTabWidthWithTwoLongTabs); 
    
    // add tabs
    //iTabGroup->AddTabL(ETabGeneral, _L("General"));
    //iTabGroup->AddTabL(ETabScreenShot, _L("Screen shot"));
    //iTabGroup->SetActiveTabByIndex(ETabGeneral);
    
    iNaviContainer->PushL( *iDecoratedTabGroup );
    }

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
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

void CLoadGenSettingsViewDlg::TabChangedL(TInt /*aIndex*/)
    {
    iListBox->SetCurrentItemIndex(0);

    SetVisibilitiesOfSettingItemsL();   
    }

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::ProcessCommandL(TInt aCommandId)
    {
    CAknDialog::ProcessCommandL(aCommandId);

    switch (aCommandId)
        {
        case ELoadGenCmdSettingsChange:
            ShowSettingPageL(ETrue);
            break;
        case ELoadGenCmdSettingsExit:
            TryExitL(EAknCmdExit);
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CLoadGenSettingsViewDlg::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (iTabGroup == NULL)
        {
        return EKeyWasNotConsumed;
        }

    TInt active = iTabGroup->ActiveTabIndex();
    TInt count = iTabGroup->TabCount();

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            if (active > 0)
                {
                active--;
                iTabGroup->SetActiveTabByIndex(active);
                TabChangedL(active); 
                }
            break;
        
        case EKeyRightArrow:
            if((active + 1) < count)
                {
                active++;
                iTabGroup->SetActiveTabByIndex(active);
                TabChangedL(active); 
                }
            break;
        }

    return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }    

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::PreLayoutDynInitL()
    {
    iListBox = static_cast<CAknSettingStyleListBox*>( Control(ELoadGenSettingItemList) );
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

TBool CLoadGenSettingsViewDlg::OkToExitL(TInt aButtonId)
    {
    return CAknDialog::OkToExitL(aButtonId);
    }

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::ShowSettingPageL(TInt aCalledFromMenu)
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

void CLoadGenSettingsViewDlg::SetVisibilitiesOfSettingItemsL()
    {
    if (iSettingItemArray->Count() > 0)
        {
/*
        switch (iTabGroup->ActiveTabIndex())
            {

            case ETabGeneral:
                {
                ((*iSettingItemArray)[ESettingListDisplayModeSelection])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListFileViewModeSelection])->SetHidden(EFalse);
                break;
                }
            
            case ETabScreenShot:
                {
                ((*iSettingItemArray)[ESettingListDisplayModeSelection])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListFileViewModeSelection])->SetHidden(ETrue);
                break;            
                }

            default:
                User::Panic(_L("TabIOOB"), 50);
                break;
            }
*/
        iSettingItemArray->RecalculateVisibleIndicesL();
        iListBox->HandleItemAdditionL();
        iListBox->UpdateScrollBarsL();
        }
    }   

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::UpdateListBoxL()
    {
    iSettingItemArray->ResetAndDestroy();

/* 
    // create items
    TInt ordinal(0);

    AddSettingItemL(ESettingListDisplayModeSelection,
                    R_DISPLAYMODESELECTION_SETTING_TITLE,
                    R_DISPLAYMODESELECTION_SETTING_PAGE,
                    R_DISPLAYMODESELECTION_SETTING_TEXTS,
                    ordinal++);
                    
    AddSettingItemL(ESettingListFileViewModeSelection,
                    R_FILEVIEWMODESELECTION_SETTING_TITLE,
                    R_FILEVIEWMODESELECTION_SETTING_PAGE,
                    R_FILEVIEWMODESELECTION_SETTING_TEXTS,
                    ordinal++); 
*/                                                              
    SetVisibilitiesOfSettingItemsL(); 
    }

// --------------------------------------------------------------------------------------------

void CLoadGenSettingsViewDlg::AddSettingItemL(TInt aId,
                                                 TInt aTitleResource,
                                                 TInt aSettingPageResource,
                                                 TInt aAssociatedResource,
                                                 TInt aOrdinal)
    {
    // create a setting item
    CAknSettingItem* settingItem = NULL;
    
    switch (aId)
        {
/*
        case ESettingListDisplayModeSelection:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iDisplayMode);
            break;
            
        case ESettingListFileViewModeSelection:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iFileViewMode);
            break;

        default:
            User::Panic(_L("NotSetItem"), 50);
            break;
*/
        }
        
    CleanupStack::PushL(settingItem);

    // get title text
    HBufC* itemTitle = StringLoader::LoadLC(aTitleResource);

    // construct the setting item
    settingItem->ConstructL(EFalse, aOrdinal, *itemTitle, NULL, aSettingPageResource,
                            EAknCtPopupSettingList, NULL, aAssociatedResource);

    // append the setting item to settingitem array
    iSettingItemArray->InsertL(aOrdinal, settingItem);

    CleanupStack::PopAndDestroy(); //itemTitle
    CleanupStack::Pop(); //settingItem
    }

// --------------------------------------------------------------------------------------------
    
// End of File
