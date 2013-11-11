
/**
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
* Description:   Stub VideoListDataModel class for VideoCollectionWrapperPrivate unit tests
* 
*/

#ifndef __STUB_VIDEOLISTDATAMODEL_H
#define __STUB_VIDEOLISTDATAMODEL_H

#include <QObject>
#include <qabstractitemmodel.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class VideoCollectionClient;


class  VideoListDataModel : public QAbstractItemModel
{

    Q_OBJECT
    
public: // Constructors and destructor

    /**
     * constructor
     *
     */
    VideoListDataModel(QObject *parent = 0);

    /**
     * destructor
     */
    virtual ~VideoListDataModel();

public:

    /**
     * if gInitFails is true, return -1
     * if gInitFails is false, return 0
     * 
     * @param not used
     * 
     * @return int
     */
    int initialize();

    /**
     * if gFetchMediaIDFails is true, return -1
     * if gFetchMediaIDFails is false, return 0
     * 
     * @param not used
     * 
     * @return int
     */
    int mediaIdAtIndex(int index) const;
    
signals:

    /**
     * not used in stub, but needed to make sure testable object is linked correctly
     */
    void shortDetailsReady(TMPXItemId);
    
    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    void fullVideoDetailsReady(TMPXItemId);
            
public: // from QAbstractItemModel
    
    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    
    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * not used ion stub, but needed to make sure testable object is linked correctly
     */
    QModelIndex parent(const QModelIndex & index) const;     

    /*
     * returns gFailDeleteFile
     */
    bool removeRows( QModelIndexList list);
    
    /**
     * if true, initialize returns -1
     */
    static bool mInitFails;
};

#endif // __STUB_VIDEOLISTDATAMODEL_H
