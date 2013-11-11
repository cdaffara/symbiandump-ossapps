/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPEClientEmergencyCallMonitor class.
*
*/

#ifndef CPECLIENTEMERGENCYCALLMONITOR_H
#define CPECLIENTEMERGENCYCALLMONITOR_H

//INCLUDES
#include <e32base.h>
#include <cphcltemergencycall.h>
#include <mphcltdialemergencyrequestobserver.h>
#include "mpeclientcallrequestmonitor.h"

// FORWARD DECLARATIONS
class RTASecuritySession;
class CPEMessageHandler;
class MPEPhoneModelInternal;
class CPhCltCallNotify;
class CPhCltDialData;

// CLASS DECLARATION

/**
*  Monitor Emergency call requests from the phone server(/phone client). 
*  
*
*  @lib PhoneEngine.lib
*  @since S60_5.0
*/
NONSHARABLE_CLASS( CPEClientEmergencyCallMonitor ) 
        : public CBase,
          public MPEClientCallRequestMonitor,
          public MPhCltDialEmergencyRequestObserver  
{
    public: //Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aModel Used to send client dial message to phone application.
        * @param aEngineInfo To where phone number is saved.
        * @param aPhoneServer Gived to RPhCltCallNotify in Open call.
        */
        static CPEClientEmergencyCallMonitor* NewL( 
            MPEPhoneModelInternal& aModel, 
            CPhCltCallNotify& ACallNotifier );
    
        /**
        * Destructor.
        */
        virtual ~CPEClientEmergencyCallMonitor(); 
        
    public:    //From MPEClientCallMonitor
        /**
        * Gets dial data
        * @return Reference to TPhCltTelephoneNumber, telephony number.
        */
        CPhCltDialData* ClientDialData();
        
        /**
        * Sends respond to the phone client
        * @param aRespond  contains respond to the phone client.
        */
        void SendRespond( const TInt aRespond );
        
    public: 
        /**
        * Emergency dial request observer
        */
        void EmergencyDialRequest();

    public:    //New functions
        /**
        * Starts monitoring phone client.
        */
        void StartMonitoring( );
        
        /**
        * Cancels asyncronous request(s).
        */
        void Cancel();
        
        /**
        * Tells whether this monitor is active.
        */
        TBool IsActive();

        
    public:

        /**
        * Informs object that asyncronous request is ready.
        */
        void DialEmergencyRequest();

    private:
        /**
        * C++ default constructor.
        * @param aModel Used to send client dial message to phone application.
        * @param aEngineInfo To where phone number is saved.
        */
        CPEClientEmergencyCallMonitor( MPEPhoneModelInternal& aModel,
                                     CPhCltCallNotify& aCallNotifier );
      
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private:    //Data
        // MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        //  Notifies call request from the client/server interface.
        CPhCltCallNotify& iCallNotifier;
        // Client originated call (on/off)
        TBool iClientOriginatedCall;
        //Emergency number.
        TBuf<KPhCltEmergencyNumberSize> iEmergencyNumber;        
        // Monitoring flag
        TBool iMonitoring;
    };

#endif // CPECLIENTEMERGENCYCALLMONITOR_H

// End of File
