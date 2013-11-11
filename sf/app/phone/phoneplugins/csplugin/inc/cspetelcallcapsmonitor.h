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


#ifndef CSPETELCALLCAPSMONITOR_H
#define CSPETELCALLCAPSMONITOR_H

#include <e32base.h>
#include <etelmm.h>


class MCSPCallObserver;


/**
* Monitors single ETel RMobileCall for call capss.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelCallCapsMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aCall the line to monitor
        */
        static CSPEtelCallCapsMonitor* NewL( 
                MCSPCallObserver& aObserver,
                RMobileCall& aCall );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelCallCapsMonitor( );

        /**
        * Start monitoring call capss
        * @since S60 3.2
        */
        void StartMonitoring();
        
        /**
         * Fetches call control caps.
         * @return call control caps flags.
         */
        TUint32 FetchCallControlCapsL();
    
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
        * @param aObserver the observer for status change (incoming call)
        * @param aCall the line associated with the call
        */
        CSPEtelCallCapsMonitor( MCSPCallObserver& aObserver,
                            RMobileCall& aCall );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();

        
    private: // data
        /**
        * Forwards caps change.
        */
        MCSPCallObserver& iObserver;
       
        /**
        * Call that is being observed.
        */
        RMobileCall& iCall;
        
        /**
        * Call capabilities.
        */
        RMobileCall::TMobileCallCapsV1 iCaps;
        RMobileCall::TMobileCallCapsV1Pckg iCapsPckg;
      

        
    };

#endif // CSPETELCALLCAPSMONITOR_H