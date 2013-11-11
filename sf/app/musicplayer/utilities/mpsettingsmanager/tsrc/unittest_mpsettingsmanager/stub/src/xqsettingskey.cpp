/*
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
* Description: xqsettingitem stub for testing MpSettingsManager.
*
*/
#include <qglobal.h>
#include "stub/inc/xqsettingskey.h"

/*!
internal
*/
XQSettingsKey::XQSettingsKey(
    XQSettingsKey::Target target, long int uid, unsigned long int key) :
    m_key(key)
{
    (void)target;
    (void)uid;
}

/*!
internal
*/
XQSettingsKey::~XQSettingsKey()
{
}

/*!
internal
*/
unsigned long int XQSettingsKey::key() const
{
    return m_key;
}

// End of file
