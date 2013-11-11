/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of 
*                CPEExternalDataHandler class 
*
*/


// INCLUDE FILES
#include "penetworkhandler.h"
#include "cpeexternaldatahandler.h"
#include "mpephonemodelinternal.h"
#include <accessoriescrkeys.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <ctsydomainpskeys.h>
#include <mpedatastore.h>
#include <pepanic.pan>
#include <settingsinternalcrkeys.h>
#include <talogger.h>
#include <telephonyvariant.hrh>
#include <telinternalcrkeys.h>

CPEExternalDataHandler* CPEExternalDataHandler::NewL( MPEPhoneModelInternal& aModel )
    {
    CPEExternalDataHandler* self = new ( ELeave ) CPEExternalDataHandler( *aModel.DataStore() );
    CleanupStack::PushL( self );
    self->ConstructL( aModel );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::CPEExternalDataHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEExternalDataHandler::CPEExternalDataHandler(
        MPEDataStore& aDataStore
        ) : iDataStore( aDataStore )
    {
    }

// Destructor
CPEExternalDataHandler::~CPEExternalDataHandler()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEExternalDataHandler::~CPEExternalDataHandler" );    

    delete iTelephonyVariationRepository;
    delete iTelephonySettingsRepository;
    delete iProfileSettings;
    delete iNetworkRegistrationStatus;   
    delete iCallDurationDisplay;
    delete iAudioOutputPreferenceMonitor;
    delete iAccessorySettingsRepository;
    delete iAccessoryModeMonitor;
    delete iCoreApplicationRepository;
    delete iNetworkHandler;
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEExternalDataHandler::ConstructL(
        MPEPhoneModelInternal& aModel )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEExternalDataHandler::BaseConstructL" );

    // Instantiate monitor objects
    iAudioOutputPreferenceMonitor = CPEAudioOutputPreferenceMonitor::NewL( aModel );    
    iCallDurationDisplay = CPECallDurationDisplaySettingMonitor::NewL(); 
    iProfileSettings = CPEProfileSettingMonitor::NewL( aModel );
    
    iNetworkHandler = new PeNetworkHandler(aModel);
    //TODO
    /*
    iNetworkRegistrationStatus = CPENetworkRegistrationStatusMonitor::NewL( aModel );
    iAccessoryModeMonitor = CPEAccessoryModeMonitor::NewL(); 

    // Instantiate repository objects for later use
    iAccessorySettingsRepository = CRepository::NewL( KCRUidAccessorySettings );*/
    //iTelephonySettingsRepository = CRepository::NewL( KCRUidTelephonySettings );
    iTelephonyVariationRepository = CRepository::NewL( KCRUidTelVariation );
    iCoreApplicationRepository = CRepository::NewL( KCRUidCoreApplicationUIs ); 
   
    TEFLOGSTRING( KTAOBJECT, "PE CPEExternalDataHandler::BaseConstructL 2" );
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::Get
// Gets the current value of the specified setting, if the operation was successful
// aValue contains the current valid value of the setting
// -----------------------------------------------------------------------------
//
TInt CPEExternalDataHandler::Get(
        const TPEExternalDataId aSetting,
        TInt& aValue ) const
    {
    TInt errorCode( KErrNotFound );

    //TODO

    if ( EPETelephonyVariationFlags == aSetting )
        {
        errorCode = iTelephonyVariationRepository->Get( KTelVariationFlags, aValue );
        TEFLOGSTRING2( KTAINT, "CPEExternalDataHandler::Get EPETelephonyVariationFlags, error code: %d", errorCode );
        }
    else if ( EPEAudioOutputPreference == aSetting )
        {
        errorCode = iAudioOutputPreferenceMonitor->Get( aValue );
        TEFLOGSTRING2( KTAINT, "CPEExternalDataHandler::Get EPEAudioOutputPreference, error code: %d", errorCode );
        }    
    else if ( EPECallDurationDisplaySetting == aSetting )
        {
        errorCode = iCallDurationDisplay->Get( aValue );
        TEFLOGSTRING2( KTAINT, "CPEExternalDataHandler::Get EPECallDurationDisplaySetting, error code: %d", errorCode );
        }
    else if ( EPENetworkConnectionAllowedSetting == aSetting )
        {
        // Fetches setting that indicates if network connection is allowed,
        // i.e. is the phone in off-line mode. 
        errorCode = iCoreApplicationRepository
                ->Get( KCoreAppUIsNetworkConnectionAllowed, aValue );
        }
    else
        {
        errorCode = KErrNone;
        aValue = 0;        
        }

    // Process Common id
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::Get
// Gets the current value of the specified setting, if the operation was successful
// aValue contains the current valid value of the setting
// -----------------------------------------------------------------------------
//
TInt CPEExternalDataHandler::GetText(
        const TPEExternalDataId /*aSetting*/,
        TDesC& aValue ) const
    {
    TInt errorCode( KErrNotFound );
    //TODO
    aValue = KNullDesC;
    errorCode = KErrNone;
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::GetAutomaticAnswer
// Check if automatic answer is defined for connected accessory.
// -----------------------------------------------------------------------------
//
TInt CPEExternalDataHandler::GetAutomaticAnswer(
        TUint32 /*aSetting*/,
        TInt& aValue ) const
    {
    TInt errorCode( KErrNotFound );
    
    TEFLOGSTRING( KTAINT, "CPEExternalDataHandler::GetAutomaticAnswer" );
    errorCode = KErrNone;
    aValue = 0;
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::Set
// Sets the value of the specified setting
// -----------------------------------------------------------------------------
//
TInt CPEExternalDataHandler::Set(
        const TPEExternalDataId aSetting,
        const TInt aValue )
    {
    TInt errorCode( KErrNotFound );

    switch ( aSetting )
        {
        case EPEEmergencyCallInfo:
            {
            errorCode = RProperty::Set( KPSUidCtsyEmergencyCallInfo, 
                                        KCTSYEmergencyCallInfo, aValue );
            TEFLOGSTRING2( KTAERROR, 
            "PE CPEExternalDataHandler::Set KPSUidCtsyEmergencyCallInfo: %d", aValue );
            break;            
            }
        default:
            {
            TEFLOGSTRING2( KTAERROR, "PE CPEEXTERNALDATAHANDLER::SET UNKNOWN SETTING ID ! %d", aSetting );
            break;
            }
        }
    return errorCode;
    }

// End of file
