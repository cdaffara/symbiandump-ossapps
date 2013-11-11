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

#include <hbtoolbar.h>
#include <hbnotificationdialog.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbactivitymanager.h>

#include "HelpBaseView.h"
#include "HelpCategoryView.h"
#include "HelpKeywordView.h"
#include "HelpContentsView.h"

#include "HelpMainWindow.h"
#include "HelpDataProvider.h"

HelpMainWindow::HelpMainWindow() : 
mCategoryView(NULL),
mKeywordView(NULL),
mContentsView(NULL)
{
    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
	HelpDataProvider::instance()->createHelpCategory();
    activateCategoryView();
}

HelpMainWindow::~HelpMainWindow()
{
    HelpDataProvider::destroyInstance();
}

void HelpMainWindow::onActivateView(HelpViewName viewName)
{
	switch(viewName)
	{
	    case HelpViewCategory:
            activateCategoryView();
			break;
	    case HelpViewKeyword:
            activateKeywordView();
			break;
	    case HelpViewContents:
            activateContentsView();
			break;
		case PreviousView:
			{
				onActivateView(mPreviousViewName);
			}
			break;
	    default:
			HbApplication::exit();
	        break;
	}
}

void HelpMainWindow::activateCategoryView()
{
    if(!mCategoryView)
    {
        mCategoryView = new HelpCategoryView();
        addView(mCategoryView);
        mCategoryView->init();
        emit currentViewChanged(mCategoryView);
		connectViewSignal(mCategoryView);
    }

	mPreviousViewName = HelpViewCategory;
    setCurrentView(mCategoryView);
}

void HelpMainWindow::activateKeywordView()
{
    if(!mKeywordView)
    {
        mKeywordView = new HelpKeywordView();
		addView(mKeywordView);
        mKeywordView->init();
		connectViewSignal(mKeywordView);
    }

	mPreviousViewName = HelpViewKeyword;	
    setCurrentView(mKeywordView);	
}

void HelpMainWindow::activateContentsView()
{
    if(!mContentsView)
    {
		mContentsView = new HelpContentsView();
		addView(mContentsView);
        mContentsView->init();

        connectViewSignal(mContentsView);
    }
    setCurrentView(mContentsView);
}

void HelpMainWindow::connectViewSignal(const HelpBaseView *view)
{
	connect(this, SIGNAL(orientationChanged(Qt::Orientation)), view, SLOT(onOrientationChanged(Qt::Orientation)));
    connect(view, SIGNAL(activateView(HelpViewName)), this, SLOT(onActivateView(HelpViewName)));
    
    connect(view, SIGNAL(showAllList()), this, SLOT(onShowAllList()));
    connect(view, SIGNAL(showFindList()), this, SLOT(onShowFindList()));
}


////////////////////////////////////////////////////////////////////////////////////
// handle view event

void HelpMainWindow::onShowAllList()
{
    activateCategoryView();
}

void HelpMainWindow::onShowFindList()
{	
    activateKeywordView();
	mKeywordView->loadAllContent();
}

void HelpMainWindow::saveActivity()
{
	HbActivityManager* activityManager = qobject_cast<HbApplication*>(qApp)->activityManager();

	// clean up any previous versions of this activity from the activity manager.
	bool ok = activityManager->removeActivity("UserGuideMainView");
	if ( !ok )
	{
		//qFatal("Remove failed" );
	}

	// get a screenshot for saving to the activity manager
	QVariantHash metadata;
	metadata.insert("screenshot", QPixmap::grabWidget(this, rect()));

	// save any data necessary to save the state
	QByteArray serializedActivity;
	QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);

	// add the activity to the activity manager
	ok = activityManager->addActivity("UserGuideMainView", serializedActivity, metadata);
	if ( !ok )
	{
		qFatal("Add failed" );
	}
}

// end of file

