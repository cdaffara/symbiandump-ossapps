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
*     CMsgErrorSmsDiskSpaceObserver declaration file
*
*
*/


#ifndef __MSGERROREXTSMSDISKSPACEOBSERVER_H
#define __MSGERROREXTSMSDISKSPACEOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcherPrivate;

// CLASS DECLARATION

/**
* CMsgErrorSmsDiskSpaceObserver
*
* @lib msgerrorwatcher.dll
* @since 5.0
*/
class CMsgErrorExtSmsDiskSpaceObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        */
        static CMsgErrorExtSmsDiskSpaceObserver* NewL( CMsgErrorWatcherPrivate& aWatcher);
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorExtSmsDiskSpaceObserver();

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
        CMsgErrorExtSmsDiskSpaceObserver( CMsgErrorWatcherPrivate& aWatcher);

    private:

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
         * Reference to watcher
         * Doesn't own
         */
        CMsgErrorWatcherPrivate&   iWatcher;
        
        /*
         * Diskspace property
         */
        RProperty iSmsDiskSpaceProperty; 

    };

#endif      // __MSGERROREXTSMSDISKSPACEOBSERVER_H
            
// End of File
