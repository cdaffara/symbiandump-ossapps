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

#include <QStandardItemModel>
#include <QDebug>

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbaction.h>

#include <hbtreeview.h>
#include <hbscrollbar.h>
#include <hbmenu.h>
#include <hbtoolbar.h>

#include "HelpDataProvider.h"

#include "HelpCategoryView.h"


//////////////////////////////////////////////////////////////////////////////////////////////

HelpCategoryView::HelpCategoryView() : 
mListAll(NULL),
mExpandCount(0)
{
}

HelpCategoryView::~HelpCategoryView()
{
}


////////////////////////////////////////////////////////////////////////////////////////////

void HelpCategoryView::init()
{
	initDocMl();
    initAllList();
    emit applicationReady();
}

void HelpCategoryView::initDocMl()
{
	initBaseDocMl();
    mBuilder.load(QRC_DOCML_CATEGORY);
}

void HelpCategoryView::initAllList()
{
    mListAll = mBuilder.findWidget<HbTreeView*>(DOCML_LIST_CATEGORY_ALL);
    mListAll->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    mListAll->setModel(HelpDataProvider::instance()->getCategoryData());
    connect(mListAll, SIGNAL(activated(const QModelIndex&)), this, SLOT(onAllListActivated(const QModelIndex&)));
}

////////////////////////////////////////////////////////////////////////////////////////////

void HelpCategoryView::expandCollapseAllList(QStandardItem* item, bool expand)
{
	if(item->rowCount() <= 0)
	{
		return;
	}
	mListAll->setExpanded(item->index(),expand);
	for(int i = 0; i < item->rowCount(); i++)
	{		
		expandCollapseAllList(item->child(i),expand);
	}
}

void HelpCategoryView::updateExpandCollapseAction(bool expand)
{
	HbAction* allAction = mBuilder.findObject<HbAction*>(DOCML_ACTION_EXPAND_COLLAPSE_ALL);
	allAction->setText(expand ? hbTrId(TXT_EXPAND_ALL) : hbTrId(TXT_COLLAPSE_ALL));
}

////////////////////////////////////////////////////////////////////////////////////////////
// handle list event

void HelpCategoryView::onAllListActivated(const QModelIndex& index)
{
    if(!index.isValid() ||          // invalid
       index.child(0,0).isValid())  // this is a node
    {
		if(index.parent().isValid())
		{
			return;
		}
		
		if(mListAll->isExpanded(index))
		{
			mExpandCount++;
		}
		else
		{
			mExpandCount--;
		}
		updateExpandCollapseAction(mExpandCount == 0);
        return;
    }

	QString uid = mListAll->model()->data(index, UidRole).toString();
    QString href = mListAll->model()->data(index, HrefRole).toString();
    HelpDataProvider::instance()->setHelpContentUrl(uid, href);
    emit activateView(HelpViewContents);
}

////////////////////////////////////////////////////////////////////////////////////////////
// handle menu event
void HelpCategoryView::onExpandOrCollapseAll()
{
	bool needExpand = (mExpandCount == 0);
	QStandardItemModel* model = (QStandardItemModel*)(mListAll->model());
	expandCollapseAllList(model->invisibleRootItem(),needExpand);
	mExpandCount = needExpand ?  model->invisibleRootItem()->rowCount() : 0;
	updateExpandCollapseAction(mExpandCount == 0);
}

// end of file
