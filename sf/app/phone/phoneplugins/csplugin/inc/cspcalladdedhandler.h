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
* Description:  Declarations for class CSPCallAddedHandler
*
*/


#ifndef CSPCALLADDEDHANDLER_H
#define CSPCALLADDEDHANDLER_H

#include <e32base.h>
#include <etelmm.h>
#include "rcsplinecontainer.h"
#include "mcsplinestatusobserver.h" // For MCSPLineStatusObserver

class CSPCall;
class CSPCallArray;
class CSProvider;
class MCCPCSObserver;
class RCSPLineContainer;
class CSPEtelLineStatusMonitor; 
class CSPEtelCallWaitingRequester;
class MCSPCommonInfo;
class CSPAudioHandlerBase;

/**
* Handles call adding from calls not done by the plugin.
*
*  @lib csplugin.dll
*/
NONSHARABLE_CLASS(CSPCallAddedHandler): public CBase,                            
                           public MCSPLineStatusObserver
    {
    
    public: //Constructors and descructor    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aLine the line to monitor
        * @param aLineId line identifier
        */
        static CSPCallAddedHandler* NewL( 
                const MCCPCSObserver& aObserver,
                RCSPLineContainer& aLineContainer,
                CSPCallArray& aCallArray,
                MCSPCommonInfo& aCommonInfo,
                CSPAudioHandlerBase& aAudioHandler );

        /**
        * C++ default destructor
        */
        virtual ~CSPCallAddedHandler( );


// from base class MCSPLineStatusMonitor        
        
        /**
        * Handles line status change notifications 
        * @param aLine the line to monitor
        * @param aLineId line identifier
        * @param aLineStatus
        */        
        void LineStatusChanged( RMobileLine& aLine,
                RCSPLineContainer::TCSPLineId aLineId, 
                RMobileCall::TMobileCallStatus& aLineStatus );

    protected: // From CActive
        
        /**
        * From CActive
        * RunL
        */
        void RunL();
        
        /**
        * From CActive
        * Catches errors if RunL leaves
        * @param aError error code
        * @return error code
        */
        TInt RunError( TInt aError );
        
        /**
        * From CActive
        * Cancels the monitor
        */
        void DoCancel();

    private:
        
        /**
        * C++ default constructor
        * @param aObserver the observer for status change (incoming call)
        * @param aLine the line associated with the call
        * @param aLineId line identifier
        */
        CSPCallAddedHandler( const MCCPCSObserver& aObserver, 
                             RCSPLineContainer& aLineContainer,
                             CSPCallArray& aCallArray,
                             MCSPCommonInfo& aCommonInfo,
                             CSPAudioHandlerBase& aAudioHandler );
                                          
        /**
        * Constructs the monitor in the second phase.
        */
        void ConstructL();
        
        /**         
        * Handles call creation 
        * @param aLine the line to monitor
        * @param aCallName the call name 
        * @param aLineId line identifier
        */
        void CallAdded( RMobileLine& aLine, 
                        const TName& aCallName,
                        RCSPLineContainer::TCSPLineId aLineId );

    private: // data
        
        /**
        * Line container. 
        */        
        RCSPLineContainer& iLineContainer;
        
        /**
        * Notifications observer.
        */
        const MCCPCSObserver& iObserver;
        
        /**
        * Line status monitors.
        */
        CSPEtelLineStatusMonitor* iVoiceLineMonitor;        
        CSPEtelLineStatusMonitor* iDataLineMonitor;
        CSPEtelLineStatusMonitor* iAuxLineMonitor;
        
        /**
        * Array of calls.
        */
        CSPCallArray& iCallArray;
        
        /**
        * Common info handle.
        */
        MCSPCommonInfo& iCommonInfo;
        
        /**
        * Audio Handler reference.
        */
        CSPAudioHandlerBase& iAudioHandler;
    };

#endif // CSPCALLADDEDHANDLER_H
