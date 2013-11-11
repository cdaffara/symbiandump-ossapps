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

#include "MemSpyViewThreadInfoItemServer.h"

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

// User includes
#include "MemSpyContainerObserver.h"

// Constants
const TInt KMemSpyServerSessionsIndex = 2;



CMemSpyViewThreadInfoItemServer::CMemSpyViewThreadInfoItemServer( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewThreadInfoItemGeneric( aSession, aObserver, aProcId, aId, EMemSpyThreadInfoItemTypeServer )
    {
    }


void CMemSpyViewThreadInfoItemServer::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    // Causes list of server's to be prepared
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );

    _LIT( KTitle, "Server List" );
    SetTitleL( KTitle );

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreadInfoItemServer::ConstructL() - aSelectionRune: 0x%08x", aSelectionRune );
#endif
    /* TODO
    // Try to select the correct server
    CMemSpyThreadInfoHandleObjectBase* infoItem = static_cast< CMemSpyThreadInfoHandleObjectBase* >( iInfoItem );
    */
    TInt selectedIndex = 0;
    /*if  ( aSelectionRune )
        {
        // Treat the rune as a handle, and try to look it up
        selectedIndex = infoItem->DetailsIndexByHandle( aSelectionRune );
        }

    // Select item
    if  ( infoItem->DetailsCount() > 0 )
        {*/
        iListBox->SetCurrentItemIndex( selectedIndex );
        HandleListBoxItemSelectedL( selectedIndex );
        /*}*/
    }


TBool CMemSpyViewThreadInfoItemServer::HandleCommandL( TInt aCommand )
    {
    TBool handled = CMemSpyViewBase::HandleCommandL( aCommand );
    return handled;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemServer::PrepareChildViewL()
    {
	/*
    CMemSpyViewThreadInfoItemServerDetails* child = new(ELeave) CMemSpyViewThreadInfoItemServerDetails( iEngine, iObserver, iContainer, iCurrentInfoItemDetails );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    */
    }


void CMemSpyViewThreadInfoItemServer::HandleListBoxItemSelectedL( TInt aIndex )
    {/* TODO
    // Identify the type of item to display...
    CMemSpyThreadInfoHandleObjectBase* infoItem = static_cast< CMemSpyThreadInfoHandleObjectBase* >( iInfoItem );
    iCurrentInfoItemDetails = infoItem->DetailsAt( aIndex );

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreadInfoItemServer::HandleListBoxItemSelectedL() - iCurrentInfoItemDetails.iHandle: 0x%08x", iCurrentInfoItemDetails.iHandle );
#endif
*/
    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }

























/*
CMemSpyViewThreadInfoItemServerDetails::CMemSpyViewThreadInfoItemServerDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, const TMemSpyDriverHandleInfoGeneric& aInfoItemDetails )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeServer ), iInfoItemDetails( aInfoItemDetails )
    {
    }


void CMemSpyViewThreadInfoItemServerDetails::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    // Causes list of server's to be prepared
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );

    _LIT( KTitle, "Server Details" );
    SetTitleL( KTitle );

    // Try to select the correct server
    TInt selectedIndex = 0;
    if  ( aSelectionRune )
        {
        // Presence of non-null item means that we came from the "Server sessions" list
        selectedIndex = KMemSpyServerSessionsIndex;
        }

    // Select item
    iListBox->SetCurrentItemIndex( selectedIndex );
    HandleListBoxItemSelectedL( selectedIndex );
    }


TBool CMemSpyViewThreadInfoItemServerDetails::HandleCommandL( TInt aCommand )
    {
    TBool handled = CMemSpyViewBase::HandleCommandL( aCommand );
    return handled;
    }


TMemSpyViewType CMemSpyViewThreadInfoItemServerDetails::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemServerDetails;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemServerDetails::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    const TInt currentIndex = iListBox->CurrentItemIndex();
    //
    if  ( currentIndex == KMemSpyServerSessionsIndex )
        {
        CMemSpyViewThreadInfoItemServerSessions* serverSessions = new(ELeave) CMemSpyViewThreadInfoItemServerSessions( iEngine, iObserver, iInfoItem->Container(), iInfoItemDetails );
        CleanupStack::PushL( serverSessions );
        serverSessions->ConstructL( Rect(), *Parent(), NULL );
        CleanupStack::Pop( serverSessions );
        child = serverSessions;
        }
    //
    return child;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemServerDetails::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemServer* parent = new(ELeave) CMemSpyViewThreadInfoItemServer( iEngine, iObserver, iInfoItem->Container() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), iInfoItemDetails.iHandle );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewThreadInfoItemServerDetails::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    // 1st item = session type
    const TPtrC pServerSessionType = CMemSpyThreadInfoServer::SessionType( iInfoItemDetails.iSessionType );
    _LIT(KItem1Format, "\tServer Type\t\t%S");
    item.Format( KItem1Format, &pServerSessionType );
    model->AppendL( item );

    // 2nd item = session count
    _LIT(KItem2Format, "\tConnected Sessions\t\t%d");
    item.Format( KItem2Format, iInfoItemDetails.iCount );
    model->AppendL( item );

    // 3rd item = view sessions
    if  ( iInfoItem->Container().MdcaCount() > 0 )
        {
        _LIT(KItem3Format, "\tView Sessions");
        model->AppendL( KItem3Format );
        }

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }

*/
/*
void CMemSpyViewThreadInfoItemServerDetails::HandleListBoxItemActionedL( TInt /*aIndex*/ /*)
*/
/*    
   	{
    // Notify observer about an item being 'fired'    
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewThreadInfoItemServerDetails::HandleListBoxItemSelectedL( TInt /*aIndex*/ /*)
*/
/*
    {
    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }
*/
























/*


CMemSpyViewThreadInfoItemServerSessions::CMemSpyViewThreadInfoItemServerSessions( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer, const TMemSpyDriverHandleInfoGeneric& aInfoItemDetails )
:   CMemSpyViewThreadInfoItemGeneric( aEngine, aObserver, aContainer, EMemSpyThreadInfoItemTypeServer ), iInfoItemDetails( aInfoItemDetails )
    {
    }


void CMemSpyViewThreadInfoItemServerSessions::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    CMemSpyViewThreadInfoItemGeneric::ConstructL( aRect, aContainer, aSelectionRune );
    //
    _LIT( KTitle, "Connected Session List" );
    SetTitleL( KTitle );
    }


TBool CMemSpyViewThreadInfoItemServerSessions::HandleCommandL( TInt aCommand )
    {
    TBool handled = CMemSpyViewBase::HandleCommandL( aCommand );
    return handled;
    }


TMemSpyViewType CMemSpyViewThreadInfoItemServerSessions::ViewType() const
    {
    return EMemSpyViewTypeThreadInfoItemServerSessions;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemServerSessions::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemServerDetails* parent = new(ELeave) CMemSpyViewThreadInfoItemServerDetails( iEngine, iObserver, iInfoItem->Container(), iInfoItemDetails );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), this );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewThreadInfoItemServerSessions::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL( model );

    // Get handles
    RArray<TMemSpyDriverServerSessionInfo> sessions;
    CleanupClosePushL( sessions );
    iEngine.HelperServer().GetServerSessionsL( iInfoItemDetails, sessions );

    // Prepare items
    TBuf<KMaxFullName + 1> item;
    const TInt count = sessions.Count();
    for( TInt i=0; i<count; i++ )
        {
        _LIT(KInsertTab, "\t");

        const TMemSpyDriverServerSessionInfo& session = sessions[ i ];
        item.Zero();

        if  ( session.iOwnerType == TMemSpyDriverServerSessionInfo::EOwnerThread )
            {
            // Find the thread object
            CMemSpyThread* thread = NULL;
            CMemSpyProcess* process = NULL;
            //
            const TInt error = iEngine.Container().ProcessAndThreadByThreadId( session.iOwnerId, process, thread );
            //
            if  ( error == KErrNone )
                {
                item.Append( process->Name() );
                item.Append( KInsertTab );
                item.Append( KInsertTab );
                item.Append( thread->Name() );
                }
            }
        else if ( session.iOwnerType == TMemSpyDriverServerSessionInfo::EOwnerProcess )
            {
            // Find the process object
            CMemSpyProcess* process = NULL;
            TRAPD( error, process = &iEngine.Container().ProcessByIdL( session.iOwnerId ) );
            if  ( error == KErrNone )
                {
                item.Append( process->Name() );
                }
            }

        if  ( !item.Length() )
            {
            // Fall-back case
            item.Copy( session.iName );
            }

        // Add item
        item.Insert( 0, KInsertTab );
        model->AppendL( item );
        }
    CleanupStack::PopAndDestroy( &sessions );

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }

*/









