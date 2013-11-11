/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


#ifndef C_ACTIVEWAIT_H
#define C_ACTIVEWAIT_H

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
/**
*  Active waiter class for waiting any active request. When asynchronous
*  request has been completed a given callback method will be called.
*
*  @lib videosource.lib
*/
template < class R > class CVSActiveWait: public CActive
    {
    private :
    
  			typedef void ( R::*TVSActiveWaitCallback) ( TInt );  		
    
    public : // Constructors and destructor

        /**
        * Constructor.
        * @param "aRequester" ?description
        */
        inline CVSActiveWait( R* aRequester );

        /**
        * Destructor.
        */
        inline ~CVSActiveWait();

        /**
        * Initializes the waiter class.
        * @param "aCallback" Pointer to callback method that will be called
        * when asynchronous request has been completed.
        */
        inline void InitializeWaiter( TVSActiveWaitCallback aCallback );

        /**
        * Returns pointer to this active object's TRequestStatus member
        * (iStatus).
        * @return Pointer to this classes TRequestStatus member (iStatus).
        */
        inline TRequestStatus* RequestStatus();

        /**
        * Signals waiter with given error code.
        * @param aError Integer error code.
        */
        inline void Signal( TInt aError );

    private :    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        inline void RunL();

        /**
        * From CActive. See CActive for description.
        */
        inline void DoCancel();

    private :    // Data

        // Pointer to requester. Callback method call will be made in this
        // context.
        R* iRequester; // not owned

        // Pointer to callback method.
        TVSActiveWaitCallback iCallback;
    };

#include "cactivewait.inl"
    
#endif      // C_ACTIVEWAIT_H
     