/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CBTSDPADVERTISER_H
#define CBTSDPADVERTISER_H

#include <e32base.h>
#include <btsdp.h>
#include <btmanclient.h>

/**
* CBtSdpAdvertiser
* Base class for advertising service in the SDP database.
*/
NONSHARABLE_CLASS( CBtSdpAdvertiser ) : public CBase
    {
    public: // Constructors and destructor

        /**
        * ~CBtSdpAdvertiser()
        * Destroy the object, close all open handles
        * and remove the advertised service.
        */
        virtual ~CBtSdpAdvertiser();

        /**
        * StartAdvertisingL()
        * Start the advertising of this service
        * @param aPort the port being used by the service
        */
        void StartAdvertisingL( TInt aPort );

        /**
        * StopAdvertisingL()
        * Stop advertising this service. Remove the record
        * from the sdp database
        */
        void StopAdvertisingL();

        /**
        * IsAdvertising()
        * Does the SDP database contain a record for this service
        * @return ETrue whether the service is being advertised
        */
        TBool IsAdvertising();

        /**
        * UpdateAvailabilityL()
        * Update the service availability field of the service record
        * @param aIsAvailable ETrue is the service is not busy.
        */
        void UpdateAvailabilityL( TBool aIsAvailable );

    protected:  // Constructor

        /**
        * CBtSdpAdvertiser()
        * Construct this object
        */
        CBtSdpAdvertiser();

        /**
        * BuildProtocolDescriptionL()
        * Builds the protocol description
        * @param aProtocolDescriptor the protocol descriptor
        * @param aPort the service port
        */
        virtual void BuildProtocolDescriptionL(
            CSdpAttrValueDES* aProtocolDescriptor, TInt aPort ) = 0;

        /**
        * ServiceClass()
        * @return the service class
        */
        virtual TUint ServiceClass() = 0;

        /**
        * ServiceName()
        * @return the service name
        */
        virtual const TDesC& ServiceName() = 0;

        /**
        * ServiceDescription()
        * @return the service description
        */
        virtual const TDesC& ServiceDescription() = 0;

    private:

        /**
        * ConnectL
        * Connect to the SDP database
        */
        void ConnectL();

    private:    //data

        /** iSdpSession a handle to the SDP session */
        RSdp iSdpSession;

        /** iSdpDatabase a connection to the SDP database */
        RSdpDatabase iSdpDatabase;

        /**iRecord the record handle of the service
        record for this server
        */
        TSdpServRecordHandle iRecord;

        /**iRecordState the current record state - change number */
        TInt iRecordState;

        /** iIsConnected has a connection been made to the SDP Database */
        TBool iIsConnected;
    };

#endif // CBTSDPADVERTISER_H

// End of File
