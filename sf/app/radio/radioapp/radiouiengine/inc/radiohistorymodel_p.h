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

#ifndef RADIOHISTORYMODEL_P_H
#define RADIOHISTORYMODEL_P_H

// System includes
#include <QList>
#include <QString>
#include <QScopedPointer>
#include <QIcon>
#include <QSqlQuery>

// User includes
#include "radiohistoryitem.h"

// Forward declarations
class RadioHistoryModel;
class RadioUiEngine;
class RadioHistoryItem;
class QSqlDatabase;
class RadioStation;
class QStringList;
class QSqlQueryModel;

class RadioHistoryModelPrivate
{
public:

    explicit RadioHistoryModelPrivate( RadioHistoryModel* model, RadioUiEngine& uiEngine );

    ~RadioHistoryModelPrivate();

    bool connectToDatabase();

    void addItem( const QString& artist, const QString& title, const RadioStation& station, bool fromRds = true );

    int rowCount() const;
    QVariant data( const int row, const int role ) const;

    void removeAll( bool removeTagged );
    
    void removeByModelIndices( QModelIndexList& indices,  bool removeTags );

    enum ViewMode{ ShowAll, ShowTagged };
    void setViewMode( ViewMode mode );

    void toggleTagging( const RadioHistoryItem& item, const int row );

    RadioHistoryItem itemAtIndex( const QModelIndex& index ) const;

private:

    void refreshModel();
    QSqlQuery beginTransaction();
    enum Operation{ NoOp, InsertRows, RemoveRows, ChangeData };
    void commitTransaction( QSqlQuery& query, Operation operation, int start, int end = -1 );
    bool prepareAndExec( QSqlQuery& query, const QString& sqlStr );

public: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioHistoryModel*              q_ptr;

    /**
     * Reference to the ui engine
     */
    RadioUiEngine&                  mUiEngine;

    QScopedPointer<QSqlDatabase>    mDatabase;

    QScopedPointer<QSqlQueryModel>  mQueryModel;

    QString                         mRtItemHolder;
    int                             mRtItemClass;

    bool                            mTopItemIsPlaying;

    bool                            mShowDetails;

    /**
     * Non Tagged icon
     */
    QIcon                           mNonTaggedIcon;

    /**
     * Tagged icon
     */
    QIcon                           mTaggedIcon;

    ViewMode                        mViewMode;

};

#endif // RADIOHISTORYMODEL_P_H
