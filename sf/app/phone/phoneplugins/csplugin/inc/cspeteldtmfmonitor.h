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
* Description:  Declarations for class CSPEtelDtmfMonitor
*
*/


#ifndef CSPETELDTMFMONITOR_H
#define CSPETELDTMFMONITOR_H

#include <e32base.h>
#include <etelmm.h>
#include <rmmcustomapi.h>


class CSPCall;
class CSPDTMFProvider;



/**
* Monitors ETel for DTMF events.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSPEtelDtmfMonitor: public CActive
    {
    
    public: //Constructors and descructor
    
    
        /**
        * Two-phased constructing for the monitor.
        *
        * @param aObserver the observer for getting notification
        * @param aMmCustom Custom API handle
        */
        static CSPEtelDtmfMonitor* NewL( 
                CSPDTMFProvider& aObserver,
                RMmCustomAPI& aMmCustom );

        /**
        * C++ default destructor
        */
        virtual ~CSPEtelDtmfMonitor( );

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
        * @param aMmCustom Custom API handle
        */
        CSPEtelDtmfMonitor( CSPDTMFProvider& aObserver,
                            RMmCustomAPI& aMmCustom );
                                          
    private: // data
        /**
        * Forwards events.
        */
        CSPDTMFProvider& iObserver;
       
        /**
        * Event data. Updated when event occurs.
        */
        RMmCustomAPI::TDtmfInfo      iEventData;
        
        /**
        * Custom API reference.
        */
        RMmCustomAPI& iMmCustom;
        
    };
    
#endif // CSPETELDTMFMONITOR_H