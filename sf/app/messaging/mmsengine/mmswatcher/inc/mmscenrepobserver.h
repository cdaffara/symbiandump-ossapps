/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class observes given cenrep value.
*
*/



#ifndef CMMSCENREPOBSERVER_H
#define CMMSCENREPOBSERVER_H

//  INCLUDES
#include    <e32base.h>
#include    <centralrepository.h>
#include    <flogger.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* Interface for requster
*/
class MMmsCenRepNotifyHandler
    {  
    public: // New functions              
        /**
        * Callback for timer cancellation
        * @param aRepositoryUid identifies the repository where the key under
        *   observation is located.
        * @param aKey identifies the particular key under observation.
        * @param aValue specifies the new changed value of the key.
        */               
        virtual void HandleCenRepNotificationL(
            const TUid aRepositoryUid,
            const TInt32 aKey, 
            const TInt aValue ) = 0;
    };

/**
* Class for observing a particular CenRep value
*/
class CMmsCenRepObserver : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aRConnection opened RConnection
        */
        static CMmsCenRepObserver* NewL();

        /**
        * Destructor.
        */
        virtual ~CMmsCenRepObserver();

    public: // New functions

        /**
        * Subscribe to a particular key in CenRep
        * @param aRepositoryUid identifies the repository where the key under
        *   observation is located.
        * @param aKey identifies the particular key under observation.
        * @param aRequester pointer to notification handler
        */
        void SubscribeNotification(
            const TUid aRepositoryUid,
            const TInt32 aKey, 
            MMmsCenRepNotifyHandler* aRequester );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Active object cancellation
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMmsCenRepObserver();

        /**
        * By default Symbian OS constructor is private.
        * @param aRConnection opened RConnection
        */
        void ConstructL();

        /**
        * Completion
        */
        void RunL();

        // By default, prohibit copy constructor
        CMmsCenRepObserver( const CMmsCenRepObserver& );

        // Prohibit assignment operator
        CMmsCenRepObserver& operator= ( const CMmsCenRepObserver& );

    private:    // Data

        MMmsCenRepNotifyHandler*   iRequester;
        CRepository*               iRepository;
        TUid                       iRepositoryUid;
        TInt32                     iKey;
    };

#endif      // CMMSCENREPOBSERVER_H

// End of File
