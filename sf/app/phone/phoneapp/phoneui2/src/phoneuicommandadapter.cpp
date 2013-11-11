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
* Description:  Adapter for UI commands from view to state.
*
*/

#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include "mphonemenuandcbaevents.h"
#include "phoneuicommandadapter.h"

PhoneUiCommandAdapter::PhoneUiCommandAdapter (
    MPhoneMenuAndCbaEvents &commandHandler, QObject *parent ) :
    QObject (parent), m_handler (commandHandler)
{
}

PhoneUiCommandAdapter::~PhoneUiCommandAdapter ()
{   
}

void PhoneUiCommandAdapter::handleCommand (int command)
{
    TRAP_IGNORE(m_handler.HandleCommandL ( (TInt) command ));
}
