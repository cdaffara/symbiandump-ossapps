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
* Description:  Implements CSPVoiceCall for CS Call Plug-in
*
*/


#ifndef CSPVOICECALL_H
#define CSPVOICECALL_H


#include <mccpemergencycall.h> // For MCCPEmergencyCall

#include "cspcall.h" // For CSPCall


class MCCPForwardProvider;
class MCCPObserver;
class RMobileCall;
class CSPEtelCallStatusMonitor;
class CSPEtelCallEventMonitor;
class CSPEtelCallWaitingRequester;

/**
* Call class for CS Call Plug-in. Implements the API defined by CCP in classes
* MCCPCall and MCCPCSCall
*
*  @lib csplugin.dll
*/
class CSPVoiceCall : public CSPCall,
                     public MCCPEmergencyCall
    {
    
    public:
        /*
        * Creates a new CSPVoiceCall object for MO or MT call.
        * @param aName dialled number (MO) or call name (MT)
        * @param aLine line for opening the call
        * @param aMobileOriginated direction of call
        * @param aParams call parameters
        * @param aCommonInfo common info interface  
        * @param aIsEmergency is emergency call
        * @return pointer to created call object
        */
        static CSPVoiceCall* NewL( const TDesC& aName,
                              RMobileLine& aLine,
                              TBool aMobileOriginated,
                              const CCCECallParameters& aParams,
                              MCSPCommonInfo& aCommonInfo,
                              TBool aIsEmergency );

        /**
        * C++ default destructor
        */
        virtual ~CSPVoiceCall();
        
        /**
        * Goes one-to-one from conference.
        * @return system wide error code        
        */
        TInt GoOneToOne();

// from base class CSPCall        
        
        /**
        * From MCCPCSCall
        * Switch in the alternating call mode of an alternating call
        * @return system wide error code
        */
        TInt SwitchAlternatingCall();
        
        /**
        * From MCCPCall
        * Answer call.
        * @return system wide error code
        */
        TInt Answer();
        
        /**
        * Ends an ongoing call.
        * @return KErrNone If request was started succesfully.
        * @return KErrAlreadyExists If call is already in idle state.
        */
        TInt HangUp();
    
        /**
        * From MCCPCall
        * Cancels the ongoing dial request
        * @return system wide error code
        */
        TInt Cancel();
        
// from base class MCCPEmergencyCall
        
         /**
        * Dials an emergency call.
        * @param aRecipient Emergency call address suggested to be used.
        * Can be given also in CConvergedCallProvider::NewEmergencyCallL.
        * Plug-in will use given address and/or its own known one. 
        * Preferred address usage order:
        * 1) Plug-in own emergency address
        * 2) Address given in Dial
        * 3) Address given in CConvergedCallProvider::NewEmergencyCallL.
        * @return KErrNone if request was started succesfully. 
        * @return KErrNotReady If call is not in idle state.
        * @return KErrNotSupported Emergency call is not supported.
        */
        TInt Dial( const TDesC& aEmergencyNumber);

    protected:
        
        /*
        * Voice call constructor.
        * @param aLine open line ready for use
        * @param aServiceId service ID
        * @param aCallType type of call (voice/video/data/fax)
        * @param aLineType primary/aux line type
        * @param aMobileOriginated direction of call
        * @param aName remote party or call name depending on direction
        * @param aExtMoCall true in case of externally created MO call
        * @param aIsEmergency is emergency call flag
        * @param aBearer bearer capabilities for SAT
        * @param aSubAddress sub adrress for SAT
        * @param aOrigin call origin (i.e. SAT or Phone)
        */
        CSPVoiceCall( RMobileLine& aLine,  
                 TBool aMobileOriginated,
                 const TDesC& aName,
                 MCSPCommonInfo& aCommonInfo,
                 TBool aIsEmergency );
                 
        /*
        * Constructing 2nd phase.
        */
        void ConstructL( const CCCECallParameters& aParams );

        /** 
        * Set default call parameters to be used in dial.
        */
        void SetDefaultCallParameters();

        /**
        * From CSPCall. 
        * Starts dialing to recipient.
        * Bypasses FDN check according to aFdnCheck.
        * @param aFdnCheck True if FDN checking is used
        * @return system wide error code
        */
        TInt DialFdnCond( TBool aFdnCheck );
 
    protected: // Data
        
        /**
        * Call parametrs.
        */
        RMobileCall::TMobileCallParamsV7 iCallParams;
      
        /**
        * Package of call paremeters.
        */
        RMobileCall::TMobileCallParamsV7Pckg iCallParamsPckg;
      
        /**
        * Stores the emergency number.
        */
        RMobileENStore::TEmergencyNumber iEmergencyNumber;
        
        friend class UT_CSPVoiceCall;
        
    };


#endif // CSPVOICECALL_H
