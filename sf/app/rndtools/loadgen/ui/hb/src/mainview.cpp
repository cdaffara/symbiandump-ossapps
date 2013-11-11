/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mainview.h"
#include "enginewrapper.h"
#include "notifications.h"
#include "loadgen.hrh"
#include <hblabel.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmessagebox.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
//#include <hbcommonnote.h>
//#include <hbconfirmationquery.h>
#include <hbprogressdialog.h>
#include <hbtextedit.h>
#include <hblistview.h>
#include <hbselectiondialog.h>

#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSignalMapper>

// ---------------------------------------------------------------------------

MainView::MainView(HbMainWindow &mainWindow) :
					  mMainWindow(mainWindow),
					  mEngineWrapper(0),
					  mIsLoadListEmpty(true),
					  mStopAll(NULL),
					  mSuspendAll(NULL),
					  mResumeAll(NULL)
					  
{
}

// ---------------------------------------------------------------------------

MainView::~MainView()
{
    if (mEngineWrapper != 0) {
        delete mEngineWrapper;
    }
    delete mListModel;    
    delete mListView;
	delete mSm;	
}

// ---------------------------------------------------------------------------

void MainView::init(HbApplication &app)
{
	this->setTitle("Load Gener.");
    mEngineWrapper = new EngineWrapper(mMainWindow, *this);
    int error = mEngineWrapper->init();
    Q_ASSERT_X(error == 1, "LoadGen", "Engine initialization failed");
    createMenu(app);
	loadListIinit();
}

// ---------------------------------------------------------------------------

void MainView::createMenu(HbApplication &app)
{
    HbMenu *menu = this->menu();
	
    // signal mapper for identifying the different command actions  
	// in executeCommand() handler function:
    mSm = new QSignalMapper(this);
    connect(mSm, SIGNAL(mapped(int)), this, SLOT(executeMenuCommand(int)));

    if (menu != NULL) {
		// Temp work-around for menu item bug in Orbit Wk38,
		// sub menu cannot contain more than 6 functional action items!
		// two sub menus created to fix this problem.
		mLoadSubMenu1 = menu->addMenu("New loads");
		mLoadSubMenu2 = menu->addMenu("More new loads");
		mCpuLoadAction = mLoadSubMenu1->addAction("CPU load");
		connect(mCpuLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mCpuLoadAction, ELoadGenCmdNewLoadCPULoad);

		mMemoryEatLoadAction = mLoadSubMenu1->addAction("Eat memory");
		connect(mMemoryEatLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mMemoryEatLoadAction, ELoadGenCmdNewLoadEatMemory);

		mPhoneCallLoadAction = mLoadSubMenu1->addAction("Phone calls");
		connect(mPhoneCallLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mPhoneCallLoadAction, ELoadGenCmdNewLoadPhoneCall);

		mMessagesLoadAction = mLoadSubMenu1->addAction("Messages");
		connect(mMessagesLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mMessagesLoadAction, ELoadGenCmdNewLoadMessages);
//TODO to be fixed using replacement for downloadmanager

		mNetConnLoadAction = mLoadSubMenu1->addAction("Network conn.");
		connect(mNetConnLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mNetConnLoadAction, ELoadGenCmdNewLoadNetConn);
	
		mKeyPressLoadAction = mLoadSubMenu2->addAction("Key presses");
		connect(mKeyPressLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mKeyPressLoadAction, ELoadGenCmdNewLoadKeyPress);

		mAppsLoadAction = mLoadSubMenu2->addAction("Applications");
		connect(mAppsLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mAppsLoadAction, ELoadGenCmdNewLoadApplications);
		
		mPhotoCapturesLoadAction = mLoadSubMenu2->addAction("Photo captures");
		connect(mPhotoCapturesLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mPhotoCapturesLoadAction, ELoadGenCmdNewLoadPhotoCaptures);

		mBtActionsLoadAction = mLoadSubMenu2->addAction("Bluetooth actions");
		connect(mBtActionsLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mBtActionsLoadAction, ELoadGenCmdNewLoadBluetooth);

		mPointerEventsLoadAction = mLoadSubMenu2->addAction("Pointer events");
		connect(mPointerEventsLoadAction, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mPointerEventsLoadAction, ELoadGenCmdNewLoadPointerEvent);
		
		mActionPerfMon = menu->addAction("Launch PerfMon");
		connect(mActionPerfMon, SIGNAL(triggered()), this, SLOT( launchPerfMon()));
		
		mActionAbout = menu->addAction("About");
		connect(mActionAbout, SIGNAL(triggered()), this, SLOT( showAboutPopup()));

		mActionExit = menu->addAction("Exit");
		connect(mActionExit, SIGNAL(triggered()), &app, SLOT(quit()));

		// menu dynamic update
		connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));        		
	}
}

// ---------------------------------------------------------------------------

void MainView::loadListIinit()
{
    // Create list view and model for it
    mListView = new HbListView(this);
	mListModel = new QStandardItemModel(this);
	mListView->setModel(mListModel); 
	mListView->setSelectionMode(HbListView::NoSelection);
	mListView->setVisible( false ); //HighlightMode(HbItemHighlight::Hidden);

	QStandardItem *item = new QStandardItem(QString("No loads activated. (Select new load from menu to generate new load)"));
    mListModel->setItem(0, item);

    connect(mListView, 
			SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), 
			this, 
			SLOT(handleLoadListEvent(HbAbstractViewItem*, QPointF)));	

	
    // Create layout and add list view there:
    QGraphicsLinearLayout *mainlayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mainlayout->addItem(mListView);
	mainlayout->setAlignment(mListView, Qt::AlignCenter);
    setLayout(mainlayout);
}

// ---------------------------------------------------------------------------

void MainView::showAboutPopup()
{
    Notifications::about();
}

// ---------------------------------------------------------------------------

void MainView::executeMenuCommand(int cmd)
{
	mEngineWrapper->startNewLoad(cmd);
}

// ---------------------------------------------------------------------------

void MainView::handleLoadListEvent(HbAbstractViewItem */*listViewItem*/, const QPointF &/*coords*/)
{
    const QStringList items = (QStringList() << "Stop" << "Resume/Suspend" << "Edit");
    HbSelectionDialog *dlg = new HbSelectionDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setStringItems(items);
    dlg->setSelectionMode(HbAbstractItemView::SingleSelection);
    dlg->open(this, SLOT(ItemActionPopupClosed(HbAction*)));
}

// ---------------------------------------------------------------------------

void MainView::ItemActionPopupClosed(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(!action && dlg->selectedItems().count()){
        int userSelectionIndex = dlg->selectedItems().at(0).toInt();   
        // all operations for selected list item only. Query selected load list item index from model.
        if (userSelectionIndex == 0) {
            // stop selected load from the load list:
            stopLoad(false);
        }
        else if (userSelectionIndex == 1) {
            // Suspend or Resume
            mEngineWrapper->suspendOrResumeSelectedOrHighlightedItems();
        }
        else {
            // current selected row number from the load list. 
            int selectedRow = mListView->selectionModel()->currentIndex().row();
            // edit load setttings:
            mEngineWrapper->loadEdit(selectedRow);
        }
    }
}

// --------------------------------------------------------------------------

void MainView::stopAllLoads()
{
	stopLoad(true);
}

// ---------------------------------------------------------------------------

bool MainView::stopLoad(bool stopAll)
{
	int success = false;
	if (mListView != NULL) {
		if (stopAll == true) {
			success = mEngineWrapper->stopLoad(stopAll);
			mIsLoadListEmpty = true;
		}
		else {
			success = mEngineWrapper->stopLoad(stopAll);
		}
	}
	return success;
}

// ---------------------------------------------------------------------------

void MainView::updateMenu()
{
	if (mIsLoadListEmpty != true) {
		if (mStopAll == NULL) {
			mStopAll = new HbAction("Stop all", menu());
			connect(mStopAll, SIGNAL(triggered()), this, SLOT(stopAllLoads()));
			menu()->insertAction(mActionAbout, mStopAll);
		}
		else{
            mStopAll->setVisible(true);
		}
		if (mSuspendAll == NULL) {
			mSuspendAll = new HbAction("Suspend all", menu());
			connect(mSuspendAll, SIGNAL(triggered()), this, SLOT(suspendAllLoads()));
			menu()->insertAction(mActionAbout, mSuspendAll);
		}
		else{
            mSuspendAll->setVisible(true);
        }
		if (mResumeAll == NULL) {
			mResumeAll = new HbAction("Resume all", menu());
			connect(mResumeAll, SIGNAL(triggered()), this, SLOT(resumeAllLoads()));
			menu()->insertAction(mActionAbout, mResumeAll);
		}
		else{
            mResumeAll->setVisible(true);
        }
	}
	else if (mIsLoadListEmpty == true) {
		if (mStopAll != NULL && mStopAll->text() == "Stop all") {
            mStopAll->setVisible(false);
		}
		if (mSuspendAll != NULL && mSuspendAll->text() == "Suspend all") {
            mSuspendAll->setVisible(false);
		}
		if (mResumeAll != NULL && mResumeAll->text() == "Resume all") {
            mSuspendAll->setVisible(false);
		}		
	}
}


// ---------------------------------------------------------------------------

int MainView::currentItemIndex()
{
	return mListView->selectionModel()->currentIndex().row();
}

// ---------------------------------------------------------------------------

void MainView::setCurrentItemIndex(int index) 
{
    QModelIndex newIndex = mListView->model()->index(index, 0);
    mListView->selectionModel()->setCurrentIndex(newIndex, 
                                                QItemSelectionModel::SelectCurrent);
	//mListView->setCurrentIndex( index ); //setCurrentRow(index);
}	

// ---------------------------------------------------------------------------

void MainView::launchPerfMon()
{
	mEngineWrapper->launchPerfMonApp();
}

// ---------------------------------------------------------------------------

QList<int> MainView::listSelectionIndexes()
    {
	QList<int> indices;
    if (mListView != NULL) {
		QItemSelectionModel *selectionModel = mListView->selectionModel();
		if (selectionModel->hasSelection()) {
			QModelIndexList modelIndexes = selectionModel->selectedIndexes();
			QModelIndex index;
			foreach (index, modelIndexes) {
				indices.append(index.row());
			}
		}
    }
    return indices;
	}

// --------------------------------------------------------------------------------------------

void MainView::suspendAllLoads()
{
	mEngineWrapper->suspendAllLoadItems();
}

// ---------------------------------------------------------------------------
	
void MainView::resumeAllLoads()
{
	mEngineWrapper->resumeAllLoadItems();
}

void MainView::clearListSelection()
{
    if (mListView != NULL) {
		mListView->clearSelection();
	}
}

// ---------------------------------------------------------------------------

void MainView::setLoadListData(QStringList& items)
{
	QString loadItem;
	QString iconName;

	if (mListView) {
		mListView->setSelectionMode(HbListView::MultiSelection);
		mListView->setVisible(true); //setHighlightMode(HbItemHighlight::AlwaysVisible);
		if(mListModel->rowCount())
			{
			mListModel->removeRows(0,mListModel->rowCount());
			}
//		mListModel->clear();
	}
	if (items.count() > 0) {
		mIsLoadListEmpty = false;
	}
	else if (items.count() == 0) {
		mIsLoadListEmpty = true;
	}
	for (int i = 0; i < items.count(); i++) {
		QStandardItem *item = new QStandardItem();
		loadItem = items.at(i);
		if (loadItem.startsWith("1")) {
			iconName = ":/paused.svg";
		}
		else {
			iconName = ":/active.svg";
		}
		// remove icon indicator info from string
		// to show in load in load list
		loadItem.remove(0,2); 
		item->setText(loadItem);
		item->setIcon(QIcon(iconName));
		// set new load to correct row in load list
		mListModel->insertRow(i, item);
	}
}	

// ---------------------------------------------------------------------------
