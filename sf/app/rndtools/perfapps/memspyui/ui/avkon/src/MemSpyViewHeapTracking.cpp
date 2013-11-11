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

#include "MemSpyViewHeapTracking.h"

// System includes
#include <hal.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyViewHeapTrackingSettings.h"
#include "MemSpyViewHeapTrackingResults.h"



CMemSpyViewHeapTracking::CMemSpyViewHeapTracking( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver ),    
    iState( EMemSpyViewHeapTrackingStateIdle )
    {
	if( iMemSpySession.IsSwmtRunningL() )
		{
		iState = EMemSpyViewHeapTrackingStateTimerOn;
		}	   
    }


CMemSpyViewHeapTracking::~CMemSpyViewHeapTracking()
    {
    delete iStopTimerCallBack;        
    }


void CMemSpyViewHeapTracking::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "System-Wide Memory Tracking" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
            
    // Backup current config because it may be overwritten with Basic/Full mode settings.       
    TMemSpyEngineHelperSysMemTrackerConfig config;

    GetSwmtConfigL( config );
    
    iOriginalConfig = config;     
        
    if ( config.iMode != TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom )
        {
        // Set config. Needed for updating config for Basic or Full mode.               
		SetConfigByModeL( config.iMode, config );
        }
    
    // Make sure the correct item is selected
    TInt index = 0;
    if  ( aSelectionRune != NULL )
        {
        const TMemSpyViewType viewType = (TMemSpyViewType) ((TInt) aSelectionRune);
        index = IndexByViewTypeL( viewType );
        }
    iListBox->SetCurrentItemIndex( index );
    HandleListBoxItemSelectedL( index );       
    }


void CMemSpyViewHeapTracking::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::SizeChanged();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewHeapTracking::ViewType() const
    {
    return EMemSpyViewTypeHeapTracking;
    }


CMemSpyViewBase* CMemSpyViewHeapTracking::PrepareParentViewL()
    {
    
    // Save custom settings even if mode is Basic or Full	
	TRAP_IGNORE(	        
	        iMemSpySession.SetSwmtCategoriesL( iOriginalConfig.iEnabledCategories );	        
	        iMemSpySession.SetSwmtFilter( iOriginalConfig.iThreadNameFilter );
	        iMemSpySession.SetSwmtHeapDumpsEnabledL( iOriginalConfig.iDumpData );	        
	        Settings().StoreSettingsL();
	        );
    
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewHeapTracking::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    const TInt index = iListBox->CurrentItemIndex();
    
    // Get current config    
    TMemSpyEngineHelperSysMemTrackerConfig config;    
    
    GetSwmtConfigL( config );
    
    //
    if  ( index == 0 )
        {
        // This is the start/stop toggle so we don't make a child view
        child = NULL; 

        // ... instead we either start or stop the tracker
       // if  ( !iEngine.HelperSysMemTracker().IsActive() )
        if  ( !iMemSpySession.IsSwmtRunningL() )
            {
			DumpNowL();            
            }

        // Redraw listbox 
        RefreshL();
        }
    else if  ( index == 1 )
        {
        // This is the start/stop toggle so we don't make a child view
        child = NULL; 

        // ... instead we either start or stop the tracker        
        if  ( iMemSpySession.IsSwmtRunningL() && iState == EMemSpyViewHeapTrackingStateTimerOn )
            {
            iState = EMemSpyViewHeapTrackingStateIdle;
            iMemSpySession.StopSwmtTimerL();
            
            }
        else if ( iState == EMemSpyViewHeapTrackingStateIdle )
            {
			iState = EMemSpyViewHeapTrackingStateTimerOn;
			RefreshL();
            StartSwmtTimerL();
            }

        // Redraw listbox 
        RefreshL();
        }
    else if ( index == 2 )
        {
        switch ( config.iMode )
            {
            case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeBasic:
                {
                // Set Full mode
                SetConfigByModeL( TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeFull, config );
                break;
                }
            case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeFull:
                {
                // Set Custom mode
                SetConfigByModeL( TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom, config );
                break;
                }
            case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom:
                {
                // Set Basic mode
                SetConfigByModeL( TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeBasic, config );
                break;
                }
            default: break;
            }
                
        // Redraw listbox 
        RefreshL();
        }
    else if ( index == 3 )
        {        
        if ( config.iMode == TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom )
            {
            child = new(ELeave) CMemSpyViewHeapTrackingSettings( iMemSpySession, iObserver );
            }       
        }
    if  ( child )
        {
        CleanupStack::PushL( child );
        child->ConstructL( Rect(), *Parent() );
        CleanupStack::Pop( child );
        }
    
    //
    return child;
    }


void CMemSpyViewHeapTracking::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;
    
    // Get current config    
    TMemSpyEngineHelperSysMemTrackerConfig config;       
    
    GetSwmtConfigL( config );

    // 1st item
    _LIT( KItem1FormatEnable, "\tGet dump now" );
    model->AppendL( KItem1FormatEnable );

    
    // 1st item
    //if  ( iEngine.HelperSysMemTracker().IsActive() && iState == EMemSpyViewHeapTrackingStateTimerOn )
    if ( iState == EMemSpyViewHeapTrackingStateTimerOn ) //iMemSpySession.IsSwmtRunningL() &&
        {
        _LIT( KItem1FormatEnable, "\tStop timer\t\t%d (sec)" );
        TName item;
        item.Format( KItem1FormatEnable, config.iTimerPeriod.Int() / 1000000 );
        model->AppendL( item );
        }
    else
        {
        _LIT( KItem1FormatEnable, "\tStart timer\t\t%d (sec)" );
        TName item;
        item.Format( KItem1FormatEnable, config.iTimerPeriod.Int() / 1000000 );
        model->AppendL( item );
        }
    
    // 2nd item
    switch ( config.iMode )
        {
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeBasic:
            {
            _LIT( KItem2Format, "\tTracking mode\t\tBasic" );
            model->AppendL( KItem2Format );        
            break;
            }
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeFull:
            {
            _LIT( KItem2Format, "\tTracking mode\t\tFull" );
            model->AppendL( KItem2Format );        
            break;
            }
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom:
            {
            _LIT( KItem2Format, "\tTracking mode\t\tCustom" );
            model->AppendL( KItem2Format );
            
            // 3rd item
            TFullName item;
            if ( config.iDumpData && 
                config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap )
                {
                _LIT( KItem3Format, "\tSettings\t\t%d categ., dump" );
                
                TInt count(0);
                for( TInt bit = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups; bit > 0; bit = bit >> 1 )
                    {
                    if ( bit & config.iEnabledCategories )
                        {
                        count++;
                        }
                    }
                item.Format( KItem3Format, count );
                }
            else
                {
                _LIT( KItem3Format, "\tSettings\t\t%d categories" );
                TInt count(0);
                for( TInt bit = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups; bit > 0; bit = bit >> 1 )
                    {
                    if ( bit & config.iEnabledCategories )
                        {
                        count++;
                        }
                    }
                             
                item.Format( KItem3Format, count );
                }
            model->AppendL( item );            
            break;
            }
        default: break;
        }         

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }

void CMemSpyViewHeapTracking::HandleCyclesResetL()
    {
    }


void CMemSpyViewHeapTracking::HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }


void CMemSpyViewHeapTracking::HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    // Stopping SWMT does not work directly from this function.
    // It has to be made asynchronously.
    iStopTimerCallBack = new (ELeave) CAsyncCallBack( TCallBack( CMemSpyViewHeapTracking::AsyncStopTimerCallback, this ), CActive::EPriorityStandard );
    iStopTimerCallBack->CallBack();
    }


TInt CMemSpyViewHeapTracking::IndexByViewTypeL( TMemSpyViewType aType )
    {
    // Get current config	
    TMemSpyEngineHelperSysMemTrackerConfig config;    
    
    GetSwmtConfigL( config );
    
    TInt index = 0;
    //
    switch( aType )
        {
    default:
    case EMemSpyViewTypeHeapTrackingResults:
        {
        if ( config.iMode == TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom )
            {
            index = 4;
            }
        else
            {
            index = 3;
            }
        break;
        }
    case EMemSpyViewTypeHeapTrackingSettings:
        index = 3;
        break;
        }
    //
    return index;
    }


void CMemSpyViewHeapTracking::SetConfigByModeL( TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode aMode, TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    switch ( aMode )
        {
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeBasic:
            {
            aConfig.iMode = aMode;
            aConfig.iEnabledCategories = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap |
                                         //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks |
                                         //TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData |
                                         TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory;
            aConfig.iDumpData = EFalse;
            break;
            }
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeFull:
            {
            aConfig.iMode = aMode;
            aConfig.iEnabledCategories = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
            aConfig.iDumpData = ETrue;
            aConfig.iThreadNameFilter = KNullDesC;
            break;
            }
        case TMemSpyEngineHelperSysMemTrackerConfig::MemSpyEngineSysMemTrackerModeCustom:
            {
            aConfig.iMode = aMode;
            aConfig.iEnabledCategories = iOriginalConfig.iEnabledCategories;
            aConfig.iDumpData = iOriginalConfig.iDumpData;
            aConfig.iThreadNameFilter = iOriginalConfig.iThreadNameFilter;
            break;
            }
        default: User::Leave( KErrArgument );
        }
    // Push changes to SWMT
    SetSwmtConfigL( aConfig );
    Settings().StoreSettingsL();
    }


TInt CMemSpyViewHeapTracking::AsyncStopTimerCallback( TAny* aParam )
    {
    CMemSpyViewHeapTracking* view = static_cast<CMemSpyViewHeapTracking*>( aParam );
    return view->AsyncStopTimerCallback();
    }


TInt CMemSpyViewHeapTracking::AsyncStopTimerCallback()
    {
    iState = EMemSpyViewHeapTrackingStateIdle;
   // iEngine.HelperSysMemTracker().RemoveObserver( this );
    TRAP_IGNORE(
    	iMemSpySession.StopSwmtTimerL();
        RefreshL();
        );
    delete iStopTimerCallBack;
    iStopTimerCallBack = NULL;
    return KErrNone;
    }

void CMemSpyViewHeapTracking::GetSwmtConfigL( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
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
		
	TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode mode;
	iMemSpySession.GetSwmtMode( mode );
	aConfig.iMode = mode;	     	
	}

void CMemSpyViewHeapTracking::SetSwmtConfigL( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
	{
	iMemSpySession.SetSwmtMode( aConfig.iMode );
	iMemSpySession.SetSwmtCategoriesL( aConfig.iEnabledCategories );
	iMemSpySession.SetSwmtTimerIntervalL( aConfig.iTimerPeriod.Int() );
	iMemSpySession.SetSwmtHeapDumpsEnabledL( aConfig.iDumpData );
	iMemSpySession.SetSwmtFilter( aConfig.iThreadNameFilter );
	}

void CMemSpyViewHeapTracking::DumpNowL()
	{
	iState = EMemSpyViewHeapTrackingStateSingleOn;	            	
		
	CMemSpySwmtDumpTracker* tracker = new (ELeave) CMemSpySwmtDumpTracker( iMemSpySession );		
	
	tracker->StartL();		
	}

void CMemSpyViewHeapTracking::StartSwmtTimerL()
	{
	CMemSpySwmtStartTimerTracker* tracker = new (ELeave) CMemSpySwmtStartTimerTracker( iMemSpySession );
	
	tracker->StartL();
	}
