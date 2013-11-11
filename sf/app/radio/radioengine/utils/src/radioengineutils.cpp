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
#include <StringLoader.h>

// User includes
#include "radioengineutils.h"
#include "cradioenginetls.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RadioEngineUtils::InitializeL()
    {
    CRadioEngineTls::InitializeL();
    CRadioEngineTls::Instance().AddRef();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RadioEngineUtils::Release()
    {
    CRadioEngineTls::Instance().Release();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C MRadioEngineLogger* RadioEngineUtils::Logger()
    {
    return CRadioEngineTls::Instance().Logger();
    }

// ---------------------------------------------------------------------------
// Returns the file server session
// ---------------------------------------------------------------------------
//
EXPORT_C RFs& RadioEngineUtils::FsSession()
    {
    return CRadioEngineTls::Instance().FsSession();
    }
