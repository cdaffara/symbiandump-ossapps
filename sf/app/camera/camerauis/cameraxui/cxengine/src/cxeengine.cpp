/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxutils.h"
#include "cxeengine.h"
#ifdef Q_OS_SYMBIAN
#include "cxeenginesymbian.h"
#else
#include "cxeenginedesktop.h"
#endif

// This should be the only exported method
CAMERAX_ENGINE_EXPORT CxeEngine* CxeEngine::createEngine()
{
    CX_DEBUG_ENTER_FUNCTION();

#ifdef Q_OS_SYMBIAN
    CxeEngineSymbian *res = new CxeEngineSymbian();
#else // Q_OS_SYMBIAN
    CxeEngineDesktop *res = new CxeEngineDesktop();
#endif // Q_OS_SYMBIAN

    res->construct();
    CX_DEBUG_EXIT_FUNCTION();
    return res;
}

CxeEngine::CxeEngine()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeEngine::~CxeEngine()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}
