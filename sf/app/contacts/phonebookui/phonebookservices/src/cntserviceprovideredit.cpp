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
 * Description: This class offers contact viewing UI services.
 *
 */

#include "cntserviceprovideredit.h"
#include "cntservices.h"
#include "cntservicestrings.h"
#include "cntdebug.h"

#include <qapplication.h>
#include <QPixmap>
#include <QFile>
#include <QUrl>

CntServiceProviderEdit::CntServiceProviderEdit(CntServices& aServices, QObject *parent):
    XQServiceProvider(QLatin1String(KCntServiceInterfaceEdit), parent),
    mServices(aServices),
    mCurrentRequestIndex(0)
{
    CNT_ENTRY
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), &mServices, SLOT(quitApp()));
    CNT_EXIT
}

CntServiceProviderEdit::~CntServiceProviderEdit()
{
    CNT_ENTRY
    CNT_EXIT
}

void CntServiceProviderEdit::editCreateNew(const QString& definitionName, const QString& value)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editCreateNew( definitionName, value, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::editCreateNew(const QString& definitionName, const QString& value, const QString &subType)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editCreateNew( definitionName, value, subType, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::editCreateNewFromVCard(const QString &vCardFile)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editCreateNewFromVCard( vCardFile, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::editExisting(int contactId)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editExisting( contactId, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::editUpdateExisting(const QString &definitionName, const QString &value)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editUpdateExisting( definitionName, value, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::editUpdateExisting(const QString &definitionName, const QString &value, const QString &subType)
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editUpdateExisting( definitionName, value, subType, *this );
    CNT_EXIT
    }

void CntServiceProviderEdit::CompleteServiceAndCloseApp(const QVariant& retValue)
    {
    CNT_ENTRY
    connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    if ( mCurrentRequestIndex != 0 )
        {
        const bool success = completeRequest(mCurrentRequestIndex, retValue);
        if ( !success )
        {
            CNT_LOG_ARGS("Failed to complete highway request.");
        }
        mCurrentRequestIndex = 0;
        }
    CNT_EXIT
    }
