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
* Description:  Monitors changes in remote party info 
*                (RMobileCall::TMobileCallRemotePartyInfoV1)
*  Interface   : 
*
*/


#ifndef     CSPCALLINFOMONITOR_H
#define     CSPCALLINFOMONITOR_H

//  INCLUDES
#include    <etelmm.h>
#include    "mcspcallobserver.h"

//  FORWARD DECLARATIONS

class   CSPCallData;
class   CSPPhoneData;
class   RMobileCall;

//  CLASS DEFINITIONS 

/** Monitors changes in remote party info.
*   This class monitors remote party info changes in ETel's call objects.
*   @since 3.1
*/
NONSHARABLE_CLASS( CSPCallInfoMonitor ): public CActive
    {
    public:
        /** 
        * Two-phased constructor 
        * @param aObserver Call info change observer
        * @param aCall This is the call to be monitored
        */
        static CSPCallInfoMonitor* NewL(
            MCSPCallObserver& aObserver, RMobileCall &aCall );

        /** D'tor. */
        ~CSPCallInfoMonitor();

    public:
        /** 
        * Starts monitoring.
        */
        void StartMonitoring( );

    private:
        /** Prohibited. */
        CSPCallInfoMonitor& operator=( const CSPCallInfoMonitor& );

        /**
        * C++ Constructor
        * @param aObserver aObserver Call info change observer        
        * @param aCall This is the call to be monitored.         
        */
        CSPCallInfoMonitor( MCSPCallObserver& aObserver, 
                            RMobileCall &aCall ); 


    private:    // from CActive
        /** @see CActive::RunL() */
        void RunL();

        /**  @see CActive::DoCancel() */
        void DoCancel();
      
        

    private:
    
        /**
        * Observer for info change.
        */
        MCSPCallObserver& iObserver;
        
        /**
        * Pointer to Call object.
        * Not own.
        */
        RMobileCall& iCall;
        
        /**
        * Remote party info.
        */ 
        RMobileCall::TMobileCallRemotePartyInfoV1 iRemotePartyInfo;
        
        /**
        * packaged remote party info.
        */         
        RMobileCall::TMobileCallRemotePartyInfoV1Pckg iRemotePartyInfoPckg;
    };

#endif // CSPCALLINFOMONITOR_H

// End of File
