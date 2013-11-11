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
#include <QDebug>
#include <smcmockclassincludes.h>
#include "cppluginlauncher.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CpPluginLauncher::launchCpPluginView
// -----------------------------------------------------------------------------
//
bool CpPluginLauncher::launchCpPluginView( 
        const QString & pluginFile,
        int index )
    {
    SMC_MOCK_METHOD2( bool, const QString &, pluginFile, 
        int, index )
    }


// -----------------------------------------------------------------------------
// CpPluginLauncher::launchSettingView
// -----------------------------------------------------------------------------
//
CpBaseSettingView * CpPluginLauncher::launchSettingView( 
        const QString & pluginFile,
        const QVariant & hint )
    {
    SMC_MOCK_METHOD2( CpBaseSettingView *, const QString &, pluginFile, 
        const QVariant &, hint )
    }


