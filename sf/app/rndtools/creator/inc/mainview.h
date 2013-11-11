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
#include <enginewrapper.h>

class HbMainWindow;
class HbApplication;
class HbAction;
class QSignalMapper;

class MemoryDetails
{
public:
	QString 		mFree;
	QString			mSize;
	QString			mDriveLetter;
	
    QString         mRamFree;
	bool			mENotAvailable;
	QString         mRamSize;
};

/**
 * Class that implements the main view of Creator
 */
class MainView : public HbView
{
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    MainView(HbMainWindow &mainWindow);
    
    /**
     * Destructor
     */
	~MainView();

	/**
     * Initializes Engine Wrapper
     * @param app application class of Creator
     */
    void init(HbApplication &app);
	
	/**
	* Call application exit from engine.
	*/
	static void closeApp();

private:
    
    /**
     * Creates menu
     */
    void createMenu(HbApplication &app);
    
    /**
     * Creates memory info text to UI
     */
    void createMemoryInfoText();
    
private slots:

    
    /**
     * Shows about pop-up
     */
    void showAboutPopup();
	/**
	* calls engine wrapper to complete service request, specified with commandId.
	*/
	void executeMenuCommand(int cmd);
  
private:
    
    /* Main window of Creator */
    HbMainWindow &mMainWindow;
    
   
    /* Engine Wrapper */
    EngineWrapper mEngineWrapper;
    
    /* Actions that are used in toolbars and menus */
	HbAction* mActionRunScript;
	HbAction* mActionCalendarAppointments;
	HbAction* mActionCalendarEvents;
	HbAction* mActionCalendarAnniversaries;
	HbAction* mActionCalendarTodos;
	HbAction* mActionCalendarReminders;	
	HbAction* mActionBrowserBookmark;
	HbAction* mActionBrowserBmarkFolder;
	HbAction* mActionLogsMissedCalls;
	HbAction* mActionLogsReceivedCalls;
	HbAction* mActionLogsDialledNumbers;
	HbAction* mActionMessageMailboxes;
	HbAction* mActionMessages;
	HbAction* mActionFilesJpeg25;
	HbAction* mActionFilesJpeg200;
	HbAction* mActionFilesJpeg500;
	HbAction* mActionFilesPng15;
	HbAction* mActionFilesGif2;
	HbAction* mActionFilesBmp25;
	HbAction* mActionFilesJp265;
	HbAction* mActionFilesSvg15;
	HbAction* mActionFilesTif25;
	HbAction* mActionFilesRng1;
	HbAction* mActionFilesMidi10;
	HbAction* mActionFilesWave20;
	HbAction* mActionFilesAmr20;
	HbAction* mActionFilesExcel15;
	HbAction* mActionFilesWord20;
	HbAction* mActionFilesPPoint40;
	HbAction* mActionFilesText10;
	HbAction* mActionFilesText70;
	HbAction* mActionFilesHtml20;
	HbAction* mActionFilesMxmf40;
	HbAction* mActionFilesRam1;
	HbAction* mActionFilesJad1;
	HbAction* mActionFilesJar10;
	HbAction* mActionFilesVcf1;
	HbAction* mActionFilesVcs1;
	HbAction* mActionFilesSisx10;
	HbAction* mActionFiles3Gpp70;
	HbAction* mActionFilesMp3250;
	HbAction* mActionFilesAAC100;
	HbAction* mActionFilesSwf15;
	HbAction* mActionFilesMp4200;
	HbAction* mActionFilesRm95;
	HbAction* mActionFilesWma50;
	HbAction* mActionFilesWmv200;
	HbAction* mActionPhonebookContacts;
	HbAction* mActionPhonebookGroups;
	HbAction* mActionBrowserSavedPages;
	HbAction* mActionBrowserSavedPageFolders;
	HbAction* mActionFilesEmptyFolder;
	HbAction* mActionMiscConnection;
	HbAction* mActionMiscImps;
	HbAction* mActionMiscNotes;
	HbAction* mActionMiscLandmarks;
    HbAction* mActionAbout;
    HbAction* mActionExit;
	HbAction* mActionDeleteAllItems;
	HbAction* mActionDeleteCreatorItems;
	HbAction* mActionSelectRandomFile;
	HbAction* mActionDeleteCalAllItems;
	HbAction* mActionDeleteCalCreatorItems;
	HbAction* mActionDeleteBookmarkAllItems;
	HbAction* mActionDeleteBookmarkCreatorItems;
	HbAction* mActionDeleteBookmarkFolderAllItems;
	HbAction* mActionDeleteBookmarkFolderCreatorItems;
	HbAction* mActionDeleteSavedPagesAllItems;
	HbAction* mActionDeleteSavedPagesCreatorItems;
	HbAction* mActionDeleteSavedPagesFolderAllItems;
	HbAction* mActionDeleteSavedPagesFolderCreatorItems;
	HbAction* mActionDeleteFiles;
	HbAction* mActionDeleteLogsAll;
	HbAction* mActionDeleteLogsByCreator;
	HbAction* mActionDeleteMessagesAll;
	HbAction* mActionDeleteMessagesByCreator;
	HbAction* mActionDeleteConnMethsAll;
	HbAction* mActionDeleteConnMethsByCreator;
	HbAction* mActionDeleteImpsAll;
	HbAction* mActionDeleteImpsByCreator;
	HbAction* mActionDeleteNotes;
	HbAction* mActionDeleteLandmarksAll;
	HbAction* mActionDeleteLandmarksByCreator;
	HbAction* mActionDeleteContactsAll;
	HbAction* mActionDeleteContactsByCreator;
	HbAction* mActionDeleteContactGroupsAll;
	HbAction* mActionDeleteContactGroupsByCreator;
	
	
	MemoryDetails mMemoryDetails;
	QList<MemoryDetails> mMemoryDetailsList;
    // signal mapper
    QSignalMapper *mSm;    
};

#endif // MAINVIEW_H
