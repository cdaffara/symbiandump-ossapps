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
* Description:  Declarations for class CSPEtelConferenceStatusMonitor
*
*/


#ifndef CSPETELCONFERENCECALLSTATUSMONITOR_H
#define CSPETELCONFERENCECALLSTATUSMONITOR_H

#include <e32base.h>
#include <etelmm.h>


class MCSPConferenceStatusObserver; 

/**
* Monitors single ETel RMobileConferenceCall for conference call statuss.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
NONSHARABLE_CLASS(CSPEtelConferenceStatusMonitor): public CActive
    {
    
    public: //Constructors and descructor
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aPhone the line to monitor
        */
        static CSPEtelConferenceStatusMonitor* NewL( 
                MCSPConferenceStatusObserver& aObserver,
                RMobileConferenceCall& aPhone );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelConferenceStatusMonitor( );

        /**
        * Start monitoring call statuss
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
        * @param aPhone the line associated with the call
        */
        CSPEtelConferenceStatusMonitor( MCSPConferenceStatusObserver& aObserver,
                            RMobileConferenceCall& aPhone );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();

        
    private: // data
        /**
        * Owner.
        */
        MCSPConferenceStatusObserver& iOwner;
       
        /**
        * Phone that is being observed for incoming call.
        */
        RMobileConferenceCall& iCall;
        
        /**
        * Status received.
        */ 
        RMobileConferenceCall::TMobileConferenceStatus iConferenceStatus;
                
        /**
        * Pointer for recovering from a situation where CCE releases call 
        * during observer-call sequence.
        */
        TBool* iDestrPtr;
        
    };

#endif // CSPETELCONFERENCECALLSTATUSMONITOR_H