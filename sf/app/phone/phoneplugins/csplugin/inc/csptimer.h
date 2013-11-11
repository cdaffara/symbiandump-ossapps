/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer service
*
*/


#ifndef CSPTIMER_H
#define CSPTIMER_H

#include <e32base.h>

class MCSPTimerObserver;

/**
 *  Timer service.
 *
 *  @lib PhoneEngine.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CSPTimer ) : protected CTimer
    {
public:

    static CSPTimer* NewL();

    static CSPTimer* NewLC();

    virtual ~CSPTimer();

    /**
     * Request for notify after aTimeout.
     * After timeout has passed observer is called. Panics
     * if there is notify already ongoing ie IsNotifyOngoing
     * returns ETrue.
     *
    *
     * @param aTimeout Time out in microseconds. 
     * @param aTimerObserver Observer for notify.
     */
    virtual void NotifyAfter( TInt aTimeout, 
        MCSPTimerObserver& aTimerObserver );
    
    /**
     * Cancels the outstanding notify request. After
     * this call notify is no longer ongoing ie IsNotifyOngoing
     * return EFalse.
     *
    *
     */
    virtual void CancelNotify();
    
    /**
     * Determines if there is notify ongoing.
     *
    *
     * @return ETrue - notify is ongoing, EFalse - notify is not ongoing.
     */
    virtual TBool IsNotifyOngoing();
    
protected:    
    
// from base class CActive

    /**
     * From CActive
     * Called when timer timeout is up.
     *
    *
     */
    void RunL();
    
    CSPTimer();

    void ConstructL();

protected: // data

    /**
     * Observer for notify service.
     * Not own.
     */
    MCSPTimerObserver* iObserver;

    };

#endif // CSPTIMER_H
