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
* Description: CSettingsData stores the settings for ConnTest
*
*/

// INCLUDE FILES
#include <eikenv.h>
#include "SettingData.h"


// ================= MEMBER FUNCTIONS =========================================
// ----------------------------------------------------------------------------
// CSettingData::NewL()
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CSettingData* CSettingData::NewL()
    {
    CSettingData* data = new(ELeave) CSettingData;
    CleanupStack::PushL(data);
    data->ConstructL();
    CleanupStack::Pop(data); // data
    return data;
    }

// ----------------------------------------------------------------------------
// CSettingData::~CSettingData()
// Destructor
// ----------------------------------------------------------------------------
//
CSettingData::~CSettingData()
    {
    }

// ----------------------------------------------------------------------------
// CSettingData::CSettingData()
// Constructor
// ----------------------------------------------------------------------------
//
CSettingData::CSettingData()
    {
    }

// ----------------------------------------------------------------------------
// CSettingData::CSettingData()
// Epoc default constructor
// ----------------------------------------------------------------------------
//
void CSettingData::ConstructL()
    {
    //CEikonEnv* env = CEikonEnv::Static();

    // Default data
    iServerName.Copy(_L("www.google.com"));
    iPort = 80;
    iPacketSize = 5;
    iPackets = 1;
    iDelay = 500;
    iProtocol = 0;
    iRoaming = 1;
    iHttpPage.Copy(_L("index.html"));
    }

// End of File
