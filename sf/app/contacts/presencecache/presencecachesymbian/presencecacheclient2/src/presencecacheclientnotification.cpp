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
* Description:  Implementation for presence cache client.
*
*/

#include <e32std.h>

#include <mpresencebuddyinfo2.h>
#include <presencecachereadhandler2.h>

#include "presencecacheclientnotification.h"
#include "presencecachedefs2.h"
#include "cacheobjecthelpers.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::NewL
// ---------------------------------------------------------------------------
//
CPresenceCacheClientNotification* CPresenceCacheClientNotification::NewL(
    CPresenceCacheClient& aClient)
    {
    CPresenceCacheClientNotification* self = new(ELeave)CPresenceCacheClientNotification(aClient);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::~CPresenceCacheClientNotification
// ---------------------------------------------------------------------------
//
CPresenceCacheClientNotification::~CPresenceCacheClientNotification()
    {
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::CPresenceCacheClientNotification
// ---------------------------------------------------------------------------
//
CPresenceCacheClientNotification::CPresenceCacheClientNotification(CPresenceCacheClient& aClient)
: CActive( EPriorityStandard ), iClient( aClient ), iNotificationSubscribed( EFalse )
    {        
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::ConstructL
// ---------------------------------------------------------------------------
//
void CPresenceCacheClientNotification::ConstructL()
    {
    CActiveScheduler::Add( this );   
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::RunL
// ---------------------------------------------------------------------------
//
void CPresenceCacheClientNotification::RunL()
    {    
    TInt origStatus = iStatus.Int();
    
    MPresenceCacheReadHandler2* client = ClientNotifyHandler();    
    
    if ( origStatus )
        {
        iNotificationSubscribed = EFalse;        
        TRAP_IGNORE( client->HandlePresenceNotificationL( origStatus, NULL ));
        }
    else
        {
        StartNotifySubscription();        
        PreseneceFromServerToClientL( *client, iSizePckg() );
        }              
    }
// ----------------------------------------------------------
// CPresenceCacheClientNotification::RunError
// ----------------------------------------------------------
//
TInt CPresenceCacheClientNotification::RunError( TInt aError )
    {         
    if ( KErrServerTerminated == aError )
        {
        // When server is terminated we notify the clienrt about that
        iNotificationSubscribed = EFalse;         
        MPresenceCacheReadHandler2* client = ClientNotifyHandler();
        TRAP_IGNORE( client->HandlePresenceNotificationL( aError, NULL ));        
        }
    else
        {
        // StartNotifySubscription is already called.
        }
    return KErrNone;    
    } 

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::DoCancel
// ---------------------------------------------------------------------------
//
void CPresenceCacheClientNotification::DoCancel()
    {
    iClient.SendReceive( NRequest::ECancelWaitingForNotification );
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::SubscribePresenceBuddyChangeL
// ---------------------------------------------------------------------------
//    
TInt CPresenceCacheClientNotification::SubscribePresenceBuddyChangeL(
	const TDesC& aIdentity)
    {       
    if ( !iNotificationSubscribed )
        {
        StartNotifySubscription();
        }
                   
    // Package message arguments before sending to the server
    TIpcArgs args;
    args.Set(1, &aIdentity);  
        
    TInt err = iClient.SendReceive( NRequest::ESubscribeBuddyPresenceChange, args );    
        
    return err;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::UnsubscribePresenceBuddyChangeL
// ---------------------------------------------------------------------------
//    
void CPresenceCacheClientNotification::UnsubscribePresenceBuddyChangeL(
	const TDesC& aIdentity)
    {
    // Package message arguments before sending to the server
    TIpcArgs args;
    args.Set(1, &aIdentity);  
        
    TInt err = iClient.SendReceive( NRequest::EUnSubscribeBuddyPresenceChange, args );
    User::LeaveIfError( err );
    }
 
// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::StartNotifySubscriptionL
// ---------------------------------------------------------------------------
//    
void CPresenceCacheClientNotification::StartNotifySubscription()
    {                                    
    // Package message arguments before sending to the server
    SetActive();
    TIpcArgs args(&iSizePckg);            
    iClient.SendReceive( NRequest::EWaitingForNotification, args, iStatus );
    iNotificationSubscribed = ETrue;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::ClientNotifyHandler
// ---------------------------------------------------------------------------
//    
MPresenceCacheReadHandler2* CPresenceCacheClientNotification::ClientNotifyHandler()
    {
	return iClient.iNotifyClient;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClientNotification::PreseneceFromServerToClientL
// ---------------------------------------------------------------------------
//    
void CPresenceCacheClientNotification::PreseneceFromServerToClientL( 
    MPresenceCacheReadHandler2& aClient, TInt aSize )
    {     
    MPresenceBuddyInfo2* buddyInfo = MPresenceBuddyInfo2::NewLC();     
    HBufC8* presInfoDes = HBufC8::NewLC( aSize );
    TPtr8 ptrBuf( presInfoDes->Des() );
    TInt err = iClient.SendReceive( NRequest::EGetLastNotifiedtPacket, TIpcArgs( &ptrBuf ));    
    if ( !err )
        {
        TCacheObjectPacker< MPresenceBuddyInfo2 >::UnPackL( *buddyInfo, *presInfoDes );        
        CleanupStack::PopAndDestroy( presInfoDes );                          
        CleanupStack::Pop( ); //  buddyInfo
        aClient.HandlePresenceNotificationL( KErrNone, buddyInfo );
        }
    else
        {
        CleanupStack::PopAndDestroy( presInfoDes );         
        CleanupStack::PopAndDestroy( buddyInfo );          
        }
    }

// end of file
