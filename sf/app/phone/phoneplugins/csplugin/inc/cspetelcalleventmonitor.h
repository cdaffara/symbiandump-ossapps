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
* Description:  Monitors call events from ETel
*
*/


#ifndef CSPETELCALLEVENTMONITOR_H
#define CSPETELCALLEVENTMONITOR_H

#include <e32base.h>
#include <etelmm.h>

class CSPCall;
class MCCPCallObserver;
class MCSPCallObserver;

class CSPEtelCallEventMonitor
        : public CActive
    {
    public: //Constructors and descructor

        /**
        * Two-phased constructing for the monitor.
        * @param aObserver the observer for events
        * @param aCall actual object of the monitoring
        */
        static CSPEtelCallEventMonitor* NewL( MCSPCallObserver& aObserver,
                                              RMobileCall& aCall );
        
        /**
        * C++ default destructor
        */
        virtual ~CSPEtelCallEventMonitor( );
        
        /**
        * Start monitoring call events
        */
        void StartMonitoring();
    
    protected: // From CActive

        /**
        * From CActive
        * RunL
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
        * C++ constructor for the monitor.
        * @param aObserver observers events
        * @param aCall actual object of the monitoring
        */
        CSPEtelCallEventMonitor( MCSPCallObserver& aObserver, 
                                 RMobileCall& aCall );
        
        
    private: // data
    
        /**
        * Observer for call events.
        */
        MCSPCallObserver& iObserver;
        
        /**
        * ETel call which this monitors
        */
        RMobileCall& iCall;
        
        /**
        * Pointer for recovering from a situation where CCE releases call 
        * during observer-call sequence.
        */
        TBool* iDestrPtr;


    protected:
        /**
        * Call event
        */
        RMobileCall::TMobileCallEvent iCallEvent;

    };

#endif // CSPETELCALLEVENTMONITOR_H