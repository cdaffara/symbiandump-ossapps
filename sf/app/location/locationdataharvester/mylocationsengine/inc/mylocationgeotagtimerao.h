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
* Description:  3AM geo tagger handler
*
*/

#ifndef __MYLOCATIONGEOTAGTIMERAO_H__
#define __MYLOCATIONGEOTAGTIMERAO_H__


// INCLUDES
#include <e32std.h>
#include <e32base.h>

/*
 * MyLocationTimerObserver class
 * observer class to notify the mylocation engine about the 3AM timer expiry.
 */
class MyLocationTimerObserver
{
public:
   virtual void MyLocationThreeAMTimerExpiredL() = 0; 
};

/*
 * CLocationGeoTagTimerAO class
 * Class to handle 3AM timer expiry.
 */
class CLocationGeoTagTimerAO : public CTimer
{
    
public:
     /**
      * 1st phase constructor.
      * @param aObserver, 3AM timer observer
      */
     static CLocationGeoTagTimerAO* NewL(
             MyLocationTimerObserver& aObserver ); 

     /**
      * Destructor
      */
     ~CLocationGeoTagTimerAO();

     /**
      * Start 3AM timer
      */
     void StartTimer();

     
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
protected:
#endif
     /*
      * @see CActive::RunL()
      */
     void RunL();
    
     /**
      * 2nd Phase constructor
      */
     void ConstructL();
    
     /**
	  * CLocationGeoTagTimerAO
	  * Constructor
	  * @param aObserver, 3AM timer observer
	  */
     CLocationGeoTagTimerAO( MyLocationTimerObserver& aObserver );
      
private:      
      MyLocationTimerObserver* iTimerObserver;
};


#endif /*__MYLOCATIONGEOTAGTIMERAO_H__*/

// End of file
