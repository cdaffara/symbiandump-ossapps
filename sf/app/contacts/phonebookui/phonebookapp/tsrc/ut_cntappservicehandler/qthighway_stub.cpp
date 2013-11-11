/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qglobal.h>
#include <xqserviceprovider.h>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQServiceProvider::XQServiceProvider( const QString& service, QObject *parent )
{
    Q_UNUSED(service);
    Q_UNUSED(parent);
}

XQServiceProvider::~XQServiceProvider()
{
    
}

void XQServiceProvider::SetPlugin(QObject* impl_plugin)
{
    Q_UNUSED(impl_plugin);
}

void XQServiceProvider::publishAll()
{
    
}

int XQServiceProvider::setCurrentRequestAsync()
{
    return 0;
}
    
bool XQServiceProvider::completeRequest(int index, const QVariant& retValue)
{
    Q_UNUSED(index);
    Q_UNUSED(retValue);
    return true;
}
