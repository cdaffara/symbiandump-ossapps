/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPTimer class
*
*/


#ifndef CUPTIMER_H
#define CUPTIMER_H

#include <e32base.h>

class CUpnpItem;

class MUPTimerObserver
	{
	public:
	    
	    /**
    	* Method which is called when timer in CUPTimer expires
    	*/
		virtual void TimerExpiredL(CUpnpItem* aItemToShare) = 0;
	};
	
class CUPTimer :  public CActive
	{
	
	public:

    /**
    * Static method for creating an instance
    * @return CUPTimer object
    */
    static CUPTimer* NewL();

    /**
    * Static method for creating an instance
    * @return CUPTimer object
    */
    static CUPTimer* NewLC();

    /**
    * Destructor
    */
    ~CUPTimer();

    /**
    * Method for seting the active object active
    */
    void SetActiveNow();

    /**
    * RunL
    */
    void RunL();

    /**
    * RunError
    * @param aError  Symbian error code which should indicate what happened
    */
    TInt RunError(TInt aError);
    
    /**
    * DoCancel
    */
    void DoCancel();

    /**
    * Method which will wait until given microseconds have passed and then call the given callback
    * @param aObserver  Observer class which called when timer expires
    * @param aTimeOut  Interval which waited before observer is called
    * @param aStatus  Request status object
    */    
    void WaitUntilTimeExpired(MUPTimerObserver* aObserver, 
    						  TTimeIntervalMicroSeconds32 aTimeOut,
    						  CUpnpItem* aItemToShare);//, 
    //						  TRequestStatus &aStatus);
    
    /**
    * CPeriodic timed out when this method is called
    * @param aPtr  Any pointer
    * @return Error code
    */
    static TInt TimedOut(TAny* aPtr);

    /**
    *  For canceling the request
    */ 
    void CancelTimer();
       
private:

    /**
    *  ConstructL
    */
    void ConstructL();
    
    /**
    * Private constructor
    */    
    CUPTimer();
    
private:

    /**
    * Pointer to a observer class
    */
    MUPTimerObserver* iObserver;
    
    /**
    * Instance of periodic timer
    */
	CPeriodic* iTimeOutTimer;
	
	CUpnpItem* iItemToShare;

	};

#endif // CUPTIMER_H

//  End of File
