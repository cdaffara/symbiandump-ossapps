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
* Description: stub VideoProxyModelGeneric class definition
* 
*/

#ifndef VIDEOPROXYMODELGENERIC_H
#define VIDEOPROXYMODELGENERIC_H
    
// INCLUDES
#include <e32const.h>
#include <mpxitemid.h>
#include <QModelIndex>

class VideoListDataModel;

// CLASS DECLARATION
class VideoProxyModelGeneric : public QObject 
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
    * Opens the defined item. This is quite abstract as depending
    * on the type of the item, the behaviour or result could be
    * quite different.
    * 
    * @param item id id of the item to be opened
    * @return int 0 succeeds, <0 if opening does not succeed.
    */    
    virtual int openItem(TMPXItemId mediaId);
        
	/**
	 * Gets the currently opened item.
	 * 
	 * @param None.
	 * @return TMPXItemId.
	 */
    virtual TMPXItemId getOpenItem() const;
    
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
    void disconnectSignals();
    
    /**
     * Does actual sorting and emits sortingReady.
     *  
     */
    virtual void processSorting();
    
    /**
     * Returns sourceModel;
     */
    virtual const VideoListDataModel *sourceModel() const;
    
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
     * Stub method.
     */
    void invalidateFilter();
    
    /**
     * Stub method.
     */
    void sort(int column, Qt::SortOrder order);
    
    /**
     * Stub method.
     */
    void setSortRole(int sortRole);
    
public:
    static int mOpenItemCallCount;
    static int mGetOpenItemCallCount;
    static int mLessThanCallCount;
    static int mFilterAcceptsRowCallCount;
    static int mConnectSignalsCallCount;
    static bool mConnectSignalsReturnValue;
    static int mDisconnectSignalsCallCount;
    static int mProcessSortingCallCount;
    static int mInvalidateFilterCallCount;
    static Qt::SortOrder mLastSortedOrder;
    static int mLastSortRole;

public:

    /**
     * Pointer to the actual model.
     */
    VideoListDataModel *mModel;
    
    /**
     * Collection client.
     */
    int mCollectionClient;
    
    /**
     * Sort order (ascending or descending).
     */
    Qt::SortOrder mWantedSortOrder;

    /*
     * Sort role.
     */
    int mWantedSortRole;     
    
    /*
     * Default sort role for the model.
     */
    int mDefaultSortRole;        
};

#endif // VIDEOPROXYMODELGENERIC_H
