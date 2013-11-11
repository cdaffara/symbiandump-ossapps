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
* Definition file for NotesMainView class.
*
*/

// System includes
#include <QDateTime>
#include <HbListView>
#include <HbListWidget>
#include <HbAction>
#include <HbTextEdit>
#include <HbInstance>
#include <HbMainWindow>
#include <HbMenu>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbAbstractItemView>
#include <HbGroupBox>
#include <HbListViewItem>
#include <hbapplication> // hbapplication
#include <AfActivityStorage.h>


// User includes
#include <agendautil.h>
#include "agendaeventviewer.h"
#include "notesmainview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"
#include "notescommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesmainviewTraces.h"
#endif
 // NotesNamespace

/*!
	\class NotesMainView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesMainView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesMainView::NotesMainView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0),
 mIsLongTop(false),
 mIsScreenShotCapruted(false)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_NOTESMAINVIEW_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( NOTESMAINVIEW_NOTESMAINVIEW_EXIT );
}

/*!
	Destructor.
 */
NotesMainView::~NotesMainView()
{
	OstTraceFunctionEntry0( DUP1_NOTESMAINVIEW_NOTESMAINVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}

	OstTraceFunctionExit0( DUP1_NOTESMAINVIEW_NOTESMAINVIEW_EXIT );
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesMainView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_SETUPVIEW_ENTRY );
    // Get a pointer to activity storage
	mActivityStorage = new AfActivityStorage(this);
    
    
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	mProxyModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mNotesModel->sourceModel());

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *> (mDocLoader->findWidget("listView"));
	mListView->setItemPixmapCacheEnabled(true);
	Q_ASSERT_X(
			mListView,
			"notesmainview.cpp",
			"Unable to find list view.");
	// Update the list view model.
	mListView->setModel(subModel);

	// Setup the operations that can be done with a list view.
	connect(
			mListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleItemReleased(const QModelIndex &)));
	connect(
			mListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));
	connect(
			mNotesModel, SIGNAL(rowAdded(QModelIndex)),
			this, SLOT(scrollTo(QModelIndex)));

	// Get the empty list label.
	mEmptyListLabel = static_cast<HbLabel *> (
			mDocLoader->findWidget("emptyListLabel"));
	mEmptyListLabel->hide();

	// Get the view heading label
	mSubTitle = static_cast<HbGroupBox *>(
			mDocLoader->findWidget("viewHeading"));

	// Handles the orientation change for list items
	HbMainWindow *window = hbInstance->allMainWindows().first();
	handleOrientationChanged(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChanged(Qt::Orientation)));

	// Set the graphics size for the icons.
	HbListViewItem *prototype = mListView->listItemPrototype();
	prototype->setGraphicsSize(HbListViewItem::SmallIcon);
	

	// clean up any previous versions of this activity 
	// ignore return value as the first boot would always return a false
	// bool declared on for debugging purpose
	bool ok = removeActivity();

	// connect main view for the first time to recieve aboutToQuit signal
	connect(
			qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()),
			this, SLOT(saveActivity()));
	
	OstTraceFunctionExit0( NOTESMAINVIEW_SETUPVIEW_EXIT );
}

void NotesMainView::setupAfterViewReady()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_SETUPAFTERVIEWREADY_ENTRY );
	// Get the toolbar/menu actions.
	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	Q_ASSERT_X(
			mAddNoteAction,
			"notesmainview.cpp",
			"Unable to find addNoteAction.");
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));

	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	Q_ASSERT_X(
			mAllNotesAction,
			"notesmainview.cpp",
			"Unable to find allNotesAction.");
	mAllNotesAction->setCheckable(true);
	mAllNotesAction->setChecked(true);
	connect(
			mAllNotesAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("collectionsViewAction"));
	Q_ASSERT_X(
			mViewCollectionAction,
			"notescollectionview.cpp",
			"Unable to find viewCollectionAction.");

	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(displayCollectionView()));

	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateSubTitle(ulong)));
	OstTraceFunctionExit0( NOTESMAINVIEW_SETUPAFTERVIEWREADY_EXIT );
}

/*
	Updates the title text for the first launch
 */
void NotesMainView::updateTitle()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_UPDATETITLE_ENTRY );
	updateSubTitle();
	OstTraceFunctionExit0( NOTESMAINVIEW_UPDATETITLE_EXIT );
}

/*!
	Slot which gets called when `+ New note' action is triggered from the view
	toolbar. This is responsible for launching the editor to create a new note.
 */
void NotesMainView::createNewNote()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_CREATENEWNOTE_ENTRY );

	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->create(NotesEditor::CreateNote);
	// capture screenshot for future use, if application
	// is exited/Quit from notesEditor
	captureScreenShot(true);
	OstTraceFunctionExit0( NOTESMAINVIEW_CREATENEWNOTE_EXIT );
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesMainView::handleItemReleased(const QModelIndex &index)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEITEMRELEASED_ENTRY );
	if(!mIsLongTop) {
		// Sanity check.
		if (!index.isValid()) {
			OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// First get the id of the note and get the corresponding information
		// from agendautil.
		ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

		if (0 >= noteId) {
			// Something wrong.
			OstTraceFunctionExit0( DUP1_NOTESMAINVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// Get the entry details.
		AgendaEntry entry = mAgendaUtil->fetchById(noteId);
		if (entry.isNull()) {
			// Entry invalid.
			OstTraceFunctionExit0( DUP2_NOTESMAINVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		if(AgendaEntry::TypeTodo == entry.type()) {
			// Construct agenda event viewer.
			mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

			connect(
					mAgendaEventViewer, SIGNAL(viewingCompleted(const QDate)),
					this, SLOT(handleViewingCompleted()));
			// Launch agenda event viewer
			mAgendaEventViewer->view(
					entry, AgendaEventViewer::ActionEditDelete);
		}else if(AgendaEntry::TypeNote == entry.type()) {
			// Construct notes editor.
			mNotesEditor = new NotesEditor(mAgendaUtil, this);
			connect(
					mNotesEditor, SIGNAL(editingCompleted(bool)),
					this, SLOT(handleEditingCompleted(bool)));

			// Launch the notes editor with the obtained info.
			mNotesEditor->edit(entry);
		}
		// capture screenshot for future use, if application
		// is exited/Quit from eventViewer/notesEditor
		captureScreenShot(true);
	}
	OstTraceFunctionExit0( DUP3_NOTESMAINVIEW_HANDLEITEMRELEASED_EXIT );
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesMainView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEITEMLONGPRESSED_ENTRY );
	mIsLongTop = true;
	mSelectedItem = item;

	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();
	connect(
			contextMenu,SIGNAL(aboutToClose()),
			this, SLOT(handleMenuClosed()));

	mOpenAction =
			contextMenu->addAction(hbTrId("txt_common_menu_open"));

	// Add actions to the context menu.
	if (AgendaEntry::TypeTodo == entry.type()) {
		mEditTodoAction =
				contextMenu->addAction(hbTrId("txt_common_menu_edit"));
	}

	mDeleteAction =
			contextMenu->addAction(hbTrId("txt_common_menu_delete"));

	if (AgendaEntry::TypeNote == entry.type()) {
		if (entry.favourite()) {
			mMakeFavouriteAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_remove_from_favorites"));
		} else {
			mMakeFavouriteAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_favorite"));
		}

		mMarkTodoAction =
				contextMenu->addAction(
						hbTrId("txt_notes_menu_make_it_as_todo_note"));

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoCompleted != entry.status()) {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_done"));
		} else {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_not_done"));
		}
	}

	// Show the menu.
	contextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	contextMenu->setPreferredPos(coords);
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEITEMLONGPRESSED_EXIT );
}

/*!
	Slot to delete a selected note.
 */
void NotesMainView::deleteNote()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_DELETENOTE_ENTRY );
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESMAINVIEW_DELETENOTE_EXIT );
		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {

		OstTraceFunctionExit0( DUP1_NOTESMAINVIEW_DELETENOTE_EXIT );
		return;
	}

	// Emit the signal.Deletion would happen in view manager.
	emit deleteEntry(noteId);

	mSelectedItem = 0;
	OstTraceFunctionExit0( DUP2_NOTESMAINVIEW_DELETENOTE_EXIT );
}

/*!
	Marks to-do entry as done or undone based on the completed value

	\param entry reference to the agenda entry
	\param status is true if to-do entry to be marked as done
			is false if to-do entry to be marked as undone
 */
void NotesMainView::markTodoStatus()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_MARKTODOSTATUS_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	QDateTime currentDateTime = QDateTime::currentDateTime();

	if (AgendaEntry::TodoCompleted != entry.status()) {
		mAgendaUtil->setCompleted(entry, true, currentDateTime);
	} else {
		mAgendaUtil->setCompleted(entry, false, currentDateTime);
	}

	OstTraceFunctionExit0( NOTESMAINVIEW_MARKTODOSTATUS_EXIT );
}

/*!
	Marks/unmarks the note as favourite.
 */
void NotesMainView::markNoteAsFavourite()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_MARKNOTEASFAVOURITE_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
				NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.favourite()) {
		entry.setFavourite(0);
	} else {
		entry.setFavourite(1);
	}
	mAgendaUtil->store(entry);

	OstTraceFunctionExit0( NOTESMAINVIEW_MARKNOTEASFAVOURITE_EXIT );
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesMainView::handleEditingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEEDITINGCOMPLETED_ENTRY );
	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();
	// set captured screenshot as invalid as the control is returned back 
	// to the main view
	captureScreenShot(false);
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEEDITINGCOMPLETED_EXIT );
}

/*!
	Displays the collections' view.
 */
void NotesMainView::displayCollectionView()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_DISPLAYCOLLECTIONVIEW_ENTRY );
	// no need to capture the screen shot for future use as 
	// NotesViewManager::switchToView takes care of it
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

	OstTraceFunctionExit0( NOTESMAINVIEW_DISPLAYCOLLECTIONVIEW_EXIT );
}

/*!
	Slot where the list view is made to scroll to the QModelIndex index.

	\param index QModelIndex to be scrolled to.
 */
void NotesMainView::scrollTo(QModelIndex index)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_SCROLLTO_ENTRY );
	QModelIndex modelIndex = mListView->model()->index(
			index.row(), index.column());
	mListView->scrollTo(modelIndex, HbAbstractItemView::EnsureVisible);
	OstTraceFunctionExit0( NOTESMAINVIEW_SCROLLTO_EXIT );
}

/*!
	Slot to handle viewing Completed signal from agenda event viewer

	\param status Indicates the status of viewing
 */
void NotesMainView::handleViewingCompleted()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEVIEWINGCOMPLETED_ENTRY );
	mAgendaEventViewer->deleteLater();
	// set captured screenshot as invalid as the control is returned back 
	// to the main view
	captureScreenShot(false);
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEVIEWINGCOMPLETED_EXIT );
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesMainView::handleActionStateChanged()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEACTIONSTATECHANGED_ENTRY );
	mAllNotesAction->setChecked(true);
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEACTIONSTATECHANGED_EXIT );
}

/*!
	Launches the to-do editor to edit the to-do entry
 */

void NotesMainView::editTodo()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_EDITTODO_ENTRY );
	// Get the selected list item index
	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESMAINVIEW_EDITTODO_EXIT );
		return;
	}
	ulong todoId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!todoId) {

		OstTraceFunctionExit0( DUP1_NOTESMAINVIEW_EDITTODO_EXIT );
		return;
	}

	// Construct notes editor.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	// Launch the to-do editor with the obtained info.
	mNotesEditor->edit(todoId);
	// capture screenshot for future use, if application
	// is exited/Quit from notesEditor
	captureScreenShot(true);

	OstTraceFunctionExit0( DUP2_NOTESMAINVIEW_EDITTODO_EXIT );
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesMainView::handleOrientationChanged(Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEORIENTATIONCHANGED_ENTRY );
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);
	}
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEORIENTATIONCHANGED_EXIT );
}

/*!
	Updates the sub heading text
 */
void NotesMainView::updateSubTitle(ulong id)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_UPDATESUBTITLE_ENTRY );
	Q_UNUSED(id)

	// Get the number of notes and to-do entries.
	QList<ulong> entries = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags)
			(AgendaUtil::IncludeNotes
			| AgendaUtil::IncludeCompletedTodos
			| AgendaUtil::IncludeIncompletedTodos));
	
	if (0 >= entries.count()) {
		mEmptyListLabel->show();
		mListView->hide();
	} else {
		mEmptyListLabel->hide();
		mListView->show();
	}
	
	mSubTitle->setHeading(
			hbTrId("txt_notes_subhead_ln_notes",entries.count()));
	OstTraceFunctionExit0( NOTESMAINVIEW_UPDATESUBTITLE_EXIT );
}

/*!
	Slot to make a note as to-do.
 */
void NotesMainView::markNoteAsTodo()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_MARKNOTEASTODO_ENTRY );
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESMAINVIEW_MARKNOTEASTODO_EXIT );
		return;
	}
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong> ();
	if (!noteId) {
		OstTraceFunctionExit0( DUP1_NOTESMAINVIEW_MARKNOTEASTODO_EXIT );
		return;
	}
	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		// Entry invalid.
		OstTraceFunctionExit0( DUP2_NOTESMAINVIEW_MARKNOTEASTODO_EXIT );
		return;
	}

	// Here change the type of modified note and destroy the noteeditor and
	// construct the to-do editor.
	entry.setType(AgendaEntry::TypeTodo);

	QDateTime dueDateTime;
	QDate currentDate(QDate::currentDate());
	dueDateTime.setDate(
			QDate(currentDate.year(),currentDate.month(),currentDate.day()+1));
	dueDateTime.setTime(QTime::fromString("12:00 am", "hh:mm ap"));

	entry.setStartAndEndTime(dueDateTime, dueDateTime);

	entry.setSummary(entry.description().left(80));

	if (80 > entry.description().length()) {
		entry.setDescription("");
	}

	// Remove favourite if marked so.
	entry.setFavourite(0);
	
	// Set the priority of the to-do as normal
	entry.setPriority(2);

	// Set the status of the to-do.
	entry.setStatus(AgendaEntry::TodoNeedsAction);

	// First clone the todoEntry for the new type.
	mAgendaUtil->cloneEntry(entry, AgendaEntry::TypeTodo);

	// Delete the old entry.
	mAgendaUtil->deleteEntry(entry.id());
	OstTraceFunctionExit0( DUP3_NOTESMAINVIEW_MARKNOTEASTODO_EXIT );
}


/*
	Opens the notes editor if selected item is note otherwise opens
	to-do viewer if selected item is to-do event
 */
void NotesMainView::openNote()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_OPENNOTE_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (AgendaEntry::TypeNote == entry.type()) {
		// Construct notes editor.
		mNotesEditor = new NotesEditor(mAgendaUtil, this);
		connect(
				mNotesEditor, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleEditingCompleted(bool)));

		// Launch the notes editor with the obtained info.
		mNotesEditor->edit(entry);
	} else if (AgendaEntry::TypeTodo == entry.type()) {

		// Construct agenda event viewer.
		mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

		connect(
				mAgendaEventViewer, SIGNAL(viewingCompleted(bool)),
				this, SLOT(handleViewingCompleted(bool)));
		// Launch agenda event viewer
		mAgendaEventViewer->view(
				entry, AgendaEventViewer::ActionEditDelete);
	}
	// capture screenshot for future use, if application
	// is exited/Quit from notesEditor/eventViewer
	captureScreenShot(true);
	OstTraceFunctionExit0( NOTESMAINVIEW_OPENNOTE_EXIT );
}

/*!
	 Slot to handle the selected context menu actions
 */
void NotesMainView::selectedMenuAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_SELECTEDMENUACTION_ENTRY );
	if (action == mOpenAction) {
		openNote();
	} else if (action == mEditTodoAction) {
		editTodo();
	} else if (action == mDeleteAction) {
		deleteNote();
	} else if (action == mMakeFavouriteAction) {
		markNoteAsFavourite();
	} else if (action == mMarkTodoAction) {
		markNoteAsTodo();
	} else if (action == mTodoStatusAction) {
		markTodoStatus();
	}
	OstTraceFunctionExit0( NOTESMAINVIEW_SELECTEDMENUACTION_EXIT );
}

/*!
	Slot to handle the context menu closed.
 */
void NotesMainView::handleMenuClosed()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_HANDLEMENUCLOSED_ENTRY );
	mIsLongTop = false;
	OstTraceFunctionExit0( NOTESMAINVIEW_HANDLEMENUCLOSED_EXIT );
}

/*!
	CaptureScreenShot captures screen shot 
	\param captureScreenShot bool to indicate if screenshot needs to be captured
*/ 
void NotesMainView::captureScreenShot(bool captureScreenShot)
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_CAPTURESCREENSHOT_ENTRY );
	// check if screen shot needs to be captured
	if (captureScreenShot) {
		mScreenShot.clear();
		mScreenShot.insert(
				"screenshot", QPixmap::grabWidget(
						mainWindow(), mainWindow()->rect()));
	}
	// set mIsScreenShotCapruted set validity of screenshot
	mIsScreenShotCapruted = captureScreenShot;
	OstTraceFunctionExit0( NOTESMAINVIEW_CAPTURESCREENSHOT_EXIT );
}

/*!
	saveActivity saves main view as an activity 
*/
void NotesMainView::saveActivity()
{
	OstTraceFunctionEntry0( NOTESMAINVIEW_SAVEACTIVITY_ENTRY );

	
	// check if a valid screenshot is already captured
	if (!mIsScreenShotCapruted) {
		mScreenShot.clear();
		mScreenShot.insert(
				"screenshot", QPixmap::grabWidget(
						mainWindow(), mainWindow()->rect()));
	}

	// save any data necessary to save the state
	QByteArray serializedActivity;
	QDataStream stream(
			&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
	stream << NotesNamespace::NotesMainViewId;

	// add the activity to the activity manager
	bool ok = mActivityStorage->saveActivity(
			notes, serializedActivity, mScreenShot);

	if (!ok) {
		qFatal("Add failed" );
	}
	OstTraceFunctionExit0( NOTESMAINVIEW_SAVEACTIVITY_EXIT );
}

bool NotesMainView::removeActivity()
{
    OstTraceFunctionEntry0( NOTESMAINVIEW_REMOVEACTIVITY_ENTRY );
    OstTraceFunctionExit0( NOTESMAINVIEW_REMOVEACTIVITY_EXIT);
    return mActivityStorage->removeActivity(notes);
}
// End of file	--Don't remove this.
