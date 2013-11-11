/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Accessory handler.
*
*/



#ifndef CVTENGACCHANDLER_H
#define CVTENGACCHANDLER_H

//  INCLUDES
#include    <e32base.h>
#include    <AccPolAccessoryMode.h>
#include    <AccessoryServer.h>
#include    <AccessoryMode.h>

// FORWARD DECLARATIONS
class MVtEngAccessoryObserver;

// CLASS DECLARATION

/**
*  Accessory mode listener for video telephony.
*
*  @lib videoteleng
*  @since 3.0
*/
NONSHARABLE_CLASS( CVtEngAccessoryHandler ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        ? @return instance of CVtEngAccessoryHandler
        */
        static CVtEngAccessoryHandler* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngAccessoryHandler();

    public: // From base classes.

        /**
        * Handles accessory mode changes.
        */
        void RunL();

        /**
        * Cancels accessory mode monitoring.
        */
        void DoCancel();

        /**
        * Handles exceptions in RunL.
        * @param aError error code.
        * @return error code.
        */
        TInt RunError( TInt aError );

    public: // New functions

        /**
        * Sets a new listener interested in accessory changes.
        * @param aObserver call back interface for notifications.
        */
        void RegisterObserverL( MVtEngAccessoryObserver* aObserver );

        /**
        * Unregisters a listener.
        * NOTE, do not call this from NotifyObserversL!
        * @param aObserver call back interface for notifications.
        */
        void RemoveObserver( MVtEngAccessoryObserver* aObserver );

        /**
        * Returns current accessory mode.
        * @param aAccessoryMode accessory mode in return
        * @return Symbian error code
        */
        TInt AccessoryMode( TAccPolAccessoryMode& aAccessoryMode ) const;

    private: // New functions.
    
        /**
        * Removes all observers
        */
        void RemoveAllObservers();

        /**
        * Sets object active and renews subscription.
        */
        void StartListening();

        /**
        * Notifies all registered listeners.
        */
        void NotifyObserversL();

    private: // Constructors.

        /**
        * C++ default constructor.
        */
        CVtEngAccessoryHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
    
    private:    // Data
        // Current accessory mode.
        TAccPolAccessoryMode iCurrentAccessoryMode;

        // Owned array of accessory mode observers.
        RPointerArray< MVtEngAccessoryObserver > iAccessoryObservers;

        // Owned accessory server session.
        RAccessoryServer iAccessoryServer;

        // Owned accessory mode subsession.
        RAccessoryMode iAccessoryMode;

        // Accessory mode structure.
        TAccPolAccessoryMode iMode;

    };

#endif      // CVTENGACCHANDLER_H
// End of File
