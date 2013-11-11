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
* Description: Implementation of applicationmanagement components
*     
*
*/


// INCLUDE FILES
#include "ApplicationManagementRfs.h"
#include "RfsAppMgmtPlugin.h"
#include "debug.h"

_LIT(KScriptPath, "");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CRfsAppMgmtPlugin::CRfsAppMgmtPlugin()
    {
    }

CRfsAppMgmtPlugin::CRfsAppMgmtPlugin(TAny* /*aInitParams*/)
    {
    }

// Destructor
CRfsAppMgmtPlugin::~CRfsAppMgmtPlugin()
    {
    }

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CRfsAppMgmtPlugin* CRfsAppMgmtPlugin::NewL(TAny* aInitParams)
    {
    RDEBUG("CRfsAppMgmtPlugin::NewL()");
    CRfsAppMgmtPlugin* self = new (ELeave) CRfsAppMgmtPlugin(aInitParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CRfsAppMgmtPlugin::ConstructL()
    {
    RDEBUG("CRfsAppMgmtPlugin::ConstructL()");
    }

void CRfsAppMgmtPlugin::RestoreFactorySettingsL( const TRfsReason /*aType*/ )
    {
    RDEBUG("CRfsAppMgmtPlugin::RestoreFactorySettingsL()");
    RAppMgmtRfs appManagement;
    if( appManagement.Connect() == KErrNone )
        {
        RDEBUG("CRfsAppMgmtPlugin::RestoreFactorySettingsL(): Connected to AppMgmt ");
        appManagement.PerformRfsL();
        appManagement.Close();
        }
    else
        {
        RDEBUG("CStartupAppUi::RTCStatus(): Unable to connect to AppMgmt ");
        }
    }

void CRfsAppMgmtPlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    aPath.Copy( KScriptPath);
    }

void CRfsAppMgmtPlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/,
                                        TDesC& /*aCommand*/ )
    {
    }

// End of file
