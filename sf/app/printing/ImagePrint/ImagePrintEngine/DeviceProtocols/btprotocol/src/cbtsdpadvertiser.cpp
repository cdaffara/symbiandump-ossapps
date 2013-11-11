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


#include <bt_sock.h>
#include <StringLoader.h>

#include "cbtsdpadvertiser.h"
#include "clog.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::CBtSdpAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CBtSdpAdvertiser::CBtSdpAdvertiser()
:   iRecord( 0 ),
    iIsConnected( EFalse )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::~CBtSdpAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtSdpAdvertiser::~CBtSdpAdvertiser()
    {
    // If this fails there is nothing to do.
    TRAPD( err,StopAdvertisingL() );
    if ( err != KErrNone )
        {
			LOG1("[CBtSdpAdvertiser::~CBtSdpAdvertiser]\t StopAdvertisingL leaves with: %d", err);
        }
    iSdpDatabase.Close();
    iSdpSession.Close();

	LOG("[CBtSdpAdvertiser::~CBtSdpAdvertiser]\t done");
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::ConnectL()
// Connect to the SDP database.
// ----------------------------------------------------------------------------
//
void CBtSdpAdvertiser::ConnectL()
    {
    if ( !iIsConnected )
        {
        User::LeaveIfError( iSdpSession.Connect() );
        User::LeaveIfError( iSdpDatabase.Open( iSdpSession ) );
        iIsConnected = ETrue;
        }
		LOG1("[CBtSdpAdvertiser::ConnectL]\t iIsConnected: %d", iIsConnected);
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::StartAdvertisingL()
// Start the advertising of this service.
// ----------------------------------------------------------------------------
//
void CBtSdpAdvertiser::StartAdvertisingL( TInt aPort )
    {
    // could be advertising on a different port
    StopAdvertisingL();

    if ( !iIsConnected )
        {
        ConnectL();
        }
    iSdpDatabase.CreateServiceRecordL( ServiceClass(), iRecord );

	LOG1("[CBtSdpAdvertiser::StartAdvertisingL]\t iRecord: %d", iRecord);

    // add a Protocol to the record
    CSdpAttrValueDES* vProtocolDescriptor = CSdpAttrValueDES::NewDESL( NULL );
    CleanupStack::PushL( vProtocolDescriptor );

    BuildProtocolDescriptionL( vProtocolDescriptor,aPort );

    iSdpDatabase.UpdateAttributeL( iRecord, KSdpAttrIdProtocolDescriptorList,
        *vProtocolDescriptor );

    CleanupStack::PopAndDestroy( vProtocolDescriptor );

    // Add a name to the record
    iSdpDatabase.UpdateAttributeL( iRecord,
                                  KSdpAttrIdBasePrimaryLanguage +
                                  KSdpAttrIdOffsetServiceName,
                                  ServiceName() );

    // Add a description to the record
    iSdpDatabase.UpdateAttributeL( iRecord,
                                  KSdpAttrIdBasePrimaryLanguage +
                                  KSdpAttrIdOffsetServiceDescription,
                                  ServiceDescription() );

	LOG("[CBtSdpAdvertiser::StartAdvertisingL]\t end");
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::UpdateAvailabilityL()
// Update the service availability field of the service record.
// ----------------------------------------------------------------------------
//
void CBtSdpAdvertiser::UpdateAvailabilityL( TBool aIsAvailable )
    {
	LOG("[CBtSdpAdvertiser::UpdateAvailabilityL]\t ");
    TUint state;
    if ( aIsAvailable )
        {
        state = 0xFF;  //  Fully unused
        }
    else
        {
        state = 0x00;  //  Fully used -> can't connect
        }

    //  Update the availibility attribute field
    iSdpDatabase.UpdateAttributeL( iRecord,
        KSdpAttrIdServiceAvailability, state );


    //  Mark the record as changed - by increasing its state number (version)
    iSdpDatabase.UpdateAttributeL( iRecord,
        KSdpAttrIdServiceRecordState, ++iRecordState );

	LOG("[CBtSdpAdvertiser::UpdateAvailabilityL]\t end");
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::StopAdvertisingL()
// Stop advertising this service. Remove the record from the sdp database.
// ----------------------------------------------------------------------------
//
void CBtSdpAdvertiser::StopAdvertisingL()
    {
    if ( IsAdvertising() )
        {
        iSdpDatabase.DeleteRecordL( iRecord );
        iRecord = 0;
        }
    }

// ----------------------------------------------------------------------------
// CBtSdpAdvertiser::IsAdvertising()
// Does the SDP database contain a record for this service.
// ----------------------------------------------------------------------------
//
TBool CBtSdpAdvertiser::IsAdvertising()
    {
	LOG1("[CBtSdpAdvertiser::IsAdvertising]\t %d", iRecord != 0);
    return iRecord != 0;
    }

// End of File
