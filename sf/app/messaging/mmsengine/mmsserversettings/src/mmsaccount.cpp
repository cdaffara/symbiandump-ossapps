/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsaccount implementation
*
*/


   
// INCLUDE FILES   
#include <centralrepository.h>
#include <msvids.h>

// USERINCLUDE FILES
#include "mmsaccount.h"
#include "mmssettings.h"
#include "mmsdebuglogging.h"
#include "mmsschedulesettingsutils.h"
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
// CMmsAccount::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsAccount* CMmsAccount::NewL()
	{
	CMmsAccount* self = new(ELeave) CMmsAccount();
	CleanupStack::PushL( self );
	self->ConstructL();
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMmsAccount::CMmsAccount
//
// -----------------------------------------------------------------------------
//
CMmsAccount::CMmsAccount()
	{
	}

// -----------------------------------------------------------------------------
// CMmsAccount::~CMmsAccount
//
// -----------------------------------------------------------------------------
//
CMmsAccount::~CMmsAccount()
	{
    LOG(_L("~CMmsAccount"));
	}

// -----------------------------------------------------------------------------
// CMmsAccount::ConstructL
//
// -----------------------------------------------------------------------------
//
void CMmsAccount::ConstructL()
	{
    LOG(_L("CMmsAccount::ConstructL"));
	}

// -----------------------------------------------------------------------------
// CMmsAccount::LoadSettingsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsAccount::LoadSettingsL( CMmsSettings& aMmsSettings )
    {
    LOG(_L("CMmsAccount::LoadSettingsL"));
    aMmsSettings.LoadSettingsL();
    }

// -----------------------------------------------------------------------------
// CMmsAccount::SaveSettingsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsAccount::SaveSettingsL( CMmsSettings& aMmsSettings )
    {
    LOG(_L("CMmsAccount::SaveSettingsL"));
    aMmsSettings.SaveSettingsL();
    }

// -----------------------------------------------------------------------------
// CMmsAccount::LoadScheduleSendSettingsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsAccount::LoadScheduleSendSettingsL(
    CMsvScheduleSettings& aScheduleSettings,
    CMsvOffPeakTimes& aOffPeakTimes,
    CMsvSendErrorActions& aErrorActions,
    CMsvSysAgentActions& aSysAgentActions )
    {
    LOG(_L("CMmsAccount::LoadSendScheduleSettingsL"));

    //
    // Connect to repository
    //
    CRepository* repository;
    repository = CRepository::NewLC( KCRUidMmsEngineRegistryId ); // ***
    
    //
    // Load settings
    //
    TMmsScheduleSettingsUtils::LoadScheduleSendSettingsL( aScheduleSettings, *repository );
    TMmsScheduleSettingsUtils::LoadOffPeakSettingsL( aOffPeakTimes, *repository ); // does nothing
    TMmsScheduleSettingsUtils::LoadSendErrorSettingsL( aErrorActions, *repository );
    TMmsScheduleSettingsUtils::LoadSysAgentSettingsL( aSysAgentActions, *repository ); // does nothing
    
    CleanupStack::PopAndDestroy( repository );
    }

// -----------------------------------------------------------------------------
// CMmsAccount::LoadScheduleReceiveSettingsL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsAccount::LoadScheduleReceiveSettingsL(
    CMsvScheduleSettings& aScheduleSettings,
    CMsvOffPeakTimes& aOffPeakTimes,
    CMsvSendErrorActions& aErrorActions,
    CMsvSysAgentActions& aSysAgentActions )
    {
    LOG(_L("CMmsAccount::LoadReceiveScheduleSettingsL"));

    //
    // Connect to repository
    //
    CRepository* repository;
    repository = CRepository::NewLC( KCRUidMmsEngineRegistryId ); // ***
    
    //
    // Load settings
    //
    TMmsScheduleSettingsUtils::LoadScheduleReceiveSettingsL( aScheduleSettings, *repository );
    TMmsScheduleSettingsUtils::LoadOffPeakSettingsL( aOffPeakTimes, *repository ); // does nothing
    TMmsScheduleSettingsUtils::LoadReceiveErrorSettingsL( aErrorActions, *repository );
    TMmsScheduleSettingsUtils::LoadSysAgentSettingsL( aSysAgentActions, *repository ); // does nothing
    
    CleanupStack::PopAndDestroy( repository );
    }
    
// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
