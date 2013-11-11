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
* Description:  Handles phone notes.
*/

#include "phonenotecontroller.h"
#include "tphonecmdparamglobalnote.h"
#include "phoneresourceadapter.h"
#include <QtDebug>
#include <hbdevicemessagebox.h>

bool m_showGlobalNoteCalled;
bool m_showQueryCalled;
bool m_showNoteCalled;
bool m_removeDtmfNoteCalled;
bool m_removeNoteCalled;
bool m_removeQueryCalled;
bool m_removeGlobalWaitNoteCalled;

PhoneNoteController::PhoneNoteController(QObject *parent) : QObject (parent)
{
    m_showGlobalNoteCalled = false;
}

PhoneNoteController::~PhoneNoteController()
{

}

void PhoneNoteController::showGlobalNote(TPhoneCommandParam */*commandParam*/)
{
    m_showGlobalNoteCalled = true;
}

void PhoneNoteController::showNote(TPhoneCommandParam */*commandParam*/)
{
    m_showNoteCalled = true;
}

void PhoneNoteController::showQuery(TPhoneCommandParam */*commandParam*/)
{
    m_showQueryCalled = true;
}

void PhoneNoteController::removeDtmfNote()
{
    m_removeDtmfNoteCalled = true;
}

void PhoneNoteController::removeNote()
{
    m_removeNoteCalled = true;
}

void PhoneNoteController::removeQuery()
{
    m_removeQueryCalled = true;
}

void PhoneNoteController::removeGlobalWaitNote()
{
    m_removeGlobalWaitNoteCalled = true;
}

void PhoneNoteController::removeMappings()
{
}

