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
* Description: VideoProxyModelGeneric class definition
* 
*/

#ifndef VIDEOPROXYMODELGENERIC_H
#define VIDEOPROXYMODELGENERIC_H

// INCLUDES
#include <qsortfilterproxymodel.h>
#include <e32const.h>
#include <mpxitemid.h>
#include "videocollectionexport.h"
#include "videocollectioncommon.h"

// FORWARD DECLARATIONS
class QTimer;
class VideoListDataModel;
class VideoCollectionClient;

// CLASS DECLARATION
class VIDEOCOLLECTION_DLL_EXPORT VideoProxyModelGeneric : public QSortFilterProxyModel 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param QObject parent object.
     */
    VideoProxyModelGeneric(QObject *parent = 0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelGeneric();

    /**
     * Initializes model. Sets the given sourceModel as source model 
     * for this proxy model.
     * 
     * @param sourceModel the source model
     * 
     * @return int 0 if initialization was ok. 
     */
    virtual int initialize(VideoListDataModel *sourceModel);

public:
           
    /**
     * First call opens the collection at the defined level. Subsequent calls only change the level.
     * This starts populating the model data from the MyVideos collection plugin.
     * 
     * @param level The level where the collection is opened.
     * @return 0 if opening was successful, negative if not.
     */
    int open(VideoCollectionCommon::TCollectionLevels level);
    
    /**
     * Starts sorting. If async parameter is defined as true uses idle timer: 
     * starts timer with zero interval. When system has time to run timer sort()
     * -method is called. If async is false, then the sort() -method is called
     * immediatelly.
     * 
     * @param sortingRole The role that sorting is done by.
     * @param sortingOrder The sorting order, e.g. ascending or descending.
     * @param async Defines if the sorting is done asyncronously or not. Default
     *      is true.
     */
    void doSorting(int sortingRole, Qt::SortOrder order, bool async = true);

    /**
     * To get current actice sorting parameters of the model
     * 
     * @param sortingRole The role that sorting is done by.
     * @param sortingOrder The sorting order, e.g. ascending or descending.
     */
    void getSorting(int &sortingRole, Qt::SortOrder &order);
    
    /**
     * Method to delete items at provided indeces
     *
     * @param indexList list of indeces.
     * @return int 0 succeeds, -1 does not succeed.
     */
    int deleteItems(const QModelIndexList &indexList);
    
    /**
    * Opens the defined item. This is quite abstract as depending
    * on the type of the item, the behaviour or result could be
    * quite different.
    * 
    * @param item id id of the item to be opened
    * @return int 0 succeeds, <0 if opening does not succeed.
    */    
    virtual int openItem(TMPXItemId mediaId);

    /**
    * Return to collection level
    *
    */    
    int back();

    /**
     * Starts fetching full details for the defined item.
     * 
     * @param index index of the item from where to get details from
     * @return int 0 succeeds, < 0 if detail fetching startup does not succeed.
     */   
    int fetchItemDetails(const QModelIndex &index);

    /**
     * Method checks that model exists and asks for mpx id from
     * given object.
     * 
     * @param index index of the item in UI.
     * @return TMPXItemId mpx id if succeeds TMPXItemId::InvalidId() in case of error
     *  
     */
    const TMPXItemId& getMediaIdAtIndex(const QModelIndex &index) const;
  
    /**
     * returns qmodelindex of item id provided.
     * 
     * @param item id
     * @return QModelIndex;
     */
    QModelIndex indexOfId(const TMPXItemId &id);
    
    /**
     * Method checks that model exists and asks for file path from
     * given object.
     * 
     * @param mediaId id of the item.
     * @return QString file path if succeeds, empty string in case of error.
     *  
     */
    QString getMediaFilePathForId(const TMPXItemId &mediaId);
        
    /**
     * Add a new album.
     * 
     * @param title, Album title.
     * @param mediaIds, Media items to add in the album.
     * @return TMPXItemId id of created album TMPXItemId:::InvalidId() in case of failure
     */
    TMPXItemId addNewAlbum(const QString &title);
	
    /**
     * Remove an album.
     * 
     * @param indexList, list of album indexes to be removed.
     * @return 0 if no errors.
     */
    int removeAlbums(const QModelIndexList &indexList);

	/**
	 * Add items in an existing album.
	 * 
	 * @param albumId, Album where to add items.
	 * @param mediaIds, list of item ids
	 * @return 0 if no errors.
	 */
    int addItemsInAlbum(TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);    
    
    /**
     * Removes items from existing album.
     * 
     * @param albumId, Album where to add items.
     * @param mediaIds, list of item ids
     * @return 0 if no errors.
     */
    int removeItemsFromAlbum(TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);    
	
    /**
     * Renames a video or user defined colletion.
     * 
     * @param itemId, id of item  to be renamed.
     * @param newTitle, New title for the item.
     * @return 0 if no errors.
     */
    int renameItem(const TMPXItemId &itemId, const QString &newTitle);

    /**
	 * Resolves duplicate album names and returns the resolved name.
	 * 'New collection' -> 'New collection (1)' -> 'New collection (2)', etc.
	 * 
	 * @param albumName, Name specified by the user.
	 * @return Resolved name given to the album.
	 */
    QString resolveAlbumName(const QString& albumName) const;
	
    /**
     * sets item id filter used in generic proxy model
     * while filtering rows. 
     * 
     * If provided filterValue -flag is true, we filter off items that 
     * do not exist in container indicated as itemId. 
     * If false, filtering works other way around. 
     * 
     * 
     * @param filterValue item id used as filter
     * @param filterValue
     */
    virtual void setGenericIdFilter(const TMPXItemId &itemId, bool filterValue);
    
    /**
     * Set album in use and invalidates filtering.
     * 
     * @param albumId album id
     */
    virtual void setAlbumInUse(const TMPXItemId &albumId);
    
	/**
	 * Gets the currently opened item.
	 * 
	 * @param None.
	 * @return TMPXItemId.
	 */
    virtual TMPXItemId getOpenItem() const;

signals:

    /**
    * Signal to be emitted if detail fetching started ok
    * 
    * @param index, index of the item
    */ 
    void shortDetailsReady(TMPXItemId itemId);
 
    /**
     * Notifies that model has been sorted.
     */
    void modelSorted();

protected:

    /**
     * Compares items based in the role setted before sorting. From QSortFilterProxyModel.
     *  
     */
    virtual bool lessThan(const QModelIndex & left, const QModelIndex & right) const;
    
    /**
     * Filtering functionality. Called by the FW. From QSortFilterProxyModel.
     *  
     * @param source_row row to check
     * @param source_parent row's parent
     * 
     * @return bool row accepted(true) or not accepted(false)
     */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
        
    /**
     * Connects all signals emitted from or throught this object.
     * 
     * @return bool
     */
    virtual bool connectSignals();
    
    /**
     * Disconnects all signals.
     */
    virtual void disconnectSignals();
    
    /**
     * Does actual sorting and emits sortingReady.
     *  
     */
    virtual void processSorting();
    
private slots:

    /**
     * Timeout slot for zero timer. Stops timer and calls processSorting.
     *  
     */
    void processSortingSlot();
    
protected:
    
    /**
     * Pointer to the actual model.
     * Not owned.
     */
    VideoListDataModel *mModel;
    
    /**
     * Collection client.
     * Not owned. 
     */
    VideoCollectionClient *mCollectionClient;
    
    /**
     * Currently open level.
     */
    VideoCollectionCommon::TCollectionLevels mLevel;
    
    /**
     * Item id used as filter if model type is generic model.
     */
    TMPXItemId mGenericFilterId;
    
    /**
     * Flag used. 
     */
    bool mGenericFilterValue;
    
    /**
     * Timer object. Owned.
     */
    QTimer *mIdleSortTimer;
    
    /**
     * Sort role to be setted after idle timer time outs for sorting to be able to be executed.
     */
    int mWantedSortRole;
    
    /**
     * Sort order (ascending or descending).
     */
    Qt::SortOrder mWantedSortOrder;
    
    /*
     * Default sort role for the model.
     */
    int mDefaultSortRole;
};

#endif // VIDEOPROXYMODELGENERIC_H
