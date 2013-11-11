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
* Description: VideoListDataModel class definition*
*/

#ifndef __VIDEOLISTDATAMODEL_H__
#define __VIDEOLISTDATAMODEL_H__


// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>
#include <e32const.h>
#include <mpxitemid.h>


// FORWARD DECLARATIONS
class VideoListDataModelPrivate;
class VideoThumbnailData;
class VideoCollectionClient;
class VideoDeleteWorker;


class VideoListDataModel : public QAbstractItemModel
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoListDataModel) 
    
    /**
     * private class declaration macro
     */    
    Q_DECLARE_PRIVATE_D( d_ptr, VideoListDataModel )

public: 

	/**
	 * Default constructor
     */
	VideoListDataModel(QObject *parent = 0);	
	
	/**
     * Destructor
     */
	~VideoListDataModel();	

	/**
     * Initializes model. Sets model cache proxy and cache's plugin to 
     * to be this object. Creates videolist and puts it to observe 
     * collection client. Calls  connectSignals to connect signals into
     * appropriate slots
     * 
     * 
     * @return int: 0 if everything ok
     */
	int initialize( );
	
	/**
	 * returns collectionclient object
	 * 
	 * @return VideoCollectionClient*
	 */
	VideoCollectionClient* getCollectionClient();
	
	/**
     * Method calls video list data to check if there are valid media object 
     * at the given index. If there is, item id is returned.
     * 
     * @param index index of the item to be opened
     * @return TMPXItemId item id  or TMPXItemId::Invalid() if no valid item.
     */ 
	const TMPXItemId& mediaIdAtIndex(const int &index) const;

	/**
	 * returns model index of id provided
	 * 
	 * @param id of the item
	 * @return modelIndex
	 */
	QModelIndex indexOfId(const TMPXItemId &id);
	
    /**
     * Method calls video list data to check if there are valid media object 
     * for the given media id. If there is, file path for is returned.
     * 
     * @param mediaId id of the item to be opened
     * @return QString file path of the media at index, empty string if not valid item.
     */ 
	QString mediaFilePathForId(const TMPXItemId &mediaId) const;
    
	/**
     * Called by the client when removal of videos are requested.
     * Sets video status to be removed to videolistdata and
     * eventually calls collection to handle the actual removing.
     * 
     * @param indexlist list of indeces of items requested for deletion
     * 
     * @return bool true if removal startup succeeds
     */
    bool removeRows(const QModelIndexList &indexList);

    /**
     * Checks if the specified item belongs to currently open album.
     * 
     * @param albumId, Album to be checked. If zero, using the currently open album.
     * @param itemId, Item to be checked.
     * @return true if item belongs to the album.
     */
    bool belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId = TMPXItemId::InvalidId()) const;
    
    /**
     * Set album in use.
     * 
     * @param albumId, Album to set in use.
     * @return None.
     */
    void setAlbumInUse(const TMPXItemId &albumId);
    
    /**
     * returns album id currently in use
     * 
     * @return TMPXItemId
     */
    const TMPXItemId& albumInUse() const;
    
    /**
     * removes provided items from provided album id and calls
     * collectionclient to update mds as well.
     * 
     * @param albumId album from where to remove
     * @param items items to remove
     * 
     * @return count of items removed or -1 if failed 
     */
    int removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &items);    
    
public: // from QAbstractItemModel
    
    /**
     * Returns video item count to the proxy cache
     * 
     * @param parent, parent's index, not used
     * 
     * @return int count of items
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Returns video item data from index pointed by
     * given QModelIndex's row index:
     * - video name
     * - video detail row
     * - video thumbnail 
     * 
     * If fetch more is setted for given index tries to append
     * additional details into map as well. 
     * 
     * @param index, index of the item data is requested
     * 
     * @return QMap<int, QVariant> item data
     */
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    /**
     * data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    
    /**
     * columnCount
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * index
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * Parent
     */
    QModelIndex parent(const QModelIndex & index) const;
   
signals:

    /**
    * This signal is connected to video list's details ready
    * -signal indicating that video details data is fetched
    * and provides a QMap of the details'.
    */
    void fullVideoDetailsReady(QVariant &);
    
    /**
    * This signal is connected to proxy models short details ready
    * -signal indicating that video details data fetching started ok
    * 
    * @param id of the video item
    */
    void shortDetailsReady(TMPXItemId id);
   
    /**
     * Signals that the model is ready, ie. loaded all data from
     * myvideocollection.
     */
    void modelReady();
    
    /**
     * Signals that all albums have been loaded from myvideoscollection.
     */
    void albumListReady();

    /**
     * Signal to be emitted when something has happened in the 
     * model's data container
     */
    void modelChanged();
    
    /**
     * Signals that album content has been updated.
     */
    void albumChanged();
    
private slots:
    
    /**
     * signaled by the deleteworker in case some delete startup fails
     * 
     * @param ids list of ids whose deletion fails 
     */
    void deleteStartingFailsSlot(QList<TMPXItemId>& ids);
    
private:
      
    /**
     * Method connects signals used for delete worker
     */
    void connectSignals();
    
    /**
     * method disconnects signals
     */
    void disconnectSignals();
    
    /**
     * Generates a detail row string for the video item
     * at the given index. 
     * 
     * In case item is not found in the provided index, empty
     * string is returned.
     * 
     * @param index, index of the item data is requested
     * 
     * @return QString detail string
     */
    QString prepareDetailRow(int &index) const; 

      /**
     * Formats a detail row string for the video item
     * at the given index. 
     * 
     * In case item is not found in the provided index, empty
     * string is returned.
     * 
     * @param index, index of the item data is requested
     * @param duration, duration of the video
     *
     * @return QString detail string
     */
    QString doDetailRow(int &index) const;

    /**
     * Generates a video count string for category or album at given index.
     * 
     * In case item is not found in the provided index, empty
     * string is returned.
     * 
     * @param index, index of the item data is requested
     * @param index, item id of the item data is requested  
     * 
     * @return QString video count string
     */
    QString prepareVideoCountString(int &index) const;
    
    /**
     * Generates a video size string from video item at given index
     * 
     * In case item is not found in the provided index, empty
     * string is returned.
     * 
     * @param index, index of the item data is requested
     * 
     * @return QString size string
     */
    QString prepareSizeString(int &index) const;
    
    /**
     * Called when there are status changes in some async operation
     * 
     * @param statusCode status of operation
     * @data data from operation
     */
    void reportAsyncStatus(int statusCode, QVariant &additional);

private:
    
    /**
     * Private implementation Contains the actual video data and
     * video array. Own.
     */
    VideoListDataModelPrivate* const d_ptr;
        
    /**
     * Pointer to collection client. Used to send commands to collection.
     * Owned.
     */
    VideoCollectionClient *mCollectionClient;
    
    /**
     * object for handling item deletion conveniently in small blocks.
     * Own
     */
    VideoDeleteWorker *mDeleteWorker;
            
    /**
     * initialized flag
     */
    bool mInitialized;
   
};
#endif  // __VIDEOLISTDATAMODEL_H__

// End of file
    
