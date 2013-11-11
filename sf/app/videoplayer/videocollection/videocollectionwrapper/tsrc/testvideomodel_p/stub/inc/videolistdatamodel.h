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
* Description: contains stub model for unit testing videolistdatamodel_p*
*/

#ifndef __VIDEOLISTDATAMODEL_H__
#define __VIDEOLISTDATAMODEL_H__

#include <qabstractitemmodel.h>
#include "videodeleteworker.h"
#include "videocollectionclient.h"

class VideoListDataModel : public QAbstractItemModel
{    
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoListDataModel) 
    
    friend class VideoListDataModelPrivate;

public: 

    /**
     * Default constructor
     */
    VideoListDataModel(){};  
    
    /**
     * Destructor
     */
    ~VideoListDataModel(){}; 

signals:

    /**
     * dummy signal
     */
    void modelReady();
    
    /**
     * dummy signal
     */
    void albumListReady();
    
    /**
     * dummy signal
     */
    void modelChanged();
    
    /**
     * dummy signal
     */
    void albumChanged();
    
    /**
     * dummy signal
     */
    void fullVideoDetailsReady(QVariant &);

public: // from QAbstractItemModel
    
    /**
     * return 0 always
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        Q_UNUSED(parent); 
        return 0;
    }

   
    QMap<int, QVariant> itemData(const QModelIndex &index) const
    { 
        Q_UNUSED(index);
        return QMap<int, QVariant>();
    }
    
    /**
     * return empty qvariant
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const 
    {
        Q_UNUSED(index); 
        Q_UNUSED(role); 
        return QVariant();
    }
    
    /**
     * columnCount
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const
    {
        Q_UNUSED(parent); 
        return 0;
    }
    
    /**
     * index
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        if(mIndexReturnsInvalid)
        {
            return QModelIndex();
        }
        return createIndex(row, column);
    }
    
    /**
     * Parent
     */
    QModelIndex parent(const QModelIndex & index) const
    { 
        Q_UNUSED(index); 
        return QModelIndex();
    }
    
    void beginInsertRows(const QModelIndex &parent, int first, int last)
    {
        Q_UNUSED(parent);
        mFirstInserted = first;
        mLastInserted = last;
    }
    
    void endInsertRows(){}
    
    void beginRemoveRows(const QModelIndex &parent, int first, int last)
    {
        Q_UNUSED(parent);
        mFirstRemoved = first;
        mLastRemoved = last;
    }
       
    void endRemoveRows(){}
    
    void reportAsyncStatus(int status, QVariant data)
    {
        mStatus = status;
        mStatusData = data;
    }

    void itemModified(const TMPXItemId itemId)
    {
        mModifiedItemId = itemId;
    }
    
    /**
     * dummy collectionclient
     */
    VideoCollectionClient *mCollectionClient;
    
    /**
     * dummy delete worker
     */
    VideoDeleteWorker *mDeleteWorker;
    
    /**
     * setted in beginInsertRows
     */
    static int mFirstInserted;
    
    /**
     * setted in beginInsertRows;
     */
    static int mLastInserted;
    
    /**
    * setted in beginRemoveRows
    */
    static int mFirstRemoved;
   
   /**
    * setted in beginRemoveRows;
    */
    static int mLastRemoved;
    
    /**
     * setted in reportAsyncStatus
     */
    static int mStatus;
    
    /**
     * setted in reportAsyncStatus
     */
    static QVariant mStatusData;
    
    /**
     * setted in itemModified 
     */
    static TMPXItemId mModifiedItemId;
    
    /**
     * Flag if index() should return invalid index.
     */
    static bool mIndexReturnsInvalid;
};
#endif  // __STUBTESTMODEL_H__

// End of file
