/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declarations for class MCSPCommonInfo
*
*/



#ifndef MCSPCOMMONINFO_H
#define MCSPCOMMONINFO_H

//  INCLUDES
#include <e32std.h>
#include <etelmm.h> // for ciphering status
#include <rmmcustomapi.h>
#include <mccpcall.h> // For CCCPCallParameters
#include <mccecall.h>


class CSPCall;
class MCCPCallCommandHandling;
class MCCPCSCall;

// CLASS DECLARATION


/**
*  Observer interface for indicating P&S changes
*
*/
class MCSPCommonInfo
    {
    public:  // New functions

        /**
        * Makes the request. Note that ERequestTypeDial is made with a 
        * separate MakeDialRequest function.
        *
        * @param aRequest type of request
        * @return ETrue if active, EFalse if not
        */
        virtual void GetCallWaitingL( const CCCECallParameters& iParams,
                              TBool& aCallWaitingStatus ) = 0;
                              
        /**
        * Diagnostic error fetching.
        * @param aCallName name of call
        * @return diagnostic error code
        */
        virtual TInt GetDiagnosticError( TName& aCallName ) = 0;
       

        /**
        * Network security status.
        * @since Series60_4.0
        * @return true if calls use secured connection
        */
        virtual TBool NetworkSecurityStatus() const = 0;
        
        
        /**
        * Remote alerting tone playing status. 
        * @return tone status (tone type/not played)
        */
        virtual RMmCustomAPI::TRemoteAlertingToneStatus GetRemoteAlertingToneStatus() = 0;
        
        /**
         * Initializes with current settings.
         * @param aParams call params 
         */
        virtual void InitializeCallParameters( RMobileCall::TMobileCallParamsV7& aParams ) = 0;

        /**
         * Initializes with current settings.
         * @param aParams call params 
         */
        virtual void InitializeDataCallParameters( 
                RMobileCall::TMobileHscsdCallParamsV1& aParams ) = 0;
        
        /**
        * Indicates that client call has been created.
        * @param aCall client call pointer
        */
        virtual void IndicateClientCall( MCCPCSCall* aCall ) = 0;
        
        /**
        * Indicates incoming call.
        * @param aCall MT call
        * @return system wide error code
        */
        virtual TInt IndicateIncomingCall( MCCPCSCall* aCall ) = 0;
        
        /**
        * Notify data port name of a loaned data port.
        * @param aDataPort
        */
        virtual void NotifyDataPortName( TName& aDataPortName ) = 0;
        
        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        virtual TInt IndicateActiveHangup( MCCPCallCommandHandling& aCall ) = 0;
        
        /**
        * Indicates hangup command complete.
        * @param aCall MT call
        * @return system wide error code
        */
        virtual TInt IndicateHangupComplete( MCCPCallCommandHandling& aCall ) = 0;
        
        /**
        * Indicates dial command.
        * @param aCall MT call
        * @return system wide error code
        */
        virtual TInt IndicateDialRequest( MCCPCallCommandHandling& aCall ) = 0;
        
        /**
        * Indicates answer command.
        * @param aCall MT call
        * @return system wide error code
        */
        virtual TInt IndicateAnswerRequest( MCCPCallCommandHandling& aCall ) = 0;

        /**
        * Secure specified status.
        * @since Series60_5.0
        * @return ETrue if specified 
        */
        virtual TBool SecureSpecified() const = 0;
        
        /**
        * Don't report termination error
        * @since Series60_5.0
        * @return system wide error code
        */
        virtual TInt DontReportTerminationError() = 0;
        
    };

#endif      // MCSPCOMMONINFO_H   

// End of File
