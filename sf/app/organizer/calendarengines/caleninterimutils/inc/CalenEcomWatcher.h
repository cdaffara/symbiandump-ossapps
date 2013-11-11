/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*             This class watches for changes to the ECOM registry
*             and forces a recheck of Meeting request availablity
*       
*
*/


#ifndef __CALENECOMWATCHER_H__
#define __CALENECOMWATCHER_H__

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

class CCalenInterimUtils2Impl;
class MCalenEComChangeObserver;

/**
* CCalenEComWatcher watches for changes to the ECOM registry and forces a recheck
* of meetng request availablity
*/
NONSHARABLE_CLASS(CCalenEComWatcher) : public CActive
    {
    public: 
        /**
        * Constructor.
        * @param aUtils CCalenInterimUtils2Impl reference
        * @return a pointer to the new CCalenEComWatcher instance
        */
        IMPORT_C static CCalenEComWatcher* NewL( MCalenEComChangeObserver& aObserver );

        /**
        * Destructor
        */
        ~CCalenEComWatcher();

        /**
        * From CActive
        * Called when outstanding asynchronous request completes
        * This will be called when iNotificationTimer either completes
        * or is cancelled
        */
        void RunL();

        /**
        * From CActive
        * Called by the active scheduler if RunL leaves
        * Ensures we are ready to receive the next database event
        */
        TInt RunError();

        /**
        * From CActive
        * Implements cancellation of outstanding asynchronous requests
        * Cancels iNotificationTimer if started
        */
        void DoCancel();

    private:
        /**
        * C++ default constructor.
        * @param aUtils CCalenInterimUtils2Impl reference
        */
        CCalenEComWatcher( MCalenEComChangeObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        * Performs any construction which may leave
        */
        void ConstructL();

    private:
        REComSession iSession;
        MCalenEComChangeObserver& iEComObserver;
    };

#endif // __CALENECOMWATCHER_H__

// End of File
