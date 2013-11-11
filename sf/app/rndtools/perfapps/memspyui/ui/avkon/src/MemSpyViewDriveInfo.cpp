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

#include "MemSpyViewDriveInfo.h"

// System includes
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"



CMemSpyViewDriveInfoBase::CMemSpyViewDriveInfoBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


CMemSpyViewDriveInfoBase::~CMemSpyViewDriveInfoBase()
    {
    if  ( iList )
        {
        iList->SetObserver( NULL );
        }
    //
    delete iList;
    }


void CMemSpyViewDriveInfoBase::BaseConstructL()
    {
    iList = iEngine.HelperFileSystem().DriveListL();
    }


TBool CMemSpyViewDriveInfoBase::HandleCommandL( TInt aCommand )
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






























CMemSpyViewDriveList::CMemSpyViewDriveList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewDriveInfoBase( aEngine, aObserver ), iUseDriveNumber( EFalse ), iDriveNumber( EDriveA )
    {
    }


CMemSpyViewDriveList::CMemSpyViewDriveList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TDriveNumber aDriveNumber )
:   CMemSpyViewDriveInfoBase( aEngine, aObserver ), iUseDriveNumber( ETrue ), iDriveNumber( aDriveNumber )
    {
    }


void CMemSpyViewDriveList::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    BaseConstructL();

    _LIT( KTitle, "Drive Summary" );
    SetTitleL( KTitle );

    // Start watching list
    iList->SetObserver( this );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    if  ( iUseDriveNumber )
        {
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyViewDriveList::ConstructL() - iDriveNumber: %c:", iDriveNumber + 'A' );
#endif
        iCurrentDrive = iList->EntryByDriveNumber( iDriveNumber );

#ifdef _DEBUG
        RDebug::Printf( "CMemSpyViewDriveList::ConstructL() - iCurrentDrive: 0x%08x", iCurrentDrive );
#endif
        if  ( iCurrentDrive )
            {
            const TInt index = iList->ItemIndex( *iCurrentDrive );
#ifdef _DEBUG
            RDebug::Printf( "CMemSpyViewDriveList::ConstructL() - index: %d", index );
#endif
            iListBox->SetCurrentItemIndex( index );
            HandleListBoxItemSelectedL( index );
            }
        }
    
    iListBox->DrawDeferred();
    }


TMemSpyViewType CMemSpyViewDriveList::ViewType() const
    {
    return EMemSpyViewTypeDriveSummary;
    }


CMemSpyViewBase* CMemSpyViewDriveList::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    //
    if  ( parent )
        {
        iList->SetObserver( NULL );
        }
    //
    return parent;
    }


CMemSpyViewBase* CMemSpyViewDriveList::PrepareChildViewL()
    {
    ASSERT( iCurrentDrive );
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewDriveInfo( iEngine, iObserver, iCurrentDrive->DriveNumber() );

    // Stop watching list
    if  ( child )
        {
        iList->SetObserver( NULL );
        }

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iList = NULL;

    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewDriveList::RefreshL()
    {
    if  ( iList )
        {
        iList->SetObserver( NULL );
        }
    //
    CMemSpyEngineDriveList* list = iEngine.HelperFileSystem().DriveListL();
    delete iList;
    iList = list;
    iList->SetObserver( this );
    //
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewDriveList::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iList );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewDriveList::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineDriveEntry& entry = iList->At( aIndex );
        iCurrentDrive = &entry;
        }
    else
        {
        iCurrentDrive = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewDriveList::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineDriveEntry& entry = iList->At( aIndex );
        iCurrentDrive = &entry;
        }
    else
        {
        iCurrentDrive = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


void CMemSpyViewDriveList::HandleDriveListChangedL( const CMemSpyEngineDriveList& /*aList*/ )
    {
    RefreshL();
    }

































CMemSpyViewDriveInfo::CMemSpyViewDriveInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TDriveNumber aDriveNumber )
:   CMemSpyViewDriveInfoBase( aEngine, aObserver ), iDriveNumber( aDriveNumber )
    {
    }


void CMemSpyViewDriveInfo::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    BaseConstructL();
    //
    iDriveInfo = iList->EntryByDriveNumber( iDriveNumber );
    if  ( !iDriveInfo )
        {
        User::Leave( KErrNotFound );
        }
    iDriveInfo->SetObserver( this );

    _LIT( KTitleFormat, "Drive %S Info" );
    const TDriveName name( iDriveInfo->DriveUnit().Name() );
    TName title;
    title.Format( KTitleFormat, &name );
    SetTitleL( title );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewDriveInfo::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewDriveInfo::ViewType() const
    {
    return EMemSpyViewTypeDriveInfo;
    }


CMemSpyViewBase* CMemSpyViewDriveInfo::PrepareParentViewL()
    {
    ASSERT( iDriveInfo );
    CMemSpyViewDriveList* parent = new(ELeave) CMemSpyViewDriveList( iEngine, iObserver, iDriveInfo->DriveNumber() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( parent );
    //
    if  ( parent )
        {
        iDriveInfo->SetObserver( NULL );
        }
    //
    return parent;
    }


void CMemSpyViewDriveInfo::SetListBoxModelL()
    {
    if  ( iDriveInfo )
        {
        CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
        listbox->Model()->SetItemTextArray( iDriveInfo );
        listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
        }
    }


void CMemSpyViewDriveInfo::HandleDriveEntryChangedL( const CMemSpyEngineDriveEntry& /*aEntry*/ )
    {
    RefreshL();
    }












