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


#ifndef QCONTACTMANAGER_H
#define QCONTACTMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QList>

#include "qcontact.h"
#include "qcontactid.h"
#include "qcontactsortorder.h"
#include "qcontactfetchhint.h"

QTM_BEGIN_NAMESPACE

class QContactFilter;
class QContactAction;

class QContactManager
{
public:
    explicit QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
    ~QContactManager();

    QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;

    QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint = QContactFetchHint()) const;  // retrieve a contact
};

QTM_END_NAMESPACE

#endif
