/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles phone indicators.
*
*/

#include <qglobal.h>

#include "lcvtindicatorcontroller.h"
#include "lcvtindicatorcontrollerimpl.h"

    
EXPORT_C LcVtIndicatorController::LcVtIndicatorController()
{
    mIndicatorimpl = new LcVtIndicatorControllerImpl();
}

EXPORT_C LcVtIndicatorController::~LcVtIndicatorController()
{
    delete mIndicatorimpl;
}


EXPORT_C void LcVtIndicatorController::enableActiveCallIndicator()
{
    mIndicatorimpl->enableActiveCallIndicator();
}

EXPORT_C void LcVtIndicatorController::disableActiveCallIndicator()
{
    mIndicatorimpl->disableActiveCallIndicator();
 
}

// End of file

