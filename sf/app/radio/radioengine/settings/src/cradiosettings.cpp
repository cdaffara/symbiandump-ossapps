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
#include "cradiosettings.h"
#include "cradiosettingsimp.h"
#include "cradioenginelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioSettings* CRadioSettings::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    return CRadioSettingsImp::NewL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioSettings::~CRadioSettings()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }
