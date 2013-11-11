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

#include <QVariant>
#include <QKeyEvent>

#include "cxuicapturekeyhandler.h"
#ifdef Q_OS_SYMBIAN
#include "cxuicapturekeyhandler_symbian_p.h"
#else
#include "cxuicapturekeyhandler_desktop_p.h"
#endif

#include "cxutils.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"



CxuiCaptureKeyHandler::CxuiCaptureKeyHandler(CxeEngine &aEngine) :
        d_ptr(NULL)
{
    d_ptr = new CxuiCaptureKeyHandlerPrivate(aEngine, this);
    
    d_ptr->startListeningKeys();
}

CxuiCaptureKeyHandler::~CxuiCaptureKeyHandler()
{    
    CX_DEBUG_ENTER_FUNCTION();

    Q_D(CxuiCaptureKeyHandler);
    d->stopListeningKeys();

    delete d;

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiCaptureKeyHandler::startListeningKeys()
{
    Q_D(CxuiCaptureKeyHandler);
    d->startListeningKeys();
}

void CxuiCaptureKeyHandler::stopListeningKeys()
{
    Q_D(CxuiCaptureKeyHandler);
    d->stopListeningKeys();
}

bool CxuiCaptureKeyHandler::isAutofocusKeyPressed()
{
    Q_D(CxuiCaptureKeyHandler);
    return d->isAutofocusKeyPressed();
}

bool CxuiCaptureKeyHandler::isCaptureKeyPressed()
{
    Q_D(CxuiCaptureKeyHandler);
    return d->isCaptureKeyPressed();
}

bool CxuiCaptureKeyHandler::handleKeyEvent(QEvent *event)
{
    Q_D(CxuiCaptureKeyHandler);
    return d->handleKeyEvent(event);
}

// end of file
