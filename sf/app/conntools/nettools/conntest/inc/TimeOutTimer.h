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
* Description: CTimeOutTimer is used to generate time out events
*
*/

#ifndef __TIMEOUTTIMER_H__
#define __TIMEOUTTIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MTimeOutNotify;

/**
*  This class will notify an object after a specified timeout.
*/
class CTimeOutTimer : public CTimer
{
public:
    
    /**
    * Create a CTimeOutTimer object
    * @param aPriority priority to use for this timer
    * @param aTimeOutNotify object to notify of timeout event
    * @result a pointer to the created instance of CTimeOutTimer
    */
    static CTimeOutTimer* NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
    
    /**
    * Create a CTimeOutTimer object
    * @param aPriority priority to use for this timer
    * @param aTimeOutNotify object to notify of timeout event
    * @result a pointer to the created instance of CTimeOutTimer
    */
    static CTimeOutTimer* NewLC(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
    
    /**
    * Destroy the object and release all memory objects
    */
    ~CTimeOutTimer();
    
protected: // From CTimer
    
    /**
    * Invoked when a timer event occurs
    */
    virtual void RunL();
    
private:
    
    /**
    * Perform the first phase of two phase construction 
    * @param aPriority priority to use for this timer
    */
    CTimeOutTimer(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
    
    /**
    * Perform the second phase construction of a CTimeOutTimer 
    */
    void ConstructL();
    
private:
    // Member variables
    MTimeOutNotify& iNotify;
};

#endif // __TIMEOUTTIMER_H__

