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

#ifndef BUBBLEPARTICIPANTLISTMODEL_H
#define BUBBLEPARTICIPANTLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>

class Participant; // helper class

class BubbleParticipantListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    BubbleParticipantListModel();
    ~BubbleParticipantListModel();

    void addParticipant(
        int bubbleId,
        const QString &name,
        Qt::TextElideMode nameClip = Qt::ElideRight);

    void removeParticipant(
        int bubbleId);

    int bubbleId(int row);

    void reset();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;

    QVariant data( const QModelIndex &index,
                   int role = Qt::DisplayRole ) const;

private:
    inline bool isDataChanged(
        const Participant& participant,
        const QString &name,
        Qt::TextElideMode nameClip) const;

    inline void updateData(
        Participant& participant,
        const QString &name,
        Qt::TextElideMode nameClip) const;

private:
    QList<Participant*> mParticipants;
};


#endif // BUBBLEPARTICIPANTLISTMODEL_H
