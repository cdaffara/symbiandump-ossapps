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
* 
* Description:
*     The header file of the file widget
*
*/
#ifndef FMFILEWIDGET_H
#define FMFILEWIDGET_H

#include "hbwidget.h"
#include <QFileInfo>
#include <QDir>
#include <QString>

class FmDriveModel;
class FmDriveWatcher;
class FmFileIconProvider;
class FmFileSystemProxyModel;

class QFileSystemModel;
class QGraphicsLinearLayout;

class HbListView;

class FmFileWidget : public HbWidget
{
Q_OBJECT
public:
    enum ViewType{
        DirView,
        DriveView
    };

    explicit FmFileWidget( HbWidget *parent = 0 );
    ~FmFileWidget();

    QFileInfo currentPath() const;

    void setModelFilter( QDir::Filters filters );
    void setNameFilters( const QStringList &nameFilters );
    void setRootPath( const QString &pathName );

    FmFileWidget::ViewType currentViewType();
    bool cdUp();
signals:
    void pathChanged( const QString &path );
    void fileActivated( const QString &path );
    void listActivated();

private slots:
    void on_list_activated( const QModelIndex &index );
    void on_listActivated();
    void on_driveWatcher_driveAddedOrChanged();

private:
    void init();
    void changeRootIndex( const QModelIndex &index );
    void setModel( QAbstractItemModel *model );

private:
    HbListView *mListView;
    QGraphicsLinearLayout   *mLayout;
    QFileSystemModel        *mFileSystemSourceModel;
    FmFileSystemProxyModel  *mFileModel;
    FmDriveModel		    *mDriveModel;

    QAbstractItemModel *mCurrentModel;
    FmDriveWatcher  *mDriveWatcher;
    
    QModelIndex mActivatedModelIndex;
    
    FmFileIconProvider *mFileIconProvider;

};

#endif
