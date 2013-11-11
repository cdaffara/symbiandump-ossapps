/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "cphonepubsubproxy.h"
       


CPhonePubSubProxy* CPhonePubSubProxy::Instance()
{
    static CPhonePubSubProxy p;
    return &p;
}

CPhonePubSubProxy::~CPhonePubSubProxy()
{
    
}

void CPhonePubSubProxy::CancelAllNotifications( 
            MPhonePubSubObserver* aObserver )
{
    
}
        
TInt CPhonePubSubProxy::Value( const TUid& aCategory, const TUint aKey )
{
    
}

void CPhonePubSubProxy::NotifyChangeL( 
            const TUid& aCategory, 
            const TUint aKey,
            MPhonePubSubObserver* aObserver )
{
    
}

void CPhonePubSubProxy::ChangePropertyValue( 
            const TUid& aCategory,
            const TUint aKey,
            const TInt aValue )
{
    
}

CPhonePubSubProxy::CPhonePubSubProxy()
{
    
}

void CPhonePubSubProxy::CancelAllObserverNotifies( 
    MPhonePubSubObserver* aObserver )
{
    
}

void CPhonePubSubProxy::HandlePropertyChangedL( 
    const TUid& aCategory, 
    const TUint aKey, 
    const TInt aValue )
{
    
}

