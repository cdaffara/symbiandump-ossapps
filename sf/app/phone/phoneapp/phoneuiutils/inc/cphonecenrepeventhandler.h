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
* Description:  Central Repository event handler 
*
*/


#ifndef __CPHONECENREPEVENTHANDLER_H
#define __CPHONECENREPEVENTHANDLER_H

// INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Class for observing central repository variable changes.
*/

class CPhoneCenRepEventHandler : 
    public CBase, 
    public MCenRepNotifyHandlerCallback
    {
    public: // Constructors and destructor
        /**
        * Default constructor
        */
        static CPhoneCenRepEventHandler* NewL( const TUid& aUid );

        /**
        * Destructor.
        */
        virtual ~CPhoneCenRepEventHandler();

    private:

        /**
        * EPOC default constructor.
        * @return void
        */
        void ConstructL();
        
        /**
        * Constructor
        * @param aUid identifing the central repository UID.
        * @return void
        */         
        CPhoneCenRepEventHandler( const TUid& aUid );
    
        /**
        * C++ default constructor.
        * @param None
        * @return void
        */
        CPhoneCenRepEventHandler();
        
    public: // From MCenRepNotifyHandlerCallback

        /**
        * Notify the client about generic changes for the central repository
        * key.
        * @param aId Id of the key that has changed.
        * @return none
        */    
        void HandleNotifyGeneric( TUint32 aId );

        /**
        * Notify the client about errors in the handler. Any error in 
        * handling causes the handler to stop handling any more notifications. 
        * Handling can be restarted with a call to aHandler->StartListeningL(),
        * if the error is non-fatal. However, be careful to trap any errors 
        * from this call if this is done.
        * @param aId Id of the key this instance listens for or if 
        * notifications for whole repository are listened, could also be 
        * KInvalidNotificationId.
        * @param aError Error code.
        * @param aHandler Pointer to the handler instance. This pointer can 
        *        be used to identify the handler or restart the listening.
        * @return none
        */    
        void HandleNotifyError( 
            TUint32 aId, 
            TInt error, 
            CCenRepNotifyHandler* aHandler );

    private: // Data

        TUid                  iUid;
        CRepository*          iRepository;
        CCenRepNotifyHandler* iCenRepNotifyHandler;
    };

#endif      // __CPHONECENREPEVENTHANDLER_H
            
// End of File
