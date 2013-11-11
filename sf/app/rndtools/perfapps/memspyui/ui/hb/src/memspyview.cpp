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

#include <QSignalMapper>
#include <QGraphicsLinearLayout>
#include <QDebug>
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <HbApplication>
#include <HbMessageBox>
#include <HbMainWindow>
#include <HbLabel>

#include "memspyview.h"
#include "enginewrapper.h"

#include "memspysettingsview.h"

MemSpyView::MemSpyView(EngineWrapper &engine, ViewManager &viewManager) : 
	HbView(),
	mEngine(engine),
	mViewManager(viewManager),
	mOutputMenu(0), 
	mOutputGenInfoMenu(0), 
	mOutputHeapInfoMenu(0),
	mOutputStackInfoMenu(0),
	mToolBar(0),
	mBreadCrumb(0)
{
}

MemSpyView::~MemSpyView()
{
	delete mBreadCrumb;
	delete mToolBar;
	delete mOutputStackInfoMenu;
	delete mOutputHeapInfoMenu;
	delete mOutputGenInfoMenu; 
	delete mOutputMenu;
}

void MemSpyView::initialize(const QVariantMap& params)
{
	Q_UNUSED(params);
	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, this);
	
	if (isBreadCrumbVisible()) {
		mBreadCrumb = new HbLabel(this);
		mBreadCrumb->setTextWrapping(Hb::TextWordWrap);
		mBreadCrumb->setElideMode(Qt::ElideLeft);
		mBreadCrumb->setPlainText(getBreadCrumbText());
		HbFontSpec spec(HbFontSpec::PrimarySmall);
		mBreadCrumb->setFontSpec(spec);
		layout->addItem(mBreadCrumb);
	}
	layout->addItem(createCentralWidget());
	setLayout(layout); 
	
	if (isRefreshable())
		menu()->addAction(tr("Refresh"), this, SLOT(refresh()));
	
	HbMenu* toolMenu = createToolMenu();
	if (toolMenu)
		menu()->addMenu(toolMenu);
	
	menu()->addAction(tr("Settings ..."), this, SLOT(showSettings()));
	menu()->addAction(tr("About ..."), this, SLOT(showAbout()));
	menu()->addAction(tr("Exit"), qApp, SLOT(quit()));
	
	mToolBar = createToolBar();
	if (mToolBar)
		setToolBar(mToolBar);
}

QList<QAction*> MemSpyView::createOutputActions()
{
	return QList<QAction*>();
}

HbMenu* MemSpyView::createToolMenu()
{
	return 0;
}

HbToolBar* MemSpyView::createToolBar()
{
	return 0;
}

bool MemSpyView::isRefreshable() const
{
	return false;
}

bool MemSpyView::isBreadCrumbVisible() const
{
    return false;
}

QString MemSpyView::getBreadCrumbText() const
{
	QStringList views;
	for (int i=1; i<mainWindow()->views().count() - 1; i++)	{
		const HbView *view = mainWindow()->views().at(i);
		if (view == this)
			break;
		views.append(view->title());
	}
	views.append(title());
	
	return views.join(" > ");
}

void MemSpyView::refresh()
{
	// Empty default implementation
}

void MemSpyView::showSettings()
{
	MemSpySettingsView* settings = new MemSpySettingsView(mEngine);
	connect(settings, SIGNAL(finished(bool)), this, SLOT(closeSettings()));
	mainWindow()->addView(settings);
	mainWindow()->setCurrentView(settings);
}

void MemSpyView::showAbout()
{
	HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
	messageBox->setText("Version 2.2.2 - 21st September 2010. Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. Licensed under Eclipse Public License v1.0.");
	HbLabel *header = new HbLabel("About MemSpy", messageBox);
	messageBox->setHeadingWidget(header);
	messageBox->setAttribute(Qt::WA_DeleteOnClose);
	messageBox->setTimeout(HbPopup::NoTimeout);
	messageBox->open();
}

void MemSpyView::closeSettings()
{
	sender()->deleteLater();
	mainWindow()->setCurrentView(this);
}
