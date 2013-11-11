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
#include <HbMessageBox>
#include <HbLabel>

#include "memspyheapdumpsview.h"
#include "enginewrapper.h"

const QStringList MODE_ITEMS = QStringList() << 
	MemSpyHeapDumpsView::tr("Kernel") << 
	MemSpyHeapDumpsView::tr("User") << 
	MemSpyHeapDumpsView::tr("Both");
	
MemSpyHeapDumpsView::MemSpyHeapDumpsView(EngineWrapper &engine, ViewManager &viewManager) : 
	MemSpyView(engine, viewManager),
	mModel(0),
	mModeItem(0),
	mFilterItem(0),
	mDwoProgressDialog(0)
{
}

MemSpyHeapDumpsView::~MemSpyHeapDumpsView()
{
	delete mDwoProgressDialog;
}

void MemSpyHeapDumpsView::initialize(const QVariantMap& params)
{
	setTitle(tr("Heap Dumps"));
	
	MemSpyView::initialize(params);
}

HbToolBar* MemSpyHeapDumpsView::createToolBar()
{
	HbToolBar* toolBar = new HbToolBar();
	toolBar->addAction(tr("Dump Now"), this, SLOT(dump()));
	return toolBar;
}

HbWidget* MemSpyHeapDumpsView::createCentralWidget()
{
	mModel = new HbDataFormModel(this);
	
	//TODO: uncomment after kernel heap dumps are implemented 
//	mModeItem = mModel->appendDataFormItem(
//		HbDataFormModelItem::ComboBoxItem, tr("Heap Dump"));
//	mModeItem->setContentWidgetData("items", MODE_ITEMS);
//	mModeItem->setContentWidgetData("currentIndex", mEngine.settings().heapDumpsMode());
//	modeChanged(mEngine.settings().heapDumpsMode());
	
	//TODO: remove after kernel heap dumps are implemented
	createFilterItem();
	
	HbDataForm* form = new HbDataForm(this);
	form->setModel(mModel);
	
	//TODO: uncomment after kernel heap dumps are implemented
//	form->addConnection(mModeItem, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));
	
	return form;
}

void MemSpyHeapDumpsView::modeChanged(int mode)
{
	if (mode == HeapDumpsModeKernel && mFilterItem)
		removeFilterItem();
	else if (mode != HeapDumpsModeKernel && !mFilterItem)
		createFilterItem();
}

void MemSpyHeapDumpsView::createFilterItem()
{
	mFilterItem = mModel->appendDataFormItem(
			HbDataFormModelItem::TextItem, tr("Filter"));
}

void MemSpyHeapDumpsView::removeFilterItem()
{
	mModel->removeItem(mFilterItem);
	mFilterItem = 0;
}

void MemSpyHeapDumpsView::dump()
{
    mEngine.updateOutputSettings();
    
	HeapDumpsMode mode = HeapDumpsModeUser;
	//TODO: uncomment after kernel heap dumps are implemented 
	//static_cast<HeapDumpsMode>(mModeItem->contentWidgetData("currentIndex").toInt());
	
	// save settings
	//TODO: uncomment after kernel heap dumps are implemented
	//mEngine.settings().setHeapDumpsMode(mode);

	if (mode == HeapDumpsModeKernel || mode == HeapDumpsModeBoth) {
		// dump kernel heap
		mDwoProgressDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
		mDwoProgressDialog->setText(tr("Please wait..."));
		mDwoProgressDialog->show();
		
		MemSpyKernelHeapDumpTracker* tracker = mEngine.createKernelHeapDumpTracker();
		connect(tracker, SIGNAL(finished(int)), this, SLOT(asyncOperationFinished(int)));
		tracker->start();
	}
	
	if (mode == HeapDumpsModeUser || mode == HeapDumpsModeBoth) {
		// dump user heap
		
		QString filter = mFilterItem->contentWidgetData("text").toString();
		if (!filter.isEmpty()) {
		    
			int errorCode = mEngine.outputThreadHeapData(filter);
            if (errorCode == KErrNotFound) {
                HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
                messageBox->setText(tr("No thread matches your filter"));
                HbLabel *header = new HbLabel(tr("No such thread"), messageBox);
                messageBox->setHeadingWidget(header);
                messageBox->setAttribute(Qt::WA_DeleteOnClose);
                messageBox->setTimeout(HbPopup::StandardTimeout);
                messageBox->open();
            }
			    
			
		} else {
			mDwoProgressDialog = new HbProgressDialog(HbProgressDialog::ProgressDialog);
			mDwoProgressDialog->setMinimum(0);
			mDwoProgressDialog->setMaximum(100);
			mDwoProgressDialog->show();
			
			MemSpyDwoTracker* tracker = mEngine.createDeviceWideOperation(OutputHeapData);
			connect(tracker, SIGNAL(progress(int,QString)), this, SLOT(updateDwoProgress(int,QString)));
			connect(tracker, SIGNAL(finished(int)), this, SLOT(asyncOperationFinished(int)));
			connect(mDwoProgressDialog, SIGNAL(cancelled()), tracker, SLOT(cancel()));
			tracker->start();
		}
	}
}

void MemSpyHeapDumpsView::updateDwoProgress(int progress, const QString& processName)
{
	mDwoProgressDialog->setText(processName);
	mDwoProgressDialog->setProgressValue(progress);
}

void MemSpyHeapDumpsView::asyncOperationFinished(int errorCode)
{
	mDwoProgressDialog->hide();
	delete mDwoProgressDialog;
	mDwoProgressDialog = 0;
	
	delete sender();    
	
	if (errorCode != KErrNone && errorCode != KErrCancel) {
	    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeWarning);
        messageBox->setText(tr("An error occured during the operation. Error code: %1").arg(errorCode));
        HbLabel *header = new HbLabel(tr("Error"), messageBox);
        messageBox->setHeadingWidget(header);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setTimeout(HbPopup::StandardTimeout);
        messageBox->open();
	}
}
