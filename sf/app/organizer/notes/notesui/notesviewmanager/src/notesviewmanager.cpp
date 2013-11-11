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
* Description: Definition file for class NotesViewManager.
*
*/

// System includes
#include <qglobal.h>
#include <QtGui>
#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include <HbListView>
#include <HbMessageBox>
#include <HbAction>
#include <hbapplication> // hbapplication
#include <AfActivation.h>
#include <AfActivityStorage.h>

// User includes
#include "notesviewmanager.h"
#include "notesmainview.h"
#include "notescollectionview.h"
#include "notestodoview.h"
#include "notesfavoriteview.h"
#include "notesnoteview.h"
#include "notesdocloader.h"
#include "notescommon.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include <agendautil.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesviewmanagerTraces.h"
#endif


/*!
	\class NotesViewManager

	This is the heart of the notes application. This is responsible for
	managing the notes views and owning the agenda interface object.
	This also is the source for the notes list model.
 */
/*!
	Constructor.

	\param controller A reference to the object of NotesAppController.
 */
NotesViewManager::NotesViewManager(
		NotesAppControllerIf &controllerIf, QObject *parent)
:QObject(parent),
 mAppControllerIf(controllerIf),
 mMainView(0),
 mCollectionView(0),
 mTodoView(0),
 mFavoriteView(0),
 mNoteView(0),
 instanceViewCreated(false)
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_NOTESVIEWMANAGER_ENTRY );
	HbMainWindow *window = hbInstance->allMainWindows().first();

	mAgendaUtil = mAppControllerIf.agendaUtil();

	// Check the Application Startup reason from Activity Manager
	 AfActivation *activation = new AfActivation();
	
	// Check if application is started from an application
	if (Af::ActivationReasonActivity == activation->reason()) {
		// Application is started from an activity
		// extract activity data
	    AfActivityStorage *activitystorage = new AfActivityStorage();
        QVariant data = activitystorage->activityData(activation->name());
        delete activitystorage;
		// Restore state from activity data
		QByteArray serializedModel = data.toByteArray();
		QDataStream stream(&serializedModel, QIODevice::ReadOnly);
		int viewId;
		stream >> viewId; // read stream into an int

		// Check if viewId is main view.
		if (NotesNamespace::NotesMainViewId == viewId) {
			// Load MainView
			loadNotesMainView();
		} else if (NotesNamespace::NotesCollectionViewId == viewId) {
			// Check if the viewId is collective view
			//no implementation yet, UI Specs not available
		} 
	} else {
			// application started by either service framework or normally
			// Load the main view at the start up.
			loadNotesMainView();
		}
	//delet the activity instance
	delete activation;
	connect(
			mAgendaUtil, SIGNAL(instanceViewCreationCompleted(int)),
			this,SLOT(handleInstanceViewCreationCompleted(int)));

	// Delay loading of other views till main view is loaded.
	connect(
			window, SIGNAL(viewReady()),
			this, SLOT(loadOtherViews()));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_NOTESVIEWMANAGER_EXIT );
}

/*!
	Destructor.
 */
NotesViewManager::~NotesViewManager()
{
	OstTraceFunctionEntry0( DUP1_NOTESVIEWMANAGER_NOTESVIEWMANAGER_ENTRY );
	// No implementation yet
	OstTraceFunctionExit0( DUP1_NOTESVIEWMANAGER_NOTESVIEWMANAGER_EXIT );
}

/*!
	Switches to view specified by viewId.

	\param viewId View Id.
 */
void NotesViewManager::switchToView(NotesNamespace::NotesViewIds viewId)
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_SWITCHTOVIEW_ENTRY );
	HbMainWindow *window = hbInstance->allMainWindows().first();

	switch (viewId) {
		case NotesNamespace::NotesMainViewId:
		{
			window->removeView(window->currentView());
			window->addView(mMainView);
			window->setCurrentView(mMainView);
			mMainView->captureScreenShot(false);
			break;
		}
		case NotesNamespace::NotesCollectionViewId:
		{
			if (mMainView) {
				if (mMainView == window->currentView()) {
					mMainView->captureScreenShot(true);
				}
			}
			window->removeView(window->currentView());
			window->addView(mCollectionView);
			window->setCurrentView(mCollectionView);
			break;
		}
		case NotesNamespace::NotesTodoViewId:
		{
			if (mMainView) {
				if (mMainView == window->currentView()) {
					mMainView->captureScreenShot(true);
				}
			}
			window->removeView(window->currentView());
			window->addView(mTodoView);
			window->setCurrentView(mTodoView);
			break;
		}
		case NotesNamespace::NotesFavoritesViewId:
		{
			if (mMainView) {
				if (mMainView == window->currentView()) {
					mMainView->captureScreenShot(true);
				}
			}
			window->removeView(window->currentView());
			window->addView(mFavoriteView);
			window->setCurrentView(mFavoriteView);
			break;
		}
		case NotesNamespace::NotesNoteViewId:
		{
			if (mMainView) {
				if (mMainView == window->currentView()) {
					mMainView->captureScreenShot(true);
				}
			}
			window->removeView(window->currentView());
			window->addView(mNoteView);
			window->setCurrentView(mNoteView);
			break;
		}
		default:
			break;
	}
	OstTraceFunctionExit0( NOTESVIEWMANAGER_SWITCHTOVIEW_EXIT );
}

/*!
	Loads the notes main view.
 */
void NotesViewManager::loadNotesMainView()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADNOTESMAINVIEW_ENTRY );
	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_MAIN_VIEW_XML, &loadSuccess);
	
	// Find the main view.
	mMainView = static_cast<NotesMainView *> (
			docLoader->findWidget(NOTES_MAIN_VIEW));
	mMainView->setParent(this);
	
	// Setup the view.
	mMainView->setupView(mAppControllerIf, docLoader);
	// Connect to main view signal for entry deletion.
	connect (
			mMainView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));

	// Set the main view to the window
	hbInstance->allMainWindows().first()->addView(mMainView);
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADNOTESMAINVIEW_EXIT );
}

/*!
	Loads the notes collection view.
 */
void NotesViewManager::loadNotesCollectionView()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADNOTESCOLLECTIONVIEW_ENTRY );
	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_COLLECTION_VIEW_XML, &loadSuccess);

	// Find the collection view.
	mCollectionView = static_cast<NotesCollectionView *> (
			docLoader->findWidget(NOTES_COLLECTION_VIEW));
	mCollectionView->setParent(this);
	
	// Setup the view.
	mCollectionView->setupView(mAppControllerIf, docLoader);
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADNOTESCOLLECTIONVIEW_EXIT );
}

/*!
	Loads the to-do view.
 */
void NotesViewManager::loadTodoView()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADTODOVIEW_ENTRY );

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_TODO_VIEW_XML, &loadSuccess);

	// Find the to-do view.
	mTodoView = static_cast<NotesTodoView *> (
			docLoader->findWidget(NOTES_TODO_VIEW));
	mTodoView->setParent(this);
	
	// Setup the view.
	mTodoView->setupView(mAppControllerIf, docLoader);
	// Connect to to-do view signal for entry deletion.
	connect (
			mTodoView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADTODOVIEW_EXIT );
}

/*!
	Loads the favorites view.
 */
void NotesViewManager::loadFavoritesView()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADFAVORITESVIEW_ENTRY );
	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_FAVORITES_VIEW_XML, &loadSuccess);

	// Find the favorites view.
	mFavoriteView = static_cast<NotesFavoriteView *> (
			docLoader->findWidget(NOTES_FAVORITES_VIEW));
	mFavoriteView->setParent(this);
	// Setup the view.
	mFavoriteView->setupView(mAppControllerIf, docLoader);

	// Connect to favourite view signal for entry deletion.
	connect (
			mFavoriteView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADFAVORITESVIEW_EXIT );
}

/*!
	Loads the recent notes view.
 */
void NotesViewManager::loadNoteView()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADNOTEVIEW_ENTRY );
	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_NOTE_VIEW_XML, &loadSuccess);

	// Find the note view.
	mNoteView = static_cast<NotesNoteView *> (
			docLoader->findWidget(NOTES_NOTE_VIEW));
	mNoteView->setParent(this);
	
	// Setup the view.
	mNoteView->setupView(mAppControllerIf, docLoader);

	connect(
			mNoteView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADNOTEVIEW_EXIT );
}

/*!
	Update all the other views.
 */
void NotesViewManager::updateOtherViews()
{
	if (mCollectionView) {
		// Populate collections view.
		mCollectionView->populateListView();
	}

	if (mTodoView) {
		// Update the title for to-do view.
		mTodoView->updateTitle();
	}
	
	if (mNoteView) {
		// Update the plain notes view.
		mNoteView->updateNoteView();
	}
	
	if (mFavoriteView) {
		// Update the favorites view.
		mFavoriteView->updateFavoriteView();
	}
	
}

/*!
	Loads other views from the docml file.
 */
void NotesViewManager::loadOtherViews()
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_LOADOTHERVIEWS_ENTRY );
	mMainView->setupAfterViewReady();

	// Load the collection view.
	loadNotesCollectionView();
	// Load the to-do view.
	loadTodoView();
	// Load the favorites view.
	loadFavoritesView();
	// Load the recent notes view.
	loadNoteView();

	if (instanceViewCreated) {
		// update other views.
		updateOtherViews();
	}
	// Disconnect the signal viewReady as all the views are loaded.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	disconnect(
			window, SIGNAL(viewReady()),
			this, SLOT(loadOtherViews()));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_LOADOTHERVIEWS_EXIT );
}

/*!
	 Delete the entry.
 */
void NotesViewManager::deleteEntryFromView(ulong entryId)
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_DELETEENTRYFROMVIEW_ENTRY );
	mEntryId = entryId;
	HbMessageBox *confirmationQuery = new HbMessageBox(
			HbMessageBox::MessageTypeQuestion);
	confirmationQuery->setDismissPolicy(HbDialog::NoDismiss);
	confirmationQuery->setTimeout(HbDialog::NoTimeout);
	confirmationQuery->setIconVisible(true);

	QString displayText;
	QString x;
	AgendaEntry entry = mAgendaUtil->fetchById(entryId);
	if (AgendaEntry::TypeTodo == entry.type()) {
		displayText += hbTrId("txt_notes_info_delete_todo_note");
	} else {
		displayText += hbTrId("txt_notes_info_delete_note");
	}

	confirmationQuery->setText(displayText);

	// Remove the default actions.
	QList<QAction *> defaultActions = confirmationQuery->actions();
	for (int index=0;index<defaultActions.count();index++) {
		confirmationQuery->removeAction(defaultActions[index]);
	}
	defaultActions.clear();

	// Add delete and cancel actions
	mDeleteAction = new HbAction(hbTrId("txt_common_button_delete"));
	mCancelAction = new HbAction(hbTrId("txt_common_button_cancel"));

	confirmationQuery->addAction(mDeleteAction);
	confirmationQuery->addAction(mCancelAction);

	confirmationQuery->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( NOTESVIEWMANAGER_DELETEENTRYFROMVIEW_EXIT );
}

/*!
	Slot to handle the delete action
 */
void NotesViewManager::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_SELECTEDACTION_ENTRY );
	if (action == mDeleteAction) {
		// Delete the given note.
		mAgendaUtil->deleteEntry(mEntryId);
	}
	OstTraceFunctionExit0( NOTESVIEWMANAGER_SELECTEDACTION_EXIT );
}

/*!
	Slot to handle instance view creation complete.
 */
void NotesViewManager::handleInstanceViewCreationCompleted(int status)
{
	OstTraceFunctionEntry0( NOTESVIEWMANAGER_HANDLEINSTANCEVIEWCREATIONCOMPLETED_ENTRY );
	Q_UNUSED(status)
	
	instanceViewCreated = true;
	
	if (mMainView) {
		// Update the title for main view.
		mMainView->updateTitle();
	}

	// update other views.
	updateOtherViews();

	// Need to emit this signal after the view is fully constructed & populated
	// with actual data and ready to be used. So entry view & instance view
	// needs to be created so that a new entry can also be created. Finally
	// NotesApplication object needs to emit applicationReady Signal.
	emit appReady();
	OstTraceFunctionExit0( NOTESVIEWMANAGER_HANDLEINSTANCEVIEWCREATIONCOMPLETED_EXIT );
}

// End of file	--Don't remove this.
