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

// System includes
#include <QSqlRecord>
#include <QVariant>

// User includes
#include "radiohistoryitem.h"
#include "radiohistoryitem_p.h"

/*!
 *
 */
RadioHistoryItemPrivate::RadioHistoryItemPrivate()
{
    init( "", "" );
}

/*!
 *
 */
RadioHistoryItemPrivate::RadioHistoryItemPrivate( const RadioHistoryItemPrivate& other ) :
    QSharedData( other ),
    mId( other.mId ),
    mArtist( other.mArtist ),
    mTitle( other.mTitle ),
    mFrequency( other.mFrequency ),
    mTagged( other.mTagged ),
    mFromRds( other.mFromRds )
{
}

/*!
 *
 */
RadioHistoryItemPrivate::RadioHistoryItemPrivate( const QString& artist,
                                                  const QString& title )
{
    init( artist, title );
}

/*!
 *
 */
void RadioHistoryItemPrivate::init( const QString& artist, const QString& title )
{
    mId = -1;
    mArtist = artist;
    mTitle = title;
    mFrequency = 0;
    mTagged = false;
    mFromRds = true;
}

/*!
 *
 */
void RadioHistoryItemPrivate::initFromRecord( const QSqlRecord& record )
{
    mId = record.value( RadioHistoryValue::Id ).toInt();
    mArtist = record.value( RadioHistoryValue::Artist ).toString();
    mTitle = record.value( RadioHistoryValue::Title ).toString();
    mFrequency = record.value( RadioHistoryValue::Frequency ).toUInt() * 1000;
    mStation = record.value( RadioHistoryValue::Station ).toString();
    mTagged = record.value( RadioHistoryValue::Tagged ).toBool();
    mFromRds = record.value( RadioHistoryValue::FromRds ).toBool();
    mTime = record.value( RadioHistoryValue::Time ).toDateTime();
}
