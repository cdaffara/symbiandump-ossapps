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
*     The header file of the file find widget of file manager
*
*/

#ifndef FMFINDWIDGET_H
#define FMFINDWIDGET_H

#include "fmcommon.h"
#include "fmfindresultmodel.h"

#include <hbwidget.h>

class HbListView;
class HbSearchPanel;
class HbWidget;
class HbStackedWidget;

class QGraphicsLinearLayout;

class FmFindWidget : public HbWidget
{
    Q_OBJECT
public:
    FmFindWidget( QGraphicsItem *parent = 0 );
    ~FmFindWidget();

    void find( const QString &keyword, const QStringList &pathList );
    void stopFind();
    void sortFiles( FmFindResultModel::SortFlag sortFlag );

signals:
    void activated( const QString &pathName );
    void setEmptyMenu( bool isMenuEmpty );

private slots:
    void itemActivated(const QModelIndex &index);
        
    void on_searchPanel_criteriaChanged( const QString &criteria );
    void on_searchPanel_exitClicked();

    void on_resultModel_modelCountChanged( int count );

    void on_resultModel_findStarted();
    void on_resultModel_findFinished();
    
private:
    void init();
    void initSearchPanel();

private:
    HbListView          *mListView;
    FmFindResultModel   *mModel;
    HbWidget            *mEmptyTipWidget;
    
    QGraphicsLinearLayout *mLayout;
	
	// used to stack emptytip widget and list view.
	// only one widget can be shown at the same time.
    HbStackedWidget  *mContentWidget;
	
    HbSearchPanel*  mSearchPanel;
	
	// store the find path list
	// for example, if start find from drive view. All available drive will be used to find
    QStringList     mPathList;
};

#endif
