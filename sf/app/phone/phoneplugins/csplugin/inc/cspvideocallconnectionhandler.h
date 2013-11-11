/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Delivers asynchronous requests to ETel
*
*/


#ifndef CSPVIDEOCALLCONNECTIONHANDLER_H
#define CSPVIDEOCALLCONNECTIONHANDLER_H


#include <e32base.h>
#include <etelmm.h>
#include <rmmcustomapi.h>
#include <es_sock.h>
#include <es_enum.h>

class CSPVideoCall;

/**
* Handles data call connection specific requests.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPVideoCallConnectionHandler : public CActive
    {
        
    public:
        
        /**
        * Constructs the requester via two phase.
        *
        * @param aObserver the observer for request errors
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        static CSPVideoCallConnectionHandler* NewL( CSPVideoCall& aObserver, 
                                           RMobileCall& aCall );
        
        /**
        * C++ default destructor
        */
        virtual ~CSPVideoCallConnectionHandler( );

        /**
        * Initializes connection including data port loaning.
        * @since S60 3.2
        * @return KErrNone if successful, else error code
        */
        TInt InitializeConnection(  );
        
        /**
        * Releases connection including data port loaning.
        * @since S60 3.2
        * @return KErrNone if successful, else error code
        */        
        TInt ReleaseConnection( );

        /**
        * Connection initialized.
        * @since S60 3.2
        * @return connection initialized status
        */
        TBool ConnectionInitialized(  );
     
    protected: // From CActive
        
        /**
        * From CActive
        * RunL
        * @since S60 3.2
        */
        void RunL();
        
        /**
        * From CActive
        * Cancels the monitor
        * @since S60 3.2
        */
        void DoCancel();
        
    private:
    
        /**
        * Constructs the connection handler.
        *
        * @param aObserver the observer for request errors
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        CSPVideoCallConnectionHandler( CSPVideoCall& aObserver, 
                              RMobileCall& aCall );
        
        /**
        * Constructing in the second phase.
        */
        void ConstructL();

        /**
        * Create socket connection.  
        */
        TInt CreateSocketConnection();

        /**
        * Loan data port. 
        * Initiates asynchronous request to loan dataport. 
        */
        TInt LoanDataPort();
        
        /**
        * Close socket connection.  
        */
        void CloseConnection();
               
        
    private: // data
        /**
        * Observer for request errors.
        */
        CSPVideoCall& iObserver;        
       
        /**
        * ETel call which this monitors
        */
        RMobileCall& iCall;        
        
        /**
        * Comm port info.
        */
        RCall::TCommPort iCommPort;
        
        /**
        * Connection reference
        */
        RConnection iConnection;
        
        /**
        * Tells if socket connection has been initialized
        */
        TBool iConnectionInitialized;
        
        /**
        * Socket Server handle.
        */
        RSocketServ iSocketServer;
        
    };
    
#endif // CSPVIDEOCALLCONNECTIONHANDLER_H
