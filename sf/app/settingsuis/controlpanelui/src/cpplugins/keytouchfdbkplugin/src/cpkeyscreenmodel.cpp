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


#include "cpkeyscreenmodel.h"
#include <qglobal.h>

#include "cpkeyscreenmodel_p.h"


/*
 * Constructor
 */
CpKeyScreenModel::CpKeyScreenModel()
{
    ptr = new CpKeyScreenModelPrivate();
}

/*
 * Desctructor
 */
CpKeyScreenModel::~CpKeyScreenModel()
{
    delete ptr;
    ptr = 0;
}

/*
 * Feature check for key guard setting
 */
bool CpKeyScreenModel::isKeyguardSupported()
{
    return ptr->isKeyguardSupported();
}

/*
 * Feature check for screen saver setting
 */
bool CpKeyScreenModel::isScreensaverSupported()
{
    return ptr->isScreensaverSupported();
}

/*
 * Returen key guard setting value
 */
int CpKeyScreenModel::keyguard()
{
    return ptr->keyguard();
}

/*
 * Set key guard setting value
 */
void CpKeyScreenModel::setKeyguard(int value)
{
    ptr->setKeyguard(value);
}

/*
 * Feature check for rotate setting 
 */
bool CpKeyScreenModel::isRotateSupported()
{
    return ptr->isRotateSupported();
}

/*
 * Return rotate setting value
 */
bool CpKeyScreenModel::rotate()
{
    return ptr->rotate();
}

/*
 * Set rotate setting value
 */
void CpKeyScreenModel::setRotate(bool value)
{
    ptr->setRotate(value);
}

/*
 * Return screen saver setting
 */
bool CpKeyScreenModel::screensaver()
{
    return ptr->screensaver();
}

/*
 * Set screen saver setting
 */
void CpKeyScreenModel::setScreensaver(bool value)
{
    ptr->setScreensaver(value);
}

/*
 * Feature check for the bright setting
 */
bool CpKeyScreenModel::isBrightnessSupported()
{
    return ptr->isBrightnessSupported();
}

/*
 * Return bright setting value
 */
int CpKeyScreenModel::brightness()
{
    return ptr->brightness(); 
}

/*
 * Set bright setting value
 */
void CpKeyScreenModel::setBrightness(int value)
{
    ptr->setBrightness(value);
}
// End of the file
