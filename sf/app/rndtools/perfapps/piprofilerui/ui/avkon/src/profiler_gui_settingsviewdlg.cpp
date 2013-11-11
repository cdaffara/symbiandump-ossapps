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
#include "profiler_gui_settingsviewdlg.h"
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
_LIT8(KDebugOutput, "debug_output");
_LIT8(KFileSystem, "file_system");
_LIT(KEmptySettingItem, "");

// ===================================== MEMBER FUNCTIONS =====================================

CProfilerGuiSettingsViewDlg* CProfilerGuiSettingsViewDlg::NewL(TGeneralAttributes& aSettings)
    {
    CProfilerGuiSettingsViewDlg* self = new(ELeave) CProfilerGuiSettingsViewDlg(aSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CProfilerGuiSettingsViewDlg::~CProfilerGuiSettingsViewDlg()
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

CProfilerGuiSettingsViewDlg::CProfilerGuiSettingsViewDlg(TGeneralAttributes& aSettings) : iSettings(aSettings)
    {
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::ConstructL()
    {
    // construct a menu bar
    CAknDialog::ConstructL(R_PROFILER_GUI_SETTINGS_MENUBAR);
   
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    
    // set empty navi pane label
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    iNaviContainer->PushDefaultL();
    
    // set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( _L("Settings") );

    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
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

void CProfilerGuiSettingsViewDlg::TabChangedL(TInt /*aIndex*/)
    {
    iListBox->SetCurrentItemIndex(0);

    SetVisibilitiesOfSettingItemsL();   
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::ProcessCommandL(TInt aCommandId)
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

TKeyResponse CProfilerGuiSettingsViewDlg::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    /*
    if (iTabGroup == NULL)
        {
        return EKeyWasNotConsumed;
        }
        */
/*
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
*/
    return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }    

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::PreLayoutDynInitL()
    {
    iListBox = static_cast<CAknSettingStyleListBox*>( Control(EProfilerGuiSettingItemList) );
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

TBool CProfilerGuiSettingsViewDlg::OkToExitL(TInt aButtonId)
    {
    // save settings to iSettings before exiting
    
    // check if trace output target changed
    if(iTraceOutput == EOutputToDebugPort)
        {
        iSettings.iTraceOutput.Copy(KDebugOutput);
        }
    else
        {
        iSettings.iTraceOutput.Copy(KFileSystem);
        }
    
    // save trace file prefix
    if(iFilePrefix.CompareF(KNullDesC) != 0)
        {
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iSettings.iTraceFilePrefix, iFilePrefix);
        }
    
    // save the drive 
    if(iSaveDrive.CompareF(KNullDesC) != 0)
        {
        CnvUtfConverter::ConvertFromUnicodeToUtf8(iSettings.iSaveFileDrive, iSaveDrive);
        }
    
    return CAknDialog::OkToExitL(aButtonId);
    }



// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::ShowSettingPageL(TInt aCalledFromMenu)
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

void CProfilerGuiSettingsViewDlg::SetVisibilitiesOfSettingItemsL()
    {

    // check if setting item array contains more than 0 items
    if (iSettingItemArray->Count() > 0)
        {
//        ((*iSettingItemArray)[ESettingListItemPluginNameMode])->SetHidden(ETrue);
        ((*iSettingItemArray)[ESettingListItemPluginTraceOutput])->SetHidden(EFalse);
        if(iTraceOutput == EOutputToDebugPort)
        	{
        	((*iSettingItemArray)[ESettingListItemPluginSaveFileDrive])->SetHidden(ETrue);
        	((*iSettingItemArray)[ESettingListItemPluginTraceFilePrefix])->SetHidden(ETrue);
        	}
        else 
        	{
        	((*iSettingItemArray)[ESettingListItemPluginSaveFileDrive])->SetHidden(EFalse);
        	((*iSettingItemArray)[ESettingListItemPluginTraceFilePrefix])->SetHidden(EFalse);
        	}
//        ((*iSettingItemArray)[ESettingListItemPluginTraceMode])->SetHidden(ETrue);

        iSettingItemArray->RecalculateVisibleIndicesL();
        iListBox->HandleItemAdditionL();
        iListBox->UpdateScrollBarsL();
        }
    }   

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::UpdateListBoxL()
    {
    iSettingItemArray->ResetAndDestroy();

    // create items
    TInt ordinal(0);
    
//    AddSettingItemL(ESettingListItemPluginNameMode,
//                    R_PLUGIN_NAME_MODE_SETTING_TITLE,
//                    R_PLUGIN_NAME_MODE_SETTING_PAGE,
//                    R_PLUGIN_NAME_MODE_SETTING_TEXTS,
//                    ordinal++);    

    AddSettingItemL(ESettingListItemPluginTraceOutput,
                    R_TRACE_OUTPUT_SETTING_TITLE,
                    R_TRACE_OUTPUT_SETTING_PAGE,
                    R_TRACE_OUTPUT_SETTING_TEXTS,
                    ordinal++);
    
    AddSettingItemL(ESettingListItemPluginSaveFileDrive,
                    R_SAVE_FILE_DRIVE_SETTING_TITLE,
                    R_SAVE_FILE_DRIVE_SETTING_PAGE,
                    NULL, //R_SAVE_FILE_DRIVE_SETTING_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemPluginTraceFilePrefix,
                    R_SAVE_FILE_PREFIX_SETTING_TITLE,
                    R_SAVE_FILE_PREFIX_SETTING_PAGE,
                    NULL,
                    ordinal++);
    
    AddSettingItemL(ESettingListItemPluginSamplingTime,
                    R_TIMED_SAMPLING_PERIOD_SETTING_TITLE,
                    R_TIMED_SAMPLING_PERIOD_SETTING_PAGE,
                    NULL,
                    ordinal++);
    
    

//    AddSettingItemL(ESettingListItemPluginTraceMode,
//                    R_TRACE_MODE_SETTING_TITLE,
//                    R_TRACE_MODE_SETTING_PAGE,
//                    R_TRACE_MODE_SETTING_TEXTS,
//                    ordinal++);

    SetVisibilitiesOfSettingItemsL(); 
    }

// --------------------------------------------------------------------------------------------

void CProfilerGuiSettingsViewDlg::AddSettingItemL(TInt aId,
                                                 TInt aTitleResource,
                                                 TInt aSettingPageResource,
                                                 TInt aAssociatedResource,
                                                 TInt aOrdinal)
    {
    // create a setting item
    CAknSettingItem* settingItem = NULL;
    
    switch (aId)
        {

        case ESettingListItemPluginSaveFileDrive:
            {
            CnvUtfConverter::ConvertToUnicodeFromUtf8(iSaveDrive, iSettings.iSaveFileDrive);
            settingItem = new(ELeave) CAknTextSettingItem(aId, iSaveDrive);
            settingItem->SetEmptyItemTextL(KEmptySettingItem); 
            break;
            }
        case ESettingListItemPluginTraceOutput:
            if(iSettings.iTraceOutput.MatchF(KDebugOutput) != KErrNotFound)
                {
                iTraceOutput = EOutputToDebugPort;
                }
            else
                {
                iTraceOutput = EOutputToFileSystem;
                }
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iTraceOutput); 

            break;

        case ESettingListItemPluginTraceFilePrefix:
            {
            CnvUtfConverter::ConvertToUnicodeFromUtf8(iFilePrefix, iSettings.iTraceFilePrefix);
            settingItem = new(ELeave) CAknTextSettingItem(aId, iFilePrefix);
            settingItem->SetEmptyItemTextL(KEmptySettingItem); 
            break;
            }
        case ESettingListItemPluginSamplingTime:
            {
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aId, iSettings.iTimedSamplingPeriod);
            break;
            }
        default:
            User::Panic(_L("NotSetItem"), 50);
            break;

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

// End of File

