/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cbttimer.h"
#include "clog.h"

//  CONSTANTS
 
CBtTimer* CBtTimer::NewL( MBtTimerObserver& aObserver )
    {
	CBtTimer* obj = new (ELeave) CBtTimer( aObserver );
    CleanupStack::PushL( obj ) ;
    obj->ConstructL();
	CleanupStack::Pop(); // obj
    return obj;
    }

CBtTimer::~CBtTimer()
	{
	}

CBtTimer::CBtTimer( MBtTimerObserver& aObserver ) : 
									CTimer( -1 ),
									iObserver( aObserver )
    {
	CActiveScheduler::Add( this );
    }

void CBtTimer::ConstructL()
    {
	CTimer::ConstructL();
    }

void CBtTimer::Start(TUint aDelay)
    {
    LOG1("[CBtTimer::Start] timeout %d", aDelay);
	After( KDiscoveryDelay );
    }

void CBtTimer::RunL()
	{
	LOG2("[CBtTimer::RunL] iStatus.Int(): %d/0x%X", iStatus.Int(),iStatus.Int());
	if( iStatus.Int() == KErrNone )
		{
		iObserver.HandleTimeoutL();	
		}
	LOG("CBtTimer::RunL end");	
	}

//  End of File
