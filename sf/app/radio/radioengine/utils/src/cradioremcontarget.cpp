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
#include "cradioremcontarget.h"
#include "cradioremcontargetimp.h"
#include "cradioenginelogger.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRemConTarget* CRadioRemConTarget::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    return CRadioRemConTargetImp::NewL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTarget::~CRadioRemConTarget()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }
