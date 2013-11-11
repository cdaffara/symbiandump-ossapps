/*!
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
#include "phoneuiqtbuttonscontroller.h"
#include <QtDebug>

int m_phoneButtonFlags;

PhoneUIQtButtonsController::PhoneUIQtButtonsController() : m_buttonFlags(0)
{
}

PhoneUIQtButtonsController::~PhoneUIQtButtonsController ()
{
}

void PhoneUIQtButtonsController::setButtonFlags (PhoneButtonFlags flag,bool set)
{
    if (set) 
        {
        // Set the flag on (=1)
        m_buttonFlags |= flag; 
        }
    else 
        {
        // Set the flag off (=0)
        m_buttonFlags &= (~flag);     
        }
    
    m_phoneButtonFlags = m_buttonFlags;
}
