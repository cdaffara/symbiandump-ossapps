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
* Description:  
*
*/


#include "presencecacheevent.h"



// ----------------------------------------------------------
// 
CPresenceCacheEvent::CPresenceCacheEvent(  )
: iNotifySent(EFalse)
    {
    }
    
// ----------------------------------------------------------
// 
CPresenceCacheEvent* CPresenceCacheEvent::NewL( HBufC8* aMsg )
    {
    CPresenceCacheEvent* self = new( ELeave ) CPresenceCacheEvent();
    CleanupStack::PushL( self );
    self->ConstructL( aMsg );
    CleanupStack::Pop();
    return self;   
    }


// ----------------------------------------------------------
// 
CPresenceCacheEvent::~CPresenceCacheEvent()
    {
    delete iMessage;
    }

// ----------------------------------------------------------
// CPresenceCacheEvent::ConstructL
// 
// ----------------------------------------------------------
// 
void CPresenceCacheEvent::ConstructL( HBufC8* aMsg )
    {
    iMessage = aMsg;
    } 
// ----------------------------------------------------------
// 
TPtrC8 CPresenceCacheEvent::Message()
    {
    return iMessage ? iMessage->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// 
TBool CPresenceCacheEvent::NotifySent()
    {
    return iNotifySent;
    }

// ----------------------------------------------------------
// 
void CPresenceCacheEvent::SetNotifySent()
    {
    iNotifySent = ETrue;    
    }


