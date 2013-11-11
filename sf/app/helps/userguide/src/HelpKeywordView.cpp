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
#include <hbtoolbar.h>
#include <hblabel.h>

#include <hbtreeview.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <hbscrollbar.h>
#include <hblineedit.h>
#include <hbmenu.h>
#include <hbstaticvkbhost.h>

#include "HelpDataProvider.h"
#include "HelpProxyModel.h"

#include "HelpKeywordView.h"


//////////////////////////////////////////////////////////////////////////////////////////////

HelpKeywordView::HelpKeywordView() : 
mListSearch(NULL), 
mSearchPanel(NULL)
{
}

HelpKeywordView::~HelpKeywordView()
{
}


////////////////////////////////////////////////////////////////////////////////////////////

void HelpKeywordView::init()
{
	initDocMl();
    initBackAction();
    initSearchList();
    initSearchPanel();
	initVirtualKeyboard();
    
    connect(mainWindow(), SIGNAL(viewReady()), this, SLOT(onViewReady()));
}

void HelpKeywordView::initDocMl()
{
	initBaseDocMl(); 
	mBuilder.load(QRC_DOCML_KEYWORD);
}

void HelpKeywordView::initBackAction()
{
    mSoftKeyAction = new HbAction(Hb::BackNaviAction);
    connect(mSoftKeyAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
}

void HelpKeywordView::initSearchList()
{
    mListSearch = mBuilder.findWidget<HbListView*>(DOCML_LIST_SEARCH);
    mListSearch->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    mListSearch->setModel(HelpDataProvider::instance()->getSearchData());
    connect(mListSearch, SIGNAL(activated(const QModelIndex&)), this, SLOT(onSearchListActivated(const QModelIndex&)));
}
void HelpKeywordView::initSearchPanel()
{
    mSearchPanel = mBuilder.findWidget<HbSearchPanel*>(DOCML_SEARCH_PANEL);
    connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(onSearchPanelExitClicked()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(const QString&)), this, SLOT(onSearchPanelCriteriaChanged(const QString&)));
}

void HelpKeywordView::initVirtualKeyboard()
{
	mVirtualKeyboard = new HbStaticVkbHost(this);
    connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(onHandleKeypadOpen()));
    connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(onHandleKeypadClose()));
}

HbLabel* HelpKeywordView::label()
{
	return mBuilder.findWidget<HbLabel*>(DOCML_NO_MATCH_LABEL);
}

void HelpKeywordView::loadAllContent()
{
	showToolBar(false);
	mBuilder.load(QRC_DOCML_KEYWORD, DOCML_LAYOUT_SEARCH);
	ResetSearchPanel();
	
}

////////////////////////////////////////////////////////////////////////////////////////////

void HelpKeywordView::updateVisibleItems(bool visible)
{
	static Hb::SceneItems items = Hb::TitleBarItem | Hb::StatusBarItem;
	if(visible)
	{
		showItems(items);
	}
	else
	{
		hideItems(items);
	}
}

void HelpKeywordView::ResetSearchPanel()
{
    foreach(QGraphicsItem *obj, mSearchPanel->childItems())
    {
        QGraphicsWidget *const widget = static_cast<QGraphicsWidget*>(obj);
        if(widget != NULL)
        {
            HbLineEdit *const lineEdit = qobject_cast<HbLineEdit*>(widget);
            if(lineEdit != NULL)
            {
                lineEdit->setText("");
				lineEdit->setFocus();
                break;
            }
        }
    }
}

void HelpKeywordView::updateLabelPos()
{
	HbDeviceProfile profile = HbDeviceProfile::profile(mainWindow());
	qreal unValue = profile.unitValue();
	if(mainWindow()->orientation() == Qt::Vertical)
	{
		if(mVirtualKeyboard->keypadStatus() == HbVkbHost::HbVkbStatusClosed)
		{
			label()->setContentsMargins(0,30 * unValue,0,0);
		}
		else
		{
			label()->setContentsMargins(0,10 * unValue,0,0);
		}
	}
	else
	{
		if(mVirtualKeyboard->keypadStatus() == HbVkbHost::HbVkbStatusClosed)
		{
			label()->setContentsMargins(0,10 * unValue,0,0);
		}
		else
		{
            label()->setContentsMargins(0,0,0,0);
			label()->setAlignment(label()->alignment() | Qt::AlignVCenter);
		}
	}
}

void HelpKeywordView::showToolBar(bool visible)
{
	Hb::SceneItems items = Hb::ToolBarItem;
	if(visible)
	{
		showItems(items);
	}
	else
	{
		hideItems(items);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// handle system event

void HelpKeywordView::onViewReady()
{
	if(isVisible())
	{
		setNavigationAction(mSoftKeyAction);  
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// handle button back action

void HelpKeywordView::onBackAction()
{
	emit activateView(HelpViewCategory);
}


////////////////////////////////////////////////////////////////////////////////////////////
// handle list event

void HelpKeywordView::onSearchListActivated(const QModelIndex& index)
{
    if(!index.isValid() ||          // invalid
       index.child(0,0).isValid())  // this is a node
    {
        return;
    }
    
    QString uid = mListSearch->model()->data(index, UidRole).toString();
    QString href = mListSearch->model()->data(index, HrefRole).toString();
    HelpDataProvider::instance()->setHelpContentUrl(uid, href);
    emit activateView(HelpViewContents);
}


////////////////////////////////////////////////////////////////////////////////////////////
// handle search panel event

void HelpKeywordView::onSearchPanelExitClicked()
{
	if(mListSearch->model()->rowCount() == 0)
	{
		mBuilder.load(QRC_DOCML_KEYWORD, DOCML_LAYOUT_SEARCH_NO_SRHPAL_NO_MATCH);
	}
	else
	{
		mBuilder.load(QRC_DOCML_KEYWORD, DOCML_LAYOUT_SEARCH_NO_SRHPAL);
	}	
	showToolBar(true);
}

void HelpKeywordView::onSearchPanelCriteriaChanged(const QString &criteria)
{
    HelpDataProvider::instance()->getSearchData(criteria);
	if(mListSearch->model()->rowCount() == 0)
	{
		mBuilder.load(QRC_DOCML_KEYWORD, DOCML_LAYOUT_SEARCH_NO_MATCH);
	}
	else
	{
		mBuilder.load(QRC_DOCML_KEYWORD, DOCML_LAYOUT_SEARCH);
	}
	showToolBar(false);
}

////////////////////////////////////////////////////////////////////////////////////////////
// handle virtual keyboard event

void HelpKeywordView::onHandleKeypadOpen()
{
	updateVisibleItems(false);
    qreal heightToSet = mainWindow()->layoutRect().height() - mVirtualKeyboard->keyboardArea().height();
    this->setMaximumHeight(heightToSet);
	updateLabelPos();
}

void HelpKeywordView::onHandleKeypadClose()
{	
	updateVisibleItems(true);
	qreal mainHeight  = mainWindow()->layoutRect().height();
	qreal toolbarHeight = toolBar()->size().height();
	qreal height = mainHeight - ( toolBar()->isVisible() ? toolbarHeight : 0);
	this->setMaximumHeight(height);
	updateLabelPos();
}

void HelpKeywordView::onOrientationChanged(Qt::Orientation orientation)
{
	Q_UNUSED(orientation);
	updateLabelPos();
}

// end of file
