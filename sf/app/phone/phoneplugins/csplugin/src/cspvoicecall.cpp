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
* Description:  Implements class CSPVoiceCall which provides call functionality
*
*/


#include <etelmm.h>
#include <etel.h>
#include <mccpcallobserver.h>
#include <cccecallparameters.h>

#include "cspvoicecall.h"
#include "cspetelcallrequester.h"
#include "cspetelcalleventmonitor.h"
#include "cspetelcallstatusmonitor.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"
#include "mcspcommoninfo.h"


// ---------------------------------------------------------------------------
// CSPVoiceCall::NewL Two phased construction.
// ---------------------------------------------------------------------------
//
CSPVoiceCall* CSPVoiceCall::NewL( const TDesC& aName, 
                          RMobileLine& aLine, TBool aMobileOriginated, 
                          const CCCECallParameters& aParams,
                          MCSPCommonInfo& aCommonInfo,
                          TBool aIsEmergency )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::NewL <");
    
    CSPVoiceCall* self = new ( ELeave ) CSPVoiceCall( aLine, 
                                            aMobileOriginated,
                                            aName,
                                            aCommonInfo,
                                            aIsEmergency );
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::NewL >");
    return self;
    }
    
// ---------------------------------------------------------------------------
// CSPVoiceCall::~CSPVoiceCall
// ---------------------------------------------------------------------------
//
CSPVoiceCall::~CSPVoiceCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::~CSPVoiceCall");
    }

// ---------------------------------------------------------------------------
// CSPVoiceCall::GoOneToOneL
// Goes one to one from conference.
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::GoOneToOne( )
    {
    CSPLOGSTRING(CSPREQIN, "CSPVoiceCall::GoOneToOne");
    return iRequester->MakeRequest( 
        CSPEtelCallRequester::ERequestTypeGoOneToOne );
    }
    
// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPVoiceCall::Dial
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::DialFdnCond( TBool aFdnCond )
    {
    CSPLOGSTRING(CSPREQIN, "CSPVoiceCall::DialFdnCond <");

    if ( !iMobileOriginated )
        {
        CSPLOGSTRING(CSPINT, 
                "CSPVoiceCall::DialFdnCond MT CALL, Dial not supported");
        return KErrNotSupported;
        }

    CSPLOGSTRING(CSPINT, "CSPVoiceCall::DialFdnCond making request");

    SetDefaultCallParameters();
    iCommonInfo.InitializeCallParameters( iCallParams );
    iCallParams.iSubAddress = iParams->SubAddress();
    iCallParams.iBearerCap1 = iParams->Bearer();
    
    if ( CCCECallParameters::ECCECallOriginSAT == iParams->Origin() )
        {
        iCallParams.iCallParamOrigin = RMobileCall::EOriginatorSIM;
        }

    TInt err(KErrNone);
    if ( aFdnCond )
        {
        err = iRequester->MakeDialRequest( iCallParamsPckg, iRemotePartyNumber );
        }
    else
        {
        err = iRequester->MakeDialNoFdnCheckRequest( 
                        iCallParamsPckg, iRemotePartyNumber );
        }
    CSPLOGSTRING2(CSPINT, "CSPVoiceCall::DialFdnCond > res: %d", err );
    return err;    
    }

// ---------------------------------------------------------------------------
// Make the DialEmergencyCall request to ETel ( impl. of MCCPEmergencyCall )
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::Dial( const TDesC& aEmergencyNumber )
    {    
    CSPLOGSTRING2(CSPREQIN, "CSPVoiceCall::Dial EMERGENCY %S", &aEmergencyNumber);
    iDialCompletionCode = KErrNone;
    iTerminationErrorNotified = EFalse;
    iIsEmergencyCall = ETrue;
    iEmergencyNumber.Zero(); 
    TInt emergencyNumberLength = aEmergencyNumber.Length(); 
    
    if ( emergencyNumberLength > 0 && 
        emergencyNumberLength <= RMobileENStore::KEmergencyNumberSize )
        {
        iEmergencyNumber.Append( aEmergencyNumber );
        }
    else if ( emergencyNumberLength > RMobileENStore::KEmergencyNumberSize )
        {                
        CSPLOGSTRING(CSPERROR, "CSPVoiceCall::Dial invalid argument" );
        return KErrArgument; 
        }
    
    CSPLOGSTRING2(CSPREQIN, "CSPVoiceCall::Dial using %S", &iEmergencyNumber);
    
    TInt err( iRequester->MakeDialEmergencyRequest( iEmergencyNumber ) );

    CSPLOGSTRING2(CSPINT, "CSPVoiceCall::Dial res %d", err );
    return err;
    }
    
// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPVoiceCall::Answer
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::Answer()
    {
    CSPLOGSTRING(CSPREQIN, "CSPVoiceCall::Answer");    
    return CSPCall::Answer();
    }
    
// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::HangUp
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::HangUp()
    {
    CSPLOGSTRING(CSPREQIN, "CSPVoiceCall::HangUp");    
    return CSPCall::HangUp();   
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPVoiceCall::Cancel
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::Cancel()
    {
    CSPLOGSTRING(CSPREQIN, "CSPVoiceCall::Cancel");    
    return CSPCall::Cancel();
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPVoiceCall::SwitchAlternatingCall
// ---------------------------------------------------------------------------
//
TInt CSPVoiceCall::SwitchAlternatingCall()
    {
    CSPLOGSTRING(CSPERROR, "CSPVoiceCall::SwitchAlternatingCall not supported!");
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CSPVoiceCall::CSPVoiceCall
// ---------------------------------------------------------------------------
//
CSPVoiceCall::CSPVoiceCall( RMobileLine& aLine, 
                  TBool aMobileOriginated,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo,
                  TBool aIsEmergency ):CSPCall( aLine,
                     aMobileOriginated, aName, 
                     aCommonInfo, aIsEmergency ),
                     iCallParams(),
                     iCallParamsPckg (iCallParams)
                     
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::CSPVoiceCall");
    }

// ---------------------------------------------------------------------------
// CSPVoiceCall::ConstructL
// Constructing CSPVoiceCall for MT call.
// ---------------------------------------------------------------------------
//    
void CSPVoiceCall::ConstructL( const CCCECallParameters& aParams )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::ConstructL <");
    CSPCall::ConstructL( aParams );
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::ConstructL >");
    }

// ---------------------------------------------------------------------------
// CSPVoiceCall::SetDefaultCallParameters
// ---------------------------------------------------------------------------
//
void CSPVoiceCall::SetDefaultCallParameters()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPVoiceCall::SetDefaultCallParameters");
    
    //Set defaults
    iCallParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOn;
    iCallParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeMedium;
    iCallParams.iInterval = KCSPDefaultInterval;
    iCallParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
    iCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    iCallParams.iAutoRedial = KCSPDefaultAutoRedial;
    }

// End of File
