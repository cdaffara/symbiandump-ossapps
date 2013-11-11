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

#include "MemSpyViewKernel.h"

// System includes
#include <hal.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyViewKernelHeap.h"
#include "MemSpyViewKernelContainers.h"

/*
CMemSpyViewKernel::CMemSpyViewKernel( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }
*/

CMemSpyViewKernel::CMemSpyViewKernel( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }


CMemSpyViewKernel::~CMemSpyViewKernel()
    {
    }


void CMemSpyViewKernel::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Kernel" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    
    // Make sure the correct item is selected
    const TMemSpyViewType viewType = (TMemSpyViewType) ((TInt) aSelectionRune);
    const TInt index = IndexByViewType( viewType );
    iListBox->SetCurrentItemIndex( index );
    HandleListBoxItemSelectedL( index );
    }


void CMemSpyViewKernel::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewKernel::ViewType() const
    {
    return EMemSpyViewTypeKernel;
    }


CMemSpyViewBase* CMemSpyViewKernel::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewKernel::PrepareChildViewL()
    {
	
    CMemSpyViewBase* child = NULL;
    const TInt index = iListBox->CurrentItemIndex();
    //
    if  ( index == 0 )
        {        
		child = new(ELeave) CMemSpyViewKernelContainers( iMemSpySession, iObserver );
        }
    else if ( index == 1 )
        {        
		child = new(ELeave) CMemSpyViewKernelHeap( iMemSpySession, iObserver );
        }
	
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;    
    }


void CMemSpyViewKernel::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    // 1st item = Processes
    _LIT( KItem1Format, "\tObjects" );
    item.Format( KItem1Format );
    model->AppendL( item );
    
    // 2nd item = System Config
    _LIT( KItem2Format, "\tHeap" );
    model->AppendL( KItem2Format );
	
    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


TInt CMemSpyViewKernel::IndexByViewType( TMemSpyViewType aType )
    {
    TInt index = 0;
    //
    switch( aType )
        {
    default:
    case EMemSpyViewTypeKernelContainers:
        index = 0;
        break;
        
    case EMemSpyViewTypeKernelHeap:
        index = 1;
        break;
        }
        
    //
    return index;
    }








