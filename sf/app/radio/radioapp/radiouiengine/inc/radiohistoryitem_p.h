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

#ifndef RADIOHISTORYITEM_P_H
#define RADIOHISTORYITEM_H

// System includes
#include <QSharedData>
#include <QString>
#include <QDateTime>

// User includes

// Forward declarations
class QSqlRecord;

// Constants
namespace RadioHistoryValue
{
    enum Name
    {
        Id,
        Artist,
        Title,
        Station,
        Frequency,
        Tagged,
        FromRds,
        Time
    };
}

class RadioHistoryItemPrivate : public QSharedData
{
public:

    RadioHistoryItemPrivate();
    RadioHistoryItemPrivate( const RadioHistoryItemPrivate& other );
    RadioHistoryItemPrivate( const QString& artist, const QString& title );

    void init( const QString& artist, const QString& title );

    void initFromRecord( const QSqlRecord& record );

public: // data

    int             mId;

    QString         mArtist;

    QString         mTitle;

    QString         mStation;

    QDateTime       mTime;

    uint            mFrequency;

    bool            mTagged;

    bool            mFromRds;

};

#endif // RADIOHISTORYITEM_H
