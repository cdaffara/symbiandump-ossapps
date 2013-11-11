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
* Description:  This module contains the implementation of CPEAudioOutputPreferenceMonitor class 
*
*/


// INCLUDES
#include "cpeaudiooutputpreferencemonitor.h"
#include "cpepubsubmonitor.h"
#include "mpephonemodelinternal.h"
#include <e32property.h>
#include <mpedatastore.h>
#include <talogger.h>
#include <telinternalpskeys.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEAudioOutputPreferenceMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CPEAudioOutputPreferenceMonitor* CPEAudioOutputPreferenceMonitor::NewL(
        MPEPhoneModelInternal& aModel ) 
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEAudioOutputPreferenceMonitor::NewL");
    CPEAudioOutputPreferenceMonitor* self = new (ELeave) CPEAudioOutputPreferenceMonitor( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self );
    }
                                                                            
// -----------------------------------------------------------------------------
// CPEAudioOutputPreferenceMonitor::CPEAudioOutputPreferenceMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAudioOutputPreferenceMonitor::CPEAudioOutputPreferenceMonitor(
        MPEPhoneModelInternal& aModel
        ): CPEPubSubMonitor( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CPEAudioOutputPreferenceMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEAudioOutputPreferenceMonitor::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEAudioOutputPreferenceMonitor::ConstructL");
   
    BaseConstructL( KPSUidTelAudioPreference, KTelAudioOutput, RProperty::EInt );
  
    //  Now retrieve the value
    TInt value;
    TInt error = Get( value );
    
    // if not able to retrieve the value, set it to the default value
    if ( error != KErrNone )
        {   
        value = EPSAudioPrivate;
        }
   
    iModel.DataStore()->SetAudioOutputPreference( static_cast<EPSTelAudioOutput>(value) );
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::UpdateL
// -----------------------------------------------------------------------------
//
void CPEAudioOutputPreferenceMonitor::UpdateL()
    {
    TEFLOGSTRING( KTAINT, "PE CPEAudioOutputPreferenceMonitor::UpdateL" );
   
    //  Now retrieve the value
    TInt value;
    User::LeaveIfError(Get(value)); 
    
    TInt old = iModel.DataStore()->AudioOutputPreference();
    
    // If value has not changed, do not send message about change.
    // This reason for this check is that during phone startup sequence,
    // the key gets updated but phoneengine already has te value correctly.
    if (value != old)
        {
        iModel.DataStore()->SetAudioOutputPreference( static_cast<EPSTelAudioOutput>(value) );    
        iModel.SendMessage( MEngineMonitor::EPEMessageAudioOutputPreferenceChanged );
        }
    }
    
// End of file
