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


#include <hbview.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtextedit.h>

#include <QSignalMapper>
#include <QGraphicsLinearLayout>

#include "notifications.h"
#include "enginewrapper.h"
#include "mainview.h"
#include "hbtoolbar.h"
#include "creator.hrh" // for command ids

// ---------------------------------------------------------------------------

MainView::MainView(HbMainWindow &mainWindow):
    mMainWindow(mainWindow)
{
}

// ---------------------------------------------------------------------------

MainView::~MainView()
{
	delete mSm;
}

// ---------------------------------------------------------------------------

void MainView::init(HbApplication &app)
{
    mEngineWrapper.init();
    this->setTitle("Creator");
    createMenu(app);
    createMemoryInfoText();
}

void MainView::createMenu(HbApplication& app)
{
    HbMenu* menu = this->menu();

    // signal mapper for identifying the different command actions  
	// in executeCommand() handler function:
    mSm = new QSignalMapper(this);
    connect(mSm, SIGNAL(mapped(int)), this, SLOT(executeMenuCommand(int)));
    
    if (menu != NULL) {

		// Run script menu item:
		mActionRunScript = menu->addAction("Run script"); 
		connect(mActionRunScript, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionRunScript, ECmdCreateFromFile);	

		// Calendar sub-menu items:
		HbMenu* calendarSubMenu = new HbMenu("Calendar");
		menu->addMenu(calendarSubMenu);
		mActionCalendarAppointments = calendarSubMenu->addAction("Appointments");
		mActionCalendarEvents = calendarSubMenu->addAction("Events");
		mActionCalendarAnniversaries = calendarSubMenu->addAction("Anniversaries");
		mActionCalendarTodos = calendarSubMenu->addAction("Todos");
		mActionCalendarReminders = calendarSubMenu->addAction("Reminders");
		
		connect(mActionCalendarAppointments, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionCalendarAppointments, ECmdCreateCalendarEntryAppointments);

		connect(mActionCalendarEvents, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionCalendarEvents, ECmdCreateCalendarEntryEvents);

		connect(mActionCalendarAnniversaries, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionCalendarAnniversaries, ECmdCreateCalendarEntryAnniversaries);

		connect(mActionCalendarTodos, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionCalendarTodos, ECmdCreateCalendarEntryToDos);

		connect(mActionCalendarReminders, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionCalendarReminders, ECmdCreateCalendarEntryReminders);		
/*		
		// Browser sub-menu items:
		HbMenu* browserSubMenu = new HbMenu("Browser");
		menu->addMenu(browserSubMenu);
		mActionBrowserBookmark = browserSubMenu->addAction("Bookmarks");
		mActionBrowserBmarkFolder = browserSubMenu->addAction("Bookmark folders");
		mActionBrowserSavedPages = browserSubMenu->addAction("Saved pages");
		mActionBrowserSavedPageFolders = browserSubMenu->addAction("Saved page folders");

		connect(mActionBrowserBookmark, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionBrowserBookmark, ECmdCreateBrowserBookmarkEntries);

		connect(mActionBrowserBmarkFolder, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionBrowserBmarkFolder, ECmdCreateBrowserBookmarkFolderEntries);

		connect(mActionBrowserSavedPages, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionBrowserSavedPages, ECmdCreateBrowserSavedPageEntries);

		connect(mActionBrowserSavedPageFolders, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionBrowserSavedPageFolders, ECmdCreateBrowserSavedPageFolderEntries);	
*/
		// Files sub-menu items:
		HbMenu* filesSubMenu = new HbMenu("Files");
		menu->addMenu(filesSubMenu);
		
		mActionFilesEmptyFolder = filesSubMenu->addAction("Empty folder");
		
		connect(mActionFilesEmptyFolder, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesEmptyFolder, ECmdCreateFileEntryEmptyFolder);
		
		mActionFilesJpeg25 = filesSubMenu->addAction("JPEG 25kB");
		connect(mActionFilesJpeg25, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJpeg25, ECmdCreateFileEntryJPEG_25kB);

		mActionFilesJpeg200 = filesSubMenu->addAction("JPEG 200kB");
		connect(mActionFilesJpeg200, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJpeg200, ECmdCreateFileEntryJPEG_200kB);

		mActionFilesJpeg500 = filesSubMenu->addAction("JPEG 500kB");
		connect(mActionFilesJpeg500, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJpeg500, ECmdCreateFileEntryJPEG_500kB);		

		mActionFilesPng15 = filesSubMenu->addAction("PNG 15kB");
		connect(mActionFilesPng15, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesPng15, ECmdCreateFileEntryPNG_15kB);
		
		mActionFilesGif2 = filesSubMenu->addAction("GIF 2kB");
		connect(mActionFilesGif2, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesGif2, ECmdCreateFileEntryGIF_2kB);
		
		mActionFilesBmp25 = filesSubMenu->addAction("BMP 25kB");
		connect(mActionFilesBmp25, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesBmp25, ECmdCreateFileEntryBMP_25kB);
		
		mActionFilesJp265 = filesSubMenu->addAction("JP2 65kB");
		connect(mActionFilesJp265, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJp265, ECmdCreateFileEntryJP2_65kB);
		
		mActionFilesSvg15 = filesSubMenu->addAction("SVG 15kB");
		connect(mActionFilesSvg15, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesSvg15, ECmdCreateFileEntrySVG_15kB);
		
		mActionFilesTif25 = filesSubMenu->addAction("TIF 25kB");
		connect(mActionFilesTif25, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesTif25, ECmdCreateFileEntryTIF_25kB);
		
		mActionFilesRng1 = filesSubMenu->addAction("RNG 1kB");
		connect(mActionFilesRng1, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesRng1, ECmdCreateFileEntryRNG_1kB);		
		
		mActionFilesMidi10 = filesSubMenu->addAction("MIDI 10kB");
		connect(mActionFilesMidi10, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesMidi10, ECmdCreateFileEntryMIDI_10kB);
		
		mActionFilesWave20 = filesSubMenu->addAction("WAVE 20kB");
		connect(mActionFilesWave20, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesWave20, ECmdCreateFileEntryWAV_20kB);

		mActionFilesAmr20 = filesSubMenu->addAction("AMR 20kB");
		connect(mActionFilesAmr20, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesAmr20, ECmdCreateFileEntryAMR_20kB);		

		mActionFilesExcel15 = filesSubMenu->addAction("Excel 15kB");
		connect(mActionFilesExcel15, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesExcel15, ECmdCreateFileEntryXLS_15kB);

		mActionFilesWord20 = filesSubMenu->addAction("Word 20kB");
		connect(mActionFilesWord20, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesWord20, ECmdCreateFileEntryDOC_20kB);		

		mActionFilesPPoint40 = filesSubMenu->addAction("PowerPoint 40kB");
		connect(mActionFilesPPoint40, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesPPoint40, ECmdCreateFileEntryPPT_40kB);		

		mActionFilesText10 = filesSubMenu->addAction("Text 10kB");
		connect(mActionFilesText10, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesText10, ECmdCreateFileEntryTXT_10kB);		

		mActionFilesText70 = filesSubMenu->addAction("Text 70kB");
		connect(mActionFilesText70, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesText70, ECmdCreateFileEntryTXT_70kB);

		mActionFilesHtml20 = filesSubMenu->addAction("HTML 20kB");
		connect(mActionFilesHtml20, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesHtml20, ECmdCreateFileEntryHTML_20kB);		

		mActionFilesMxmf40 = filesSubMenu->addAction("MXMF 40kB");
		connect(mActionFilesMxmf40, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesMxmf40, ECmdCreateFileEntryMXMF_40kB);	
		
		mActionFilesRam1 = filesSubMenu->addAction("RAM 1kB");
		connect(mActionFilesRam1, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesRam1, ECmdCreateFileEntryRAM_1kB);			

		mActionFilesJad1 = filesSubMenu->addAction("JAD 1kB");
		connect(mActionFilesJad1, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJad1, ECmdCreateFileEntryJAD_1kB);			

		mActionFilesJar10 = filesSubMenu->addAction("JAR 10kB");
		connect(mActionFilesJar10, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesJar10, ECmdCreateFileEntryJAR_10kB);

		mActionFilesVcf1 = filesSubMenu->addAction("VCF 1kB");
		connect(mActionFilesVcf1, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesVcf1, ECmdCreateFileEntryVCF_1kB);
		
		mActionFilesVcs1 = filesSubMenu->addAction("VCS 1kB");
		connect(mActionFilesVcs1, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesVcs1, ECmdCreateFileEntryVCS_1kB);
		
		mActionFilesSisx10 = filesSubMenu->addAction("SISX 10kB");
		connect(mActionFilesSisx10, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesSisx10, ECmdCreateFileEntrySISX_10kB);

		mActionFiles3Gpp70 = filesSubMenu->addAction("3GPP 70kB");
		connect(mActionFiles3Gpp70, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFiles3Gpp70, ECmdCreateFileEntry3GPP_70kB);

		mActionFilesMp3250 = filesSubMenu->addAction("MP3 250kB");
		connect(mActionFilesMp3250, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesMp3250, ECmdCreateFileEntryMP3_250kB);	

		mActionFilesAAC100 = filesSubMenu->addAction("AAC 100kB");
		connect(mActionFilesAAC100, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesAAC100, ECmdCreateFileEntryAAC_100kB);	

		mActionFilesSwf15 = filesSubMenu->addAction("SWF 15kB");
		connect(mActionFilesSwf15, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesSwf15, ECmdCreateFileEntrySWF_15kB);	

		mActionFilesMp4200 = filesSubMenu->addAction("MP4 200kB");
		connect(mActionFilesMp4200, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesMp4200, ECmdCreateFileEntryMP4_200kB);	

		mActionFilesRm95 = filesSubMenu->addAction("RM 95kB");
		connect(mActionFilesRm95, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesRm95, ECmdCreateFileEntryRM_95kB);	

		mActionFilesWma50 = filesSubMenu->addAction("WMA 50kB");
		connect(mActionFilesWma50, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesWma50, ECmdCreateFileEntryWMA_50kB);			

		mActionFilesWmv200 = filesSubMenu->addAction("WMV 200kB");
		connect(mActionFilesWmv200, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionFilesWmv200, ECmdCreateFileEntryWMV_200kB);	
		
		// Logs sub-menu items:	
		HbMenu* logsSubMenu = new HbMenu("Logs");
		menu->addMenu(logsSubMenu);
        
		mActionLogsMissedCalls = logsSubMenu->addAction("Missed calls");
		connect(mActionLogsMissedCalls, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionLogsMissedCalls, ECmdCreateLogEntryMissedCalls);

		mActionLogsReceivedCalls = logsSubMenu->addAction("Received calls");
		connect(mActionLogsReceivedCalls, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionLogsReceivedCalls, ECmdCreateLogEntryReceivedCalls);

		mActionLogsDialledNumbers = logsSubMenu->addAction("Dialled numbers");
		connect(mActionLogsDialledNumbers, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionLogsDialledNumbers, ECmdCreateLogEntryDialledNumbers);	

		// Messaging sub-menu items:	
		HbMenu* messageSubMenu = new HbMenu("Messaging");
		menu->addMenu(messageSubMenu);
        
		mActionMessageMailboxes = messageSubMenu->addAction("Mailboxes");
		connect(mActionMessageMailboxes, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMessageMailboxes, ECmdCreateMessagingEntryMailboxes);

		mActionMessages = messageSubMenu->addAction("Messages");
		connect(mActionMessages, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMessages, ECmdCreateMessagingEntryMessages);		
		
		// Misc sub-menu items:	
		HbMenu* miscSubMenu = new HbMenu("Misc");
		menu->addMenu(miscSubMenu);
        
		//mActionMiscConnection = miscSubMenu->addAction("Connection methods");
		//mActionMiscImps = miscSubMenu->addAction("IMPS servers");
		mActionMiscNotes = miscSubMenu->addAction("Notes");
		//mActionMiscLandmarks = miscSubMenu->addAction("Landmarks");
/*    
		connect(mActionMiscConnection, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMiscConnection, ECmdCreateMiscEntryAccessPoints);

		connect(mActionMiscImps, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMiscImps, ECmdCreateMiscEntryIMPSServers);
*/
		connect(mActionMiscNotes, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMiscNotes, ECmdCreateMiscEntryNotes);
/*
		connect(mActionMiscLandmarks, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionMiscLandmarks, ECmdCreateMiscEntryLandmarks);
*/
		// Phonebook sub-menu items:	
		HbMenu* phonebookSubMenu = new HbMenu("Phonebook");
		menu->addMenu(phonebookSubMenu);
        
		mActionPhonebookContacts = phonebookSubMenu->addAction("Contacts");
		connect(mActionPhonebookContacts, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionPhonebookContacts, ECmdCreatePhoneBookEntryContacts);

		mActionPhonebookGroups = phonebookSubMenu->addAction("Groups");
		connect(mActionPhonebookGroups, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionPhonebookGroups, ECmdCreatePhoneBookEntryGroups);	
		
		// Delete sub-menu items: 
		HbMenu* deleteSubMenu = new HbMenu("Delete");
		menu->addMenu(deleteSubMenu);

		HbMenu* deleteAllSubMenu = new HbMenu("All items");
		deleteSubMenu->addMenu(deleteAllSubMenu);
        
		mActionDeleteAllItems = deleteAllSubMenu->addAction("All");
		connect(mActionDeleteAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteAllItems, ECmdDeleteAllEntries);

		mActionDeleteCreatorItems = deleteAllSubMenu->addAction("By Creator");
		connect(mActionDeleteCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteCreatorItems, ECmdDeleteAllCreatorEntries);
		
		HbMenu* deleteCalendarSubMenu = new HbMenu("Calendar entries");
		deleteSubMenu->addMenu(deleteCalendarSubMenu);
        
		mActionDeleteCalAllItems = deleteCalendarSubMenu->addAction("All");
		connect(mActionDeleteCalAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteCalAllItems, ECmdDeleteCalendarEntries);

		mActionDeleteCalCreatorItems = deleteCalendarSubMenu->addAction("By Creator");
		connect(mActionDeleteCalCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteCalCreatorItems, ECmdDeleteCreatorCalendarEntries);
/*
		HbMenu* deleteBookmarksSubMenu = new HbMenu("Br bookmarks");
		deleteSubMenu->addMenu(deleteBookmarksSubMenu);
        
		mActionDeleteBookmarkAllItems = deleteBookmarksSubMenu->addAction("All");
		connect(mActionDeleteBookmarkAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteBookmarkAllItems, ECmdDeleteBrowserBookmarks);

		mActionDeleteBookmarkCreatorItems = deleteBookmarksSubMenu->addAction("By Creator");
		connect(mActionDeleteBookmarkCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteBookmarkCreatorItems, ECmdDeleteCreatorBrowserBookmarks);

		HbMenu* deleteBookmarkFoldersSubMenu = new HbMenu("Br bookmark folders");
		deleteSubMenu->addMenu(deleteBookmarkFoldersSubMenu);
        
		mActionDeleteBookmarkFolderAllItems = deleteBookmarkFoldersSubMenu->addAction("All");
		connect(mActionDeleteBookmarkFolderAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteBookmarkFolderAllItems, ECmdDeleteBrowserBookmarkFolders);

		mActionDeleteBookmarkFolderCreatorItems = deleteBookmarkFoldersSubMenu->addAction("By Creator");
		connect(mActionDeleteBookmarkFolderCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteBookmarkFolderCreatorItems, ECmdDeleteCreatorBrowserBookmarkFolders);		

		HbMenu* deleteSavedPagesSubMenu = new HbMenu("Br saved pages");
		deleteSubMenu->addMenu(deleteSavedPagesSubMenu);
        
		mActionDeleteSavedPagesAllItems = deleteSavedPagesSubMenu->addAction("All");
		connect(mActionDeleteSavedPagesAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteSavedPagesAllItems, ECmdDeleteBrowserSavedPages);

		mActionDeleteSavedPagesCreatorItems = deleteSavedPagesSubMenu->addAction("By Creator");
		connect(mActionDeleteSavedPagesCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteSavedPagesCreatorItems, ECmdDeleteCreatorBrowserSavedPages);	

		HbMenu* deleteSavedPagesFolderSubMenu = new HbMenu("Br saved page folders");
		deleteSubMenu->addMenu(deleteSavedPagesFolderSubMenu);
        
		mActionDeleteSavedPagesFolderAllItems = deleteSavedPagesFolderSubMenu->addAction("All");
		connect(mActionDeleteSavedPagesFolderAllItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteSavedPagesFolderAllItems, ECmdDeleteBrowserSavedPageFolders);

		mActionDeleteSavedPagesFolderCreatorItems = deleteSavedPagesFolderSubMenu->addAction("By Creator");
		connect(mActionDeleteSavedPagesFolderCreatorItems, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteSavedPagesFolderCreatorItems, ECmdDeleteCreatorBrowserSavedPageFolders);	
*/
		HbMenu* deleteFilesSubMenu = new HbMenu("Files");
		deleteSubMenu->addMenu(deleteFilesSubMenu);

		mActionDeleteFiles = deleteFilesSubMenu->addAction("By Creator");
		connect(mActionDeleteFiles, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteFiles, ECmdDeleteCreatorFiles);			
		
		HbMenu* deleteLogsSubMenu = new HbMenu("Logs");
		deleteSubMenu->addMenu(deleteLogsSubMenu);

		mActionDeleteLogsAll = deleteLogsSubMenu->addAction("All");
		connect(mActionDeleteLogsAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteLogsAll, ECmdDeleteLogs);
		
		mActionDeleteLogsByCreator = deleteLogsSubMenu->addAction("By Creator");
		connect(mActionDeleteLogsByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteLogsByCreator, ECmdDeleteCreatorLogs);

		HbMenu* deleteMessagesSubMenu = new HbMenu("Messages");
		deleteSubMenu->addMenu(deleteMessagesSubMenu);

		mActionDeleteMessagesAll = deleteMessagesSubMenu->addAction("All");
		connect(mActionDeleteMessagesAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteMessagesAll, ECmdDeleteMessages);
		
		mActionDeleteMessagesByCreator = deleteMessagesSubMenu->addAction("By Creator");
		connect(mActionDeleteMessagesByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteMessagesByCreator, ECmdDeleteCreatorMessages);
/*
		HbMenu* deleteConnMethsSubMenu = new HbMenu("Connection methods");
		deleteSubMenu->addMenu(deleteConnMethsSubMenu);

		mActionDeleteConnMethsAll = deleteConnMethsSubMenu->addAction("All");
		connect(mActionDeleteConnMethsAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteConnMethsAll, ECmdDeleteMessages);
		
		mActionDeleteConnMethsByCreator = deleteConnMethsSubMenu->addAction("By Creator");
		connect(mActionDeleteConnMethsByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteConnMethsByCreator, ECmdDeleteCreatorMessages);

		HbMenu* deleteImpsSubMenu = new HbMenu("IMPS servers");
		deleteSubMenu->addMenu(deleteImpsSubMenu);

		mActionDeleteImpsAll = deleteImpsSubMenu->addAction("All");
		connect(mActionDeleteImpsAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteImpsAll, ECmdDeleteIMPSs);
		
		mActionDeleteImpsByCreator = deleteImpsSubMenu->addAction("By Creator");
		connect(mActionDeleteImpsByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteImpsByCreator, ECmdDeleteCreatorIMPSs);		
*/		
		HbMenu* deleteNotesSubMenu = new HbMenu("Notes");
		deleteSubMenu->addMenu(deleteNotesSubMenu);

		mActionDeleteNotes = deleteNotesSubMenu->addAction("All");
		connect(mActionDeleteNotes, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteNotes, ECmdDeleteNotes);
/*
		HbMenu* deleteLandmarksSubMenu = new HbMenu("Landmarks");
		deleteSubMenu->addMenu(deleteLandmarksSubMenu);

		mActionDeleteLandmarksAll = deleteLandmarksSubMenu->addAction("All");
		connect(mActionDeleteLandmarksAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteLandmarksAll, ECmdDeleteLandmarks);
		
		mActionDeleteLandmarksByCreator = deleteLandmarksSubMenu->addAction("By Creator");
		connect(mActionDeleteLandmarksByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteLandmarksByCreator, ECmdDeleteCreatorLandmarks);
*/
		HbMenu* deleteContactsSubMenu = new HbMenu("Contacts");
		deleteSubMenu->addMenu(deleteContactsSubMenu);

		mActionDeleteContactsAll = deleteContactsSubMenu->addAction("All");
		connect(mActionDeleteContactsAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteContactsAll, ECmdDeleteContacts);
		
		mActionDeleteContactsByCreator = deleteContactsSubMenu->addAction("By Creator");
		connect(mActionDeleteContactsByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteContactsByCreator, ECmdDeleteCreatorContacts);

		HbMenu* deleteContactGroupsSubMenu = new HbMenu("Contact groups");
		deleteSubMenu->addMenu(deleteContactGroupsSubMenu);

		mActionDeleteContactGroupsAll = deleteContactGroupsSubMenu->addAction("All");
		connect(mActionDeleteContactGroupsAll, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteContactGroupsAll, ECmdDeleteContactGroups);
		
		mActionDeleteContactGroupsByCreator = deleteContactGroupsSubMenu->addAction("By Creator");
		connect(mActionDeleteContactGroupsByCreator, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionDeleteContactGroupsByCreator, ECmdDeleteCreatorContactGroups);		
/*		
		// select random data file:
		mActionSelectRandomFile = menu->addAction("Select random data file"); 
		connect(mActionSelectRandomFile, SIGNAL( triggered() ), mSm, SLOT(map()));
		mSm->setMapping(mActionSelectRandomFile, ECmdSelectRandomDataFile);	
*/		
		// About & exit menu items:
		mActionAbout = menu->addAction("About Creator");
        connect(mActionAbout, SIGNAL( triggered() ), this, SLOT( showAboutPopup() ) );
    
        mActionExit = menu->addAction("Exit");
        connect(mActionExit, SIGNAL(triggered()), &app, SLOT( quit() ) );

        this->setMenu(menu);
    }
}

// ---------------------------------------------------------------------------

void MainView::createMemoryInfoText()
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);

    // get memory details from engine;
    mMemoryDetails = mEngineWrapper.GetMemoryDetails();
	
    QList<MemoryDetails> memDet = mEngineWrapper.GetMemoryDetailsList();
    
    if (layout != NULL) 
    	{
		HbLabel* edit = new HbLabel(this);
        //HbTextEdit* edit = new HbTextEdit(this);
        if(edit != NULL) 
        	{
        QString text = QString("<BR><BR>Free RAM Memory:<BR> %1 of %2 kB").arg(mMemoryDetails.mRamFree).arg(mMemoryDetails.mRamSize);	
        //QString text = QString("\n\nFree RAM Memory:\n %1 of %2 kB").arg(mMemoryDetails.mRamFree).arg(mMemoryDetails.mRamSize);
			for(int i=0 ; i < memDet.count() ; i++ )
				{
			text += QString("<BR><BR>Free %1: Memory:<BR> %2 of %3 kB").arg(memDet[i].mDriveLetter).arg(memDet[i].mFree).arg(memDet[i].mSize);//text += QString("\n\nFree %1: Memory:\n %2 of %3 kB").arg(memDet[i].mDriveLetter).arg(memDet[i].mFree).arg(memDet[i].mSize);
				}
			edit->setTextWrapping(Hb::TextWordWrap);
			edit->setHtml(text);
			//edit->setPlainText(text);
                	
            //edit->setReadOnly(true);
            //edit->setCursorHidden(true);
            edit->setAttribute(Qt::WA_InputMethodEnabled, false);
            layout->addItem(edit);
            layout->setContentsMargins(5, 5, 5, 5);
            this->setLayout(layout);
        	}
        
    }
    
}


// ---------------------------------------------------------------------------

void MainView::showAboutPopup()
{
	Notifications::about();

}

// ---------------------------------------------------------------------------

void MainView::executeMenuCommand(int cmd)
{
    switch (cmd) {
		case ECmdCreateFromFile:
		case ECmdCreateCalendarEntryAppointments:
		case ECmdCreateCalendarEntryEvents:
		case ECmdCreateCalendarEntryAnniversaries:
		case ECmdCreateCalendarEntryToDos:
		case ECmdCreateCalendarEntryReminders:
		case ECmdCreateBrowserBookmarkEntries:
        case ECmdCreateBrowserBookmarkFolderEntries:
        case ECmdCreateBrowserSavedPageEntries:
        case ECmdCreateBrowserSavedPageFolderEntries: 
		case ECmdCreateFileEntryEmptyFolder:
	    case ECmdCreateFileEntry3GPP_70kB:
	    case ECmdCreateFileEntryAAC_100kB:
	    case ECmdCreateFileEntryAMR_20kB:
	    case ECmdCreateFileEntryBMP_25kB:
	    case ECmdCreateFileEntryDeck_1kB:
	    case ECmdCreateFileEntryDOC_20kB:
	    case ECmdCreateFileEntryGIF_2kB:
	    case ECmdCreateFileEntryHTML_20kB:
	    case ECmdCreateFileEntryJAD_1kB:
	    case ECmdCreateFileEntryJAR_10kB:
	    case ECmdCreateFileEntryJP2_65kB:
	    case ECmdCreateFileEntryJPEG_200kB:
	    case ECmdCreateFileEntryJPEG_25kB:
	    case ECmdCreateFileEntryJPEG_500kB:
	    case ECmdCreateFileEntryMIDI_10kB:
	    case ECmdCreateFileEntryMP3_250kB:
	    case ECmdCreateFileEntryMP4_200kB:
	    case ECmdCreateFileEntryMXMF_40kB:
	    case ECmdCreateFileEntryPNG_15kB:
	    case ECmdCreateFileEntryPPT_40kB:
	    case ECmdCreateFileEntryRAM_1kB:
	    case ECmdCreateFileEntryRM_95kB:
	    case ECmdCreateFileEntryRNG_1kB:
	    case ECmdCreateFileEntrySVG_15kB:
	    case ECmdCreateFileEntrySWF_15kB:
	    case ECmdCreateFileEntryTIF_25kB:
	    case ECmdCreateFileEntryTXT_10kB:
	    case ECmdCreateFileEntryTXT_70kB:
	    case ECmdCreateFileEntryVCF_1kB:
	    case ECmdCreateFileEntryVCS_1kB:
	    case ECmdCreateFileEntryWAV_20kB:
	    case ECmdCreateFileEntryXLS_15kB:
	    case ECmdCreateFileEntrySISX_10kB:
	    case ECmdCreateFileEntryWMA_50kB:
	    case ECmdCreateFileEntryWMV_200kB:
	    case ECmdCreateLogEntryMissedCalls:
	    case ECmdCreateLogEntryReceivedCalls:
	    case ECmdCreateLogEntryDialledNumbers:
	    case ECmdCreateMessagingEntryMailboxes:
	    case ECmdCreateMessagingEntryMessages:		
        case ECmdCreateMiscEntryAccessPoints:
		case ECmdCreateMiscEntryIMPSServers:
		case ECmdCreateMiscEntryNotes:
		case ECmdCreateMiscEntryLandmarks:		
		case ECmdCreatePhoneBookEntryContacts:
	    case ECmdCreatePhoneBookEntryGroups:
		case ECmdDeleteAllEntries:
	    case ECmdDeleteAllCreatorEntries:
	    case ECmdDeleteContacts:
        case ECmdDeleteCreatorContacts:
        case ECmdDeleteContactGroups:
        case ECmdDeleteCreatorContactGroups:
        case ECmdDeleteCalendarEntries:
        case ECmdDeleteCreatorCalendarEntries:
        case ECmdDeleteBrowserBookmarks:
        case ECmdDeleteCreatorBrowserBookmarks:
        case ECmdDeleteBrowserBookmarkFolders:
        case ECmdDeleteCreatorBrowserBookmarkFolders:
        case ECmdDeleteBrowserSavedPages:
        case ECmdDeleteCreatorBrowserSavedPages:
        case ECmdDeleteBrowserSavedPageFolders:
        case ECmdDeleteCreatorBrowserSavedPageFolders:
        case ECmdDeleteCreatorFiles:
        case ECmdDeleteLogs:
        case ECmdDeleteCreatorLogs:
        case ECmdDeleteMessages:
        case ECmdDeleteCreatorMessages:
        case ECmdDeleteMailboxes:
        case ECmdDeleteCreatorMailboxes:
        case ECmdDeleteIAPs:
        case ECmdDeleteCreatorIAPs:
        case ECmdDeleteIMPSs:
        case ECmdDeleteCreatorIMPSs:
        case ECmdDeleteNotes:
        case ECmdDeleteLandmarks:
        case ECmdDeleteCreatorLandmarks:
		case ECmdSelectRandomDataFile: {
			mEngineWrapper.ExecuteOptionsMenuCommand(cmd);
            break;
        }
        default: {
            Notifications::error(QString("Unsupported command: %1").arg(cmd));
            break;
        }
    }
}

void MainView::closeApp()
{
	qApp->quit();
}

// ---------------------------------------------------------------------------
