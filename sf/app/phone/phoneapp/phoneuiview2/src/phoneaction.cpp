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
* Description:  Actions and associated translated texts.
*
*/

#include "phoneaction.h"

PhoneAction::PhoneAction () : 
    m_text (QString ()), m_command (-1), m_icon (HbIcon ()), m_disabled(false),
    m_role(PhoneAction::None)
{
}

PhoneAction::~PhoneAction ()
{
}

QString PhoneAction::text () const
{
    return m_text;
}

void PhoneAction::setText (QString text)
{
    m_text = text;
}

int PhoneAction::command () const
{
    return m_command;
}

void PhoneAction::setCommand (int command)
{
    m_command = command;
}

HbIcon PhoneAction::icon () const
{
    return m_icon;
}

void PhoneAction::setIcon (HbIcon icon)
{
    m_icon = icon;
}

void PhoneAction::setDisabled(bool disabled)
{
    m_disabled = disabled;
}

bool PhoneAction::isDisabled() const
{
    return m_disabled;
}

void PhoneAction::setActionRole(ActionRole role)
{
    m_role = role;
}

PhoneAction::ActionRole PhoneAction::actionRole() const
{
    return m_role;
}

