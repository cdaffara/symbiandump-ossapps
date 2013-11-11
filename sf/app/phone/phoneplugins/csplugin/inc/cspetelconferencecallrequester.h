/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CSPETELCONFERENCECALLREQUESTER_H
#define CSPETELCONFERENCECALLREQUESTER_H


#include <e32base.h>
#include <etelmm.h>
#include <rmmcustomapi.h>

class CSPCall;
class MCCPCallObserver;
class CSPConferenceCall;

/**
* Makes asynchronic request to ETel interface according to given request type.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPEtelConferenceCallRequester
        : public CActive
    {
        
    public:
        /**
        * Supported request types.
        */
        enum TConferenceRequestType {
            EConferenceRequestTypeAddCall,
            EConferenceRequestTypeHangup,
            EConferenceRequestTypeHold,
            EConferenceRequestTypeResume,
            EConferenceRequestTypeCreateConference,
            EConferenceRequestTypeSwap
            };
        
        /**
        * Constructs the requester via two phase.
        *
        * @param aOwner the owner for the requester
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        static CSPEtelConferenceCallRequester* NewL( 
                                           CSPConferenceCall& aOwner, 
                                           RMobileConferenceCall& aCall );
        
        /**
        * C++ default destructor
        */
        virtual ~CSPEtelConferenceCallRequester( );
        
        /**
        * Makes the request. Note that ERequestTypeDial is made with a 
        * separate MakeDialRequest function.
        *
        * @since S60 3.2
        * @param aRequest type of request
        * @return KErrNone if request was sent successfully.
        *         KErrNotSupported if the given request is not supported
        *         KErrArgument if the specified request is not known.
        *         KErrUnknown if unspecified error (should not happen)
        */
        TInt MakeRequest( TConferenceRequestType aRequest );
     

        /**
        * Makes a dial request.
        * @since S60 3.2
        * @param aCallName call name to be added
        * @return KErrNone if successful, else error code
        */         
        TInt MakeAddCallRequest( const TName& aCallName );

    
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
        * Constructs the requester.
        *
        * @param aOwner the owner for the requester
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        CSPEtelConferenceCallRequester( CSPConferenceCall& aOwner, 
                                        RMobileConferenceCall& aCall );
        
        /**
        * Constructing in the second phase.
        */
        void ConstructL();
        
    private: // data
        /**
        * Call object which owns this monitor
        */
        CSPConferenceCall& iOwner;
        
        /**
        * ETel call which this monitors
        */
        RMobileConferenceCall& iCall;
        
        /**
        * Type of request.
        */
        TConferenceRequestType iRequestType;

    };

#endif // CSPETELCONFERENCECALLREQUESTER_H