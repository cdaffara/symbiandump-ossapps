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
* Description:  Declarations for class CSPCipheringStatusMonitor
*
*/


#ifndef CSPCIPHERINGSTATUSMONITOR_H
#define CSPCIPHERINGSTATUSMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>

#include "cspconsts.h"


// FORWARD DECLARATIONS
class RMmCustomAPI;
class MCSPSecuritySettingObserver;

// CLASS DECLARATION


class CSPCipheringStatusMonitor : public CActive
    {
    public: //Constructors and descructor

        /**
        * C++ default constructor
        */
        CSPCipheringStatusMonitor( RMmCustomAPI& aMmCustom,
            MCSPSecuritySettingObserver& aObs );

        /**
        * Destructor.
        */
        ~CSPCipheringStatusMonitor();

    public:

        static CSPCipheringStatusMonitor* NewL( RMmCustomAPI& aMmCustom, 
            MCSPSecuritySettingObserver& aObs );
        
        /**
        * Starts monitoring the CW notifications
        */
        void StartMonitoring();
        
        /**
        * Network security status.
        * @return secured status
        */
        TBool NetworkSecurityStatus() const;

        /**
        * Secure specified.
        * @return secure specified status
        */        
        TBool SecureSpecified() const;        
        
    protected: // From CActive
    
        /**
        * Cancels asyncronous request(s).
        */
        void DoCancel();

        /**
        * Informs object that asyncronous request is ready.
        */
        void RunL();
        
    private: // data

        /**
        * Ciphering info 
        */
        RMmCustomAPI::TCipheringInfo iCipheringInfo; 
        
        /**
        * CustomAPI handle
        */        
        RMmCustomAPI& iMmCustom; 
        
        /**
        * Security setting observer.
        */
        MCSPSecuritySettingObserver &iObserver;

        /**
        * indicates if ciphering status was retreived for the first time
        */
        TBool iIsInitialised;
        
        /**
        * Call secure status.
        */
        TBool iCallsSecured;
        
        /**
        * Call secure specified status.
        */
        TBool iSecureSpecified;

    };

#endif // CSPCIPHERINGSTATUSMONITOR_H

// End of File
