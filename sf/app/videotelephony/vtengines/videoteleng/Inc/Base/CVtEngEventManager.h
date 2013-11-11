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
* Description:  Event manager 
*
*/



#ifndef CVTENGEVENTMANAGER_H
#define CVTENGEVENTMANAGER_H

//  INCLUDES
#include <e32base.h>
#include "VtEngEvents.h"

// FORWARD DECLARATIONS
class MVtEngEventObserver;

// CLASS DECLARATION

/**
*  This class is responsible for notifying client
*  on engine events.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtEngEventManager ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVtEngEventManager* NewL( 
            MVtEngEventObserver& aEventObserver );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngEventManager();

    public: // New functions
        
        /**
        * Notifies client on session state change.
        */
        void SessionStateChanged();

        /**
        * Notifies event to observer.
        * @param aEvent event
        */
        static void NotifyEvent( const TInt aEvent );
        
        void AddObserverL( MVtEngEventObserver* aObserver );
        
        void RemoveObserver( const MVtEngEventObserver* aObserver );

    private: // from CActive

        void RunL();

        void DoCancel();

    private:

        /**
        * Notifies event to observer
        */
        void DoNotifyEvent( const TInt aEvent );

        TBool QueueAsyncEvent( const TInt aEvent );
        
        /**
        * Queue event and signal AO.
        * @param aEvent Asynchronous event to be signalled.
        */        
        void QueueAndSignal( const TInt aEvent );

        /**
        * Signals AO.
        */
        void Signal();

    private:

        /**
        * C++ default constructor.
        */
        CVtEngEventManager();
        
        // 2nd phase constructor
        void ConstructL( MVtEngEventObserver& aEventObserver );

    private:     // Data
        
        RPointerArray<MVtEngEventObserver> iObservers;
        
        // Asynchronous events are stored in this array and 
        // executed in order
        CArrayFixFlat< TInt >*  iEventQueue;

    };

#endif      // CVTENGEVENTMANAGER_H
            
// End of File
