/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "cpkeyscreenmodel_p.h"


CpKeyScreenModelPrivate::CpKeyScreenModelPrivate()
{
    
}

CpKeyScreenModelPrivate::~CpKeyScreenModelPrivate()
{
   
}

bool CpKeyScreenModelPrivate::isKeyguardSupported()
{
    return true;
}

int CpKeyScreenModelPrivate::keyguard()
{
    return 0;
}

void CpKeyScreenModelPrivate::setKeyguard(int value)
{
    Q_UNUSED(value)
}

bool CpKeyScreenModelPrivate::isRotateSupported()
{
    return true;
}

bool CpKeyScreenModelPrivate::rotate()
{
    return false;
}

void CpKeyScreenModelPrivate::setRotate(bool value)
{
   Q_UNUSED(value)
}

bool CpKeyScreenModelPrivate::screensaver()
{
    return false;
}

void CpKeyScreenModelPrivate::setScreensaver(bool value)
{
    Q_UNUSED(value)
}
bool CpKeyScreenModelPrivate::isScreensaverSupported()
{    
    return true;
}

bool CpKeyScreenModelPrivate::isBrightnessSupported()
{
    return true;
}

int CpKeyScreenModelPrivate::brightness()
{
    return 0;
}

void CpKeyScreenModelPrivate::setBrightness(int value)
{
    Q_UNUSED(value)
}

// End of the file
