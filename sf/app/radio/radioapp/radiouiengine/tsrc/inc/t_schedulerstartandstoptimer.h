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
* Description:  Timer for Starting and Stopping current Active Scheduler.
*
*/



#ifndef C_ACCSRVRESETINACTIVITYTIMETIMER_H
#define C_ACCSRVRESETINACTIVITYTIMETIMER_H



#include <e32base.h>


#include "mschedulerstartandstoptimerobserver.h"


/**  Timer's default value in micro seconds */
const TInt KTimerDefaultValue   = 400;


/**
 *  Timer class for Starting and Stopping current Active Scheduler.
 *  
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 10.1
 */
NONSHARABLE_CLASS(  CSchedulerStopAndStartTimer ): public CTimer
    {

public:
    enum TTimerId{
    ETimerIdRunMUT = 0,
    ETimerIdCreateMUT = 1,
    ETimerIdDeleteMUT = 2    
    };


    /**
    * Two-phased constructor.
    * @param aSchedulerStartAndStopTimerObserver Observer callback interface.
    */
    static CSchedulerStopAndStartTimer* NewL(
            MSchedulerStartAndStopTimerObserver& aSchedulerStartAndStopTimerObserver );

    /**
    * Destructor.
    */
    virtual ~CSchedulerStopAndStartTimer();

    /**
     * Starts the timer.
     * @since S60 10.1
     * @param aTime Time out value.
     * @param aTimeId Timer Id.
     * @return void
     */
     void StartTimer( TInt aTime = KTimerDefaultValue,
             TUint aTimerId = ETimerIdRunMUT );


protected:

    /**
     * From CActive.
     * Handles an active object's request completion event.
     *
     * @since S60 10.1
     * @param void
     */
   void RunL();
    
   /**
    * From CActive.
    * Handles RunL's leave cases
    *
    * @since S60 10.1
    * @param aError Error code.
    */
   TInt RunError( TInt aError );


private:

    /**
    * C++ default constructor.
    * @param aSchedulerStartAndStopTimerObserver Observer callback interface.
    */
    CSchedulerStopAndStartTimer(
            MSchedulerStartAndStopTimerObserver& aSchedulerStartAndStopTimerObserver );

    /**
     * By default Symbian OS constructor is private.
     * @param none.
     */
    void ConstructL();


private: // data
    enum TTmerPhase
        {
        ETimerPhaseUndefined = 0,
        ETimerPhaseStarting = 1,
        ETimerPhaseStarted = 2
        };

    /**
     * TimeOut time
     */
    TInt iTime;

    /**
     * Timer id. Client specific i.e same value is returned to observer.
     */
    TUint iTimerId;
    
    /**
     * ResetInactivityTimeTimerObserver object.
     */
    MSchedulerStartAndStopTimerObserver& iSchedulerStartAndStopTimerObserver;

    TTmerPhase iPhase;
    };


#endif // C_ACCSRVRESETINACTIVITYTIMETIMER_H
