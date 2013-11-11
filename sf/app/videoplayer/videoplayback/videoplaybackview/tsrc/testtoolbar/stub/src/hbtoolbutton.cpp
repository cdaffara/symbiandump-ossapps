/*
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
* Description:  Implementation of HbToolButton
*
*/

// Version : %version:  1 %



#include <qgraphicswidget>

#include <hbaction.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "hbtoolbutton.h"

// -------------------------------------------------------------------------------------------------
// HbToolButton::HbToolButton
// -------------------------------------------------------------------------------------------------
//
HbToolButton::HbToolButton( HbAction *action )
{
    MPX_ENTER_EXIT(_L("HbToolButton::HbToolButton()"));

    mAction = action;
}

// -------------------------------------------------------------------------------------------------
// HbToolButton::~HbToolButton
// -------------------------------------------------------------------------------------------------
//
HbToolButton::~HbToolButton()
{
    MPX_DEBUG(_L("HbToolButton::HbToolButton") );
}

// -------------------------------------------------------------------------------------------------
// HbToolButton::isEnabled
// -------------------------------------------------------------------------------------------------
//
bool HbToolButton::isEnabled()
{
    MPX_DEBUG(_L("HbToolButton::isEnabled") );

    return mAction->isEnabled();
}

// -------------------------------------------------------------------------------------------------
// HbToolButton::press
// -------------------------------------------------------------------------------------------------
//
void HbToolButton::press()
{
    MPX_DEBUG(_L("HbToolButton::press") );

    emit pressed();
}

// -------------------------------------------------------------------------------------------------
// HbToolButton::release
// -------------------------------------------------------------------------------------------------
//
void HbToolButton::release()
{
    MPX_DEBUG(_L("HbToolButton::release") );

    emit released();
}

// End of file
