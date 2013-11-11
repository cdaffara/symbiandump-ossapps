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

// INCLUDE FILES
#include    "presencecacheexpiry.h"
#include    "presencecachebuddystore.h"
#include    "presencecacheserver.h"
#include    "presencecachesession.h"
#include    "mpresencebuddyinfo2.h"
#include    "presencecacheservicestore.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPresenceCacheExpiry::CPresenceCacheExpiry
// -----------------------------------------------------------------------------
//
inline CPresenceCacheExpiry::CPresenceCacheExpiry(
        CPresenceCacheServer& aServer )
: iAsyncCallBack( TCallBack( CPresenceCacheExpiry::ExpiryCheck, this ),
        CActive::EPriorityIdle),
  iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CPresenceCacheExpiry::NewL
// -----------------------------------------------------------------------------
//
CPresenceCacheExpiry* CPresenceCacheExpiry::NewL(
        CPresenceCacheServer& aServer )
    {
    return new( ELeave ) CPresenceCacheExpiry(aServer);
    }


// Destructor
CPresenceCacheExpiry::~CPresenceCacheExpiry()
    {
    iAsyncCallBack.Cancel();
    }

// -----------------------------------------------------------------------------
// CPresenceCacheExpiry::ExpiryCheck
// -----------------------------------------------------------------------------
//
TInt CPresenceCacheExpiry::ExpiryCheck(TAny *aPtr)
    {
    CPresenceCacheExpiry* self = static_cast<CPresenceCacheExpiry*>(aPtr);
    TRAP_IGNORE(self->DoExpiryCheckL());
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPresenceCacheExpiry::NotifySubscribersL
// -----------------------------------------------------------------------------
//
void CPresenceCacheExpiry::NotifySubscribersL( 
        CPresenceCacheBuddyStore& aBuddyStore )
    {
    RPointerArray<CPresenceCacheSession> sessions = 
            aBuddyStore.GetSubscribedSessions();
    const TInt sessionCount( sessions.Count() );
    MPresenceBuddyInfo2* buddyInfoTemp =  MPresenceBuddyInfo2::NewLC();
    buddyInfoTemp->SetIdentityL( aBuddyStore.BuddyId() );
    
    for( TInt i = 0 ; i < sessionCount ; ++i )
        {
        CPresenceCacheSession* session = sessions[i];
        if (iServer.SessionExists(session))
            {
            session->NotifyPresenceChangeL( buddyInfoTemp );
            }
        }
    CleanupStack::PopAndDestroy(); // buddyInfoTemp
    }

// -----------------------------------------------------------------------------
// CPresenceCacheExpiry::DoExpiryCheckL
// -----------------------------------------------------------------------------
//
void CPresenceCacheExpiry::DoExpiryCheckL()
    {
    RPointerArray<CPresenceCacheServiceStore>& presenceCache = 
            iServer.iPresenceCache;
    
    RPointerArray<CPresenceCacheBuddyStore> buddyStores;
    
    const TInt serviceCount(presenceCache.Count());
    for( TInt i = 0 ; i < serviceCount ; ++i )
        {
        presenceCache[i]->RemoveExpiredBuddyInfos( buddyStores );
        }
    
    const TInt buddyStoreCount( buddyStores.Count() );
    for( TInt i = 0 ; i < buddyStoreCount ; ++i )
        {
        NotifySubscribersL( *( buddyStores[i] ) );
        }
    }

//  End of File
