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
* Description:  This file contains the header file of the CPEClientCallRequestMonitor class.
*
*/



#ifndef CPECLIENTCALLREQUESTMONITOR_H
#define CPECLIENTCALLREQUESTMONITOR_H

//INCLUDES
#include <e32base.h>
#include <rphcltserver.h>
#include "mpeclientcallrequestmonitor.h"
#include <mphcltdialrequestobserver.h>


// FORWARD DECLARATIONS
class RTASecuritySession;
class MPEPhoneModelInternal;
class CPhCltCallNotify;
class CPhCltDialData;


// CLASS DECLARATION

/**
*  Monitor call requests from the phone server(/phone client). 
*  
*
*  @lib PhoneEngineBase.lib
*  @since S60_5.0
*/
NONSHARABLE_CLASS( CPEClientCallRequestMonitor ) 
        : public CBase,
          public MPEClientCallRequestMonitor,
          public MPhCltDialRequestObserver
{
    public: //Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aModel Used to send client dial message to phone application.
        * @param aEngineInfo To where phone number is saved.
        * @param aPhoneServer Gived to RPhCltCallNotify in Open call.
        */
        static CPEClientCallRequestMonitor* NewL( 
            MPEPhoneModelInternal& aModel, 
            CPhCltCallNotify& aCallNotifier );
    
        /**
        * Destructor.
        */
        virtual ~CPEClientCallRequestMonitor(); 
        
    public:    //From MPEClientCallRequestMonitor
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
        
    public: // From MPhCltDialRequestObserver
        /**
        * Dial request.
        * @param aDialData data which ownership is transferred.
        */
        virtual void DialRequest( CPhCltDialData* aDialData );

    public:    //New functions
        /**
        * Starts monitoring phone client.
        */
        void StartMonitoring( );
        
    public:
        /**
        * Cancels asyncronous request(s).
        */
        void Cancel();
        
        /**
        * Tells whether this monitor is active.
        */
        TBool IsActive();


    private:
        /**
        * C++ default constructor.
        * @param aModel Used to send client dial message to phone application.
        * @param aEngineInfo To where phone number is saved.
        */
        CPEClientCallRequestMonitor( MPEPhoneModelInternal& aModel,
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
        // Dial Data
        CPhCltDialData* iDialData;
        // Client originated call (on/off)
        TBool iClientOriginatedCall;
        // Monitoring flag
        TBool iMonitoring;
        
    };

#endif // CPECLIENTCALLREQUESTMONITOR_H

// End of File
