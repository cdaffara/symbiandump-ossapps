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

#include "MemSpyEngineHelperWindowServerImpl.h"

// System includes
#include <apgwgnam.h>
#include <APGTASK.h>

// ============================= LOCAL FUNCTIONS ===============================

EXPORT_C MMemSpyEngineHelperWindowServer* NewMemSpyEngineHelperWindowServerImplL()
    {
    return CMemSpyEngineHelperWindowServerImpl::NewL();
    }


// ============================ MEMBER FUNCTIONS ===============================

CMemSpyEngineHelperWindowServerImpl::CMemSpyEngineHelperWindowServerImpl()
    {
    }

    
CMemSpyEngineHelperWindowServerImpl::~CMemSpyEngineHelperWindowServerImpl()
    {
    iWsSession.Close();
    }


void CMemSpyEngineHelperWindowServerImpl::ConstructL()
    {
    User::LeaveIfError( iWsSession.Connect() );
    }


EXPORT_C CMemSpyEngineHelperWindowServerImpl* CMemSpyEngineHelperWindowServerImpl::NewL()
    {
    CMemSpyEngineHelperWindowServerImpl* self = new(ELeave) CMemSpyEngineHelperWindowServerImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C MMemSpyEngineWindowGroupList* CMemSpyEngineHelperWindowServerImpl::WindowGroupListL()
    {
    CMemSpyEngineWindowGroupListImpl* list = CMemSpyEngineWindowGroupListImpl::NewLC();
    //
    RArray<TMemSpyEngineWindowGroupBasicInfo> groups;
    CleanupClosePushL( groups );
    //
    GetWindowGroupListL( groups );
    //
    const TInt count = groups.Count();
    for( TInt i = 0; i < count; i++ )
        {
        list->AddItemL( groups[ i ] );
        }
    //
    CleanupStack::PopAndDestroy( &groups );

    // Done
    CleanupStack::Pop( list );
    return list;
    }


EXPORT_C void CMemSpyEngineHelperWindowServerImpl::GetWindowGroupListL( RArray<TMemSpyEngineWindowGroupBasicInfo>& aWindowGroups )
    {
    TInt count;
    User::LeaveIfError( count = iWsSession.NumWindowGroups() );
    RArray<RWsSession::TWindowGroupChainInfo> array;
    CleanupClosePushL( array );
    User::LeaveIfError( iWsSession.WindowGroupList( &array ) );
    for (TInt i =0; i < count; i++ )
        {
        TMemSpyEngineWindowGroupBasicInfo group;
        group.iId = array[i].iId;
        TThreadId threadId;
        RThread thread;
        iWsSession.GetWindowGroupClientThreadId( array[i].iId, threadId );
        group.iThreadId = threadId;
        group.iOrdinalPosition = i;
        if ( !thread.Open( threadId ) )
            {
            _LIT( KLineFormatSpec, "%d\t%S" );
            group.iFullName.Format( KLineFormatSpec, group.iId, &thread.Name() );
            }
        else
            {
            _LIT( KLineFormatSpec, "%d\t<Unresolved name>" );
            group.iFullName.Format( KLineFormatSpec, group.iId );
            }
        thread.Close();        
        aWindowGroups.AppendL( group );
        }
    CleanupStack::PopAndDestroy( &array );
    }


EXPORT_C void CMemSpyEngineHelperWindowServerImpl::GetWindowGroupDetailsL( TInt aWindowGroupId, TMemSpyEngineWindowGroupDetails& aWindowGroupDetails )
    {
    TThreadId threadId;
    RThread thread;
    iWsSession.GetWindowGroupClientThreadId( aWindowGroupId, threadId );
    aWindowGroupDetails.iThreadId = threadId;
    aWindowGroupDetails.iId = aWindowGroupId;
    if ( !thread.Open( threadId ) )
        {
        aWindowGroupDetails.iFullName.Copy( thread.Name() );
        }
    else
        {
        aWindowGroupDetails.iFullName.Copy( _L("<Unresolved name>") );
        }
    thread.Close();
    aWindowGroupDetails.iPriority = iWsSession.GetWindowGroupOrdinalPriority( aWindowGroupId );
    aWindowGroupDetails.iWindowGroupHandle = iWsSession.GetWindowGroupHandle( aWindowGroupId );
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( iWsSession, aWindowGroupId );
    aWindowGroupDetails.iUID = wgName->AppUid();
    aWindowGroupDetails.iIsBusy = wgName->IsBusy();
    aWindowGroupDetails.iIsSystem = wgName->IsSystem();
    aWindowGroupDetails.iIsHidden = wgName->Hidden();
    aWindowGroupDetails.iCaption = wgName->Caption();
    aWindowGroupDetails.iDocName = wgName->DocName();
    CleanupStack::PopAndDestroy( wgName );
    }
        

EXPORT_C void CMemSpyEngineHelperWindowServerImpl::SwitchToL( TInt aWindowGroupId )
    {
    TApaTask task( iWsSession );
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( iWsSession, aWindowGroupId );
    task.SetWgId( aWindowGroupId );
    if ( wgName->Hidden() )
        {
        User::Leave( KErrGeneral );
        }
    if ( !task.Exists() )
        {
        User::Leave( KErrNotFound );
        }
    task.BringToForeground();                     
    CleanupStack::PopAndDestroy( wgName );       
    }






CMemSpyEngineWindowGroupListImpl::CMemSpyEngineWindowGroupListImpl()
    {
    }


CMemSpyEngineWindowGroupListImpl::~CMemSpyEngineWindowGroupListImpl()
    {
    iItems.Close();
    }


void CMemSpyEngineWindowGroupListImpl::ConstructL()
    {
    }


CMemSpyEngineWindowGroupListImpl* CMemSpyEngineWindowGroupListImpl::NewLC()
    {
    CMemSpyEngineWindowGroupListImpl* self = new(ELeave) CMemSpyEngineWindowGroupListImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineWindowGroupListImpl::Count() const
    {
    return iItems.Count();
    }


EXPORT_C const TMemSpyEngineWindowGroupBasicInfo& CMemSpyEngineWindowGroupListImpl::At( TInt aIndex ) const
    {
    return iItems[ aIndex ];
    }


EXPORT_C TInt CMemSpyEngineWindowGroupListImpl::ItemIndex( const TMemSpyEngineWindowGroupBasicInfo& /*aEntry*/ ) const
    {
    TInt ret = KErrNotFound;
    return ret;
    }


void CMemSpyEngineWindowGroupListImpl::AddItemL( TMemSpyEngineWindowGroupBasicInfo& aItem )
    {
    iItems.AppendL( aItem );
    }


EXPORT_C TInt CMemSpyEngineWindowGroupListImpl::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineWindowGroupListImpl::MdcaPoint( TInt aIndex ) const
    {
    return TPtrC( At( aIndex ).iFullName );
    }


