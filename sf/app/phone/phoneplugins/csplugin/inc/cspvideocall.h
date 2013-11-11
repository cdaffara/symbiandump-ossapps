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
* Description:  Implements CSPVideoCall for CS Call Plug-in
*
*/



#ifndef CSPVIDEOCALL_H
#define CSPVIDEOCALL_H

#include <mccpcscall.h>
#include <mccpcallobserver.h>
#include <etel.h>
#include <etelmm.h>
#include <cccecallparameters.h>

#include "cspcall.h"
#include "cspconsts.h"
#include "mcspcallobserver.h"
#include "mcspcallerrorobserver.h"
#include "cspvideocallconnectionhandler.h"


class MCCPObserver;
class RMobileCall;
class CSPEtelCallStatusMonitor;
class CSPEtelCallEventMonitor;
class CSPEtelVideoCallCapsMonitor;
class CSPEtelCallWaitingRequester;

/**
* Call class for CS Call Plug-in. Implements the API defined by CCP in classes
* MCCPCall and MCCPCSCall
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPVideoCall : public CSPCall
    {
    
    public:
        /*
        * Creates a new CSPVideoCall object.
        * @since S60 3.2
        * @param aName dialled number (MT) or call name (MO)
        * @param aLine line for opening the call
        * @param aMobileOriginated direction of call        
        * @param aParams call parameters 
        * @param aCommonInfo common information for call,
        * @return pointer to created call object
        */
        static CSPVideoCall* NewL( const TDesC& aName,
                              RMobileLine& aLine,
                              TBool aMobileOriginated,
                              const CCCECallParameters& aParams,
                              MCSPCommonInfo& aCommonInfo );       
        
        /**
        * C++ default destructor
        */
        virtual ~CSPVideoCall( );

        /**
        * Informs about ready data connection.
        * @aPortName name of data port
        */
        void ConnectionReady( TName& aDataPortName );
        
        /**
        * Error occurred when establishing connection.
        * @param aErr error value
        */
        void ConnectionEstablishingFailed(TInt aErr);
        
        /**
        * Notification about data call caps change.
        * @param aCaps new caps
        */
        void DataCallCapsChanged( RMobileCall::TMobileCallDataCapsV1& aCaps );
        
        /**
        * Selects call type video or voice depending of available procotol 
        * capabilities
        * @return Call type video or voice  
        */
        CCPCall::TCallType SelectCallTypeFromProtocolCaps();          
        
// from base class CSPCall

        /**
        * From CSPCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        virtual void NotifyCallStateChanged( MCCPCallObserver::TCCPCallState aState );

        /**
        * From CSPCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        virtual void NotifyCallStateChangedWithInband( MCCPCallObserver::TCCPCallState aState );
        
        /**
        * From MCCPCSCall
        * Starts dialing to recipient without FDN check.
        * @param aCallParams Call parameters used by the TSY (TCallParamsPckg)
        * @return system wide error code
        */
        TInt DialNoFdnCheck( const TDesC8& aCallParams );

        /**
        * From CSPCall
        * Switch in the alternating call mode of an alternating call
        * @since S60 3.2
        */
        TInt SwitchAlternatingCall();
        
        /**
        * From MCCPCSCall
        * Gets the calls data call capabilities
        * @since S60 3.2
        * @param aCaps TMobileCallDataCapsV1
        * @return KErrNone if the function member was successful, 
        *        KErrNotSupported if call doesn't support circuit switched data
        *        KErrNotFound if this call is not a data call
        */
        virtual TInt GetMobileDataCallCaps( TDes8& aCaps ) const;

        /**
        * From MCCPCall
        * Get call's secure status.
        * @since S60 3.2
        * @return TBool ETrue if security is enabled, else EFalse.
        */
        TBool IsSecured() const;
          
    protected: 
        
        /*
        * C++ default constructor
        * @param aLine open line ready for use
        * @param aMobileOriginated true if MO, false if MT
        * @param aName call name (MT) or recipient (MO)
        * @param aCommonInfo reference to common info interface
        */
        CSPVideoCall( RMobileLine& aLine,  
                 TBool aMobileOriginated,
                 const TDesC& aName,
                 MCSPCommonInfo& aCommonInfo );

        /*
        * Constructing 2nd phase.
        * @since S60 3.2
        */
        void ConstructL( const CCCECallParameters& aParams );

    private:    

        /** 
        * Sets default call params.
        */
        void SetDefaultCallParameters();
        
        /** 
        * Sets default video call params
        */
        void SetVideoCallParameters();
        
        /**
        * Branches dial requests according 
        * to FDN check flag.
        * @aFdnCheck ETrue if FDN is checked
        */
        TInt DialFdnCond( TBool aFdnCheck );
  
    private:

        /**
        * Data call caps monitor
        * Own.
        */
        CSPEtelVideoCallCapsMonitor* iVideoCallCapsMonitor;

        /**
        * Call parameters. 
        */
        RMobileCall::TMobileHscsdCallParamsV1 iDataCallParams;

        /**
        * Call parameters package.  
        */
        RMobileCall::TMobileHscsdCallParamsV1Pckg iDataCallParamsPckg;
      
        /**
        * Air Interface User Rate
        */
        RMobileCall::TMobileCallAiur iAirInterfaceUserRate;
        
        /**
        * Current received time slots
        */
        TInt iCurrentTimeSlots;
                
        /**
        * Connection functionality
        * Own.
        */
        CSPVideoCallConnectionHandler* iConnectionHandler;

        /**
        * Next call state 
        */
        MCCPCallObserver::TCCPCallState iNextCallState;       
        
    };


#endif // CSPVIDEOCALL_H
