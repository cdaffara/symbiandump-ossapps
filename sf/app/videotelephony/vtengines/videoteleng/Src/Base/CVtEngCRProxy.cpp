/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Central repository proxy.
*
*/



// INCLUDE FILES
#include    "CVtEngCRObserverItem.h"
#include    "CVtEngCRProxy.h"
#include    "MVtEngCRSettingObserver.h"
#include    <cvtlogger.h>

// CONSTANTS

const TInt KVtEngCRProxyObsGranularity = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngCRProxy::CVtEngCRProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngCRProxy::CVtEngCRProxy() : iObserverItems( KVtEngCRProxyObsGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngCRProxy* CVtEngCRProxy::NewL( )
    {
    __VTPRINTENTER( "CRProxy.NewL" )
    CVtEngCRProxy* self = new( ELeave ) CVtEngCRProxy;
    __VTPRINTEXIT( "CRProxy.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::~CVtEngCRProxy
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngCRProxy::~CVtEngCRProxy()
    {
    __VTPRINTENTER( "CRProxy.~" )
    RemoveAllObservers();
    RemoveSessions();
    __VTPRINTEXIT( "CRProxy.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::FindItem
// Finds observer item.
// -----------------------------------------------------------------------------
//
CVtEngCRObserverItem* CVtEngCRProxy::FindItem( 
    const TUid& aUid, 
    const TUint32 aKey,
    TInt& aIndex ) const
    {
    TInt count( iObserverItems.Count() );
    CVtEngCRObserverItem* item = NULL;
    TBool found( EFalse );
    while ( count-- && !found )
        {
        item = iObserverItems[ count ];
        if ( item->Offer( aUid, aKey ) )
            {
            aIndex = count;
            found = ETrue;
            }
        }

    // Just NULL the item if not found
    if ( !found )
        {
        item = NULL;
        }
    return item;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::FindItem
// Finds observer item.
// -----------------------------------------------------------------------------
//
CVtEngCRObserverItem* CVtEngCRProxy::FindItem( 
    const TUint32 aKey, TInt& aIndex ) const
    {
    TInt count( iObserverItems.Count() );
    CVtEngCRObserverItem* item = NULL;
    if ( aIndex < 0 )
        {
        return NULL;
        }

    for ( ; aIndex < count; aIndex++ )
        {
        item = iObserverItems[ aIndex ];
        if ( item->Offer( aKey  ) )
            {
            break;
            }
        }
    return item;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::RemoveInterested
// Removes a cent rep listener.
// -----------------------------------------------------------------------------
//
void CVtEngCRProxy::RemoveInterested( CVtEngCRObserverItem& aObserver )
    {
    __VTPRINTENTER( "CRProxy.RemoveInterested" )
    TInt index ( 0 );
    CVtEngCRObserverItem* item = FindItem( aObserver.Uid(), 
        aObserver.Key(), index );
    if ( !item )
        {
        // Should not be possible.
        __VTPRINTEXITR( "CRProxy.RemoveInterested %d", 0 )
        return;
        }

    delete item;
    iObserverItems.Remove( index );
    __VTPRINTEXITR( "CRProxy.RemoveInterested %d", 1 )
    }
    
// -----------------------------------------------------------------------------
// CVtEngCRProxy::NewCRSessionL
// New Central repository-uid pairing.
// -----------------------------------------------------------------------------
//
CVtEngCRProxy::CCRSession* CVtEngCRProxy::NewCRSessionL( TUid aUid )
    {
    __VTPRINTENTER( "CRProxy.NewCRSessionL" )
    CRepository* crSession = CRepository::NewL( aUid );
    CleanupStack::PushL( crSession );
    CCRSession* csSession = new ( ELeave ) CCRSession;
    csSession->iSession = crSession;
    csSession->iUid = aUid;    
    CleanupStack::Pop( crSession );
    __VTPRINTEXIT( "CRProxy.NewCRSessionL" )
    return csSession;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::RegisterInterested
// New Central repository listener.
// -----------------------------------------------------------------------------
//
void CVtEngCRProxy::RegisterInterestedL( CVtEngCRObserverItem* aObserver )
    {
    __VTPRINTENTER( "CRProxy.RegisterInterestedL" )
    CRepository* crSession = MapUidToCR( aObserver->Uid() );
    CCRSession* csSession = NULL;
    if( !crSession )
        {
        csSession = NewCRSessionL( aObserver->Uid() );
        CleanupStack::PushL( csSession );
        crSession = csSession->iSession;
        iCRSessions.AppendL( csSession );
        CleanupStack::Pop( csSession );
        }

    CCenRepNotifyHandler* notifier = 
        CCenRepNotifyHandler::NewL( *aObserver, *crSession, 
        aObserver->Type(), aObserver->Key() );
    //notifier ownership transfered
    aObserver->SetCenRepNotifier( notifier );
    notifier->StartListeningL();
    iObserverItems.AppendL ( aObserver );
    __VTPRINTEXIT( "CRProxy.RegisterInterestedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::MapUidToCR
// Maps a given UID to a Central repository session.
// -----------------------------------------------------------------------------
//
CRepository* CVtEngCRProxy::MapUidToCR( const TUid aUid )
    {
    __VTPRINTENTER( "CRProxy.Map" )
    CCRSession* csSession = NULL;
    TBool found( EFalse );
    TInt count( iCRSessions.Count() );
    for ( TInt i = 0 ; i < count; i++ )
        {
        csSession = iCRSessions[ i ];
        if ( csSession->iUid == aUid )
            {
            i = count; // break loop
            found = ETrue;
            }
        }
    CRepository* session = NULL;        
    // Only return something when found.
    if ( found )
        {
        session = csSession->iSession;
        }
    __VTPRINTEXITR( "CRProxy.Map %d", (TInt) session )
    return session;
    }
  
// -----------------------------------------------------------------------------
// CVtEngCRProxy::RemoveAllObservers
// All observers off they go.
// -----------------------------------------------------------------------------
//    
void CVtEngCRProxy::RemoveAllObservers()
    {
    __VTPRINTENTER( "CRProxy.RemoveAll" )
    iObserverItems.ResetAndDestroy();
    iObserverItems.Close();
    __VTPRINTEXIT( "CRProxy.RemoveAll" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::RemoveSessions
// Removes all Central repository sessions.
// -----------------------------------------------------------------------------
//
void CVtEngCRProxy::RemoveSessions()
    {
    __VTPRINTENTER( "CRProxy.RemoveAllS" )
    iCRSessions.ResetAndDestroy();
    iCRSessions.Close();
    __VTPRINTEXIT( "CRProxy.RemoveAllS" )
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::GetSession
// Gets an existing or a new Central repository session for a UID.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::GetSessionL( const TUid aUid, CRepository*& crSession )
    {
    TInt err( KErrNone );
    crSession = MapUidToCR( aUid );
    if ( !crSession )
        {
        CCRSession* session = NULL;
        session = NewCRSessionL( aUid );
        if ( session )
            {
            CleanupStack::PushL( session );
            iCRSessions.AppendL( session );
            CleanupStack::Pop( session );
            crSession = session->iSession;
            }
        else 
            {
            err = KErrNotFound;
            }
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CVtEngCRProxy::Get
// Gets integer value.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Get( const TUid aUid, const TUint32 aId, TInt& aVal )
    {
    __VTPRINTENTER( "CRProxy.GetInt" )       
    __VTPRINT3( DEBUG_GEN, "UID=%d,key=%d", aUid.iUid, (TInt) aId );

    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Get( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.GetInt %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Get
// Gets real value.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Get( const TUid aUid, const TUint32 aId, TReal& aVal )
    {
    __VTPRINTENTER( "CRProxy.GetReal" )
    __VTPRINT3( DEBUG_GEN, "UID=%d,key=%d", aUid.iUid, (TInt) aId );
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Get( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.GetR %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Get
// Gets binary value.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Get( const TUid aUid, const TUint32 aId, TDes8& aVal )
    {
    __VTPRINTENTER( "CRProxy.GetD8" )
    __VTPRINT3( DEBUG_GEN, "UID=%d,key=%d", aUid.iUid, (TInt) aId );
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Get( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.GetD8 %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Get
// Gets string value.
// -----------------------------------------------------------------------------
//      
TInt CVtEngCRProxy::Get( const TUid aUid, const TUint32 aId, TDes16& aVal )
    {
    __VTPRINTENTER( "CRProxy.GetD16" )
    __VTPRINT3( DEBUG_GEN, "UID=%d,key=%d", aUid.iUid, (TInt) aId );
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Get( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.GetD16 %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Set
// Sets integer value to Central Repository.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Set( const TUid aUid, const TUint32 aId, TInt aVal )
    {
    __VTPRINTENTER( "CRProxy.SetInt" )
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Set( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.SetInt %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Set
// Sets real value to Central Repository.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Set( const TUid aUid, const TUint32 aId, const TReal& aVal )
    {
    __VTPRINTENTER( "CRProxy.SetR" )
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Set( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.SetR %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Set
// Sets binary value to Central Repository.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Set( const TUid aUid, const TUint32 aId, const TDesC8& aVal )
    {
    __VTPRINTENTER( "CRProxy.SetD8" )
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Set( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.SetD8 %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::Set
// Sets string value to Central Repository.
// -----------------------------------------------------------------------------
//
TInt CVtEngCRProxy::Set( 
    const TUid aUid, const TUint32 aId, const TDesC16& aVal )
    {
    __VTPRINTENTER( "CRProxy.SetD16" )
    CRepository* crSession = NULL;
    TRAPD( err, GetSessionL( aUid, crSession ) );
    if ( err == KErrNone )
        {
        err = crSession->Set( aId, aVal );
        }
    __VTPRINTEXITR( "CRProxy.SetD16 %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCRProxy::CCRSession::~CCRSession
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngCRProxy::CCRSession::~CCRSession()
    {
    __VTPRINTENTER( "CRProxy.~" )
    delete iSession;
    __VTPRINTEXIT( "CRProxy.~" )
    }

// end of file
