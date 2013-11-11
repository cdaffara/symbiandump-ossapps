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
* Description: Model for conference participant list.
*
*/

#include <QtCore>
#include "bubbleparticipantlistmodel.h"

class Participant
{
public:
    int mBubbleId;
    QString mName;
    Qt::TextElideMode mNameClip;

public:
    Participant(int bubbleId, const QString &name, Qt::TextElideMode mNameClip);

};

Participant::Participant(
    int bubbleId,
    const QString &name,
    Qt::TextElideMode nameClip) :
    mBubbleId(bubbleId), mName(name), mNameClip(nameClip)
{
}


BubbleParticipantListModel::BubbleParticipantListModel()
{
}

BubbleParticipantListModel::~BubbleParticipantListModel()
{
    qDeleteAll(mParticipants);
}

int BubbleParticipantListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mParticipants.count();
}

QVariant BubbleParticipantListModel::data(
    const QModelIndex &index,
    int role ) const
{
    Participant *player = mParticipants.at(index.row());

    if (role == Qt::DisplayRole) {
        return player->mName;
    } if (role == Qt::TextAlignmentRole) {
        return player->mNameClip;
    } else {
        return QVariant();
    }
}

void BubbleParticipantListModel::addParticipant(
    int bubbleId,
    const QString &name,
    Qt::TextElideMode nameClip)
{
    bool itemExist=false;

    // check, if existing item (bubble)
    for (int i=0; i < mParticipants.count(); i++) {
        Participant* p = mParticipants[i];
        if (p->mBubbleId == bubbleId) {
            if (isDataChanged(*p,name,nameClip)) {
                updateData(*p,name,nameClip);
                QModelIndex index = QAbstractListModel::createIndex(i,0);
                emit dataChanged(index,index);
            }
            itemExist=true;
            break;
        }
    }

    if (!itemExist) {
        // insert new item
        beginInsertRows(QModelIndex(), mParticipants.count(), mParticipants.count());
        Participant* p = new Participant(bubbleId,name,nameClip);
        mParticipants.append(p);
        endInsertRows();
    }
}

bool BubbleParticipantListModel::isDataChanged(
    const Participant& participant,
    const QString &name,
    Qt::TextElideMode nameClip) const
{
    if ( participant.mName != name ) {
        return true;
    } else if (participant.mNameClip != nameClip) {
        return true;
    } else {
        return false;
    }
}

void BubbleParticipantListModel::updateData(
    Participant& participant,
    const QString &name,
    Qt::TextElideMode nameClip) const
{
    participant.mName = name;
    participant.mNameClip = nameClip;
}

void BubbleParticipantListModel::removeParticipant(int bubbleId)
{
    QMutableListIterator<Participant*> i(mParticipants);
    int j=0;
    while(i.hasNext()) {
        Participant* p = i.next();
        if (p->mBubbleId == bubbleId) {
            beginRemoveRows(QModelIndex(), j, j);
            i.remove();
            endRemoveRows();
        }
        j++;
    }
}

int BubbleParticipantListModel::bubbleId(int row)
{
    if (mParticipants.count()>=row) {
        return mParticipants.at(row)->mBubbleId;
    } else {
        return -1;
    }
}

void BubbleParticipantListModel::reset()
{
    qDeleteAll(mParticipants);
    mParticipants.clear();
}

