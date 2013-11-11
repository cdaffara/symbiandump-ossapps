/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements CSPClientVideoCall for CS Call Plug-in
*
*/


#ifndef CSPCLIENTVIDEOCALL_H
#define CSPCLIENTVIDEOCALL_H

#include <mccpcscall.h>
#include <mccpcallobserver.h>
#include <etel.h>
#include <etelmm.h>
#include <cccecallparameters.h>

#include "cspvideocall.h"
#include "cspconsts.h"
#include "mcspcallobserver.h"
#include "mcspcallerrorobserver.h"


/**
* Call class for CS Call Plug-in. Implements the API defined by CCP in classes
* MCCPCall and MCCPCSCall
*
*  @lib csplugin.dll
*  @since S60 v5.0
*/
class CSPClientVideoCall : public CSPVideoCall
    {
    
    public:
        
        /*
        * Creates a new CSPClientVideoCall object.
        * @since S60 5.0
        * @param aName dialled number (MT) or call name (MO)
        * @param aLine line for opening the call
        * @param aParams call parameters 
        * @param aCommonInfo common information for call,
        * @return pointer to created call object
        */
        static CSPClientVideoCall* NewL( const TDesC& aName,
                              RMobileLine& aLine,
                              const CCCECallParameters& aParams,
                              MCSPCommonInfo& aCommonInfo );       
        
        /**
        * C++ default destructor
        */
        virtual ~CSPClientVideoCall( );

// from base class CSPVideoCall

        /**
        * From CSPCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        void NotifyCallStateChanged( MCCPCallObserver::TCCPCallState aState );

        /**
        * From CSPCall
        * Updates the state change and forwards the state for observer.
        * @param aState new state of the call
        */
        void NotifyCallStateChangedWithInband( MCCPCallObserver::TCCPCallState aState );
      
    private:    
        
        /*
        * C++ default constructor
        * @param aLine open line ready for use
        * @param aServiceId service ID
        * @param aCallType type of call
        * @param aLineType type of line (primary/aux)
        * @param aMobileOriginated true if MO, false if MT
        * @param aName call name (MT) or recipient (MO)
        * @param aExtMoCall is externally created MO call
        */
        CSPClientVideoCall( RMobileLine& aLine,
                 const TDesC& aName,
                 MCSPCommonInfo& aCommonInfo );
                 
        /*
        * Constructing 2nd phase.
        * @since S60 3.2
        */
        void ConstructL( const CCCECallParameters& aParams );
        
        /*
        * Indicate client call. 
        * Indication is done in Dialling state and can be done only once 
        */
        void IndicateClientCall(); 
        
        /**
        * Open call handle.  
        * Opens an existing handle for client call.    
        */
        void OpenCallHandleL();

        /**
        * Update call info.  
        */
        void UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo ); 
        
    private: // Data
        
        /**
        * Defines whether client call has been already indicated for this call 
        * when Dialling state is entered. 
        */
        TBool iClientCallIndicated;
        
    };


#endif // CSPCLIENTVIDEOCALL_H
