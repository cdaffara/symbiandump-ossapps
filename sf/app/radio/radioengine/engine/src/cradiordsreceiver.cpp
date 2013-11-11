/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes
#include "cradiordsreceiver.h"
#include "mradioenginesettings.h"
#include "mradiordsdataobserver.h"
#include "cradioenginelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver::CRadioRdsReceiver( MRadioEngineSettings& aSettings )
    : CRadioRdsReceiverBase( aSettings )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver* CRadioRdsReceiver::NewL( MRadioEngineSettings& aSettings )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioRdsReceiver* self = new ( ELeave ) CRadioRdsReceiver( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver::~CRadioRdsReceiver()
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( iRdsUtility )
        {
        iRdsUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::InitL( CRadioUtility& aRadioUtility )
    {
    LOG_METHOD_AUTO;

    iRdsUtility = &aRadioUtility.RadioRdsUtilityL( *this );

    LogReceiverCapabilities();

    SetAutomaticSwitchingL( EFalse );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::SetAutomaticSwitchingL( TBool aEnable )
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( iRdsUtility )
        {
        User::LeaveIfError( iRdsUtility->SetAutomaticSwitching( aEnable ) );
        }

    // Notifies the observers
    CRadioRdsReceiverBase::SetAutomaticSwitchingL( aEnable );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::StartReceiver()
    {
    LOG_METHOD_AUTO;
    // Request to be notified of almost all RDS data.
    // Unwanted RDS values:
    //    ERdsClockTime
    //    ERdsTrafficProgramme
    //    ERdsTrafficAnnouncement
    //    ERdsAlternateFrequency
    TRdsData rdsData;
    rdsData.iRdsFunctions = ERdsProgrammeIdentification |
                            ERdsProgrammeType |
                            ERdsProgrammeService |
                            ERdsRadioText |
                            ERdsRadioTextPlus;

    rdsData.iAdditionalFunctions1 = 0;
    rdsData.iAdditionalFunctions2 = 0;

    LOG_ASSERT( iRdsUtility, LOG( "Error: RDS utility not created!" ) );
    if ( iRdsUtility && !iStarted )
        {
        TInt err = iRdsUtility->NotifyRdsDataChange( rdsData );
        if ( !err )
            {
            // Avoid further calls
            iStarted = ETrue;
            LOG( "Requested RDS notifications from receiver." );
            }
        else
            {
            LOG_FORMAT( "Failed to request RDS data with err %d", err );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::StopReceiver()
    {
    LOG_METHOD_AUTO;

    if ( iRdsUtility )
        {
        iRdsUtility->CancelNotifyRdsDataChange();
        }
    iStarted = EFalse;
    }

// ---------------------------------------------------------------------------
// Logs the RDS receiver capabilities if logging is enabled
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::LogReceiverCapabilities()
    {
    LEVEL3( LOG_METHOD_AUTO );
#ifdef LOGGING_ENABLED
    // Log the RDS utility capabilities
    if ( iRdsUtility )
        {
        TRdsCapabilities caps;
        caps.iRdsFunctions = 0;
        caps.iAdditionalFunctions1 = 0;
        caps.iAdditionalFunctions2 = 0;
        TInt err = iRdsUtility->GetCapabilities( caps );
        if ( !err )
            {
            TBuf<100> capsBuf;
            if ( caps.iRdsFunctions & ERdsProgrammeIdentification == ERdsProgrammeIdentification )
                {
                capsBuf.Append( _L("PI ") );
                }
            if ( caps.iRdsFunctions & ERdsProgrammeType == ERdsProgrammeType )
                {
                capsBuf.Append( _L("Genre ") );
                }
            if ( caps.iRdsFunctions & ERdsProgrammeService == ERdsProgrammeService )
                {
                capsBuf.Append( _L("PS ") );
                }
            if ( caps.iRdsFunctions & ERdsRadioText == ERdsRadioText )
                {
                capsBuf.Append( _L("RT ") );
                }
            if ( caps.iRdsFunctions & ERdsRadioTextPlus == ERdsRadioTextPlus )
                {
                capsBuf.Append( _L("RT+ ") );
                }
            if ( caps.iRdsFunctions & ERdsAlternateFrequency == ERdsAlternateFrequency )
                {
                capsBuf.Append( _L("AF ") );
                }
            if ( caps.iRdsFunctions & ERdsClockTime == ERdsClockTime )
                {
                capsBuf.Append( _L("Time ") );
                }
            if ( caps.iRdsFunctions & ERdsTrafficProgramme == ERdsTrafficProgramme )
                {
                capsBuf.Append( _L("TrafficProgramme ") );
                }
            if ( caps.iRdsFunctions & ERdsTrafficAnnouncement == ERdsTrafficAnnouncement )
                {
                capsBuf.Append( _L("TrafficAnnouncement ") );
                }
            LOG_FORMAT( "RDS receiver capabilities: %S", &capsBuf );
            LOG_FORMAT( "Functions: %d, AdditionalFunctions1: %d, iAdditionalFunctions2: %d",
                    caps.iRdsFunctions, caps.iAdditionalFunctions1, caps.iAdditionalFunctions2 );
            }
        else
            {
            LOG_FORMAT( "Failed to get RDS utility capabilities: err %d", err );
            }
        }
#endif // LOGGING_ENABLED
    }
