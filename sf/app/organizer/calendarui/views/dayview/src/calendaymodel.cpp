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
* Description: Single day item view model
*
*/

// System includes
#include <calenservices.h>
#include <agendautil.h>

// User includes
#include "calendaymodel.h"


/*!
 BC Day Model constructor
 \a date defined day for which entries will be fetched \a services is handle to 
 organizer services \a parent
 */
CalenDayModel::CalenDayModel(const QDateTime &date, MCalenServices &services, QObject *parent) :
    QAbstractListModel(parent), mServices(services)
{
    if (date.isValid()) {
        loadAndStoreInstances(date);
    }
}

/*
 \reimp
 */
int CalenDayModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent )
    return mEntryList.count();
}

/*
 \reimp
 */
QVariant CalenDayModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mEntryList.count()) {
        return QVariant();
    }

    if (role == CalenDayEntry) {
        return mEntryList.at(index.row());
    }
    else {
        return QVariant();
    }
}

/*!
 Resets model. Old events are removed. Evenets for given day are fetched.
 \a date defined day for which entries will be fetched
 */
void CalenDayModel::refreshModel(const QDateTime &date)
{
    beginResetModel();
    loadAndStoreInstances(date);
    endResetModel();
}

/*!
 Fetches entries via. organizer API and stores it in member container 
 */
void CalenDayModel::loadAndStoreInstances(const QDateTime &date)
{
    mDateTime = date;
    //Filter flags
    AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeAppointments
        | AgendaUtil::IncludeEvents);
    QList<AgendaEntry> list;
    // Fetch the instance list from the agenda interface
    list = mServices.agendaInterface()->createEntryIdListForDay(date, filter);

    mEntryList.clear();

    foreach(AgendaEntry entry, list){
        mEntryList.append(QVariant::fromValue(entry));
    }
}

/*!
 Retruns date (day). Model holds events for this day. 
 */
QDateTime CalenDayModel::modelDate() const
{
    return mDateTime;
}
