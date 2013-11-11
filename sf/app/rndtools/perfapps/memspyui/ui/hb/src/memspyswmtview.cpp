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

#include <HbToolBar>
#include <HbAction>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbProgressDialog>

#include "memspyswmtview.h"
#include "enginewrapper.h"

const QStringList MODE_ITEMS = QStringList() << 
	MemSpySwmtView::tr("Basic") << 
	MemSpySwmtView::tr("Full") << 
	MemSpySwmtView::tr("Custom");
 
const QStringList CATEGORIES_ITEMS = QStringList() <<
	MemSpySwmtView::tr("FileServer Cache") <<
	//MemSpySwmtView::tr("Bitmap Handles") <<
	MemSpySwmtView::tr("User Heap") <<
	//MemSpySwmtView::tr("Kernel Heap") <<
	MemSpySwmtView::tr("Local Chunks") <<
	MemSpySwmtView::tr("Global Chunks") <<
	MemSpySwmtView::tr("RAM Drive") <<
	//MemSpySwmtView::tr("User Stacks") <<
	//MemSpySwmtView::tr("Global Data") <<
	MemSpySwmtView::tr("RAM-loaded Code") <<
	MemSpySwmtView::tr("Kernel Handles") <<
	MemSpySwmtView::tr("Open Files") <<
	MemSpySwmtView::tr("Disk Usage") <<
	MemSpySwmtView::tr("System Memory") <<
	MemSpySwmtView::tr("Windows Groups");
	
MemSpySwmtView::MemSpySwmtView(EngineWrapper &engine, ViewManager &viewManager) : 
	MemSpyView(engine, viewManager), 
	mToggleTimerAction(0),
	mCategoriesItem(0),
	mProgressDialog(0)
{
}

MemSpySwmtView::~MemSpySwmtView()
{
}

void MemSpySwmtView::initialize(const QVariantMap& params)
{
	setTitle(tr("SWMT"));
	
	MemSpyView::initialize(params);
}

HbToolBar* MemSpySwmtView::createToolBar()
{
	HbToolBar* toolBar = new HbToolBar();
	mToggleTimerAction = toolBar->addAction("", this, SLOT(toggleTimer()));
	toolBar->addAction(tr("Dump Now"), this, SLOT(forceDump()));
	
	updateTimerAction(mEngine.isSwmtRunning());
	
	return toolBar;
}

HbWidget* MemSpySwmtView::createCentralWidget()
{
	mModel = new HbDataFormModel(this);
	
	mTimerItem = mModel->appendDataFormItem(
			HbDataFormModelItem::TextItem, tr("Timer (sec.)"));
	mTimerItem->setContentWidgetData("maxLength", 2);
	mTimerItem->setContentWidgetData("text", mEngine.settings().swmtTimerPeriod());
	
	mModeItem = mModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem, tr("Tracking mode"));
	mModeItem->setContentWidgetData("items", MODE_ITEMS);
	mModeItem->setContentWidgetData("currentIndex", mEngine.settings().swmtMode());
	
	modeChanged(mEngine.settings().swmtMode());
	
	HbDataForm* form = new HbDataForm(this);
	form->setModel(mModel);
	
	form->addConnection(mModeItem, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));
	
	return form;
}

void MemSpySwmtView::updateTimerAction(bool isRunning)
{
	mToggleTimerAction->setText(isRunning ? tr("Stop Timer") : tr("Start Timer"));
}

void MemSpySwmtView::toggleTimer()
{
	bool wasRunning = mEngine.isSwmtRunning();
	if (wasRunning)
		mEngine.stopSwmt();
	else
	{
		updateSettings();
		mEngine.startSwmt(qBound(5, mTimerItem->contentWidgetData("text").toInt(), 60));
	}
	
	updateTimerAction(!wasRunning);
}

void MemSpySwmtView::forceDump()
{
	updateSettings();
	mEngine.updateOutputSettings();
		
	MemSpySwmtDumpTracker* tracker = mEngine.createSwmtDumpTracker();
	connect(tracker, SIGNAL(finished(int)), this, SLOT(asyncOperationFinished(int)));
	
	mProgressDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
	mProgressDialog->setText(tr("Please wait..."));
	mProgressDialog->removeAction(mProgressDialog->actions().at(0));
	mProgressDialog->show();
	
	tracker->start();
}

void MemSpySwmtView::updateSettings()
{
	mEngine.settings().setSwmtTimerPeriod(mTimerItem->contentWidgetData("text").toInt());
	mEngine.settings().setSwmtMode(static_cast<SwmtMode>(
			mModeItem->contentWidgetData("currentIndex").toInt()));
	if (mCategoriesItem)
		mEngine.settings().setSwmtCategories(mCategoriesItem->contentWidgetData("selectedItems").toList());
	
	mEngine.setSwmtSettings(static_cast<SwmtMode>(mEngine.settings().swmtMode()), 
		mEngine.settings().swmtCategories());
}

void MemSpySwmtView::modeChanged(int mode)
{
	if (mode != SwmtModeCustom && mCategoriesItem)
		removeCategoriesItem();
	else if (mode == SwmtModeCustom && !mCategoriesItem)
		createCategoriesItem();
}

void MemSpySwmtView::asyncOperationFinished(int errorCode)
{
	Q_UNUSED(errorCode);
		
	mProgressDialog->hide();
	delete mProgressDialog;
	mProgressDialog = 0;
	
	delete sender();
}

void MemSpySwmtView::createCategoriesItem()
{
	mCategoriesItem = mModel->appendDataFormItem(
			HbDataFormModelItem::MultiselectionItem, tr("Categories"));
	mCategoriesItem->setContentWidgetData("items", CATEGORIES_ITEMS);
	mCategoriesItem->setContentWidgetData("selectedItems", mEngine.settings().swmtCategories());
}

void MemSpySwmtView::removeCategoriesItem()
{
	mModel->removeItem(mCategoriesItem);
	mCategoriesItem = 0;
}
