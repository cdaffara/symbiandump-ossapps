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
* Description:  The cache stores a single array of this type. 
*
*/


#include <s32strm.h>
#include <mpresencebuddyinfo2.h>

#include "presencecacheservicestore.h"
#include "presencecachebuddystore.h"
#include "prestrace.h"
#include "presenceobjecthelpers.h"

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CPresenceCacheServiceStore* CPresenceCacheServiceStore::NewLC()
    {
    CPresenceCacheServiceStore* self = CPresenceCacheServiceStore::NewL();
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CPresenceCacheServiceStore* CPresenceCacheServiceStore::NewL()
    {
    CPresenceCacheServiceStore* self = new( ELeave ) CPresenceCacheServiceStore;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::~CPresenceCacheServiceStore()
// ---------------------------------------------------------------------------
//
CPresenceCacheServiceStore::~CPresenceCacheServiceStore()
    {
    TInt count = iBuddyStoreCollection.Count();
    for (TInt i=0;i<count;i++)
        {
        delete iBuddyStoreCollection[i];
        }
    iBuddyStoreCollection.Close();
    delete iServiceName;
    iServiceName = NULL;        
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::CPresenceCacheServiceStore()
// ---------------------------------------------------------------------------
//
CPresenceCacheServiceStore::CPresenceCacheServiceStore(): iServiceName(NULL)
    {
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheServiceStore::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::SetServiceNameL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheServiceStore::SetServiceNameL(TDesC& aServiceName)
    {
    delete iServiceName;
    iServiceName = NULL;
    iServiceName = aServiceName.AllocL();
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::GetObjectCollection()
// ---------------------------------------------------------------------------
//
RPointerArray<CPresenceCacheBuddyStore>& CPresenceCacheServiceStore::GetObjectCollection()
    {
    return iBuddyStoreCollection;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::ServiceName()
// ---------------------------------------------------------------------------
//
TPtrC CPresenceCacheServiceStore::ServiceName() const
    {
    return *iServiceName;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::Count()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheServiceStore::Count() const
    {
    return iBuddyStoreCollection.Count();
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::PresenceCount()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheServiceStore::PresenceCount() const
    {
    return iPresenceCount;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::AddBlind()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheServiceStore::AddBlind(CPresenceCacheBuddyStore* aBuddyStore)
    {
    TRACE( _L("CPresenceCacheServiceStore::AddBlind - begin") );
    // TRACE_1( _L("______identity: %S"), &aBuddyStore->BuddyId() );
    // TRACE_1( _L("______iService: %S"), &iServiceName->Des());
    if (!aBuddyStore)
        return KErrArgument;
    
    iBuddyStoreCollection.Append(aBuddyStore);
    
    // now update the presence count
    if (aBuddyStore->HasPresence())
        {
        iPresenceCount++;        
        }
        
    TRACE( _L("CPresenceCacheServiceStore::AddBlind - return"));
    return KErrNone;
    }
    
    
// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::AddOrReplacePresenceL()
// ---------------------------------------------------------------------------
//
CPresenceCacheBuddyStore* CPresenceCacheServiceStore::AddOrReplacePresenceL
                                            (MPresenceBuddyInfo2* aBuddyPresInfo)
    {
    TRACE( _L("CPresenceCacheServiceStore::AddOrReplacePresence - begin") );
    // TRACE_1( _L("______identity: %S"), &(aBuddyPresInfo->BuddyId() ));
    // TRACE_1( _L("______iService: %d"), &(iServiceName->Des()));
    
    CPresenceCacheBuddyStore* newBuddyStore(NULL);
    if(!aBuddyPresInfo)
        return newBuddyStore;
    
    TBool oldPresExists(EFalse);
    
    TInt count = iBuddyStoreCollection.Count();
    for (TInt i=0;i<count;i++)
        {
        if( (iBuddyStoreCollection[i])->EqualsIdentity(aBuddyPresInfo) )
            {
            oldPresExists = (iBuddyStoreCollection[i])->HasPresence();
            iBuddyStoreCollection[i]->SetPresenceBuddyInfo(aBuddyPresInfo);
            newBuddyStore = iBuddyStoreCollection[i];
            TRACE( _L("_________Match found") );
            break;
            }
        }
    
    //if not edited, we need to add a new buddy store    
    if (!newBuddyStore)
        {        
        newBuddyStore = CPresenceCacheBuddyStore::NewLC(this, aBuddyPresInfo->BuddyId());
        newBuddyStore->SetPresenceBuddyInfo(aBuddyPresInfo);
        iBuddyStoreCollection.Append(newBuddyStore);
        CleanupStack::Pop(newBuddyStore); //newBuddyStore
        }
        
    // new presence is certainly added, check whether it was existed
    if (!oldPresExists)
        {
        iPresenceCount++;        
        }

    TRACE( _L("CPresenceCacheServiceStore::AddOrReplacePresence - return"));
    return newBuddyStore;
    }


// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::RemoveBlind()
// ---------------------------------------------------------------------------
//
void CPresenceCacheServiceStore::RemoveBlind(CPresenceCacheBuddyStore* aPresenceBuddyInfo)
    {
    TInt index = iBuddyStoreCollection.Find(aPresenceBuddyInfo);
    if (index>-1)
        {
        delete iBuddyStoreCollection[index];
        iBuddyStoreCollection.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::FindAndRemove()
// ---------------------------------------------------------------------------
//
CPresenceCacheBuddyStore* CPresenceCacheServiceStore::FindAndRemove
                                    (const TDesC& aIdentity, TInt& aErr)
    {
    TRACE( _L("CPresenceCacheServiceStore::FindAndRemove - begin") );
    // TRACE_1( _L("______iService: %S"), &(iServiceName->Des()));
    // TRACE_1( _L("______identity: %S"), &(aIdentity ));
    TBool oldPresExists(EFalse);
    CPresenceCacheBuddyStore* buddyStore(NULL);
    aErr = KErrNotFound;

    TInt count = iBuddyStoreCollection.Count();
    TRACE_1( _L("CPresenceCacheServiceStore::FindAndRemove - count = %d"),count );
    for (TInt i=0;i<count;i++)
        {
        if ( (iBuddyStoreCollection[i])->EqualsIdentity(aIdentity) )
            {
            oldPresExists = (iBuddyStoreCollection[i])->HasPresence();
            
            //only delete presence if there are subscribers
            if ((iBuddyStoreCollection[i])->HasSubscribers())
                {
                (iBuddyStoreCollection[i])->SetPresenceBuddyInfo(NULL);
                buddyStore = iBuddyStoreCollection[i];
                }
            else //if there are no subscribers remove the whole package
                {
                delete iBuddyStoreCollection[i];
                iBuddyStoreCollection.Remove(i);                
                }
            break;
            }
        }
    
    // update presence info counts
    if(oldPresExists)
        {
        iPresenceCount--;
        aErr = KErrNone;      
        }

    TRACE_1( _L("CPresenceCacheServiceStore::FindAndRemove - buddyStore=%d"),buddyStore );
    return buddyStore;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::RemoveExpiredBuddyInfos
// ---------------------------------------------------------------------------
//

void CPresenceCacheServiceStore::RemoveExpiredBuddyInfos( 
        RPointerArray<CPresenceCacheBuddyStore>& aBuddyStores )
    {
    TInt count = iBuddyStoreCollection.Count();
    for( TInt i = 0 ; i < count ; ++i )
        {
        CPresenceCacheBuddyStore* buddyStore = iBuddyStoreCollection[i];
        MPresenceBuddyInfo2* buddyInfo = buddyStore->PresenceBuddyInfo();

        if( buddyInfo &&
                PresenceCacheBuddyUtils::IsBuddyInfoExpired( *buddyInfo ) )
            {
            // remove buddy info
            buddyStore->SetPresenceBuddyInfo( NULL );
            --iPresenceCount;
            if( buddyStore->HasSubscribers() )
                {
                aBuddyStores.Append( buddyStore ); // ignore error
                }
            else
                {
                delete buddyStore;
                iBuddyStoreCollection.Remove( i );
                --i;
                --count;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::FindAndGet()
// ---------------------------------------------------------------------------
//
CPresenceCacheBuddyStore* CPresenceCacheServiceStore::FindAndGet(
                                     const TDesC& aIdentity) const
    {
    TRACE( _L("CPresenceCacheServiceStore::FindAndGet - begin") );
    // TRACE_1( _L("______iService: %S"), &(iServiceName->Des()));
    TRACE_1( _L("______identity: %S"), &(aIdentity ));
    CPresenceCacheBuddyStore* buddyStore(NULL);
    
    TInt count = iBuddyStoreCollection.Count();
    for (TInt i=0;i<count;i++)
        {
        if ( (iBuddyStoreCollection[i])->EqualsIdentity(aIdentity))
            {
            buddyStore = iBuddyStoreCollection[i];
            break;
            }
        }
    TRACE_1( _L("CPresenceCacheServiceStore::FindAndGet - buddyStore=%d"),buddyStore );    
    return buddyStore;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheServiceStore::RemoveAllPresences()
// ---------------------------------------------------------------------------
//
void CPresenceCacheServiceStore::RemoveAllPresences()
    {
    TRACE( _L("CPresenceCacheServiceStore::RemoveAllPresences - begin") );
    // TRACE_1( _L("______iService: %S"), &(iServiceName->Des()));
    
    TInt count = iBuddyStoreCollection.Count();
    TRACE_1( _L("______store collection count: %d"), count);
    for (TInt i=0;i<count;i++)
        {
        (iBuddyStoreCollection[i])->RemovePresenceBuddyInfo();
        //if it doesnt have subscribers delete this whole object
        if(!(iBuddyStoreCollection[i]->HasSubscribers()))
            {
            delete iBuddyStoreCollection[i];
            iBuddyStoreCollection.Remove(i);
            count--;
            i--;
            }
        }
    iPresenceCount = 0;
    TRACE(_L("CPresenceCacheServiceStore::RemoveAllPresences - end"));    
    }

// eof
