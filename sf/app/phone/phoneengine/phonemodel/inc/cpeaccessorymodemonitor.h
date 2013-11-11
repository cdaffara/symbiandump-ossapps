/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file for CPEAccessoryModeMonitor class
*
*/


#ifndef CPEACCESSORYMODEMONITOR_H
#define CPEACCESSORYMODEMONITOR_H


//  INCLUDES
#include <e32base.h>
#include <AccessoryServer.h>
#include <AccessoryMode.h>
#include <AccPolAccessoryMode.h>

// CONSTANTS

// FORWARD DECLARATIONS
 
// CLASS DECLARATION

/**
*   
* For accessory mode
* 
* @since Series 60 3.1
*/
NONSHARABLE_CLASS( CPEAccessoryModeMonitor ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPEAccessoryModeMonitor* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPEAccessoryModeMonitor();

    public: // New functions
    
        RAccessoryServer AccessoryServer();
    
        /**
        * Accessory mode
        */
        TInt AccessoryMode();
        /**
        * Accessory output status 
        */
        TBool Status();
 
    protected:  // Functions from base classes
    
        /** @see CActive::RunL */
        void RunL();

        /** @see CActive::DoCancel */
        void DoCancel();
        
           
    private:

        /**
        * C++ default constructor.
        */
        CPEAccessoryModeMonitor();
        
        /**
        * Mode notifying is started
        */
        void Start();
           
    private: // Data
    
        // Accessory mode
        TInt iAccessoryMode;

        // Status of accessory output
        TBool iAudioOutputStatus;

        // Accessory Server session 
        RAccessoryServer   iAccServer;

        // Accessory mode 
        RAccessoryMode     iMode;

        // Accessory Mode structure
        TAccPolAccessoryMode iPolAccessoryMode;
        
        // Accessory server status
        TBool iAccConn;
        
        // Accessory mode session status
        TBool iAccSess;
        };

#endif  //  CPEACCESSORYMODEMONITOR_H
