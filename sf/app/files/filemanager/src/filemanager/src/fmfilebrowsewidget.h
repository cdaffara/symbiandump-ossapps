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
*     The header file of the widget to browse files
*
*/

#ifndef FMFILEBROWSEWIDGET_H
#define FMFILEBROWSEWIDGET_H

#include "fmcommon.h"
#include "fmoperationservice.h"

#include <QFileInfo>

#include <hbwidget.h>
#include <QDir>

class QFileSystemModel;
class QGraphicsLinearLayout;
class HbListView;
class HbTreeView;
class HbAbstractViewItem;
class HbSearchPanel;
class HbLabel;

class FmFileIconProvider;
class FmFileSystemProxyModel;

class FmFileBrowseWidget : public HbWidget
{
    Q_OBJECT

public:
    enum Style {
        NoStyle,
        ListStyle,
        TreeStyle,
        LabelStyle
    };
    
    enum TSortType{
        ESortByName,
        ESortByTime,
        ESortBySize,
        ESortByType
    };

    explicit FmFileBrowseWidget( HbWidget *parent = 0, FmFileBrowseWidget::Style style = ListStyle  );
    ~FmFileBrowseWidget();

    QFileInfo currentPath() const;
    QList<QFileInfo> checkedItems() const;
	Style style() const { return mStyle; }
    void setStyle( FmFileBrowseWidget::Style style );
	bool selectable() const;
	void setSelectable( bool enable );
	void clearSelection();

    bool rename( const QString &oldName, const QString &newName );
    void setModelFilter( QDir::Filters filters );
    
    int checkPathAndSetStyle( const QString& path );
    void sortFiles( TSortType sortType );
    void activeSearchPanel();
    FmEventResponse offerBackEvent();
    void setRootLevelPath(const QString &rootPath);
public slots:
    bool cdUp();
    void setRootPath( const QString &pathName );
    
    // triggered when drive is ejected/inserted
    void on_driveChanged();
    
    void on_searchPanel_criteriaChanged( const QString &criteria );
    void on_searchPanel_exitClicked();

signals:
    void currentPathChanged( const QString& );
    void startSearch( const QString &criteria );
    void setEmptyMenu( bool isMenuEmpty );
    void setTitle( const QString &title );
    void listActivated();

private slots:
    void on_list_activated( const QModelIndex &index );
    void on_listActivated();
    void on_tree_activated( const QModelIndex &index );
	void on_list_longPressed( HbAbstractViewItem *item, const QPointF &coords );
	void on_list_pressed( const QModelIndex &  index ) ;
    void on_tree_longPressed( HbAbstractViewItem *item, const QPointF &coords );    
    
    void on_viewAction_triggered();
    void on_copyAction_triggered();
    void on_moveAction_triggered();
    void on_deleteAction_triggered();
    void on_renameAction_triggered();
    void on_sendAction_triggered();

private:
    void initListView();
    void initTreeView();
    void initFileModel();
	void initLayout();
	void initSearchPanel();
	void initEmptyTipsLabel();

    void changeRootIndex( const QModelIndex &index );
	bool isDriver(const QModelIndex &index) const;
	bool isFolder(const QModelIndex &index) const;
    
	HbTreeView *mTreeView;
    HbListView *mListView;
    QGraphicsLinearLayout *mLayout;
    QFileSystemModel *mSourceModel;
    FmFileSystemProxyModel *mModel;
    
    bool mSelectable;
    
    //currentStyle
    Style mStyle;
    //used to store original tree/list style
    Style mFileBrowseStyle;

	HbAbstractViewItem* mCurrentItem;	
	FmOperationService *mOperationService;
    
    QString mFindTargetPath;
    HbSearchPanel *mSearchPanel;
    HbLabel *mEmptyTipLabel;
    
    QString mCurrentDrive;
    
    //used to avoid activate when long press list.
    bool mListLongPressed;
    QModelIndex mActivatedModelIndex;
    
    // provide icon from filemanger
    FmFileIconProvider *mFileIconProvider;
    //root path level to contain back action
    QString mRootLevelPath;
};

#endif
