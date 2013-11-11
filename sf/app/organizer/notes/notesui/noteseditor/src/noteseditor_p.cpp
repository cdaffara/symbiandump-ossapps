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
* Definition of NotesEditorPrivate class.
*
*/

// System includes
#include <QDateTime>
#include <HbInstance>
#include <HbMainWindow>
#include <HbView>
#include <HbNotificationDialog>
#include <HbExtendedLocale>
#include <HbTranslator>

// User includes
#include "noteseditor.h"
#include "noteseditor_p.h"
#include "notesnoteeditor.h"
#include "notestodoeditor.h"
#include <agendautil.h>
#include <agendaentry.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "noteseditor_pTraces.h"
#endif


/*!
	\class NotesEditorPrivate

	TODO: description here
 */

/*!
	Constructor.

	\param agendaUtil pointer to agenda utility
	\param parent QObject pointer.
 */
NotesEditorPrivate::NotesEditorPrivate(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent),
 mSaveEntry(true),
 mTranslator(0),
 mNoteId(0)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_NOTESEDITORPRIVATE_ENTRY );
	// First get the q-pointer.
	q_ptr = static_cast<NotesEditor *> (parent);
	
	mTranslator = new HbTranslator("noteseditor");
	mTranslator->loadCommon();

	// Here we check if the agendautil passed by the client is 0. If so, then we
	// construct our own.
	if (!agendaUtil) {
		mAgendaUtil = new AgendaUtil(this);
		mOwnsAgendaUtil = true;
	} else {
		mAgendaUtil = agendaUtil;
		mOwnsAgendaUtil = false;
	}

	// Here we register for changes in the DB.
	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong> )),
			this, SLOT(handleEntriesChanged(QList<ulong> )));

	// Register for the calenInstance view creation sucessfully
	connect(
			mAgendaUtil, SIGNAL(entryViewCreationCompleted(int)),
			this, SLOT(viewCreationCompleted(int)));
	
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_NOTESEDITORPRIVATE_EXIT );
}

/*!
	Destructor.
 */
NotesEditorPrivate::~NotesEditorPrivate()
{
	OstTraceFunctionEntry0( DUP1_NOTESEDITORPRIVATE_NOTESEDITORPRIVATE_ENTRY );
	if (mOwnsAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = 0;
	}

	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_NOTESEDITORPRIVATE_EXIT );
}


/*!
	Shows the noteseditor. The argument acts as the description for the note.

	\param string reference to file string/buffer
 */
void NotesEditorPrivate::edit(const QString &string)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_EDIT_ENTRY );
	// Create a agenda entry by setting the text as description for the note.
	AgendaEntry newNote;
	newNote.setType(AgendaEntry::TypeNote);
	newNote.setDescription(string);

	mNewEntry = true;

	// Construct note editor.
	mNoteEditor = new NotesNoteEditor(this);
	mOriginalNote = mModifiedNote = newNote;

	// launch note editor
	mNoteEditor->execute(newNote);
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Shows the noteseditor by parsing a .vcs which could be of a To-do or a plain
	.txt file which will be edited as a note.

	\param handle reference to QFile handle.
 */
void NotesEditorPrivate::edit(const QFile &handle)
{
	OstTraceFunctionEntry0( DUP1_NOTESEDITORPRIVATE_EDIT_ENTRY );
	Q_UNUSED(handle)
	OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Shows the noteseditor, by parsing an AgendaEntry.

	\param entry An object of AgendaEntry.
 */
void NotesEditorPrivate::edit(AgendaEntry entry)
{
	OstTraceFunctionEntry0( DUP2_NOTESEDITORPRIVATE_EDIT_ENTRY );
	// Check if its a new note or a note being edited.
	if (0 < entry.id()) {
		mNewEntry = false;
		mNoteId = entry.id();
	} else {
		mNewEntry = true;
		mNoteId = 0;
	}

	if (entry.type()== AgendaEntry::TypeNote) {
		mOriginalNote = mModifiedNote = entry;

		// Construct note editor.
		mNoteEditor = new NotesNoteEditor(this);

		// launch note editor
		mNoteEditor->execute(entry);
	} else if (entry.type() == AgendaEntry::TypeTodo) {
		mOriginalNote = mModifiedNote = entry;

		// Construct to-do editor.
		mTodoEditor = new NotesTodoEditor(this);

		// launch to-do editor
		mTodoEditor->execute(entry);
	} else {
		// Invalid entry type.
		OstTraceFunctionExit0( DUP2_NOTESEDITORPRIVATE_EDIT_EXIT );
		return;
	}
	OstTraceFunctionExit0( DUP3_NOTESEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Shows the noteseditor, by fetching the note using the entry id provided.

	\param id entry id of the note.
 */
void NotesEditorPrivate::edit(ulong id)
{
	OstTraceFunctionEntry0( DUP3_NOTESEDITORPRIVATE_EDIT_ENTRY );
	// Fetch the entry using the id provided
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		// save the entries , to show the entry once instances are created
		// if entry is null exit ,later  call back comes in viewCreationCompleted 
		mNoteId =id;
	} else {
		edit(entry);
	}
	OstTraceFunctionExit0( DUP5_NOTESEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Creates a new note or to-do based on the create type.It lanuches the
	respective editors for editing

	\type type of the editor to be shown for creating new note and new to-do
 */
void NotesEditorPrivate::create(NotesEditor::CreateType type)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_CREATE_ENTRY );

	switch(type) {
		case NotesEditor::CreateNote: {
			AgendaEntry entry;

			// Construct note editor.
			mNoteEditor = new NotesNoteEditor(this);

			// Set the type as Note.
			entry.setType(AgendaEntry::TypeNote);
			mOriginalNote = mModifiedNote = entry;
			mNewEntry = true;

			// Launch the notes editor
			mNoteEditor->execute(mModifiedNote);
		}
		break;

		case NotesEditor::CreateTodo: {
			AgendaEntry entry;
			// Construct to-do editor.
			mTodoEditor = new NotesTodoEditor(this);

			// Set the attributes for the to-do entry
			entry.setType(AgendaEntry::TypeTodo);
			entry.setStatus(AgendaEntry::TodoNeedsAction);
			entry.setPriority(2);

			QDateTime dueDateTime =
					QDateTime(
							QDate::currentDate(),
							QTime::fromString("12:00 am", "hh:mm ap"));
			entry.setStartAndEndTime(dueDateTime, dueDateTime);

			mOriginalNote = mModifiedNote = entry;
			mNewEntry = true;

			// Launch the to-do editor.
			mTodoEditor->execute(entry);
		}
		break;

		case NotesEditor::CreateTypeUnKnown:
		break;

		default:
		break;
	}

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_CREATE_EXIT );
}


/*!
	Closes the notes editor based on the close type

	\param type type of the close requested by the client
 */
ulong NotesEditorPrivate::close(NotesEditor::CloseType type)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_CLOSE_ENTRY );

	switch (type) {
		case NotesEditor::CloseWithSave: {
			if (AgendaEntry::TypeNote == mModifiedNote.type()) {
				// Save the note created/edited in notes editor.
				saveNote();

				// Close the notes editor
				mNoteEditor->close();
				mNoteEditor->deleteLater();
			} else if (AgendaEntry::TypeTodo == mModifiedNote.type()) {
				// Save the to-do entry created/edited in to-do editor.
				saveTodo();

				// Close the to-do editor
				mTodoEditor->close();
				mTodoEditor->deleteLater();
			}
		}
		break;
		case NotesEditor::CloseWithoutSave: {
			if (AgendaEntry::TypeNote == mModifiedNote.type()) {
				// Close the notes editor
				mNoteEditor->close();
				mNoteEditor->deleteLater();
			} else if (AgendaEntry::TypeTodo == mModifiedNote.type()) {
				// Close the to-do editor
				mTodoEditor->close();
				mTodoEditor->deleteLater();
			}
			mNoteId = 0;
		}
		break;
		case NotesEditor::CloseTypeUnKnown:
		default:
			mNoteId = 0;
		break;
	}
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_CLOSE_EXIT );
	return mNoteId;
}

/*!
	Retruns the dateformat based current locale settings.
	Common method can be used by any class.
	Can be removed once format strings are defined in hb.
 */
QString NotesEditorPrivate::dateFormatString()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_DATEFORMATSTRING_ENTRY );
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

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_DATEFORMATSTRING_EXIT );
	return dateFormat;
}

/*!
	Retruns the timeformat string based on current locale settings
	Common method can be used by any class.
	Can be removed once format strings are defined in hb.
 */
QString NotesEditorPrivate::timeFormatString()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_TIMEFORMATSTRING_ENTRY );
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

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_TIMEFORMATSTRING_EXIT );
	return timeFormat;
}

/*!
	Slot which gets called when entries have changed in the db.
 */
void NotesEditorPrivate::handleEntriesChanged(QList<ulong> ids)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_HANDLEENTRIESCHANGED_ENTRY );
	if (!mNewEntry) {
		if (ids.contains(mModifiedNote.id())) {
			mSaveEntry = false;
		}
	}
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_HANDLEENTRIESCHANGED_EXIT );
}

/*!
	Handle the case where a note is marked as a to-do.
 */
void NotesEditorPrivate::markNoteAsTodo()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_MARKNOTEASTODO_ENTRY );
	// This function should not be called by any other function except the
	// noteeditor.
	if (mModifiedNote.type() != AgendaEntry::TypeNote) {
		OstTraceFunctionExit0( NOTESEDITORPRIVATE_MARKNOTEASTODO_EXIT );
		return;
	}

	// Here change the type of modified note and destroy the noteeditor and
	// construct the to-do editor.
	mModifiedNote.setType(AgendaEntry::TypeTodo);

	QDateTime dueDateTime;
	dueDateTime.setDate(QDate::currentDate());
	dueDateTime.setTime(QTime::fromString("12:00 am", "hh:mm ap"));

	mModifiedNote.setStartAndEndTime(dueDateTime, dueDateTime);

	mModifiedNote.setSummary(mNoteEditor->getDescription().left(80));

	if (80 > mNoteEditor->getDescription().length()) {
		mModifiedNote.setDescription("");
	}

	mModifiedNote.setStatus(AgendaEntry::TodoNeedsAction);

	// Set the priority of the to-do as normal
	mModifiedNote.setPriority(2);

	// Remove favourite if marked so.
	mModifiedNote.setFavourite(0);

	// Remove the softkey action of notes editor before opening to-do editor.
	HbMainWindow *window = hbInstance->allMainWindows().first();

	// Launch the to-do editor.
	mTodoEditor = new NotesTodoEditor(this);
	mTodoEditor->execute(mModifiedNote);

	// Now close the noteeditor.
	window->removeView(mNoteEditor->mEditor);
	mNoteEditor->deleteLater();

	OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_MARKNOTEASTODO_EXIT );
}

/*!
	A slot used by the notesnoteeditor to update the description when the
	content in the text editor changes.
 */
void NotesEditorPrivate::updateNoteText()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_UPDATENOTETEXT_ENTRY );

	mModifiedNote.setDescription(mNoteEditor->getDescription());

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_UPDATENOTETEXT_EXIT );
}

/*!
	This deletes the note/to-do being edited/created and closes the editor.
 */
void NotesEditorPrivate::deleteNote()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_DELETENOTE_ENTRY );

	if (!mNewEntry) {
		// Delete the note. All the changes are discarded.
		mAgendaUtil->deleteEntry(mOriginalNote.id());
	}

	// Destroy the active editor and close.
	if (mModifiedNote.type() == AgendaEntry::TypeNote) {
		mNoteEditor->close();
		mNoteEditor->deleteLater();
	} else {
		mTodoEditor->close();
		mTodoEditor->deleteLater();
	}

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_DELETENOTE_EXIT );
}

/*!
	Called when Done is pressed in either note/to-do editor.
 */
bool NotesEditorPrivate::saveNote()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_SAVENOTE_ENTRY );

	bool status = false;
	QString description = mNoteEditor->getDescription();

	// Check the size after simplified() which removes all the space inside the
	// description. If it is 0, don't create/modify the note.
	if (0 < description.simplified().size()) {
		// Save the note here.
		if (mNewEntry) {
			if (AgendaEntry::TypeNote == mModifiedNote.type()) {
				mModifiedNote.setDescription(description);
				mModifiedNote.setLastModifiedDateTime(
						QDateTime(QDate::currentDate(), QTime::currentTime()));
				// Set the creation time as DTStamp time
				mModifiedNote.setDTStamp(
						QDateTime(QDate::currentDate(), QTime::currentTime()));
			} else if (AgendaEntry::TypeTodo == mModifiedNote.type()) {
				mModifiedNote.setSummary(description);
				mModifiedNote.setDescription(description);

				// Set the status of the to-do.
				mModifiedNote.setStatus(AgendaEntry::TodoNeedsAction);
			}
			// Now save the entry.
			mNoteId = mAgendaUtil->store(mModifiedNote);
			if (mNoteId) {
				showNotification(
						hbTrId("txt_notes_dpopinfo_new_note_saved"));
			}
		} else {
			if (mOriginalNote.type() != mModifiedNote.type()) {
				// Entry has been morphed, delete the original one and create
				// the new one with the modified entry data.
				mAgendaUtil->deleteEntry(mOriginalNote.id());

				if (AgendaEntry::TypeNote == mModifiedNote.type()) {
					mModifiedNote.setDescription(description);
				} else if (AgendaEntry::TypeTodo == mModifiedNote.type()) {
					mModifiedNote.setSummary(description);
					mModifiedNote.setDescription(description);

					// Now save the entry.
					mNoteId = mAgendaUtil->store(mModifiedNote);
				}
			} else {
				mModifiedNote.setDescription(description);
				mModifiedNote.setLastModifiedDateTime(
						QDateTime(QDate::currentDate(), QTime::currentTime()));

				if (isNoteEdited()) {
					ulong updateStatus = mAgendaUtil->store(mModifiedNote);
					if (updateStatus) {
						showNotification(
								hbTrId("txt_notes_dpopinfo_note_saved"));
					}
				}
				mNoteId = mModifiedNote.id();
			}
		}
		status = true;
	} else {
		mNoteId = 0;
	}

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_SAVENOTE_EXIT );
	return status;
}

/*!
	 Saves a to-do entry
 */
bool NotesEditorPrivate::saveTodo()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_SAVETODO_ENTRY );

	if (AgendaEntry::TypeTodo != mModifiedNote.type()) {
		// This should never happen.
		OstTraceFunctionExit0( NOTESEDITORPRIVATE_SAVETODO_EXIT );
		return false;
	}
	bool status = false;

	if (mNewEntry) {
		if (isTodoEdited()) {
			// Add the new to-do.
			mNoteId = mAgendaUtil->store(mModifiedNote);
			if (mNoteId) {
				status = true;
				showNotification(
						hbTrId("txt_notes_dpopinfo_new_todo_note_saved"));
			}
		}
	} else {
		// Here there are two cases.
		// 1) A note was being edited and the type was changed. In which case,
		// we delete the note and create a new entry of type to-do.
		// 2) A to-do was edited.
		if (mOriginalNote.type() != mModifiedNote.type()) {
			// Set the status of the to-do.
			mModifiedNote.setStatus(AgendaEntry::TodoNeedsAction);

			// First clone the mModifiedNote for the new type.
			ulong id = mAgendaUtil->cloneEntry(
					mModifiedNote, AgendaEntry::TypeTodo);

			// Delete the old entry.
			bool result = mAgendaUtil->deleteEntry(mModifiedNote.id());

			if (id && result) {
				status = true;
			}
		} else {
			if (isTodoEdited()) {
				mNoteId = mAgendaUtil->store(mModifiedNote);
				if (mNoteId) {
					status = true;
				}
			}
		}
		if (status) {
			showNotification(
					hbTrId("txt_notes_dpopinfo_todo_note_saved"));
		}
	}

	OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_SAVETODO_EXIT );
	return status;
}

/*!
	Emits the signal NotesEditor::editingCompleted() .

	\sa NotesEditor::editingCompleted()
 */
void NotesEditorPrivate::editingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_EDITINGCOMPLETED_ENTRY );

	if(mNoteEditor) {
		mNoteEditor->deleteLater();
	}
	if(mTodoEditor) {
		mTodoEditor->deleteLater();
	}
	emit q_ptr->editingCompleted(status);

	OstTraceFunctionExit0( NOTESEDITORPRIVATE_EDITINGCOMPLETED_EXIT );
}

/*!
	Called when instances of calenInstanceview and entryInstanceview is created
	successfully.
	
	\param int status to indicate whether the creation was successful or not.
 */
void NotesEditorPrivate::viewCreationCompleted(int error)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_VIEWCREATIONCOMPLETED_ENTRY );
	if ((KErrNone == error)) {
		AgendaEntry entry;
		entry = mAgendaUtil->fetchById(mNoteId);
		// if entry is there , then show the view.
		if (!entry.isNull()) {
			// Edit the agenda entry based on the entry type.
			edit(entry);
		} else {
			// emit this signal so that the client can free the memory.
			emit q_ptr->editingCompleted(true);
		}
	}
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_VIEWCREATIONCOMPLETED_EXIT );
}

/*!
	Checks whether the note entry is edited by the user

	\return	bool returns true is user edited otherwise returns false.
 */
bool NotesEditorPrivate::isNoteEdited()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_ISNOTEEDITED_ENTRY );

	if (mModifiedNote.description().compare(mOriginalNote.description())) {
		OstTraceFunctionExit0( NOTESEDITORPRIVATE_ISNOTEEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.favourite() != mOriginalNote.favourite()) {
		OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_ISNOTEEDITED_EXIT );
		return ETrue;
	}

	OstTraceFunctionExit0( DUP2_NOTESEDITORPRIVATE_ISNOTEEDITED_EXIT );
	return EFalse;
}

/*!
	Checks whether the to-do entry is edited by the user

	\return	bool returns true is user edited otherwise returns false.
 */
bool NotesEditorPrivate::isTodoEdited()
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_ISTODOEDITED_ENTRY );

	if ( mModifiedNote.summary().compare(mOriginalNote.summary())) {
		OstTraceFunctionExit0( NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.status() != mOriginalNote.status()) {
		OstTraceFunctionExit0( DUP1_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.priority() != mOriginalNote.priority()) {
		OstTraceFunctionExit0( DUP2_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.endTime() != mOriginalNote.endTime()) {
		OstTraceFunctionExit0( DUP3_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.description().compare(mOriginalNote.description())) {
		OstTraceFunctionExit0( DUP4_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}
	if (mModifiedNote.alarm() != mOriginalNote.alarm()) {
		OstTraceFunctionExit0( DUP5_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
		return ETrue;
	}

	OstTraceFunctionExit0( DUP6_NOTESEDITORPRIVATE_ISTODOEDITED_EXIT );
	return EFalse;
}

/*!
	Shows the save/update notification.

	\param text Text to be shown as notification.
 */
void NotesEditorPrivate::showNotification(QString text)
{
	OstTraceFunctionEntry0( NOTESEDITORPRIVATE_SHOWNOTIFICATION_ENTRY );
	HbNotificationDialog *notificationDialog = new HbNotificationDialog();
	notificationDialog->setTimeout(
			HbNotificationDialog::ConfirmationNoteTimeout);
	notificationDialog->setTitle(text);
	notificationDialog->show();
	OstTraceFunctionExit0( NOTESEDITORPRIVATE_SHOWNOTIFICATION_EXIT );
}
// End of file	--Don't remove this.
