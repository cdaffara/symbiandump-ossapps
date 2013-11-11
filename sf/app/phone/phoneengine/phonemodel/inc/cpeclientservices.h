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
* Description:  This file contains the header file of the CPEClientServices class.
*
*/



#ifndef CPECLIENTSERVICES_H
#define CPECLIENTSERVICES_H

//INCLUDES
#include <e32base.h>
#include <rphcltserver.h>
#include  <cphcltemergencycall.h>


// FORWARD DECLARATIONS
class CPEClientCallRequestMonitor;
class CPEClientCommandHandlerMonitor;
class CPEDeviceModeHandler;
class CPEMessageHandler;
class MPEPhoneModelInternal;
class CPhCltUssd;
class MPECallHandling;
class RTASecuritySession;
class CPEClientEmergencyCallMonitor;
class CPhCltCallNotify;
class MPEClientCallRequestMonitor;
class CPhCltDialData;
class CPEManualCallControlHandler;

// CLASS DECLARATION

/**
*  Monitor client requests from the phone server(/phone client). 
*  
*
*  @lib PhoneEngine.lib
*  @since Series60_2.7
*/
NONSHARABLE_CLASS( CPEClientServices ) : public CBase
    {
    public: //Constructors and destructor
    
        /**
        * Destructor.
        */
        ~CPEClientServices();

        /**
        * Two-phased constructor.
        */
         static CPEClientServices* NewL(
             MPEPhoneModelInternal& aModel,
             CPEMessageHandler& aMessageHandler, 
             MPECallHandling& aCallHandling,
             CPEManualCallControlHandler& aManualCallControlHandler );  
        
    public:    //New functions

        /**
        * Return whether the number specified would initiate an emergency call
        * @param aNumber a number to be checked
        * @param aIsEmergencyNumber Etrue, if the number is an emergency number.
        * @param aMode emergency number checking mode.
        * @return TInt Return value from PhoneClient.
        */
        TInt IsEmergencyPhoneNumber( 
            TPhCltTelephoneNumber& aNumber, 
            TBool& aIsEmergencyNumber );

        /**
        * Process USSD request
        * @param aString, Unicode string to be sent.
        */
        TInt SendUssd( const TDesC& aString );

        /**
        * Start Monitoring
        */
        void StartMonitoring();

        /**
        * Return pointer of dial data.
        */
        MPEClientCallRequestMonitor* CallRequestMonitor();
        
        /**
        * Return pointer of CPEClientCommandHandlerMonitor
        */
        CPEClientCommandHandlerMonitor* CommandHandlerMonitor();
        

    protected:

        /**
        * C++ default constructor.
        */
        CPEClientServices(
            MPEPhoneModelInternal& aModel,
            CPEMessageHandler& aMessageHandler,
            MPECallHandling& aCallHandling,
            CPEManualCallControlHandler& aManualCallControlHandler );

        void ConstructL();


    private:    //Data
        // MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // CPEMessageHandler object which owns this requester.
        CPEMessageHandler& iMessageHandler;
        // Handle to Call Handling subsystem
        MPECallHandling& iCallHandling;
        // Handle to Phone Client/Server
        RPhCltServer iPhoneServer;
        // Service for checking emergency number.
        CPhCltEmergencyCall* iEmergency;
        // Handle to CPEClientEmergencyCallMonitor
        CPEClientEmergencyCallMonitor* iClientEmergencyMonitor;
        //  Notifies call request from the client/server interface.
        CPhCltCallNotify* iCallNotifier;
        // Handle to Phone Client/Server USSD interface
        CPhCltUssd* iUssdClient;
        // Handle to CPEClientCommandHandlerMonitor
        CPEClientCommandHandlerMonitor* iClientCommandHandlerMonitor;
        // Handle to CPEClientCallRequestMonitor
        CPEClientCallRequestMonitor* iClientCallRequestMonitor;
        // USSD string to be sent
        TPtrC iString;
        // Handle to CPEDeviceModeHandler
        CPEDeviceModeHandler* iDeviceModeHandler;
        // Manual Call Control Handler
        CPEManualCallControlHandler& iManualCallControlHandler;
    };

#endif // CPECLIENTSERVICES_H

// End of File
