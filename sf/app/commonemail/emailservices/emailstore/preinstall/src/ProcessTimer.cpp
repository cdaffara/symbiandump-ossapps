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
* Description: Timeout timer implementation.
*
*/



#include "ProcessTimer.h"

const TInt KSecond = 1000000; // 1 seconds

/**********************************
 * Method: NewL
 **********************************/
CProcessTimer* CProcessTimer::NewL(MObserverPreInstall& aObserver)
    {
    CProcessTimer * self = new ( ELeave ) CProcessTimer(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/**********************************
 * Method: Constructor
 **********************************/
CProcessTimer::CProcessTimer(MObserverPreInstall& aObserver) :
        CTimer( CActive::EPriorityStandard ),iObserver(aObserver)
    {
    __LOG_CONSTRUCT( "IMS", "CProcessTimer" )
    }

/**********************************
 * Method: ConstructorL
 **********************************/
void CProcessTimer::ConstructL()
    {
    __LOG_ENTER( "ConstructL" ) 
    
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    
    __LOG_EXIT
    }

/**********************************
 * Method: Destructor
 **********************************/
CProcessTimer::~CProcessTimer()
    {
    Cancel();
    __LOG_ENTER( "Destructor Timeout Timer" )
    __LOG_EXIT
    __LOG_DESTRUCT
    }

/**********************************
 * Method: StartTimeoutTimer
 * aTimerValue - value of the timeout 
 **********************************/
void CProcessTimer::StartTimeoutTimer( TInt aTimerValue )
    {
    CTimer::After( aTimerValue * KSecond );
    }

/**********************************
 * Method: RunL
 **********************************/
void CProcessTimer::RunL()
    {
    __LOG_ENTER("RunL")
    __LOG_WRITE_FORMAT1_INFO( "iStatus=%d", CTimer::iStatus.Int() )

    if ( KErrNone == iStatus.Int() )
        {
        iObserver.Timeout();
        }
    __LOG_EXIT;
    }
    
