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

#include "MemSpyViewThreadInfoItemMemoryTracking.h"

// System includes
#include <AknQueryDialog.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyengineprocessmemorytracker.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// User includes
#include "MemSpyContainerObserver.h"


CMemSpyViewThreadInfoItemMemoryTracking::CMemSpyViewThreadInfoItemMemoryTracking( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeMemoryTracking )
    {
    }


void CMemSpyViewThreadInfoItemMemoryTracking::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverAddL( *this );

    // Reset the title
    _LIT( KCustomTitle, "Memory Statistics" );
    SetTitleL( KCustomTitle );
    
    // Select the appropriate view
    const TMemSpyViewType viewType = (TMemSpyViewType) ((TInt) aSelectionRune );
    const TInt index = IndexByViewType( viewType );
    //
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyViewThreadInfoItemMemoryTracking::ConstructL() - aSelectionRune: 0x%08x, viewType: %d, index: %d", aSelectionRune, viewType, index );
#endif
    //
    if  ( index >= 0 && index < iListBox->Model()->NumberOfItems() )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        }
    }


TMemSpyViewType CMemSpyViewThreadInfoItemMemoryTracking::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemMemoryTrackingStatistics;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemMemoryTracking::PrepareParentViewL()
    {
    CMemSpyViewBase* parent = CMemSpyViewThreadInfoItemGeneric::PrepareParentViewL();
    
    // Stop observing changes
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverRemove( *this );

    return parent;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemMemoryTracking::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    //
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    const TInt index = iListBox->CurrentItemIndex();
    if  ( index == 1 )
        {
        child = new(ELeave) CMemSpyViewThreadInfoItemMemoryTrackingCurrent( iEngine, iObserver, Container() );
        }
    else if ( index == 2 )
        {
        child = new(ELeave) CMemSpyViewThreadInfoItemMemoryTrackingHWM( iEngine, iObserver, Container() );
        }
    else if ( index == 3 )
        {
        child = new(ELeave) CMemSpyViewThreadInfoItemMemoryTrackingPeak( iEngine, iObserver, Container() );
        }
    //        
    if  ( child )
        {
        CleanupStack::PushL( child );
        child->ConstructL( Rect(), *Parent() );
        CleanupStack::Pop( child );
           
        // Stop observing changes
        item->TrackingObserverRemove( *this );
        }
    //
    return child;
    }


TBool CMemSpyViewThreadInfoItemMemoryTracking::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdMemoryTrackingStart:
        OnCmdTrackingStartL();
        break;
    case EMemSpyCmdMemoryTrackingStop:
        OnCmdTrackingStopL();
        break;
    case EMemSpyCmdMemoryTrackingHWMReset:
        OnCmdHWMResetL();
        break;
    case EMemSpyCmdMemoryTrackingTotalWithSharedMem:
        OnCmdTotalWithSharedMemL();
        break;
    case EMemSpyCmdMemoryTrackingTotalWithoutSharedMem:
        OnCmdTotalWithoutSharedMemL();
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


TInt CMemSpyViewThreadInfoItemMemoryTracking::IndexByViewType( TMemSpyViewType aType )
    {
    TInt index = 0;
    //
    switch( aType )
        {
    default:
    case EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsCurrent:
        index = 1;
        break;
    case EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsHWM:
        index = 2;
        break;
    case EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsPeak:
        index = 3;
        break;
        }
    //
    return index;
    }


void CMemSpyViewThreadInfoItemMemoryTracking::HandleListBoxItemActionedL( TInt aIndex )
    {
    if  ( aIndex == 0 )
        {
        CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
        if  ( item->TrackingActive() )
            {
            OnCmdTrackingStopL();
            }
        else
            {
            OnCmdTrackingStartL();
            }
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );

    // Ensure we refresh the listbox content
    iListBox->DrawDeferred();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    iListBox->DrawDeferred();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == MenuCascadeResourceId() )
        {
        CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
        //
        if  ( !item->TrackerExists() || item->TrackingActive() )
            {
            aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingStart, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingStop, ETrue );
            }
        //
        if ( item->TotalIncludesSharedMemory() )
            {
            aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingTotalWithSharedMem, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingTotalWithoutSharedMem, ETrue );
            }

        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingHWMReset, Thread().IsDead() );
        }
    else if ( aResourceId == R_MEMSPY_MENUPANE )
        {
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTracking, Thread().IsDead() );
        aMenuPane->SetItemDimmed( EMemSpyCmdMemoryTrackingAutoStart, Thread().IsDead() );
        }
    }


void CMemSpyViewThreadInfoItemMemoryTracking::HandleMemoryTrackingStartedL()
    {
    RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::HandleMemoryTrackingStoppedL()
    {
    RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aCurrentInfo*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyViewThreadInfoItemMemoryTracking::HandleMemoryChangedL() - START - this: 0x%08x", this );
#endif 

    CMemSpyViewBase::RefreshL();
    iListBox->DrawDeferred();

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyViewThreadInfoItemMemoryTracking::HandleMemoryChangedL() - END - this: 0x%08x", this );
#endif 
    }


void CMemSpyViewThreadInfoItemMemoryTracking::OnCmdTrackingStartL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingStartL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::OnCmdTrackingStopL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingStopL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::OnCmdHWMResetL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingResetHWML();
    RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::OnCmdTotalWithSharedMemL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingSetTotalIncludesSharedMemoryL( ETrue );
    RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTracking::OnCmdTotalWithoutSharedMemL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingSetTotalIncludesSharedMemoryL( EFalse );
    RefreshL();
    }









































CMemSpyViewThreadInfoItemMemoryTrackingCurrent::CMemSpyViewThreadInfoItemMemoryTrackingCurrent( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeMemoryTracking )
    {
    }


void CMemSpyViewThreadInfoItemMemoryTrackingCurrent::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );

    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverAddL( *this );

    // Reset the title
    _LIT( KCustomTitle, "Current Statistics" );
    SetTitleL( KCustomTitle );
    }


TMemSpyViewType CMemSpyViewThreadInfoItemMemoryTrackingCurrent::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsCurrent;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemMemoryTrackingCurrent::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemMemoryTracking* parent = new(ELeave) CMemSpyViewThreadInfoItemMemoryTracking( iEngine, iObserver, Container() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    
    // Stop observing changes
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverRemove( *this );

    return parent;
    }


void CMemSpyViewThreadInfoItemMemoryTrackingCurrent::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTrackingCurrent::SetListBoxModelL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    MDesCArray* model = &item->InfoCurrent();

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }
    

void CMemSpyViewThreadInfoItemMemoryTrackingCurrent::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aCurrentInfo*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
    CMemSpyViewBase::RefreshL();
    }










































CMemSpyViewThreadInfoItemMemoryTrackingHWM::CMemSpyViewThreadInfoItemMemoryTrackingHWM( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeMemoryTracking )
    {
    }


void CMemSpyViewThreadInfoItemMemoryTrackingHWM::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );

    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverAddL( *this );

    // Reset the title
    _LIT( KCustomTitle, "HWM Statistics" );
    SetTitleL( KCustomTitle );
    }


TMemSpyViewType CMemSpyViewThreadInfoItemMemoryTrackingHWM::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsHWM;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemMemoryTrackingHWM::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemMemoryTracking* parent = new(ELeave) CMemSpyViewThreadInfoItemMemoryTracking( iEngine, iObserver, Container() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    
    // Stop observing changes
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverRemove( *this );

    return parent;
    }


TBool CMemSpyViewThreadInfoItemMemoryTrackingHWM::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdMemoryTrackingHWMReset:
        OnCmdHWMResetL();
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewThreadInfoItemMemoryTrackingHWM::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTrackingHWM::SetListBoxModelL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    MDesCArray* model = &item->InfoHWM();

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }
    

void CMemSpyViewThreadInfoItemMemoryTrackingHWM::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aCurrentInfo*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTrackingHWM::OnCmdHWMResetL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingResetHWML();
    }


























CMemSpyViewThreadInfoItemMemoryTrackingPeak::CMemSpyViewThreadInfoItemMemoryTrackingPeak( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeMemoryTracking )
    {
    }


void CMemSpyViewThreadInfoItemMemoryTrackingPeak::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );

    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverAddL( *this );

    // Reset the title
    _LIT( KCustomTitle, "Peak Statistics" );
    SetTitleL( KCustomTitle );
    }


TMemSpyViewType CMemSpyViewThreadInfoItemMemoryTrackingPeak::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemMemoryTrackingStatisticsPeak;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemMemoryTrackingPeak::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemMemoryTracking* parent = new(ELeave) CMemSpyViewThreadInfoItemMemoryTracking( iEngine, iObserver, Container() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    
    // Stop observing changes
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    item->TrackingObserverRemove( *this );

    return parent;
    }


TBool CMemSpyViewThreadInfoItemMemoryTrackingPeak::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewThreadInfoItemMemoryTrackingPeak::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewThreadInfoItemMemoryTrackingPeak::SetListBoxModelL()
    {
    CMemSpyThreadInfoMemoryTracking* item = static_cast< CMemSpyThreadInfoMemoryTracking* >( iInfoItem );
    MDesCArray* model = &item->InfoPeak();

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }
    

void CMemSpyViewThreadInfoItemMemoryTrackingPeak::HandleMemoryChangedL( const TProcessId& /*aPid*/, const TMemSpyDriverProcessInspectionInfo& /*aCurrentInfo*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoIncShared*/, const TMemSpyDriverProcessInspectionInfo& /*aHWMInfoExcShared*/ )
    {
    CMemSpyViewBase::RefreshL();
    }



























