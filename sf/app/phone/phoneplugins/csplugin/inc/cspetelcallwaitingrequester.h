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
* Description:  Fetches call waiting status from ETel
*
*/


#ifndef CSPETELCALLWAITINGREQUESTER_H
#define CSPETELCALLWAITINGREQUESTER_H


#include <e32base.h>
#include <mccpcscall.h>
#include <mccecall.h>
#include <etelmm.h>

class RMobilePhone;
class CRetrieveMobilePhoneCWList;

/**
* Makes asynchronic request to ETel interface according to given request type.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPEtelCallWaitingRequester
        : public CActive
    {
        
    public:
        
        /**
        * Constructs the requester via two phase.
        *
        * @param aPhone mobile phone reference needed for reading cw setting
        * @return requester object
        */
        static CSPEtelCallWaitingRequester* NewL( RMobilePhone& aPhone );
        
        /**
        * C++ default destructor
        */
        virtual ~CSPEtelCallWaitingRequester( );
        
        /**
        * Makes the request. Note that ERequestTypeDial is made with a 
        * separate MakeDialRequest function.
        *
        * @since S60 3.2
        * @param aRequest type of request
        * @return ETrue if active, EFalse if not
        */
        void GetCallWaitingL( const CCCECallParameters& iParams,
                              TBool& aCallWaitingStatus );
        

    
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
        * Requester can be inactive (from boot to first Cw request),
        * requesting value from network 
        *       (with first MT call that gets Queue-request from CCE),
        * or monitoring (after first request).
        */        
        enum ECSPCwRequesterMode {
            ECwRequesterInactive,
            ECwRequesterGetStarted,
            ECwRequesterMonitoring
        };
        
    private:
    
        /**
        * Constructor.
        * @param aPhone mobile phone reference needed for reading cw setting
        *
        */
        CSPEtelCallWaitingRequester(RMobilePhone& aPhone);
                              
        /**
        * Constructs the requester.
        *
        * @param aPhone phone reference
        */
        void ConstructL();
        
        /**
        * Starts monitoring call waiting setting.
        */
        void StartMonitoring();
        
        /**
        * Returns the call waiting status from call waiting status list by linetype.
        */
        void GetCallWaitingStatusByLineType( const CCCECallParameters& iParams,
                                             TBool& aCallWaitingStatus );
        
        
    private: // data
        
        /**
        * List of call waiting settings.
        * Own.
        */        
        CRetrieveMobilePhoneCWList* iList;
        
        /**
        * Making the asynchronous request synchronous.
        */ 
        CActiveSchedulerWait iWait;
        
        /**
        * Phone handle for listening cw status changes.
        */
        RMobilePhone& iPhone;
        
        /**
        * Cw status descriptor for reading changed cw status.
        */
        TPckgBuf<RMobilePhone::TMobilePhoneCWInfoEntryV1> iDesCwStatus;
        
        /**
        * Current mode according to the enumeration.
        */
        ECSPCwRequesterMode iMode;
        
        /**
        * Actual call waiting status for primary line: On/Off = True/False.
        */
        TBool iPrimaryLineCwStatus;
        
        /**
        * Actual call waiting status for aux line: On/Off = True/False.
        */
        TBool iAuxLineCwStatus;
    };

#endif // CSPETELCALLWAITINGREQUESTER_H
