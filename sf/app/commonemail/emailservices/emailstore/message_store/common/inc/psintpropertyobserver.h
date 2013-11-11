/*
* Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Int P/S key value observer header.
*
*/


#ifndef __PSINTPROPERTYOBSERVER_H__
#define __PSINTPROPERTYOBSERVER_H__

#include <e32base.h>
#include <e32property.h>

/**
 * P/S Property watcher observer.
 */
class MPSPropertyWatcherObserver
    {
    public:
        virtual void OnPSValueChangedToRequired() = 0;
    };

/**
 * P/S Integer Property watcher.
 * 
 */
NONSHARABLE_CLASS( CPSIntPropertyWatcher ): public CActive
    {

    public:        
        
        /**
         * Two-pase constructor.
         */
        static CPSIntPropertyWatcher* NewL( MPSPropertyWatcherObserver* aObserver );
        static CPSIntPropertyWatcher* NewLC( MPSPropertyWatcherObserver* aObserver );
        ~CPSIntPropertyWatcher();

        void StartL( TUid aCategory, TUint aKey, 
                              TUint32 aValueWhenToTrigger, 
                              TBool aDefineIfNeeded = EFalse,  
                              TSecurityPolicy aReadPolicy = TSecurityPolicy( TSecurityPolicy::EAlwaysPass ),
                              TSecurityPolicy aWritePolicy = TSecurityPolicy( TSecurityPolicy::EAlwaysPass ) );
        void Stop();
        

    private:
        CPSIntPropertyWatcher( );
        void ConstructL( MPSPropertyWatcherObserver* aObserver );

    protected:

        // CActive methods
        virtual void RunL();
        virtual void DoCancel();

    private:
        /** P/S property observed. */
        RProperty   iProperty;
        
        /** Property value needed to trigger the callback. */
        TUint32     iObservedValue;
        
        MPSPropertyWatcherObserver* iObserver;
    };

#endif  // __PSINTPROPERTYOBSERVER_H__
