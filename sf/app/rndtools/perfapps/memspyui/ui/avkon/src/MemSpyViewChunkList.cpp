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

#include "MemSpyViewChunkList.h"

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
#include <memspy/engine/memspyenginehelperchunk.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"



CMemSpyViewChunkBase::CMemSpyViewChunkBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList* aList )
:   CMemSpyViewBase( aEngine, aObserver ), iList( aList )
    {
    }


CMemSpyViewChunkBase::~CMemSpyViewChunkBase()
    {
    delete iList;
    }


TBool CMemSpyViewChunkBase::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdChunkSortByName:
        iList->SortByNameL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdChunkSortBySize:
        iList->SortBySizeL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdChunkListing:
        OnCmdListingL();
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewChunkBase::OnCmdListingL()
    {
    // Begin a new data stream
    _LIT( KMemSpyContext, "Chunk List" );
    _LIT( KMemSpyFolder, "Chunks" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    // Set prefix for overall listing
    _LIT( KOverallPrefix, "Chunk List - " );
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    // Create header
    CMemSpyEngineChunkList::OutputDataColumnsL( iEngine );
    
    // List items
    const TInt count = iList->Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineChunkEntry& entry = iList->At( i );
        //
        entry.OutputDataL( iEngine.HelperChunk() );
        }

    // Tidy up
    CleanupStack::PopAndDestroy(); // prefix

    // End data stream
    iEngine.Sink().DataStreamEndL();
    }






























CMemSpyViewChunkList::CMemSpyViewChunkList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewChunkBase( aEngine, aObserver, NULL )
    {
    }


CMemSpyViewChunkList::CMemSpyViewChunkList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList& aList )
:   CMemSpyViewChunkBase( aEngine, aObserver, &aList )
    {
    }


void CMemSpyViewChunkList::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Chunks" );
    SetTitleL( KTitle );

    if  ( iList == NULL )
        {
        // Prepare code segment list
        iList = iEngine.HelperChunk().ListL();
        }

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    if  ( aSelectionRune != NULL )
        {
        iCurrentChunk = reinterpret_cast< CMemSpyEngineChunkEntry* >( aSelectionRune );
        const TInt index = iList->ItemIndex( *iCurrentChunk );
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        }

    iListBox->DrawDeferred();
    }


TMemSpyViewType CMemSpyViewChunkList::ViewType() const
    {
    return EMemSpyViewTypeChunkList;
    }


CMemSpyViewBase* CMemSpyViewChunkList::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewChunkList::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewChunkDetails( iEngine, iObserver, *iList, *iCurrentChunk );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iList = NULL;

    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewChunkList::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iList );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewChunkList::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineChunkEntry& chunk = iList->At( aIndex );
        iCurrentChunk = &chunk;
        }
    else
        {
        iCurrentChunk = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewChunkList::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineChunkEntry& chunk = iList->At( aIndex );
        iCurrentChunk = &chunk;
        }
    else
        {
        iCurrentChunk = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }



































CMemSpyViewChunkDetails::CMemSpyViewChunkDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineChunkList& aList, CMemSpyEngineChunkEntry& aSelectedChunk )
:   CMemSpyViewChunkBase( aEngine, aObserver, &aList ), iChunk( aSelectedChunk )
    {
    }


void CMemSpyViewChunkDetails::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Chunk Details" );
    SetTitleL( KTitle );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewChunkDetails::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewChunkDetails::ViewType() const
    {
    return EMemSpyViewTypeChunkDetails;
    }


CMemSpyViewBase* CMemSpyViewChunkDetails::PrepareParentViewL()
    {
    CMemSpyViewChunkList* parent = new(ELeave) CMemSpyViewChunkList( iEngine, iObserver, *iList );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iList = NULL;

    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iChunk );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewChunkDetails::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iChunk );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }















