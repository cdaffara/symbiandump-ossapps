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

#include "MemSpyViewECom.h"

// System includes
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginehelperecom.h>

// User includes
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"



CMemSpyViewECom::CMemSpyViewECom( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


void CMemSpyViewECom::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "ECom" );
    SetTitleL( KTitle );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Select first item
    TInt index = KErrNotFound;
    if  ( aSelectionRune != NULL )
        {
        iCurrent = reinterpret_cast< CMemSpyEngineEComCategory* >( aSelectionRune );
        index = iEngine.HelperECom().IndexOf( *iCurrent );
        }

    if  ( index >= 0 && index < iEngine.HelperECom().Count() )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewECom::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewECom::ViewType() const
    {
    return EMemSpyViewTypeECom;
    }


CMemSpyViewBase* CMemSpyViewECom::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewECom::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewEComCategory( iEngine, iObserver, *iCurrent );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewECom::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iEngine.HelperECom() );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewECom::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iEngine.HelperECom().Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComCategory& category = iEngine.HelperECom().At( aIndex );
        iCurrent = &category;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewECom::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iEngine.HelperECom().Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComCategory& category = iEngine.HelperECom().At( aIndex );
        iCurrent = &category;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }













CMemSpyViewEComCategory::CMemSpyViewEComCategory( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComCategory& aCategory )
:   CMemSpyViewBase( aEngine, aObserver ), iCategory( aCategory )
    {
    }


void CMemSpyViewEComCategory::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    SetTitleL( iCategory.Name() );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Select first item
    TInt index = KErrNotFound;
    if  ( aSelectionRune != NULL )
        {
        iCurrent = reinterpret_cast< CMemSpyEngineEComInterface* >( aSelectionRune );
        index = iCategory.IndexOf( *iCurrent );
        }

    if  ( index >= 0 && index < iCategory.Count() )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewEComCategory::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewEComCategory::ViewType() const
    {
    return EMemSpyViewTypeEComCategory;
    }


CMemSpyViewBase* CMemSpyViewEComCategory::PrepareParentViewL()
    {
    CMemSpyViewECom* parent = new(ELeave) CMemSpyViewECom( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iCategory );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewEComCategory::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewEComInterface( iEngine, iObserver, *iCurrent );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewEComCategory::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iCategory );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewEComCategory::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iCategory.Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComInterface& entry = iCategory.At( aIndex );
        iCurrent = &entry;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewEComCategory::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iCategory.Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComInterface& entry = iCategory.At( aIndex );
        iCurrent = &entry;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


























CMemSpyViewEComInterface::CMemSpyViewEComInterface( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComInterface& aInterface )
:   CMemSpyViewBase( aEngine, aObserver ), iInterface( aInterface )
    {
    }


void CMemSpyViewEComInterface::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    SetTitleL( iInterface.Name() );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Select first item
    TInt index = KErrNotFound;
    if  ( aSelectionRune != NULL )
        {
        iCurrent = reinterpret_cast< CMemSpyEngineEComImplementation* >( aSelectionRune );
        index = iInterface.IndexOf( *iCurrent );
        }

    if  ( index >= 0 && index < iInterface.Count() )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewEComInterface::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewEComInterface::ViewType() const
    {
    return EMemSpyViewTypeEComInterface;
    }


CMemSpyViewBase* CMemSpyViewEComInterface::PrepareParentViewL()
    {
    CMemSpyViewEComCategory* parent = new(ELeave) CMemSpyViewEComCategory( iEngine, iObserver, iInterface.Category() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iInterface );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewEComInterface::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewEComImplementation( iEngine, iObserver, *iCurrent );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewEComInterface::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iInterface );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewEComInterface::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iInterface.Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComImplementation& entry = iInterface.At( aIndex );
        iCurrent = &entry;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewEComInterface::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iInterface.Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineEComImplementation& entry = iInterface.At( aIndex );
        iCurrent = &entry;
        }
    else
        {
        iCurrent = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }



































CMemSpyViewEComImplementation::CMemSpyViewEComImplementation( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineEComImplementation& aImplementation )
:   CMemSpyViewBase( aEngine, aObserver ), iImplementation( aImplementation )
    {
    }


void CMemSpyViewEComImplementation::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    SetTitleL( iImplementation.Name() );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Select first item
    if  ( iImplementation.MdcaCount() )
        {
        iListBox->SetCurrentItemIndex( 0 );
        HandleListBoxItemSelectedL( 0 );
        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewEComImplementation::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewEComImplementation::ViewType() const
    {
    return EMemSpyViewTypeEComImplementation;
    }


CMemSpyViewBase* CMemSpyViewEComImplementation::PrepareParentViewL()
    {
    CMemSpyViewEComInterface* parent = new(ELeave) CMemSpyViewEComInterface( iEngine, iObserver, iImplementation.Interface() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iImplementation );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewEComImplementation::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iImplementation );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

