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
* Definition file for NotesCollectionView class.
*
*/

// System includes
#include <HbListWidget>
#include <QStandardItemModel>
#include <HbListView>
#include <HbAction>
#include <HbTextEdit>
#include <HbInstance>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDialog>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbStyleLoader>
#include <HbParameterLengthLimiter>

// User includes
#include <agendautil.h>
#include "notescollectionview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "notesmodel.h"
#include "noteseditor.h"
#include "notessortfilterproxymodel.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notescollectionviewTraces.h"
#endif


/*!
	\class NotesCollectionView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesCollectionView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesCollectionView::NotesCollectionView(QGraphicsWidget *parent)
:HbView(parent)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_NOTESCOLLECTIONVIEW_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_NOTESCOLLECTIONVIEW_EXIT );
}

/*!
	Destructor.
 */
NotesCollectionView::~NotesCollectionView()
{
	OstTraceFunctionEntry0( DUP1_NOTESCOLLECTIONVIEW_NOTESCOLLECTIONVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESCOLLECTIONVIEW_NOTESCOLLECTIONVIEW_EXIT );
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesCollectionView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_SETUPVIEW_ENTRY );
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong>)),
			this, SLOT(updateData(QList<ulong>)));

	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this, SLOT(updateData(ulong)));

	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this, SLOT(updateData(ulong)));

	mFavouriteModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mFavouriteModel->setDynamicSortFilter(true);
	mFavouriteModel->setFilterRole(NotesNamespace::FavouriteRole);
	mFavouriteModel->setFilterRegExp(QRegExp("favourites"));
	mFavouriteModel->setSourceModel(
			mAppControllerIf->notesModel()->sourceModel());
	connect(
			mFavouriteModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));
	connect(
			mFavouriteModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));

	// Get the list view from the document and update the model.
	mListView = static_cast<HbListView *> (
			mDocLoader->findWidget("listView"));
	mListView->setItemPixmapCacheEnabled(true);
	connect(
			mListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleActivated(const QModelIndex &)));

	// Get the toolbar/menu actions.
	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	Q_ASSERT_X(
			mAllNotesAction,
			"notescollectionview.cpp",
			"Unable to find allNotesAction.");
	connect(
			mAllNotesAction, SIGNAL(triggered()),
			this, SLOT(displayAllNotesView()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("collectionsViewAction"));
	Q_ASSERT_X(
			mViewCollectionAction,
			"notescollectionview.cpp",
			"Unable to find viewCollectionAction.");
	mViewCollectionAction->setCheckable(true);
	mViewCollectionAction->setChecked(true);
	connect(
			mViewCollectionAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));
	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(resetCollectionView()));

	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	Q_ASSERT_X(
			mAddNoteAction,
			"notescollectionview.cpp",
			"Unable to find addNoteAction.");
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_SETUPVIEW_EXIT );
}

/*!
	Populate the content of the view.
 */
void NotesCollectionView::populateListView()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_POPULATELISTVIEW_ENTRY );
	QStandardItemModel *model = new QStandardItemModel(this);
	model->setColumnCount(1);

	// Add To-do's item.
	QStandardItem *item = new QStandardItem();
	QStringList todoStringList;
	todoStringList.append(hbTrId("txt_notes_list_todos"));
	todoStringList.append(
			HbParameterLengthLimiter(
					hbTrId("txt_notes_list_note_count")).arg(QString::number(
							todosCount())));
	item->setData(todoStringList, Qt::DisplayRole);
	model->appendRow(item);

	// Add Favorites item.
	item = new QStandardItem();
	QStringList favStringList;
	favStringList.append(hbTrId("txt_notes_list_favorites"));
	favStringList.append(
			HbParameterLengthLimiter(
					hbTrId("txt_notes_list_note_count")).arg(QString::number(
							mFavouriteModel->rowCount())));
	item->setData(favStringList, Qt::DisplayRole);
	model->appendRow(item);

	// Add Recent notes item.
	item = new QStandardItem();
	QStringList notesStringList;
	notesStringList.append(hbTrId("txt_notes_list_plain_notes"));
	notesStringList.append(
			HbParameterLengthLimiter(
					hbTrId("txt_notes_list_note_count")).arg(QString::number(
							recentNotesCount())));
	item->setData(notesStringList, Qt::DisplayRole);
	model->appendRow(item);

	HbStyleLoader::registerFilePath(":/style");
	mListView->setLayoutName("custom");
	mListView->setModel(model);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_POPULATELISTVIEW_EXIT );
}

/*!
	Displays all notes view.
 */
void NotesCollectionView::displayAllNotesView()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_DISPLAYALLNOTESVIEW_ENTRY );
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_DISPLAYALLNOTESVIEW_EXIT );
}

/*!
	Refreshes the content of the collection view if needed.
 */
void NotesCollectionView::resetCollectionView()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_RESETCOLLECTIONVIEW_ENTRY );

	// Update the count of to-do's.
	QModelIndex mdlIndex = mListView->model()->index(0, 0);
	QStringList todoStringList;
	todoStringList.append(hbTrId("txt_notes_list_todos"));
	todoStringList.append(
			HbParameterLengthLimiter(
					hbTrId("txt_notes_list_note_count")).arg(QString::number(
							todosCount())));
	mListView->model()->setData(mdlIndex, todoStringList, Qt::DisplayRole);

	// Update the count of notes in the view.
	mdlIndex = mListView->model()->index(2, 0);
	QStringList notesStringList;
	notesStringList.append(hbTrId("txt_notes_list_plain_notes"));
	notesStringList.append(
			HbParameterLengthLimiter(
					hbTrId("txt_notes_list_note_count")).arg(QString::number(
							recentNotesCount())));
	mListView->model()->setData(mdlIndex, notesStringList, Qt::DisplayRole);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_RESETCOLLECTIONVIEW_EXIT );
}

/*!
	Create a new Note
 */
void NotesCollectionView::createNewNote()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_CREATENEWNOTE_ENTRY );
	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	mNotesEditor->create(NotesEditor::CreateNote);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_CREATENEWNOTE_EXIT );
}

/*!
	Handles editing complete of the notes editor
 */
void NotesCollectionView::handleEditingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_HANDLEEDITINGCOMPLETED_ENTRY );
	Q_UNUSED(status)

	// Refresh the content of the view.
	resetCollectionView();

	// Cleanup.
	mNotesEditor->deleteLater();
	
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_HANDLEEDITINGCOMPLETED_EXIT );
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when entry is added or deleted by notes application.
 */
void NotesCollectionView::updateData(ulong id)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_UPDATEDATA_ENTRY );
	Q_UNUSED(id)

	// Refresh the content of the view.
	resetCollectionView();
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_UPDATEDATA_EXIT );
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when db is updated by a different client.
 */
void NotesCollectionView::updateData(QList<ulong> ids)
{
	OstTraceFunctionEntry0( DUP1_NOTESCOLLECTIONVIEW_UPDATEDATA_ENTRY );
	Q_UNUSED(ids)

	// Refresh the content of the view.
	resetCollectionView();
	OstTraceFunctionExit0( DUP1_NOTESCOLLECTIONVIEW_UPDATEDATA_EXIT );
}

/*!
	Handles the case when a list item is activated and the corresponding
	collections view (viz., notes, to-dos, favourites) is opened.

	\param index The index of the item that was activated.
 */
void NotesCollectionView::handleActivated(const QModelIndex &index)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_HANDLEACTIVATED_ENTRY );

	switch (index.row()) {
		case 0: {
			if (todosCount()) {
				// To-do item selected.
				// Switch to to-do view if to-do entries exist.
				mAppControllerIf->switchToView(NotesNamespace::NotesTodoViewId);
			}
		}
		break;

		case 1: {
			if (mFavouriteModel->rowCount()) {
				// Favorites item selected.
				// Switch to favorites view if favorites exist.
				mAppControllerIf->switchToView(
						NotesNamespace::NotesFavoritesViewId);
			}
		}
		break;

		case 2: {
			if (recentNotesCount()) {
				// Recent notes item selected.
				// Switch to recent notes view if notes exist.
				mAppControllerIf->switchToView(
						NotesNamespace::NotesNoteViewId);
			}
		}
		break;

		default:
			// Nothing yet.
			break;
	}
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_HANDLEACTIVATED_EXIT );
}

/*!
	Updates the number of favourites displayed.

	\sa QAbstractItemModel
 */
void NotesCollectionView::updateFavouritesCount(
		const QModelIndex &index, int start, int end)
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_UPDATEFAVOURITESCOUNT_ENTRY );
	Q_UNUSED(index)
	Q_UNUSED(start)
	Q_UNUSED(end)
	
	// Check model is set for mListView  before update 	
	if( mListView->model())
	{
		// Update the count of notes in the view.
		QModelIndex mdlIndex = mListView->model()->index(1, 0);
		QStringList favStringList;
		favStringList.append(hbTrId("txt_notes_list_favorites"));
		favStringList.append(
				HbParameterLengthLimiter(
						hbTrId("txt_notes_list_note_count")).arg(
								QString::number(mFavouriteModel->rowCount())));
		mListView->model()->setData(mdlIndex, favStringList, Qt::DisplayRole);
	}
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_UPDATEFAVOURITESCOUNT_EXIT );
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesCollectionView::handleActionStateChanged()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_HANDLEACTIONSTATECHANGED_ENTRY );
	mViewCollectionAction->setChecked(true);
	OstTraceFunctionExit0( NOTESCOLLECTIONVIEW_HANDLEACTIONSTATECHANGED_EXIT );
}


/*!
	Returns the todos count

	\return int todos count.
 */
int NotesCollectionView::todosCount()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_TODOSCOUNT_ENTRY );
	QList<ulong> entries = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags)
			(AgendaUtil::IncludeCompletedTodos
			| AgendaUtil::IncludeIncompletedTodos));
	return entries.count();
}

/*!
	Returns recent notes count.

	\return int recent notes count.
 */
int NotesCollectionView::recentNotesCount()
{
	OstTraceFunctionEntry0( NOTESCOLLECTIONVIEW_RECENTNOTESCOUNT_ENTRY );
	QList<ulong> entries = mAgendaUtil->entryIds(AgendaUtil::IncludeNotes);
	return entries.count();
}

// End of file	--Don't remove this.
