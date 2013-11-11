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
#include <QStringList>

// User includes
#include "radiohistorymodel.h"
#include "radiohistorymodel_p.h"
#include "radiouiengine.h"
#include "radiohistoryitem.h"
#include "radiostation.h"
#include "radiouiengine.h"
#include "radio_global.h"
#include "radiologger.h"

/*!
 *
 */
RadioHistoryModel::RadioHistoryModel( RadioUiEngine& uiEngine ) :
    QAbstractListModel( &uiEngine ),
    d_ptr( new RadioHistoryModelPrivate( this, uiEngine ) )
{
    Radio::connect( &uiEngine,  SIGNAL(tunedToFrequency(uint,int)),
                    this,       SLOT(resetCurrentSong()) );
    Radio::connect( &uiEngine,  SIGNAL(seekingStarted(int)),
                    this,       SLOT(resetCurrentSong()) );

    Q_D( RadioHistoryModel );
    d->connectToDatabase();
}

/*!
 *
 */
RadioHistoryModel::~RadioHistoryModel()
{
}

/*!
 * \reimp
 */
int RadioHistoryModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    Q_D( const RadioHistoryModel );
    return d->rowCount();
}

/*!
 * \reimp
 */
QVariant RadioHistoryModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    Q_D( const RadioHistoryModel );
    return d->data( index.row(), role );
}

/*!
 * Public slot
 */
void RadioHistoryModel::resetCurrentSong()
{
    Q_D( RadioHistoryModel );
    d->mTopItemIsPlaying = false;
    emit currentSongReset();
}

/*!
 * Sets the icons to be used in the list
 */
void RadioHistoryModel::setIcons( const QIcon& nonTaggedIcon, const QIcon& taggedIcon )
{
    Q_D( RadioHistoryModel );
    d->mNonTaggedIcon = nonTaggedIcon;
    d->mTaggedIcon = taggedIcon;
}

/*!
 *
 */
bool RadioHistoryModel::isCurrentSongRecognized() const
{
    Q_D( const RadioHistoryModel );
    return d->mTopItemIsPlaying;
}

/*!
 *
 */
void RadioHistoryModel::setShowDetails( bool showDetails )
{
    Q_D( RadioHistoryModel );
    d->mShowDetails = showDetails;
    reset();
}

/*!
 *
 */
void RadioHistoryModel::setShowTagged( bool showTagged )
{
    Q_D( RadioHistoryModel );
    d->setViewMode( showTagged ? RadioHistoryModelPrivate::ShowTagged : RadioHistoryModelPrivate::ShowAll );
    reset();
}

/*!
 *
 */
void RadioHistoryModel::toggleTagging( const RadioHistoryItem& item, const int row )
{
    Q_D( RadioHistoryModel );
    d->toggleTagging( item, row );
}

/*!
 *
 */
RadioHistoryItem RadioHistoryModel::itemAtIndex( const QModelIndex& index ) const
{
    Q_D( const RadioHistoryModel );
    return d->itemAtIndex( index );
}

/*!
 *
 */
void RadioHistoryModel::removeAll( bool removeTagged )
{
    Q_D( RadioHistoryModel );
    d->removeAll( removeTagged );
}
/*!
 *
 */
void RadioHistoryModel::removeByModelIndices( QModelIndexList& indices,  bool removeTags )
{
    Q_D( RadioHistoryModel );
    d->removeByModelIndices( indices, removeTags );
}

/*!
 *
 */
void RadioHistoryModel::addItem( const QString& artist, const QString& title, const RadioStation& station )
{
    Q_D( RadioHistoryModel );
    d->addItem( artist, title, station );
}

/*!
 *
 */
void RadioHistoryModel::clearRadioTextPlus()
{
    Q_D( RadioHistoryModel );
    d->mRtItemHolder = "";
    resetCurrentSong();
}

/*!
 *
 */
void RadioHistoryModel::addRadioTextPlus( int rtClass, const QString& rtItem, const RadioStation& station )
{
    if ( rtClass == RtPlus::Dummy || rtClass == RtPlus::Artist || rtClass == RtPlus::Title ) {
        Q_D( RadioHistoryModel );
        if ( d->mRtItemClass == -1 ) {
            d->mRtItemClass = rtClass;
            d->mRtItemHolder = rtItem;
        } else {
            // Received: Artist - Title
            if ( d->mRtItemClass == RtPlus::Artist && rtClass == RtPlus::Title ) {
                addItem( d->mRtItemHolder, rtItem, station );

            // Received: Title - Artist
            } else if ( rtClass == RtPlus::Artist && d->mRtItemClass == RtPlus::Title ) {
                addItem( rtItem, d->mRtItemHolder, station );

            // Received: Dummy - Title
            } else if ( d->mRtItemClass == RtPlus::Dummy && rtClass == RtPlus::Title ) {
                addItem( "", rtItem, station );

            // Received: Title - Dummy
            } else if ( rtClass == RtPlus::Dummy && d->mRtItemClass == RtPlus::Title ) {
                addItem( "", d->mRtItemHolder, station );
            }

            d->mRtItemHolder = "";
            d->mRtItemClass = -1;
        }
    }
}

/*!
 *
 */
void RadioHistoryModel::reportChangedData( int start, int end )
{
    if ( end == -1 ) {
        end = start;
    }
    const QModelIndex startIndex = index( start, 0, QModelIndex() );
    const QModelIndex endIndex = index( end, 0, QModelIndex() );
    emit dataChanged( startIndex, endIndex );
}

/*!
 *
 */
void RadioHistoryModel::emitItemAdded()
{
    emit itemAdded();
}
