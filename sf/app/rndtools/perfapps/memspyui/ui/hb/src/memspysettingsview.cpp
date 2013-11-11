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

#include "memspysettingsview.h"
#include "enginewrapper.h"

const QStringList MODE_ITEMS = QStringList() << 
	MemSpySettingsView::tr("RDebug") << 
	MemSpySettingsView::tr("File");
	
MemSpySettingsView::MemSpySettingsView(EngineWrapper &engine) :
	mForm(new HbDataForm(this)),
	mModel(0),
	mModeItem(0),
	mPathItem(0),
	mCustomPathItem(0),
	mEngine(engine)
{
	setTitle(tr("Settings"));
	
	toolBar()->addAction(tr("OK"), this, SLOT(accept()));
	toolBar()->addAction(tr("Cancel"), this, SLOT(reject()));
	
	mModel = new HbDataFormModel(this);
		
	mModeItem = mModel->appendDataFormItem(
		HbDataFormModelItem::ComboBoxItem, tr("Output"));
	mModeItem->setContentWidgetData("items", MODE_ITEMS);
	mModeItem->setContentWidgetData("currentIndex", mEngine.settings().outputMode());
	
	updateModel();
		
	
	mForm->setModel(mModel);
	mForm->addConnection(mModeItem, SIGNAL(currentIndexChanged(int)), this, SLOT(updateModel()));
	setWidget(mForm);
	
	// change navigation action
	HbAction* action = new HbAction(Hb::BackNaviAction, this);
	connect(action, SIGNAL(triggered()), this, SLOT(reject()));
	setNavigationAction(action);
}

MemSpySettingsView::~MemSpySettingsView()
{
}

void MemSpySettingsView::updateModel()
{
	OutputMode mode = static_cast<OutputMode>(mModeItem->contentWidgetData("currentIndex").toInt());
	if (mode == OutputModeTrace) {
		// remove both path item and custom path item
		if (mPathItem)
			mModel->removeItem(mPathItem);
		if (mCustomPathItem)
			mModel->removeItem(mCustomPathItem);
		
		mPathItem = mCustomPathItem = 0;
	} else if (mode == OutputModeFile) {
		if (!mPathItem) {
			// create path item
			mPathItem = mModel->appendDataFormItem(
					HbDataFormModelItem::CheckBoxItem, tr("Path"));
			mPathItem->setContentWidgetData("text", tr("Use Default Path (\\MemSpy)"));
			mPathItem->setContentWidgetData("checkState", 
					mEngine.settings().outputPath().isEmpty() ? Qt::Checked : Qt::Unchecked);
			
			mForm->addConnection(mPathItem, SIGNAL(stateChanged(int)), 
					this, SLOT(updateModel()));
		}
		
		if (mPathItem->contentWidgetData("checkState").toInt() == Qt::Unchecked && !mCustomPathItem) {
			// create custom path item
			mCustomPathItem = mModel->appendDataFormItem(
					HbDataFormModelItem::TextItem, tr("Custom Path"));
			mCustomPathItem->setContentWidgetData("text", 
					mEngine.settings().outputPath().isEmpty() ? "\\MemSpy" : mEngine.settings().outputPath());
		}
		
		if (mPathItem->contentWidgetData("checkState").toInt() == Qt::Checked && mCustomPathItem) {
			// remove cusom path item
			mModel->removeItem(mCustomPathItem);
			mCustomPathItem = 0;
		}
	}
}

void MemSpySettingsView::accept()
{
	OutputMode mode = static_cast<OutputMode>(mModeItem->contentWidgetData("currentIndex").toInt());
	
	mEngine.settings().setOutputMode(mode);
	
	if (mode == OutputModeFile) {
		QString path = mPathItem->contentWidgetData("checkState").toInt() == Qt::Checked ? 
				"" :
				mCustomPathItem->contentWidgetData("text").toString();
		mEngine.settings().setOutputPath(path);
	}
	emit finished(true);
}

void MemSpySettingsView::reject()
{
	emit finished(false);
}

