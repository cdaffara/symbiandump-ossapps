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

// User includes
#include "radiohistoryitem.h"
#include "radiohistoryitem_p.h"

/*!
 *
 */
RadioHistoryItem::RadioHistoryItem() :
    mData( new RadioHistoryItemPrivate() )
{
    mData->ref.ref();
}

/*!
 *
 */
RadioHistoryItem::RadioHistoryItem( const QString& artist, const QString& title ) :
    mData( new RadioHistoryItemPrivate( artist, title ) )
{
}

/*!
 *
 */
RadioHistoryItem::RadioHistoryItem( const RadioHistoryItem& other ) :
    mData( other.mData )
{
}

/*!
 *
 */
RadioHistoryItem::~RadioHistoryItem()
{
}

/*!
 *
 */
RadioHistoryItem& RadioHistoryItem::operator=( const RadioHistoryItem& other )
{
    mData = other.mData;
    return *this;
}

/*!
 *
 */
bool RadioHistoryItem::isValid() const
{
    return id() != 0 && !title().isEmpty();
}

/*!
 *
 */
void RadioHistoryItem::reset()
{
    mData->ref.deref();
    mData = NULL;
    mData = new RadioHistoryItemPrivate();
    mData->ref.ref();
}

/*!
 *
 */
int RadioHistoryItem::id() const
{
    return mData->mId;
}

/*!
 *
 */
QString RadioHistoryItem::artist() const
{
    return mData->mArtist;
}

/*!
 *
 */
void RadioHistoryItem::setArtist( const QString& artist )
{
    if ( artist.compare( mData->mArtist ) != 0 ) {
        mData->mArtist = artist;
    }
}

/*!
 *
 */
QString RadioHistoryItem::title() const
{
    return mData->mTitle;
}

/*!
 *
 */
void RadioHistoryItem::setTitle( const QString& title )
{
    if ( title.compare( mData->mTitle ) != 0 ) {
        mData->mTitle = title;
    }
}

/*!
 *
 */
QString RadioHistoryItem::station() const
{
    return mData->mStation;
}

/*!
 *
 */
void RadioHistoryItem::setStation( const QString& station )
{
    if ( station.compare( mData->mStation ) != 0 ) {
        mData->mStation = station;
    }
}

/*!
 *
 */
uint RadioHistoryItem::frequency() const
{
    return mData->mFrequency;
}

/*!
 *
 */
void RadioHistoryItem::setFrequency( uint frequency )
{
    if ( frequency != mData->mFrequency ) {
        mData->mFrequency = frequency;
    }
}

/*!
 *
 */
QString RadioHistoryItem::time() const
{
    return mData->mTime.toString();
}

/*!
 *
 */
void RadioHistoryItem::setCurrentTime()
{
    mData->mTime = QDateTime::currentDateTime();
}

/*!
 *
 */
bool RadioHistoryItem::isTagged() const
{
    return mData->mTagged;
}

/*!
 *
 */
bool RadioHistoryItem::isRecognizedByRds() const
{
    return mData->mFromRds;
}

/*!
 *
 */
bool RadioHistoryItem::isDetached() const
{
    return mData->ref == 1;
}
