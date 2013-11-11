/** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "tstasksettings.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TsTaskSettings::TsTaskSettings
// -----------------------------------------------------------------------------
//
TsTaskSettings::TsTaskSettings(  )
    {
    
    }


// -----------------------------------------------------------------------------
// TsTaskSettings::~TsTaskSettings
// -----------------------------------------------------------------------------
//
TsTaskSettings::~TsTaskSettings(  )
    {
    
    }


// -----------------------------------------------------------------------------
// TsTaskSettings::registerScreenshot
// -----------------------------------------------------------------------------
//
bool TsTaskSettings::registerScreenshot( 
        const QPixmap & screenshot )
    {
    SMC_MOCK_METHOD1( bool, const QPixmap &, screenshot )
    }


// -----------------------------------------------------------------------------
// TsTaskSettings::unregisterScreenshot
// -----------------------------------------------------------------------------
//
bool TsTaskSettings::unregisterScreenshot(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// TsTaskSettings::setVisibility
// -----------------------------------------------------------------------------
//
bool TsTaskSettings::setVisibility( bool Visibility )
    {
    SMC_MOCK_METHOD0( bool )
    }


