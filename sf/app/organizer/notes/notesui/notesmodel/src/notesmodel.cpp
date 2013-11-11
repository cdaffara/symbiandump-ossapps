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
* Definition file for class NotesModel.
*
*/

// System includes
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QDateTime>
#include <HbIcon>
#include <HbExtendedLocale>

// User includes
#include "notesmodel.h"
#include <agendautil.h>
#include <agendaentry.h>
#include "notescommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesmodelTraces.h"
#endif


/*!
	\class NotesModel

	This is the source model class for the notes application views. This owns
	a QStandardItemModel which is a model for the data containing notes, todos
	and reminders.
 */

/*!
	Constructor.

	\param agendaUtil Pointer to the object of AgendaUtil.
	\param parent Parent of type QObject.
 */
NotesModel::NotesModel(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent),
 mAgendaUtil(agendaUtil)
{
	OstTraceFunctionEntry0( NOTESMODEL_NOTESMODEL_ENTRY );
	// Construct the source model.
	mSourceModel = new QStandardItemModel(0, 1, this);

	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong>)),
			this, SLOT(populateSourceModel(QList<ulong>)));

	// Connect to entryAdded signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this, SLOT(addEntryToModel(ulong)));
	// Connect to entryDeleted signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this, SLOT(removeEntryFromModel(ulong)));
	// Connect to entryUpdated signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateSourceModel(ulong)));

	// Connect for instance view creation completed signal
	connect(
			mAgendaUtil, SIGNAL(instanceViewCreationCompleted(int)),
			this,SLOT(handleInstanceViewCreationCompleted(int)));
	OstTraceFunctionExit0( NOTESMODEL_NOTESMODEL_EXIT );
}

/*!
	Destructor.
 */
NotesModel::~NotesModel()
{
	OstTraceFunctionEntry0( DUP1_NOTESMODEL_NOTESMODEL_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( DUP1_NOTESMODEL_NOTESMODEL_EXIT );
}

/*!
	Returns the source model to be used with a view.

	\return QAbstractItemModel
	\sa QAbstractItemModel, HbListView.
 */
QAbstractItemModel *NotesModel::sourceModel()
{
	OstTraceFunctionEntry0( NOTESMODEL_SOURCEMODEL_ENTRY );
	Q_ASSERT(mSourceModel);
	OstTraceFunctionExit0( NOTESMODEL_SOURCEMODEL_EXIT );
	return mSourceModel;
}

/*!
	Populates the source model.
 */
void NotesModel::populateSourceModel()
{
	OstTraceFunctionEntry0( NOTESMODEL_POPULATESOURCEMODEL_ENTRY );
	// Clear the model if it has any data already
	mSourceModel->clear();
	mSourceModel->setColumnCount(1);
	mNotesCount = mInCompTodoCount = mCompTodoCount = 0;

	// The order of appending the items to the model is:
	// 1. Notes, 2. Incompleted to-dos, 3. Completed to-dos.
	// First get only the notes and populate the model.
	QList<AgendaEntry> agendaEntyList = mAgendaUtil->fetchAllEntries(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeNotes));

	// Add the notes to the model.
	appendNotesToModel(agendaEntyList);

	// Get the incompleted to-dos.
	agendaEntyList.clear();
	agendaEntyList = mAgendaUtil->fetchAllEntries(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeIncompletedTodos));

	// Add the incompleted to-dos to the model.
	appendInCompTodosToModel(agendaEntyList);

	// Get the completed to-dos.
	agendaEntyList.clear();
	agendaEntyList = mAgendaUtil->fetchAllEntries(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeCompletedTodos));

	// Add the completed to-dos to the model.
	appendCompTodosToModel(agendaEntyList);
	OstTraceFunctionExit0( NOTESMODEL_POPULATESOURCEMODEL_EXIT );
}

/*!
	Updates the source model with the given entry `id'.
	The entry could be either have undergone a status change or have just been
	modified.

	\param id Of type ulong, identifies the entry which was changed.
 */
void NotesModel::updateSourceModel(ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_UPDATESOURCEMODEL_ENTRY );
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		OstTraceFunctionExit0( NOTESMODEL_UPDATESOURCEMODEL_EXIT );
		return;
	}

	// Entry can be updated in many ways.
	// 1. Notes can just change their content or can be marked/unmarked as
	// favourites. So for any change with a note, we move the model index
	// representing the note to the top of the list.
	if (AgendaEntry::TypeNote == entry.type()) {
		QModelIndex foundIndex;
		for (int iter = 0; iter < mNotesCount; iter++) {
			foundIndex = mSourceModel->index(iter, 0);
			if (!foundIndex.isValid()) {
				continue;
			}

			ulong noteId = foundIndex.data(
					NotesNamespace::IdRole).value<qulonglong>();
			if (id == noteId) {
				break;
			}
		}
		QStandardItem *item = mSourceModel->takeItem(foundIndex.row());
		mSourceModel->removeRow(foundIndex.row());
		mNotesCount--;

		// Insert the note to the 0th postion.
		insertNoteToModel(foundIndex, id);

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		// 2. To-dos content can be modified.
		// So we check the status of the to-do. If the status has changed, we
		// check the previous status and remove the model index from the
		// corresponding section. Else we just update the to-do contents.
		QModelIndex foundIndex;
		for (int iter = mNotesCount;
				iter < mNotesCount + mInCompTodoCount + mCompTodoCount + 1;
				iter++) {
			foundIndex = mSourceModel->index(iter, 0);
			if (!foundIndex.isValid()) {
				continue;
			}

			ulong noteId = foundIndex.data(
					NotesNamespace::IdRole).value<qulonglong>();
			if (id == noteId) {
				break;
			}
		}

		// Get the status of the to-do.
		AgendaEntry::Status entryStatus = (AgendaEntry::Status) foundIndex.data(
				NotesNamespace::StatusRole).value<int>();

		if (entry.status() == entryStatus) {
			if (AgendaEntry::TodoCompleted != entryStatus) {
				// Make sure that the old model index is removed.
				mSourceModel->removeRow(foundIndex.row());
				mInCompTodoCount--;

				// Now insert the to-do.
				insertInCompTodoToModel(foundIndex, id);

			} else {
				// Make sure that the old model index is removed.
				mSourceModel->removeRow(foundIndex.row());
				mCompTodoCount--;

				// Now insert the completed to-do.
				insertCompTodoToModel(foundIndex, id);

			}
		} else {
			if (AgendaEntry::TodoCompleted != entryStatus) {
				// Here the to-do was marked completed. Hence we need to remove
				// the incompleted to-do and insert a completed to-do.
				mSourceModel->removeRow(foundIndex.row());
				mInCompTodoCount--;

				// Now insert the completed to-do.
				insertCompTodoToModel(foundIndex, id);

			} else {
				// Here the to-do was marked incompleted. Hence we need to
				// remove the completed to-do and insert an incompleted to-do.
				mSourceModel->removeRow(foundIndex.row());
				mCompTodoCount--;

				// Now insert the incompleted to-do.
				insertInCompTodoToModel(foundIndex, id);

			}
		}
	}
	OstTraceFunctionExit0( DUP1_NOTESMODEL_UPDATESOURCEMODEL_EXIT );
}

/*!
	Populates the source model. This slot is called when we get a db change
	notification from agenda server caused by another session.

	\param ids List of ids containing the entries that got changed.
 */
void NotesModel::populateSourceModel(QList<ulong> ids)
{
	OstTraceFunctionEntry0( DUP1_NOTESMODEL_POPULATESOURCEMODEL_ENTRY );
	Q_UNUSED(ids)

	QTimer::singleShot(1, this, SLOT(populateSourceModel()));
	OstTraceFunctionExit0( DUP1_NOTESMODEL_POPULATESOURCEMODEL_EXIT );
}

/*!
	Adds an entry to the model i.e., creates a QModelIndex and inserts it to the
	model.

	\param id The id of the entry to be added to the model.
 */
void NotesModel::addEntryToModel(ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_ADDENTRYTOMODEL_ENTRY );
	// We have different logic for adding a note or an incompleted to-do or a
	// completed to-do.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		OstTraceFunctionExit0( NOTESMODEL_ADDENTRYTOMODEL_EXIT );
		return;
	}
	bool notify = false;
	QModelIndex indexToNotify;

	if (AgendaEntry::TypeNote == entry.type()) {
		// All new notes are inserted at the top of the list by default.
		// Create a new model index at the 0th position.
		notify = insertNoteToModel(indexToNotify, id);

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoCompleted != entry.status()) {
			// Try to insert the to-do at its appropriate position.
			notify = insertInCompTodoToModel(indexToNotify, id);

		} else {
			// Try to insert the to-do at its appropriate position.
			insertCompTodoToModel(indexToNotify, id);
		}
	}

	if (notify) {
		emit rowAdded(indexToNotify);
	}
	OstTraceFunctionExit0( DUP1_NOTESMODEL_ADDENTRYTOMODEL_EXIT );
}

/*!
	Deletes an entry from model.

	\param Deletes the entry from model and hence updating the view.
 */
void NotesModel::removeEntryFromModel(ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_REMOVEENTRYFROMMODEL_ENTRY );
	for (int iter = 0; iter < mSourceModel->rowCount(); iter++) {
		QModelIndex mdlIndex = mSourceModel->index(iter, 0);

		if (!mdlIndex.isValid()) {
			OstTraceFunctionExit0( NOTESMODEL_REMOVEENTRYFROMMODEL_EXIT );
			return;
		}

		ulong noteId = mdlIndex.data(
				NotesNamespace::IdRole).value<qulonglong>();
		AgendaEntry::Type entryType = (AgendaEntry::Type) mdlIndex.data(
				NotesNamespace::TypeRole).value<int>();
		AgendaEntry::Status entryStatus = (AgendaEntry::Status) mdlIndex.data(
				NotesNamespace::StatusRole).value<int>();

		if (id == noteId) {
			mSourceModel->removeRow(mdlIndex.row());

			// Update the note, to-do counts depending on the type.
			if (entryType == AgendaEntry::TypeNote) {
				mNotesCount--;
			} else if (entryType == AgendaEntry::TypeTodo) {
				if (entryStatus == AgendaEntry::TodoCompleted) {
					mCompTodoCount--;
				} else {
					mInCompTodoCount--;
				}
			}
			break;
		}
	}
	OstTraceFunctionExit0( DUP1_NOTESMODEL_REMOVEENTRYFROMMODEL_EXIT );
}

/*!
	Populate the model after instance view creation.
 */
void NotesModel::handleInstanceViewCreationCompleted(int status)
{
	OstTraceFunctionEntry0( NOTESMODEL_HANDLEINSTANCEVIEWCREATIONCOMPLETED_ENTRY );
	Q_UNUSED(status);
	populateSourceModel();
	OstTraceFunctionExit0( NOTESMODEL_HANDLEINSTANCEVIEWCREATIONCOMPLETED_EXIT );
}

/*!
	Modifies the content of a given `row' with the data of the entry given by
	`id'.

	\param id The id of the entry.
	\param row The row corresponding to the given id.
 */
void NotesModel::modifyEntryInModel(ulong id, int row)
{
	OstTraceFunctionEntry0( NOTESMODEL_MODIFYENTRYINMODEL_ENTRY );
	// Get the model index.
	QModelIndex modelIndex = mSourceModel->index(row, 0);
	Q_ASSERT(modelIndex.isValid());

	// Fetch the entry
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	Q_ASSERT(!entry.isNull());

	// Set the note id.
	mSourceModel->setData(
			modelIndex, (qulonglong) id, NotesNamespace::IdRole);
	// Set the type of the note.
	mSourceModel->setData(
			modelIndex, entry.type(), NotesNamespace::TypeRole);

	// Set the display data to the index.
	if (AgendaEntry::TypeNote == entry.type()) {
		QString displayText;
		QString dateTimeText;
		QString modifiedText;

		// Read modification time from agenda entry
		QDateTime entryStartDateTime = entry.startTime();

		// If modified on today,show only modified time otherwise show the
		// modified date.
		if ((QDate::currentDate()) == entryStartDateTime.date() ) {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_modified_at_time");
			modifiedText =
					entryStartDateTime.time().toString(timeFormatString());
		} else {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_modified_on_date");
			modifiedText =
					entryStartDateTime.date().toString(dateFormatString());
		}

		displayText = dateTimeText.arg(modifiedText);

		QStringList stringList;
		stringList << entry.description().left(15) << displayText;
		// Set the data.
		mSourceModel->setData(
				modelIndex, stringList, Qt::DisplayRole);
		// Set the favourite icon.
		if (entry.favourite()) {
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_note"));
			iconList.append(HbIcon("qtg_mono_favourites"));
			mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
		} else {
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_note"));
			iconList.append(QVariant(QVariant::Invalid));
			mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
		}
	} else if (AgendaEntry::TypeTodo == entry.type()) {
		QStringList stringList;
		if (entry.summary().isEmpty()) {
			// If empty set unnamed text
			stringList<<hbTrId("txt_notes_dblist_unnamed");
		} else {
			stringList << entry.summary();
		}
		if (AgendaEntry::TodoCompleted != entry.status()) {
			// Read due date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
			QString dueDateText =
					entry.startTime().toString(dateFormatString());
			displayText = timeText.arg(dueDateText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(
				modelIndex, stringList, Qt::DisplayRole);

		// Set the to-do done/undone icon based on the status.
		QList<QVariant> iconList;
		if (AgendaEntry::TodoCompleted == entry.status()) {
			iconList.append(HbIcon("qtg_small_todo_done"));
		} else {
			iconList.append(HbIcon("qtg_small_todo"));
		}

		if (1 == entry.priority()) {
			// Set the High Priority icon if priority is high.
			iconList.append(HbIcon("qtg_small_priority_high"));
		} else if (!entry.alarm().isNull()) {
			// Set the alarm if set.
			iconList.append(HbIcon("qtg_mono_alarm"));
		} else {
			iconList.append(QVariant(QVariant::Invalid));
		}

		// Set the icons.
		mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
	}
	OstTraceFunctionExit0( NOTESMODEL_MODIFYENTRYINMODEL_EXIT );
}

/*!
	Appends notes to the model.

	\param ids QList of uids containing the notes.
 */
void NotesModel::appendNotesToModel(QList<AgendaEntry> &agendaEntryList)
{
	OstTraceFunctionEntry0( NOTESMODEL_APPENDNOTESTOMODEL_ENTRY );
	int entriesCount = agendaEntryList.count();
	// Iterate and add notes to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), entriesCount);
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - entriesCount;
			idIter < entriesCount; idIter++, modelIter++) {

		// Read the note details.
		AgendaEntry entry = agendaEntryList[rowCount - 1 - idIter];

		if (AgendaEntry::TypeNote != entry.type()) {
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(qAbs(modelIter), 0);

		// Set the note id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) entry.id(), NotesNamespace::IdRole);
		// Set the type of the note.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the favourite property.
		mSourceModel->setData(
				mdlIndex, entry.favourite(), NotesNamespace::FavouriteRole);

		// Set the display data now.
		// Read modification time from agenda entry
		QString displayText;
		QString dateTimeText;
		QString modifiedText;

		// Show the creation time if entry is not modified.
		if (entry.dtStamp().isValid()) {
			QDateTime creationDateTime = entry.dtStamp();

			// If created on today,show only creation time otherwise show the
			// creation date.
			if ((QDate::currentDate()) == creationDateTime.date()) {
				dateTimeText =
						hbTrId("txt_notes_dblist_note_created_at_time");
				modifiedText =
						creationDateTime.time().toString(timeFormatString());
			} else {
				dateTimeText =
						hbTrId("txt_notes_dblist_note_created_on_date");
				modifiedText =
						creationDateTime.date().toString(dateFormatString());
			}
		} else {
			QDateTime modifiedDateTime = entry.lastModifiedDateTime();

			// If modified on today,show only modified time otherwise show the
			// modified date.
			if ((QDate::currentDate()) == modifiedDateTime.date() ) {
				dateTimeText =
						hbTrId("txt_notes_dblist_note_modified_at_time");
				modifiedText =
						modifiedDateTime.time().toString(timeFormatString());
			} else {
				dateTimeText =
						hbTrId("txt_notes_dblist_note_modified_on_date");
				modifiedText =
						modifiedDateTime.date().toString(dateFormatString());
			}
		}

		displayText = dateTimeText.arg(modifiedText);

		QStringList stringList;
		stringList << entry.description().left(100) << displayText;
		// Set the data.
		mSourceModel->setData(
				mdlIndex, stringList, Qt::DisplayRole);

		if (entry.favourite()) {
			// Set the favourite icon.
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_note"));
			iconList.append(HbIcon("qtg_mono_favourites"));
			mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
		} else {
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_note"));
			iconList.append(QVariant(QVariant::Invalid));
			mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
		}

		// Update the notes count.
		mNotesCount++;
	}
	OstTraceFunctionExit0( NOTESMODEL_APPENDNOTESTOMODEL_EXIT );
}

/*!
	Appends incompleted to-dos to the model.

	\param ids QList of uids containing the incompleted to-dos.
 */
void NotesModel::appendInCompTodosToModel(QList<AgendaEntry> &agendaEntryList)
{
	OstTraceFunctionEntry0( NOTESMODEL_APPENDINCOMPTODOSTOMODEL_ENTRY );
	int entriesCount = agendaEntryList.count();
	// Iterate and add incomplete to-do to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), entriesCount);
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - entriesCount;
			idIter < entriesCount; idIter++, modelIter++) {

		// Read the to-do details.
		AgendaEntry entry = agendaEntryList[idIter];

		if (AgendaEntry::TypeTodo != entry.type()) {
			// delete the inserted row in the model
			mSourceModel->removeRow(modelIter);
			// decrement the iterator
			modelIter--;
			continue;
		}

		if (AgendaEntry::TodoCompleted == entry.status()) {
			// delete the inserted row in the model
			mSourceModel->removeRow(modelIter);
			// decrement the iterator
			modelIter--;
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(modelIter, 0);

		// Set the to-do id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) entry.id(), NotesNamespace::IdRole);
		// Set the type of the to-do.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the status of the to-do.
		mSourceModel->setData(
				mdlIndex, (int) entry.status(), NotesNamespace::StatusRole);

		// Set the display data now.
		QStringList stringList;
		if (entry.summary().isEmpty()) {
			// If empty set unnamed text
			stringList<<hbTrId("txt_notes_dblist_unnamed");
		} else {
			stringList << entry.summary();
		}

		if (AgendaEntry::TodoCompleted != entry.status()) {
			// Read due date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
			QString dueDateText =
					entry.startTime().toString(dateFormatString());
			displayText = timeText.arg(dueDateText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(mdlIndex, stringList, Qt::DisplayRole);
		// Set the to-do done/undone icon based on the status.
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_todo"));

		if (1 == entry.priority()) {
			// Set the High Priority icon if priority is high.
			iconList.append(HbIcon("qtg_small_priority_high"));
		} else if (!entry.alarm().isNull()) {
			// Set the alarm if set.
			iconList.append(HbIcon("qtg_mono_alarm"));
		} else {
			iconList.append(QVariant(QVariant::Invalid));
		}

		// Set the icons.
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);

		// Update the incompleted to-do count.
		mInCompTodoCount++;
	}
	OstTraceFunctionExit0( NOTESMODEL_APPENDINCOMPTODOSTOMODEL_EXIT );
}

/*!
	Appends incompleted to-dos to the model.

	\param ids QList of uids containing the completed to-dos.
 */
void NotesModel::appendCompTodosToModel(QList<AgendaEntry> &agendaEntryList)
{
	OstTraceFunctionEntry0( NOTESMODEL_APPENDCOMPTODOSTOMODEL_ENTRY );
	int entriesCount = agendaEntryList.count();
	// Iterate and add complete to-do to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), entriesCount);
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - entriesCount;
			idIter < entriesCount; idIter++, modelIter++) {

		// Read the completed to-do details.
		AgendaEntry entry = agendaEntryList[idIter];

		if (AgendaEntry::TypeTodo != entry.type()) {
			continue;
		}

		if (AgendaEntry::TodoCompleted != entry.status()) {
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(modelIter, 0);

		// Set the to-do id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) entry.id(), NotesNamespace::IdRole);
		// Set the type of the to-do.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the status of the to-do.
		mSourceModel->setData(
				mdlIndex, (int) entry.status(), NotesNamespace::StatusRole);

		// Set the display data now.
		QStringList stringList;
		if (entry.summary().isEmpty()) {
			// If empty set unnamed text
			stringList<<hbTrId("txt_notes_dblist_unnamed");
		} else {
			stringList << entry.summary();
		}
		if (AgendaEntry::TodoCompleted == entry.status()) {
			// Read completed date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_completed_on_1"));
			QString completedTimeText =
					entry.completedDateTime().toString(dateFormatString());
			displayText = timeText.arg(completedTimeText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(mdlIndex, stringList, Qt::DisplayRole);

		// Set the to-do done icon.
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_todo_done"));
		// To-do is already completed. No need to set alarm.
		iconList.append(QVariant(QVariant::Invalid));

		// Set the icons.
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);

		// Update the completed to-do count.
		mCompTodoCount++;
	}
	OstTraceFunctionExit0( NOTESMODEL_APPENDCOMPTODOSTOMODEL_EXIT );
}

/*!
	Inserts a note at the 0th position.

	\param index Will contain the model index of the newly created row.
	\param id The id of the note to be inserted.
	\return bool true if the insertion was successful, false otherwise.
 */
bool NotesModel::insertNoteToModel(QModelIndex &index, ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_INSERTNOTETOMODEL_ENTRY );
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		OstTraceFunctionExit0( NOTESMODEL_INSERTNOTETOMODEL_EXIT );
		return false;
	}

	mSourceModel->insertRows(0, 1);
	QModelIndex mdlIndex = mSourceModel->index(0, 0);

	// Now set the details of this note.
	// Set the note id.
	mSourceModel->setData(
			mdlIndex, (qulonglong) id, NotesNamespace::IdRole);
	// Set the type of the note.
	mSourceModel->setData(
			mdlIndex, entry.type(), NotesNamespace::TypeRole);
	// Set the favourite property.
	mSourceModel->setData(
			mdlIndex, entry.favourite(), NotesNamespace::FavouriteRole);

	// Set the display data now.
	// Read modification time from agenda entry
	QString displayText;
	QString dateTimeText;
	QString modifiedText;

	// Show the creation time if entry is not modified.
	if (entry.dtStamp().isValid()) {
		QDateTime creationDateTime = entry.dtStamp();

		// If created on today,show only creation time otherwise show the
		// creation date.
		if ((QDate::currentDate()) == creationDateTime.date()) {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_created_at_time");
			modifiedText =
					creationDateTime.time().toString(timeFormatString());
		} else {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_created_on_date");
			modifiedText =
					creationDateTime.date().toString(dateFormatString());
		}
	} else {
		QDateTime modifiedDateTime = entry.lastModifiedDateTime();

		// If modified on today,show only modified time otherwise show the
		// modified date.
		if ((QDate::currentDate()) == modifiedDateTime.date() ) {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_modified_at_time");
			modifiedText =
					modifiedDateTime.time().toString(timeFormatString());
		} else {
			dateTimeText =
					hbTrId("txt_notes_dblist_note_modified_on_date");
			modifiedText =
					modifiedDateTime.date().toString(dateFormatString());
		}
	}
	displayText = dateTimeText.arg(modifiedText);

	QStringList stringList;
	stringList << entry.description().left(100) << displayText;
	// Set the data.
	mSourceModel->setData(
			mdlIndex, stringList, Qt::DisplayRole);

	// Set the favourite icon.
	if (entry.favourite()) {
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_note"));
		iconList.append(HbIcon("qtg_mono_favourites"));
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
	} else {
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_note"));
		iconList.append(QVariant(QVariant::Invalid));
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
	}
	// Update the notes count.
	mNotesCount++;

	index = mdlIndex;

	OstTraceFunctionExit0( DUP1_NOTESMODEL_INSERTNOTETOMODEL_EXIT );
	return true;
}

/*!
	Inserts an incomplete to-do at a position based on the id of the
	to-do given.

	\param index This will hold the model index of the newly inserted to-do.
	\param id The id of the to-do to be inserted.
	\return bool true if the Insertion was successful, false otherwise.
 */
bool NotesModel::insertInCompTodoToModel(QModelIndex &index, ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_INSERTINCOMPTODOTOMODEL_ENTRY );

	bool success = false;

	// Fetch the entry first.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		OstTraceFunctionExit0( NOTESMODEL_INSERTINCOMPTODOTOMODEL_EXIT );
		return success;
	}

	// First fetch the list of incompleted to-dos.
	QList<ulong> entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeIncompletedTodos));

	// Get the index of the added to-do in the above list.
	int newTodoIndex = -1;
	for (int iter = 0; iter < entryIds.count(); iter++) {
		if (id == entryIds[iter]) {
			newTodoIndex = iter;
			break;
		}
	}

	// We display the to-dos in the same order that we get from the
	// agenda server. This is a new to-do that has been added. Hence we
	// have to insert a model index at the same position as we found the
	// id in the list of to-dos above.
	// Calculate the position.
	if (-1 != newTodoIndex) {
		mSourceModel->insertRows(mNotesCount + newTodoIndex, 1);
		QModelIndex newModelIndex =
				mSourceModel->index(mNotesCount + newTodoIndex, 0);

		// We continue only if the index is valid.
		if (newModelIndex.isValid()) {
			// Things look ok, start adding data to the model index.
			// Set the to-do id.
			mSourceModel->setData(
					newModelIndex, (qulonglong) id, NotesNamespace::IdRole);
			// Set the type of the to-do.
			mSourceModel->setData(
					newModelIndex, entry.type(), NotesNamespace::TypeRole);
			// Set the status of the to-do.
			mSourceModel->setData(
					newModelIndex, (int) entry.status(),
					NotesNamespace::StatusRole);

			// Set the display data now.
			QStringList stringList;
			if (entry.summary().isEmpty()) {
				// If empty set unnamed text
				stringList<<hbTrId("txt_notes_dblist_unnamed");
			} else {
				stringList << entry.summary();
			}
			if (AgendaEntry::TodoCompleted != entry.status()) {
				// Read due date from agenda entry
				QString displayText;
				QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
				QString dueDateText =
						entry.startTime().toString(dateFormatString());
				displayText = timeText.arg(dueDateText);
				stringList << displayText;
			}
			// Set the data.
			mSourceModel->setData(
					newModelIndex, stringList, Qt::DisplayRole);
			// Set the to-do done/undone icon based on the status.
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_todo"));

			if (1 == entry.priority()) {
				// Set the High Priority icon if priority is high.
				iconList.append(HbIcon("qtg_small_priority_high"));
			} else if (!entry.alarm().isNull()) {
				// Set the alarm if set.
				iconList.append(HbIcon("qtg_mono_alarm"));
			} else {
				iconList.append(QVariant(QVariant::Invalid));
			}

			// Set the icons.
			mSourceModel->setData(
					newModelIndex, iconList, Qt::DecorationRole);

			// Update the incompleted to-do count.
			mInCompTodoCount++;

			success = true;
			index = newModelIndex;
		}
	}

	OstTraceFunctionExit0( DUP1_NOTESMODEL_INSERTINCOMPTODOTOMODEL_EXIT );
	return success;
}

/*!
	Inserts a complete to-do at a position based on the id of the
	to-do given.

	\param index This will hold the model index of the newly inserted to-do.
	\param id The id of the to-do to be inserted.
	\return bool true if the Insertion was successful, false otherwise.
 */
bool NotesModel::insertCompTodoToModel(QModelIndex &index, ulong id)
{
	OstTraceFunctionEntry0( NOTESMODEL_INSERTCOMPTODOTOMODEL_ENTRY );
	bool success = false;

	// Fetch the entry first.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		OstTraceFunctionExit0( NOTESMODEL_INSERTCOMPTODOTOMODEL_EXIT );
		return success;
	}

	// Now fetch all the incomplete to-dos again and add them.
	QList<ulong> entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeCompletedTodos));

	// Get the index of the added to-do in the above list.
	int newTodoIndex = -1;
	for (int iter = 0; iter < entryIds.count(); iter++) {
		if (id == entryIds[iter]) {
			newTodoIndex = iter;
			break;
		}
	}

	// We display the to-dos in the same order that we get from the
	// agenda server. This is a new to-do that has been added. Hence we
	// have to insert a model index at the same position as we found the
	// id in the list of to-dos above.
	// Calculate the position.
	if (-1 != newTodoIndex) {
		mSourceModel->insertRows(
				mNotesCount + mInCompTodoCount + newTodoIndex, 1);
		QModelIndex newModelIndex = mSourceModel->index(
						mNotesCount + mInCompTodoCount + newTodoIndex, 0);

		// We continue only if the index is valid.
		if (newModelIndex.isValid()) {
			// Things look ok, start adding data to the model index.
			AgendaEntry entry = mAgendaUtil->fetchById(id);

			// Set the to-do id.
			mSourceModel->setData(
					newModelIndex, (qulonglong) id, NotesNamespace::IdRole);
			// Set the type of the to-do.
			mSourceModel->setData(
					newModelIndex, entry.type(), NotesNamespace::TypeRole);
			// Set the status of the to-do.
			mSourceModel->setData(
					newModelIndex, (int) entry.status(),
					NotesNamespace::StatusRole);

			// Set the display data now.
			QStringList stringList;
			if (entry.summary().isEmpty()) {
				// If empty set unnamed text
				stringList<<hbTrId("txt_notes_dblist_unnamed");
			} else {
				stringList << entry.summary();
			}
			if (AgendaEntry::TodoCompleted == entry.status()) {
				// Read completed date from agenda entry
				QString displayText;
				QString timeText(hbTrId("txt_notes_dblist_val_completed_on_1"));
				QString completedTimeText =
						entry.completedDateTime().toString(dateFormatString());
				displayText = timeText.arg(completedTimeText);
				stringList << displayText;
			}
			// Set the data.
			mSourceModel->setData(
					newModelIndex, stringList, Qt::DisplayRole);

			// Set the to-do done icon.
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_todo_done"));
			if (1 == entry.priority()) {
				// Set the High Priority icon if priority is high or else not.
				iconList.append(HbIcon("qtg_small_priority_high"));
			} else {
				// To-do is already completed. No need to set alarm.
				iconList.append(QVariant(QVariant::Invalid));
			}

			// Set the icons.
			mSourceModel->setData(
					newModelIndex, iconList, Qt::DecorationRole);

			// Update the completed to-do count.
			mCompTodoCount++;

			success = true;
			index = newModelIndex;
		}
	}

	OstTraceFunctionExit0( DUP1_NOTESMODEL_INSERTCOMPTODOTOMODEL_EXIT );
	return success;
}

/*!
	Retruns the dateformat based current locale settings.
	Common method can be used by any class.
	Can be removed once format strings are defined in hb.
 */
QString NotesModel::dateFormatString()
{
	OstTraceFunctionEntry0( NOTESMODEL_DATEFORMATSTRING_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();

	QString dateFormat;
	switch (locale.dateStyle()) {
		case HbExtendedLocale::American:
			dateFormat.append("MM");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("dd");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("yyyy");
			break;

		case HbExtendedLocale::European:
			dateFormat.append("dd");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("MM");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("yyyy");
			break;

		case HbExtendedLocale::Japanese:
			dateFormat.append("yyyy");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("MM");
			dateFormat.append(locale.dateSeparator(1));
			dateFormat.append("dd");
			break;
	}

	OstTraceFunctionExit0( NOTESMODEL_DATEFORMATSTRING_EXIT );
	return dateFormat;
}

/*!
	Retruns the timeformat string based on current locale settings
	Common method can be used by any class.
	Can be removed once format strings are defined in hb.
 */
QString NotesModel::timeFormatString()
{
	OstTraceFunctionEntry0( NOTESMODEL_TIMEFORMATSTRING_ENTRY );
	QString timeFormat;

	HbExtendedLocale locale = HbExtendedLocale::system();

	if (locale.timeStyle() == HbExtendedLocale::Time12) {
		timeFormat.append("h");
		timeFormat.append(locale.timeSeparator(1));
		timeFormat.append("mm");
		timeFormat.append(" ap");
	} else {
		timeFormat.append("hh");
		timeFormat.append(locale.timeSeparator(1));
		timeFormat.append("mm");
	}

	OstTraceFunctionExit0( NOTESMODEL_TIMEFORMATSTRING_EXIT );
	return timeFormat;
}


// End of file	--Don't remove this.
