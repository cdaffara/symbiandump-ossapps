/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the
*                CSPSupplementaryServicesMonitor object.
*
*/


#ifndef CSPSUPPLEMENTARYSERVICESMONITOR_H
#define CSPSUPPLEMENTARYSERVICESMONITOR_H


//INCLUDES
#include <e32base.h>
#include <rmmcustomapi.h>


// FORWARD DECLARATIONS
class CSProvider;

// CLASS DECLARATION

/**
* Monitors Supplementary Services
*
* @lib CallHandling.lib
* @since Series60_4.0
*/
class CSPSupplementaryServicesMonitor: public CActive
    {
    public:    //Constructors and descructor

        /**
        * C++ default constructor.
        */
        CSPSupplementaryServicesMonitor( 
            CSProvider& aOwner, 
            RMmCustomAPI& aCustomAPI );
        
        /**
        * Destructor.
        */
        virtual ~CSPSupplementaryServicesMonitor();

    public: //New functions
        
        /**
        * Starts monitoring new calls
        * @since Series60_4.0
        * @param None. 
        * @return None. 
        */
        void StartMonitoring();

    protected: // From CActive
        
        /**
        * Cancels asyncronous request(s).
        * @since Series60_4.0
        * @param None.    
        * @return None.
        */
        void DoCancel();

        /**
        * Informs object that asyncronous request is ready.
        * @since Series60_4.0
        * @param None. 
        * @return None.
        */
        void RunL();
      
    private:
        
        /** Owner of this object */
        CSProvider& iOwner;
        /** Subsession to custom api */
        RMmCustomAPI& iCustomAPI;
        /** Suplementary Services Type And Mode */
        RMmCustomAPI::TSsTypeAndMode iSsTypeAndMode;
        /** Suplementary Services info */
        RMmCustomAPI::TSsInfo iSsInfo;
    };

#endif // CSPSUPPLEMENTARYSERVICESMONITOR_H
