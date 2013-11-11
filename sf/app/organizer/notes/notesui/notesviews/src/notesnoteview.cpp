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
 * Definition file for NotesNoteView class.
 *
 */

// System includes
#include <QDateTime>
#include <HbListView>
#include <HbListWidget>
#include <HbAction>
#include <HbMenu>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbListViewItem>
#include <HbInstance>
#include <HbNotificationDialog>
#include <HbLabel>

// User includes
#include <agendautil.h>
#include "notesnoteview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesnoteviewTraces.h"
#endif



/*!
	\class NotesNoteView
	\brief The Note view of the notes application. Responsible for displaying
			the all notes.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesNoteView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesNoteView::NotesNoteView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0),
 mIsLongTop(false)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_NOTESNOTEVIEW_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( NOTESNOTEVIEW_NOTESNOTEVIEW_EXIT );
}

/*!
	Destructor.
 */
NotesNoteView::~NotesNoteView()
{
	OstTraceFunctionEntry0( DUP1_NOTESNOTEVIEW_NOTESNOTEVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESNOTEVIEW_NOTESNOTEVIEW_EXIT );
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesNoteView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_SETUPVIEW_ENTRY );
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	mProxyModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mNotesModel->sourceModel());

	mProxyModel->setFilterRole(NotesNamespace::TypeRole);
	mProxyModel->setFilterRegExp(QRegExp(QString("note")));

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *> (
			mDocLoader->findWidget("noteListView"));
	
	// Update the list view model.
	mListView->setModel(subModel);
	mListView->setItemPixmapCacheEnabled(true);
	// Setup the operations that can be done with a list view.
	connect(
			mListView, SIGNAL(released(const QModelIndex &)),
			this, SLOT(handleItemReleased(const QModelIndex &)));
	connect(
			mListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));

	// Get the empty list label.
	mEmptyListLabel = static_cast<HbLabel *> (
			mDocLoader->findWidget("emptyListLabel"));

	// Get the toolbar/menu actions.
	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));

	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	connect(
			mAllNotesAction, SIGNAL(triggered()),
			this, SLOT(displayAllNotesView()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("displayCollectionsAction"));
	
	connect(
			mViewCollectionAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));
	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(displayCollectionView()));

	// Handles the orientation change for list items
	HbMainWindow *window = hbInstance->allMainWindows().first();
	handleOrientationChanged(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChanged(Qt::Orientation)));


	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this,SLOT(updateView(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this,SLOT(updateView(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateView(ulong)));
	

	// Set the graphics size for the icons.
	HbListViewItem *prototype = mListView->listItemPrototype();
	prototype->setGraphicsSize(HbListViewItem::SmallIcon);
	OstTraceFunctionExit0( NOTESNOTEVIEW_SETUPVIEW_EXIT );
}

/*
	Updates the note view either to show notes or emptyListLabel.
 */
void NotesNoteView::updateNoteView()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_UPDATENOTEVIEW_ENTRY );
	updateView();
	OstTraceFunctionExit0( NOTESNOTEVIEW_UPDATENOTEVIEW_EXIT );
}

/*!
	Opens the note editor to create a new note.
 */
void NotesNoteView::createNewNote()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_CREATENEWNOTE_ENTRY );
	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->create(NotesEditor::CreateNote);
	OstTraceFunctionExit0( NOTESNOTEVIEW_CREATENEWNOTE_EXIT );
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesNoteView::handleItemReleased(const QModelIndex &index)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEITEMRELEASED_ENTRY );
	if(!mIsLongTop) {
		// Sanity check.
		if (!index.isValid()) {
			OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// First get the id of the note and get the corresponding information
		// from agendautil.
		ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

		if (0 >= noteId) {
			// Something wrong.
			OstTraceFunctionExit0( DUP1_NOTESNOTEVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// Get the entry details.
		AgendaEntry entry = mAgendaUtil->fetchById(noteId);

		if (entry.isNull()) {
			// Entry invalid.
			OstTraceFunctionExit0( DUP2_NOTESNOTEVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// Now launch the editor with the obtained info.
		mNotesEditor = new NotesEditor(mAgendaUtil, this);
		connect(
				mNotesEditor, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleEditingCompleted(bool)));
		mNotesEditor->edit(entry);
	}
	OstTraceFunctionExit0( DUP3_NOTESNOTEVIEW_HANDLEITEMRELEASED_EXIT );
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesNoteView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEITEMLONGPRESSED_ENTRY );
	mSelectedItem = item;
	mIsLongTop = true;

	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();
	connect(
			contextMenu,SIGNAL(aboutToClose()),
			this, SLOT(handleMenuClosed()));

	// Add actions to the context menu.
	mOpenAction =
			contextMenu->addAction(hbTrId("txt_common_menu_open"));

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
		mMarkTodoAction = contextMenu->addAction(
				hbTrId("txt_notes_menu_make_it_as_todo_note"));
	}

	// Show the menu.
	contextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	contextMenu->setPreferredPos(coords);
	OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEITEMLONGPRESSED_EXIT );
}

/*!
	Slot to delete a selected note.
 */
void NotesNoteView::deleteNote()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_DELETENOTE_ENTRY );
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESNOTEVIEW_DELETENOTE_EXIT );
		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {
		OstTraceFunctionExit0( DUP1_NOTESNOTEVIEW_DELETENOTE_EXIT );
		return;
	}
	// Emitting the signal , deletion would be handle in view manager.
	emit deleteEntry(noteId);

	mSelectedItem = 0;
	OstTraceFunctionExit0( DUP2_NOTESNOTEVIEW_DELETENOTE_EXIT );
}

/*!
    Marks/unmarks the note as favourite.
 */
void NotesNoteView::markNoteAsFavourite()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_MARKNOTEASFAVOURITE_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.favourite()) {
		entry.setFavourite(0);
	} else {
		entry.setFavourite(1);
	}
	mAgendaUtil->store(entry);
	OstTraceFunctionExit0( NOTESNOTEVIEW_MARKNOTEASFAVOURITE_EXIT );
}

/*!
	Slot to make a note as to-do.
 */
void NotesNoteView::markNoteAsTodo()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_MARKNOTEASTODO_ENTRY );
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESNOTEVIEW_MARKNOTEASTODO_EXIT );
		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {
		OstTraceFunctionExit0( DUP1_NOTESNOTEVIEW_MARKNOTEASTODO_EXIT );
		return;
	}
	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		// Entry invalid.
		OstTraceFunctionExit0( DUP2_NOTESNOTEVIEW_MARKNOTEASTODO_EXIT );
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

	// Set the status of the to-do.
	entry.setStatus(AgendaEntry::TodoNeedsAction);

	// First clone the todoEntry for the new type.
	mAgendaUtil->cloneEntry(
			entry, AgendaEntry::TypeTodo);

	// Delete the old entry.
	mAgendaUtil->deleteEntry(entry.id());

	// Show the soft notification.
	HbNotificationDialog *notificationDialog = new HbNotificationDialog();
	notificationDialog->setTimeout(
			HbNotificationDialog::ConfirmationNoteTimeout);
	notificationDialog->setTitle(
			hbTrId("txt_notes_dpopinfo_note_moved_to_todos"));
	notificationDialog->show();
	OstTraceFunctionExit0( DUP3_NOTESNOTEVIEW_MARKNOTEASTODO_EXIT );
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesNoteView::handleEditingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEEDITINGCOMPLETED_ENTRY );
	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();
	OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEEDITINGCOMPLETED_EXIT );
}

/*!
	Directs the view manager to display the Collections view.
 */
void NotesNoteView::displayCollectionView()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_DISPLAYCOLLECTIONVIEW_ENTRY );
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

	OstTraceFunctionExit0( NOTESNOTEVIEW_DISPLAYCOLLECTIONVIEW_EXIT );
}

/*!
	Directs the view manager to display the All notes view.
 */
void NotesNoteView::displayAllNotesView()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_DISPLAYALLNOTESVIEW_ENTRY );
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
	OstTraceFunctionExit0( NOTESNOTEVIEW_DISPLAYALLNOTESVIEW_EXIT );
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesNoteView::handleActionStateChanged()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEACTIONSTATECHANGED_ENTRY );
	mAllNotesAction->setChecked(true);
	OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEACTIONSTATECHANGED_EXIT );
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesNoteView::handleOrientationChanged(Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEORIENTATIONCHANGED_ENTRY );
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);
	}
	OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEORIENTATIONCHANGED_EXIT );
}

/*
	Opens the notes editor to edit the note.
 */
void NotesNoteView::openNote()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_OPENNOTE_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();

	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Construct notes editor.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	// Launch the notes editor with the obtained info.
	mNotesEditor->edit(entry);
	OstTraceFunctionExit0( NOTESNOTEVIEW_OPENNOTE_EXIT );
}

/*!
	Slot to handle context menu actions.
 */
void NotesNoteView::selectedMenuAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_SELECTEDMENUACTION_ENTRY );
	if (action == mOpenAction) {
		openNote();
	} else if (action == mDeleteAction) {
		deleteNote();
	} else if (action == mMakeFavouriteAction) {
		markNoteAsFavourite();
	} else if (action == mMarkTodoAction) {
		markNoteAsTodo();
	}
	OstTraceFunctionExit0( NOTESNOTEVIEW_SELECTEDMENUACTION_EXIT );
}

/*!
	Slot to handle the context menu closed.
 */
void NotesNoteView::handleMenuClosed()
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_HANDLEMENUCLOSED_ENTRY );
	mIsLongTop = false;
	OstTraceFunctionExit0( NOTESNOTEVIEW_HANDLEMENUCLOSED_EXIT );
}

/*!
	Handles the visibility of empty list label.
 */
void NotesNoteView::updateView(ulong id)
{
	OstTraceFunctionEntry0( NOTESNOTEVIEW_UPDATEVIEW_ENTRY );
	Q_UNUSED(id)

	// Get the numbers of notes.
	if (0 >= mListView->model()->rowCount()) {
		mEmptyListLabel->show();
		mListView->hide();
	} else {
		mEmptyListLabel->hide();
		mListView->show();
	}
	OstTraceFunctionExit0( NOTESNOTEVIEW_UPDATEVIEW_EXIT );
}

// End of file	--Don't remove this.
