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
* Description:   Videolist sort filter proxy stub implementation
*
*/

#ifndef VIDEOPROXYMODELGENERIC_H
#define VIDEOPROXYMODELGENERIC_H

#include <qsortfilterproxymodel.h>
#include <QString>
#include <QList>
#include <mpxitemid.h>

class VideoProxyModelGeneric : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoProxyModelGeneric(QObject *parent=0);

    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelGeneric();
    
public:

    /**
     * Method checks that model exists and asks for mpx id from
     * given object.
     *
     * @param index index of the item.
     * @return int mpx id if succeeds, -1 in case of error
     *
     */
    TMPXItemId getMediaIdAtIndex(QModelIndex index);

    /**
     * Returns filepath.
     * 
     * @param mediaId id for the item, only iId1 is used.
     * @returns file path for the item.
     */
    QString getMediaFilePathForId(TMPXItemId mediaId);

    /**
    * Return to collection level
    *
    */
    void back();
    
    /**
     * Return source model.
     * 
     */
    VideoProxyModelGeneric* sourceModel();    

public: // Simplified data access.
    /**
    * Return count of filenames.
    *
    */
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /**
     * Return specified index.
     *
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /**
     * Returns copy of the given index.
     *
     */
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

signals:

    /**
     * Signals that the model is ready, ie. that all data has been
     * loaded from myvideoscollection.
     */
    void modelReady();
    
    /**
     * notifies that model's physical data structure has changed:
     * - item inserted
     * - item removed 
     */
    void modelChanged();    
    
public: // Test helper methods.

    static void reset();

    void appendData(QString fileName);

protected: // from QSortFilterProxyModel

    /**
     * Compares items based in the role setted before sorting.
     *
     */
    bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;

public:

    // Incremented at rowCount calls.
    static int mRowCountCallCount;

    // If true index() returns invalid indexes.
    static bool mReturnInvalidIndexes;

    // Model items.
    QList<QString> mFileNames;
};

#endif // VIDEOPROXYMODELGENERIC_H
