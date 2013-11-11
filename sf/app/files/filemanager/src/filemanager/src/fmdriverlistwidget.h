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
 *     The header file of file manager disk list widget
 */

#ifndef FMDRIVERLISTWIDGET_H
#define FMDRIVERLISTWIDGET_H

#include "fmcommon.h"
#include <hbwidget.h>

class FmDriveModel;
class HbListView;
class HbAbstractViewItem;
class HbSearchPanel;
class HbMenu;
class QFileSystemWatcher;

class QGraphicsLinearLayout;

class FmDriverListWidget : public HbWidget
{
    Q_OBJECT
public:
    FmDriverListWidget( QGraphicsItem *parent = 0 );
    ~FmDriverListWidget();

public:
    int updatePwd( const QString& oldPwd, const QString& pwd);
    void refreshDrive();
    void activeSearchPanel();

signals:
	void activated( const QString &path );
	void startSearch( const QString &criteria );
    
private slots:
	void on_list_activated( const QModelIndex &index );
    void on_list_longPressed( HbAbstractViewItem *item, const QPointF &coords );
    void on_list_pressed( const QModelIndex &  index ) ;

    void on_viewAction_triggered();
    void on_renameAction_triggered();
    void on_nameAction_triggered();
    void on_changePwdAction_triggered();
    void on_removePwdAction_triggered();
    void on_setPwdAction_triggered();
    void on_unLockedAction_triggered();
    void on_formatAction_triggered();
    void on_ejectAction_triggered();
    void on_directoryChanged( const QString &path );
    
    void on_searchPanel_criteriaChanged( const QString &criteria );
    void on_searchPanel_exitClicked();
private:
	void init();

private:
    HbListView *mListView;
	FmDriveModel  *mModel;
    HbAbstractViewItem* mCurrentItem;
    HbSearchPanel* mSearchPanel;
    
    QFileSystemWatcher *mFileSystemWatcher;
    
    QGraphicsLinearLayout *mLayout;
	HbMenu *mContextMenu;
    
    //used to avoid activate when long press list.
    bool mListLongPressed;
};

#endif
