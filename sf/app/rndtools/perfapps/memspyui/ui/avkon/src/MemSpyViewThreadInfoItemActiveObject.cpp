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

#include "MemSpyViewThreadInfoItemActiveObject.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperactiveobject.h>
#include <memspysession.h>

// User includes
#include "MemSpyContainerObserver.h"


CMemSpyViewThreadInfoItemActiveObjectBase::CMemSpyViewThreadInfoItemActiveObjectBase(RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemGeneric( aSession, aObserver, aProcId, aId, EMemSpyThreadInfoItemTypeActiveObject )
    {
    }


TBool CMemSpyViewThreadInfoItemActiveObjectBase::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    /*
    //
    switch ( aCommand )
        {
    case EMemSpyCmdActiveObjectListing:
        OnCmdWriteAOListingL();
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
     * */
    return handled;
    }


void CMemSpyViewThreadInfoItemActiveObjectBase::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == R_MEMSPY_MENUPANE )
        {
     //   aMenuPane->SetItemDimmed( EMemSpyCmdActiveObject, Thread().IsDead() );
        }
    }


CMemSpyEngineActiveObjectArray& CMemSpyViewThreadInfoItemActiveObjectBase::ActiveObjectArray() const
    {
 //   CMemSpyThreadInfoActiveObjects* activeObjectArray = static_cast< CMemSpyThreadInfoActiveObjects* >( iInfoItem );
 //   return activeObjectArray->Array();
    }
    
    
void CMemSpyViewThreadInfoItemActiveObjectBase::OnCmdWriteAOListingL()
    {
	iMemSpySession.OutputAOListL( ThreadId(), EMemSpyThreadInfoItemTypeActiveObject );
	
	/*
    CMemSpyEngineActiveObjectArray& objects = ActiveObjectArray();

    // Begin a new data stream
    _LIT( KMemSpyContext, "Active Object List - " );
    _LIT( KMemSpyFolder, "Active Objects" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    // Set prefix for overall listing
    iEngine.Sink().OutputPrefixSetLC( KMemSpyContext );

    // Create header
    CMemSpyEngineActiveObjectArray::OutputDataColumnsL( iEngine );
    
    // List items
    const TInt count = objects.Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineActiveObject& object = objects.At( i );
        //
        object.OutputDataL( iEngine );
        }

    // Tidy up
    CleanupStack::PopAndDestroy(); // prefix

    // End data stream
    iEngine.Sink().DataStreamEndL();
    */
    }











CMemSpyViewThreadInfoItemActiveObject::CMemSpyViewThreadInfoItemActiveObject( RMemSpySession& aSession, MMemSpyViewObserver& aObserver,  TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemActiveObjectBase( aSession, aObserver, aProcId, aId, aType )
    {
    }


void CMemSpyViewThreadInfoItemActiveObject::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    // Causes list of server's to be prepared
    CMemSpyViewThreadInfoItemActiveObjectBase::ConstructL( aRect, aContainer, aSelectionRune );

    _LIT( KTitle, "Active Objects" );
    SetTitleL( KTitle );

    // Try to select the correct server
    /* TODO: to consider this
    CMemSpyThreadInfoHandleObjectBase* infoItem = static_cast< CMemSpyThreadInfoHandleObjectBase* >( iInfoItem );
    TInt selectedIndex = 0;
    if  ( aSelectionRune )
        {
        CMemSpyEngineActiveObjectArray& activeObjectArray = ActiveObjectArray();
        const TInt index = activeObjectArray.ObjectIndexByAddress( aSelectionRune );
        if  ( index >= 0 && index < activeObjectArray.Count() )
            {
            selectedIndex = index + 1;
            }
        }

    // Select item
    if  ( infoItem->DetailsCount() > 0 )
        {
        */
        iListBox->SetCurrentItemIndex( 0 ); //selectedIndex
        HandleListBoxItemSelectedL( 0 ); //selectedIndex
        /*
        }
        */
    }


TMemSpyViewType CMemSpyViewThreadInfoItemActiveObject::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemActiveObject;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemActiveObject::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    /*
    // Get current entry address
    TInt index = iListBox->CurrentItemIndex();
    if  ( index > 0 )
        {
        --index;
        CMemSpyEngineActiveObjectArray& activeObjectArray = ActiveObjectArray();
        //
        if  ( index >= 0 && index < activeObjectArray.MdcaCount() )
            {
            CMemSpyEngineActiveObject& object = activeObjectArray.At( index );
            TAny* aoAddress = object.Address();
            //
            child = new(ELeave) CMemSpyViewThreadInfoItemActiveObjectDetails( iEngine, iObserver, iInfoItem->Container(), aoAddress );
            CleanupStack::PushL( child );
            child->ConstructL( Rect(), *Parent(), NULL );
            CleanupStack::Pop( child );
            }
        }
    //
     * 
     */
    return child;
    }










/*
CMemSpyViewThreadInfoItemActiveObjectDetails::CMemSpyViewThreadInfoItemActiveObjectDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, TAny* aObjectAddress )
:   CMemSpyViewThreadInfoItemActiveObjectBase( aEngine, aObserver, aContainer ), iObjectAddress( aObjectAddress )
    {
    }


void CMemSpyViewThreadInfoItemActiveObjectDetails::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemActiveObjectBase::ConstructL( aRect, aContainer, aSelectionRune );

    _LIT( KTitle, "Active Object Details" );
    SetTitleL( KTitle );
    }
    

TMemSpyViewType CMemSpyViewThreadInfoItemActiveObjectDetails::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemActiveObjectDetails;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemActiveObjectDetails::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemActiveObject* parent = new(ELeave) CMemSpyViewThreadInfoItemActiveObject( iEngine, iObserver, iInfoItem->Container() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), iObjectAddress );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewThreadInfoItemActiveObjectDetails::SetListBoxModelL()
    {
    // Try to find the right entry
    CMemSpyEngineActiveObjectArray& activeObjectArray = ActiveObjectArray();
    const TInt index = activeObjectArray.ObjectIndexByAddress( iObjectAddress );
    User::LeaveIfError( index );
    CMemSpyEngineActiveObject& object = activeObjectArray.At( index );
    //
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &object );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }
*/
