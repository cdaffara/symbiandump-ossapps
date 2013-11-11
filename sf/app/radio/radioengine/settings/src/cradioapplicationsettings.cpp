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

// System includes
#include <centralrepository.h>

// User includes
#include "cradiosettingsimp.h"
#include "radiointernalcrkeys.h"
#include "cradioapplicationsettings.h"
#include "cradioenginelogger.h"

// This has to be the last include.
#ifdef STUB_CONSTELLATION
#   include <RadioStubManager.h>
#endif //STUB_CONSTELLATION

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings* CRadioApplicationSettings::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioApplicationSettings* self = new ( ELeave ) CRadioApplicationSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioApplicationSettings::ConstructL()
    {
    LEVEL2( LOG_METHOD_AUTO );
    iRepository = CRepository::NewL( KRadioCRUid );
    LOG_FORMAT( "iRepository = %i", iRepository );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings::CRadioApplicationSettings()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings::~CRadioApplicationSettings()
    {
    LEVEL3( LOG_METHOD_AUTO );
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioApplicationSettings::IsFirstTimeStart()
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt startCount = 0;
    iRepository->Get( KRadioCRLaunchCount, startCount );

    return startCount == 0;
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
void CRadioApplicationSettings::SetFirstTimeStartPerformed( TBool aFirstTimeStartPerformed )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iRepository->Set( KRadioCRLaunchCount, aFirstTimeStartPerformed ? 1 : 0 );
    }


// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioApplicationSettings::SetUiFlags( TUint aUiFlags )
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRepository->Set( KRadioCRUiFlags, static_cast<TInt>( aUiFlags ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TUint CRadioApplicationSettings::UiFlags() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt uiFlags( 0 );
    iRepository->Get( KRadioCRUiFlags, uiFlags );
    return static_cast<TUint>( uiFlags );
    }
