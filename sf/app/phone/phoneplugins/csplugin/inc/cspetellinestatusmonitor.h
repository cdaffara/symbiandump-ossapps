/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declarations for class CSPEtelLineStatusMonitor
*
*/


#ifndef CSPETELLINESTATUSMONITOR_H_
#define CSPETELLINESTATUSMONITOR_H_

#include <e32base.h>
#include <etelmm.h>
#include "rcsplinecontainer.h"

class MCSPLineStatusObserver;

/**
* Monitors single ETel RMobileLine for calls.
*
*  This monitor is applied only for listening line status changes 
*
*  @lib csplugin.dll
*  @since S60 5.0
*/
NONSHARABLE_CLASS(CSPEtelLineStatusMonitor): public CActive
    {
    
    public: //Constructors and descructor    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aLine the line to monitor
        * @param aLineId line identifier
        */
        static CSPEtelLineStatusMonitor* NewL( 
                MCSPLineStatusObserver& aObserver,
                RMobileLine& aLine,
                RCSPLineContainer::TCSPLineId aLineId );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelLineStatusMonitor( );

        /**
        * Start monitoring call events
        * @since S60 5.0
        */
        void StartMonitoring();
    
    protected: // From CActive
        /**
        * From CActive
        * RunL
        * @since S60 5.0 
        */
        void RunL();
        
        /**
        * From CActive
        * Cancels the monitor
        * @since S60 5.0 
        */
        void DoCancel();

    private:
        
        /**
        * C++ default constructor
        * @param aObserver the observer for status change
        * @param aLine the line associated with the call
        * @param aLineId line identifier
        */
        CSPEtelLineStatusMonitor( MCSPLineStatusObserver& aObserver, 
                                 RMobileLine& aLine,
                                 RCSPLineContainer::TCSPLineId aLineId );
                                                  
    private: // data

        /**
        * Line status from ETel.
        */         
        RMobileCall::TMobileCallStatus iLineStatus; 
        
        /**
        * Line that is being observed for status changes.
        */
        RMobileLine& iLine;
        
        /**
        * Line identifier / type.
        */
        RCSPLineContainer::TCSPLineId iLineId;
        
        /**
        * Line status notifications observer.
        */
        MCSPLineStatusObserver& iObserver;
        
    };

#endif /*CSPETELLINESTATUSMONITOR_H_*/
