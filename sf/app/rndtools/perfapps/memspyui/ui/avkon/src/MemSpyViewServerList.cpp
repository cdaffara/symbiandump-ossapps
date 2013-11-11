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

#include "MemSpyViewServerList.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperserver.h>
#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyViewThreads.h"
#include "MemSpyContainerObserver.h"



/*
CMemSpyViewServerList::CMemSpyViewServerList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }
*/
CMemSpyViewServerList::CMemSpyViewServerList( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }



CMemSpyViewServerList::~CMemSpyViewServerList()
    {
    delete iList;
    
    iServers.ResetAndDestroy();
    }


void CMemSpyViewServerList::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
	iSort = ESortServByName;
    _LIT( KTitle, "Running Servers" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


TBool CMemSpyViewServerList::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdServerListSortByName:
        OnCmdServerListSortByNameL();
        break;
    case EMemSpyCmdServerListSortBySessionCount:
        OnCmdServerListSortBySessionCountL();
        break;
    case EMemSpyCmdServerListOutputListCSV:
        OnCmdServerListOutputSummaryL();
        break;
    case EMemSpyCmdServerListOutputListDetailed:
        OnCmdServerListOutputDetailedL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewServerList::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewServerList::ViewType() const
    {
    return EMemSpyViewTypeServerList;
    }


CMemSpyViewBase* CMemSpyViewServerList::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewServerList::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;

    if  ( iActionedItemIndex >= 0 )
        {
		//child = new(ELeave) CMemSpyViewProcesses( iMemSpySession, iObserver, iServers[iActionedItemIndex]->ProcessId() );
		child = new(ELeave) CMemSpyViewThreads( iMemSpySession, iObserver, iServers[iActionedItemIndex]->ProcessId(), iServers[iActionedItemIndex]->ThreadId() );
		CleanupStack::PushL( child );
		child->ConstructL( Rect(), *Parent() );
		CleanupStack::Pop( child );        	
        }
    //
    
    return child;
    }

void CMemSpyViewServerList::SetListBoxModelL()
    {
	TInt err = KErrNone;	
	
	RArray<CMemSpyApiServer*> servers;
	iMemSpySession.GetServersL( servers, iSort );
	CleanupClosePushL( servers );
	
	// copy servers to iServers array
	iServers.ResetAndDestroy();
	for (TInt i=0; i<servers.Count(); i++)
	    {
	    iServers.Append(servers[i]);
	    }
	
	CleanupStack::PopAndDestroy( &servers );
	
    CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iServers.Count() ); //array for formated items
    CleanupStack::PushL( model );
    
    _LIT(KSession, "session");		
    
    for( TInt i = 0; i < iServers.Count(); i++ )
        {		
        HBufC* combined = HBufC::NewLC( iServers[i]->Name().Length() + 128 );

        TPtr pCombined( combined->Des() );
        pCombined.Zero();
        pCombined.Copy( _L("\t") );
        
        if( iServers[i]->Name() != KNullDesC )
            {
            pCombined.Append( iServers[i]->Name() );
            }
        
        pCombined.Append( _L("\t\t") );
        TBuf<16> count;
        _LIT( KCount, "%d session");
        
        count.Format( KCount, iServers[i]->SessionCount() );
        pCombined.Append( count );
        
        if( iServers[i]->SessionCount() != 1 )				
            {
            _LIT( KS, "s" );
            pCombined.Append( KS );
            }
        
        model->AppendL( pCombined );	
        
        CleanupStack::PopAndDestroy( combined );
        }
	
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    CleanupStack::Pop( model );
    }


void CMemSpyViewServerList::HandleListBoxItemActionedL( TInt aCurrentIndex )
    {
    if  ( aCurrentIndex >= 0 && aCurrentIndex < iListBox->Model()->NumberOfItems() )
        {        
 		iActionedItemIndex = aCurrentIndex;
        }
    else
        {
		iActionedItemIndex = -1;
        }

    // Notify observer about an item being 'fired'
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );    
    }


void CMemSpyViewServerList::OnCmdServerListSortByNameL()
    {
	iSort = ESortServByName;
	RefreshL();
    }


void CMemSpyViewServerList::OnCmdServerListSortBySessionCountL()
    {
	iSort = ESortServBySessionCount;
	RefreshL();
    }
   

void CMemSpyViewServerList::OnCmdServerListOutputSummaryL()
    {
    OnCmdServerListOutputGenericL( EFalse );
    }


void CMemSpyViewServerList::OnCmdServerListOutputDetailedL()
    {
    OnCmdServerListOutputGenericL( ETrue );
    }


void CMemSpyViewServerList::OnCmdServerListOutputGenericL( TBool aDetailed )
    {
	iMemSpySession.ServerListOutputGenericL( aDetailed );			
    }







