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
* Description:  This module contains the implementation of CPEProfileSettingMonitor
*                class 
*
*/


// INCLUDE FILES
#include "cpeprofilesettingmonitor.h"
#include "mpephonemodelinternal.h"
#include <mpedatastore.h>
#include <MProfileExtraTones.h>
#include <talogger.h>
#include <ProfileEngineInternalCRKeys.h>


/////////////////////////////////////////////////////////////////////////
// CPEProfileSettingMonitor
/////////////////////////////////////////////////////////////////////////

CPEProfileSettingMonitor* CPEProfileSettingMonitor::NewL(
        MPEPhoneModelInternal& aModel )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEProfileSettingMonitor::NewL" );
    CPEProfileSettingMonitor* self = new ( ELeave ) CPEProfileSettingMonitor( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return ( self );                                                         
    }

// Destructor
CPEProfileSettingMonitor::~CPEProfileSettingMonitor( )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEProfileSettingMonitor::~CPEProfileSettingMonitor" );
    if ( iProfileEventHandler )
        {
        delete iProfileEventHandler;
        iProfileEventHandler = NULL;
        }
    if ( iProfileEngine )
        {
        iProfileEngine->Release();
        }
    }

// -----------------------------------------------------------------------------
// CPEProfileSettingMonitor::CPEProfileSettingMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEProfileSettingMonitor::CPEProfileSettingMonitor(
        MPEPhoneModelInternal& aModel
        ) : CPECenRepMonitor( KProEngSilenceMode ), 
            iModel( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CPEProfileSettingMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEProfileSettingMonitor::ConstructL()
    {
    // Create the profile engine
    iProfileEngine = CreateProfileEngineL();
    
    // Retrieve current profile settings    

    BaseConstructL( KCRUidProfileEngine );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageProfileChanged ); 
    
    // Start listening for profiles change notifications
    iProfileEventHandler = CProfileChangeNotifyHandler::NewL( this );    
    }

// -----------------------------------------------------------------------------
// CPEProfileSettingMonitor::GetProfileSettingsL
// Retrieves settings from profiles that are relevant to Phone Engine
// Called on construction and whenever notification is received that
// a change has occurred to the profile settings
// -----------------------------------------------------------------------------
//
void CPEProfileSettingMonitor::GetProfileSettingsL()
    {
    TEFLOGSTRING( KTAINT, "PE CPEProfileSettingMonitor::GetProfileSettings" );
    __UHEAP_MARK;
    //  Get a pointer to the active profile (with ownership)
    MProfile* activeProfile = iProfileEngine->ActiveProfileLC();
    
    // Get profile id
    iModel.DataStore()->SetProfileId( activeProfile->ProfileName().Id() );
    // Get profile name
    iModel.DataStore()->SetProfileName( activeProfile->ProfileName().Name() );
    
    const MProfileTones& activeTones = activeProfile->ProfileTones();
    
    // Get ringtones for line 1 and 2
    iModel.DataStore()->SetRingingTone1( activeTones.RingingTone1() );
    iModel.DataStore()->SetRingingTone2( activeTones.RingingTone2() );
    
    // Get video call ringing tone, if it is supported
    if ( iProfileEngine->IsFeatureSupported( EProfileFeatureVTRingingTone ) )
        {
        const MProfileExtraTones& extraTones = activeProfile->ProfileExtraTones();
        iModel.DataStore()->SetVideoCallRingingTone( 
            extraTones.VideoCallRingingTone() );
        }
    const TProfileToneSettings& toneSettings = activeTones.ToneSettings();  
    
    // Get ringing type and volume and keypad volume
    TProfileRingingType ringingType = toneSettings.iRingingType;
    
    TInt silentVolume(0);
    if ( KErrNone == Get(silentVolume) && silentVolume )
        {
        iModel.DataStore()->SetRingingType(EProfileRingingTypeSilent);
        }
    else 
        {
        iModel.DataStore()->SetRingingType( static_cast<TProfileRingingType>( ringingType ) );
        }
    
    iModel.DataStore()->SetRingingVolume( toneSettings.iRingingVolume );
    iModel.DataStore()->SetTextToSpeech( toneSettings.iTextToSpeech );
                  
    TProfileKeypadVolume keypadVolume = toneSettings.iKeypadVolume;
        
    if ( keypadVolume < EProfileKeypadVolumeOff 
        || keypadVolume > EProfileKeypadVolumeLevel3 )
        {
        // Value is out of range - use default
        keypadVolume = EProfileKeypadVolumeLevel2; // default value
        }
    
    const TInt KKeypadVolume[4] = {0, 2, 5, 10}; // Previous PE settings
    TInt volume = KKeypadVolume[ keypadVolume ];
    iModel.DataStore()->SetKeypadVolume( volume );

    //  Get alert for group Ids
    iModel.DataStore()->SetAlertForGroup( activeProfile->AlertForL() );

    // Pop( activeProfile ) does not recognize ActiveProfileLC created 
    // item because of casting. However, functionality and cleanup works OK.
    CleanupStack::Pop(); // activeProfile
    activeProfile->Release();
    __UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
// CPEProfileSettingMonitor::UpdateL
// Callback function - implements virtual function from CPECenRepMonitor
// This is called whenever cr settings change
// -----------------------------------------------------------------------------
//
void CPEProfileSettingMonitor::UpdateL()
    {
    TEFLOGSTRING( KTAINT, "PE CPEProfileSettingMonitor::UpdateL" );
    GetProfileSettingsL();
    iModel.SendMessage( MEngineMonitor::EPEMessageProfileChanged );
    }

// -----------------------------------------------------------------------------
// CPEProfileSettingMonitor::HandleActiveProfileEventL
// Callback function - implements virtual function from MProfileChangeObserver
// This is called whenever profiles settings change
// We're not interested in aProfileId which identifies the number of the active profile changed
// This notifier doesn't identify the property changed - so update them all by 
// calling the GetProfileSettingsL() method
// -----------------------------------------------------------------------------
//
void CPEProfileSettingMonitor::HandleActiveProfileEventL(
        TProfileEvent aProfileEvent,
        TInt /*aProfileId*/ )
    {
    TEFLOGSTRING2( KTAINT, "PE CPEProfileSettingMonitor::HandleActiveProfileEventL %d", aProfileEvent );
    
    ASSERT( aProfileEvent != EProfileUnknownEvent );
    
    // Update the iEngineInfo with the required profile settings    
    GetProfileSettingsL();
    
    if ( aProfileEvent != EProfileUnknownEvent )
        {
        // A new profile has been set
        iModel.SendMessage( MEngineMonitor::EPEMessageProfileChanged );        
        }        
    }

// End of file
