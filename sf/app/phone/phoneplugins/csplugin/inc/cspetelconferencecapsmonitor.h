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
* Description:  Declarations for class CSPEtelConferenceCapsMonitor
*
*/


#ifndef CSPETELCONFERENCECAPSMONITOR_H
#define CSPETELCONFERENCECAPSMONITOR_H

#include <e32base.h>
#include <etelmm.h>


class CSPConferenceCall;


/**
* Monitors single ETel RMobileConferenceCall for conference call capss.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelConferenceCapsMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aOwner the observer for getting notification
        * @param aCall the line to monitor
        */
        static CSPEtelConferenceCapsMonitor* NewL( 
                CSPConferenceCall& aOwner,
                RMobileConferenceCall& aCall );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelConferenceCapsMonitor( );

        /**
        * Start monitoring call capss
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
        * @param aCall the line associated with the call
        */
        CSPEtelConferenceCapsMonitor( CSPConferenceCall& aOwner,
                            RMobileConferenceCall& aCall );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();

        
    private: // data
        /**
        * Forwards capss.
        */
        CSPConferenceCall& iOwner;
       
        /**
        * Conference call that is being observed.
        */
        RMobileConferenceCall& iCall;
        
        /**
        * Caps received.
        */ 
        TUint32 iCaps;
        
        /**
        * Pointer for recovering from a situation where CCE releases call 
        * during observer-call sequence.
        */
        TBool* iDestrPtr;

        
    };

#endif // CSPETELCONFERENCECAPSMONITOR_H