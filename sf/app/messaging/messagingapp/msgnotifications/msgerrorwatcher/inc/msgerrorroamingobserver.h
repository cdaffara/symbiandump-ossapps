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
*     CMsgErrorRoamingObserver declaration file
*
*/



#ifndef MSGERRORROAMINGOBSERVER_H
#define MSGERRORROAMINGOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <etel.h>                   //RTelServer
#include <etelmm.h>



// FORWARD DECLARATIONS
class CMsgErrorWatcherPrivate;

// CLASS DECLARATION

/**
* CMsgErrorRoamingObserver
*/
class CMsgErrorRoamingObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        */
        static CMsgErrorRoamingObserver* NewL( CMsgErrorWatcherPrivate& aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorRoamingObserver();

    private:

        /**
        * Checks whether roaming status has changed
        * from home network to roaming or vice versa.
        * If so, calls CMsgErrorWatcherPrivate::HandleRoamingEventL
        */
        void UpdateRoamingStatusL();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcherPrivate
        */
        CMsgErrorRoamingObserver( CMsgErrorWatcherPrivate& aWatcher );

    private:

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

        CMsgErrorWatcherPrivate&   iWatcher;
        TBool               iRoaming;
        RMobilePhone        iMobilePhone;
        RTelServer          iETelServer;
        RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus;
        TInt                iRequestId;

    };

#endif      // MSGERRORROAMINGOBSERVER_H
            
// End of File
