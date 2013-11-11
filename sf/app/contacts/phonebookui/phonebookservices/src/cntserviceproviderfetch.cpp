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

#include "cntserviceproviderfetch.h"
#include "cntservices.h"
#include "cntservicestrings.h"
#include "cntdebug.h"

#include <qapplication.h>
#include <QPixmap>
#include <QFile>
#include <QUrl>

CntServiceProviderFetch::CntServiceProviderFetch(CntServices& aServices, QObject *parent):
    XQServiceProvider(QLatin1String(KCntServiceInterfaceFetch), parent),
    mServices(aServices),
    mCurrentRequestIndex(0)
{
    CNT_ENTRY
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), &mServices, SLOT(quitApp()));
    CNT_EXIT
}

CntServiceProviderFetch::~CntServiceProviderFetch()
{
    CNT_ENTRY
    CNT_EXIT
}

void CntServiceProviderFetch::multiFetch(const QString &title, const QString &action )
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.multiFetch( title, action, *this );
    CNT_EXIT
}

void CntServiceProviderFetch::singleFetch(const QString &title, const QString &action )
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.singleFetch( title, action, *this );
    CNT_EXIT
}

void CntServiceProviderFetch::CompleteServiceAndCloseApp(const QVariant& retValue)
{
    CNT_ENTRY
    if ( mCurrentRequestIndex != 0 )
    {
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        const bool success = completeRequest(mCurrentRequestIndex, retValue);
        if ( !success )
        {
            CNT_LOG_ARGS("Failed to complete highway request.");
        }
        mCurrentRequestIndex = 0;
    }
    CNT_EXIT
}

