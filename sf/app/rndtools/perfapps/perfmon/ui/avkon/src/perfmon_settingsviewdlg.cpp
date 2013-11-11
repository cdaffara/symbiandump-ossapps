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
#include "perfmon_settingsviewdlg.h"
#include "perfmon_model.h"
#include "perfmon.hrh"
#include "perfmon_std.h"
#include "perfmon_powerlistener.h"
#include <perfmon.rsg>

#include <aknsettingitemlist.h>
#include <CAknMemorySelectionSettingItem.h> 
#include <aknlists.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <aknnotewrappers.h> 

// ===================================== MEMBER FUNCTIONS =====================================

CPerfMonSettingsViewDlg* CPerfMonSettingsViewDlg::NewL(TPerfMonSettings& aSettings)
    {
    CPerfMonSettingsViewDlg* self = new(ELeave) CPerfMonSettingsViewDlg(aSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CPerfMonSettingsViewDlg::~CPerfMonSettingsViewDlg()
    {
    // restore default navi pane by popping the tab group from stack
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

CPerfMonSettingsViewDlg::CPerfMonSettingsViewDlg(TPerfMonSettings& aSettings) : iSettings(aSettings)
    {
    }

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::ConstructL()
    {
    // construct a menu bar
    CAknDialog::ConstructL(R_PERFMON_SETTINGS_MENUBAR);

    // get pointer to status pane   
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    
    // set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    tp->SetTextL( _L("Settings") );
    
    // create a new tab group
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    iDecoratedTabGroup = iNaviContainer->CreateTabGroupL(this);
    iTabGroup = static_cast<CAknTabGroup*>(iDecoratedTabGroup->DecoratedControl());
    iTabGroup->SetTabFixedWidthL(KTabWidthWithTwoLongTabs); 
    
    // add tabs
    iTabGroup->AddTabL(ETabSettingsGeneral, _L("General"));
    iTabGroup->AddTabL(ETabSettingsDataPopup, _L("Data popup"));
    iTabGroup->AddTabL(ETabSettingsGraphs, _L("Graphs"));
    iTabGroup->AddTabL(ETabSettingsLogging, _L("Logging"));
    iTabGroup->SetActiveTabByIndex(ETabSettingsGeneral);
    
    // add new tab group to stack
    iNaviContainer->PushL( *iDecoratedTabGroup );
    }

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
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

void CPerfMonSettingsViewDlg::TabChangedL(TInt /*aIndex*/)
    {
    iListBox->SetCurrentItemIndex(0);

    SetVisibilitiesOfSettingItemsL();   
    }

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::ProcessCommandL(TInt aCommandId)
    {
    CAknDialog::ProcessCommandL(aCommandId);

    switch (aCommandId)
        {
        case EPerfMonCmdSettingsChange:
            ShowSettingPageL(ETrue);
            break;
        case EPerfMonCmdSettingsExit:
            TryExitL(EAknCmdExit);
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CPerfMonSettingsViewDlg::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
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

void CPerfMonSettingsViewDlg::PreLayoutDynInitL()
    {
    iListBox = static_cast<CAknSettingStyleListBox*>( Control(EPerfMonSettingItemList) );
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

TBool CPerfMonSettingsViewDlg::OkToExitL(TInt aButtonId)
    {
    return CAknDialog::OkToExitL(aButtonId);
    }

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::ShowSettingPageL(TInt aCalledFromMenu)
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

void CPerfMonSettingsViewDlg::SetVisibilitiesOfSettingItemsL()
    {
    if (iSettingItemArray->Count() > 0)
        {
        switch (iTabGroup->ActiveTabIndex())
            {
            case ETabSettingsGeneral:
                {
                ((*iSettingItemArray)[ESettingListItemHeartBeat])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemMaxSamples])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemPriority])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemCPUMode])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemKeepBackLightOn])->SetHidden(EFalse);
                if (CPerfMonPowerListener::IsSupported())
                    ((*iSettingItemArray)[ESettingListItemPowerMonitoringEnabled])->SetHidden(EFalse);
                else
                    ((*iSettingItemArray)[ESettingListItemPowerMonitoringEnabled])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemDataPopupVisbility])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupLocation])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemGraphsVerticalBarPeriod])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemGraphsSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemLoggingMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingFilePath])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingSources])->SetHidden(ETrue);

                break;
                }
            
            case ETabSettingsDataPopup:
                {
                ((*iSettingItemArray)[ESettingListItemHeartBeat])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemMaxSamples])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPriority])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemCPUMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemKeepBackLightOn])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPowerMonitoringEnabled])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemDataPopupVisbility])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemDataPopupLocation])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemDataPopupSources])->SetHidden(EFalse);

                ((*iSettingItemArray)[ESettingListItemGraphsVerticalBarPeriod])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemGraphsSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemLoggingMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingFilePath])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingSources])->SetHidden(ETrue);

                break;
                }

            case ETabSettingsGraphs:
                {
                ((*iSettingItemArray)[ESettingListItemHeartBeat])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemMaxSamples])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPriority])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemCPUMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemKeepBackLightOn])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPowerMonitoringEnabled])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemDataPopupVisbility])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupLocation])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemGraphsVerticalBarPeriod])->SetHidden(EFalse);
                ((*iSettingItemArray)[ESettingListItemGraphsSources])->SetHidden(EFalse);

                ((*iSettingItemArray)[ESettingListItemLoggingMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingFilePath])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemLoggingSources])->SetHidden(ETrue);

                break;
                }

            case ETabSettingsLogging:
                {
                ((*iSettingItemArray)[ESettingListItemHeartBeat])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemMaxSamples])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPriority])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemCPUMode])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemKeepBackLightOn])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemPowerMonitoringEnabled])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemDataPopupVisbility])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupLocation])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemDataPopupSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemGraphsVerticalBarPeriod])->SetHidden(ETrue);
                ((*iSettingItemArray)[ESettingListItemGraphsSources])->SetHidden(ETrue);

                ((*iSettingItemArray)[ESettingListItemLoggingMode])->SetHidden(EFalse);
                
                if (iSettings.iLoggingMode == ELoggingModeLogFile || iSettings.iLoggingMode == ELoggingModeRDebugLogFile)
                    ((*iSettingItemArray)[ESettingListItemLoggingFilePath])->SetHidden(EFalse);
                else
                    ((*iSettingItemArray)[ESettingListItemLoggingFilePath])->SetHidden(ETrue);
                
                ((*iSettingItemArray)[ESettingListItemLoggingSources])->SetHidden(EFalse);

                break;
                }                

            default:
                User::Panic(_L("TabIOOB"), 50);
                break;
            }

        iSettingItemArray->RecalculateVisibleIndicesL();
        iListBox->HandleItemAdditionL();
        iListBox->UpdateScrollBarsL();
        }
    }   

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::UpdateListBoxL()
    {
    iSettingItemArray->ResetAndDestroy();

    // create items
    TInt ordinal(0);

    AddSettingItemL(ESettingListItemHeartBeat,
                    R_HEARTBEAT_SETTING_TITLE,
                    R_HEARTBEAT_SETTING_PAGE,
                    NULL,
                    ordinal++);    
    
    AddSettingItemL(ESettingListItemMaxSamples,
                    R_MAXSAMPLES_SETTING_TITLE,
                    R_MAXSAMPLES_SETTING_PAGE,
                    NULL,
                    ordinal++);

    AddSettingItemL(ESettingListItemPriority,
                    R_PRIORITY_SETTING_TITLE,
                    R_PRIORITY_SETTING_PAGE,
                    R_PRIORITY_SETTING_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemCPUMode,
                    R_CPUMODE_SETTING_TITLE,
                    R_CPUMODE_SETTING_PAGE,
                    R_CPUMODE_SETTING_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemKeepBackLightOn,
                    R_KEEPBACKLIGHTON_SETTING_TITLE,
                    R_BINARY_SETTING_PAGE,
                    R_YESNO_BINARYSELECTION_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemPowerMonitoringEnabled,
                    R_POWERMONITORINGENABLED_SETTING_TITLE,
                    R_BINARY_SETTING_PAGE,
                    R_YESNO_BINARYSELECTION_TEXTS,
                    ordinal++);
//
    AddSettingItemL(ESettingListItemDataPopupVisbility,
                    R_DATAPOPUPVISIBILITY_SETTING_TITLE,
                    R_DATAPOPUPVISIBILITY_SETTING_PAGE,
                    R_DATAPOPUPVISIBILITY_SETTING_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemDataPopupLocation,
                    R_DATAPOPUPLOCATION_SETTING_TITLE,
                    R_DATAPOPUPLOCATION_SETTING_PAGE,
                    R_DATAPOPUPLOCATION_SETTING_TEXTS,
                    ordinal++);

    AddSettingItemL(ESettingListItemDataPopupSources,
                    R_SOURCES_SETTING_TITLE,
                    R_SOURCES_SETTING_PAGE,
                    NULL,
                    ordinal++);                    
//
    AddSettingItemL(ESettingListItemGraphsVerticalBarPeriod,
                    R_GRAPHSVERTICALBAR_SETTING_TITLE,
                    R_GRAPHSVERTICALBAR_SETTING_PAGE,
                    NULL,
                    ordinal++);
    
    AddSettingItemL(ESettingListItemGraphsSources,
                    R_SOURCES_SETTING_TITLE,
                    R_SOURCES_SETTING_PAGE,
                    NULL,
                    ordinal++);                    
//
    AddSettingItemL(ESettingListItemLoggingMode,
                    R_LOGGINGMODE_SETTING_TITLE,
                    R_LOGGINGMODE_SETTING_PAGE,
                    R_LOGGINGMODE_SETTING_TEXTS,
                    ordinal++);

     AddSettingItemL(ESettingListItemLoggingFilePath,
                    R_LOGGINGFILEPATH_SETTING_TITLE,
                    R_LOGGINGFILEPATH_SETTING_PAGE,
                    NULL,
                    ordinal++);
    
    AddSettingItemL(ESettingListItemLoggingSources,
                    R_SOURCES_SETTING_TITLE,
                    R_SOURCES_SETTING_PAGE,
                    NULL,
                    ordinal++);                    
                                                             
    SetVisibilitiesOfSettingItemsL(); 
    }

// --------------------------------------------------------------------------------------------

void CPerfMonSettingsViewDlg::AddSettingItemL(TInt aId,
                                                 TInt aTitleResource,
                                                 TInt aSettingPageResource,
                                                 TInt aAssociatedResource,
                                                 TInt aOrdinal)
    {
    // create a setting item
    CAknSettingItem* settingItem = NULL;
    
    switch (aId)
        {
        case ESettingListItemHeartBeat:
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aId, iSettings.iHeartBeat);
            break;

        case ESettingListItemMaxSamples:
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aId, iSettings.iMaxSamples);
            break;

        case ESettingListItemPriority:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iPriority);
            break;

        case ESettingListItemCPUMode:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iCPUMode);
            break;

        case ESettingListItemKeepBackLightOn:
            settingItem = new(ELeave) CAknBinaryPopupSettingItem(aId, iSettings.iKeepBacklightOn);
            break;

        case ESettingListItemPowerMonitoringEnabled:
            settingItem = new(ELeave) CAknBinaryPopupSettingItem(aId, iSettings.iPowerMonitoringEnabled);
            break;

        case ESettingListItemDataPopupVisbility:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iDataPopupVisibility);
            break;

        case ESettingListItemDataPopupLocation:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iDataPopupLocation);
            break;

        case ESettingListItemDataPopupSources:
            settingItem = new(ELeave) CSourceSelectionCheckBoxSettingItem(aId, iSettings.iDataPopupSources);
            break;

        case ESettingListItemGraphsVerticalBarPeriod:
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aId, iSettings.iGraphsVerticalBarPeriod);
            break;

        case ESettingListItemGraphsSources:
            settingItem = new(ELeave) CSourceSelectionCheckBoxSettingItem(aId, iSettings.iGraphsSources);
            break;

        case ESettingListItemLoggingMode:
            settingItem = new(ELeave) CAknEnumeratedTextPopupSettingItem(aId, iSettings.iLoggingMode);
            break;

        case ESettingListItemLoggingFilePath:
            settingItem = new(ELeave) CAknTextSettingItem(aId, iSettings.iLoggingFilePath);
            break;

        case ESettingListItemLoggingSources:
            settingItem = new(ELeave) CSourceSelectionCheckBoxSettingItem(aId, iSettings.iLoggingSources);
            break;

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


// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
	
CSourceSelectionCheckBoxSettingItem::CSourceSelectionCheckBoxSettingItem( TInt aIdentifier, TPerfMonSources& aMemoryInUse ): 
	CAknSettingItem(aIdentifier), iExternalSources( aMemoryInUse ) 
	{
	}	

CSourceSelectionCheckBoxSettingItem::~CSourceSelectionCheckBoxSettingItem()
	{
	delete iSettingText;

    if( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        }
	}

void CSourceSelectionCheckBoxSettingItem::AddNewItemToArrayL(const TDesC& aLabel)
	{
	TBuf<64> buf;
	buf.Copy(aLabel);
	
    CSelectableItem* item = new(ELeave) CSelectableItem(buf, EFalse);
    CleanupStack::PushL(item);
    item->ConstructL();
    iItemArray->AppendL(item); // Ownership is transferred
    CleanupStack::Pop(); // item	
	}
	
void CSourceSelectionCheckBoxSettingItem::CompleteConstructionL()
	{
    SetEmptyItemTextL(_L("Nothing selected"));

    iItemArray = new(ELeave) CSelectionItemList(16);
    AddNewItemToArrayL(_L("CPU"));
    AddNewItemToArrayL(_L("RAM"));
    AddNewItemToArrayL(_L("C:"));
    AddNewItemToArrayL(_L("D:"));
    AddNewItemToArrayL(_L("E:"));
    AddNewItemToArrayL(_L("F:"));
    AddNewItemToArrayL(_L("G:"));
    AddNewItemToArrayL(_L("H:"));
    AddNewItemToArrayL(_L("I:"));
    AddNewItemToArrayL(_L("Power"));

	__ASSERT_ALWAYS(ESourcesLength==iItemArray->Count(), User::Panic(_L("Src.mismatch"),443));
    }

// transfer settings to the variables defined in the constructor    	
void CSourceSelectionCheckBoxSettingItem::StoreL()
	{
	for (TInt i=0; i<ESourcesLength; i++)
	    {
        iExternalSources.iSrcEnabled[i] = iItemArray->At(i)->SelectionStatus();
	    }
	}

// load settings from the variables defined in the constructor to our internal variables
void CSourceSelectionCheckBoxSettingItem::LoadL()
	{
	for (TInt i=0; i<ESourcesLength; i++)
	    {
        iItemArray->At(i)->SetSelectionStatus( iExternalSources.iSrcEnabled[i] );
	    }
	}

// returns the text shown in the setting item list
const TDesC& CSourceSelectionCheckBoxSettingItem::SettingTextL()
	{
    TBuf<32> settingText;

    if (iItemArray->At(ESourceCPU)->SelectionStatus())
        settingText.Append(_L("CPU "));
    if (iItemArray->At(ESourceRAM)->SelectionStatus())
        settingText.Append(_L("RAM "));
    if (iItemArray->At(ESourceC)->SelectionStatus())
        settingText.Append(_L("C: "));
    if (iItemArray->At(ESourceD)->SelectionStatus())
        settingText.Append(_L("D: "));
    if (iItemArray->At(ESourceE)->SelectionStatus())
        settingText.Append(_L("E: "));
    if (iItemArray->At(ESourceF)->SelectionStatus())
        settingText.Append(_L("F: "));
    if (iItemArray->At(ESourceG)->SelectionStatus())
        settingText.Append(_L("G: "));
    if (iItemArray->At(ESourceH)->SelectionStatus())
        settingText.Append(_L("H: "));
    if (iItemArray->At(ESourceI)->SelectionStatus())
        settingText.Append(_L("I: "));
    if (iItemArray->At(ESourcePwr)->SelectionStatus())
        settingText.Append(_L("Power "));
    settingText.TrimAll();

    if (iSettingText)
        {
        delete iSettingText;
        iSettingText = NULL;
        }

    iSettingText = HBufC::NewL(settingText.Length());
    iSettingText->Des().Copy(settingText);

	if ( iSettingText->Length() > 0 )
		return *iSettingText;
	else
		return EmptyItemText();
	}


// launches the setting page
void CSourceSelectionCheckBoxSettingItem::EditItemL(TBool /*aCalledFromMenu*/)
	{
	CSourceSelectionCheckBoxSettingPage* dlg = new(ELeave) CSourceSelectionCheckBoxSettingPage(SettingPageResourceId(), iItemArray);

	SetSettingPage( dlg );
	SettingPage()->SetSettingPageObserver(this);

	SettingPage()->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
	SetSettingPage(0); // it is deleted now
	}


// handles setting page events
void CSourceSelectionCheckBoxSettingItem::HandleSettingPageEventL( CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventSettingCancelled:
            {
            // If setting is cancelled, load old values
            LoadL();
            break;
            }
        case EEventSettingChanged:
            {
            // If setting has changed, update CBA visibility
            static_cast<CSourceSelectionCheckBoxSettingPage*>( aSettingPage )->UpdateCba();
            break;
            }
        default:
            {
            break;
            }
        }
    CAknSettingItem::HandleSettingPageEventL( aSettingPage, aEventType );
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CSourceSelectionCheckBoxSettingPage::CSourceSelectionCheckBoxSettingPage(TInt aResourceID, CSelectionItemList* aItemArray )
    : CAknCheckBoxSettingPage( aResourceID, aItemArray )
	{
	}

void CSourceSelectionCheckBoxSettingPage::UpdateCba()
	{
	// Cache the pointer to button group container
	CEikButtonGroupContainer* cba = Cba();
	// Left softkey should be visible if there are items selected,
	// i.e. the selection index array has items.
	TBool visible( ListBoxControl()->View()->SelectionIndexes()->Count() > 0 );
	// Resolve left softkey command id
	TInt leftId( cba->ButtonGroup()->CommandId( 0 ) );
	// Check if left softkey visibility has changed
	if( visible != cba->IsCommandVisible( leftId ) )
		{
		// Left softkey visibility has changed, update it
		cba->MakeCommandVisible( leftId, visible );
		}
	}

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
    
// End of File
