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
 *     Zhiqiang Yang <zhiqiang.yang@nokia.com>
 * 
 * Description:
 *     The find result model header file of file manager
 */

#ifndef FMFINDRESULTMODEL_H
#define FMFINDRESULTMODEL_H

#include "fmcommon.h"

#include <QPair>
#include <QFileInfo>
#include <QStringList>
#include <QAbstractListModel>
#include <QTimer>
#include <QEventLoop>

class FmFindThread;
class FmFileIconProvider;

class FmFindResultModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        FileIconRole = Qt::DecorationRole
    };

    enum SortFlag {
        Name = 0,
        Time = 1,
        Size = 2,
        Type = 3
    };

    FmFindResultModel( QObject *parent = 0 );
    ~FmFindResultModel();
    
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool insertRows( int row, const QStringList &dataList, const QModelIndex &parent = QModelIndex() );
	bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() );
    QString filePath ( const QModelIndex & index ) const;

    QFileInfo fileInfo( const QModelIndex &index ) const;

    void find( const QRegExp &regExp, const QStringList &pathList );
    void stopFind();
    bool isFinding() const;

    virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );

    static bool caseNameLessThan( const QPair<QString,int> &s1,
                                  const QPair<QString,int> &s2 );
    static bool caseTimeLessThan( const QPair<QString,int> &s1,
                                  const QPair<QString,int> &s2 );
    static bool caseSizeLessThan( const QPair<QString,int> &s1,
                                  const QPair<QString,int> &s2 );
    static bool caseTypeLessThan( const QPair<QString,int> &s1,
                                  const QPair<QString,int> &s2 );

signals:

    // pass modelCountChanged signal to parent widget
    // so parent widget could change contentWiget between emptyTipsWidget and listWidget
    void modelCountChanged( int count );

    // emit when start find thread. view can set empty menu for itself(sort).
    // because sort while search inprogress may cause data synchornous problem in model.
    void findStarted();

    // emit when finished find thread. view can set normal menu for itself(sort).
    void findFinished();

private slots:
    void on_findThread_found( const QStringList &dataList );
    void startFind();
    void onThreadFinished();

private:
    bool indexValid( const QModelIndex &index ) const;
    void init();
    

    FmFindThread *mFindThread;
    QStringList mFindResult;
    FmFileIconProvider *mIconProvider;

    QStringList mFindPath;
    QRegExp     mRegExp;
    bool        mIsFindRequestPending;
    QTimer      mTimer;

    QEventLoop  mStopEventLoop;

};

#endif
