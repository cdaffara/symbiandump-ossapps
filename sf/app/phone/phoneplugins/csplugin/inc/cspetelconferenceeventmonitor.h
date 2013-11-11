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
* Description:  Declarations for class CSPEtelConferenceEventMonitor
*
*/


#ifndef CSPETELCONFERENCEEVENTMONITOR_H
#define CSPETELCONFERENCEEVENTMONITOR_H

#include <e32base.h>
#include <etelmm.h>


class CSPCall;
class CSPConferenceCall;


/**
* Monitors single ETel RMobileConferenceCall for conference call events.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelConferenceEventMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aOwner the observer for getting notification
        * @param aPhone the line to monitor
        */
        static CSPEtelConferenceEventMonitor* NewL( 
                CSPConferenceCall& aOwner,
                RMobileConferenceCall& aPhone );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelConferenceEventMonitor( );

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
        * @param aOwner the observer for status change (incoming call)
        * @param aPhone the line associated with the call
        */
        CSPEtelConferenceEventMonitor( CSPConferenceCall& aOwner,
                            RMobileConferenceCall& aPhone );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();

        
    private: // data
        /**
        * Forwards events.
        */
        CSPConferenceCall& iOwner;
       
        /**
        * Phone that is being observed for incoming call.
        */
        RMobileConferenceCall& iCall;
        
        /**
        * Event received.
        */ 
        RMobileConferenceCall::TMobileConferenceEvent iEvent;
        
        /**
        * The call name related to the event.
        */
        TName iCallName;
        
        /**
        * Pointer for recovering from a situation where CCE releases call 
        * during observer-call sequence.
        */
        TBool* iDestrPtr;
        
    };

#endif // CSPETELCONFERENCEEVENTMONITOR_H