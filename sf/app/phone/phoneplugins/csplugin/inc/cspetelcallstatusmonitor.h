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
* Description:  Monitors call status changes from ETel
*
*/


#ifndef CSPETELCALLSTATUSMONITOR_H
#define CSPETELCALLSTATUSMONITOR_H

#include <e32base.h>
#include <etelmm.h>

class MCSPCallObserver;


NONSHARABLE_CLASS(CSPEtelCallStatusMonitor) : public CActive
    {
    public: //Constructors and descructor
    
        /**
        * Two-phased constructing for the monitor.
        * @param aObserver observer for call status changes
        * @param aCall actual object of the monitoring
        */
        static CSPEtelCallStatusMonitor* NewL( MCSPCallObserver& aObserver, 
                                               RMobileCall& aCall );
        
        /**
        * Destructor of the monitor which cancels ongoing monitoring.
        */
        virtual ~CSPEtelCallStatusMonitor( );
        
        /**
        * Start monitoring call events
        * @since S60 3.2
        */
        void StartMonitoring();
        
        /**
        * State of call.
        * @return ETel state
        */
        RMobileCall::TMobileCallStatus State();
        
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
        * C++ constructor for the monitor.
        * @param aObserver observer for status changes
        * @param aCall actual object of the monitoring
        */
        CSPEtelCallStatusMonitor( MCSPCallObserver& aObserver, RMobileCall& aCall );
        
    private: // data
        /**
        * Observer for state changes.
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
        * ETel call status
        */
        RMobileCall::TMobileCallStatus iCallStatus;

    };

#endif // CSPETELCALLSTATUSMONITOR_H