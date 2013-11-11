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
* Description:   CpItemDataHelper stub
* 
*/

// INCLUDE FILES
#include "cpitemdatahelper.h"

// ---------------------------------------------------------------------------
// CpItemDataHelper
// ---------------------------------------------------------------------------
//
CpItemDataHelper::CpItemDataHelper()
{
}

// ---------------------------------------------------------------------------
// CpItemDataHelper
// ---------------------------------------------------------------------------
//
CpItemDataHelper::~CpItemDataHelper()
{
}

// ---------------------------------------------------------------------------
// addConnection
// ---------------------------------------------------------------------------
//
void CpItemDataHelper::addConnection(HbDataFormModelItem *item,
    const char *signal,
    QObject *receiver,
    const char *method)
{
    mConnections[item] = ConnectionHolder(signal, receiver, method);
}

// ---------------------------------------------------------------------------
// connectToForm
// ---------------------------------------------------------------------------
//
void CpItemDataHelper::connectToForm(const char *signal,
    QObject *receiver,
    const char *method)
{
    mFormConnection = ConnectionHolder(signal, receiver, method);
}

// ---------------------------------------------------------------------------
// widgetFromModelIndex
// ---------------------------------------------------------------------------
//
HbWidget* CpItemDataHelper::widgetFromModelIndex(const QModelIndex &index)
{
    Q_UNUSED(index);
    return mWidgetReturnValue;
}

HbDataFormModelItem* CpItemDataHelper::modelItemFromModelIndex(const QModelIndex &index)
{
    Q_UNUSED(index);
    return mModelItemReturnValue;
}
