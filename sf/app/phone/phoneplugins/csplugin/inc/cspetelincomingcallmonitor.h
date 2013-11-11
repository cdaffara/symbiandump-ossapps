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
* Description:  Declarations for class CSPEtelIncomingCallMonitor
*
*/


#ifndef CSPETELINCOMINGCALLMONITOR_H
#define CSPETELINCOMINGCALLMONITOR_H

#include <e32base.h>
#include <etelmm.h>

#include "mcspincomingcallobserver.h"
#include "rcsplinecontainer.h"

class CSPCall;
class MCCPCallObserver;



/**
* Monitors single ETel RMobileLine for incoming calls.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelIncomingCallMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aLine the line to monitor
        * @param aLineId line identifier
        */
        static CSPEtelIncomingCallMonitor* NewL( 
                MCSPIncomingCallObserver& aObserver, 
                RMobileLine& aLine,
                RCSPLineContainer::TCSPLineId aLineId );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelIncomingCallMonitor( );

        /**
        * Start monitoring call events
        * @since S60 3.2
        */
        void StartMonitoring();
    
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
        * @param aLine the line associated with the call
        * @param aLineId line identifier
        */
        CSPEtelIncomingCallMonitor( MCSPIncomingCallObserver& aObserver,
                                  RMobileLine& aLine,
                                  RCSPLineContainer::TCSPLineId aLineId );
        
    private: // data
        /**
        * Forwards the incoming call information.
        */
        MCSPIncomingCallObserver& iIncomingCallObserver;

        /**
        * Incoming call name received from ETel.
        */ 
        TName iCallName;
        
        /**
        * Line that is being observed for incoming call.
        */
        RMobileLine& iLine;
        
        /**
        * Line identifier / type.
        */
        RCSPLineContainer::TCSPLineId iLineId;
    };
    
#endif // CSPETELINCOMINGCALLMONITOR_H