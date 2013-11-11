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
* Description: Music Player secondary collection abstract data model. This is
*              primarily used to support Mediawall in Collection View. This
*              model represents the list containing album songs.
*
*/

#include <hbnamespace.h>

#include "mpcollectiontbonelistdatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mpplaybackdata.h"
#include "mptrace.h"

/*!
    \class MpCollectionTBoneListDataModel
    \brief Music Player collection data model.

    Secondary collection data model implements the interface specified by
    QAbstractListModel, which defines the standard interface that item models
    must use to be able to interoperate with other components in the model/view
    architecture.

    Every item of data that can be accessed via a model has an associated model
    index.

    Each item has a number of data elements associated with it and they can be
    retrieved by specifying a role (see Qt::ItemDataRole) to the model's data
    returned by itemData() function.

    MpCollectionTBoneListDataModel is primarily used to support Mediawall in
    Collection View. This model represents the list containing album songs.

    \sa QAbstractListModel
*/

/*!
    \fn void albumDataChanged()

    This signal is specific to views with TBone. This signal is emitted when
    there's a change in album data. This is an indication that the container
    should re-fetch the album content. Currently, the only operation that can
    trigger this is the delete operation.
 */

/*!
    \fn void albumDataAvailable()

    This signal is specific to views with TBone. This signal is emitted when
    a new data set is available for the list section of the TBone. This is
    triggered as a result of container re-fetching the album content.

    \sa albumDataChanged
 */

/*!
 Constructs the collection data model.
 */
MpCollectionTBoneListDataModel::MpCollectionTBoneListDataModel( MpMpxCollectionData *collectionData, 
        MpPlaybackData *playbackData, QObject *parent )
    : QAbstractListModel( parent ),
      mCollectionData( collectionData ),
      mPlaybackData( playbackData ),
      mRowCount( 0 ),
      mCurrentSongId( 0 ),
      mPlaybackActive( false ),
      mPlaybackIndicatorEnabled( false )
{
    TX_ENTRY
    connect( mCollectionData, SIGNAL(refreshAlbumSongs()),
             this, SLOT(refreshModel()) );
    connect( mCollectionData, SIGNAL(albumDataChanged()),
             this, SIGNAL(albumDataChanged()) );
    
    if ( mPlaybackData ) {
        connect( mPlaybackData, SIGNAL(fileCorrupted( int )), 
                this, SLOT(fileCorrupted( int )));
    }
    
    TX_EXIT
}

/*!
 Destructs the collection data model.
 */
MpCollectionTBoneListDataModel::~MpCollectionTBoneListDataModel()
{
    TX_LOG
}

/*!
 Returns the number of rows under the given \a parent.

 \reimp
 */
int MpCollectionTBoneListDataModel::rowCount( const QModelIndex &parent ) const
{
    TX_LOG
    Q_UNUSED(parent);
    return mRowCount;
}

/*!
 Returns the data stored for the item referred to by the \a index.

 \reimp
 */
QVariant MpCollectionTBoneListDataModel::data(const QModelIndex &index, int role) const
{
    TX_ENTRY
    QVariant returnValue = QVariant();
    if ( !index.isValid() ) {
        return returnValue;
    }

    int row = index.row();
    TX_LOG_ARGS("index=" << row << ", role=" << role);
    if ( role == Qt::DisplayRole ) {
        // Fetch the primary text, which is the title, if available.
        QString songTitle = mCollectionData->albumSongData(row, MpMpxCollectionData::Title);
        if ( !songTitle.isEmpty() ) {
            returnValue = songTitle;
        }
        else {
            returnValue = hbTrId("txt_mus_other_unknown4");
        }
    }
    else if ( role == Qt::DecorationRole ) {
        if ( mCollectionData->hasAlbumSongProperty(row, MpMpxCollectionData::Corrupted) ) {
                QList<QVariant> iconList;
                iconList << QVariant();
                iconList << HbIcon("qtg_mono_corrupted");
                returnValue = iconList;
        }
        else if ( mCollectionData->hasAlbumSongProperty(row, MpMpxCollectionData::DrmExpired) ) {
            QList<QVariant> iconList;
            iconList << QVariant();
            iconList << HbIcon("qtg_small_drm_rights_expired");
            returnValue = iconList;
        }
        else if ( mPlaybackActive && mPlaybackIndicatorEnabled
                && mPlaybackData->id() == mCollectionData->albumSongId( row ) ) {
            QList<QVariant> iconList;
            iconList << QVariant(); //primary icon is not used.
            if ( mPlaybackData->playbackState() == MpPlaybackData::Playing ) {
                iconList << HbIcon("qtg_mono_play");
            }
            else {
                iconList << HbIcon("qtg_mono_pause");
            }
            returnValue = iconList;
        }
    }
    TX_EXIT
    return returnValue;
}

/*!
 Set Playback Indicator in TBone List Model 
 */
void MpCollectionTBoneListDataModel::enablePlaybackIndicatorEnable(bool enable)
{   
    TX_ENTRY
    if ( !mPlaybackData ) {
        return;
    }
    if ( enable ){
        connect( mPlaybackData, SIGNAL(playbackInfoChanged()),
                        this, SLOT(updateSong()));
                
        connect( mPlaybackData, SIGNAL(playbackStateChanged()), 
                this, SLOT(updatePlaybackState()));
        mPlaybackActive = mPlaybackData->playbackState() != MpPlaybackData::NotPlaying;
        mPlaybackIndicatorEnabled = true;
    }
    else{
        disconnect( mPlaybackData, SIGNAL(playbackInfoChanged()),
                        this, SLOT(updateSong()));
                
        disconnect( mPlaybackData, SIGNAL(playbackStateChanged()), 
                this, SLOT(updatePlaybackState()));
        mPlaybackActive = false;
        mPlaybackIndicatorEnabled = false;
    }
    TX_EXIT
}

/*!
 Slot to be called when data has changed and model needs to be refreshed
 to reflect the new data.
 */
void MpCollectionTBoneListDataModel::refreshModel()
{
    TX_ENTRY
    mRowCount = mCollectionData->albumSongsCount();
    reset();
    emit albumDataAvailable();
    TX_EXIT
}

/*!
 Slot to be called when playing song status has changed.
 */
void MpCollectionTBoneListDataModel::updateSong()
{
    TX_ENTRY
    if( mPlaybackActive ) {
        int newSongId = mPlaybackData->id();

        if ( mCurrentSongId && newSongId != mCurrentSongId) {
            //Attempt to remove old song icon.
            QModelIndex oldSongIndex;
            oldSongIndex = index( mCollectionData->albumSongIndex( mCurrentSongId ) );
            if ( oldSongIndex.isValid() ) {
               emit dataChanged( oldSongIndex, oldSongIndex );
            } 
        }

        //Attempt to update current song data and state.
        QModelIndex songIndex;
        songIndex = index( mCollectionData->albumSongIndex( newSongId ) );
        if ( songIndex.isValid() ) {
            emit dataChanged( songIndex, songIndex );
        }
        mCurrentSongId = newSongId;
    }
    TX_EXIT
}

/*!
 Slot to be called when playback state has changed.
 */
void MpCollectionTBoneListDataModel::updatePlaybackState()
{
    mPlaybackActive = mPlaybackData->playbackState() != MpPlaybackData::NotPlaying;
    updateSong();
}    

/*!
 Slot to be called when a song is marked as corrupted
 */
void MpCollectionTBoneListDataModel::fileCorrupted(int songId)
{
    TX_ENTRY
    QModelIndex corruptedSongIndex;
    corruptedSongIndex = index( mCollectionData->albumSongIndex( songId ) );
    if ( !(corruptedSongIndex.isValid()) ){
        mCollectionData->setReloadAlbumContent( true );
    }
    else if (!(mCollectionData->hasAlbumSongProperty(corruptedSongIndex.row(),
            MpMpxCollectionData::Corrupted))) {   
        mCollectionData->setCorruptValue(corruptedSongIndex, true);
        emit dataChanged( corruptedSongIndex, corruptedSongIndex );
    }
    TX_EXIT
}
    

//EOF


