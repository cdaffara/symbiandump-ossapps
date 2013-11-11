/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CCustomPrefsData stores the Extended Connection Preferences
*
*/

// INCLUDE FILES
#include <eikenv.h>
#include "CustomPrefsData.h"
#include "extendedconnpref.h"


// ================= MEMBER FUNCTIONS =========================================
// ----------------------------------------------------------------------------
// CCustomPrefsData::NewL()
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CCustomPrefsData* CCustomPrefsData::NewL()
    {
    CCustomPrefsData* data = new(ELeave) CCustomPrefsData;
    CleanupStack::PushL(data);
    data->ConstructL();
    CleanupStack::Pop(data); // data
    return data;
    }

// ----------------------------------------------------------------------------
// CCustomPrefsData::~CCustomPrefsData()
// Destructor
// ----------------------------------------------------------------------------
//
CCustomPrefsData::~CCustomPrefsData()
    {
    }

// ----------------------------------------------------------------------------
// CCustomPrefsData::CCustomPrefsData()
// Constructor
// ----------------------------------------------------------------------------
//
CCustomPrefsData::CCustomPrefsData() :
    iSnapPurpose( CMManager::ESnapPurposeUnknown ),
    iSnapId( 0 ),
    iIapId( 0 ),
    iBearerSet( TExtendedConnPref::EExtendedConnBearerUnknown ),
    iNoteBehaviour( TExtendedConnPref::ENoteBehaviourDefault ),
    iDisconnectDialog( ETrue ),
    iConnSelectionDialog( EFalse ),
    iForcedRoaming( ETrue )
    {
    }

// ----------------------------------------------------------------------------
// CCustomPrefsData::CCustomPrefsData()
// Symbian default constructor
// ----------------------------------------------------------------------------
//
void CCustomPrefsData::ConstructL()
    {
    }

// End of File
