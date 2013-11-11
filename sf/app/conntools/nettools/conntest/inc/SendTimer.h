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
* Description: CSendTimer is used for sending data at specified intervals
*
*/

#ifndef __SENDTIMER_H__
#define __SENDTIMER_H__

#include <e32base.h>

class CSocketsEngine;

/**
* This class will send data over socket after given
* time has elapsed.
*/
class CSendTimer : public CTimer
{
public:

    /**
    * Create a CSendTimer object
    * @param aPriority priority to use for this timer
    * @param aEngine object that knows how to send data
    * @result a pointer to the created instance of CTimeOutTimer
    */
	static CSendTimer* NewL(const TInt aPriority, CSocketsEngine* aEngine);

    /**
    * Create a CSendTimer object
    * @param aPriority priority to use for this timer
    * @param aEngine object that knows how to send data
    * @result a pointer to the created instance of CTimeOutTimer
    */
    static CSendTimer* NewLC(const TInt aPriority, CSocketsEngine* aEngine);

    /**
    * Destroy the object and release all memory objects
    */
	~CSendTimer();

protected: // From CTimer

    /**
    * Invoked when a timer event occurs
    */
    virtual void RunL();

private:

    /**
    * Perform the first phase of two phase construction 
    * @param aPriority priority to use for this timer
    * @param aEngine object that knows how to send data
    */
	CSendTimer(const TInt aPriority, CSocketsEngine* aEngine);

    /**
    * Perform the second phase construction of a CSendTimer 
    */
    void ConstructL();

private:
	// Member variables
	CSocketsEngine*      iEngine;
};

#endif // __SENDTIMER_H__

