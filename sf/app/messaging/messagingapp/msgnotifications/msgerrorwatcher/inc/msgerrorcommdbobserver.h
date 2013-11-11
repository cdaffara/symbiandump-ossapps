/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     CMsgErrorCommDbObserver declaration file
*
*/



#ifndef MSGERRORCOMMDBOBSERVER_H
#define MSGERRORCOMMDBOBSERVER_H

#include    <e32base.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcherPrivate;
class CCommsDatabase;

// CLASS DECLARATION

/**
* CMsgErrorCommDbObserver
*/
class CMsgErrorCommDbObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        */
        static CMsgErrorCommDbObserver* NewL( CMsgErrorWatcherPrivate& aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorCommDbObserver();

        /**
        * For external reactivating the active object.
        */
        void Restart();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        */
        CMsgErrorCommDbObserver( CMsgErrorWatcherPrivate&  aWatcher );

    private: // from CActive

        /**
        * For starting the active object.
        */
        void Start();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:    // Data

        /*
         * Pointer to commdb
         * Own
         */
        CCommsDatabase*     iDb;
        
        /**
         * Reference to watcher to give call back
         * Doesn't own
         */
        CMsgErrorWatcherPrivate&   iWatcher;
    };

#endif      // MSGERRORCOMMDBOBSERVER_H
            
// End of File
