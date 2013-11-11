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
* Description:  XQSettingsKey stub for FM Radio home screen widget
*               unit testing.
*
*/

// User includes
#include "xqsettingskey.h"
#include "radiologger.h"

/*!
    \class XQSettingsKey
    \brief Stub implementation of XQSettingsKey for unit testing
*/

// ======== MEMBER FUNCTIONS ========

XQSettingsKey::XQSettingsKey(XQSettingsKey::Target target, long int uid,
    unsigned long int key)
{
    LOG_METHOD_ENTER;
    m_target = target;
    m_uid = uid;
    m_key = key;
}

XQSettingsKey::~XQSettingsKey()
{
    LOG_METHOD_ENTER;
}

XQSettingsKey::Target XQSettingsKey::target() const
{
    LOG_METHOD_ENTER;
    return m_target;
}

long int XQSettingsKey::uid() const
{
    LOG_METHOD_ENTER;
    return m_uid;
}

unsigned long int XQSettingsKey::key() const
{
    LOG_METHOD_ENTER;
    return m_key;
}
