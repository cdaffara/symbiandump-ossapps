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


#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <hbview.h>

class EngineWrapper;
class HbMainWindow;
class HbApplication;
class HbAction;
class HbLabel;
class HbProgressNote;
class HbListView;
class HbAbstractViewItem;

class QStandardItemModel;
class QSignalMapper;
class QPointF;

class MainView : public HbView
{
    Q_OBJECT
    
public:

    MainView(HbMainWindow &mainWindow);
	~MainView();
	void init(HbApplication &app);
	int currentItemIndex();
	void setCurrentItemIndex(int index);
	void clearListSelection();
	void setLoadListData(QStringList& items);
	QList<int> listSelectionIndexes();
	
private:
    void createMenu(HbApplication &app);
	void loadListIinit();
	bool showItemActionPopup(int &index);
	void loadListDelete();
	bool stopLoad(bool stopAll /*, int selectedLoad = 0*/);
   
private slots:
    void handleLoadListEvent(HbAbstractViewItem */*listViewItem*/, const QPointF &/*coords*/);
    void ItemActionPopupClosed(HbAction* action );
	void showAboutPopup();
	void stopAllLoads();
	void updateMenu();
	void launchPerfMon();
	void suspendAllLoads();
	void resumeAllLoads();
	
	/**
	* calls engine wrapper to complete service request, specified by cmd.
	*/
	void executeMenuCommand(int cmd);
    
private:
    HbMainWindow &mMainWindow;
    EngineWrapper *mEngineWrapper;
	HbListView *mListView;
	QStandardItemModel *mListModel;
	QSignalMapper *mSm;
	bool mIsLoadListEmpty;
	
	HbAction *mActionAbout;
	HbMenu   *mLoadSubMenu1;
	HbMenu   *mLoadSubMenu2;
	HbAction *mActionExit;
	HbAction *mStopAll;
	HbAction *mSuspendAll;	
	HbAction *mResumeAll;	
	HbAction *mActionPerfMon;
	HbAction *mCpuLoadAction;
	HbAction *mMemoryEatLoadAction;
	HbAction *mPhoneCallLoadAction;
	HbAction *mMessagesLoadAction;
	HbAction *mNetConnLoadAction;
	HbAction *mAppsLoadAction;
	HbAction *mKeyPressLoadAction;
	HbAction *mPhotoCapturesLoadAction;
	HbAction *mBtActionsLoadAction;
	HbAction *mPointerEventsLoadAction;
	
};

#endif // MAINVIEW_H
