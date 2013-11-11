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
* Description:  mmsschedulesettingsutils implementation
*
*/


   
// INCLUDE FILES   
#include <centralrepository.h>
#include <msvids.h>
#include <msvschedulesettings.h>
#include <msvoffpeaktime.h>
#include <msvsenderroraction.h>
#include <msvsysagentaction.h>

// USERINCLUDE FILES
#include "mmsschedulesettingsutils.h"
#include "mmsdebuglogging.h"
#include "MmsEnginePrivateCRKeys.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ============================== LOCAL FUNCTIONS ==============================

// ============================== MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// LoadScheduleSendSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadScheduleSendSettingsL(
    CMsvScheduleSettings& aScheduleSettings,
    CRepository& aRepository )
    {
    LOG(_L("TMmsScheduleSettingsUtils::LoadScheduleSendSettingsL"));
    
    LoadScheduleSettingsL(
        aScheduleSettings,
        aRepository,
        KMmsScheduleSendLongInterval,
        KMmsScheduleSendShortInterval,
        KMmsScheduleSendVariableIntervalsCount,
        KMmsScheduleSendVariableIntervalArrayBase );
       
    }

// -----------------------------------------------------------------------------
// LoadScheduleReceiveSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadScheduleReceiveSettingsL(
    CMsvScheduleSettings& aScheduleSettings,
    CRepository& aRepository )
    {
    LOG(_L("TMmsScheduleSettingsUtils::LoadScheduleReceiveSettingsL"));
    
    LoadScheduleSettingsL(
        aScheduleSettings,
        aRepository,
        KMmsScheduleRecvLongInterval,
        KMmsScheduleRecvShortInterval,
        KMmsScheduleRecvVariableIntervalsCount,
        KMmsScheduleRecvVariableIntervalArrayBase );
       
    }
    
// -----------------------------------------------------------------------------
// LoadScheduleSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadScheduleSettingsL(
    CMsvScheduleSettings& aScheduleSettings,
    CRepository& aRepository,
    const TUint32 aLongInterval,
    const TUint32 aShortInterval,
    const TUint32 aVariableIntervalsCount,
    const TUint32 aVariableIntervalArrayBase )
    {
	aScheduleSettings.Reset();

	TInt temp = 0;
	User::LeaveIfError( aRepository.Get( KMmsSchedulePriority, temp ) );
	aScheduleSettings.SetPriority( temp );
		
	User::LeaveIfError( aRepository.Get( KMmsScheduleValidityPeriod, temp ) );
	aScheduleSettings.SetValidityPeriod( static_cast<TTimeIntervalMinutes>( temp ) );
		
	User::LeaveIfError( aRepository.Get( KMmsScheduleIntervalType, temp ) );
	aScheduleSettings.SetIntervalType( static_cast<TIntervalType>( temp ) );
		
	User::LeaveIfError( aRepository.Get( KMmsScheduleLatency, temp ) );
	aScheduleSettings.SetLatency( static_cast<TTimeIntervalMicroSeconds32>( temp ) );

	User::LeaveIfError( aRepository.Get( aLongInterval, temp ) );
	aScheduleSettings.SetLongInterval( static_cast<TTimeIntervalSeconds>( temp ) );

	User::LeaveIfError( aRepository.Get( aShortInterval, temp) );
	aScheduleSettings.SetShortInterval( static_cast<TTimeIntervalSeconds>( temp ) );

	User::LeaveIfError( aRepository.Get( KMmsSchedulePendingConditionsTimeout, temp ) );
	aScheduleSettings.SetPendingConditionsTimeout(static_cast<TTimeIntervalMinutes>(temp));

	User::LeaveIfError( aRepository.Get( aVariableIntervalsCount, temp ) );
	TInt count = temp;

	if( count > 0 )
		{		
		TUint32 variableIntervalId = aVariableIntervalArrayBase;
	    CArrayFixFlat<TTimeIntervalSeconds>* intervals 
	        = new(ELeave) CArrayFixFlat<TTimeIntervalSeconds>( count );    
	    CleanupStack::PushL( intervals ); // ***
		for( TInt cur = 0; cur < count; cur++ )
			{
			User::LeaveIfError(aRepository.Get(variableIntervalId, temp));
			intervals->AppendL(static_cast<TTimeIntervalSeconds>(temp));
			variableIntervalId++;
			}
			
		if( intervals->Count() > 0 )
			{		
	   		aScheduleSettings.SetVariableIntervalsL( *intervals );
			}
		CleanupStack::PopAndDestroy( intervals );
		}
    }
    
    	    
// -----------------------------------------------------------------------------
// LoadOffPeakSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadOffPeakSettingsL(
    CMsvOffPeakTimes& /*aOffPeakTimes*/, 
    CRepository& /*aRepository*/ )
    {
    // DOES NOTHING
    }
        	
// -----------------------------------------------------------------------------
// LoadSendErrorSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadSendErrorSettingsL(
    CMsvSendErrorActions& aErrorActions, 
    CRepository& aRepository )
    {
    LOG(_L("TMmsScheduleSettingsUtils::LoadSendErrorSettingsL"));
    
    LoadErrorSettingsL( aErrorActions,
        aRepository,
        KMmsScheduleSendErrorDefaultAction,
        KMmsScheduleSendErrorActionsCount,
        KMmsScheduleSendErrorActionsBase );
        
    }

// -----------------------------------------------------------------------------
// LoadReceiveErrorSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadReceiveErrorSettingsL(
    CMsvSendErrorActions& aErrorActions, 
    CRepository& aRepository )
    {
    LOG(_L("TMmsScheduleSettingsUtils::LoadReceiveErrorSettingsL"));
    
    LoadErrorSettingsL( aErrorActions,
        aRepository,
        KMmsScheduleRecvErrorDefaultAction,
        KMmsScheduleRecvErrorActionsCount,
        KMmsScheduleRecvErrorActionsBase );
    
    }
    
// -----------------------------------------------------------------------------
// LoadErrorSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadErrorSettingsL(
    CMsvSendErrorActions& aErrorActions, 
    CRepository& aRepository,
    const TUint32 aDefaultErrorActionId,
    const TUint32 aErrorActionCount,
    const TUint32 aErrorActionBase )
    {
	TMsvSendErrorAction action;
	LoadErrorActionL( aDefaultErrorActionId, action, aRepository );
	aErrorActions.SetDefault( action );

	LoadImmediateFailuresL( aErrorActions, aRepository );

	TInt temp = 0;
	User::LeaveIfError( aRepository.Get( aErrorActionCount, temp ) );	
	TInt count = temp;
		
	TUint32 errorActionId = aErrorActionBase;
	LOG2(_L("- loading %d error actions"), count);
	for( TInt counter = 0; counter < count; ++counter )
		{
		LoadErrorActionL( errorActionId, action, aRepository );
		aErrorActions.AddSendErrorActionL( action );		
		errorActionId += KMmsErrorActionElementIncrement;
		}
    }
    
        	
// -----------------------------------------------------------------------------
// LoadSysAgentSettingsL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadSysAgentSettingsL(
    CMsvSysAgentActions& /*aSysAgentActions*/, 
    CRepository& /*aRepository*/ )
    {
    // DOES NOTHING
    }


// -----------------------------------------------------------------------------
// LoadErrorActionL
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadErrorActionL(
    TUint32 aErrorActionId, 
    TMsvSendErrorAction& aErrorAction, 
    CRepository& aRepository )
	{
	aErrorAction.Reset();

	TInt temp = 0;
	TUint32 settingId = aErrorActionId + KMmsErrorActionError;
	User::LeaveIfError( aRepository.Get( settingId, temp ) );
	aErrorAction.iError = temp;
		
	settingId = aErrorActionId + KMmsErrorActionMaxRetries;
	User::LeaveIfError( aRepository.Get(settingId, temp ) );
	aErrorAction.SetMaxRetries( static_cast<TInt16>( temp ) );

	settingId = aErrorActionId + KMmsErrorActionRetries;
	User::LeaveIfError( aRepository.Get( settingId, temp ) );
	aErrorAction.iRetries = static_cast<TMsvSendRetries>( temp );

	settingId = aErrorActionId + KMmsErrorActionAction;
	User::LeaveIfError( aRepository.Get( settingId, temp ) );
	aErrorAction.iAction = static_cast<TMsvSendAction>( temp );

	settingId = aErrorActionId + KMmsErrorActionRetrySpacing;
	User::LeaveIfError( aRepository.Get( settingId, temp ) );
	aErrorAction.iRetrySpacing = static_cast<TMsvSendRetrySpacing>( temp );
	
    LOG2(_L("- load error action for error value %d"), aErrorAction.iError );
	}
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TMmsScheduleSettingsUtils::LoadImmediateFailuresL(
    CMsvSendErrorActions& aErrorActions, 
    CRepository& aRepository )
    {
    
    TInt temp = 0;
	TMsvSendErrorAction action;
    
	// Immediate failure actions
	action.iAction = ESendActionFail;
	action.iError = KErrNotFound;
	action.SetMaxRetries( 0 );
	action.iRetries = ESendRetriesFixed;
	action.iRetrySpacing = ESendRetrySpacingStatic;
	
	TInt count = 0;
	User::LeaveIfError( aRepository.Get( KMmsImmediateFailureErrorCount, temp ) );
    count = temp;
    
    TInt i = 0;
	LOG2(_L("- loading %d immediate failures error"), count);
    for (i = 0; i < count; ++i )
        {
        User::LeaveIfError( aRepository.Get( KMmsImmediateFailureErrorBase + i, temp ) );
        action.iError = temp;
		aErrorActions.AddSendErrorActionL( action );		
        }
        
    }
	
    
// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
