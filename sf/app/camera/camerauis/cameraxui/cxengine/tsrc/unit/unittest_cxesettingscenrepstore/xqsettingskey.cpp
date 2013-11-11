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
* Description:
*
*/

/*!
* XQSettingsKey class fake implementation
*/

#include <QVariant>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "cxutils.h"
#include "xqsettingskey.h"
#include "cxenamespace.h"




/*!
* Reads the setting value for the given key from cenrep
*/
long int XQSettingsKey::uid() const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    return mUid;
}


/*!
* Reads the setting value for the given key from cenrep
*/
unsigned long int XQSettingsKey::key() const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    return mKey;
}



/*!
* Reads the setting value for the given key from cenrep
*/
XQSettingsKey::Target XQSettingsKey::target() const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    return mTarget;
}



/*!
* XQSettingsKey::XQSettingsKey
*/
XQSettingsKey::XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key)
:mTarget(target),
 mUid(uid),
 mKey(key)
{
}



/*!
* XQSettingsKey::close
*/
XQSettingsKey::~XQSettingsKey()
{
}

// end of file
