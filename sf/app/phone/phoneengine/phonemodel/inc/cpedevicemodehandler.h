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
* Description:  Device mode handler
*
*/



#ifndef CPEDEVICEMODEHANDLER_H
#define CPEDEVICEMODEHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <mphonedevicemodeobserver.h>

// FORWARD DECLARATIONS
class MPECallHandling;
class CPEMessageHandler;
class CTelDMHandler;


/**
*  Device mode handler.
*/
NONSHARABLE_CLASS( CPEDeviceModeHandler ) : public CBase, public MPhoneDeviceModeObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return Pointer to created CPEDeviceModeHandler instance.
        */
        static CPEDeviceModeHandler* NewL(          
            MPECallHandling& aCallHandling,
            CPEMessageHandler& aMessageHandler );

        /**
        * Destructor.
        */
        virtual ~CPEDeviceModeHandler();

    public: // New functions
        
    protected:
      
        /**
        * @see MPhoneDeviceModeObserver
        */
        void Answer();

        /**
        * @see MPhoneDeviceModeObserver
        */
        void EndVoiceCalls();

 
    private: // Constructors

        /**
        * Constructor.
        */
        CPEDeviceModeHandler(           
            MPECallHandling& aCallHandling,
            CPEMessageHandler& aMessageHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Loads Telephony Device mode library.
        */
        void LoadLibraryL();
    
    private:    // Data
       
        // Library loader.
        RLibrary            iLibrary;   

        // MPECallHandling handles call related commands to CallHandling subsystem.
        MPECallHandling&    iCallHandling;

        // CPEMessageHandler object which handles messages
        CPEMessageHandler&  iMessageHandler;    
   
        // TelDeviceMode instance.
        CTelDMHandler*      iTelDevModeHandler;

    };

#endif      // CPEDEVICEMODEHANDLER_H
            
// End of File
