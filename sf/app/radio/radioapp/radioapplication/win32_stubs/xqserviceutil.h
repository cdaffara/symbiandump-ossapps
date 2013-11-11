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
* Description:
*
*/

#ifndef XQSERVICEUTIL_H
#define XQSERVICEUTIL_H

#include <QString>

// THIS IS A STUB CLASS FOR WIN32 ENVIRONMENT!
// It is only meant to compile, not to actually do anything

namespace XQServiceUtil
{
    void toBackground(bool) {}
    bool isEmbedded() { return false; }
    bool isService() { return false; }
    bool isService( int &argc, char *argv[] ) { return false; }
    QString interfaceName() { return ""; }
    QString operationName() { return ""; }
}

#endif
