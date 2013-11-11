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


#ifndef ETELINCOMINGCALLMONITOR_H
#define ETELINCOMINGCALLMONITOR_H

#include <e32base.h>
#include <etelmm.h>


/**
* Monitors single ETel RMobileLine for incoming calls.
*
*  This monitor is applied only for listening incoming calls, i.e.
*  line capabilites are not monitored.
*
*/
class CEtelIncomingCallMonitor: public CActive
    {
    
    public: //Constructors and descructor

        /**
        * C++ default constructor
        * @param aLine the line associated with the call
        * @param aLineId line identifier
        */
        CEtelIncomingCallMonitor( /*RMobileLine& aLine,
                                  TInt aLineId */);    
        /**
        * C++ default destructor
        */
        virtual ~CEtelIncomingCallMonitor( );

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

    private: // data

        /**
        * Incoming call name received from ETel.
        */ 
        TName iCallName;
        
        /**
        * Line that is being observed for incoming call.
        */
        RMobileLine iLine;
        
        /**
        * Line identifier / type.
        */
        TInt iLineId;
        
        RTelServer iServer;
        
        RTelServer::TPhoneInfo iPhoneInfo;
    };
    
#endif // ETELINCOMINGCALLMONITOR_H
