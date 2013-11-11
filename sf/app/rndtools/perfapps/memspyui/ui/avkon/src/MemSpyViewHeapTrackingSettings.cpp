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

#include "MemSpyViewHeapTrackingSettings.h"

// System includes
#include <hal.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif
#include <AknQueryDialog.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspyui.rsg>
#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpySettings.h"
#include "MemSpyViewHeapTracking.h"
#include "MemSpyContainerObserver.h"

/*
CMemSpyViewHeapTrackingSettings::CMemSpyViewHeapTrackingSettings( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }
*/
CMemSpyViewHeapTrackingSettings::CMemSpyViewHeapTrackingSettings( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }


CMemSpyViewHeapTrackingSettings::~CMemSpyViewHeapTrackingSettings()
    {
    }


void CMemSpyViewHeapTrackingSettings::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Settings" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewHeapTrackingSettings::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::SizeChanged();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewHeapTrackingSettings::ViewType() const
    {
    return EMemSpyViewTypeHeapTrackingSettings;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingSettings::PrepareParentViewL()
    {
    CMemSpyViewHeapTracking* parent = new(ELeave) CMemSpyViewHeapTracking( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingSettings::PrepareChildViewL()
    {
    const TInt index = iListBox->CurrentItemIndex();

    // Get current config    
    TMemSpyEngineHelperSysMemTrackerConfig config;    
    GetSwmtConfig( config );

    // This view abuses the child view callback to toggle field values and/or
    // display settings dialogs.
    if  ( index == 0 )
        {
        TInt period = config.iTimerPeriod.Int() / 1000000;
        CAknNumberQueryDialog* dialog = new(ELeave) CAknNumberQueryDialog( period );
        if  ( dialog->ExecuteLD( R_MEMSPY_HEAP_TRACKER_TIMER_PERIOD ) )
            {
            config.iTimerPeriod = period * 1000000;
            }
        }
    else if ( index == 1 )
        {
        TInt categories( config.iEnabledCategories );
        CSWMTCategorySelectionCheckBoxSettingItem* settingItem = new (ELeave) CSWMTCategorySelectionCheckBoxSettingItem( 0, categories );
        CleanupStack::PushL( settingItem );
        settingItem->ConstructL( EFalse,
                                 0,
                                 _L("SWMT Categories"),
                                 NULL,
                                 R_MEMSPYUI_SWMT_CATEGORIES_SETTING_PAGE,
                                 EAknCtPopupSettingList,
                                 NULL,
                                 NULL );
        
        CMemSpySWMTCategorySelectionCheckBoxSettingPage* dlg = new(ELeave) CMemSpySWMTCategorySelectionCheckBoxSettingPage( R_MEMSPYUI_SWMT_CATEGORIES_SETTING_PAGE, settingItem->ItemArray() );
        dlg->SetSettingPageObserver( settingItem );
        if  ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            config.iEnabledCategories = categories;
            }
        CleanupStack::PopAndDestroy( settingItem );
        }
    else if ( index == 2 )
        {
        config.iDumpData = !config.iDumpData;
        }
    else if  ( index == 3 )
        {
        TName nameFilter;
        nameFilter.Copy( config.iThreadNameFilter );
        CMemSpyAllowEmptyDataDialog* dialog = new (ELeave) CMemSpyAllowEmptyDataDialog( nameFilter );
        if  ( dialog->ExecuteLD( R_MEMSPY_HEAP_TRACKER_NAME_FILTER ) )
            {
            config.iThreadNameFilter = nameFilter;
            }
        }
    
    // Push changes to SWMT    
    SetSwmtConfig( config );
    Settings().StoreSettingsL();

    RefreshL();
    return NULL;
    }


void CMemSpyViewHeapTrackingSettings::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    // Get current config    
    TMemSpyEngineHelperSysMemTrackerConfig config;    
    GetSwmtConfig( config );

    // 1st item
    _LIT( KItem1Format, "\tTracking timer period\t\t%d (sec)" );
    item.Format( KItem1Format, config.iTimerPeriod.Int() / 1000000 );
    model->AppendL( item );
    
    // 2nd item
    _LIT( KItem3Format, "\tCategories\t\t%d selected" );
    
    TInt count(0);
    for( TInt bit = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups; bit > 0; bit = bit >> 1 )
        {
        if ( bit & config.iEnabledCategories )
            {
            count++;
            }
        }
    
    item.Format( KItem3Format, count );
    model->AppendL( item );

    // following items are related only to User Heap category, 
    // so hide them if User Heap is not selected
    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap )
        {
        // 3th item
        _LIT( KItem2Format, "\tDump data" );
        PrepareItemBufL( item, KItem2Format, config.iDumpData, config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap && config.iThreadNameFilter.Length() > 0 );
        model->AppendL( item );
        
        // 4th item
        if ( config.iDumpData )
            {
            _LIT( KItem4Format, "\tHeap data thread filter\t\t%S" );
            if ( config.iThreadNameFilter.Length() > 0 )
                {
                item.Format( KItem4Format, &config.iThreadNameFilter );
                }
            else
                {
                item.Format( KItem4Format, &_L("No filtering") );
                }
            model->AppendL( item );        
            }        
        }
    
    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


void CMemSpyViewHeapTrackingSettings::PrepareItemBufL( TDes& aBuf, const TDesC& aCaption, TBool aStatus, TBool aHeapCategorySelected )
    {
    aBuf.Copy( aCaption ); 
    //
    if  ( aStatus )
        {
        if ( aHeapCategorySelected )
            {
            _LIT( KItemEnabled, "\t\tCustom" );
            aBuf.Append( KItemEnabled );            
            }
        else
            {
            _LIT( KItemEnabled, "\t\tAll user heaps" );
            aBuf.Append( KItemEnabled );   
            }
        }
    else
        {
        _LIT( KItemDisabled, "\t\tDisabled" );
        aBuf.Append( KItemDisabled );
        }
    }

void CMemSpyViewHeapTrackingSettings::GetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
	TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode mode;
	iMemSpySession.GetSwmtMode( mode );
	aConfig.iMode = mode;
	     
	TInt categories = 0;
	iMemSpySession.GetSwmtCategoriesL( categories );
	aConfig.iEnabledCategories = categories;
	     
	TInt timerPeriod = 0;
	iMemSpySession.GetSwmtTimerIntervalL( timerPeriod );
	aConfig.iTimerPeriod = timerPeriod;
	     
	TBool dumpData = EFalse;
	iMemSpySession.GetSwmtHeapDumpsEnabledL( dumpData );
	aConfig.iDumpData = dumpData;
	     
	TName threadNameFilter;
	iMemSpySession.GetSwmtFilter( threadNameFilter );
	aConfig.iThreadNameFilter = threadNameFilter;
	}

void CMemSpyViewHeapTrackingSettings::SetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
	iMemSpySession.SetSwmtMode( aConfig.iMode );
	iMemSpySession.SetSwmtCategoriesL( aConfig.iEnabledCategories );
	iMemSpySession.SetSwmtTimerIntervalL( aConfig.iTimerPeriod.Int() );
	iMemSpySession.SetSwmtHeapDumpsEnabledL( aConfig.iDumpData );
	iMemSpySession.SetSwmtFilter( aConfig.iThreadNameFilter );
	}










// --------------------------------------------------------------------------------------------

CSWMTCategorySelectionCheckBoxSettingItem::CSWMTCategorySelectionCheckBoxSettingItem( TInt aIdentifier, TInt& aCategories ): 
    CAknSettingItem(aIdentifier), iExtCategories( aCategories ) 
    {
    }   


CSWMTCategorySelectionCheckBoxSettingItem::~CSWMTCategorySelectionCheckBoxSettingItem()
    {
    delete iSettingText;
    if( iItemArray )
        {
        iItemArray->ResetAndDestroy();
        delete iItemArray;
        }
    }


CSelectionItemList* CSWMTCategorySelectionCheckBoxSettingItem::ItemArray() const
    {
    return iItemArray;
    }


void CSWMTCategorySelectionCheckBoxSettingItem::AddNewItemToArrayL(const TDesC& aLabel)
    {
    TBuf<64> buf;
    buf.Copy( aLabel );    
    CSelectableItem* item = new (ELeave) CSelectableItem( buf, EFalse );
    CleanupStack::PushL( item );
    item->ConstructL();
    iItemArray->AppendL( item ); // Ownership is transferred
    CleanupStack::Pop(); // item    
    }
    

void CSWMTCategorySelectionCheckBoxSettingItem::CompleteConstructionL()
    {
    SetEmptyItemTextL(_L("Nothing selected"));
    iItemArray = new(ELeave) CSelectionItemList(16);
    AddNewItemToArrayL( _L("User Heap") );       // 0 >> 0
    //AddNewItemToArrayL( _L("Kernel Heap") );     // 1
    //AddNewItemToArrayL( _L("User Stacks") );     // 2
    //AddNewItemToArrayL( _L("Global Data") );     // 3
    AddNewItemToArrayL( _L("Disk Usage") );      // 4 >> 1
    AddNewItemToArrayL( _L("Open Files") );      // 5 >> 2
    AddNewItemToArrayL( _L("RAM-loaded Code") ); // 6 >> 3
    AddNewItemToArrayL( _L("System Memory") );   // 7 >> 4
    AddNewItemToArrayL( _L("Local Chunks") );    // 8 >> 5
    AddNewItemToArrayL( _L("Global Chunks") );   // 9 >> 6
    AddNewItemToArrayL( _L("RAM Drive") );       // 10 >> 7
    AddNewItemToArrayL( _L("FileServer Cache") );// 11 >> 8
    AddNewItemToArrayL( _L("Kernel Handles") );  // 12 >> 9
    //AddNewItemToArrayL( _L("Bitmap Handles") );  // 13
    AddNewItemToArrayL( _L("Window Groups") );   // 14 >> 10
    }


void CSWMTCategorySelectionCheckBoxSettingItem::StoreL()
    {
    iExtCategories = 0;
    if ( iItemArray->At( 8 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryFileServerCache; 
    //if ( iItemArray->At( 13 )->SelectionStatus() )
    //    iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryBitmapHandles; 
    if ( iItemArray->At( 0 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap; 
    //if ( iItemArray->At( 1 )->SelectionStatus() )
    //    iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHeap; 
    if ( iItemArray->At( 5 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryLocalChunks; 
    if ( iItemArray->At( 6 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalChunks; 
    if ( iItemArray->At( 7 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMDrive; 
    //if ( iItemArray->At( 2 )->SelectionStatus() )
    //    iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks; 
    //if ( iItemArray->At( 3 )->SelectionStatus() )
    //    iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData; 
    if ( iItemArray->At( 3 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode; 
    if ( iItemArray->At( 9 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHandles; 
    if ( iItemArray->At( 2 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryOpenFiles; 
    if ( iItemArray->At( 1 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryDiskusage; 
    if ( iItemArray->At( 4 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory; 
    if ( iItemArray->At( 10 )->SelectionStatus() )
        iExtCategories |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups; 
    }


void CSWMTCategorySelectionCheckBoxSettingItem::LoadL()
    {
    iItemArray->At( 8 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryFileServerCache );
    //iItemArray->At( 13 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryBitmapHandles );
    iItemArray->At( 0 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap );
    //iItemArray->At( 1 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHeap );
    iItemArray->At( 5 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryLocalChunks );
    iItemArray->At( 6 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalChunks );
    iItemArray->At( 7 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMDrive );
    //iItemArray->At( 2 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks );
    //iItemArray->At( 3 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData );
    iItemArray->At( 3 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode );
    iItemArray->At( 9 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHandles );
    iItemArray->At( 2 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryOpenFiles );
    iItemArray->At( 1 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryDiskusage );
    iItemArray->At( 4 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory );
    iItemArray->At( 10 )->SetSelectionStatus( iExtCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups );
    }


void CSWMTCategorySelectionCheckBoxSettingItem::HandleSettingPageEventL( CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventSettingOked:
            {
            // If settings oked, save new values
            StoreL();
            break;
            }
        case EEventSettingCancelled:
            {
            // If setting is cancelled, load old values
            LoadL();
            break;
            }
        case EEventSettingChanged:
            {
            // If setting has changed, update CBA visibility
            static_cast<CMemSpySWMTCategorySelectionCheckBoxSettingPage*>( aSettingPage )->UpdateCba();
            break;
            }
        default:
            {
            break;
            }
        }
    CAknSettingItem::HandleSettingPageEventL( aSettingPage, aEventType );
    }


CMemSpySWMTCategorySelectionCheckBoxSettingPage::CMemSpySWMTCategorySelectionCheckBoxSettingPage(TInt aResourceID, CSelectionItemList* aItemArray )
    : CAknCheckBoxSettingPage( aResourceID, aItemArray )
    {
    }


void CMemSpySWMTCategorySelectionCheckBoxSettingPage::UpdateCba()
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




CMemSpyAllowEmptyDataDialog::CMemSpyAllowEmptyDataDialog( TDes& aText, const TTone& aTone )
    : CAknTextQueryDialog( aText, aTone )
    {
    }
 

void CMemSpyAllowEmptyDataDialog::UpdateLeftSoftKeyL()
    {
    MakeLeftSoftkeyVisible( ETrue );
    }
