/*!
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
*/

#include "xqappmgr.h"
bool m_serviceCreated = false;

XQApplicationManager::XQApplicationManager()
{
}
XQApplicationManager::~XQApplicationManager()
{
}

XQAiwRequest* XQApplicationManager::create(const QString& /*interface*/, 
        const QString& /*operation*/,
        bool embedded)
{
    Q_UNUSED(embedded);
    m_serviceCreated = true;
    return new XQAiwRequest;
}

XQAiwRequest* XQApplicationManager::create(const QString& /*service*/, 
        const QString& /*interface*/,
        const QString& /*operation*/,
        bool embedded)
{
    Q_UNUSED(embedded);
    m_serviceCreated = true;
    return new XQAiwRequest;
}
