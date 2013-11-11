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

#include "cntserviceproviderold.h"
#include "cntservices.h"
#include "cntdebug.h"

#include <qapplication.h>
#include <QPixmap>
#include <QFile>
#include <QUrl>

CntServiceProviderOld::CntServiceProviderOld(CntServices& aServices, QObject *parent):
    XQServiceProvider(QLatin1String("com.nokia.services.phonebookservices.Fetch"), parent),
    mServices(aServices),
    mCurrentRequestIndex(0)
{
    CNT_ENTRY
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), &mServices, SLOT(quitApp()));
    CNT_EXIT
}

CntServiceProviderOld::~CntServiceProviderOld()
{
    CNT_ENTRY
    CNT_EXIT
}

/*!
Launch fetch service with given parameters. OLD INTERFACE.
*/
void CntServiceProviderOld::fetch(const QString &title, const QString &action, const QString& /*filter*/)
{
    CNT_ENTRY
    // Ignore the filter parameter
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.multiFetch( title, action, *this );
    CNT_EXIT
}


/*
Launch fetch service with given parameters. Fetching mode is No-select by default. OLD INTERFACE.
*/
void CntServiceProviderOld::Dofetch(const QString &title, const QString &action, const QString& /*filter*/, const QString& /*mode*/)
{
    CNT_ENTRY
    // Ignore the filter parameter
    // Ignoring fourth argument, because it's not needed. DoFetch() by old contract is only for single-fetching.
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.singleFetch( title, action, *this );
    CNT_EXIT
}


/*
Launch new contact editor with a given detail
*/
void CntServiceProviderOld::editCreateNew(const QString& definitionName, const QString& value)
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editCreateNew( definitionName, value, *this, false ); // defaultForOnlineAccountIsImpp = false
    CNT_EXIT
}

void CntServiceProviderOld::editCreateNew(const QString &vCardFile)
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editCreateNewFromVCard( vCardFile, *this );
    CNT_EXIT
}


/*!
Launch contact selection view with a given detail (selecting detail there opens the editor and adds the new detail)
*/
void CntServiceProviderOld::editUpdateExisting(const QString &definitionName, const QString &value)
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.editUpdateExisting( definitionName, value, *this, false ); // defaultForOnlineAccountIsImpp = false
    CNT_EXIT
}


/*!
Launch communication launcher view for the contact with the given ID (QContactLocalId -> int)
*/
void CntServiceProviderOld::open(int contactId)
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.launchContactCard( contactId, *this );
    CNT_EXIT
}

/*!
  Launch temporary communication launcher view with the given detail.
*/
void CntServiceProviderOld::open(const QString &definitionName, const QString &value)
{
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.launchTemporaryContactCard( definitionName, value, *this ); // definitionName = field type      value = actual field value for prefilling editor
    CNT_EXIT
}


void CntServiceProviderOld::CompleteServiceAndCloseApp(const QVariant& retValue)
{
    CNT_ENTRY
    connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    if ( mCurrentRequestIndex != 0 )
    {
        bool success = false;
        
        if (!mOverriddenReturnValue.isNull() && retValue.value<int>() != KCntServicesReturnValueContactDeleted)
        {
            CNT_LOG_ARGS(mOverriddenReturnValue.value<int>());
            success = completeRequest(mCurrentRequestIndex, mOverriddenReturnValue);
        }
        else
        {
            CNT_LOG_ARGS(retValue.value<int>());
            success = completeRequest(mCurrentRequestIndex, retValue);
        }
        
        if ( !success )
        {
            CNT_LOG_ARGS("Failed to complete highway request.");
        }
        mCurrentRequestIndex = 0;
    }
    CNT_EXIT
}

void CntServiceProviderOld::overrideReturnValue(const QVariant& retValue)
{
    CNT_ENTRY
    
    if (mOverriddenReturnValue.isNull() || mOverriddenReturnValue.value<int>() != KCntServicesReturnValueContactSaved)
    {
        mOverriddenReturnValue = retValue;
    }
    
    CNT_EXIT
}

// end of file
