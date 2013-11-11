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


#ifndef CSPETELCALLREQUESTER_H
#define CSPETELCALLREQUESTER_H


#include <e32base.h>
#include <etelmm.h>
#include <rmmcustomapi.h>
#include <ccpdefs.h>

class MCSPCallErrorObserver;

/**
* Makes asynchronic request to ETel interface according to given request type.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPEtelCallRequester
        : public CActive
    {
        
    public:
        /**
        * Supported request types.
        */
        enum TRequestType {
            ERequestTypeNone,
            ERequestTypeDial,
            ERequestTypeAnswer,
            ERequestTypeHangup,
            ERequestTypeHold,
            ERequestTypeResume,
            ERequestTypeGoOneToOne,
            ERequestTypeSwap,
            ERequestTypeDialEmergency
            };
        
        /**
        * Constructs the requester via two phase.
        *
        * @param aObserver the observer for request errors
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        static CSPEtelCallRequester* NewL( MCSPCallErrorObserver& aObserver, 
                                           RMobileCall& aCall );
        
        /**
        * C++ default destructor
        */
        virtual ~CSPEtelCallRequester( );
        
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
        TInt MakeRequest( TRequestType aRequest );
        
        /**
        * Cancels the request. 
        *
        * @since S60 3.2
        * @param aRequest type of request
        * @return KErrNone if request was sent successfully.
        *         KErrNotSupported if the given request is not supported
        *         KErrArgument if the specified request is not known.
        *         KErrUnknown if unspecified error (should not happen)
        */
        TInt CancelRequest( TRequestType aRequest );
     

        /**
        * Makes a dial request.
        * @since S60 3.2
        * @param aCallParamsPckg call parametrs
        * @param aRemoteParty phone number
        * @return KErrNone if successful, else error code
        */         
        TInt MakeDialRequest( 
            const TDesC8& aCallParams, 
            const TDesC& aRemoteParty );

        /**
        * Makes a dial request without FDN check.
        * @since S60 3.2
        * @param aCallParamsPckg call parametrs
        * @param aRemoteParty phone number
        * @return KErrNone if successful, else error code
        */         
        TInt MakeDialNoFdnCheckRequest( 
            const TDesC8& aCallParams, 
            const TDesC& aRemoteParty );
            
        
        /**
        * Makes a answer request.
        * @since S60 3.2
        * @param aCallParams call parametrs
        * @return KErrNone if successful, else error code
        */         
        TInt MakeAnswerRequest( 
            const TDesC8& aCallParams );

        /**
        * Makes emergency request.
        * @param aRecipient emengency number
        * @return error code
        */
        TInt MakeDialEmergencyRequest( 
            const RMobileENStore::TEmergencyNumber& aRecipient );
            
        /**
        * Mapping between ETel and CCP errors.
        * @param aErrorCode ETel error code
        * @return CCP error code
        */ 
        TCCPError MapError( TInt aErrorCode );
        
        /**
        * Getter for the active request.
        * @return request.
        */
        TRequestType Request();
    
        /**
        * Cancel dial (applies both emergency and normal calls)
        * @return result of cancel (system wide error code)
        */
        TInt DialCancel();
        
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
        * @param aObserver the observer for request errors
        * @param aCall ETel RMobileCall reference that is the object
        * of the request.
        */
        CSPEtelCallRequester( MCSPCallErrorObserver& aObserver, 
                              RMobileCall& aCall );
        
        
    private: // data
        /**
        * Observer for request errors.
        */
        MCSPCallErrorObserver& iObserver;
        
        /**
        * ETel call which this monitors
        */
        RMobileCall& iCall;
        
        /**
        * Type of request.
        */
        TRequestType iRequestType;

    };

#endif // CSPETELCALLREQUESTER_H

