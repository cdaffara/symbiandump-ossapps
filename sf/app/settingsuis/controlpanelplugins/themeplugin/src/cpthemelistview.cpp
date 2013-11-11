/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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


#include <QGraphicsLinearLayout>
#include <QModelIndex>

#include <hbview.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbgroupbox.h>


#include "cpthemelistview.h"

/*!
    \class CpThemeListView
    \brief CpThemeListView displays a heading (e.g Theme) and a list of themes with
    corresponding icons.

    Note: This class is a subclass of CpBaseSettingView for compatibility with Control Panel
          framework.  
 */

/*!
    constructor.  Creates the heading label and the list and adds it to layout.
*/
CpThemeListView::CpThemeListView(QGraphicsItem *parent) : CpBaseSettingView(0, parent),
    mThemeList(new HbListView(this))
{
   
    //Create a layout with a heading at top and the list below it.
    HbWidget* contentWidget = new HbWidget(this);
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0,0,0,0);
    
    //setup the heading.
    HbGroupBox *simpleLabel = new HbGroupBox();
    simpleLabel->setHeading(hbTrId("txt_cp_title_select_theme"));
     
    layout->addItem(simpleLabel);
      
    connect(mThemeList, SIGNAL(activated(QModelIndex)),
            this, SIGNAL(newThemeSelected(QModelIndex)));
    
    //set list item icons to be large.
    HbListViewItem* listViewItem = mThemeList->listItemPrototype();
    listViewItem->setGraphicsSize(HbListViewItem::LargeIcon);
    //set singleSelection to enable showing an indicator (e.g check mark) next to active theme.
    mThemeList->setSelectionMode(HbAbstractItemView::SingleSelection);
  
    //add the list to layout.
    layout->addItem(mThemeList);
    
    //Create the toolbar for Ovi Store.
    HbToolBar* toolBar = new HbToolBar(this);

    HbAction* oviAction = new HbAction(HbIcon("qtg_large_ovistore"), hbTrId("txt_cp_list_get_more_themes"));
    QObject::connect( oviAction, SIGNAL(triggered()), 
                          this, SIGNAL(oviClicked()));
       
    //Add Action to the toolbar and show toolbar
    toolBar->addAction(oviAction);
       
    setToolBar(toolBar);

    contentWidget->setLayout(layout);
   
    setWidget(contentWidget);
   
}

/*!
    destructor.
*/
CpThemeListView::~CpThemeListView()
{
}

/*!
    returns the listview instance (list of themes).
*/
HbListView* CpThemeListView::themeList() const
{
    return mThemeList;
}

/*!
    Sets the model of its listView.
*/
void CpThemeListView::setModel(QAbstractItemModel* model)
{
    mThemeList->setModel(model);
}

/*!
    sets the widget. Reimplementation from HbView. 
*/
void CpThemeListView::setWidget(QGraphicsWidget *widget)
{
    HbView::setWidget(widget);
}

/*!
    emits aboutToClose() signal.
*/
void CpThemeListView::closeView()
{
    emit aboutToClose();
}


