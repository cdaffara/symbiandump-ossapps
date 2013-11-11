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
* Description:  Impementation for Presence Cache Server session
*
*/


// INCLUDE FILES

#include <mpresencebuddyinfo2.h>

#include "presenceobjecthelpers.h"
#include "presencecachesession.h"
#include "presencecacheserver.h"
#include "prestrace.h"
#include "cacheobjecthelpers.h"
#include "presencecacheservicestore.h"
#include "presencecachebuddystore.h"
#include "presencecacheevent.h"

//Include Cache server namespace
using namespace NCacheSrv2;


// ---------------------------------------------------------------------------
// CPresenceCacheSession::NewL()
// ---------------------------------------------------------------------------
//
CPresenceCacheSession* CPresenceCacheSession::NewL( CPresenceCacheServer& aServer )
    {
    CPresenceCacheSession* self = CPresenceCacheSession::NewLC( aServer );
    CleanupStack::Pop(self);
    return self;    
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::NewLC()
// ---------------------------------------------------------------------------
//
CPresenceCacheSession* CPresenceCacheSession::NewLC( CPresenceCacheServer& aServer )
    {
    CPresenceCacheSession* self = new ( ELeave ) CPresenceCacheSession( aServer );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::~CPresenceCacheSession()
// ---------------------------------------------------------------------------
//
CPresenceCacheSession::~CPresenceCacheSession()
    {
    if(iDataPack)
        delete iDataPack;
    if(iDataPackAsync)
        delete iDataPackAsync;
    iBuddypresInfoList.ResetAndDestroy();
    iBuddypresInfoList.Close();
        
    if(iActiveHelper)
        delete iActiveHelper;
    iServer.DecrementSessions();
    RemoveMySubscriptions(); // remove all subscriptions subscribed by this session
    iSubscribedStores.Close();
    
    EmptyNotificationQueue();
    
    iNotifyEvents.Close();
    
    TRACE_1( _L("CPresenceCacheSession[%d]::~CPresenceCacheSession()"), this );
    }


// ---------------------------------------------------------------------------
// CPresenceCacheSession::ServiceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case NRequest::EPrepReadPresenceInfoSync:
            HandlePrepReadPresenceInfoSyncL(aMessage);
            break;
            
        case NRequest::EGetLastPreparedPacket:
            HandleGetLastPreparedPacketL(aMessage);
            break;
            
        case NRequest::EGetLastAsyncPreparedPacket:
            HandleGetLastPreparedAsyncPacketL(aMessage);
            break;
            
        case NRequest::EBuddyCountInAllServices:
            HandleBuddyCountInAllServicesL(aMessage);
            break;
            
        case NRequest::EBuddyCountInService:
            HandleBuddyCountInServiceL(aMessage);
            break;
            
        case NRequest::EGetServiceCount:
            HandleGetServiceCountL(aMessage);
            break;
            
        case NRequest::EDeleteService:
            HandleDeleteServiceL(aMessage);
            break;
            
        case NRequest::EDeletePresence:
            HandleDeletePresenceL(aMessage);
            break;
            
        case NRequest::ECancelAsyncOperation:
            HandleCancel(aMessage);
            break;

        case NRequest::EWritePresenceInfoSync:
            HandleWritePresenceInfoSyncL(aMessage);
            break;

        case NRequest::EWritePresenceInfoAsync:
            HandleWritePresenceInfoAsyncL(aMessage);
            break;
            
        case NRequest::EPrepReadAllBuddiesPresenceInService:
            HandlePrepReadAllBuddiesPresenceInService(aMessage);
            break;
            
        case NRequest::ESubscribeBuddyPresenceChange:// 0:aService.iUid, 1:myIdPack
            HandleSubscribeBuddyPresenceChangeL(aMessage);
            break;
        
        case NRequest::EUnSubscribeBuddyPresenceChange: // 0:aService.iUid, 1:myIdPack
            HandleUnSubscribeBuddyPresenceChangeL(aMessage);
            break;
        
        case NRequest::EGetLastNotifiedtPacket: // 0:&ptrBuf
            HandleGetLastNotifiedtPacketL(aMessage);
            break;
        
        case NRequest::EWaitingForNotification: // 0:&sizePckg
            HandleWaitingForNotificationL(aMessage);
            break;
        
        case NRequest::ECancelWaitingForNotification:
            HandleCancelWaitingForNotification(aMessage);
            break;

        default:
        	TRACE( _L("CPresenceCacheSession::ServiceL - default"));
            aMessage.Complete(KErrArgument);
            break;
        }
    iServer.StartExpiryCheck();
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandlePrepReadPresenceInfoSyncL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandlePrepReadPresenceInfoSyncL( const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandlePrepReadPresenceInfoSyncL - begin"));

    TInt sizeOfReturnPacket(NULL);
 
    // unpacking identity
    const TInt pckSize = aMessage.GetDesLengthL(1);
    HBufC* idPack = HBufC::NewLC( pckSize );
    TPtr paramPckPtr = idPack->Des();    
    aMessage.ReadL( 1, paramPckPtr );
    
    TPtrC serviceName = GetServiceName( paramPckPtr );  
    TPtrC identity( paramPckPtr ); 
    
    // now processing
    TInt serviceCount = iServer.iPresenceCache.Count();
    MPresenceBuddyInfo2* buddyPresenceInfo(NULL);
    CPresenceCacheBuddyStore* buddyStore(NULL);
    
    TRACE_1( _L("_______serviceCount[%d]"), serviceCount);
    
    for(TInt i=0; i<serviceCount; i++)
        {
        if (!(serviceName.CompareF( (iServer.iPresenceCache[i])->ServiceName())))
            {
            buddyStore = (iServer.iPresenceCache[i])->FindAndGet(identity);
            if(buddyStore)
                buddyPresenceInfo = buddyStore->PresenceBuddyInfo();
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(idPack);
    
    TInt err = KErrNotFound;
    
    TRACE_1( _L("_______buddyPresenceInfo[%d]"), buddyPresenceInfo);
    if (buddyPresenceInfo) // if found
        {
        if(iDataPack)
            delete iDataPack; iDataPack=NULL;
        iDataPack = PackPresenceInfoLC(*buddyPresenceInfo);
        CleanupStack::Pop(iDataPack);
        sizeOfReturnPacket = (iDataPack->Des()).Size();
        err = KErrNone;
        }
    
    TPckgBuf<TInt> p(sizeOfReturnPacket);
    aMessage.WriteL(2,p);            
    aMessage.Complete(err);
    TRACE( _L("CPresenceCacheSession::HandlePrepReadPresenceInfoSyncL - end"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleGetLastPreparedPacketL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleGetLastPreparedPacketL( const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleGetLastPreparedPacketL() - begin"));
    if(iDataPack)
        aMessage.WriteL(0, *(iDataPack));
    aMessage.Complete(KErrNone);   
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleGetLastPreparedAsyncPacketL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleGetLastPreparedAsyncPacketL( const RMessage2& aMessage )
    {
    if(iDataPackAsync)
        aMessage.WriteL(0, *(iDataPackAsync));
    aMessage.Complete(KErrNone);   
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleWritePresenceInfoSyncL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleWritePresenceInfoSyncL(const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleWritePresenceInfoSyncL - begin"));
    TInt err(KErrNone);
    const TInt pckSize = aMessage.GetDesLengthL(0);
    if(iDataPack)
        delete iDataPack; iDataPack=NULL;
    iDataPack = HBufC8::NewL( pckSize );
    TPtr8 paramPckPtr = iDataPack->Des();
    aMessage.ReadL( 0, paramPckPtr );
    
    MPresenceBuddyInfo2* buddypresenceInfo = MPresenceBuddyInfo2::NewLC();          
    TCacheObjectPacker< MPresenceBuddyInfo2 >::UnPackL(*buddypresenceInfo, *iDataPack);        
    err = TryWriteBuddyToCacheL(buddypresenceInfo);
    
    if(err==KErrNone || err == KErrNoMemory)  //ownership  transferred
        {
        CleanupStack::Pop();       //buddypresenceInfo
        }
    else       //KErrGeneral, ownership not transferred
        {
        CleanupStack::PopAndDestroy();
        }
    aMessage.Complete(err);
    TRACE( _L("CPresenceCacheSession::HandleWritePresenceInfoSyncL - end"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandlePrepReadAllBuddiesPresenceInService()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandlePrepReadAllBuddiesPresenceInService
                                                        (const RMessage2& aMessage )
    {
    if(iActiveHelper->IsActive())
        aMessage.Complete(KErrServerBusy);
    else
        {
        iMessage = aMessage;
        
        iAsyncReq = NRequest::EPrepReadAllBuddiesPresenceInService;
        iActiveHelper->Start();
        }
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::ReadAllBuddiesPresenceInServiceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::ReadAllBuddiesPresenceInServiceL()
    {
    const TInt pckSize = iMessage.GetDesLengthL(0);
    HBufC* serviceName = HBufC16::NewLC( pckSize );
    TPtr16 paramPckPtr = serviceName->Des();
    iMessage.ReadL( 0, paramPckPtr );
    
    TInt serviceCount = iServer.iPresenceCache.Count();
    CPresenceCacheServiceStore* serviceStore(NULL);
    CPresenceCacheBuddyStore* buddyStore(NULL);
    MPresenceBuddyInfo2* presBuddyInfo(NULL);
    TInt presCount(0);
    TInt count(0);
    
    RPointerArray<MPresenceBuddyInfo2> tempArray;
    CleanupClosePushL( tempArray );
        
    for (TInt i=0; i<serviceCount; i++)
        {
        serviceStore = iServer.iPresenceCache[i];
        if (!(serviceName->Des().CompareF( serviceStore->ServiceName())))
            {
            presCount = serviceStore->PresenceCount();
            // if it has presences
            if (presCount)
                {
                count = serviceStore->Count();
                for (TInt j=0; j<count; j++)
                    {
                    buddyStore = serviceStore->GetObjectCollection()[j];
                    presBuddyInfo = buddyStore->PresenceBuddyInfo();
                    if (presBuddyInfo)
                        {
                        tempArray.AppendL( presBuddyInfo );
                        }
                    }                  
                }
            break;
            }
        }
        
    TInt size(KErrNotFound); // this may also indicate error to client if negative
    if ( tempArray.Count() ) // if found
        {
        if(iDataPackAsync)
            delete iDataPackAsync; iDataPackAsync = NULL;
        iDataPackAsync = TPresenceArrayPacker::PackArrayL( tempArray );    
        size = (iDataPackAsync->Des()).Size();
        }
    
    CleanupStack::PopAndDestroy( ); // tempArray    
    CleanupStack::PopAndDestroy(serviceName);
    iAsyncReq = NRequest::ENoRequestMade;
    iMessage.Complete(size);    
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::TryWriteBuddyToCache()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheSession::TryWriteBuddyToCacheL(MPresenceBuddyInfo2* aBuddyPresInfo)
    {
    if (!aBuddyPresInfo)
        return KErrArgument;
    
    TPtrC serviceName = GetServiceName( aBuddyPresInfo->BuddyId() );
    TInt serviceCount = iServer.iPresenceCache.Count();
    CPresenceCacheBuddyStore* buddyStore(NULL);
    
    for (TInt i=0; i<serviceCount; i++)
        {
        CPresenceCacheServiceStore* serviceStore = iServer.iPresenceCache[i];
        if ( serviceName.CompareF( serviceStore->ServiceName() ) == 0)
            {
            buddyStore = serviceStore->AddOrReplacePresenceL(aBuddyPresInfo);
            break;
            }
        }
    if (!buddyStore) // if not written
        {
        CPresenceCacheServiceStore* newServiceStore = CPresenceCacheServiceStore::NewLC();
        newServiceStore->SetServiceNameL(serviceName);
        buddyStore = newServiceStore->AddOrReplacePresenceL(aBuddyPresInfo);
        (iServer.iPresenceCache).Append(newServiceStore); //ownership transferred
        CleanupStack::Pop(newServiceStore);
        }
        
    TInt err = KErrGeneral;
    if (buddyStore)
        {
        TRAP( err, NotifyAllSubscribersL(buddyStore) );
        if (err != KErrNone && err != KErrNoMemory)
            {
            User::LeaveIfError(err);
            }
        
        // reset expiry time stamp
        PresenceCacheBuddyUtils::ResetBuddyInfoTimeStamp( *aBuddyPresInfo );
        }
    return err;
    }
    

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleCancel()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleCancel(const RMessage2& aMessage )
    {
    if (iActiveHelper->IsActive())
        iActiveHelper->Cancel(); //see also iActiveHelper->DoCancel()
    aMessage.Complete(KErrNotFound);
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleBuddyCountInAllServicesL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleBuddyCountInAllServicesL(const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleBuddyCountInAllServices - begin"));
    TInt count(NULL);
    TInt serviceCount = iServer.iPresenceCache.Count();
    
    for(TInt i=0; i<serviceCount; i++)
        {
        count = (count) + ((iServer.iPresenceCache[i])->PresenceCount());
        }
    
    TPckgBuf<TInt> p(count);
    aMessage.WriteL(0,p);               
    aMessage.Complete(KErrNone);
    TRACE( _L("CPresenceCacheSession::HandleBuddyCountInAllServices - end"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleBuddyCountInServiceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleBuddyCountInServiceL(const RMessage2& aMessage )
    {
    TInt count(NULL);
    
    const TInt pckSize = aMessage.GetDesLengthL(1);
    HBufC* serviceName = HBufC16::NewLC( pckSize );
    TPtr16 paramPckPtr = serviceName->Des();
    aMessage.ReadL( 1, paramPckPtr );
    
    TInt serviceCount = iServer.iPresenceCache.Count();
    TInt err(KErrNotFound);
    
    for(TInt i=0; i<serviceCount; i++)
        {
        if (!(serviceName->Des().CompareF((iServer.iPresenceCache[i])->ServiceName())))
            {
            count = ((iServer.iPresenceCache[i])->PresenceCount());
            err = KErrNone;
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(serviceName);
    TPckgBuf<TInt> p(count);
    aMessage.WriteL(0,p);
    aMessage.Complete(err);    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleDeleteServiceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleDeleteServiceL(const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleDeleteServiceL - begin"));
    TInt err(KErrNotFound);
        
    const TInt pckSize = aMessage.GetDesLengthL(0);
    HBufC* serviceName = HBufC16::NewLC( pckSize );
    TPtr16 paramPckPtr = serviceName->Des();
    aMessage.ReadL( 0, paramPckPtr );
    
    TInt serviceCount = iServer.iPresenceCache.Count();
    TInt buddyCount(NULL);
    
    TRACE_1( _L("_______serviceCount[%d]"), serviceCount);
    for(TInt i=serviceCount-1; i >= 0; i--)
        {
        if (!(serviceName->Des().CompareF((iServer.iPresenceCache[i])->ServiceName())))
            {
            (iServer.iPresenceCache[i])->RemoveAllPresences();
            buddyCount = (iServer.iPresenceCache[i])->GetObjectCollection().Count();
            TRACE_1( _L("_______buddyCountAfter[%d]"), buddyCount);
            
            //if there are no buddies left in this service delete this whole service
            if (buddyCount == 0)
                {
                delete iServer.iPresenceCache[i];
                iServer.iPresenceCache.Remove(i);
                }
                
            //inform all subscribers
            for(TInt j=0; j < buddyCount; j++)
                {
                NotifyAllSubscribersL((iServer.iPresenceCache[i])->GetObjectCollection()[j]);
                }
            err = KErrNone;
            // break;
            }
        }
    CleanupStack::PopAndDestroy(serviceName);
    aMessage.Complete(err);
    TRACE( _L("CPresenceCacheSession::HandleDeleteServiceL - end"));
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleDeletePresenceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleDeletePresenceL(const RMessage2& aMessage )
    {
    const TInt pckSize = aMessage.GetDesLengthL(0);
    HBufC* idPack = HBufC::NewLC( pckSize );
    TPtr paramPckPtr = idPack->Des();
    aMessage.ReadL( 0, paramPckPtr );
    TInt err(KErrNotFound);
    TInt buddyCount(NULL);
        
    TPtrC serviceName = GetServiceName( paramPckPtr );  
    TPtrC identity( paramPckPtr );     
            
    TInt serviceCount = iServer.iPresenceCache.Count();
    
    CPresenceCacheBuddyStore* buddyStore(NULL);
    for(TInt i=0; i<serviceCount; i++)
        {
        if( !((iServer.iPresenceCache[i])->ServiceName()).CompareF(serviceName) )
            {
            buddyStore = (iServer.iPresenceCache[i])->FindAndRemove( identity,err);
            
            buddyCount = (iServer.iPresenceCache[i])->GetObjectCollection().Count();
            TRACE_1( _L("_______buddyCountAfter[%d]"), buddyCount);
            
            //if there are no buddies left in this service delete this whole service
            if(buddyCount==0)
                {
                delete iServer.iPresenceCache[i];
                iServer.iPresenceCache.Remove(i);
                }
            break;
            }
        }

    CleanupStack::PopAndDestroy(idPack);
    aMessage.Complete(err);
    
    if(buddyStore) // if subscribers
        NotifyAllSubscribersL(buddyStore);
    }
    

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleGetServiceCountL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleGetServiceCountL(const RMessage2& aMessage )
    {
    // iLastServiceCount is also used for mem aloc by client before calling
    // GetAllServices
    TInt serviceCount = iServer.iPresenceCache.Count();
    iLastServiceCount = 0;
    TRACE_1( _L("CPresenceCacheSession::HandleGetServiceCount realcount[%d]- end"),serviceCount);
    for(TInt i=0;i<serviceCount;i++)
        {
        if((iServer.iPresenceCache[i])->PresenceCount())
            iLastServiceCount++;
        }

    TPckgBuf<TInt> p(iLastServiceCount);
    aMessage.WriteL(0,p);
    aMessage.Complete(KErrNone);
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleGetAllServicesL()
// ---------------------------------------------------------------------------
//
/*
void CPresenceCacheSession::HandleGetAllServicesL(const RMessage2& aMessage )   
    {
    TInt serviceCount = iServer.iPresenceCache.Count();
    TInt size(NULL);
    
    if(serviceCount)
    	{
        CArrayFixFlat<TUid>* services = new (ELeave) CArrayFixFlat<TUid>(1);
        CleanupStack::PushL(services);
    	
    	for(TInt i=0; i<serviceCount; i++)
	        {
	        if((iServer.iPresenceCache[i])->PresenceCount())
	            services->AppendL((iServer.iPresenceCache[i])->Service());
	        }
	    
	    if (iDataPack)
	        {
	        delete iDataPack; iDataPack = NULL;
	        }
	    iDataPack = PackServicesL(services);
	    CleanupStack::PopAndDestroy(services);
	    size = (iDataPack->Des()).Size();
    	}
    aMessage.Complete(size);
    }*/
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleWritePresenceInfoAsyncL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleWritePresenceInfoAsyncL(const RMessage2& aMessage )
    {
    if(iActiveHelper->IsActive())
        {
        aMessage.Complete(KErrServerBusy);
        return;        
        }
    iMessage = aMessage;
           
    const TInt pckSize = aMessage.GetDesLengthL(0);
    if(iDataPackAsync)
        delete iDataPackAsync; iDataPackAsync = NULL;
    iDataPackAsync = HBufC8::NewL( pckSize );
    TPtr8 paramPckPtr = iDataPackAsync->Des();
    aMessage.ReadL( 0, paramPckPtr );  
    
    TPresenceArrayPacker::UnPackArrayL( iBuddypresInfoList,*iDataPackAsync  );
        
    iAsyncReq = NRequest::EWritePresenceInfoAsync;
    iActiveHelper->Start();
    return;          
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::WritePresenceInfoAsyncL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::WritePresenceInfoAsyncL()
    {
    TInt err(KErrNone);
    TInt thisTimeWrites(0);
    
    // This routine reads data from iBuddypresInfoList and tries to write 
    // data to cache on each runl cycle. It writes either KMaxWriteEachCycle 
    // times, or upto data is finnished. After data is written KMaxWriteEachCycle
    // times and still more data is left, it makes this class active so that this
    // function is again called on next RunL.
    
    while (iBuddypresInfoList.Count())
        {
        err = TryWriteBuddyToCacheL( iBuddypresInfoList[0]);        
        if (err == KErrNone || err == KErrNoMemory)       // ownership transferred
            {
            iBuddypresInfoList.Remove( 0 );            
            }
        else        
            {
            iBuddypresInfoList.ResetAndDestroy();            
            break; 
            }
        thisTimeWrites++;    
        if(thisTimeWrites == NConstants::KMaxWriteEachCycle) // this times writes are over
            {
            if(iActiveHelper->IsActive())// unusual condition
                {
                err = KErrGeneral;
                break;
                }
            iActiveHelper->Start();
            return; // set for next time writing.
            }
       }
              
    // if we able to come out of while loop either writing is complete or
    // there was some error while writing, in both cases finnish writing
    iAsyncReq = NRequest::ENoRequestMade;
    iMessage.Complete(err);
    return;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleSubscribeBuddyPresenceChangeL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleSubscribeBuddyPresenceChangeL
                                                (const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleSubscribeBuddyPresenceChange - begin"));

    // Extract info from message
    const TInt pckSize = aMessage.GetDesLengthL(1);
    HBufC* idPack = HBufC::NewLC( pckSize );
    TPtr paramPckPtr = idPack->Des();
    aMessage.ReadL( 1, paramPckPtr );
    
    TPtrC identity = paramPckPtr;
    TPtrC serviceName = GetServiceName( identity );
    
    CPresenceCacheBuddyStore* buddyStore(NULL);
    CPresenceCacheServiceStore* serviceStore(NULL);
    
    TInt err(KErrNone);
    
    // now processing
    TInt serviceCount = iServer.iPresenceCache.Count();
    
    TRACE_1( _L("_______serviceCount[%d]"), serviceCount);
    
    for (TInt i=0; i<serviceCount; i++)
        {
        if (!(serviceName.CompareF((iServer.iPresenceCache[i])->ServiceName())))
            {
            serviceStore = iServer.iPresenceCache[i];
            buddyStore = (iServer.iPresenceCache[i])->FindAndGet(identity);
            break;
            }
        }
        
    if (!buddyStore) // if buddy was not found, create and add it
        {
        if (!serviceStore) //if service was also not found, create it
            {
            serviceStore = CPresenceCacheServiceStore::NewLC();
            serviceStore->SetServiceNameL(serviceName);
            (iServer.iPresenceCache).Append(serviceStore); //ownership transferred
            CleanupStack::Pop(serviceStore);  
            }
        else
            {            
            }
        buddyStore = CPresenceCacheBuddyStore::NewLC(serviceStore,identity);
        err = serviceStore->AddBlind(buddyStore); //ownership transferred
        CleanupStack::Pop(buddyStore);
        } 
    else
        {
        }
    
    CleanupStack::PopAndDestroy(idPack);
    
    TRACE_1( _L("buddyStore[%d]"), buddyStore);

    buddyStore->AddSubscribedSession(this);
    if (iSubscribedStores.Find(buddyStore) < 0) //if session not already there
        iSubscribedStores.Append(buddyStore);
    
    aMessage.Complete(err); // there shouldnt be any errors
    TRACE( _L("CPresenceCacheSession::HandleSubscribeBuddyPresenceChange - end"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleUnSubscribeBuddyPresenceChangeL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleUnSubscribeBuddyPresenceChangeL
                                                (const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleUnSubscribeBuddyPresenceChange - begin"));

    // Extract info from message
    const TInt pckSize = aMessage.GetDesLengthL(1);
    HBufC* idPack = HBufC::NewLC( pckSize );
    TPtr paramPckPtr = idPack->Des();
    aMessage.ReadL( 1, paramPckPtr );
    TInt err(KErrNotFound);
    TPtrC identity = paramPckPtr;        
    TPtrC serviceName = GetServiceName(identity);
        
    // now processing
    TInt serviceCount = iServer.iPresenceCache.Count();
    CPresenceCacheBuddyStore* buddyStore(NULL);
    
    TRACE_1( _L("_______serviceCount[%d]"), serviceCount);
    
    for(TInt i=0; i<serviceCount; i++)
        {
        if (!(serviceName.CompareF((iServer.iPresenceCache[i])->ServiceName())))
            {
            buddyStore = (iServer.iPresenceCache[i])->FindAndGet(identity);
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(idPack);
    
    TRACE_1( _L("buddyStore[%d]"), buddyStore);
    if(buddyStore) // if found
        {
        buddyStore->RemoveSubscribedSession(this);
		
        const TInt buddyStoreIndex = iSubscribedStores.Find(buddyStore);
        if ( KErrNotFound != buddyStoreIndex )
            {
            iSubscribedStores.Remove(buddyStoreIndex);
            }
        err = KErrNone;
        }
    
    aMessage.Complete(err);
    TRACE_1( _L("CPresenceCacheSession::HandleUnSubscribeBuddyPresenceChange (%d) - end"),err);
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleGetLastNotifiedtPacketL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleGetLastNotifiedtPacketL(const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::HandleGetLastNotifiedtPacketL() - begin"));
    
    SendNextEventL( aMessage );
    
    aMessage.Complete(KErrNone); 
    
    SendNextEventNotificationL();
        
    TRACE( _L("CPresenceCacheSession::HandleGetLastNotifiedtPacketL() - end"));    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleWaitingForNotificationL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleWaitingForNotificationL(const RMessage2& aMessage )
    {
    iMessageForNoti = aMessage;
    SendNextEventNotificationL( );
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::HandleCancelWaitingForNotification()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::HandleCancelWaitingForNotification
                                                    (const RMessage2& aMessage )
    {
    aMessage.Complete(KErrNone);
    if(!iMessageForNoti.IsNull())
        iMessageForNoti.Complete(KErrCancel);
    EmptyNotificationQueue();    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::NotifyPresenceChangeL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::NotifyPresenceChangeL(
    const MPresenceBuddyInfo2* aPresenceBuddyInfo )  
    {
    TRACE( _L("CPresenceCacheSession::NotifyPresenceChangeL() - begin"));
       
    if ( !aPresenceBuddyInfo)
        {        
        return;
        }

    HBufC8* msg = PackBuddyPresenceInfoLC(aPresenceBuddyInfo);        
    AddIntoQueueL( msg );
    // ownership is taken
    CleanupStack::Pop( msg );
        
    SendNextEventNotificationL( );
    
    TRACE( _L("CPresenceCacheSession::NotifyPresenceChangeL - end"));
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::NotifyAllSubscribersL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::NotifyAllSubscribersL
                                        (CPresenceCacheBuddyStore* aBuddyStore)
    {
    TRACE( _L("CPresenceCacheSession::NotifyAllSubscribers() - begin"));
    if (!aBuddyStore)
        return;
    TInt subsCount = aBuddyStore->GetSubscribedSessions().Count();
    TRACE_1( _L("_______total subscribed sessions[%d]"), subsCount);
    TBool ownBuddyInfo(EFalse);
    const MPresenceBuddyInfo2* buddyInfo;
    
    if (subsCount) // start processing if there are subscribers
        {
        //if presence has been deleted we need to identity and service id in the packed
        //which will be sent to clients
        if (!(aBuddyStore->PresenceBuddyInfo()))
            {           
            MPresenceBuddyInfo2* buddyInfoTemp =  MPresenceBuddyInfo2::NewLC();
            buddyInfoTemp->SetIdentityL( aBuddyStore->BuddyId() );
            ownBuddyInfo = ETrue;
            buddyInfo = buddyInfoTemp;
            }
        else
            {
            buddyInfo = aBuddyStore->PresenceBuddyInfo();  
            }
            
        CPresenceCacheSession* session(NULL);
        for (TInt i=0;i<subsCount;i++)
            {
            session = aBuddyStore->GetSubscribedSessions()[i];
            if (iServer.SessionExists(session))
                {
                TRACE_1( _L("_______valid session notified[%d]"), session);
                session->NotifyPresenceChangeL(buddyInfo);
                }
            else
                {
                aBuddyStore->RemoveSubscribedSession(session);
                subsCount--;//substract this removed session from total count
                i--;
                TRACE_1( _L("_______invalid session removed[%d]"), session);
                }    
            }
        }
        
    if (ownBuddyInfo)
        CleanupStack::PopAndDestroy(1); //buddyInfoTemp
    TRACE( _L("CPresenceCacheSession::NotifyAllSubscribers() - end"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::GetServiceName()
// ---------------------------------------------------------------------------
//
TPtrC CPresenceCacheSession::GetServiceName(const TDesC& aXspId)
    {
    _LIT(KColon, ":");
    TInt pos = aXspId.Find(KColon);
    TPtrC serviceName;
    if(pos>0) // if colon found and there is something before colon, i.e. xsp id
        {
        serviceName.Set(aXspId.Left(pos));
        }
    else
        serviceName.Set(TPtrC());
    return serviceName;
    }
        
// ---------------------------------------------------------------------------
// CPresenceCacheSession::Cancel()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::Cancel()
    {
    if (iAsyncReq != NRequest::ENoRequestMade)
        {
        iMessage.Complete(KErrCancel);
        iAsyncReq = NRequest::ENoRequestMade;
        }
    return;   
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::PackPresenceDocLC()
// ---------------------------------------------------------------------------
//
HBufC8* CPresenceCacheSession::PackPresenceInfoLC(const MPresenceBuddyInfo2& aPresInfo)
    {
    HBufC8* pack = TCacheObjectPacker< MPresenceBuddyInfo2 >::PackL( aPresInfo );    
    CleanupStack::PushL( pack );
    return pack;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::PackBuddyPresenceInfoLC()
// ---------------------------------------------------------------------------
//
HBufC8* CPresenceCacheSession::PackBuddyPresenceInfoLC(
    const MPresenceBuddyInfo2* aBuddyPresInfo )
    {
    HBufC8* pack(NULL);
    if ( aBuddyPresInfo )
        {
        pack = TCacheObjectPacker< const MPresenceBuddyInfo2>::PackL( *aBuddyPresInfo );
        CleanupStack::PushL( pack );
        }

    return pack;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::CPresenceCacheSession()
// ---------------------------------------------------------------------------
//
CPresenceCacheSession::CPresenceCacheSession( CPresenceCacheServer& aServer )
                                                        : iServer( aServer ),
                                                        iLastServiceCount(0)
    {
    TRACE( _L("CPresenceCacheSession::CPresenceCacheSession()"));
    }

// ---------------------------------------------------------------------------
// CPresenceCacheSession::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::ConstructL()
    {
    iActiveHelper = CCacheSessionActiveHelper::NewL(this);    
    iServer.IncrementSessions();
    return;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheSession::RemoveMySubscriptions()
// ---------------------------------------------------------------------------
//
void CPresenceCacheSession::RemoveMySubscriptions()
    {
    TInt count = iSubscribedStores.Count();
    for(TInt i=0;i<count;i++)
        {
        // it can be assumed that store is alive, since it won't be deleted
        // until there is any session subscribed to it
        (iSubscribedStores[i])->RemoveSubscribedSession(this);
        }
    }

// ----------------------------------------------------
// 
void CPresenceCacheSession::EmptyNotificationQueue()
    {
    const TInt count = iNotifyEvents.Count();
    for( TInt i = 0;i < count;i++ )
        {
        CPresenceCacheEvent* event( iNotifyEvents[0] );
        iNotifyEvents.Remove( 0 );
        delete event;
        event = NULL;
        }
    }

// ----------------------------------------------------
// 
void CPresenceCacheSession::AddIntoQueueL( HBufC8* aMsg )
    {
    TRACE( _L("CPresenceCacheSession::AddIntoQueueL() - begin"));    
    CPresenceCacheEvent* event = CPresenceCacheEvent::NewL( aMsg );
    CleanupStack::PushL( event );
    User::LeaveIfError( iNotifyEvents.Append( event ) );
    CleanupStack::Pop();  //event
    TRACE( _L("CPresenceCacheSession::AddIntoQueueL() - end"));    
    }

// ----------------------------------------------------
// 
void CPresenceCacheSession::SendNextEventL( const RMessage2& aMessage )
    {
    TRACE( _L("CPresenceCacheSession::SendNextEventL() - begin"));    
    
    if ( iNotifyEvents.Count() > 0 )
        {
        CPresenceCacheEvent* event( iNotifyEvents[0] );
        aMessage.WriteL(0, event->Message());            
        // aMessage.Complete(KErrNone);       
        iNotifyEvents.Remove( 0 );
        delete event;
        event = NULL;
        }
    else
        {        
        }
    
    TRACE( _L("CPresenceCacheSession::SendNextEventL() - end"));
    
    return;
    }

// ----------------------------------------------------
// 
void CPresenceCacheSession::SendNextEventNotificationL( )
    {
    TRACE( _L("CPresenceCacheSession::SendNextEventNotificationL() - begin"));
    
    TInt sizeOfReturnPacket(NULL);    
    
    if ( iNotifyEvents.Count() > 0 && (!iMessageForNoti.IsNull()))
        {
        CPresenceCacheEvent* event( iNotifyEvents[0] );
        if ( !event->NotifySent() )
            {
            event->SetNotifySent();              
            sizeOfReturnPacket = event->Message().Size();
            TPckgBuf<TInt> p(sizeOfReturnPacket);          
            iMessageForNoti.WriteL(0,p);            
            iMessageForNoti.Complete(KErrNone); 
            }
        else
            {
            // Do not sent twice the same, 
            // client first refresh notification request and after that gets the last notification packet.
            }
        }
    else
        {        
        }
    
    TRACE( _L("CPresenceCacheSession::SendNextEventNotificationL() - end"));
    
    return;
    }
    
// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::NewL()
// ---------------------------------------------------------------------------
//
CCacheSessionActiveHelper* CCacheSessionActiveHelper::NewL
                                            (CPresenceCacheSession* aSession )
    {
    CCacheSessionActiveHelper* self = new(ELeave) CCacheSessionActiveHelper(aSession);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::~CCacheSessionActiveHelper()
// ---------------------------------------------------------------------------
//
CCacheSessionActiveHelper::~CCacheSessionActiveHelper( )
    {
    Cancel();   
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::Start()
// ---------------------------------------------------------------------------
//
void CCacheSessionActiveHelper::Start()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::CCacheSessionActiveHelper()
// ---------------------------------------------------------------------------
//
CCacheSessionActiveHelper::CCacheSessionActiveHelper(CPresenceCacheSession* aSession ) 
                                                    : CActive(EPriorityStandard)
    {
    iCacheSession = aSession;
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::RunL()
// ---------------------------------------------------------------------------
//
void CCacheSessionActiveHelper::RunL()
    {
    switch (iCacheSession->iAsyncReq)
        {
        case NRequest::EWritePresenceInfoAsync:
            iCacheSession->WritePresenceInfoAsyncL();
            break;

        case NRequest::EPrepReadAllBuddiesPresenceInService:
            iCacheSession->ReadAllBuddiesPresenceInServiceL();
            break;

        case NRequest::ENoRequestMade:
        default:
            break;
        }    
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::DoCancel()
// ---------------------------------------------------------------------------
//
void CCacheSessionActiveHelper::DoCancel()
    {
    iCacheSession->Cancel();
    }

// ---------------------------------------------------------------------------
// CCacheSessionActiveHelper::ConstructL()
// ---------------------------------------------------------------------------
//
void CCacheSessionActiveHelper::ConstructL()
    {
	CActiveScheduler::Add(this);
    }

TInt CCacheSessionActiveHelper::RunError(TInt /*aError*/)
    {
    iCacheSession->Cancel();    
    return KErrNone;
    }


// End of File
