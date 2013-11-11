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
* Definition file for NotesTodoView class.
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
#include <HbDialog>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbListViewItem>

// User includes
#include <agendautil.h>
#include "agendaeventviewer.h"
#include "notestodoview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notestodoviewTraces.h"
#endif


/*!
	\class NotesTodoView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesTodoView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesTodoView::NotesTodoView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0),
 mIsLongTop(false)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_NOTESTODOVIEW_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( NOTESTODOVIEW_NOTESTODOVIEW_EXIT );
}

/*!
	Destructor.
 */
NotesTodoView::~NotesTodoView()
{
	OstTraceFunctionEntry0( DUP1_NOTESTODOVIEW_NOTESTODOVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESTODOVIEW_NOTESTODOVIEW_EXIT );
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesTodoView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_SETUPVIEW_ENTRY );
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	mProxyModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mNotesModel->sourceModel());

	mProxyModel->setFilterRole(NotesNamespace::TypeRole);
	mProxyModel->setFilterRegExp(QRegExp(QString("todo")));

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *>
			(mDocLoader->findWidget("todoListView"));
	// Update the list view model.
	mListView->setModel(subModel);
	mListView->setItemPixmapCacheEnabled(true);
	// Setup the operations that can be done with a list view.
	connect(
			mListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleItemReleased(const QModelIndex &)));
	connect(
			mListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));
	
	// Get the empty list string.
	mEmptyListLabel = static_cast<HbLabel *> (
			mDocLoader->findWidget("emptyListLabel"));
	
	// Get the toolbar/menu actions.
	mAddTodoAction = static_cast<HbAction *> (
			mDocLoader->findObject("newTodoAction"));
	connect(
			mAddTodoAction, SIGNAL(triggered()),
			this, SLOT(createNewTodo()));

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

	mSubTitle = static_cast<HbGroupBox *>(
			mDocLoader->findWidget("subtitleGroupBox"));

	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateSubTitle(ulong)));

	// Set the graphics size for the icons.
	HbListViewItem *prototype = mListView->listItemPrototype();
	prototype->setGraphicsSize(HbListViewItem::SmallIcon);
	OstTraceFunctionExit0( NOTESTODOVIEW_SETUPVIEW_EXIT );
}

/*
	Updates the title text for the first launch
 */
void NotesTodoView::updateTitle()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_UPDATETITLE_ENTRY );
	updateSubTitle();
	OstTraceFunctionExit0( NOTESTODOVIEW_UPDATETITLE_EXIT );
}

/*!
	Opens the to-do editor to create a new to-do.
 */
void NotesTodoView::createNewTodo()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_CREATENEWTODO_ENTRY );
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	mNotesEditor->create(NotesEditor::CreateTodo);
	OstTraceFunctionExit0( NOTESTODOVIEW_CREATENEWTODO_EXIT );
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesTodoView::handleItemReleased(const QModelIndex &index)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEITEMRELEASED_ENTRY );
	if(!mIsLongTop) {
		// Sanity check.
		if (!index.isValid()) {
			OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// First get the id of the to-do and get the corresponding information from
		// agendautil.
		ulong toDoId = index.data(NotesNamespace::IdRole).value<qulonglong>();

		if (0 >= toDoId) {
			// Something wrong.
			OstTraceFunctionExit0( DUP1_NOTESTODOVIEW_HANDLEITEMRELEASED_EXIT );
			return;
		}

		// Construct agenda event viewer.
		mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

		connect(
				mAgendaEventViewer, SIGNAL(viewingCompleted(const QDate)),
				this, SLOT(handleViewingCompleted()));
		// Launch agenda event viewer
		mAgendaEventViewer->view(toDoId, AgendaEventViewer::ActionEditDelete);
	}
	OstTraceFunctionExit0( DUP2_NOTESTODOVIEW_HANDLEITEMRELEASED_EXIT );
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesTodoView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEITEMLONGPRESSED_ENTRY );
	mSelectedItem = item;
	mIsLongTop = true;

	// Get the entry of the selected item.
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

	mEditAction = contextMenu->addAction(
			hbTrId("txt_common_menu_edit"));

	mDeleteAction = contextMenu->addAction(
			hbTrId("txt_common_menu_delete"));

	if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoCompleted != entry.status()) {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_done"));
		} else {
			mTodoStatusAction =
					contextMenu->addAction(
							hbTrId("txt_notes_menu_mark_as_not_done"));
		}
	}

	// Show the menu.
	contextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	contextMenu->setPreferredPos(coords);
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEITEMLONGPRESSED_EXIT );
}

/*!
	Slot to delete a selected to-do.
 */
void NotesTodoView::deleteTodo()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_DELETETODO_ENTRY );
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESTODOVIEW_DELETETODO_EXIT );
		return;
	}
	ulong entryId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!entryId) {
		OstTraceFunctionExit0( DUP1_NOTESTODOVIEW_DELETETODO_EXIT );
		return;
	}

	// Emit the signal. Delete would be handled in view manager.
	emit deleteEntry(entryId);

	mSelectedItem = 0;
	OstTraceFunctionExit0( DUP2_NOTESTODOVIEW_DELETETODO_EXIT );
}

/*!
	Marks to-do entry as done or undone based on the completed value.
 */
void NotesTodoView::markTodoStatus()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_MARKTODOSTATUS_ENTRY );
	ulong entryId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(entryId);

	QDateTime currentDateTime = QDateTime::currentDateTime();

	if (AgendaEntry::TodoCompleted != entry.status()) {
		mAgendaUtil->setCompleted(entry, true, currentDateTime);
	} else {
		mAgendaUtil->setCompleted(entry, false, currentDateTime);
	}
	OstTraceFunctionExit0( NOTESTODOVIEW_MARKTODOSTATUS_EXIT );
}

/*!
	Opens to-do in an editor.
 */
void NotesTodoView::editTodo()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_EDITTODO_ENTRY );
	QModelIndex index = mSelectedItem->modelIndex();

	// Sanity check.
	if (!index.isValid()) {
		OstTraceFunctionExit0( NOTESTODOVIEW_EDITTODO_EXIT );
		return;
	}

	// First get the id of the note and get the corresponding information from
	// agendautil.
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= noteId) {
		// Something wrong.
		OstTraceFunctionExit0( DUP1_NOTESTODOVIEW_EDITTODO_EXIT );
		return;
	}

	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		// Entry invalid.
		OstTraceFunctionExit0( DUP2_NOTESTODOVIEW_EDITTODO_EXIT );
		return;
	}

	// Now launch the editor with the obtained info.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->edit(entry);
	OstTraceFunctionExit0( DUP3_NOTESTODOVIEW_EDITTODO_EXIT );
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesTodoView::handleEditingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEEDITINGCOMPLETED_ENTRY );
	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEEDITINGCOMPLETED_EXIT );
}

/*!
	Directs the view manager to display the Collections view.
 */
void NotesTodoView::displayCollectionView()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_DISPLAYCOLLECTIONVIEW_ENTRY );
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);
	OstTraceFunctionExit0( NOTESTODOVIEW_DISPLAYCOLLECTIONVIEW_EXIT );
}

/*!
	Directs the view manager to display the All notes view.
 */
void NotesTodoView::displayAllNotesView()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_DISPLAYALLNOTESVIEW_ENTRY );
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
	OstTraceFunctionExit0( NOTESTODOVIEW_DISPLAYALLNOTESVIEW_EXIT );
}

/*!
	Slot to handle signal editingCompleted by the to-do editor
 */
void NotesTodoView::handleEditingCompleted()
{
	OstTraceFunctionEntry0( DUP1_NOTESTODOVIEW_HANDLEEDITINGCOMPLETED_ENTRY );
	// Cleanup.
	mNotesEditor->deleteLater();
	OstTraceFunctionExit0( DUP1_NOTESTODOVIEW_HANDLEEDITINGCOMPLETED_EXIT );
}

/*!
	Slot to handle signal handleViewingompleted by the agenda event viewer
 */
void NotesTodoView::handleViewingCompleted()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEVIEWINGCOMPLETED_ENTRY );
	// Cleanup.
	mAgendaEventViewer->deleteLater();
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEVIEWINGCOMPLETED_EXIT );
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesTodoView::handleActionStateChanged()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEACTIONSTATECHANGED_ENTRY );
	mAllNotesAction->setChecked(true);
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEACTIONSTATECHANGED_EXIT );
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesTodoView::handleOrientationChanged(Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEORIENTATIONCHANGED_ENTRY );
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);
	}
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEORIENTATIONCHANGED_EXIT );
}

/*!
	Updates the sub heading text
 */
void NotesTodoView::updateSubTitle(ulong id)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_UPDATESUBTITLE_ENTRY );
	Q_UNUSED(id)

	// Get the number of incompleted to-do entries.
	QList<ulong> entries = mAgendaUtil->entryIds(
		AgendaUtil::IncludeIncompletedTodos);
	mSubTitle->setHeading(
			hbTrId("txt_notes_subhead_todos_ln_pending",entries.count()));
	
	if (0 < mProxyModel->rowCount()) {
		mEmptyListLabel->hide();
		mListView->show();
	} else {
		mEmptyListLabel->show();
		mListView->hide();
	}
	OstTraceFunctionExit0( NOTESTODOVIEW_UPDATESUBTITLE_EXIT );
}

/*
	Opens the to-do viewer to view the to-do.
 */
void NotesTodoView::openTodo()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_OPENTODO_ENTRY );
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Construct agenda event viewer.
	mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

	connect(
			mAgendaEventViewer, SIGNAL(viewingCompleted(bool)),
			this, SLOT(handleViewingCompleted(bool)));

	// Launch agenda event viewer
	mAgendaEventViewer->view(
			entry, AgendaEventViewer::ActionEditDelete);
	OstTraceFunctionExit0( NOTESTODOVIEW_OPENTODO_EXIT );
}

/*
	Slot to handle the context menu actions.
 */
void NotesTodoView::selectedMenuAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_SELECTEDMENUACTION_ENTRY );
	if (action == mOpenAction) {
		openTodo();
	} else if (action == mEditAction) {
		editTodo();
	} else if (action == mDeleteAction) {
		deleteTodo();
	} else if (action == mTodoStatusAction) {
		markTodoStatus();
	}
	OstTraceFunctionExit0( NOTESTODOVIEW_SELECTEDMENUACTION_EXIT );
}


/*!
	Slot to handle the context menu closed.
 */
void NotesTodoView::handleMenuClosed()
{
	OstTraceFunctionEntry0( NOTESTODOVIEW_HANDLEMENUCLOSED_ENTRY );
	mIsLongTop = false;
	OstTraceFunctionExit0( NOTESTODOVIEW_HANDLEMENUCLOSED_EXIT );
}

// End of file	--Don't remove this.
