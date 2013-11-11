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

 
#include "xqaiwrequest.h"
 
bool m_aiwRequestCreated = false;
bool m_aiwRequestSended = false;

XQAiwRequest::XQAiwRequest() :
    QObject()
{
}
XQAiwRequest::~XQAiwRequest()
{
}

void XQAiwRequest::setArguments(const QList<QVariant> &/*arguments*/)
{
    m_aiwRequestCreated = true;
}

bool XQAiwRequest::send()
{
    m_aiwRequestSended = true;
    return true;
}
 
int XQAiwRequest::lastError() const
{
    return 0;
}

