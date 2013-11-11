/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRDATAPROVIDERTIMER_H
#define IRDATAPROVIDERTIMER_H

#include <e32base.h>

/**
 * Call for timer expiry
 */
class MIRDataProviderTimeoutNotifier
    {
public:

    /**
     * Called when the timer raises an event.
     */
    virtual void TimerExpired() = 0;
    };

/**
 * Timer class for response timerouts
 */
NONSHARABLE_CLASS ( CIRDataProviderTimer ) : public CTimer
    {
public:

    /**
     * CIRDataProviderTimer::NewL()
     * Two phase constructor
     * @param aPriority Specifies the priority of the timer
     * @param aNotify Reference of the observer
     * @return CIRDataProviderTimer object
     */

    static CIRDataProviderTimer *NewL( const TInt aPriority,
                    MIRDataProviderTimeoutNotifier &aNotify );

    /** 
     * CIRDataProviderTimer::NewLC()
     * @param aPriority Specifies the priority of the timer
     * @param aNotify Reference of the observer
     * @return CIRDataProviderTimer object
     */
    static CIRDataProviderTimer *NewLC( const TInt aPriority,
                    MIRDataProviderTimeoutNotifier &aNotify );

    /**
     * CIRDataProviderTimer::~CIRDataProviderTimer()
     * Default C++ destructor
     */
    virtual ~CIRDataProviderTimer();

protected:

    /** 
     * Derived from CTimer
     * CIRDataProviderTimer::RunL()
     */
    void RunL();

    /** 
     * Derived from CTimer
     * CIRDataProviderTimer::RunError()
     */
    TInt RunError( TInt aError );

private:

    /**
     * CIRDataProviderTimer::CIRDataProviderTimer()
     * 
     */
    CIRDataProviderTimer( const TInt aPriority,
                    MIRDataProviderTimeoutNotifier &aNotify );

    /**
     * CIRDataProviderTimer::ConstructL()
     * Second phase constructor
     */
    void ConstructL();

private:

    /** 
     * Handle to the class to be notified of a timeout event.
     */
    MIRDataProviderTimeoutNotifier &iNotify;

    };

#endif // IRDATAPROVIDERTIMER_H


