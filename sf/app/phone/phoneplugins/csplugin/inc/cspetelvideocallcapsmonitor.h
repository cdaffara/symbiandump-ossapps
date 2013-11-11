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
* Description:  Declarations for class CSPEtelCallCapsMonitor
*
*/


#ifndef CSPETELDATACALLCAPSMONITOR_H
#define CSPETELDATACALLCAPSMONITOR_H

#include <e32base.h>
#include <etelmm.h>


class CSPVideoCall;


/**
* Monitors single ETel RMobileCall for call capss.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelVideoCallCapsMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aOwner the observer for getting notification
        * @param aCall the line to monitor
        */
        static CSPEtelVideoCallCapsMonitor* NewL( 
                CSPVideoCall& aOwner,
                RMobileCall& aCall );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelVideoCallCapsMonitor( );

        /**
        * Start monitoring call capss
        * @since S60 3.2
        */
        void StartMonitoring();
        
        /**
        * Getter for fetching mobile data call caps.
        */
        TInt GetMobileDataCallCaps( TDes8& aCaps ) const;
        
        /**
        * Gets protocol caps.
        * @return protocol caps 
        */ 
        TUint GetProtocolCaps();
    
        /**
        * Updates protocol caps for call
        * @since S60 5.0
        */ 
        void UpdateProtocolCaps(); 
    
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
        * C++ default constructor
        * @param aOwner the observer for status change (incoming call)
        * @param aCall the line associated with the call
        */
        CSPEtelVideoCallCapsMonitor( CSPVideoCall& aOwner,
                            RMobileCall& aCall );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();

        
    private: // data
        /**
        * Forwards capss.
        */
        CSPVideoCall& iOwner;
       
        /**
        * Call that is being observed.
        */
        RMobileCall& iCall;
        
        /**
        * Call capabilities.
        */
        RMobileCall::TMobileCallDataCapsV1 iDataCallCaps;
        RMobileCall::TMobileCallDataCapsV1Pckg iDataCallCapsPckg;
              
    };
    
#endif // CSPETELDATACALLCAPSMONITOR_H
