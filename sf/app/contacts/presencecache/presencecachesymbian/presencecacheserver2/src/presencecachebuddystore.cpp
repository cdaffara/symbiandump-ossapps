/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Store object for single buddy in a service
*
*/


#include <mpresencebuddyinfo2.h>

#include "presencecachebuddystore.h"
#include "presencecacheservicestore.h"
#include "presencecachesession.h"

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CPresenceCacheBuddyStore* CPresenceCacheBuddyStore::NewLC(
                                    CPresenceCacheServiceStore* aServiceStore,
                                    const TDesC& aIdentity)
    {
    CPresenceCacheBuddyStore* self =CPresenceCacheBuddyStore::NewL(aServiceStore,aIdentity);
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CPresenceCacheBuddyStore* CPresenceCacheBuddyStore::NewL(
                                    CPresenceCacheServiceStore* aServiceStore,
                                    const TDesC& aIdentity)
    {
    CPresenceCacheBuddyStore* self = new( ELeave ) CPresenceCacheBuddyStore(
                                               aServiceStore);
    CleanupStack::PushL( self );
    self->ConstructL(aIdentity);
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::~CPresenceCacheBuddyStore()
// ---------------------------------------------------------------------------
//
CPresenceCacheBuddyStore::~CPresenceCacheBuddyStore()
    {
    if (iPresenceBuddyInfo)
        delete iPresenceBuddyInfo;
    if(iIdentity)
        delete iIdentity;
    iSubscribedSessions.Close();
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::CPresenceCacheBuddyStore()
// ---------------------------------------------------------------------------
//
CPresenceCacheBuddyStore::CPresenceCacheBuddyStore(
          CPresenceCacheServiceStore* aServiceStore):iPresenceBuddyInfo(NULL),
                                                    iServiceStore(aServiceStore)
    {
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::ConstructL(const TDesC& aIdentity)
    {
    SetBuddyIdL(aIdentity);
    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::SetPresenceBuddyInfo()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::SetPresenceBuddyInfo(MPresenceBuddyInfo2* aPresBuddyInfo)
    {
    delete iPresenceBuddyInfo;
    iPresenceBuddyInfo = aPresBuddyInfo;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::RemovePresenceBuddyInfo()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::RemovePresenceBuddyInfo()
    {
    if (iPresenceBuddyInfo)
        delete iPresenceBuddyInfo;
    iPresenceBuddyInfo = NULL;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::GetSubscribedSessions()
// ---------------------------------------------------------------------------
//
RPointerArray<CPresenceCacheSession>& CPresenceCacheBuddyStore::GetSubscribedSessions()
    {
    return iSubscribedSessions;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::AddSubscribedSession()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::AddSubscribedSession(CPresenceCacheSession* aSession)
    {
    if(iSubscribedSessions.Find(aSession)<0)
        iSubscribedSessions.Append(aSession);
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::RemoveSubscribedSession()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::RemoveSubscribedSession(CPresenceCacheSession* aSession)
    {
    TInt index = iSubscribedSessions.Find(aSession);
    if(index>-1)
        iSubscribedSessions.Remove(index);
    
    // delete this object if it doesnt have any presence or subscribers
    if(!(iPresenceBuddyInfo || HasSubscribers()))
        {
        if(iServiceStore)
            {
            iServiceStore->RemoveBlind(this);
            }
        }
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::SetBuddyId()
// ---------------------------------------------------------------------------
//
void CPresenceCacheBuddyStore::SetBuddyIdL(const TDesC& aIdentity)
    {
    delete iIdentity;
    iIdentity = NULL;
    iIdentity = aIdentity.AllocL();
    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::ServiceName()
// ---------------------------------------------------------------------------
//
TPtrC CPresenceCacheBuddyStore::ServiceName() const
    {
    _LIT(KColon, ":");
    TInt pos = 0;
    if ( iIdentity )
        {
        pos = iIdentity->Des().Find(KColon);
        }
    TPtrC serviceName;
    if(pos>0) // if colon found and there is something before colon, i.e. xsp id
        {
        serviceName.Set( iIdentity->Des().Left(pos) );
        }
    else
        serviceName.Set(TPtrC());
    return serviceName;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::PresenceBuddyInfo()
// ---------------------------------------------------------------------------
//
MPresenceBuddyInfo2* CPresenceCacheBuddyStore::PresenceBuddyInfo() const
    {
    return iPresenceBuddyInfo;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::BuddyId()
// ---------------------------------------------------------------------------
//
TPtrC  CPresenceCacheBuddyStore::BuddyId() const
    {
    return iIdentity ? iIdentity->Des() : TPtrC();
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::EqualsIdentity()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheBuddyStore::EqualsIdentity(
                    const CPresenceCacheBuddyStore* aOtherInstance ) const
    {
    TBool ret(EFalse);
    if(aOtherInstance)
        {
        ret = 0 == BuddyId().CompareF( aOtherInstance->BuddyId() ); 
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::EqualsIdentity()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheBuddyStore::EqualsIdentity(
                    const MPresenceBuddyInfo2* aOtherInstance ) const
    {
    TBool ret(EFalse);
    if(aOtherInstance)
        {
        ret = 0 == BuddyId().CompareF( aOtherInstance->BuddyId() );  
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::EqualsIdentity()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheBuddyStore::EqualsIdentity(
                    const TDesC& aOtherIdentity ) const
    {
    return 0 == BuddyId().CompareF( aOtherIdentity );         
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::HasPresence()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheBuddyStore::HasPresence() const
    {
    if (iPresenceBuddyInfo)
        return ETrue;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheBuddyStore::HasSubscribers()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheBuddyStore::HasSubscribers() const
    {
    if (iSubscribedSessions.Count())
        return ETrue;
    return EFalse;
    }
    
