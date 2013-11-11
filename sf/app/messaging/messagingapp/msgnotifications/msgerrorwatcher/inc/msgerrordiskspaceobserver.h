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
*     CMsgErrorDiskSpaceObserver declaration file
*
*/



#ifndef MSGERRORDISKSPACEOBSERVER_H
#define MSGERRORDISKSPACEOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcherPrivate;
class CMsvSession;

// CLASS DECLARATION

/**
* CMsgErrorDiskSpaceObserver
*/
class CMsgErrorDiskSpaceObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        * @param aSession A reference to message server session
        * @param aFs A reference to file session
        */
        static CMsgErrorDiskSpaceObserver* NewL( CMsgErrorWatcherPrivate& aWatcher, CMsvSession& aSession, RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorDiskSpaceObserver();

    public: // New functions

        /**
        * Updates the trigger limit to aLimit, if it
        * is smaller than current limit. (Re)activates
        * the active object.
        *
        * @param aLimit The trigger limit in bytes
        */
        void SetLimitAndActivateL( TInt32 aLimit );

    private:

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        * @param aSession A reference to message server session
        * @param aFs A reference to file session
        */
        CMsgErrorDiskSpaceObserver( CMsgErrorWatcherPrivate& aWatcher, CMsvSession& aSession, RFs& aFs );

    private:

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:    // Data

        CMsgErrorWatcherPrivate&   iWatcher;
        CMsvSession&        iSession;
        RFs&                iFs;
        TInt32              iLimit;
    };

#endif      // MSGERRORDISKSPACEOBSERVER_H
            
// End of File
