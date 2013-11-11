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
#include "cradioengine.h"
#include "cradioengineimp.h"
#include "cradioenginelogger.h"
#include "radioengineutils.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Starts the radio engine initialization.
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioEngine* CRadioEngine::NewL()
    {
    RadioEngineUtils::InitializeL();
    LEVEL3( LOG_METHOD_AUTO );

    return CRadioEngineImp::NewL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngine::CRadioEngine( CRadioAudioRouter* aAudioRouter )
    : CRadioRoutableAudio( aAudioRouter )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioEngine::~CRadioEngine()
    {
    RadioEngineUtils::Release();
    }
